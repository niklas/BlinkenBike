int numLEDs = 50;
int LED_SIZE = 10;
int LED_SPACING = 5;

LED [] leds;



int STRIP_WIDTH = (numLEDs * LED_SIZE) + ( (numLEDs-1) * LED_SPACING);

void setup() {
  fullScreen();
  frameRate(30);

  leds = new LED[numLEDs];

  for (int i=0; i < numLEDs; i++) {
    leds[i] = new LED();
  }
}

void draw() {
  background(0,0,0);

  pushMatrix();
  translate(width/2,height/2);
  translate(-STRIP_WIDTH/2, 0);
  for (int i=0; i < numLEDs; i++) {
    leds[i].display();
    translate(LED_SIZE + LED_SPACING, 0);
  }
  popMatrix();
}

class LED {
  int r,g,b;

  LED() {
    r = int(random(10, 255));
    g = int(random(10, 255));
    b = int(random(10, 255));
  }

  void display() {
    fill(r,g,b);
    ellipse(0,0, LED_SIZE,LED_SIZE);
  }
}

// vi:syntax=cpp
