Packet format (14 bytes):


[0] = 0xFF (header byte 1)

[1] = 0x00 (header byte 2)

[2] = Value from 0x00 to 0xFF (PWM channel 1)

[3] = Value from 0x00 to 0xFF (PWM channel 2)

[4] = Value from 0x00 to 0xFF (PWM channel 3)

[5] = Value from 0x00 to 0xFF (PWM channel 4)

[6] = Value from 0x00 to 0xFF (PWM channel 5)

[7] = Value from 0x00 to 0xFF (PWM channel 6)

[8] = Value from 0x00 to 0xFF (PWM channel 7)

[9] = Value from 0x00 to 0xFF (PWM channel 8)

[10] = Value from 0x00 to 0xFF (PWM channel 9)

[11] = Value from 0x00 to 0xFF (PWM channel 10)

[12] = Value from 0x00 to 0xFF (CRC-16 High Byte)

[13] = Value from 0x00 to 0xFF (CRC-16 Low Byte)


CRC-16 is generated from first 12 bytes. Baud rate: 115200. PWMs are disconnected after 250ms without a valid packet. PWMs are generated on Arduino pins 2-11.


http://arduino.cc/it/Hacking/PinMapping168
