#include <SoftwareSerial.h>

// HC05 pins
#define RX 2
#define TX 3

SoftwareSerial btSerial(RX, TX);

// enable motor
#define ENABLE_MOTOR_LEFT_BACK A0
#define ENABLE_MOTOR_RIGHT_BACK A1
#define ENABLE_MOTOR_LEFT_FRONT A2
#define ENABLE_MOTOR_RIGHT_FRONT A3

// motor pins
#define PIN_1_MOTOR_LEFT_BACK 5
#define PIN_2_MOTOR_LEFT_BACK 6
#define PIN_1_MOTOR_RIGHT_BACK 7
#define PIN_2_MOTOR_RIGHT_BACK 8
#define PIN_1_MOTOR_LEFT_FRONT 9
#define PIN_2_MOTOR_LEFT_FRONT 10
#define PIN_1_MOTOR_RIGHT_FRONT 11
#define PIN_2_MOTOR_RIGHT_FRONT 12

// constants
#define NO_SIGNAL_NO_MOVEMENT 127

typedef struct _PacketData_
{
  byte lx_axis_value_ = NO_SIGNAL_NO_MOVEMENT;
  byte ly_axis_value_ = NO_SIGNAL_NO_MOVEMENT;
  byte rx_axis_value_ = NO_SIGNAL_NO_MOVEMENT;
  byte ry_axis_value_ = NO_SIGNAL_NO_MOVEMENT;
} PacketData;

// forward declaration
void setMotor(PacketData data);
void rotateMotor(bool state_left_front, bool state_right_front, bool state_left_back, bool state_right_back,
                 int speed_left_front, int speed_right_front, int speed_left_back, int speed_right_back);

//---------------------------------------------------------------------------------------------------------------------
///
/// values from 0 to 254, 127 is center
///
//
void setup()
{
  pinMode(ENABLE_MOTOR_LEFT_BACK, OUTPUT);
  pinMode(ENABLE_MOTOR_LEFT_FRONT, OUTPUT);
  pinMode(ENABLE_MOTOR_RIGHT_BACK, OUTPUT);
  pinMode(ENABLE_MOTOR_RIGHT_FRONT, OUTPUT);
  pinMode(PIN_1_MOTOR_LEFT_BACK, OUTPUT);
  pinMode(PIN_2_MOTOR_LEFT_BACK, OUTPUT);
  pinMode(PIN_1_MOTOR_LEFT_FRONT, OUTPUT);
  pinMode(PIN_2_MOTOR_LEFT_FRONT, OUTPUT);
  pinMode(PIN_1_MOTOR_RIGHT_BACK, OUTPUT);
  pinMode(PIN_2_MOTOR_RIGHT_BACK, OUTPUT);
  pinMode(PIN_1_MOTOR_RIGHT_FRONT, OUTPUT);
  pinMode(PIN_2_MOTOR_RIGHT_FRONT, OUTPUT);
  
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
  String dataString;
  PacketData x_y_data;

  unsigned long lastRecvTime = 0;

  if (btSerial.available())
  {
    dataString = btSerial.readStringUntil('\n');
    sscanf(dataString.c_str(), "%d,%d,%d,%d", &x_y_data.lx_axis_value_, &x_y_data.ly_axis_value_, &x_y_data.rx_axis_value_, &x_y_data.ry_axis_value_);
    // Serial.println("Received the value");
    // Serial.println(x_y_data.lx_axis_value_);
    Serial.println(x_y_data.ly_axis_value_); // needed
    Serial.println(x_y_data.rx_axis_value_); // needed
    // Serial.println(x_y_data.ry_axis_value_);

    setMotor(x_y_data);
    lastRecvTime = millis(); 
  }
  else
  {
    Serial.println("btSerial not avaiable");
    unsigned long now = millis();
    if (now - lastRecvTime > 1000 ) // Signal lost after 1 second. Reset the motor to stop
    {
      x_y_data.ly_axis_value_ = NO_SIGNAL_NO_MOVEMENT;
      x_y_data.rx_axis_value_ = NO_SIGNAL_NO_MOVEMENT;
      setMotor(x_y_data);
    }
  }
  delay(10);
}

//---------------------------------------------------------------------------------------------------------------------
///
/// values from 0 to 254, 127 is center
///
//
void setMotor(PacketData data)
{
  // forward - all tires forwards
  if (data.ly_axis_value_ > 130 && data.rx_axis_value_ == 127) 
  {
    rotateMotor(HIGH, HIGH, HIGH, HIGH, 
                255, 255,
                255, 255);
    Serial.println("forward");
    return;
  }
  // backwards - all tires backwards
  else if (data.ly_axis_value_ < 122 && data.rx_axis_value_ == 127) 
  {
    rotateMotor(LOW, LOW, LOW, LOW, 
                255, 255,
                255, 255);
    Serial.println("backwards"); 
    return;
  }
  // going right - left front and right back forward, left back and right front backwards
  else if (data.ly_axis_value_ == 127 && data.rx_axis_value_ > 130)
  {
    rotateMotor(HIGH, LOW, LOW, HIGH, 
                255, 255,
                255, 255);
    Serial.println("right");
    return;
  }
  // going left - left front and right back backwards, left back and right front forward
  else if (data.ly_axis_value_ == 127 && data.rx_axis_value_ < 122) 
  {
    rotateMotor(LOW, HIGH, HIGH, LOW, 
                255, 255,
                255, 255);
    Serial.println("left");
    return;
  }
  // going right forward - left front and right back forward, left back and right front nothing
  else if (data.ly_axis_value_ > 130 && data.rx_axis_value_ > 130) 
  {
    rotateMotor(HIGH, LOW, LOW, HIGH, 
                255, 0,
                0, 255);
    Serial.println("right forward");
    return;
  }
  // going left forward - left back and right front forward, left front and right back nothing
  else if (data.ly_axis_value_ > 130 && data.rx_axis_value_ < 122) 
  {
    rotateMotor(HIGH, LOW, LOW, HIGH, 
                0, 255,
                255, 0);
    Serial.println("left forward");
    return;
  }
  // going right backwards - left back and right front backwards, left front and right back nothing
  else if (data.ly_axis_value_ < 122 && data.rx_axis_value_ > 130) 
  {
    rotateMotor(HIGH, LOW, LOW, HIGH, 
                0, 255,
                255, 0);
    Serial.println("right backwards");
    return;
  }
  // going left backwards - left front and right back backwards, left back and right front nothing
  else if (data.ly_axis_value_ < 122 && data.rx_axis_value_ < 122) 
  {
    rotateMotor(LOW, HIGH, HIGH, LOW, 
                255, 0,
                0, 255);
    Serial.println("left backwards");
    return;
  }
  // no connection or no movementn
  else /*(data.ly_axis_value_ == NO_SIGNAL_NO_MOVEMENT && data.rx_axis_value_ == NO_SIGNAL_NO_MOVEMENT) */
  {
    rotateMotor(LOW, LOW, LOW, LOW, 0, 0, 0, 0);
    Serial.println("nothing");
    return;
  }
}


//---------------------------------------------------------------------------------------------------------------------
///
/// this functions writes all the values to the specific pins for the motors
///
/// @param values for the motors
//
void rotateMotor(bool state_left_front, bool state_right_front, bool state_left_back, bool state_right_back,
                 int speed_left_front, int speed_right_front, int speed_left_back, int speed_right_back)
{
  digitalWrite(PIN_1_MOTOR_LEFT_BACK, state_left_back);
  digitalWrite(PIN_2_MOTOR_LEFT_BACK, !state_left_back);

  digitalWrite(PIN_1_MOTOR_LEFT_FRONT, state_left_front);
  digitalWrite(PIN_2_MOTOR_LEFT_FRONT, !state_left_front);

  digitalWrite(PIN_1_MOTOR_RIGHT_BACK, state_right_back);
  digitalWrite(PIN_2_MOTOR_RIGHT_BACK, !state_right_back);

  digitalWrite(PIN_1_MOTOR_RIGHT_FRONT, state_right_front);
  digitalWrite(PIN_2_MOTOR_RIGHT_FRONT, !state_right_front);

  analogWrite(ENABLE_MOTOR_LEFT_BACK, speed_left_back);
  analogWrite(ENABLE_MOTOR_LEFT_FRONT, speed_left_front);
  analogWrite(ENABLE_MOTOR_RIGHT_BACK, speed_right_back);
  analogWrite(ENABLE_MOTOR_RIGHT_FRONT, speed_right_front);
}

