/**
 * DeskClock2 - A Desktop Companion Device
 */

#include <DallasTemperature.h>
#include <OneWire.h>

#include <IRremote.h>

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

#include <Wire.h>


#define FONT    OverpassMono14pt7b
#include "OverpassMono14pt7b.h"
#define TEXT_LINE_HEIGHT    20
#define TEXT_MAX_HEIGHT     24

//#define FONT    Inconsolata18pt7b
//#include "Fonts/Inconsolata18pt7b.h"


#define SCREEN_WIDTH    320
#define SCREEN_HEIGHT   240

// For the Adafruit shield, these are the default.
#define TFT_DC      9
#define TFT_CS      10
//#define TFT_RST     5
//#define TFT_CLK     6
//#define TFT_MOSI    11
//#define TFT_MISO    12

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

const GFXfont *font = &FONT;


#define RGB(r, g, b)    ((r) << 11) | ((g) << 5) | (b)

//define some colour values
#define BLACK           0x0000
#define BLUE            0x001F
#define RED             0xF800
#define GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
#define WHITE           0xFFFF
#define DARKGREEN       RGB(0, 48, 0)
#define DARKYELLOW      RGB(31, 56, 0)


word colours[] = {
    BLACK,
    BLUE,
    RED,
    GREEN,
    CYAN,
    MAGENTA,
    YELLOW,
    WHITE,
    DARKGREEN,
    DARKYELLOW,
};


/******************************
 * Serial Communications Code *
 ******************************/

#define SERIAL_SIZE     512

char serial_r = 0;
char serial_avail = 0;
char serial_rb[SERIAL_SIZE];

void read_serial()
{
    int b;

    if (serial_avail == 1)
        return;

    b = Serial.read();
    if (b == -1)
        return;

    serial_rb[serial_r] = b;
    if (b == '\n')
        serial_avail = 1;
    if (serial_r < SERIAL_SIZE)
        serial_r++;
}

void clear_serial()
{
    serial_avail = 0;
    serial_r = 0;
}


void process_msg()
{
    int val;

    if (!serial_avail)
        return;
    serial_rb[serial_r] = '\0';

    if (serial_rb[0] == 'L') {
        if (serial_rb[2] == '=') {
            byte lineno = serial_rb[1] - '0';
            tft.setCursor(0, (1 + lineno) * font->yAdvance);
            tft.fillRect(0, tft.getCursorY() - TEXT_LINE_HEIGHT, SCREEN_WIDTH, TEXT_LINE_HEIGHT + 7, BLACK);

            char *start = &serial_rb[3];
            for (int i = 3; serial_rb[i] != '\0'; i++) {
                if (serial_rb[i] == '\r' || serial_rb[i] == '\n') {
                    serial_rb[i] = '\0';
                    tft.println(start);
                    start = &serial_rb[i + 1];
                } else if (serial_rb[i] == '\x1b' && serial_rb[i + 1] == '[') {
                    serial_rb[i] = '\0';
                    tft.print(start);
                    start = &serial_rb[i + 4];

                    byte type;
                    i += 2;

                    val = serial_rb[i] - '0';
                    type = serial_rb[++i];
                    tft.setTextColor(colours[val]);
                }
            }
        }
    }
    clear_serial();
}


/************************
 * Button Debounce Code *
 ************************/

#define I2C_GPIO_ADDR    0x26

#define NUM_BUTTONS    8
unsigned long button_last = 0;
unsigned int button_st[NUM_BUTTONS];
char button_ch[NUM_BUTTONS];
char buttons[NUM_BUTTONS];

void init_buttons()
{
    Wire.begin();

    Wire.beginTransmission(I2C_GPIO_ADDR);

    // Set GPPU (Pull-Ups) to On
    Wire.write(0x06);
    Wire.write(0xff);

    Wire.endTransmission();

    for (int i = 0; i < NUM_BUTTONS; i++) {
        buttons[i] = 0;
        button_ch[i] = 0;
        button_st[i] = 0;
    }
}

void debounce_buttons()
{
    uint8_t button_values;

    button_values = read_buttons();

    if ((millis() - button_last) < 3)
        return;

    button_last = millis();
    for (int i = 0; i < NUM_BUTTONS; i++) {
        button_st[i] <<= 1;
        button_st[i] |= (button_values >> (NUM_BUTTONS - i - 1)) & 0x01;
        if (buttons[i] == 0) {
            /// If at least one bit is not zero, then call it a 1
            if (button_st[i] != 0) {
                buttons[i] = 1;
                button_ch[i] = 1;
            }
        }
        else {
            /// If it's all zeros then call it a 0
            if (button_st[i] == 0) {
                buttons[i] = 0;
                button_ch[i] = 1;
            }
        }
    }
}

void process_buttons()
{
    for (int i = 0; i < NUM_BUTTONS; i++) {
        if (button_ch[i]) {
            Serial.print('B');
            Serial.print(i);
            Serial.print('=');
            Serial.print(buttons[i], HEX);
            Serial.print('\n');
            button_ch[i] = 0;
        }
    }
}

byte read_buttons()
{
    char buttons;

    // Request read GPIO (Input values)
    Wire.beginTransmission(I2C_GPIO_ADDR);
    Wire.write(0x09);
    Wire.endTransmission();

    // Wait to read data
    Wire.requestFrom(I2C_GPIO_ADDR, 1);

    while (Wire.available()) {
        buttons = Wire.read();
    }

    return ~buttons;
}


/**********************
 * Temperature Sensor *
 **********************/

#define PIN_TEMP    5

OneWire oneWire(PIN_TEMP);
DallasTemperature sensors(&oneWire);

void init_temperature()
{
	sensors.begin();
}

void send_temperature()
{
	sensors.requestTemperatures();
	Serial.print("T0=");
	Serial.print(sensors.getTempCByIndex(0), 4);
	Serial.print('\n');
}


/********************
 * IR Recevier Code *
 ********************/

#define PIN_IRRX      6

IRData ir_last;

void init_ir(void)
{
    IrReceiver.begin(PIN_IRRX, DISABLE_LED_FEEDBACK);
}

int read_ir(void)
{
    if (!IrReceiver.decode())
        return 0;

    ir_last = IrReceiver.decodedIRData;
    IrReceiver.resume();

    if (!(IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT))
        return 1;
    else
        return 0;
}

void process_ir(void)
{
    if (read_ir()) {
        Serial.print("I0=");
        Serial.print(get_protocol(ir_last.protocol));
        Serial.print(':');
        Serial.print(ir_last.address, HEX);
        Serial.print(':');
        Serial.print(ir_last.command, HEX);
        Serial.print('\n');
    }
}

char *get_protocol(decode_type_t protocol)
{
    switch (protocol) {
        case NEC:
            return "N";
        case PANASONIC:
            return "P";
        case SONY:
            return "S";
        case ONKYO:
            return "O";
        case JVC:
            return "J";
        case SHARP:
            return "SH";
        case DENON:
            return "D";
        case LG:
        case LG2:
            return "L";
        case RC5:
            return "RC5";
        case RC6:
            return "RC6";
        case SAMSUNG:
            return "SA";
        case APPLE:
            return "A";
        default:
            return "x";
    }
}


/*********************
 * Setup & Main Loop *
 *********************/

void setup()
{
    Serial.begin(115200);
    clear_serial();

    tft.begin();
    tft.setRotation(1);
    tft.setFont(font);
    tft.setTextSize(1);
    tft.fillScreen(ILI9341_BLACK);

    init_buttons();

    init_ir();
    init_temperature();
}

unsigned long last_time = 0;

void loop()
{
    char ret = 0;
    
    debounce_buttons();
    read_serial();

    process_buttons();
    process_msg();

    process_ir();

    if (millis() - last_time > 30000) {
        last_time = millis();
        send_temperature();
    }
}

