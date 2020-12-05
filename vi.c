#include <stdio.h>
#include <string.h>
#include <curses.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define LEFT 75 
#define RIGHT 77
#define UP 72
#define DOWN 80

#define LEFT_2 4 
#define RIGHT_2 5
#define UP_2 3
#define DOWN_2 2
#define KEY_ESC 27
#define KEY_HOME_ 72
#define KEY_END_ 71
#define INSERT_MODE 105

#define MODE_1 "COMMAND_MODE"
#define MODE_2 "--INSERT--"

struct line_tag {
	int line_number;
	int line_count;
	struct line_tag *down;
	struct line_tag *up;
	struct char_tag *char_point;
};

struct char_tag {
	char character;
	int char_number;
	struct char_tag *prev;
	struct char_tag *next;
};

struct line_tag* init_line;
struct char_tag* init_char;

int x, y;
int Lx,Ly;
int Lb, LmB;
int MODE;
char buffer[20000];
int Linebreak;
int maxLinebreak;

void Lineposition(int a,int b, int c, int d) {
	Lx = a;
	Ly = b;
	Lb = c;
	LmB = d;
}

void add_str(char c, struct char_tag* currchar, struct line_tag* currline ) {
	if (c == '\0')
		return;
	struct char_tag* newchar = malloc(sizeof(struct char_tag));
	newchar->character = c;
	newchar->char_number = 1;
	newchar->prev = currchar->prev;
	newchar->next = currchar;
	if (currchar->prev != NULL) 
		currchar->prev->next = newchar;
	currchar->prev = newchar;
	if (currline->char_point == currchar)
		currline->char_point = newchar;
	++(currline->line_count);
}

void screen_refresh(struct char_tag* currchar, struct line_tag* currline) {
	clear();
	int i = 0, j = 0;
	move(i, j);
	while (currline) {
		currchar = currline->char_point;
		while(currchar) {
			if (currchar->character != '\0') {
				if (j >= COLS - 1) {
					++i;
					j = 0;
					if (i > LINES - 2)
						break;
				}
				move(i, j++);
				addch(currchar->character);
			}
			currchar = currchar->next;
		}
		currline = currline->down;
		++i;
		j = 0;
		if (i > LINES - 2)
			break;
	}
	for (int k = i + 1; k < LINES - 2; k++) {
		move(k, 0);
		addch('~');
	}
	move(LINES - 1, 0);
	if (MODE == 2) {
		addstr(MODE_2);
	}
	else if (MODE == 1) {
		if (buffer[0] == ':') {
			addstr(buffer);
		}
	}
}

int main(int argc, char* argv[]) {

	init_line = malloc(sizeof(struct line_tag));
	init_char = malloc(sizeof(struct char_tag));
	init_line->line_number = 1;
	init_line->line_count = 0;
	init_line->down = NULL;
	init_line->up = NULL;
	init_line->char_point = init_char;
	init_char->character = '\0';
	init_char->char_number = 1;
	init_char->prev = NULL;
	init_char->next = NULL;

	Linebreak = 0;

	if (argc != 2) {	
		perror("# of argument is wrong: ");
		exit(1);
	}
	char* FILENAME = argv[1];
	int fd, fd2=0;
	int fexist;
	if (access(FILENAME, F_OK) != -1) {
		fexist = 1;
	}
	else {
		fexist = 0;
	}
	if ((fd = open(FILENAME, O_RDONLY | O_CREAT)) == -1) {
		perror("Can't open file: ");
		exit(1);
	}
	int len = 0;
	
	struct char_tag* currchar = init_char;
	struct line_tag* currline = init_line;
	buffer[19999] = '\0';
	while (read(fd, buffer, 19999) != 0) {
		len = strlen(buffer);
		if (len < 19999) buffer[len--] = '\0';
		for (int i = 0; i <= len; i++) {
			if (buffer[i] == '\0')
				continue;
			struct char_tag* newchar = malloc(sizeof(struct char_tag));
			newchar->character = buffer[i];
			newchar->char_number = 1;
			newchar->prev = currchar->prev;
			newchar->next = currchar;
			if (currchar == init_char) {
				init_line->char_point = newchar;
				init_char = newchar;
			}
			if (currchar->prev != NULL) 
				currchar->prev->next = newchar;
			currchar->prev = newchar;
			if (currline->char_point == currchar)
				currline->char_point = newchar;
			++(currline->line_count);
			if (buffer[i] == '\n') {
				struct line_tag* newline = malloc(sizeof(struct line_tag));
				struct char_tag* newchar = malloc(sizeof(struct char_tag));
				newline->line_number = 1;
				newline->line_count = 0;
				newline->down = currline->down;
				newline->up = currline;
				if (currline == init_line) init_line->down = newline;
				newline->char_point = newchar;
				newchar->character = '\0';
				newchar->char_number = 1;
				newchar->prev = NULL;
				newchar->next = NULL;
				currline->down = newline;
				currchar = newchar;
				currline = newline;
			}
		}
		memset(buffer, 0, sizeof(buffer - 1));
	}

	close(fd);

	char c;
	initscr();
	crmode();
	keypad(stdscr, TRUE);
	noecho();
	clear();
	init_char = init_line->char_point;
	screen_refresh(init_char, init_line);
	x = 0;
	y = 0;
	move(x, y);
	refresh();


	struct char_tag* print_char = init_char;
	struct line_tag* print_line = init_line;
	struct line_tag* Linebreak_line;
	currline = init_line;
	currchar = init_char;
	MODE = 1;
	memset(buffer, 0, sizeof(buffer));
	int idx = 0;
	while(1) {
		while(1) {
			c = getch();
			if ((int)c <= 5 && (int)c >= 2) { //direction key
				switch(c) {
					case LEFT_2:
						if (currline->line_count >= 2 && currline->down != NULL && y >= currline->line_count - 1) {
							y = currline->line_count - 2;
							currchar = currchar->prev;
						}
						else if (currline->line_count >= 2 && currline->down == NULL && y >= currline->line_count) {
							y = currline->line_count - 1;
							currchar = currchar->prev;
						}
						else if (currline->line_count == 0 || (currline->line_count == 1 && currchar->character == '\n'))
							break;
						else if (currline->line_count == 1) {
							y = 0;
							currchar = currline->char_point;
						}
						else if (y > 0) {
							--y;
							currchar = currchar->prev;
						}
						else if(y == 0 && currchar->prev != NULL) {    // it is core dump
							y = COLS - 2;
							--x;
							currchar = currchar->prev;
						}
						break;
					case RIGHT_2:
						if (y < COLS - 1 && currchar->character != '\0' && currchar->character != '\n') {
							++y;
							currchar = currchar->next;
						}
						break;
					case UP_2:
						if (x > 0 && currline->up != NULL) {
							--x;
							currline = currline->up;
							currchar = currline->char_point;
							for (int i = 0; i < y; i++) {
								if (currchar->character == '\0' || currchar->character == '\n' || currchar->next == NULL)
									break;
								currchar = currchar->next;
							}
						}
						else {
							if (currline->up != NULL) {
								print_line = print_line->up;
								print_char = print_line->char_point;
								currline = currline->up;
								currchar = currline->char_point;
							}
						}
						break;
					case DOWN_2:
						if (x < LINES - 2 && currline->down != NULL) {
							++x;
							currline = currline->down;
							currchar = currline->char_point;
							for (int i = 0; i < y; i++) {
								if (currchar->character == '\0' || currchar->character == '\n' || currchar->next == NULL)
									break;
								currchar = currchar->next;
							}
						}
						else {
							if (currline->down != NULL) {
								print_line = print_line->down;
								print_char = print_line->char_point;
								currline = currline->down;
								currchar = currline->char_point;
							}
						}
						break;
				}
			}
			else if (c == 'i') { //INSERT_MODE
				MODE = 2;
				break;
			}
			else if (c == ':') {
				buffer[idx++] = ':';
				buffer[idx] = '\0';
				screen_refresh(print_char, print_line);
				refresh();
				while (c = getch()) {
					if (c >= 33 && c <= 126) { //key
						buffer[idx++] = c;
						buffer[idx] = '\0';
					}
					else if (c == (char)7) { //backspace
						if (idx >= 2)
						       	buffer[--idx] = '\0';
						else {
							idx = 0;
							memset(buffer, 0, sizeof(buffer));
							break;
						}
					}
					else if (c == (char)32) {	//space
						buffer[idx++] = ' ';
						buffer[idx] = '\0';
					}
					else if (c == (char)10) {
						len = strlen(buffer);
						for (int i = 0; i < len; i++) {
							if (buffer[i] == 'q') {
								if (buffer[i + 2] == '-' && buffer[i + 3] == 'd' && buffer[i + 4] == 'u' ) {
									char TESTFILE[500] = "/bin/cp ";
									char myfile[500] = "";
									int cnt = 0;
									for (int j = i + 6; j < len; j++) {
										myfile[cnt++] = (char)buffer[j];
									}
									strcat(TESTFILE, FILENAME);
									strcat(TESTFILE, " ");
									strcat(TESTFILE, myfile);
									system(TESTFILE);
									endwin();
									return 0;
								}
								if (buffer[i + 2] == '-' && buffer[i+3] == 't') {
									char TESTFILE[500] = "/bin/cp ";
									strcat(TESTFILE, FILENAME);
									strcat(TESTFILE, " test.");
									strcat(TESTFILE, FILENAME);
									system(TESTFILE);
									endwin();
									return 0;
								}
								else if (buffer[i + 2] == '-' && buffer[i + 3] == 'd' && buffer[i+4] == 'i' && buffer[i + 5] == 'r') {
									char exedir[500] = "mkdir ";
									char dir[500] = "";
									int cnt = 0;
									for (int j = i + 7; j < len; j++) {
										dir[cnt++]=(char)buffer[j];
									}
									strcat(exedir, dir);
									system(exedir);

									char execp[500] = "/bin/cp ";
									strcat(execp, FILENAME);
									strcat(execp, " ");
									strcat(execp, dir);
									strcat(execp, "/");
									strcat(execp, FILENAME);
									system(execp);
									endwin();
									return 0;
								}
								else if (buffer[i + 1] == '!') {
									if (fexist == 0) {
										unlink(FILENAME);
										close(fd);
									}
									endwin();
									return 0;
								}
								else {
									endwin();
									return 0;
								}
							}
							else if (buffer[i] == 'w') {
								if ((fd = open(FILENAME, O_CREAT | O_WRONLY | O_TRUNC, 0744)) == -1) {
									perror("Can't open file: ");
									exit(1);
								}
								if (lseek(fd, 0, SEEK_SET) == -1) {
									perror("Can't lseek: ");
									exit(1);
								}
								char newbuf[2000];
								newbuf[1999] = '\0';
								struct line_tag* newline = init_line;
								struct char_tag* newchar = init_char;
								
								while (1) {
									int i = 0;
									memset(newbuf, 0, sizeof(newbuf));
									for (; i < 1999;) {
										if (newchar == NULL || newchar->character == '\0') {
											if (newline->down == NULL)
												break;
											newline = newline->down;
											newchar = newline->char_point;
										}
										if (newchar->character != '\0') {
											newbuf[i++] = newchar->character;
											newchar = newchar->next;
										}
									}
									if (i == 0) break;
									newbuf[i] = '\0';
									if (write(fd, newbuf, i) == -1) {
										perror("Can't write: ");
										exit(1);
									}
								}
								close(fd);
							}
						}

						idx = 0;
						memset(buffer, 0, sizeof(buffer));
						break;
					}
					else if (c == KEY_ESC) {
						c = getch();
						if (c == (char)10) {
							idx = 0;
							memset(buffer, 0, sizeof(buffer));
							break;
						}
					}
					screen_refresh(print_char, print_line);
					refresh();
				}
			}
			else if (c == 'q') break;
			else if (c == '[') {
				y = 0;
				currchar = currline->char_point;
			}
			else if (c == ']') {
			while (currchar->character != '\n' && currchar->character != '\0') {
				++y;
				currchar = currchar->next;
			}
			}
			else if (c == KEY_HOME_) {	// home 'H'
			while (x > 0 && currline->up != NULL) {
					--x;
					currline = currline->up;
					currchar = currline->char_point;
			}
				y = 0;
				currchar = currline->char_point;
			}
			else if (c == KEY_END_) {	// end 'G'
			while (x < LINES - 2 && currline->down != NULL) {
				++x;
				currline = currline->down;
				currchar = currline->char_point;
			}
			while (currchar->character != '\n' && currchar->character != '\0') {
				++y;
				currchar = currchar->next;
			}
			}
			else if (c == '-') {	// 맨 윗줄로 이동
			while (x > 0 && currline->up != NULL) {
				--x;
				currline = currline->up;
				currchar = currline->char_point;
				for (int i = 0; i < y; i++) {
					if (currchar->character == '\0' || currchar->character == '\n' || currchar->next == NULL)
						break;
					currchar = currchar->next;
				}
			}
			}
			else if (c == '=') {	// 맨 아래줄로 이동
			while (x < LINES - 2 && currline->down != NULL) {
				++x;
				currline = currline->down;
				currchar = currline->char_point;
				for (int i = 0; i < y; i++) {
					if (currchar->character == '\0' || currchar->character == '\n' || currchar->next == NULL)
						break;
					currchar = currchar->next;
				}
			}
			}
			screen_refresh(print_char, print_line);
			if (currline->down != NULL) move(x, y < currline->line_count - 1 ? y : currline->line_count - 1);
			else move(x, y < currline->line_count ? y : currline->line_count);
			refresh();
		}
		if (c == 'q') break;
		screen_refresh(print_char, print_line);
		if (currline->down != NULL) move(x, y < currline->line_count - 1 ? y : currline->line_count - 1);
		else move(x, y < currline->line_count ? y : currline->line_count);
		refresh();

		while(1) {
			c = getch();
			if (c == (char)27) { //direction key
				MODE = 1;
				break;
			}
			else if ((int)c <= 5 && (int)c >= 2) {
				switch(c) {
					case LEFT_2:
						if (currline->line_count >= 2 && currline->down != NULL && y >= currline->line_count - 1) {
							y = currline->line_count - 2;
							currchar = currchar->prev;
						}
						else if (currline->line_count >= 2 && currline->down == NULL && y >= currline->line_count) {
							y = currline->line_count - 1;
							currchar = currchar->prev;
						}
						else if (currline->line_count == 0 || (currline->line_count == 1 && currchar->character == '\n'))
							break;
						else if (currline->line_count == 1) {
							y = 0;
							currchar = currline->char_point;
						}
						else if (y > 0) {
							--y;
							currchar = currchar->prev;
						}
						else if(y == 0 && currchar->prev != NULL && Linebreak) {
							Lineposition(x, y, Linebreak, maxLinebreak);
							y = COLS - 2;
							--x;
							currchar = currchar->prev;
							Linebreak--;
						}
						break;
					case RIGHT_2:
						if (y < COLS - 1 && currchar->character != '\0' && currchar->character != '\n') {
							++y;
							currchar = currchar->next;
						}
						if (y == COLS - 1 && currchar->character != '\0' && currchar->character != '\n') {
							Lineposition(x, y, Linebreak, maxLinebreak);
							++x;
							y = 0;
							currchar = currchar->next;
							Linebreak++;
						}
						break;
					case UP_2:
						if (Linebreak) {					//////////////////////////////
							Lineposition(x, y, Linebreak, maxLinebreak);
							while(Linebreak>0) {
								--x;
								currline = currline->up;
								currchar = currline->char_point;
								y = 0;
								currchar = currchar->next;
								Linebreak--;
							}
							Linebreak = 0;
							maxLinebreak = 0;
						}
						else if(!Linebreak) {
							Linebreak = 0;
							maxLinebreak = 0;
							if (x > 0 && currline->up != NULL) {
								--x;
								currline = currline->up;
								currchar = currline->char_point;
								for (int i = 0; i < y; i++) {
									if (currchar->character == '\0' || currchar->character == '\n' || currchar->next == NULL)
										break;
									currchar = currchar->next;
								}
							}
							else {
								if (currline->up != NULL) {
									print_line = print_line->up;
									print_char = print_line->char_point;
									currline = currline->up;
									currchar = currline->char_point;
								}
							}
						}
						break;
					case DOWN_2:
						if (Linebreak != maxLinebreak) {					//////////////////////////////
							Lineposition(x, y, Linebreak, maxLinebreak);
							for (int i = maxLinebreak - Linebreak; i >= 0; i--) {
								++x;
								currline = currline->down;
								currchar = currline->char_point;
								y = 0;
								currchar = currchar->next;
								}
							Linebreak = 0;
							maxLinebreak = 0;
						}
						else if (maxLinebreak == Linebreak) {
							Linebreak = 0;
							maxLinebreak = 0;
							if (x + 1 >= Lx - Lb && x + 1 <= Lx + LmB - Lb) {
								while (x <= Lx) {
									++x;
									currline = currline->down;
									currchar = currline->char_point;
								}
								y = Ly;
								currchar = currline->char_point;
							}
							else if (x >= Lx - Lb && x <= Lx + LmB - Lb) {
								while (x <= Lx+LmB-Lb) {
									++x;
									currline = currline->down;
									currchar = currline->char_point;
								}
								y = 0;
								currchar = currline->char_point;
							}
							else {
								if (x < LINES - 2 && currline->down != NULL) {
									++x;
									currline = currline->down;
									currchar = currline->char_point;
									for (int i = 0; i < y; i++) {
										if (currchar->character == '\0' || currchar->character == '\n' || currchar->next == NULL)
											break;
										currchar = currchar->next;
									}
								}
								else {
									if (currline->down != NULL) {
										print_line = print_line->down;
										print_char = print_line->char_point;
										currline = currline->down;
										currchar = currline->char_point;
									}
								}
							}
						}
						break;
				}
			}
			else if (c >= 33 && c <= 126) {
				add_str(c, currchar, currline);
				if (currline == init_line && currchar == init_char) {
					init_char = currchar->prev;
					init_line->char_point = init_char;
				}
				if (y < COLS - 1)
					++y;
				else {
					++x;
					y = 0;
					Linebreak++;
					maxLinebreak = Linebreak;
				}
			}
			else if (c == (char)32) { //space
				add_str(' ', currchar, currline);
				if (currline == init_line && currchar == init_char) {
					init_char = currchar->prev;
					init_line->char_point = init_char;
				}
				if (y < COLS - 1)
					++y;
				else {
					++x;
					y = 0;
				}
			}	
			else if (c == (char)7) { //backspace
				if (currchar->prev != NULL) {
					if (currchar->prev->prev != NULL)
						currchar->prev->prev->next = currchar;
					struct char_tag* freechar = currchar->prev;
					currchar->prev = currchar->prev->prev;
					currline->line_count -= 1;
					if (currline->char_point == freechar) currline->char_point = currchar;
					free(freechar);
					if (y != 0) y -= 1;
					else y = COLS - 1;
				}
				else if (currline->up != NULL) {
					--x;
					y = currline->up->line_count - 1;
					struct char_tag* findchar = currline->up->char_point;
					while (findchar) {
						if (findchar->character == '\n')
							break;
						findchar = findchar->next;
					}
					struct line_tag* freeline = currline;
					currline = currline->up;
					currline->down = currline->down->down;
					if (currline->down != NULL)
						currline->down->up = currline;
					currchar->prev = findchar->prev;
					if (currchar->prev != NULL)
						currchar->prev->next = currchar;
					currline->line_count += freeline->line_count - 1;
					free(freeline);
					if (findchar == currline->char_point) currline->char_point = currchar;
					struct char_tag* freechar;
					while (findchar) {
						freechar = findchar;
						findchar = findchar->next;
						free(freechar);
					}
				}
			}
			else if (c == 10) { //enter
				add_str('\n', currchar, currline);
				
				struct line_tag* newline = malloc(sizeof(struct line_tag));
				struct char_tag* newchar = malloc(sizeof(struct char_tag));
				newchar->character = '\0';
				newchar->char_number = 1;
				newchar->prev = currchar->prev;
				newchar->next = NULL;

				currchar->prev->next = newchar;
				currchar->prev = NULL;
				
				struct char_tag* countchar = currchar;
				int cnt = 0;
				while(countchar) {
					if (countchar->character == '\0')
						break;
					cnt++;
					countchar = countchar->next;
				}
				currline->line_count -= cnt;
				newline->line_number = 1;
				newline->line_count = cnt;
				newline->down = currline->down;
				if (currline->down != NULL)
					currline->down->up = newline;
				if (currline == init_line) init_line->down = newline;
				newline->up = currline;
				newline->char_point = currchar;
				currline->down = newline;

				currline = currline->down;
				if (x == LINES - 2) {
					print_line = print_line->down;
					print_char = print_line->char_point;
				}
				else
					++x;
				y = 0;
			}
			screen_refresh(print_char, print_line);
			if (currline->down != NULL) move(x, y < currline->line_count - 1 ? y : currline->line_count - 1);
			else move(x, y < currline->line_count ? y : currline->line_count);
			refresh();

		}
		screen_refresh(print_char, print_line);
		if (currline->down != NULL) move(x, y < currline->line_count - 1 ? y : currline->line_count - 1);
		else move(x, y < currline->line_count ? y : currline->line_count);
		refresh();
	}



	endwin();
}
