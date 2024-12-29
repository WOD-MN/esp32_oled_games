#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Joystick Pins
#define VRx 34
#define VRy 35
#define SW 32

// Game variables
int paddleX = 60; // Paddle position
int ballX = 64, ballY = 32; // Ball position
int ballSpeedX = 2, ballSpeedY = 2; // Ball speed
int paddleWidth = 20; // Paddle width
int paddleHeight = 4; // Paddle height
bool gameOver = false;
int score = 0; // Score

// Brick variables
int brickWidth = 16, brickHeight = 8;
int brickRows = 4, brickCols = 8;
bool bricks[4][8]; // Bricks

void setup() {
  pinMode(VRx, INPUT);
  pinMode(VRy, INPUT);
  pinMode(SW, INPUT_PULLUP);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  display.clearDisplay();
  randomSeed(analogRead(0));

  // Initialize the bricks
  for (int i = 0; i < brickRows; i++) {
    for (int j = 0; j < brickCols; j++) {
      bricks[i][j] = true;
    }
  }
}

void loop() {
  if (gameOver) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(30, 30);
    display.print("GAME OVER");
    display.setTextSize(1);
    display.setCursor(45, 45);
    display.print("Score: ");
    display.print(score);
    display.display();
    delay(2000); // Wait for 2 seconds before resetting
    resetGame();
    return;
  }

  readJoystick();
  updateBall();
  checkCollision();
  drawGame();
  delay(10); // Adjust delay for game speed
}

void readJoystick() {
  int xValue = analogRead(VRx);
  int yValue = analogRead(VRy);

  // Map joystick movement to paddle movement
  if (xValue < 1000) paddleX -= 4; // Move left
  else if (xValue > 3000) paddleX += 4; // Move right

  // Prevent the paddle from moving off-screen
  if (paddleX < 0) paddleX = 0;
  if (paddleX > SCREEN_WIDTH - paddleWidth) paddleX = SCREEN_WIDTH - paddleWidth;
}

void updateBall() {
  ballX += ballSpeedX;
  ballY += ballSpeedY;

  // Ball collision with walls
  if (ballX <= 0 || ballX >= SCREEN_WIDTH - 4) ballSpeedX = -ballSpeedX;
  if (ballY <= 0) ballSpeedY = -ballSpeedY;

  // Ball collision with paddle
  if (ballY >= SCREEN_HEIGHT - paddleHeight - 4 && ballX >= paddleX && ballX <= paddleX + paddleWidth) {
    ballSpeedY = -ballSpeedY;
    score++;
  }

  // Ball falls below the screen
  if (ballY >= SCREEN_HEIGHT) {
    gameOver = true;
  }
}

void checkCollision() {
  // Check for collision with bricks
  for (int i = 0; i < brickRows; i++) {
    for (int j = 0; j < brickCols; j++) {
      if (bricks[i][j] && ballX >= j * brickWidth && ballX < (j + 1) * brickWidth && ballY >= i * brickHeight && ballY < (i + 1) * brickHeight) {
        bricks[i][j] = false; // Remove brick
        ballSpeedY = -ballSpeedY;
        score += 10; // Increase score for breaking a brick
      }
    }
  }
}

void drawGame() {
  display.clearDisplay();

  // Draw paddle
  display.fillRect(paddleX, SCREEN_HEIGHT - paddleHeight, paddleWidth, paddleHeight, SSD1306_WHITE);

  // Draw ball
  display.fillRect(ballX, ballY, 4, 4, SSD1306_WHITE);

  // Draw bricks
  for (int i = 0; i < brickRows; i++) {
    for (int j = 0; j < brickCols; j++) {
      if (bricks[i][j]) {
        display.fillRect(j * brickWidth, i * brickHeight, brickWidth, brickHeight, SSD1306_WHITE);
      }
    }
  }

  // Draw score
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Score: ");
  display.print(score);

  display.display();
}

void resetGame() {
  // Reset the ball position
  ballX = 64;
  ballY = 32;
  ballSpeedX = 2;
  ballSpeedY = 2;

  // Reset the paddle position
  paddleX = 60;

  // Reset the bricks
  for (int i = 0; i < brickRows; i++) {
    for (int j = 0; j < brickCols; j++) {
      bricks[i][j] = true;
    }
  }

  // Reset score and game state
  score = 0;
  gameOver = false;
}
