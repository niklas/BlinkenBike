int numLEDs = 50;
int numParticles = 40;
int LED_SIZE = 20;
int LED_SPACING = 15;

LED [] leds;
Particle [] particles;

int frameCounter = 0;
color yellow = color(255, 230, 0);
color red    = color(255, 0, 0);
color black  = color(0, 0, 0);



int STRIP_WIDTH = (numLEDs * LED_SIZE) + ( (numLEDs-1) * LED_SPACING);

void setup() {
  int i;
  fullScreen();
  frameRate(30);

  leds = new LED[numLEDs];
  particles = new Particle[numParticles];

  for (i=0; i < numLEDs; i++) {
    leds[i] = new LED(i);
  }

  for (i=0; i < numParticles; i++) {
    particles[i] = new Particle( int( random(numLEDs/4, numLEDs/2)) );
  }
}

void draw() {
  int i;
  int pos;
  Particle p;
  background(0,0,0);
  color col;

  for (i=0; i < numLEDs; i++) {
    leds[i].col = black;
  }

  for (i=0; i < numParticles; i++) {
    p = particles[i];
    p.tick();
    pos = p.getPos();

    if (pos >= 0 && pos < numLEDs) {
      col = lerpColor(p.col, black, p.getBrightness());
      leds[pos].col = lerpColor(col, leds[pos].col, 0.5);
    }
  }

  pushMatrix();
  translate(width/2,height/2);
  translate(-STRIP_WIDTH/2, 0);
  for (i=0; i < numLEDs; i++) {
    leds[i].display();
    translate(LED_SIZE + LED_SPACING, 0);
    leds[i].col = black;
  }
  popMatrix();
  frameCounter++;
}

class Particle {
  color col;
  int pos, speed, max;
  int resolution;

  Particle(int newMax) {
    resolution = 100;
    max = newMax * resolution;
    col = lerpColor( yellow, red, random(1));
    pos = 0;
    speed = int( random(1, max/5) );
  }

  void tick() {
    pos += speed;
    if (pos > max) {
      pos = 0;
    }
  }

  int getPos() {
    return pos / resolution;
  }

  float getBrightness() {
    return float(pos)/max;
  }

}

class LED {
  int index;
  color col;

  LED(int newIndex) {
    index = newIndex;
    col = black;
  }

  void display() {
    stroke(23,23,23);
    strokeWeight(2);
    fill(col);
    ellipse(0,0, LED_SIZE,LED_SIZE);
  }
}

// vi:syntax=cpp
