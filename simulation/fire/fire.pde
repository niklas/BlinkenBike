int numLEDs = 50;
int LED_SIZE = 20;
int LED_SPACING = 15;

LED [] leds;

int frameCounter = 0;


int STRIP_WIDTH = (numLEDs * LED_SIZE) + ( (numLEDs-1) * LED_SPACING);

void setup() {
  fullScreen();
  frameRate(30);

  leds = new LED[numLEDs];

  for (int i=0; i < numLEDs; i++) {
    leds[i] = new LED(i);
  }
}

void draw() {
  background(0,0,0);

  pushMatrix();
  translate(width/2,height/2);
  translate(-STRIP_WIDTH/2, 0);
  for (int i=0; i < numLEDs; i++) {
    leds[i].tick(frameCounter);
    leds[i].display();
    translate(LED_SIZE + LED_SPACING, 0);
  }
  popMatrix();
  frameCounter++;
}

color yellow = color(255, 230, 0);
color red    = color(255, 0, 0);
color black  = color(0, 0, 0);

class LED {
  int index;
  color col;

  LED(int newIndex) {
    index = newIndex;
    col = black;
  }

  void tick(int t) {
    int length = numLEDs / 2;
    if (index < length) {
      color c = lerpColor( yellow, red, random(1));
      float brightness = map(index, 0, length, 0, 1);
      col = lerpColor( c, black, brightness);
    } else {
      col = black;
    }
  }

  void display() {
    stroke(23,23,23);
    strokeWeight(2);
    fill(col);
    ellipse(0,0, LED_SIZE,LED_SIZE);
  }
}

// vi:syntax=cpp
