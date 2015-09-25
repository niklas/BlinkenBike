#define MODE_NUM 4

enum {
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

  private:
    int
      mode,
      selectedMode;
};
