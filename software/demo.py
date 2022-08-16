 

import serial


serial = serial.Serial("/dev/ttyACM2", 115200)

text_limit = 19

date = "14:25 Sun Aug 14"
artist = "Nine Inch Nails"
title = "jog{}"
temp = "27.5".rjust(text_limit)
outdoor_temp = "19.1".rjust(text_limit)

msg = 'L=\x1b[2m{}\r\x1b[3m{}\r\x1b[5m{}\r\x1b[9m{}\r\x1b[1m{}'.format(date, artist, title, temp, outdoor_temp)

serial.write(bytes(msg + '\n', 'utf-8'))
