// Initialize constants and servos
#include <Servo.h>

// Drive servos
Servo DriveRight;
Servo DriveLeft;
const int DriveRightPin = 5; // Right-side drive servo pin
const int DriveLeftPin = 6; // Left-side drive servo pin
const int stop_right = 91; // Speed required to stop right-side drive servo
const int stop_left = 89; // Speed required to stop left-side drive servo
const int move_forward_slow = 10; // Additional speed required to move forward slowly
const int move_forward_fast = 30; // Additional speed required to move forward quickly
const int move_backward_slow = -10; // Additional speed required to move backward slowly
const int move_backward_fast = -30; // Additional speed required to move backward quickly

// Scan servo
Servo Scan;
const int ScanPin = 3; // Scan servo pin
const int scan_max = 100; // Maximum degrees reached by Scan servo
const int scan_min = 80; // Minimum degrees reached by Scan servo
int scan_pos = 90; // Intial position of Scan servo (i.e., straight ahead)
int scan_change = 2; // Number of degrees stepped by Scan servo, changes between +5 and -5 as code runs

// Ultrasonic sensor
const int echoPin = 2; // Ultrasonic sensor Echo pin
const int trigPin = 4; // Ultrasonic sensor Trigger pin
int obstacle = 0; // Variable that indicates the presence/distance to an obstacle
                  // 0 = no obstacle detected
                  // 1 = obstacle less than "distance_far" (in cm) away
                  // 2 = obstacle less than "distance_close" (in cm) away
const int distance_far = 15; // Distance threshold for obstacles that are deemed "far away"
const int distance_close = 5; // Distance threshold for obstacles that are deemed "close"

// Feedback
const int LEDPin = 13; // Onboard LED pin

void setup() 
{ 
  Serial.begin(9600);
  
  // Drive servos
  DriveRight.attach(DriveRightPin);
  DriveLeft.attach(DriveLeftPin);
  DriveRight.write(stop_right);
  DriveLeft.write(stop_left);
  
  // Scan servo
  Scan.attach(ScanPin);
  Scan.write(scan_pos);
  
  // Ultrasonic sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(LEDPin, OUTPUT);
} 
 

void loop() 
{ 
  // Increment scanner position
  if ( (scan_pos < scan_max && scan_change > 0) || (scan_pos > scan_min && scan_change < 0) )
  {
    scan_pos = scan_pos + scan_change;
  }
  else if ( scan_pos == scan_max )
  {
    scan_change = -5;
    scan_pos = scan_pos + scan_change;
  }
  else if ( scan_pos == scan_min )
  {
    scan_change = 5;
    scan_pos = scan_pos + scan_change;
  }
  Serial.print("scan_pos = ");
  Serial.println(scan_pos);
  Scan.write(scan_pos);
  
  // Detect object in path
  int distance = DetectObject();
  if ( distance > distance_far ) { obstacle = 0; }
  else if ( distance <= distance_far && distance > distance_close ) { obstacle = 1; }
  else if ( distance <= distance_close ) { obstacle = 2; }
  
  //Decide if should forward and then move forward, fast or slow, or not
  if ( obstacle == 0 ) { DriveForwardFast(); }
  else if ( obstacle == 1 ) { DriveForwardSlow(); }
  else if ( obstacle == 2 )
  {
    DriveBackwardSlow();
    delay(500);
    TurnRight45();
  }
  
  // Pause until next loop
  delay(100);
      
}

void DriveForwardFast()
{
  DriveRight.write(stop_right+move_forward_fast);
  DriveLeft.write(stop_left-move_forward_fast);
}

void DriveForwardSlow()
{
  DriveRight.write(stop_right+move_forward_slow);
  DriveLeft.write(stop_left-move_forward_slow);
}

void DriveBackwardFast()
{
  DriveRight.write(stop_right+move_backward_fast);
  DriveLeft.write(stop_left-move_backward_fast);
}

void DriveBackwardSlow()
{
  DriveRight.write(stop_right+move_backward_slow);
  DriveLeft.write(stop_left-move_backward_slow);
}

void TurnRight45()
{
  DriveRight.write(stop_right+move_backward_slow);
  DriveLeft.write(stop_left-move_forward_slow);
  delay(1000);
}

void TurnLeft45()
{
  DriveRight.write(stop_right+move_forward_slow);
  DriveLeft.write(stop_left-move_backward_slow);
  delay(1000);
}

int DetectObject()
{
  int maximumRange = 200; // Maximum range needed
  int minimumRange = 0; // Minimum range needed
  long duration, distance; // Duration used to calculate distance

  /* The following trigPin/echoPin cycle is used to determine the
   distance of the nearest object by bouncing soundwaves off of it. */ 

  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2); 

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); 
 
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
 
  //Calculate the distance (in cm) based on the speed of sound.
  distance = duration/58.2;
 
  if (distance >= maximumRange || distance <= minimumRange)
  {
    /* Send a negative number to computer and Turn LED ON to indicate "out of range" */
    Serial.println("-1");
    digitalWrite(LEDPin, HIGH);
  }
  else {
    /* Send the distance to the computer using Serial protocol, and turn LED OFF to indicate successful reading. */
    Serial.println(distance);
    digitalWrite(LEDPin, LOW); 
  }
  
  return distance;
}
