// Include libraries
#include <U8g2lib.h>
#include <Wire.h>

// Initialize the OLED display and I2C communication
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// Init hardware
const int startGamebuttonPin = 2; // Button to start the game
const int playGamebuttonPin = 8;  // Button to make the character jump
const int buzzerPin = 7;
const int ledPin = 4;
const int gravity = 1; // Gravity effect on the ball
const int jumpStrength = 10; // Strength of the jump
const int groundY = 50; // Ground level position
const int obstacleWidth = 5; // Width of the obstacles

// Define game variables
int ballY = groundY; // Ball's Y position
int ballSpeed = 0;
int obstacleX = 128; // Obstacle position
int score = 0;
int highScore = 0;
bool isJumping = false; // Track if the ball is jumping
bool inMenu = true; // Track if the game is in the meny
bool gameStarted = false; // Track if the game has started


void setup() {
  // Set button and pin modes
  pinMode(startGamebuttonPin, INPUT_PULLUP);
  pinMode(playGamebuttonPin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  u8g2.begin(); // Initialize the OLED display
}

void loop() {
  if (inMenu) {
    showMenu();

    // Start the game when the start button is pressed
    if (digitalRead(startGamebuttonPin) == LOW) {
      delay(200);
      while (digitalRead(startGamebuttonPin) == LOW); // Wait for button release
      inMenu = false;
      resetGame();
    }
  } else {
    // Jump logic for the play button
    if (digitalRead(playGamebuttonPin) == LOW && ballY == groundY) {
      isJumping = true;
      ballSpeed = -jumpStrength;
    }

    updateGame(); // Update game mechanics
    drawGame(); // Draw the game elements
    delay(50);
  }
}

void resetGame() {
  // Reset all game variables to their initial state
  ballY = groundY;
  ballSpeed = 0;
  obstacleX = 128;
  score = 0;
  isJumping = false;
  gameStarted = false;
  digitalWrite(ledPin, LOW);
  noTone(buzzerPin);
}

// Gravity and jumping logic
void updateGame() {
  ballY += ballSpeed; // Apply vertical movement
  ballSpeed += gravity; // Apply gravity
  if (ballY > groundY) {
    ballY = groundY; // Keep the ball on the ground
    ballSpeed = 0;
    isJumping = false;
  }

  // Obstacle movement and score
  obstacleX -= 5;
  if (obstacleX < -obstacleWidth) {
    obstacleX = 128;
    score++;
    highScore = max(score, highScore);
  }

  // Check for collision with obstacles
  if (obstacleX < 20 && obstacleX + obstacleWidth > 10 && ballY >= groundY - 10) {
    digitalWrite(ledPin, HIGH);
    tone(buzzerPin, 1000, 500);
    inMenu = true; // Return to the menu after collision
  }
}

void drawGame() {
  // Draw the game elements on the display
  u8g2.clearBuffer();
  u8g2.drawLine(0, groundY + 10, 128, groundY + 10); // Draw the ground line
  u8g2.drawCircle(15, ballY, 5, U8G2_DRAW_ALL);       // Draw the ball
  u8g2.drawBox(obstacleX, groundY - 20, obstacleWidth, 20); // Draw the obstacles

  // Display the current score and high score
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setCursor(0, 10);
  u8g2.print("Score: ");
  u8g2.print(score);
  u8g2.setCursor(0, 20);
  u8g2.print("Highscore: ");
  u8g2.print(highScore);

  u8g2.sendBuffer(); // Update the display
}

// Show the game menu with instructions
void showMenu() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setCursor(0, 10);
  u8g2.print("WELCOME TO ");
  u8g2.setCursor(0, 20);
  u8g2.print("HURDLE HOPPER!");
  u8g2.setCursor(0, 40);
  u8g2.print("Press RED to start");
  u8g2.setCursor(0, 60);
  u8g2.print("Press GREEN to play");
  u8g2.sendBuffer();
}
