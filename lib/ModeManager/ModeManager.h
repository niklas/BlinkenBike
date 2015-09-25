#define MODE_NUM 4

enum {
  ModeOff           = 0,
  ModeWandererBlue  = 1,
  ModeWandererGreen = 2,
  ModeWandererRed   = 3,
  ModeWandererWhite = 4
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
      getColor(LPD8806 leds, int bright);

  private:
    int
      mode,
      selectedMode;
};
