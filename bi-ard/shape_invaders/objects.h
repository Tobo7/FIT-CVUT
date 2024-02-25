class Alien {
  private:
    int skin, r, g, b;
  public:
    int aX, aY;
    bool dead = false;
    int bulletX, bulletY;
    int bulletWidth = 2;
    bool shooting = false;
    int max_hp = 1;
    int hp = max_hp;
    bool soundEnabled;

    Alien(bool sE) {
      soundEnabled = sE;
    }

    void del() { // delete from display
      EsploraTFT.stroke(0, 0, 0);
      EsploraTFT.fill(0, 0, 0);
      EsploraTFT.rect(aX, aY, 13, 13);

    }

    void setHp(int hp_new) {
      max_hp = hp_new;
      hp = hp_new;
    }

    void shoot(int y) {
      bulletX = aX + 5;
      bulletY = y + 12;
      shooting = true;
      drawBullet();
    }

    void stopBullet() {
      shooting = false;
      removeBullet();
    }

    void moveBullet() {
      if (bulletY >= 113) {
        stopBullet();
      } else {
        removeBullet();
        bulletY += 2;
        drawBullet();
      }


    }

    void removeBullet() {
      EsploraTFT.stroke(0, 0, 0);
      EsploraTFT.fill(0, 0, 0);
      EsploraTFT.rect(bulletX, bulletY, bulletWidth, 4);
    }

    void drawBullet() {
      EsploraTFT.stroke(0, 255, 255);
      EsploraTFT.fill(255, 0, 0);
      EsploraTFT.rect(bulletX, bulletY, bulletWidth, 4);
    }

    void destroy() { // decrease hp / destroy
      hp--;
      if (!hp) {
        dead = true;
        del();
      } else {
        movx(0, 0);
      }

    }

    void create(int sX, int sY) {
      aX = sX;
      aY = sY;
      draw(aX, aY);
    }

    void draw(int sX, int sY) {
      setColor(r, g, b);
      EsploraTFT.rect(sX, sY, 12, (int)((float)12 / (float)max_hp * (float)hp));
    }
    void movx(int dir, int down) {
      if (!dead)
        del();
      aX += dir;
      aY += down;
      if (!dead)
        draw(aX, aY);
    }


    void setColor(byte rt, byte gt, byte bt) {
      r = rt;
      g = gt;
      b = bt;
      EsploraTFT.fill(r, g, b);
      EsploraTFT.stroke(r, g, b);
    }

    void setPresetColor(int col) {
      switch (col) {
        case 0:
          r = 255;
          g = 0;
          b = 0;
          break;
        case 1:
          r = 0;
          g = 255;
          b = 0;
          break;
        case 2:
          r = 0;
          g = 0;
          b = 255;
          break;
        case 3:
          r = 255;
          g = 255;
          b = 0;
          break;
        case 4:
          r = 0;
          g = 255;
          b = 255;
          break;
        case 5:
          r = 255;
          g = 255;
          b = 255;
          break;

        case 6:
          r = 0;
          g = 191;
          b = 255;
          break;
      }

    }
};

class Tank {
  public:
    int hp = 3;
    bool shooting = false;
    int bulletX, bulletY;
    int bulletWidth = 2;
    int posX;
    int posY = 110;
    bool soundEnabled;

    Tank(bool sE,int h) {
      soundEnabled = sE;
      hp = h;
    }

    void create(int sX) {
      posX = sX;
      draw(posX);
    }
    void mov(int dir) {
        del(posX);
        if (posX + dir >= 0 && dir < 0 || posX + dir <= 145 && dir > 0)
          posX += dir;
        draw(posX);
    }

    void del(int sX) {
      EsploraTFT.stroke(0, 0, 0);
      EsploraTFT.fill(0, 0, 0);

      EsploraTFT.rect(sX + 6, posY, 3, 2);
      EsploraTFT.rect(sX, posY + 2, 15, 5);
    }
    void shoot() {
      bulletX = posX + 7;
      bulletY = posY - 5;
      shooting = true;
      drawBullet();

    }

    void stopBullet() {
      shooting = false;
      removeBullet();
    }

    void moveBullet() {
      if (bulletY <= 0) {
        stopBullet();
      } else {
        removeBullet();
        bulletY -= 3;
        drawBullet();
      }


    }
    void drawHp() {
      EsploraTFT.stroke(0, 0, 0);
      EsploraTFT.fill(0, 0, 0);
      EsploraTFT.rect(140, 120, 20, 8);
      char sensorPrintout[2];
      String(hp).toCharArray(sensorPrintout, 2);
      EsploraTFT.setTextSize(1);
      EsploraTFT.stroke(0, 255, 0);
      EsploraTFT.text(sensorPrintout, 140, 120);
      EsploraTFT.stroke(255, 255, 255);
      EsploraTFT.text("hp", 147, 120);

    }

    void removeBullet() {
      EsploraTFT.stroke(0, 0, 0);
      EsploraTFT.fill(0, 0, 0);
      EsploraTFT.rect(bulletX, bulletY, bulletWidth, 5);
    }

    void drawBullet() {
      EsploraTFT.stroke(255, 255, 0);
      EsploraTFT.fill(255, 255, 0);
      EsploraTFT.rect(bulletX, bulletY, bulletWidth, 5);
    }


  private:


    void draw(int sX) {
      EsploraTFT.stroke(0, 240, 0);
      EsploraTFT.fill(0, 240, 0);
      EsploraTFT.rect(sX + 6, posY, 3, 2);
      EsploraTFT.rect(sX, posY + 2, 15, 5);

    }
};
