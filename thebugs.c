
#include "stdio.h"
#include "stdlib.h"
#include "defs.h"
#include "string.h"

#define FEN1 "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"
#define FEN2 "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2"
#define FEN3 "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2"
#define FEN4 "1kb5/1B2p1Kp/P6n/P1b4p/p2R4/1p6/P4P2/8 w - - 0 1"
#define FEN5 "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 2"
#define FEN6 "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
#define FEN7 "8/3r1p2/8/5P2/4R3/8/8/8 w - - 0 2"
#define PAWNMOVESW "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"
#define PAWNMOVESB "rnbqkbnr/p1p1p3/3p3p/1p1p4/2P1Pp2/8/PP1P1PpP/RNBQKB1R b KQkq e3 0 1"
#define KNIGHTSKINGS "5k2/1n6/4n3/6N1/8/3N4/8/5K2 w - - 0 1"
#define ROOKS "6k1/8/5r2/8/1nR5/5N2/8/6K1 b - - 0 1 "
#define QUEENS "6k1/8/4nq2/8/1nQ5/5N2/1N6/6K1 b - - 1 1 "
#define BISHOPS "6k1/1b6/4n3/8/1n4B1/1B3N2/1N6/2b3K1 b - - 1 1 "
#define CASTLE1 "r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1 "
#define CASTLE2 "r3k2r/8/8/8/8/8/6p1/R3K2R w KQkq - 0 1 "
#define ALLMOVEGEN "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
#define CASTLEPERM "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b Qkq e3 0 1"
#define PAWNCAPT "rnbqkbnr/pppp1ppp/8/4p3/3P4/8/PPP1PPPP/RNBQKBNR w KQkq - 0 1"
#define FEN8 "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8"
#define PERFTTEST "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N w - - 0 1"
#define WAC1 "2rr3k/pp3pp1/1nnqbN1p/3pN3/2pP4/2P3Q1/PPB4P/R4RK1 w - - 0 1"
#define WAC2 "r1b1k2r/ppppnppp/2n2q2/2b5/3NP3/2P1B3/PP3PPP/RN1QKB1R w KQkq - 0 1 "
#define FENTEST "r3k2r/pppn1ppp/8/3p4/3Pn3/3B4/PPP2qP1/RNBQR1K1 w kq - 4 13 "

int main() {

	AllInit();	
	
	S_BOARD pos[1];
    S_SEARCHINFO info[1];
	info->quit = FALSE;
	pos->HashTable->pTable = NULL;
	InitHashTable(pos->HashTable, 1024);
    
	setbuf(stdin, NULL);
    setbuf(stdout, NULL);
	
	
	printf("Welcome to TheBugs! Type 'c' for console mode...\n");
	
	char line[256];
	while (TRUE) {
		memset(&line[0], 0, sizeof(line));

		fflush(stdout);
		if (!fgets(line, 256, stdin))
			continue;
		if (line[0] == '\n')
			continue;
		if (!strncmp(line, "uci",3)) {
			info->GAME_MODE = UCIMODE;
			Uci_Loop(pos, info);
			if(info->quit == TRUE) break;
			continue;
		} else if (!strncmp(line, "xboard",6))	{
			info->GAME_MODE = XBOARDMODE;
			XBoard_Loop(pos, info);
			if(info->quit == TRUE) break;
			continue;
		} else if (!strncmp(line, "c",4))	{
			Console_Loop(pos, info);
			if(info->quit == TRUE) break;
			continue;
		} else if(!strncmp(line, "quit",4))	{
			break;
		}
	}
	
	free(pos->HashTable->pTable);
	return 0;
}