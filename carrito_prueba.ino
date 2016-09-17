#include <NewPing.h>
#include <Servo.h>
//sonar
#define SONAR_NUM     2 // Number or sensors.
#define MAX_DISTANCE 500 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define PING_INTERVAL 33 // Milliseconds between sensor pings (29ms is about the min to avoid cross-sensor echo).
#define TRIGGER_PIN1  31  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN1     30  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define TRIGGER_PIN2  49  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN2     48  // Arduino pin tied to echo pin on the ultrasonic sensor.

//wired
#define HG7881_A_IA 8 // D10 --> Motor A Input A --> MOTOR A +
#define HG7881_A_IB 9 // D11 --> Motor A Input B --> MOTOR A -
#define HG7881_B_IA 10 // D10 --> Motor B Input A --> MOTOR B +
#define HG7881_B_IB 11 // D11 --> Motor B Input B --> MOTOR B -

// functional connections
#define MOTOR_A_PWM HG7881_A_IA // Motor B PWM Speed
#define MOTOR_A_DIR HG7881_A_IB // Motor B Direction
#define MOTOR_B_PWM HG7881_B_IA // Motor B PWM Speed
#define MOTOR_B_DIR HG7881_B_IB // Motor B Direction

// the actual values for "fast" and "slow" depend on the motor
#define PWM_SLOW 50  // arbitrary slow speed PWM duty cycle
#define PWM_FAST 200 // arbitrary fast speed PWM duty cycle
#define DIR_DELAY 10000 // brief delay for abrupt motor changes

//unsigned long pingTimer[SONAR_NUM]; // Holds the times when the next ping should happen for each sensor.
//unsigned int cm[SONAR_NUM]; // Where the ping distances are stored.
//uint8_t currentSensor = 0;  // Keeps track of which sensor is active.
int distance = 20;
NewPing sonar[SONAR_NUM] = {
  NewPing(TRIGGER_PIN1, ECHO_PIN1, MAX_DISTANCE), // Each sensor's trigger pin, echo pin, and max distance to ping.
  NewPing(TRIGGER_PIN2, ECHO_PIN2, MAX_DISTANCE)
};

Servo servo1;
Servo servo2;
int pos = 0;    // variable to store the servo position
bool flag = true;

void setup() {
  // put your setup code here, to run once:
  Serial.begin( 9600 );
  pinMode( MOTOR_A_DIR, OUTPUT );
  pinMode( MOTOR_B_DIR, OUTPUT );
  pinMode( MOTOR_A_PWM, OUTPUT );
  pinMode( MOTOR_B_PWM, OUTPUT );

  digitalWrite( MOTOR_A_DIR, LOW );
  digitalWrite( MOTOR_B_DIR, LOW );
  digitalWrite( MOTOR_A_PWM, LOW );
  digitalWrite( MOTOR_B_PWM, LOW );
  servo1.attach(32);
  servo2.attach(46);// attaches the servo on pin 9 to the servo object
}

void loop() {
  moveCar();
  unsigned int uS[SONAR_NUM];
  for (int i = 0; i < SONAR_NUM; i++) {
    uS[i] = sonar[i].ping(); // Send ping, get ping time in microseconds (uS).
    Serial.print("Ping ");
    Serial.print(i);
    Serial.print("=");
    Serial.print(uS[i] / US_ROUNDTRIP_CM); // Convert ping time to distance in cm and print result (0 = outside set distance range)
    Serial.println("cm");
  }
  if ( uS[0] / US_ROUNDTRIP_CM < distance ||  uS[1] / US_ROUNDTRIP_CM < distance) {
    int moveDelay=700;
    stopCar();    
    angle();
    delay(2000);
    for (int i = 0; i < SONAR_NUM; i++) {
      uS[i] = sonar[i].ping(); // Send ping, get ping time in microseconds (uS).
      Serial.print("Ping Lateral");
      Serial.print(i);
      Serial.print("=");
      Serial.print(uS[i] / US_ROUNDTRIP_CM); // Convert ping time to distance in cm and print result (0 = outside set distance range)
      Serial.println("cm");  
    }
    if(uS[0] / US_ROUNDTRIP_CM > uS[1] / US_ROUNDTRIP_CM ){
      Serial.println( "A la izquierda" );
         moveCarLeft();
         servo1.write(90);
         servo2.write(90);
         delay(moveDelay);
    } else
    if(uS[0] / US_ROUNDTRIP_CM < uS[1] / US_ROUNDTRIP_CM){
      Serial.println( "A la derecha" );
         moveCarRight();
         servo1.write(90);
         servo2.write(90);
         delay(moveDelay);
    }else
    if ( uS[0] / US_ROUNDTRIP_CM < distance &&  uS[1] / US_ROUNDTRIP_CM < distance) {
        Serial.println( "Reversa" );
        reverse();
        servo1.write(90);
        servo2.write(90);
        delay(moveDelay);
        Serial.println( "Paso el delay de reversa" );
       
    } else
    if (( uS[0] / US_ROUNDTRIP_CM < distance) && ((uS[1] / US_ROUNDTRIP_CM > distance) || (uS[1] / US_ROUNDTRIP_CM == 0 ))) {
         Serial.println( "A la derecha" );
         moveCarRight();
         servo1.write(90);
         servo2.write(90);
         delay(moveDelay);
    }  else
    if (( uS[1] / US_ROUNDTRIP_CM < distance) && ((uS[0] / US_ROUNDTRIP_CM > distance) || (uS[0] / US_ROUNDTRIP_CM == 0 ))) {
         Serial.println( "A la derecha" );
         moveCarRight();
         servo1.write(90);
         servo2.write(90);
         delay(moveDelay);
    }   
     servo1.write(90);
     servo2.write(90); 
  }
 

}

void moveCar() {
  Serial.println( "Fast forward..." );
  // set the motor speed and direction
  digitalWrite( MOTOR_A_DIR, HIGH );
  digitalWrite( MOTOR_B_DIR, HIGH ); // direction = forward
  analogWrite( MOTOR_A_PWM, 255 - PWM_FAST);
  analogWrite( MOTOR_B_PWM, 255 - PWM_FAST ); // PWM speed = fast
}

void moveCarRight() {
  Serial.println( "Right..." );
  // set the motor speed and direction
  digitalWrite( MOTOR_A_DIR, HIGH );
  digitalWrite( MOTOR_B_DIR, LOW ); // direction = forward
  analogWrite( MOTOR_A_PWM, 255 - PWM_FAST );
  //analogWrite( MOTOR_B_PWM, 255 - PWM_SLOW ); // PWM speed = fast
  //delay(700);
}

void moveCarLeft() {
  Serial.println( "Left..." );
  // set the motor speed and direction
  digitalWrite( MOTOR_A_DIR, LOW );
  digitalWrite( MOTOR_B_DIR, HIGH ); // direction = forward
  //analogWrite( MOTOR_A_PWM, 255 - PWM_FAST );
  analogWrite( MOTOR_B_PWM, 255 - PWM_FAST ); // PWM speed = fast
  //delay(700);
}
void stopCar() {
  Serial.println( "Soft stop..." );
  digitalWrite( MOTOR_A_DIR, LOW );
  digitalWrite( MOTOR_B_DIR, LOW );
  digitalWrite( MOTOR_A_PWM, LOW );
  digitalWrite( MOTOR_B_PWM, LOW );
}

void angle() {
  if (flag) {
    Serial.println( "a los lados..." );
    //servo1.write(90); es mi cero en loos 2
    servo1.write(180);
    servo2.write(0);
    flag = false;
    delay(100);
  }
  flag=true;
}

void reverse() {
  Serial.println( "Fast forward..." );
   // set the motor speed and direction
  digitalWrite( MOTOR_B_DIR, LOW ); // direction = reverse
  digitalWrite( MOTOR_A_DIR, LOW ); // direction = reverse
  analogWrite( MOTOR_B_PWM, PWM_FAST ); // PWM speed = fast
  analogWrite( MOTOR_A_PWM, PWM_FAST ); // PWM speed = fast
}
