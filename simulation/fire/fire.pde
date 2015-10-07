int numLEDs = 50;
int LED_SIZE = 10;
int LED_SPACING = 5;



int STRIP_WIDTH = (numLEDs * LED_SIZE) + ( (numLEDs-1) * LED_SPACING);

void setup() {
  fullScreen();
  frameRate(30);
}

void draw() {
  background(0,0,0);
  fill(42,42,166);

  pushMatrix();
  translate(width/2,height/2);
  translate(-STRIP_WIDTH/2, 0);
  for (int i=0; i < numLEDs; i++) {
    ellipse(0,0, LED_SIZE,LED_SIZE);
    translate(LED_SIZE + LED_SPACING, 0);
  }
  popMatrix();
}

// vi:syntax=cpp
