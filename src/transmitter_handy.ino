#include <SoftwareSerial.h>
#include <RemoteXY.h> 

#define REMOTEXY_MODE__SOFTSERIAL

// RemoteXY connection settings 
#define REMOTEXY_SERIAL_RX 3
#define REMOTEXY_SERIAL_TX 2
#define REMOTEXY_SERIAL_SPEED 9600
#define REMOTEXY_ACCESS_PASSWORD "1234"

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



// RemoteXY GUI configuration  
#pragma pack(push, 1)  
uint8_t RemoteXY_CONF[] =   // 47 bytes
{
  255,4,0,0,0,40,0,17,0,0,0,24,2,106,200,200,84,1,1,2,
  0,5,210,14,143,143,19,6,60,60,0,2,26,31,5,2,14,143,143,114,
  6,60,60,0,2,26,31
};
  
// this structure defines all the variables and events of your control interface 
struct {

    // input variables
  int8_t not_used_01_x; // from -100 to 100
  int8_t joystick_01_y; // from -100 to 100
  int8_t sideways_x; // from -100 to 100
  int8_t not_used_02_y; // from -100 to 100

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0

} RemoteXY;   
#pragma pack(pop)

// forward declaration
void setMotor(struct RemoteXY data);
void rotateMotor(bool state_left_front, bool state_right_front, bool state_left_back, bool state_right_back,
                 int speed_left_front, int speed_right_front, int speed_left_back, int speed_right_back);
void mapData();
 
/////////////////////////////////////////////
//           END RemoteXY include          //
/////////////////////////////////////////////



void setup() 
{
  RemoteXY_Init (); 

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

  //Serial.begin(9600);
  Serial.println("Serial started");
}

void loop() 
{ 
  RemoteXY_Handler ();
  mapData();
  if (RemoteXY.connect_flag == 1)
    setMotor();
}

//---------------------------------------------------------------------------------------------------------------------
///
/// values from 0 to 254, 127 is center
///
//
void setMotor()
{
  // forward - all tires forwards
  if (RemoteXY.joystick_01_y > 130 && RemoteXY.sideways_x == 127) 
  {
    rotateMotor(HIGH, HIGH, HIGH, HIGH, 
                255, 255,
                255, 255);
    Serial.println("forward");
    return;
  }
  // backwards - all tires backwards
  else if (RemoteXY.joystick_01_y < 122 && RemoteXY.sideways_x == 127) 
  {
    rotateMotor(LOW, LOW, LOW, LOW, 
                255, 255,
                255, 255);
    Serial.println("backwards"); 
    return;
  }
  // going right - left front and right back forward, left back and right front backwards
  else if (RemoteXY.joystick_01_y == 127 && RemoteXY.sideways_x > 130)
  {
    rotateMotor(HIGH, LOW, LOW, HIGH, 
                255, 255,
                255, 255);
    Serial.println("right");
    return;
  }
  // going left - left front and right back backwards, left back and right front forward
  else if (RemoteXY.joystick_01_y == 127 && RemoteXY.sideways_x < 122) 
  {
    rotateMotor(LOW, HIGH, HIGH, LOW, 
                255, 255,
                255, 255);
    Serial.println("left");
    return;
  }
  // going right forward - left front and right back forward, left back and right front nothing
  else if (RemoteXY.joystick_01_y > 130 && RemoteXY.sideways_x > 130) 
  {
    rotateMotor(HIGH, LOW, LOW, HIGH, 
                255, 0,
                0, 255);
    Serial.println("right forward");
    return;
  }
  // going left forward - left back and right front forward, left front and right back nothing
  else if (RemoteXY.joystick_01_y > 130 && RemoteXY.sideways_x < 122) 
  {
    rotateMotor(HIGH, LOW, LOW, HIGH, 
                0, 255,
                255, 0);
    Serial.println("left forward");
    return;
  }
  // going right backwards - left back and right front backwards, left front and right back nothing
  else if (RemoteXY.joystick_01_y < 122 && RemoteXY.sideways_x > 130) 
  {
    rotateMotor(HIGH, LOW, LOW, HIGH, 
                0, 255,
                255, 0);
    Serial.println("right backwards");
    return;
  }
  // going left backwards - left front and right back backwards, left back and right front nothing
  else if (RemoteXY.joystick_01_y < 122 && RemoteXY.sideways_x < 122) 
  {
    Serial.println(RemoteXY.joystick_01_y);
    Serial.println(RemoteXY.sideways_x);
    rotateMotor(LOW, HIGH, HIGH, LOW, 
                255, 0,
                0, 255);
    Serial.println("left backwards");
    return;
  }
  // no connection or no movementn
  else /*(RemoteXY.joystick_01_y == NO_SIGNAL_NO_MOVEMENT && RemoteXY.sideways_x == NO_SIGNAL_NO_MOVEMENT) */
  {
    rotateMotor(LOW, LOW, LOW, LOW, 0, 0, 0, 0);
    Serial.println("nothing");
    return;
  }
}

void mapData()
{
  RemoteXY.sideways_x = map(RemoteXY.sideways_x, -100, 100, 0, 255);
  RemoteXY.joystick_01_y = map(RemoteXY.joystick_01_y, -100, 100, 0, 255);
}


//---------------------------------------------------------------------------------------------------------------------
///
/// this functions writes all the values to the specific pins for the motors
///
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
