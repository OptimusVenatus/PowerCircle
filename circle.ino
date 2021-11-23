#include <Gamebuino-Meta.h>

//prepare saving
const SaveDefault defaults[] = {
    {0, SAVETYPE_INT, 0, 0},
};

//variable declarations

int state = 0;

int xsize;
int ysize;

float objective;
float position = 0;
float speed = 1;
int score = 0;
//way is the direction of the selector 
int way = 1;
int hb = 0;
int highscore = 0;

//function for resetting the game
void reset()
{
  objective = random(0, ((2 * PI) * 100)) / 100;
  speed = 1;
  score = 0;
  way = 1;
}

void setup()
{
  gb.begin();
  gb.save.config(defaults);
  gb.setScreenRotation(ROTATION_RIGHT);
  //initialize variables
  xsize = gb.display.width();
  ysize = gb.display.height();
  objective = random(0, ((2 * PI) * 100)) / 100;
  highscore = gb.save.get(0);
}

void loop()
{
  //this switch statement is for the different states of the game
  switch (state)
  {
  case 1:
    gameOver();
    break;
  case 0:
    game();
    break;
  default:
    break;
  }
}

//Game over screen
void gameOver()
{
  while (!gb.update())
    ;
  gb.display.clear();

  //This Game over screen is similar to the game over screen of my other game HexHell

  //Write everything to the screen
  gb.display.setColor(BEIGE);
  gb.display.setFontSize(1.5);
  String text = "GAME OVER";
  //theses weird numbers are to center the text
  gb.display.setCursor((xsize / 2) - (gb.display.getFontWidth() * text.length() / 2), ysize / 2 - 20);
  gb.display.print(text);

  gb.display.setColor(RED);
  text = "press A";
  gb.display.setFontSize(0.2);
  gb.display.setCursor((xsize / 2) - ((gb.display.getFontWidth()) * text.length() / 2), ysize - gb.display.getFontHeight());
  gb.display.print(text);

  gb.display.setColor(GRAY);
  //if the highscore is beaten, score color is green
  if (hb == 1)
  {
    gb.display.setColor(GREEN);
  }
  text = "Score :" + String(score);
  gb.display.setFontSize(0.3);
  gb.display.setCursor((xsize / 2) - (gb.display.getFontWidth() * text.length() / 2), ysize / 2);
  gb.display.print(text);

  text = "Best Score :" + String(highscore);
  gb.display.setFontSize(0.2);
  gb.display.setCursor((xsize / 2) - (gb.display.getFontWidth() * text.length() / 2), ysize / 2 + 20);
  gb.display.print(text);

//Restart the game if A is pressed
  if (gb.buttons.pressed(BUTTON_A))
  {
    state = 0;
    gb.sound.playOK();
    reset();
  }
}

//Game screen
void game()
{
  while (!gb.update())
    ;
  gb.display.clear();
  //create BEIGE circle at the center of the screen
  position = position + (0.05 * speed) * way;
  gb.display.setColor(BEIGE);
  gb.display.fillCircle(xsize / 2, ysize / 2, xsize * 0.5);
  //create BLACK circle at the center of the screen for creating a fake hollow effect to the first circle
  gb.display.setColor(BLACK);
  gb.display.fillCircle(xsize / 2, ysize / 2, xsize * 0.25);

  //Draw the target circle at the objective
  gb.display.setColor(RED);
  gb.display.fillCircle(xsize / 2 + cos(objective) * xsize * 0.375, ysize / 2 + sin(objective) * xsize * 0.375, xsize * 0.125);

  if (gb.buttons.pressed(BUTTON_A))
  {
    //check if the selector is on the target circle
    if (gb.display.getPixelColor(xsize / 2 + cos(position) * xsize * 0.375, ysize / 2 + sin(position) * xsize * 0.375) == RED)
    {
      //if it is, increase the score and speed , replace the target circle and reverse the way
      score++;
      objective = random(0, ((2 * PI) * 300)) / 300;
      way = way * -1;
      gb.sound.playOK();
      speed = speed + 0.1;
    }
    else
    {
      //if not set the state to game over
      state = 1;
      hb = 0;
      //load the highscore
      highscore = gb.save.get(0);
      //if the score is higher than the highscore
      if (score > highscore)
      {
        //save the score ,show popup and set highscore beated to 1
        gb.save.set(0, score);
        highscore = score;
        gb.gui.popup("New Highscore!", 50);
        hb = 1;
      }
      gb.sound.playCancel();
    }
  }

  //show score at the center of the screen
  gb.display.setColor(GRAY);
  gb.display.setCursor(xsize / 2 - 5, ysize / 2 - 5);
  gb.display.print(score);

  //Draw the selector line at the position of the selector
  gb.display.setColor(BLUE);
  gb.display.drawLine(xsize / 2 + cos(position) * xsize * 0.25, ysize / 2 + sin(position) * xsize * 0.25, xsize / 2 + cos(position) * xsize * 0.5, ysize / 2 + sin(position) * xsize * 0.5);
}