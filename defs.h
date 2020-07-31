#ifndef DEFS_H
#define DEFS_H

#include "stdio.h"
#include "stdlib.h"

//#define DEBUG

#ifndef DEBUG
#define ASSERT(n)
#else
#define ASSERT(n) \
if(!(n)) { \
printf("%s - Failed ", #n); \
printf("on %s ", __DATE__); \
printf("at %s ", __TIME__); \
printf("in File %s ", __FILE__); \
printf("at Line %d\n", __LINE__); \
exit(1);}
#endif

typedef unsigned long long U64;

#define NAME "TheBugs 1.0"
#define BRD_SQ_NUM 120

#define MAXGAMEMOVES 2048
#define MAXPOSITIONMOVES 256
#define MAXDEPTH 64

#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

//number the pieces, files, and ranks
enum { EMPTY, wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK };
enum { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NONE };
enum { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE };

//number the colors
enum { WHITE, BLACK, BOTH };

enum { UCIMODE, XBOARDMODE, CONSOLEMODE };

//number the board squares
enum {
	A1 = 21, B1, C1, D1, E1, F1, G1, H1,
	A2 = 31, B2, C2, D2, E2, F2, G2, H2,
	A3 = 41, B3, C3, D3, E3, F3, G3, H3,
	A4 = 51, B4, C4, D4, E4, F4, G4, H4,
	A5 = 61, B5, C5, D5, E5, F5, G5, H5,
	A6 = 71, B6, C6, D6, E6, F6, G6, H6,
	A7 = 81, B7, C7, D7, E7, F7, G7, H7,
	A8 = 91, B8, C8, D8, E8, F8, G8, H8, NO_SQ
};

enum { FALSE, TRUE };

// castle permission using four bits
enum { WKCA = 1, WQCA = 2, BKCA = 4, BQCA = 8 };

typedef struct {
	char fen[200];
	unsigned long depth[6];
} S_PERFT;

typedef struct {
	S_PERFT perfts[200];
	int count;
} S_PERFTLIST;

typedef struct {
	int move;
	int score;
} S_MOVE;

typedef struct {
	S_MOVE moves[MAXPOSITIONMOVES];
	int count;
} S_MOVELIST;

enum { HFNONE, HFALPHA, HFBETA, HFEXACT};

typedef struct {
	U64 posKey;
	int move;
	int score;
	int depth;
	int flags;
} S_HASHENTRY;

typedef struct {
	S_HASHENTRY *pTable;
	int numEntries;
	int newWrite;
	int overWrite;
	int hit;
	int cut;
} S_HASHTABLE;

typedef struct {
	
	int move;
	int castlePerm;
	int enPas;
	int fiftyMove;
	U64 posKey;
	
} S_UNDO;

typedef struct {
	
	int pieces[BRD_SQ_NUM];
	U64 pawns[3];
	
	int KingSq[2];
	
	int side;
	int enPas; //Stores an enPassant square
	int fiftyMove;
	
	int gamePly;
	int ply;
	int hisPly;
	
	int castlePerm; //four bits to store the castling permissions
	
	U64 posKey; //Unique key for storing the position
	
	int pceNum[13]; //stores the number of pieces for each chess piece (ex. if pceNum[2] = 2, there are 2 white knights)
	int bigPce[2]; //stores number of pieces that arent pawns
	int majPce[2]; //stores the number of major pieces
	int minPce[2]; //stores the number of minor pieces
	int material[2];
	
	S_UNDO history[MAXGAMEMOVES];

	// piece list (13 types of pieces with max of 10 for each piece)
	int pList[13][10];
	
	S_HASHTABLE HashTable[1];
	int PvArray[MAXDEPTH];
	
	int searchHistory[13][BRD_SQ_NUM]; //moves that beat alpha but not beta?
	int searchKillers[2][MAXDEPTH]; //beta cutoffs (the first and second best moves for each depth?)
	
} S_BOARD;


typedef struct {
	int startTime;
	int endTime;
	int depth;
	//int depthSet;
	int timeSet;
	int movesToGo;
	int infinite;
	int nullCut;
	
	long nodes;
	
	int quit;
	int stopped;
	
	float fh;
	float fhf;
	
	int GAME_MODE;
	int POST_THINKING;
	
} S_SEARCHINFO;
/* GAME MOVE */
/* Stored in one int (32 bits)
0000 0000 0000 0000 0000 0000 0111 1111 -> FROM, 0x7F
0000 0000 0000 0000 0011 1111 1000 0000 -> TO >> 7, 0x7F
0000 0000 0000 0011 1100 0000 0000 0000 -> Piece Captured >> 14, 0xF
0000 0000 0000 0100 0000 0000 0000 0000 -> EP Capture, 0x40000
0000 0000 0000 1000 0000 0000 0000 0000 -> Pawn start, 0x80000
0000 0000 1111 0000 0000 0000 0000 0000 -> Promoted piece >> 20, 0xF
0000 0001 0000 0000 0000 0000 0000 0000 -> Castling move, 0x1000000
*/

#define FROMSQ(m) ((m) & 0x7F)
#define TOSQ(m) (((m) >> 7) & 0x7F)
#define CAPTURED(m) (((m) >> 14) & 0xF)
#define PROMOTED(m) (((m) >> 20) & 0xF)

#define MFLAGEP 0x40000
#define MFLAGPS 0x80000
#define MFLAGCA 0x1000000

#define MFLAGCAP 0x7C000
#define MFLAGPROM 0xF00000

#define NOMOVE 0
#define INFINITE 30000
#define ISMATE (INFINITE - MAXDEPTH)

/* MACROS */

#define FR2SQ(f,r) ((21 + (f)) + ((r) * 10))
#define SQ64(sq120) (Sq120ToSq64[sq120])
#define SQ120(sq64) (Sq64ToSq120[sq64])
#define POP(b) PopBit(b)
#define CNT(b) CountBits(b)
#define CLRBIT(bb, sq) ((bb) &= ClearMask[(sq)])
#define SETBIT(bb, sq) ((bb) |= SetMask[(sq)])

#define IsB(p) (PieceBishop[(p)])
#define IsR(p) (PieceRook[(p)])
#define IsKn(p) (PieceKnight[(p)])
#define IsKi(p) (PieceKing[(p)])
#define IsQ(p) (PieceQueen[(p)])

#define MIRROR64(sq) (Mirror64[(sq)])

/* GLOBALS */

extern int Sq120ToSq64[BRD_SQ_NUM];
extern int Sq64ToSq120[64];

extern U64 SetMask[64];
extern U64 ClearMask[64];

extern U64 PieceKeys[13][120];
extern U64 SideKey;
extern U64 CastleKeys[16];

extern char PceChar[];
extern char SideChar[];
extern char RankChar[];
extern char FileChar[];

extern int PieceBig[13];
extern int PieceMaj[13];
extern int PieceMin[13];
extern int PieceCol[13];
extern int PieceVal[13];
extern int PiecePawn[13];

extern int FilesBrd[BRD_SQ_NUM];
extern int RanksBrd[BRD_SQ_NUM];

extern int PieceKnight[13];
extern int PieceKing[13];
extern int PieceRook[13];
extern int PieceBishop[13];
extern int PieceQueen[13];
extern int PieceSlides[13];

extern unsigned long leafNodes;

extern int Mirror64[64];

extern U64 FileBBMask[8];
extern U64 RankBBMask[8];

extern U64 BlackPassedMask[64];
extern U64 WhitePassedMask[64];
extern U64 IsolatedMask[64];
extern U64 DPawnMask[64];

extern int DefaultMatVal[13];

/* FUNCTIONS */
//init.c funcs
extern void AllInit();

//bitboards.c funcs
extern void PrintBitBoard(U64 bb);
extern int CountBits(U64 b);
extern int PopBit(U64 *bb);

//hashkeys.c
extern U64 GeneratePosKey(const S_BOARD *pos);

//board.c funcs
extern int ParseFen(char *fen, S_BOARD *pos);
extern void UpdateListsMaterial(S_BOARD *pos);
extern void ResetBoard(S_BOARD *pos);
extern void PrintBoard(const S_BOARD *pos);
extern int CheckBoard(const S_BOARD *pos);
extern void BoardDump(const S_BOARD *pos);
extern void MirrorBoard(S_BOARD *pos);
extern void PrintFBoard(const S_BOARD *pos);

//attack.c
extern int SqAttacked(const int sq, const int side, const S_BOARD *pos);
extern void ShowSqAtBySide(const int side, const S_BOARD *pos);

//myfirsteng.c
extern void ShowSqAtBySide(const int side, const S_BOARD *pos);
extern void PrintBin(int move);

//io.c
extern char *PrSq(const int sq);
extern char *PrMove(const int move);
extern void PrintMoveList(const S_MOVELIST *list);
extern void PrintBin(int move);
extern int ParseMove(char *ptrChar, S_BOARD * pos);
extern void PrintBestMove(S_BOARD *pos, S_SEARCHINFO *info, const int bestMove);
extern void PrintPvMoves(S_BOARD *pos, S_SEARCHINFO *info, const int currentDepth, const int pvMoves);
extern void PrintSearchInfo(S_SEARCHINFO *info, const int bestScore, const int currentDepth);

//validate.c
extern int SqOnBoard(const int sq);
extern int SideValid(const int side);
extern int FileRankValid(const int fr);
extern int PieceValidEmpty(const int pce);
extern int PieceValid(const int pce);

//movegen.c
extern void GenerateAllMoves(const S_BOARD *pos, S_MOVELIST *list);
extern int MoveExists(S_BOARD *pos, const int move);
extern int InitMvvLva();
extern void GenerateAllCaps(const S_BOARD *pos, S_MOVELIST *list);

//makemove.c
extern void TakeMove(S_BOARD *pos);
extern int MakeMove(S_BOARD *pos, int move);
extern void MakeNullMove(S_BOARD *pos);
extern void TakeNullMove(S_BOARD *pos);

//perft.c
extern unsigned long PerftTest(int depth, S_BOARD *pos);
extern void ParsePerftFile(S_PERFTLIST *perftList);
extern void PrintPerftList(S_PERFTLIST *perftList);
extern void Perft(int depth, S_BOARD *pos);
extern void TestPerftList(S_PERFTLIST *perftList);
extern void PrintPerftTest(int depth, S_BOARD *pos);

//search.c
extern void SearchPosition(S_BOARD *pos, S_SEARCHINFO *info);

//misc.c
extern int GetTimeMs();
extern void ReadInput(S_SEARCHINFO *info);

//pvtable.c
extern int ProbePvTable(const S_BOARD *pos);
extern int GetPvLine(const int depth, S_BOARD *pos);
extern int ProbeHashEntry(S_BOARD *pos, int *move, int *score, int alpha, int beta, int depth);
extern void StoreHashEntry(S_BOARD *pos, const int move, int score, const int flags, const int depth);
extern void InitHashTable(S_HASHTABLE *table, int size);
extern void ClearHashTable(S_HASHTABLE *table);

//evaluate.c
extern int EvalPosition(const S_BOARD *pos);

//uci.c
extern void Uci_Loop(S_BOARD *pos, S_SEARCHINFO *info);

//xboard.c
extern void XBoard_Loop(S_BOARD *pos, S_SEARCHINFO *info);
extern void Console_Loop(S_BOARD *pos, S_SEARCHINFO *info);


//time.c
extern int getMoveTime(S_BOARD *pos, int totTime);


#endif
