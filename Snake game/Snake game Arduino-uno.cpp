#include <LedControl.h> // LED matrix control library

// ###############################################################
// ####################### PIN CONFIGURATION #####################
// ###############################################################

struct Pin {
  static const short joystickX = A2;   // Joystick X-axis pin
  static const short joystickY = A3;   // Joystick Y-axis pin
  static const short joystickVCC = 15; // Virtual VCC for the joystick
  static const short joystickGND = 14; // Virtual GND for the joystick
  static const short potentiometer = A5; // Potentiometer for snake speed
  static const short potentiometer_2 = A4; // Potentiometer for brightness
  static const short CLK = 10;   // Clock for LED matrix
  static const short CS  = 11;   // Chip-select for LED matrix
  static const short DIN = 12;   // Data-in for LED matrix
};

// ###############################################################
// ###################### GAME CONSTANTS #########################
// ###############################################################

const int matrixSize = 8;
const short initialSnakeLength = 3;
const int joystickThreshold = 150;
const short DIR_UP = 1, DIR_RIGHT = 2, DIR_DOWN = 3, DIR_LEFT = 4; // Directions
const int maxSnakeLength = matrixSize * matrixSize;

// ###############################################################
// ################### GAME STATE VARIABLES ######################
// ###############################################################

LedControl lc(Pin::DIN, Pin::CLK, Pin::CS, 1);

struct Point {
  int x = 0, y = 0;
  Point(int x = 0, int y = 0) : x(x), y(y) {}
};

bool isGameOver = false, isWin = false;
Point food(-1, -1);
Point snakeBody[maxSnakeLength]; // Fixed-size array for snake body
int snakeLength = 1; // Current length of the snake
int snakeDirection = 0; // Start stationary
int snakeSpeed = 200;
int brightnessLevel = 8;
Point joystickNeutral(500, 500);

// ###############################################################
// ######################### SETUP ###############################
// ###############################################################

void setup() {
  Serial.begin(115200);
  initializeHardware();
  calibrateJoystick();
  snakeBody[0] = Point(random(matrixSize), random(matrixSize)); // Initialize snake's head
  spawnFood();
}

// ###############################################################
// ########################## LOOP ###############################
// ###############################################################

void loop() {
  if (!isGameOver) {
    adjustBrightness();
    processJoystickInput();
    moveSnake();
    handleCollisions();
    renderFrame();
    delay(snakeSpeed);
  } else {
    lc.clearDisplay(0); // Clear the display when the game is over
  }
}

// ***************************************************************
// ******************** FUNCTION DEFINITIONS ********************
// ***************************************************************

// ----------------------- Initialization -----------------------

void initializeHardware() {
  pinMode(Pin::joystickVCC, OUTPUT);
  pinMode(Pin::joystickGND, OUTPUT);
  digitalWrite(Pin::joystickVCC, HIGH);
  digitalWrite(Pin::joystickGND, LOW);

  lc.shutdown(0, false);
  lc.setIntensity(0, brightnessLevel);
  lc.clearDisplay(0);
}

void calibrateJoystick() {
  Point avg;
  for (int i = 0; i < 10; i++) {
    avg.x += analogRead(Pin::joystickX);
    avg.y += analogRead(Pin::joystickY);
  }
  joystickNeutral = Point(avg.x / 10, avg.y / 10);
}

// ------------------- Game Logic Functions ---------------------

void spawnFood() {
  do {
    food = Point(random(matrixSize), random(matrixSize));
  } while (isSnakeAt(food));
}

bool isSnakeAt(Point p) {
  for (int i = 0; i < snakeLength; i++) {
    if (snakeBody[i].x == p.x && snakeBody[i].y == p.y) return true;
  }
  return false;
}

void processJoystickInput() {
  int joyX = analogRead(Pin::joystickX);
  int joyY = analogRead(Pin::joystickY);

  if (joyY < joystickNeutral.y - joystickThreshold && snakeDirection != DIR_DOWN) snakeDirection = DIR_UP;
  if (joyY > joystickNeutral.y + joystickThreshold && snakeDirection != DIR_UP) snakeDirection = DIR_DOWN;
  if (joyX < joystickNeutral.x - joystickThreshold && snakeDirection != DIR_RIGHT) snakeDirection = DIR_LEFT;
  if (joyX > joystickNeutral.x + joystickThreshold && snakeDirection != DIR_LEFT) snakeDirection = DIR_RIGHT;

  int potValue = analogRead(Pin::potentiometer);
  snakeSpeed = map(potValue, 0, 1023, 50, 500);
}

void moveSnake() {
  if (snakeDirection == 0) return;

  Point head = snakeBody[0];
  if (snakeDirection == DIR_UP) head.y--;
  if (snakeDirection == DIR_DOWN) head.y++;
  if (snakeDirection == DIR_LEFT) head.x--;
  if (snakeDirection == DIR_RIGHT) head.x++;

  // Wrap around edges
  head.x = (head.x + matrixSize) % matrixSize;
  head.y = (head.y + matrixSize) % matrixSize;

  // Shift body segments to follow the head
  for (int i = snakeLength; i > 0; i--) {
    snakeBody[i] = snakeBody[i - 1];
  }
  snakeBody[0] = head; // Update head position

  // Check if food is eaten
  if (head.x == food.x && head.y == food.y) {
    if (snakeLength < maxSnakeLength) {
      snakeLength++;
    }
    spawnFood(); // Generate new food
  }
}

void handleCollisions() {
  Point head = snakeBody[0];

  // Check collision with body
  for (int i = 1; i < snakeLength; i++) {
    if (snakeBody[i].x == head.x && snakeBody[i].y == head.y) {
      isGameOver = true;
      return;
    }
  }
}

// ------------------- Display Functions -----------------------

void renderFrame() {
  lc.clearDisplay(0);

  // Render snake
  for (int i = 0; i < snakeLength; i++) {
    lc.setLed(0, snakeBody[i].y, snakeBody[i].x, true);
  }

  // Render food
  lc.setLed(0, food.y, food.x, true);
}

// -------------------- Utility Functions ----------------------

void adjustBrightness() {
  int potValue = analogRead(Pin::potentiometer_2);
  brightnessLevel = map(potValue, 0, 1023, 0, 15);
  lc.setIntensity(0, brightnessLevel);
}

