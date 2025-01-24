#include <U8g2lib.h>
#include <Wire.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// Init hardware
const int startGamebuttonPin = 2; // Button to start the game
const int playGamebuttonPin = 8;  // Button to make the character jump
const int buzzerPin = 7;
const int ledPin = 4;
const int gravity = 1;
const int jumpStrength = 10;
const int groundY = 50;
const int obstacleWidth = 5;

// Game variables
int ballY = groundY;
int ballSpeed = 0;
int obstacleX = 128;
int score = 0;
int highScore = 0;
bool isJumping = false;
bool inMenu = true;
bool gameStarted = false;

void setup() {
  pinMode(startGamebuttonPin, INPUT_PULLUP);
  pinMode(playGamebuttonPin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  u8g2.begin();
}

void loop() {
  if (inMenu) {
    showMenu();

    // Start the game when the start button is pressed
    if (digitalRead(startGamebuttonPin) == LOW) {
      delay(200); // Debounce
      while (digitalRead(startGamebuttonPin) == LOW); // Wait for release
      inMenu = false;
      resetGame();
    }
  } else {
    // Jump logic for the play button
    if (digitalRead(playGamebuttonPin) == LOW && ballY == groundY) {
      isJumping = true;
      ballSpeed = -jumpStrength;
    }

    updateGame();
    drawGame();
    delay(50);
  }
}

void resetGame() {
  ballY = groundY;
  ballSpeed = 0;
  obstacleX = 128;
  score = 0;
  isJumping = false;
  gameStarted = false;
  digitalWrite(ledPin, LOW);
  noTone(buzzerPin);
}

void updateGame() {
  // Gravity and jumping logic
  ballY += ballSpeed;
  ballSpeed += gravity;
  if (ballY > groundY) {
    ballY = groundY;
    ballSpeed = 0;
    isJumping = false;
  }

  // Obstacle movement and collision
  obstacleX -= 5;
  if (obstacleX < -obstacleWidth) {
    obstacleX = 128;
    score++;
    highScore = max(score, highScore);
  }

  // Collision detection
  if (obstacleX < 20 && obstacleX + obstacleWidth > 10 && ballY >= groundY - 10) {
    digitalWrite(ledPin, HIGH);
    tone(buzzerPin, 1000, 500);
    inMenu = true; // Go back to the menu on collision
  }
}

void drawGame() {
  // Draw the game elements on the display
  u8g2.clearBuffer();
  u8g2.drawLine(0, groundY + 10, 128, groundY + 10); // Ground line
  u8g2.drawCircle(15, ballY, 5, U8G2_DRAW_ALL);       // Ball
  u8g2.drawBox(obstacleX, groundY - 20, obstacleWidth, 20); // Obstacle

  // Display the score and high score
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setCursor(0, 10);
  u8g2.print("Score: ");
  u8g2.print(score);
  u8g2.setCursor(0, 20);
  u8g2.print("Highscore: ");
  u8g2.print(highScore);

  u8g2.sendBuffer();
}

void showMenu() {
  // Display the menu
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setCursor(0, 10);
  u8g2.print("WELCOME TO ");
  u8g2.setCursor(0, 20);
  u8g2.print("HURDLE HOPPER!");
  u8g2.setCursor(0, 40);
  u8g2.print("Press RED to play");
  u8g2.sendBuffer();
}
