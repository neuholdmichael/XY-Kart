#include <SoftwareSerial.h>

// HC05 pins
#define RX 2
#define TX 3

// joystick pins
#define LX_PIN A0
#define LY_PIN A1
#define RX_PIN A2
#define RY_PIN A3


SoftwareSerial btSerial(RX, TX);

// forward declration 
int mapAndAdjustJoystickDeadBandValues(int value);


typedef struct _PacketData_
{
  byte lxAxisValue_;
  byte lyAxisValue_;
  byte rxAxisValue_;
  byte ryAxisValue_;
} PacketData;
PacketData data;

//---------------------------------------------------------------------------------------------------------------------
///
/// values from 0 to 254, 127 is center
///
//
void setup()
{
  btSerial.begin(38400);
  
  Serial.begin(9600);
  Serial.println("Serial started");
}

//---------------------------------------------------------------------------------------------------------------------
///
/// values from 0 to 254, 127 is center
///
//
void loop()
{
  data.lxAxisValue_ = mapAndAdjustJoystickDeadBandValues(analogRead(LX_PIN));
  data.lyAxisValue_ = mapAndAdjustJoystickDeadBandValues(analogRead(LY_PIN));
  data.rxAxisValue_ = mapAndAdjustJoystickDeadBandValues(analogRead(RX_PIN));
  data.ryAxisValue_ = mapAndAdjustJoystickDeadBandValues(analogRead(RY_PIN));

  String dataString;
  dataString = dataString + data.lxAxisValue_ + ","  + data.lyAxisValue_ + "," + data.rxAxisValue_ + "," + data.ryAxisValue_ + "\n";
  
  Serial.println(data.lxAxisValue_);
  Serial.println(data.lyAxisValue_);
  Serial.println(data.rxAxisValue_);
  Serial.println(data.ryAxisValue_);
  Serial.println("now sending");
  btSerial.print(dataString); // send to HC05
  delay(10);
}

//---------------------------------------------------------------------------------------------------------------------
///
/// This function is used to map 0-1023 joystick value to 0-254. hence 127 is the center value which we send.
/// It also adjust the deadband in joystick.
/// Jotstick values range from 0-1023. But its center value is not always 511. It is little different.
/// So we need to add some deadband to center value. in our case 500-530. Any value in this deadband range is mapped to center 127.
///
/// @param value, analog value of the joystick of each direction
///
/// @return mapped value
//
// 
int mapAndAdjustJoystickDeadBandValues(int value)
{
  if (value >= 530)
    value = map(value, 530, 1023, 128, 254);

  else if (value <= 500)
    value = map(value, 500, 0, 126, 0);  

  else
    value = 127;

  return value;
}
