#include <Arduino.h>
#include <ArduinoVESC.h>
#include <Adafruit_NeoPixel.h>
#define PIN        6
#define NUMPIXELS 16

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 500

ArduinoVESC VESC;

void setup()
{
  Serial.begin(115200, SERIAL_8N1, 20, 21); // RX=20, TX=21
  Serial1.begin(57600, SERIAL_8N1, 4, 5); // RX=4, TX=5
  while (!Serial) {;}
  while (!Serial1) {;}
  Serial.println("TEST");
  long randNumber = random(300);
  printf("%i\n", randNumber);
  VESC.setSerialPort(&Serial1);
  if ( VESC.getVescValues() ) {
    float rpm = VESC.data.rpm;
    Serial.println(rpm);
    Serial.println(VESC.data.rpm);
    Serial.println(VESC.data.inpVoltage);
    Serial.println(VESC.data.ampHours);
    Serial.println(VESC.data.tachometerAbs);
  }
  int val = analogRead(1);
  printf("%i\n", val);

  pixels.begin();
  pixels.clear();
  for(int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 150, 0));
    pixels.show();
    delay(DELAYVAL);
  }
}

void loop()
{
  Serial.println("TEST");
  if (Serial.available()) {      // If anything comes in Serial (USB),

    Serial1.write(Serial.read());   // read it and send it out Serial1 (pins 0 & 1)

  }

  if (Serial1.available()) {     // If anything comes in Serial1 (pins 0 & 1)

    Serial.write(Serial1.read());   // read it and send it out Serial (USB)

  }
}
