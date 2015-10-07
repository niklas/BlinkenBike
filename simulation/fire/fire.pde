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

class LED {
  int index;
  int r,g,b;

  LED(int newIndex) {
    index = newIndex;
    r = int(random(10, 255));
    g = int(random(10, 255));
    b = int(random(10, 255));
  }

  void tick(int t) {
    r = t % 255;
    g = index;
  }

  void display() {
    fill(r,g,b);
    ellipse(0,0, LED_SIZE,LED_SIZE);
  }
}

// vi:syntax=cpp
