/*
 * Test closed-loop control with encoders
 */
byte m1SigPin = 4,
  m1EnablePin = 5,
  m2EnablePin = 6,
  m2SigPin = 7;
  
byte es1Pin = 2, // encoder signal 1 pin
  es2Pin = 3; // encoder signal 2 pin
  
volatile long coder[] = {0,0};

long lastRotVel[] = {0,0};

static unsigned long lastTime = 0;

long currentCount,
  currentTime,
  timeDiff,
  countDiff,
  lastCount;
  
double countsPerSec,
  rpm,
  desiredRPM = 100.0,
  err;

void setup() 
{
  Serial.begin(9600);
  
  while(!Serial);

  displayMessage("Starting ClosedLoopControlTest.ino");

  initMotionControl();
}

// Construct a motion controller
void initMotionControl()
{
  startEncoderChannels();
  
  startMotorChannels();

  stopMoving();

  displayMessage("Motion control initialized.");
}

void startEncoderChannels()
{
  attachInterrupt(digitalPinToInterrupt(es1Pin), updateEs1Vel, CHANGE);
  attachInterrupt(digitalPinToInterrupt(es2Pin), updateEs2Vel, CHANGE);

  displayMessage("Encoder channels started.");
}

void startMotorChannels()
{
  /* Start Motor Channel 1 */
  pinMode(m1EnablePin, OUTPUT);
  pinMode(m1SigPin, OUTPUT);

  /* Start Motor Channel 2 */
  pinMode(m2EnablePin, OUTPUT);
  pinMode(m2SigPin, OUTPUT);

  displayMessage("Motor channels started.");
}

void stopMoving()
{
  digitalWrite(m1EnablePin, LOW);
  digitalWrite(m2EnablePin, LOW);

  displayMessage("Stopped moving.");
}

void loop() 
{
  int ppr = 20;

  displayStatus();

  currentCount = coder[0];
  
  currentTime = micros();
  
  timeDiff = currentTime - lastTime;

  countDiff = coder[0] - lastCount;

  countsPerSec = (double) countDiff / (double) timeDiff * 1000000;

  rpm = countsPerSec / (double) ppr * 60.0;

  err = desiredRPM - rpm;

  analogWrite(m1EnablePin, constrain(err, 0, 255));

  while(micros() - lastTime < 5000);

  lastTime = currentTime;

  lastCount = currentCount;
}

void displayMessage(String msg)
{
  Serial.print("Message: '");
  Serial.print(msg);
  Serial.println("'\n");
}

void displayStatus()
{
  Serial.print("Current Count: ");
  Serial.println(currentCount);
  Serial.print("Current Time: ");
  Serial.println(currentTime);
  Serial.print("Time Diff: ");
  Serial.println(timeDiff);
  Serial.print("Count Diff: ");
  Serial.println(countDiff);
  Serial.print("Counts Per Sec: ");
  Serial.println(countsPerSec);
  Serial.print("RPM: ");
  Serial.println(rpm);
  Serial.print("Error: ");
  Serial.println(err);
  Serial.print("Last Time: ");
  Serial.println(lastTime);
  Serial.print("Last Count: ");
  Serial.println(lastCount);
  Serial.println();
}

//======Interrupt Functions======
void updateEs1Vel()
{
  if(digitalRead(m1SigPin) == LOW)
    coder[0]++;

  else
    coder[0]--;
}

void updateEs2Vel()
{
  coder[1]++;
}

