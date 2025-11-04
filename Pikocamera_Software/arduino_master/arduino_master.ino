#include <Wire.h>

#define PAYLOAD_I2C_ADD 0x9
#define PAYLOAD_I2C_RESET 0x7e
#define PAYLOAD_I2C_REFRESH 0x15
#define PAYLOAD_I2C_CAPTURE_IMAGE 0x25
#define PAYLOAD_I2C_UPDATE_SSDV_PKT 0x35
#define PAYLOAD_I2C_UPDATE_MINI_PKT 0x45
#define PAYLOAD_I2C_FETCH_SSDV_FLASH 0x55
#define PAYLOAD_I2C_MAX_INPUT_BYTES 32
#define PAYLOAD_I2C_IMAGE_CAPTURED 0x65
#define PAYLOAD_I2C_GET_SSDV_CNT 0x75

inline void payload_send_command(uint8_t add, uint8_t cmd)
{
  Wire.beginTransmission(add);
  Wire.write(cmd);
  Wire.endTransmission();

  // Wait for payload restart
  if(cmd == PAYLOAD_I2C_RESET)
  {
    for (uint8_t i = 0; i < 5; i++)
    {
      delay(1000);
    }
  }
}

void payload_request_packet()
{
  // payload_send_command(PAYLOAD_I2C_ADD, PAYLOAD_I2C_RESET);
  payload_send_command(PAYLOAD_I2C_ADD, PAYLOAD_I2C_CAPTURE_IMAGE);

     // Wait
    Serial.print("/* ");
    for (uint8_t i = 0; i < 10; i++)
    {
      Serial.print('!');
      delay(1000);
    }
    Serial.println(" */\n");
    
  uint8_t flag = 0;
  uint8_t number_of_packets = 0;
  payload_send_command(PAYLOAD_I2C_ADD, PAYLOAD_I2C_GET_SSDV_CNT);
  Wire.requestFrom(PAYLOAD_I2C_ADD, 1);
  while (Wire.available())
  {
    number_of_packets = uint8_t(Wire.read());
  }
  
  // SSDV Packet loop
  payload_send_command(PAYLOAD_I2C_ADD, PAYLOAD_I2C_REFRESH);
  Serial.println("const uint8_t ssdv_packets[] =\n{");
  
  for (uint8_t i = 0; i < number_of_packets; i++)
  {
    payload_send_command(PAYLOAD_I2C_ADD, PAYLOAD_I2C_FETCH_SSDV_FLASH);
    delay(100);

    // Mini Packet loop
    for (uint8_t j = 0; j < 8; j++)
    {
      // Print
      Serial.print("/*");
      Serial.print(i);
      Serial.print('.');
      Serial.print(j);
      Serial.print("*/ ");

      // Ask for 32 SSDV bytes
      Wire.requestFrom(PAYLOAD_I2C_ADD, PAYLOAD_I2C_MAX_INPUT_BYTES);

      while (Wire.available())
      {
        char c = Wire.read();
        Serial.print("0x");
        Serial.print(uint8_t(c), HEX);
        Serial.print(", ");
        flag = 1;
      }
      if (flag)
      {
        Serial.println();
      }
      flag = 0;
      payload_send_command(PAYLOAD_I2C_ADD, PAYLOAD_I2C_UPDATE_MINI_PKT);
    }
    payload_send_command(PAYLOAD_I2C_ADD, PAYLOAD_I2C_UPDATE_SSDV_PKT);
  }
  Serial.println("};");
  payload_send_command(PAYLOAD_I2C_ADD, PAYLOAD_I2C_RESET);
}

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  Serial.println("\r\n/* Press 'a' to capture image. */");
}

void loop()
{
  char input = 0;
  if(Serial.available())
  {
    input = Serial.read();
  }
  
  if(input == 'a')
  {
    Serial.println("/* Fetching SSDV packets. Please wait. */");
    payload_request_packet();
    //Serial.println("\r\n/* Press 'a' to capture image. */");
  }
}
