#include <LedControl.h>
#include <IRremote.hpp>

//IR setup
int RECV_PIN = 11;

IRrecv irrecv(RECV_PIN);

decode_results results;

//LED board setup

LedControl lc=LedControl(3,6,2,1);

//Snake vars

enum Direction{Up=0xFF629D, Down=0xFFA857, Left=0xFF22DD, Right=0xFFC23D, Power=0xFFA25D};

Direction currentDirection;

int snake[64];
int board[64];
int snakeLength = 0;
int apple;
bool paused = false;
int tickCount = 0;

void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver

  lc.shutdown(0,false);
  // Set brightness to a medium value
  lc.setIntensity(0,8);
  // Clear the display
  lc.clearDisplay(0);  

  snake[0] = xytoint(4, 3);
  currentDirection = Down;
  spawnApple();
  draw();
}

void loop() {
  if (irrecv.decode(&results)) {
    switch (results.value) {
      case Up :
        setDirection(Up);
        break;
      case Down :
        setDirection(Down);
        break;
      case Left :
        setDirection(Left);
        break;
      case Right :
        setDirection(Right);
        break; 
      case Power :
        paused = (paused == true) ? false : true; 
        break;
    }
    irrecv.resume(); // Receive the next value
  }
  tick();
  delay(50);
}

void tick() {
  if (paused == true) {
    return;
  }
  if (tickCount < 9) {
    tickCount = tickCount + 1;
    return;
  }
  else if (tickCount == 9) {
    tickCount = 0;
  }
  int x = inttox(snake[0]);
  int y = inttoy(snake[0]);
  int target;
  switch (currentDirection) {
    case Up :
      y = y - 1;
      break;
    case Down : 
      y = y + 1;
      break;
    case Left :
      x = x - 1;
      break;
    case Right :
      x = x + 1;
      break;
  }
  if ((x < 0) || (x > 7) || (y < 0) || (y > 7)) {
    die();
    return;
  }

  target = xytoint(x, y);
  
  if (board[target] == 1) {
    if (target == apple) {
      snakeLength = snakeLength + 1;
      apple = -1;
    }
    else {
      die();
    }
  }  
  memset(board, 0, sizeof(board));
  for (int i=snakeLength; i>0; i--) {
    snake[i] = snake[i-1];
    board[snake[i]] = 1;
  }
  snake[0] = xytoint(x, y);
  board[snake[0]] = 1;
  if (apple == -1) {
    spawnApple();
  }
  board[apple] = 1;
  draw();
}

void setDirection(Direction direction) {
  if (direction == Up) {
    if ( (currentDirection != Down)) {
      currentDirection = Up;  
    }
  }
  if (direction == Down) {
    if ( (currentDirection != Up)) {
      currentDirection = Down;  
    }
  }
  if (direction == Left) {
    if ( (currentDirection != Right)) {
      currentDirection = Left;  
    }
  }
  if (direction == Right) {
    if ( (currentDirection != Left)) {
      currentDirection = Right;  
    }
  }
}

int xytoint(int x, int y) {
  return (y*8)+x;
}

int inttox(int var) {
  return var % 8;
}

int inttoy (int var) {
  return var / 8;
}

void die() {
  for (int i=0; i<8; i++) {
    lc.setRow(0,i,0b11111111);
  }
  delay(2000);
  snakeLength = 0;
  memset(snake, 0, sizeof(snake));
  memset(board, 0, sizeof(board));
  snake[0] = xytoint(4, 3);
  board[snake[0]] = 1;
  spawnApple();
  lc.clearDisplay(0);
  draw();
}

void draw() {
  lc.clearDisplay(0);
  for (int i=0; i<64;i++) {
    if (board[i] == 1) {
      lc.setLed(0,inttoy(i),inttox(i),true);
    }
  }
}

void spawnApple() {
  int rand;
  while (true) {
    rand = random(0,64);
    if (board[rand] == 0) {
      apple = rand;
      board[rand] = 1;
      break;
    }
  }
}
