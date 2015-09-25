#define MODE_NUM 9

enum {
  ModeOff           = 0,
  ModeWandererBlue  = 1,
  ModeWandererGreen = 2,
  ModeWandererRed   = 3,
  ModeWandererPink  = 4,
  ModeWandererCyan  = 5,
  ModeWandererYellow  = 6,
  ModeWandererOrange  = 7,
  ModeWandererWhite = 8
};

class ModeManager {
  public:
    ModeManager();

    void
      selectNext(void),
      selectPrevious(void),
      apply(void);

    int
      getMode(void),
      getSelectedMode();

    uint32_t
      getColor(int m, LPD8806 leds, int bright);

  private:
    int
      mode,
      selectedMode;
};
