#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

// this macro returns the ASCII of the Ctrl + k key
#define CTRL_KEY(k) ((k) & 0x1f)

struct termios orig_termios;

void die(const char *s) {
  perror(s);
  exit(1);
}

void disableRawMode() {
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
    die("tcsetattr");
}

// this function does not prints whatever we are typing in the terminal
// We are just turning off the ECHO feature
void enableRawMode() {
  if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
    die("tcgetattr");
  atexit(disableRawMode);

  // here the orig_termios value is being modified by copying the value in raw.
  // Then we just & with the NOT of ECHO
  struct termios raw = orig_termios;
  raw.c_iflag &= ~(BRKINT | ICRNL | IXON | INPCK | ISTRIP);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag &= ~(CS8);
  raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
    die("tcsetattr");
}

char editorReadKey() {
  int nread;
  char c;
  while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
    if (nread == -1 && errno != EAGAIN)
      die("read");
  }
  return c;
}

void editorProcessKeypress() {
  char c = editorReadKey();

  // printf("%d (%c)\r\n",c,c);

  switch (c) {
  case CTRL_KEY('q'):
    exit(0);
    break;
  }
}

int main() {
  enableRawMode();

  while (1) {
    editorProcessKeypress();
  }

  return 0;
}
