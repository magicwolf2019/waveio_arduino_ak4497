# Convert WaveIO modes in AK4497 with Arduino

Decoder board WEILLIANG AK4497 has a header for connecting AMANERO compatible USB converters in I2S. In order to connect the WaveIO board to this header, needs control the AK4497 modes using a header to indicate the modes Waveio (J9). Arduino board is used with the «Waveio_AK4497.ino» program to convert Waveio modes in AK4497. Connection schema – «Schema_Waveio_i2s_arduino_Ak4497.jpg». Connection example for debugging - «P1080124.jpg». PIN 1 and 2 header I2S on the board AK4497 used for GND Arduino and header Isolated I2S WaveIO. PIN 9 and 10 header I2S on the board AK4497 used for VCC 5V Arduino and header Isolated I2S WaveIO. 

