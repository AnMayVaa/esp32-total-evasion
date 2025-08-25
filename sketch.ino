// --- ไลบรารีที่จำเป็น ---
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <math.h> // <-- เพิ่มเข้ามาสำหรับคำนวณการพุ่ง

// --- ตั้งค่า Hardware Pins ---
#define TFT_DC    2
#define TFT_CS    5
#define TFT_RST   4
#define BUTTON_PIN 13
#define JOY_X_PIN 34
#define JOY_Y_PIN 35

// --- ค่าคงที่ของจอและเกม ---
#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240
#define COLOR_PLAYER 0x001F // Blue
#define COLOR_ENEMY  0xF800 // Red (Normal)
#define COLOR_STARS  0x8410 // Gray
#define COLOR_CHASER 0xF81F // Magenta
#define COLOR_BOUNCER 0x07E0 // Green
#define COLOR_DASHER 0xFFFF // White
#define COLOR_DASH   0x07FF // Cyan

// การตั้งค่าเกม
#define PLAYER_SPEED 4
#define MAX_ENEMIES  12 // <-- เพิ่มจำนวนศัตรูสูงสุด
#define ENEMY_SPAWN_INTERVAL_START 1000
#define ENEMY_SPAWN_INTERVAL_MIN 350
#define ENEMY_SPEED_START 2

// การตั้งค่า Dash แบบ Double-Tap
#define DASH_SPEED 12
#define DASH_DURATION 150
#define DOUBLE_TAP_WINDOW 300

// สร้าง Object ของจอภาพ
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

// --- Structs ---
enum JoystickDirection { DIR_CENTER, DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT };
struct Player {
  int x, y, lastX, lastY; int w = 20, h = 20;
  bool isDashing = false; unsigned long dashStartTime = 0;
  JoystickDirection dashDirection;
};

// --- คุณสมบัติใหม่: เพิ่มประเภทของศัตรู ---
enum EnemyType { NORMAL, CHASER, BOUNCER, DASHER };

struct Enemy {
  int x, y, lastX, lastY; int w = 15, h = 15; // ลดขนาดศัตรูลงเล็กน้อย
  bool active = false; EnemyType type;
  float speedX, speedY; // สำหรับ Bouncer และ Dasher
  unsigned long lastActionTime = 0;
  bool hasLockedTarget = false; // สำหรับ Dasher
};

// --- ตัวแปร Global ---
enum GameState { STATE_STARTUP, STATE_GAMEPLAY, STATE_GAME_OVER };
GameState gameState = STATE_STARTUP;
Player player;
Enemy enemies[MAX_ENEMIES];
long score = 0;
unsigned long startTime = 0;
unsigned long lastEnemySpawnTime = 0;
// ตัวแปรสำหรับ Double-Tap
JoystickDirection lastTapDirection = DIR_CENTER;
unsigned long lastTapTime = 0;

// --- Setup และ Loop ---
void setup() {
  Serial.begin(115200); pinMode(BUTTON_PIN, INPUT_PULLUP);
  randomSeed(analogRead(0)); tft.begin();
  tft.setRotation(1); drawStartupScreen();
}

void loop() {
  switch (gameState) {
    case STATE_STARTUP:
      if (digitalRead(BUTTON_PIN) == LOW) { initGame(); gameState = STATE_GAMEPLAY; delay(200); }
      break;
    case STATE_GAMEPLAY:
      score = (millis() - startTime) / 100;
      updatePlayer(); updateEnemies();
      spawnEnemy(); checkCollisions();
      drawPlayer(); drawEnemies();
      drawScore(); delay(16);
      break;
    case STATE_GAME_OVER:
      drawGameOverScreen(); while(digitalRead(BUTTON_PIN) == HIGH) { delay(10); }
      drawStartupScreen(); gameState = STATE_STARTUP; delay(200);
      break;
  }
}

// --- ฟังก์ชันจัดการเกม ---
void initGame() {
  tft.fillScreen(ILI9341_BLACK);
  for (int i=0; i < 50; i++) { tft.drawPixel(random(SCREEN_WIDTH), random(SCREEN_HEIGHT), COLOR_STARS); }
  startTime = millis(); score = 0;
  player.x = (SCREEN_WIDTH / 2) - (player.w / 2); player.y = SCREEN_HEIGHT - player.h - 10;
  player.lastX = player.x; player.lastY = player.y; player.isDashing = false;
  for (int i=0; i<MAX_ENEMIES; i++) { enemies[i].active = false; }
  tft.setTextSize(2); tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(5, 5); tft.print("SCORE:");
}

// --- ฟังก์ชัน Update (คำนวณ) ---
JoystickDirection getJoystickDirection() {
  int joyX = analogRead(JOY_X_PIN); int joyY = analogRead(JOY_Y_PIN);
  if (joyX > 2800) return DIR_LEFT;   if (joyX < 1200) return DIR_RIGHT;
  if (joyY > 2800) return DIR_UP;     if (joyY < 1200) return DIR_DOWN;
  return DIR_CENTER;
}

void updatePlayer() {
  static JoystickDirection lastDirection = DIR_CENTER;
  JoystickDirection currentDirection = getJoystickDirection();
  if (lastDirection == DIR_CENTER && currentDirection != DIR_CENTER) {
    if (currentDirection == lastTapDirection && (millis() - lastTapTime < DOUBLE_TAP_WINDOW)) {
      player.isDashing = true; player.dashStartTime = millis();
      player.dashDirection = currentDirection; lastTapDirection = DIR_CENTER;
    } else {
      lastTapDirection = currentDirection; lastTapTime = millis();
    }
  }
  lastDirection = currentDirection;
  if (player.isDashing && (millis() - player.dashStartTime > DASH_DURATION)) { player.isDashing = false; }
  int currentSpeed = player.isDashing ? DASH_SPEED : PLAYER_SPEED;
  if (player.isDashing) {
    if(player.dashDirection == DIR_LEFT) player.x -= currentSpeed; if(player.dashDirection == DIR_RIGHT) player.x += currentSpeed;
    if(player.dashDirection == DIR_UP) player.y -= currentSpeed;   if(player.dashDirection == DIR_DOWN) player.y += currentSpeed;
  } else {
    if(currentDirection == DIR_LEFT) player.x -= currentSpeed; if(currentDirection == DIR_RIGHT) player.x += currentSpeed;
    if(currentDirection == DIR_UP) player.y -= currentSpeed;   if(currentDirection == DIR_DOWN) player.y += currentSpeed;
  }
  if (player.x < 0) player.x = 0; if (player.x > SCREEN_WIDTH - player.w) player.x = SCREEN_WIDTH - player.w;
  if (player.y < 0) player.y = 0; if (player.y > SCREEN_HEIGHT - player.h) player.y = SCREEN_HEIGHT - player.h;
}

void spawnEnemy() {
  long currentSpawnInterval = ENEMY_SPAWN_INTERVAL_START - (score * 2);
  if (currentSpawnInterval < ENEMY_SPAWN_INTERVAL_MIN) currentSpawnInterval = ENEMY_SPAWN_INTERVAL_MIN;
  if (millis() - lastEnemySpawnTime > currentSpawnInterval) {
    lastEnemySpawnTime = millis();
    for (int i = 0; i < MAX_ENEMIES; i++) {
      if (!enemies[i].active) {
        enemies[i].active = true;
        enemies[i].x = random(0, SCREEN_WIDTH - enemies[i].w); enemies[i].y = -enemies[i].h;
        enemies[i].lastX = enemies[i].x; enemies[i].lastY = enemies[i].y;
        enemies[i].lastActionTime = millis(); enemies[i].hasLockedTarget = false;
        
        // สุ่มประเภทศัตรูตามคะแนน
        int roll = random(0, 10);
        if (score > 700 && roll < 2) enemies[i].type = DASHER;    // 20%
        else if (score > 400 && roll < 4) enemies[i].type = BOUNCER; // 20%
        else if (score > 150 && roll < 7) enemies[i].type = CHASER;   // 30%
        else enemies[i].type = NORMAL; // 30%

        // ตั้งค่าเริ่มต้นให้ศัตรูใหม่
        if (enemies[i].type == BOUNCER) {
          enemies[i].speedX = random(0, 2) == 0 ? 3 : -3;
          enemies[i].speedY = random(2, 4);
          enemies[i].y = random(0, 50); // เกิดที่ด้านบนๆ
        }
        return;
      }
    }
  }
}

void updateEnemies() {
  float currentEnemySpeed = ENEMY_SPEED_START + (score / 200.0);
  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (enemies[i].active) {
      switch (enemies[i].type) {
        case NORMAL:
          enemies[i].y += currentEnemySpeed;
          break;
        case CHASER:
          enemies[i].y += currentEnemySpeed * 0.7;
          if (enemies[i].x < player.x) enemies[i].x += 1;
          if (enemies[i].x > player.x) enemies[i].x -= 1;
          break;
        case BOUNCER:
          enemies[i].x += enemies[i].speedX;
          enemies[i].y += enemies[i].speedY;
          if (enemies[i].x <= 0 || enemies[i].x >= SCREEN_WIDTH - enemies[i].w) enemies[i].speedX *= -1;
          if (enemies[i].y <= 0 || enemies[i].y >= SCREEN_HEIGHT - enemies[i].h) enemies[i].speedY *= -1;
          break;
        case DASHER:
          if (!enemies[i].hasLockedTarget) {
            enemies[i].y += 1; // ค่อยๆ เลื่อนลงมา
            if (millis() - enemies[i].lastActionTime > 1500) { // เล็ง 1.5 วิ
              float deltaX = player.x - enemies[i].x;
              float deltaY = player.y - enemies[i].y;
              float distance = sqrt(deltaX * deltaX + deltaY * deltaY);
              enemies[i].speedX = (deltaX / distance) * 8; // พุ่งด้วยความเร็ว 8
              enemies[i].speedY = (deltaY / distance) * 8;
              enemies[i].hasLockedTarget = true;
            }
          } else { // กำลังพุ่ง
            enemies[i].x += enemies[i].speedX;
            enemies[i].y += enemies[i].speedY;
          }
          break;
      }
      if (enemies[i].y > SCREEN_HEIGHT + 20 || enemies[i].y < -20) { enemies[i].active = false; }
    }
  }
}

void checkCollisions() {
  if (player.isDashing) return; // ถ้าแดชอยู่เป็นอมตะ
  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (enemies[i].active) {
      if (player.x < enemies[i].x + enemies[i].w && player.x + player.w > enemies[i].x &&
          player.y < enemies[i].y + enemies[i].h && player.y + player.h > enemies[i].y) {
        tft.fillScreen(COLOR_ENEMY); delay(100);
        gameState = STATE_GAME_OVER;
        return;
      }
    }
  }
}

// --- ฟังก์ชัน Draw ---
void drawPlayer() {
  tft.fillRect(player.lastX, player.lastY, player.w, player.h, ILI9341_BLACK);
  uint16_t currentPlayerColor = player.isDashing ? COLOR_DASH : COLOR_PLAYER;
  tft.fillRect(player.x, player.y, player.w, player.h, currentPlayerColor);
  player.lastX = player.x; player.lastY = player.y;
}
void drawEnemies() {
  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (enemies[i].active) {
      tft.fillRect(enemies[i].lastX, enemies[i].lastY, enemies[i].w, enemies[i].h, ILI9341_BLACK);
      uint16_t enemyColor;
      switch(enemies[i].type) {
        case NORMAL:  enemyColor = COLOR_ENEMY;   break;
        case CHASER:  enemyColor = COLOR_CHASER;  break;
        case BOUNCER: enemyColor = COLOR_BOUNCER; break;
        case DASHER:  // ทำให้ Dasher กระพริบตอนกำลังเล็ง
          if (!enemies[i].hasLockedTarget && (millis() / 200) % 2 == 0) {
            enemyColor = ILI9341_BLACK;
          } else {
            enemyColor = COLOR_DASHER;
          }
          break;
      }
      tft.fillRect(enemies[i].x, enemies[i].y, enemies[i].w, enemies[i].h, enemyColor);
      enemies[i].lastX = enemies[i].x; enemies[i].lastY = enemies[i].y;
    }
  }
}
void drawScore() {
  static long lastScore = -1;
  if (score != lastScore) {
    tft.fillRect(100, 5, 120, 16, ILI9341_BLACK);
    tft.setCursor(100, 5);
    tft.print(score);
    lastScore = score;
  }
}
void drawStartupScreen() {
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextSize(3);
  tft.setTextColor(ILI9341_CYAN);
  tft.setCursor(15, 60);
  tft.println("TOTAL EVASION");
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(25, 150);
  tft.println("Double-tap to Dash!");
}
void drawGameOverScreen() {
  tft.fillScreen(COLOR_ENEMY);
  tft.setTextSize(3);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(70, 60);
  tft.println("GAME OVER");
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setCursor(50, 130);
  tft.print("Final Score: ");
  tft.println(score);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(50, 180);
  tft.println("Press START to retry");
}