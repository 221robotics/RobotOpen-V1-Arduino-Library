Packet format (14 bytes):


[0] = 0xFF (header byte 1)

[1] = 0x00 (header byte 2)

[2] = 0x00 or 0xFF (Relay channel 1)

[3] = 0x00 or 0xFF (Relay channel 2)

[4] = 0x00 or 0xFF (Relay channel 3)

[5] = 0x00 or 0xFF (Relay channel 4)

[6] = 0x00 or 0xFF (Relay channel 5)

[7] = 0x00 or 0xFF (Relay channel 6)

[8] = 0x00 or 0xFF (Relay channel 7)

[9] = 0x00 or 0xFF (Relay channel 8)

[10] = 0x00 or 0xFF (Relay channel 9)

[11] = 0x00 or 0xFF (Relay channel 10)

[12] = 0x00 or 0xFF (CRC-16 High Byte)

[13] = 0x00 or 0xFF (CRC-16 Low Byte)


0x00 = OFF

0xFF = ON


(CRC-16 + 1 bit) is generated from first 12 bytes. Baud rate: 115200.


The CRC-16 is generated and then a single bit is added to this value. The reasoning behind this is that the PWM and Pneumatic relays share the same serial data pin. PWM data is CRC-16'd where as the pneumatic data is CRC-16'd + 1.