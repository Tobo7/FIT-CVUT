#include <Esplora.h>
#include <SPI.h>
#include <SD.h>
#include <TFT.h>
#include "objects.h"
#define DOWN  SWITCH_1
#define UP  SWITCH_3
#define LEFT  SWITCH_2
#define RIGHT  SWITCH_4

enum states {
  START, LVL1, LVL2, LVL3, DEFEAT, WIN
};
enum states STATE, NEXT_STATE;

Alien *** invaders;

Tank * tank;

unsigned long interval = 0;
unsigned long interval2 = 0;
const int sX = 0; //start margin of aliens
const int sY = 5;
const int distX = 15;
const int distY = 15;
int moveAlienY; //alien movespeed
int sizeX = 0;//size of alien array
int sizeY = 0;
int moveAlien; //direction of alien movement
int shoot_random; //probability of alien shooting
bool soundEnabled = false;
bool button1 = false;

void display_clear() {
  EsploraTFT.stroke(0, 0, 0);
  EsploraTFT.fill(0, 0, 0);
  EsploraTFT.rect(0, 0, 160, 128);
}
//Display START screen function
//----------------------------------------------------
void sound(int x, int y) {
  if (soundEnabled)
    Esplora.tone(x, y);
}

int start_intensity;
void display_start(bool init) {

  if (init) {
    display_clear();
    start_intensity = 255;
    interval = -500;

    if (!soundEnabled) {
      EsploraTFT.stroke(255, 255, 255);
      EsploraTFT.setTextSize(1);
      EsploraTFT.text("muted", 130, 0);
    }

    EsploraTFT.stroke(0, 255, 0);
    EsploraTFT.setTextSize(2);
    EsploraTFT.text("SHAPE", 50, 20);
    EsploraTFT.text("INVADERS", 30, 45);
  }

  //sounds control
  if (Esplora.readButton(LEFT) == LOW && !button1) {
    soundEnabled = !soundEnabled;
    Serial.println(soundEnabled);
    button1 = true;

    if (!soundEnabled) {
      EsploraTFT.stroke(255, 255, 255);
      EsploraTFT.setTextSize(1);
      EsploraTFT.text("muted", 130, 0);
    } else {
      EsploraTFT.stroke(0, 0, 0);
      EsploraTFT.fill(0, 0, 0);
      EsploraTFT.rect(130, 0, 30, 10);

    }
  } else if (Esplora.readButton(LEFT) && button1)
    button1 = false;

  //text blink
  if (millis() > interval + 500) {
    EsploraTFT.stroke(start_intensity, start_intensity, start_intensity);
    EsploraTFT.setTextSize(1);
    EsploraTFT.text("Press 'switch 1' to", 25, 80);
    EsploraTFT.text("start!", 65, 92);

    if (start_intensity == 255) {
      start_intensity = 100;
    } else
      start_intensity = 255;

    interval = millis();
  }
}

//Display NEXT LEVEL screen function
//----------------------------------------------------
void display_next_level(int level) {
  display_clear();
  char levelChar[2];
  String(level).toCharArray(levelChar, 2);

  for (int i = 0; i < 3; i++) {

    EsploraTFT.stroke(0, 255, 0);
    EsploraTFT.setTextSize(3);
    EsploraTFT.text(levelChar, 76, 70);
    EsploraTFT.text("LEVEL", 40, 40);
    delay(350);
    display_clear();
  }
}

//Display DEFEAT screen function
//------------------------------------------------------
void display_defeat() {
  display_clear();

  EsploraTFT.stroke(255, 0, 0);
  EsploraTFT.setTextSize(3);
  EsploraTFT.text("GAME", 47, 10);
  EsploraTFT.text("OVER", 47, 45);

  EsploraTFT.stroke(255, 255, 255);
  EsploraTFT.setTextSize(1);

  EsploraTFT.text("Press 'switch 3' to", 25, 80);
  EsploraTFT.text("restart!", 65, 92);
}
//Display WIN screen function
//------------------------------------------------------
void display_win() {
  display_clear();

  EsploraTFT.stroke(0, 255, 0);
  EsploraTFT.setTextSize(3);
  EsploraTFT.text("YOU", 60, 10);
  EsploraTFT.text("WIN", 60, 45);

  EsploraTFT.stroke(255, 255, 255);
  EsploraTFT.setTextSize(1);

  EsploraTFT.text("You defeated all", 32, 80);
  EsploraTFT.text("invaders!", 62, 92);
}

//INITILAZE game function
//------------------------------------------------------
void game_init(int movx, int movy, int x, int y, int level, int shoot_rand, int color, int setEnemy[], int sizeOfArray) {
  int col;

  //free previous alien objects
  for (int i = 0; i < sizeY; i++) {
    for (int j = 0; j < sizeX; j++) {
      free(invaders[i][j]);
    }
    free(invaders[i]);
  }
  free(invaders);
  //initilaze game attributes
  sizeX = x;
  sizeY = y;
  shoot_random = shoot_rand;
  moveAlien = movx;
  moveAlienY = movy;


  EsploraTFT.stroke(0, 0, 0);
  EsploraTFT.fill(0, 0, 0);
  EsploraTFT.rect(0, 0, 160, 124);
  EsploraTFT.stroke(255, 255, 255);
  EsploraTFT.line(0, 118, 160, 118);
  EsploraTFT.stroke(255, 0, 0);
  EsploraTFT.line(0, 109, 3, 109);
  EsploraTFT.line(160, 109, 157, 109);

  //create tank
  tank->create(EsploraTFT.width() / 2 - 8);
  tank->drawHp();

  //print level
  char levelChar[2];
  String(level).toCharArray(levelChar, 2);
  EsploraTFT.stroke(0, 255, 0);
  EsploraTFT.setTextSize(1);
  EsploraTFT.text(levelChar, 21, 120);
  EsploraTFT.stroke(255, 255, 255);
  EsploraTFT.text("LVL", 2, 120);

  // create aliens
  invaders = (Alien ***)malloc(y * sizeof(Alien **));
  for (int i = 0; i < y; ++i)
    invaders[i] = (Alien **)malloc(x * sizeof(Alien*));

  for (int i = 0; i < sizeY; i++) {
    for (int j = 0; j < sizeX; j++) {
      invaders[i][j] = new Alien(soundEnabled);
      invaders[i][j]->setPresetColor(color);
      invaders[i][j]->create(sX + distX * j, sY + distY * i);
    }
  }

  //special attributes from function parameters for aliens
  for ( int i = 0 ; i < sizeOfArray ; i += 3 ) {
    for ( int j = 0 ; j < setEnemy[i]; j++ ) {
      int rY = random(0, y);
      int rX = random(0, x);
      if (invaders[rY][rX]->max_hp == 1) {
        invaders[rY][rX]->setHp(setEnemy[i + 1]);
        invaders[rY][rX]->setPresetColor(setEnemy[i + 2]);
      }
      else
        j--;
    }
  }
}


//HANDLE GAME
//------------------------------------------------------
int movxAlien(int i, int j, int moveSide, int down) {
  //alien movement
  invaders[i][j]->movx(moveSide, down);

  //alien shooting
  if (!invaders[i][j]->dead && !invaders[i][j]->shooting && !random(0, shoot_random)) {
    int shootY;
    //find lowest alive in column
    for (int y = sizeY-1; y >=0 ; y--) {
      shootY = invaders[y][j]->aY;
      if (!invaders[y][j]->dead)
        break;
    }
    invaders[i][j]->shoot(shootY);
  }

  if (!invaders[i][j]->dead && invaders[i][j]->aY + 12 > 110)
    return 1;
  return 0;
}

int game() {

  //read joystick
  if (millis() > interval + 50) {
    int joystick = Esplora.readJoystickX();
    if (joystick > 50 || joystick < -50) {
      int pos = map(-joystick, -512, 512, -3, 3);
      tank->mov(pos);
    }

    //tank shooting
    if (Esplora.readButton(DOWN) == LOW && !tank->shooting) {
      tank->shoot();
      sound(4978, 10);
    } else if (tank->shooting) {
      tank->moveBullet();

      //check tank bullet collision with alien
      bool destroyed = false;
      for (int i = sizeY - 1; i >= 0; i--) {
        for (int j = sizeX - 1; j >= 0; j--) {
          if (!invaders[i][j]->dead && invaders[i][j]->aX < tank->bulletX + tank->bulletWidth &&
              invaders[i][j]->aX + 12 > tank->bulletX &&
              invaders[i][j]->aY < tank->bulletY + 5 &&
              invaders[i][j]->aY + 12 > tank->bulletY) {

            sound(100, 20);
            invaders[i][j]->destroy();
            tank->stopBullet();
            destroyed = true;
            break;

          }
        }
        if (destroyed)
          break;
      }
    }



    //check aliens bullets collision with tank
    bool someoneAlive = false; //also check if any alien exists
    for (int i = sizeY - 1; i >= 0; i--) {
      for (int j = sizeX - 1; j >= 0; j--) {
        if (invaders[i][j]->shooting) {
          invaders[i][j]->moveBullet();
          if (tank->posX <= invaders[i][j]->bulletX && tank->posX + 12 >= invaders[i][j]->bulletX && tank->posY <= invaders[i][j]->bulletY && tank->posY + 15 >= invaders[i][j]->bulletY) {
            invaders[i][j]->stopBullet();
            tank->hp--;
            tank->mov(0);
            sound(20, 500);
            if (!tank->hp) {
              return 2;
            }
            tank->drawHp();
          }
        }
        if (!someoneAlive && !invaders[i][j]->dead)
          someoneAlive = true;
      }
    }

    if (!someoneAlive)//all aliens dead
      return 1;
    interval = millis();
  }


  if (millis() > interval2 + 505) { // wait to move all aliens
    int moveSide = moveAlien;
    int down = 0;

    // change movement directions if aliens are on edge
    if ( invaders[sizeY - 1][sizeX - 1]->aX + 12 + moveAlien > 160 || invaders[0][0]->aX + moveAlien < 0) {
      moveAlien *= -1;
      moveSide = 0;
      down = moveAlienY;
    }
  
    //move aliens
    for (int i = sizeY - 1; i >= 0; i--) {
      if (moveAlien > 0) {
        for (int j = sizeX - 1; j >= 0; j--) {
          if (movxAlien(i, j, moveSide, down))
            return 2;
        }
      } else {
        for (int j = 0; j < sizeX; j++) {
          if (movxAlien(i, j, moveSide, down))
            return 2;
        }
      }
    }
    interval2 = millis();
  }
  return 0;
}


//SETUP
//------------------------------------------------------
void setup() {
  Serial.begin(9600);
  EsploraTFT.begin();
  EsploraTFT.background(0, 0, 0);

  STATE = START;
  display_start(true);

}

//CHEAT BUTTONS

bool cheat() {
  if (!Esplora.readButton(RIGHT) && !Esplora.readJoystickSwitch())
    return true;
  else
    return false;
}

//LOOP
//------------------------------------------------------
void loop()
{
  int state;
  switch (STATE)
  {
    case DEFEAT:
      if (Esplora.readButton(UP) == LOW) {
        display_start(true);
        NEXT_STATE = START;
      }
      break;

    case START:
      display_start(false);
      if (Esplora.readButton(DOWN) == LOW) {
        display_next_level(1);
        //free previous tank and create new
        free(tank);
        tank = new Tank(soundEnabled, 3);
        int s = 6;
        int setEnemy[s] = {3, 2, 0, 2, 5, 5};
        game_init(5, 5, 7, 3, 1, 80, 6, setEnemy, s);

        NEXT_STATE = LVL1;
      }

      break;
    case LVL1:
      state = game();
      if (state == 1 || cheat()) {
        display_next_level(2);

        int s = 9;
        int setEnemy[s] = {10, 2, 0, 7, 3, 5, 1, 10, 6};
        game_init(6, 5, 8, 3, 2, 60, 2, setEnemy, s);

        NEXT_STATE = LVL2;
      } else if (state == 2) {
        display_defeat();
        NEXT_STATE = DEFEAT;
      }
      break;
    case LVL2:
      state = game();
      if (state == 1 || cheat()) {
        display_next_level(3);

        int s = 9;
        int setEnemy[s] = {6, 3, 0, 6, 5, 5, 3, 12, 6};
        game_init(7, 6, 6, 3, 3, 30, 2, setEnemy, s);

        NEXT_STATE = LVL3;
      }  else if (state == 2) {
        display_defeat();
        NEXT_STATE = DEFEAT;
      }
      break;
    case LVL3:
      state = game();
      if (state == 1 || cheat()) {
        display_win();
        NEXT_STATE = WIN;
      } else if (state == 2) {
        display_defeat();
        NEXT_STATE = DEFEAT;
      }
      break;

    case WIN:
      break;
  }

  if (Esplora.readButton(UP) == LOW) {
    display_start(true);
    NEXT_STATE = START;
  }
  STATE = NEXT_STATE;
}
