// uci.c

#include "stdio.h"
#include "defs.h"
#include "string.h"

#define INPUTBUFFER 400 * 6

void ParseGo(char* line, S_SEARCHINFO *info, S_BOARD *pos) {
	int depth = -1;
	int movestogo = 30;
	int movetime = -1;
	int time = -1;
	int inc = 0;
	char *ptr = NULL;
	info->timeSet = FALSE;

	if((ptr = strstr(line, "infinite"))) {
		;
	}

	if((ptr = strstr(line, "binc")) && pos->side == BLACK) {
		inc = atoi(ptr + 5);
	}

	if((ptr = strstr(line, "winc")) && pos->side == WHITE) {
		inc = atoi(ptr + 5);
	}

	if((ptr = strstr(line, "btime")) && pos->side == BLACK) {
		time = atoi(ptr + 6);
	}

	if((ptr = strstr(line, "wtime")) && pos->side == WHITE) {
		time = atoi(ptr + 6);
	}

	if((ptr = strstr(line, "movestogo"))) {
		movestogo = atoi(ptr + 10);
	}

	if((ptr = strstr(line, "movetime"))) {
		movetime = atoi(ptr + 9);
	}

	if((ptr = strstr(line, "depth"))) {
		depth = atoi(ptr + 6);
	}

	if(movetime != -1) {
		time = movetime;
		movestogo = 1;
	}

	info->startTime = GetTimeMs();
	info->depth = depth;

	if(time != -1) {
		info->timeSet = TRUE;
		time /= movestogo;
		time -= 50;
		info->endTime = info->startTime + time + inc;
	}

	if(depth == -1) {
		info->depth = MAXDEPTH;
	}

	//printf("time: %d start: %d stop: %d depth: %d timeset: %d\n",
	//	time, info->startTime, info->endTime, info->depth, info->timeSet);
	SearchPosition(pos, info);
}

void ParsePosition(char* lineIn, S_BOARD *pos) {
	lineIn += 9;
	char *ptrChar = lineIn;

	if(strncmp(lineIn, "startpos", 8) == 0) {
		ParseFen(START_FEN, pos);
	} else {
		ptrChar = strstr(lineIn, "fen");
		if(ptrChar == NULL) {
			ParseFen(START_FEN, pos);
		} else {
			ptrChar += 4;
			ParseFen(ptrChar, pos);
		}
	}

	ptrChar = strstr(lineIn, "moves");
	int move;
	if(ptrChar != NULL) {
		ptrChar += 6;
		while(*ptrChar) {
			move = ParseMove(ptrChar, pos);
			if(move == NOMOVE) break;
			MakeMove(pos, move);
			pos->ply = 0;
			while(*ptrChar && *ptrChar != ' ') ptrChar++;
			ptrChar++;
		}
	}
	//PrintBoard(pos);
}

void Uci_Loop(S_BOARD *pos, S_SEARCHINFO *info) {
	setbuf(stdin, NULL);
	setbuf(stdout, NULL);

	char line[INPUTBUFFER];
	printf("id name %s\n", NAME);
	printf("id author Jeremy\n");
	printf("uciok\n");

	int MB = 0;

	while(TRUE) {
		memset(&line[0], 0, sizeof(line));
		fflush(stdout);
		if(!fgets(line, INPUTBUFFER, stdin)) {
			continue;
		}

		if(line[0] == '\n') {
			continue;
		}

		if(!strncmp(line, "isready", 7)) {
			printf("readyok\n");
			continue;
		} else if(!strncmp(line, "position", 8)) {
			ParsePosition(line, pos);
		} else if(!strncmp(line, "ucinewgame", 10)) {
			ParsePosition("position startpos\n", pos);
		} else if (!strncmp(line, "go", 2)) {
			ParseGo(line, info, pos);
		} else if(!strncmp(line, "quit", 4)) {
			info->quit = TRUE;
			break;
		} else if(!strncmp(line, "uci", 3)) {
			printf("id name %s\n", NAME);
			printf("id author Jeremy\n");
			printf("uciok\n");
		} else if(!strncmp(line, "setoption name Hash value ", 26)) {
			sscanf(line, "%*s %*s %*s %*s %d", &MB);
			if(MB < 4) {
				MB = 4;
			}
			if(MB > 1024) {
				MB = 1024;
			}
			printf("Set Hash to %d MB\n", MB);
			InitHashTable(pos->HashTable, MB);
		}
		if(info->quit) break;
	}
}
