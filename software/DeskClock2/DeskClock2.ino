/***************************************************
  This is our GFX example for the Adafruit ILI9341 Breakout and Shield
  ----> http://www.adafruit.com/products/1651

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#include <DallasTemperature.h>
#include <OneWire.h>

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

#include <Adafruit_NeoKey_1x4.h>
#include <seesaw_neopixel.h>



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

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
// If using the breakout, change pins as desired
//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

const GFXfont *font = &FONT;

Adafruit_NeoKey_1x4 neokey1;
Adafruit_NeoKey_1x4 neokey2;


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

/*
    int val;

    if (!serial_avail)
        return;
    serial_rb[serial_r] = '\0';

    if (serial_rb[0] == 'L') {
        if (serial_rb[1] == '=') {
            tft.setCursor(0, 1.5 * TEXT_LINE_HEIGHT);
            tft.fillRect(0, tft.getCursorY() - TEXT_LINE_HEIGHT, SCREEN_WIDTH, TEXT_LINE_HEIGHT + 7, BLACK);

            char *start = &serial_rb[2];
            for (int i = 2; serial_rb[i] != '\0'; i++) {
                if (serial_rb[i] == '\r' || serial_rb[i] == '\n') {
                    serial_rb[i] = '\0';
                    tft.println(start);
                    start = &serial_rb[i + 1];
                    tft.fillRect(0, tft.getCursorY() - TEXT_LINE_HEIGHT, SCREEN_WIDTH, TEXT_LINE_HEIGHT + 7, BLACK);
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
*/

/*
    // NOTE this is supposed to be a character by character printer
    int val;
    char next[2] = " ";

    if (!serial_avail)
        return;
    serial_rb[serial_r] = '\0';

    if (serial_rb[0] == 'L') {
        if (serial_rb[1] == '=') {
            byte colour = RED;
            byte font_size = 1;

            int x = 0;
            int y = 2 * TEXT_LINE_HEIGHT;
            int16_t bx, by;
            uint16_t bw, bh;

            for (int i = 2; serial_rb[i] != '\0'; i++) {
                if (serial_rb[i] == '\r' || serial_rb[i] == '\n') {
                    tft.fillRect(x, y, SCREEN_WIDTH - x, TEXT_LINE_HEIGHT + 6, BLACK);
                    x = 0;
                    y += TEXT_LINE_HEIGHT + 4;
                } else if (serial_rb[i] == '\x1b' && serial_rb[i + 1] == '[') {
                    byte type;
                    i += 2;

                    val = serial_rb[i] - '0';
                    type = serial_rb[++i];
                    colour = colours[val];
                } else if (serial_rb[i] >= 0x20) {
                    tft.fillRect(x, y, 20, TEXT_LINE_HEIGHT + 6, BLACK);
                    tft.drawChar(x, y, serial_rb[i], colour, BLACK, font_size);
                    x += 20;

                    //next[0] = serial_rb[i];
                    //tft.getTextBounds(next, 0, 0, &bx, &by, &bw, &bh);
                    //tft.fillRect(x, y, bx + bw, TEXT_LINE_HEIGHT + 6, BLACK);
                    //x += bx + bw;

                    if (x >= SCREEN_WIDTH && serial_rb[i + 1] != '\r') {
                        x = 0;
                        y += TEXT_LINE_HEIGHT + 4;
                    }
                }
            }
        }
    }
    clear_serial();
*/


/*
    int val;

    if (!serial_avail)
        return;
    serial_rb[serial_r] = '\0';

    if (serial_rb[0] == 'L') {
        if (serial_rb[1] == '=') {
            word colour = RED;
            byte font_size = 1;
            byte text_width = 20;
            byte text_height = 20;
            byte screen_width = SCREEN_WIDTH / text_width;
            byte screen_height = SCREEN_HEIGHT / text_height;

            int x = 0, y = 1;
            for (int i = 2; serial_rb[i] != '\0'; i++) {
                if (serial_rb[i] == '\r' || serial_rb[i] == '\n') {
                    tft.fillRect(x * text_width, y * text_height, screen_width - (x * text_width), text_height, BLACK);
                    x = 0;
                    y += 1;
                } else if (serial_rb[i] == '\x1b' && serial_rb[i + 1] == '[') {
                    byte type;
                    i += 2;

                    val = serial_rb[i] - '0';
                    type = serial_rb[++i];
                    colour = colours[val];
                } else if (serial_rb[i] >= 0x20) {
                    tft.fillRect(x * text_width, y * text_height, text_width, text_height, BLACK);
                    tft.drawChar(x * text_width, y * text_height, serial_rb[i], colour, BLACK, font_size, font_size);
                    x += 1;
                    if (x >= screen_width && serial_rb[i + 1] != '\r') {
                        x = 0;
                        y += 1;
                    }
                }
            }
        }
    }
    clear_serial();
*/
}

/************************
 * Button Debounce Code *
 ************************/

#define NUM_BUTTONS    8
unsigned long button_last = 0;
unsigned int button_st[NUM_BUTTONS];
char button_ch[NUM_BUTTONS];
char buttons[NUM_BUTTONS];

void init_buttons()
{
    for (int i = 0; i < NUM_BUTTONS; i++) {
        buttons[i] = 0;
        button_ch[i] = 0;
        button_st[i] = 0;
    }
}

void debounce_buttons()
{
    uint8_t button_values;

    button_values = neokey1.read();

    // Rotate the bits so the button order works with the current layout
    byte output = 0;
    byte input = neokey2.read();
    for (byte i = 0; i < 4; i++) {
        output |= input & 0x01;
        input >>= 1;
        output <<= 1;
    }
    button_values |= output << 3;


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

/**********************
 * Temperature Sensor *
 **********************/

OneWire oneWire(5);
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

    neokey1.begin(0x30);
    neokey2.begin(0x31);

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

    //if (read_ir()) {
    //    Serial.print("I0=");
    //    if (ir_repeat)
    //        Serial.print("-1");
    //    else {
    //        //Serial.print((word) ir_code, HEX);
    //        Serial.print(ir_types[(ir_data.decode_type < 0) ? 0 : ir_data.decode_type]);
    //        Serial.print(':');
    //        Serial.print(ir_data.value, HEX);
    //    }
    //    Serial.print('\n');
    //}

    if (millis() - last_time > 30000) {
        last_time = millis();
        send_temperature();
    }
}

