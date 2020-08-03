//perft.c

#include "defs.h"
#include "stdio.h"
#include "stdlib.h"

unsigned long leafNodes;

void Perft(int depth, S_BOARD *pos) {
	ASSERT(CheckBoard(pos));

	if(depth == 0) {
		leafNodes++;
		return;
	}

	S_MOVELIST list[1];
	GenerateAllMoves(pos, list);

	int MoveNum = 0;
	for(MoveNum = 0; MoveNum < list->count; MoveNum++) {
		//printf("Trying move: %s\n", PrMove(list->moves[MoveNum].move));
		if(!MakeMove(pos, list->moves[MoveNum].move)) {
			continue;
		}
		Perft(depth - 1, pos);
		TakeMove(pos);
	}

	return;
}

unsigned long PerftTest(int depth, S_BOARD *pos) {
	ASSERT(CheckBoard(pos));

	//PrintBoard(pos);

	//printf("\nStarting Test To Depth:%d\n", depth);

	leafNodes = 0;

	S_MOVELIST list[1];
	GenerateAllMoves(pos, list);

	int move;
	int MoveNum = 0;
	for(MoveNum = 0; MoveNum < list->count; MoveNum++) {
		move = list->moves[MoveNum].move;
		if(!MakeMove(pos, move)) {
			continue;
		}
		long cumnodes = leafNodes;
		Perft(depth - 1, pos);
		TakeMove(pos);
		long oldnodes = leafNodes - cumnodes;
		//printf("move %d : %s : %ld\n", MoveNum + 1, PrMove(move), oldnodes);
	}

	//printf("\nTest Complete : %ld nodes visited\n", leafNodes);

	return leafNodes;
}

void PrintPerftTest(int depth, S_BOARD *pos) {
	ASSERT(CheckBoard(pos));

	PrintBoard(pos);

	printf("\nStarting Test To Depth:%d\n", depth);

	leafNodes = 0;
	int start = GetTimeMs();
	S_MOVELIST list[1];
	GenerateAllMoves(pos, list);

	int move;
	int MoveNum = 0;
	for(MoveNum = 0; MoveNum < list->count; MoveNum++) {
		move = list->moves[MoveNum].move;
		if(!MakeMove(pos, move)) {
			continue;
		}
		long cumnodes = leafNodes;
		Perft(depth - 1, pos);
		TakeMove(pos);
		long oldnodes = leafNodes - cumnodes;
		printf("move %d : %s : %ld\n", MoveNum + 1, PrMove(move), oldnodes);
	}

	printf("\nTest Complete : %ld nodes visited in %dms\n", leafNodes, GetTimeMs() - start);

	return;
}

//example perft
//4k3/8/8/8/8/8/8/4K2R w K - 0 1 ;D1 15 ;D2 66 ;D3 1197 ;D4 7059 ;D5 133987 ;D6 764643
void ParsePerftFile(S_PERFTLIST *perftList) {
	char depth[6][20];
	char temp[6];
	FILE *fp = fopen("perftsuite.epd", "r");
	int fileReturn = 0;
	unsigned long depthInt = 0;
	int index = 0;
	char newline;

	while(fileReturn != EOF) {
		fscanf(fp, "%[^;]s", perftList->perfts[index].fen);
		for(int i = 0; i < 6; i++) {
			fscanf(fp, "%4c", temp);
			fscanf(fp, "%s", depth[i]);
			depthInt = strtoul(depth[i], NULL, 10);
			perftList->perfts[index].depth[i] = depthInt;
		}

		fileReturn = fscanf(fp, "%c ", &newline);
		perftList->count++;
		index++;
	}
	//perftList->count--;

	fclose(fp);
}

void PrintPerftList(S_PERFTLIST *perftList) {
	for(int i = 0; i < perftList->count; i++) {
		printf("Fen: %s; ", perftList->perfts[i].fen);
		for(int j = 0; j < 6; j++) {
			printf("D%d: %lu ", j + 1, perftList->perfts[i].depth[j]);
		}
		printf("\n");
	}
}

void TestPerftList(S_PERFTLIST *perftList) {
	S_BOARD *board = (S_BOARD *)calloc(perftList->count, sizeof(S_BOARD));
	unsigned long nSearched = 0;
	unsigned long nNeeded = 0;
	for(int i = 0; i < perftList->count; i++) {
		ParseFen(perftList->perfts[i].fen, &board[i]);
		for(int j = 0; j < 6; j++) {
			nSearched = PerftTest(j+1, &board[i]);
			nNeeded = perftList->perfts[i].depth[j];
			if(nSearched == nNeeded) {
				printf("Success: %lu of %lu searched for fen: %d and depth: %d\n", nSearched, nNeeded, i + 1, j + 1);
			}
			else {
				printf("Fail: %lu of %lu searched for fen: %d and depth: %d\n", nSearched, nNeeded, i + 1, j + 1);
			}
		}
	}
	free(board);
}
