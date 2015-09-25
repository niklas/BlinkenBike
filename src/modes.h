#define MODE_NUM 4

void mode_select_next();
void mode_select_previous();
void mode_apply();

enum {
  ModeWandererBlue  = 1,
  ModeWandererGreen = 2,
  ModeWandererRed   = 3,
  ModeWandererWhite = 4
};

static int mode;
static int selectedMode;
