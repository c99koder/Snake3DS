/*	(C) 2003 Sam Steele
	This file is part of Snake.
	Snake is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	Snake is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
	GNU General Public License for more details.
	You should have received a copy of the GNU General Public License
	along with Snake.	 If not, see <http://www.gnu.org/licenses/>.
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>

#define BLACK 0
#define WHITE 15
#define YELLOW 11
#define RED 9
#define GREEN 10

#define BOARD_X 38
#define BOARD_Y 25

#define TOP 0xC4
#define BOT 0xC4
#define LEFT 0xB3
#define RIGHT 0xB3
#define TOP_LEFT 0xDA
#define TOP_RIGHT 0xBF
#define BOT_LEFT 0xC0
#define BOT_RIGHT 0xD9
#define SNAKE '*'
#define GEM 4

char board[BOARD_X][BOARD_Y];

signed char sx,sy,sdx,sdy,dead;
unsigned char ssize,sscore,slives;

PrintConsole *console;

void clrscr() {
	consoleClear();
}

void cputs(char *string) {
	printf("%s", string);
}

void gotoxy(int x, int y) {
	console->cursorX = x;
	console->cursorY = y;
}

void cputcxy(int x, int y, char c) {
	console->cursorX = x;
	console->cursorY = y;
	printf("%c", c);
}

void textcolor(int color) {
	console->fg = color;
}

void redraw() {
	char scorebuf[10],x,y;
	clrscr();
	textcolor(WHITE);
	cputs("Score: ");
	itoa(sscore, scorebuf, 10);
	cputs(scorebuf);
	gotoxy(BOARD_X - 6,0);
	if(dead==0) {
		cputs("Lives: ");
		itoa(slives, scorebuf, 10);
		cputs(scorebuf);
	}
	textcolor(YELLOW);
	cputcxy(0,1,TOP_LEFT);
	for(x=0; x<BOARD_X; x++)
		cputcxy(x+1,1,TOP);
	cputcxy(BOARD_X+1,1,TOP_RIGHT);
	for(y=0;y<BOARD_Y;y++) {
		cputcxy(0,y+2,LEFT);
		cputcxy(BOARD_X+1,y+2,RIGHT);
	}
	cputcxy(0,BOARD_Y+2,BOT_LEFT);
	for(x=0; x<BOARD_X; x++)
		cputcxy(x+1,BOARD_Y+2,BOT);
	cputcxy(BOARD_X+1,BOARD_Y+2,BOT_RIGHT);
	gotoxy(0, BOARD_Y + 3);
	textcolor(WHITE);
	cputs("SnakeGame v0.02      (C) 2003 Sam Steele");
}

extern const u8 collect_bin[];
extern const u32 collect_bin_size;
extern const u8 gameover_bin[];
extern const u32 gameover_bin_size;

int main()
{
	int x,y;
	char dead;
	char scorebuf[10];
	char *b1;
	int go = 1;

	dead = 0;
	sx = 3;
	sy = 3;
	sdx = 1;
	sdy = 0;
	ssize = 3;
	sscore = 0;
	slives = 3;
	
	gfxInitDefault();
	console = consoleInit(GFX_BOTTOM,NULL);
	csndInit();

	//Transfer the sound effects into RAM
	u8 *collect = linearAlloc(collect_bin_size);
	memcpy(collect, collect_bin, collect_bin_size);
	
	u8 *gameover = linearAlloc(gameover_bin_size);
	memcpy(gameover, gameover_bin, gameover_bin_size);
	
	for(x=0;x<BOARD_X;++x) {
		b1 = board[x];
		for(y=0;y<BOARD_Y;++y) {
			b1[y]=0;
		}
	}

	redraw();

	sx=4+(rand()%(BOARD_X-8));
	sy=4+(rand()%(BOARD_Y-8));

	x=rand()%BOARD_X;
	y=rand()%BOARD_Y;
	board[x][y]=255;
	textcolor(RED);
	cputcxy(x+1, y+2, GEM);

	while (aptMainLoop() && go == 1)
	{
		if(dead == 0) {
			//move head
			sx+=sdx;
			sy+=sdy;
			textcolor(GREEN);
			cputcxy(sx+1,sy+2,SNAKE);

			//check bounds
			if(sx>=BOARD_X || sx<0 || sy>=BOARD_Y || sy<0) { 
				slives--; 
				ssize=3; 
				for(x=0;x<BOARD_X;++x) {
					b1 = board[x];
					for(y=0;y<BOARD_Y;++y) {
					b1[y]=0;
					}
				}
				redraw();
				sx=4+(rand()%(BOARD_X-8));
				sy=4+(rand()%(BOARD_Y-8));
				x=rand()%BOARD_X;
				y=rand()%BOARD_Y;
				board[x][y]=255;
				textcolor(RED);
				cputcxy(x+1, y+2, GEM);
				csndPlaySound(0x08, SOUND_ONE_SHOT | SOUND_FORMAT_16BIT, 16000, 1.0, 0.0, (u32*)gameover, (u32*)gameover, gameover_bin_size);
			}

			//check collisions
			if(board[sx][sy]>0) {
				if(board[sx][sy]==255) { //item
					ssize+=2; //increment snake size
					sscore++; //and score
					do { //place a new item in an empty spot
						x=rand()%BOARD_X; y=rand()%BOARD_Y;
					} while(board[x][y]!=0);
					board[x][y]=255;
					textcolor(RED);
					cputcxy(x+1, y+2, GEM);
					//make the stretch work correctly
					for(x=0;x<BOARD_X;++x) {
						b1 = board[x];
						for(y=0;y<BOARD_Y;++y) {
							if(b1[y]>0&&b1[y]!=255) b1[y]+=2;
						}
					}
					csndPlaySound(0x08, SOUND_ONE_SHOT | SOUND_FORMAT_16BIT, 16000, 1.0, 0.0, (u32*)collect, (u32*)collect, collect_bin_size);
				} else { //snake
					slives--; 
					ssize=3; 
					for(x=0;x<BOARD_X;++x) {
						b1 = board[x];
						for(y=0;y<BOARD_Y;++y) {
							b1[y]=0;
						}
					}
					redraw();
					sx=4+(rand()%(BOARD_X-8));
					sy=4+(rand()%(BOARD_Y-8));
					x=rand()%BOARD_X;
					y=rand()%BOARD_Y;
					board[x][y]=255;
					textcolor(RED);
					cputcxy(x+1, y+2, GEM);
					csndPlaySound(0x08, SOUND_ONE_SHOT | SOUND_FORMAT_16BIT, 16000, 1.0, 0.0, (u32*)gameover, (u32*)gameover, gameover_bin_size);
				}
			}
			if(slives<=0) dead=1;
			board[sx][sy]=ssize+1;

			//decrement the array (simulates motion)
			for(x=0;x<BOARD_X;++x) {
				b1 = board[x];
				for(y=0;y<BOARD_Y;++y) {
					if(b1[y]>0&&b1[y]!=255) {
					--b1[y];
					if(b1[y] == 0)
						cputcxy(x+1,y+2,' ');
					}
				}
			}
		}

		//render the screen
		gotoxy(0,0);
		textcolor(WHITE);
		cputs("Score: ");
		itoa(sscore, scorebuf, 10);
		cputs(scorebuf);
		gotoxy(BOARD_X - 6,0);
		if(dead==0) {
			cputs("Lives: ");
			itoa(slives, scorebuf, 10);
			cputs(scorebuf);
		}
		if(dead==1) {
			textcolor(RED);
			gotoxy((BOARD_X/2) - 4,11); cputs("You Died!");
			gotoxy((BOARD_X/2) - 5,12); cputs("Press START");
		}

		// Flush and swap framebuffers
		gfxFlushBuffers();
		gfxSwapBuffers();

		for(x=0;x<6;x++) { //we do this 6 times so that we can poll the keyboard 6 times more than we move the snake on screen
			gspWaitForVBlank();
			hidScanInput();

			u32 kDown = hidKeysDown();
			if (kDown & KEY_START) {
				go = 0;
			}

			if (kDown & KEY_DUP) {
				if(sdy==0) {
					sdx=0;
					sdy=-1;
				}
			}
			
			if (kDown & KEY_DDOWN) {
				if(sdy==0) {
					sdx=0;
					sdy=1;
				}
			}
			
			if (kDown & KEY_DLEFT) {
				if(sdx==0) {
					sdx=-1;
					sdy=0;
				}
			}
			
			if (kDown & KEY_DRIGHT) {
				if(sdx==0) {
					sdx=1;
					sdy=0;
				}
			}
		}
	}

	gfxExit();
	csndExit();
	linearFree(collect);
	linearFree(gameover);
	return 0;
}
