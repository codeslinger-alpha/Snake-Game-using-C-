
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#define bgcolor 1
#define ROW 15
#define COL 30
int wait_time_ms = 100;
int border_wall;
const char snake_body = '=', point = 'o', OBSTACLE = '#'; // Visuals


char *OBS[ROW * COL];

char area[ROW][COL];
char prev = 'd'; // primary direction of movement
char *snake[ROW * COL];
int pointr = -1, pointc = -1;
int snakelen = 1;
int num_of_obs = 0;
int getch();
void set_input_mode(int enabled);
int kbhit();
void obstacle(int obs) {
  if (!obs)
    return;
  printf("Percentage of obstacle(1-50%%): ");
  int per;
  scanf("%d", &per);

  int min = 0, maxR = ROW - 1, maxC = COL - 1, r, c, i = 0;
  for (; i < (per / 100.0) * ROW * COL; i++) {
  generate_obs:
    r = min + rand() % (maxR + 1), c = min + rand() % (maxC + 1);
    if (r == 0 && c == 0)
      goto generate_obs;
    if (area[r][c] == OBSTACLE)
      goto generate_obs;
    OBS[i] = &area[r][c];
  }
  num_of_obs = i;
}
void spawn() {
  if (pointr == -1) {
    int min = 0, maxR = ROW - 1, maxC = COL - 1, r, c;
  generate:
    r = min + rand() % (maxR + 1), c = min + rand() % (maxC + 1);

    for (int i = 0; i < snakelen; i++) {
      if (snake[i] == &area[r][c])
        goto generate;
    }
    for (int i = 0; i < num_of_obs; i++) {
      if (OBS[i] == &area[r][c])
        goto generate;
    }
    pointr = r;
    pointc = c;
  }
  area[pointr][pointc] = point;
}

char wait(int ms) {
  if (ms == -1) {
    return getch();
  }
  int elps = 0;
  while (elps < ms) {
    if (kbhit())
      return getch();
    else
      usleep(2000);
    elps += 2;
  }
  return 0;
}

char *dir() {
  char ch;
unpaused:
  ch = wait(wait_time_ms);

  if (ch == ' ') {
    while ((ch = wait(-1)) != ' ')
      ;
    goto unpaused;
  }
  if (!ch || !(ch == 'd' || ch == 'a' || ch == 'w' || ch == 's'))
    ch = prev;

chk:
  switch (ch) {
  case 'd': // right
    if (snakelen == 1 || (snake[0] + 1) != snake[1]) {
      snake[0]++;
      prev = ch;
      return snake[0] - 1;
    } else {
      ch = prev;
      goto chk;
    }
  case 'a': // left
    if (snakelen == 1 || snake[0] - 1 != snake[1]) {
      snake[0]--;
      prev = ch;
      return snake[0] + 1;
    } else {
      ch = prev;
      goto chk;
    }

  case 'w': // up
    if (snakelen == 1 || snake[0] - COL != snake[1]) {
      snake[0] -= COL;
      prev = ch;
      return snake[0] + COL;
    } else {
      ch = prev;
      goto chk;
    }

  case 's': // down
    if (snakelen == 1 || snake[0] + COL != snake[1]) {
      snake[0] += COL;
      prev = ch;
      return snake[0] - COL;
    } else {
      ch = prev;
      goto chk;
    }
  }
}

void clear() {
  char buff[(sizeof("\033[A\033[2K") + 5) * (ROW + 5)];
  int size_of_buff = 0;

  for (int i = 1; i <= ROW + 1 + border_wall; i++) {
    size_of_buff +=
        sprintf(buff + size_of_buff, "\033[A"); // move cursor up by 1 row
    size_of_buff += printf(buff + size_of_buff, "\033[2K"); // clear entire row
  }
  printf("%s", buff);
  memset(area, ' ', sizeof(area));
}

int check(char *past, int obs) {
  // Border checking
  if (border_wall) {
    int row = (snake[0] - &area[0][0]) / COL,
        col = (snake[0] - &area[0][0]) % COL;
    int p_col = (past - &area[0][0]) % COL;
    if (p_col == COL - 1 && col == 0)
      return -1;
    else if (p_col == 0 && (col < 0 || col == COL - 1))
      return -1;

    else if (snake[0] < &area[0][0])
      return -1;
    else if (row >= ROW)
      return -1;

  } else {
    int row = (snake[0] - &area[0][0]) / COL,
        col = (snake[0] - &area[0][0]) % COL;
    int p_col = (past - &area[0][0]) % COL;
    if (p_col == COL - 1 && col == 0)
      snake[0] -= COL;
    else if (p_col == 0 && (col < 0 || col == COL - 1))
      snake[0] += COL;
    else {
      if (snake[0] < &area[0][0])
        row = ROW;
      else if (row >= ROW)
        row = 0;

      snake[0] = &area[row][col];
    }
  }
  for (int i = 2; i < snakelen; i++) {
    if (snake[0] == snake[i])
      return -1;
  }
  for (int i = 0; i < num_of_obs; i++) {
    if (OBS[i] == snake[0])
      return -1;
  }
  if (snake[0] == &area[pointr][pointc]) {
    printf("\a");
    return 1;
  }
  return 0;
}

void move(int eaten, char *past) {
  int i = 1;
  // head design
  if (prev == 'd')
    (*snake[0]) = '>';
  else if (prev == 'a')
    (*snake[0]) = '<';
  else if (prev == 'w')
    (*snake[0]) = '^';
  else
    (*snake[0]) = '+';

  for (; i < snakelen; i++) {
    char *temp = snake[i];

    snake[i] = past;
    (*snake[i]) = snake_body;
    past = temp;
  }
  if (snakelen > 1)
    (*snake[i - 1]) = '*'; // tail design
  if (eaten) {
    ++snakelen;
    pointr = -1;
    pointc = -1;
  }
}

void print() {
  int obstacles = 0;

  for (int r = 0; r < ROW; r++) {
    if (border_wall)
      printf("|");
    for (int c = 0; c < COL; c++) {
      if (bgcolor)
        printf("\033[48;5;155m");

      for (int i = 0; i < num_of_obs; i++) {
        if (OBS[i] == &area[r][c]) {

          printf("\033[38;5;196m%c", OBSTACLE);
          obstacles = 1;
          break;
        }
      }

      if (obstacles) {
        obstacles = 0;

        printf("\033[0m");
        continue;
      }
      printf("\033[30m%c", area[r][c]);

      printf("\033[0m");
    }
    if (border_wall)
      printf("|");
    printf("\n");
  }

  printf("\033[0m");
  if (border_wall) {
    for (int i = 0; i < COL + 2; i++)
      printf("|");
    puts("");
  }
  printf("Score: %d\n", snakelen - 1);
}

int main() {
start:
  snake[0] = &area[0][0];
  memset(area, ' ', sizeof(area));
  printf("Level(1-10): ");

  unsigned short int level;
  scanf("%hu", &level);
  if (level > 10)
    level = 10;
  wait_time_ms *= (11 - level);
  printf("Do you want obstacles?(1/0): ");
  short int obs;

  scanf("%hd", &obs);
  obstacle(obs);
  printf("Enable border wall(1/0): ");
  scanf("%d", &border_wall);

  printf("\033[?25l"); // hide cursor
  fflush(stdout);
  printf("Use w/a/s/d for controls. Press <space> to pause/unpause\n");
  usleep(1000000);
  set_input_mode(1);
  srand(time(NULL));
  if (border_wall) {
    border_wall = 1;
    for (int i = 0; i < COL + 2; i++)
      printf("|");
    puts("");
  }

  while (1) {

    print();
    char *past = dir(); // takes time
    clear();
    spawn();
    int c = check(past, obs), eaten = 0;
    if (c == -1)
      break;
    else if (c == 1)
      eaten = 1;
    move(eaten, past);
  }

  set_input_mode(0);

  printf("\a\a\a");
  usleep(1000000);
  printf("\a\a\a");
  usleep(1000000);
  printf("\a\a\a");

  system("clear");
  printf("\033[1mGAME OVER\033[0m\n");

  usleep(2000000);

  printf("\nScore: %d\n", snakelen - 1);
  usleep(500000);
  printf("\033[?25h"); // show cursor

  return 0;
}
void set_input_mode(int enabled) {
  static struct termios oldt, newt;
  if (enabled) {
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  } else {
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  }
}

int getch() {
  int ch;
  ch = getchar();
  return ch;
}
int kbhit() {
  struct timeval tv = {0L, 0L};
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(STDIN_FILENO, &fds);
  return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
}