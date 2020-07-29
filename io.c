//io.c

#include "stdio.h"
#include "defs.h"

char *PrSq(const int sq) {
	static char SqStr[3];

	int file = FilesBrd[sq];
	int rank = RanksBrd[sq];
	
	sprintf(SqStr, "%c%c", ('a' + file), ('1' + rank));
	
	return SqStr;
}

char *PrMove(const int move) {
	static char MvStr[6];
	
	int ff = FilesBrd[FROMSQ(move)];
	int rf = RanksBrd[FROMSQ(move)];
	int ft = FilesBrd[TOSQ(move)];
	int rt = RanksBrd[TOSQ(move)];
	
	int promoted = PROMOTED(move);
	
	if(promoted) {
		char pchar = 'q';
		if(IsKn(promoted)) {
			pchar = 'n';
		}
		else if(IsR(promoted)) {
			pchar = 'r';
		}
		else if(IsB(promoted)) {
			pchar = 'b';
		}
		sprintf(MvStr, "%c%c%c%c%c", ('a' + ff), ('1' + rf), ('a' + ft), ('1' + rt), pchar);
	}
	else {
		sprintf(MvStr, "%c%c%c%c", ('a' + ff), ('1' + rf), ('a' + ft), ('1' + rt));
	}
	
	return MvStr;
}

int ParseMove(char *ptrChar, S_BOARD * pos) {
	if(ptrChar[0] > 'h' || ptrChar[0] < 'a') return NOMOVE;
	if(ptrChar[1] > '8' || ptrChar[1] < '1') return NOMOVE;
	if(ptrChar[2] > 'h' || ptrChar[2] < 'a') return NOMOVE;
	if(ptrChar[3] > '8' || ptrChar[3] < '1') return NOMOVE;
	
	int from = FR2SQ(ptrChar[0] - 'a', ptrChar[1] - '1');
	int to = FR2SQ(ptrChar[2] - 'a', ptrChar[3] - '1');
	
	ASSERT(SqOnBoard(from) && SqOnBoard(to));
	
	S_MOVELIST list[1];
	GenerateAllMoves(pos, list);
	int MoveNum = 0;
	int Move = 0;
	int PromPce = EMPTY;
	
	for(MoveNum = 0; MoveNum < list->count; MoveNum++) {
		Move = list->moves[MoveNum].move;
		if(FROMSQ(Move) == from && TOSQ(Move) == to) {
			PromPce = PROMOTED(Move);
			if(PromPce != EMPTY) {
				if(IsR(PromPce) && ptrChar[4] == 'r') {
					return Move;
				} else if(IsB(PromPce) && ptrChar[4] == 'b') {
					return Move;
				} else if(IsQ(PromPce) && ptrChar[4] == 'q') {
					return Move;
				} else if(IsKn(PromPce) && ptrChar[4] == 'n') {
					return Move;
				}
				continue;
			}
			return Move;
		}
	}
	return NOMOVE;
}

void PrintMoveList(const S_MOVELIST *list) {
	int move;
	int score;
	
	printf("Movelist: \n");
	for(int i = 0; i < list->count; i++) {
		  move = list->moves[i].move;
		  score = list->moves[i].score;
		  printf("move: %d > %s (score:%d)\n", i + 1, PrMove(move), score);
	}
	printf("MoveList Total %d Moves:\n\n", list->count);
}

void PrintBin(int move) {
	int index = 0;
	printf("As binary: \n");
	for(index = 31; index >= 0; index--) {
		if((1 << index) & move) printf("1");
		else printf("0");
		if(index%4 == 0) printf(" ");
	}
	printf("\n");
}

void PrintBestMove(S_BOARD *pos, S_SEARCHINFO *info, const int bestMove) {
	if(info->GAME_MODE == UCIMODE) {
		printf("bestmove %s\n", PrMove(bestMove));
	} else if(info->GAME_MODE == XBOARDMODE) {
		printf("move %s\n", PrMove(bestMove));
		MakeMove(pos, bestMove);
	} else {
		printf("\n\n***!! TheBugs make move %s !!***\n\n", PrMove(bestMove));
		MakeMove(pos, bestMove);
		PrintBoard(pos);
	}
}

void PrintPvMoves(S_BOARD *pos, S_SEARCHINFO *info, const int currentDepth, const int pvMoves) {
	//int pvMoves = 0;
	int pvNum = 0;
	
	if(info->GAME_MODE == UCIMODE || info->POST_THINKING == TRUE) {
		//pvMoves = GetPvLine(currentDepth, pos);
		printf("pv");
		for(pvNum = 0; pvNum < pvMoves; pvNum++) {
			printf(" %s", PrMove(pos->PvArray[pvNum]));
		}
		printf("\n");
	}
}

void PrintSearchInfo(S_SEARCHINFO *info, const int bestScore, const int currentDepth) {
	if(info->GAME_MODE == UCIMODE) {
		printf("info score cp %d depth %d nodes %ld time %d ", 
			bestScore, currentDepth, info->nodes, GetTimeMs() - info->startTime);
	} else if(info->GAME_MODE == XBOARDMODE && info->POST_THINKING == TRUE) {
		printf("%d %d %d %ld ",
			currentDepth, bestScore, (GetTimeMs() - info->startTime)/10, info->nodes);
	} else if(info->POST_THINKING == TRUE) {
		printf("score:%d depth:%d nodes:%ld time:%d(ms) ",
			bestScore, currentDepth, info->nodes, GetTimeMs() - info->startTime);
	}
}