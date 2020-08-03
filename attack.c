//attack.c

#include "defs.h"
#include "stdio.h"

const int KnDir[8] = { -8, -19, -21, -12, 8, 19, 21, 12 };
const int RkDir[4] = { -1, -10, 1, 10 };
const int BiDir[4] = { -9, -11, 11, 9 };
const int KiDir[8] = { -1, -10, 1, 10, -9, -11, 11, 9 };

int SqAttacked(const int sq, const int side, const S_BOARD *pos) {
	int pce, index, t_sq, dir;

	ASSERT(SqOnBoard(sq));
	ASSERT(SideValid(side));
	ASSERT(CheckBoard(pos));

	//pawns
	if(side == WHITE) {
		if(pos->pieces[sq-11] == wP || pos->pieces[sq-9] == wP) {
			return TRUE;
		}
	}
	else {
		if(pos->pieces[sq+11] == bP || pos->pieces[sq+9] == bP) {
			return TRUE;
		}
	}

	//knights
	for(index = 0; index < 8; index++) {
		pce = pos->pieces[sq + KnDir[index]];
		if(pce != NO_SQ) {
			if(IsKn(pce) && PieceCol[pce] == side) {
				return TRUE;
			}
		}
	}

	// rooks, queens
	for(index = 0; index < 4; index++) {
		dir = RkDir[index];
		t_sq = sq + dir;
		pce = pos->pieces[t_sq];
		while(pce != NO_SQ) {
			if(pce != EMPTY) {
				if((IsR(pce) || IsQ(pce)) && PieceCol[pce] == side) {
					return TRUE;
				}
				break;
			}
			t_sq += dir;
			pce = pos->pieces[t_sq];
		}
	}

	//bishops, queens
	for(index = 0; index < 4; index++) {
		dir = BiDir[index];
		t_sq = sq + dir;
		pce = pos->pieces[t_sq];
		while(pce != NO_SQ) {
			if(pce != EMPTY) {
				if((IsB(pce) || IsQ(pce)) && PieceCol[pce] == side) {
					return TRUE;
				}
				break;
			}
			t_sq += dir;
			pce = pos->pieces[t_sq];
		}
	}

	//kings
	for(index = 0; index < 8; index++) {
		pce = pos->pieces[sq + KiDir[index]];
		if(pce != NO_SQ) {
			if(IsKi(pce) && PieceCol[pce] == side) {
				return TRUE;
			}
		}
	}

	return FALSE;
}

void ShowSqAtBySide(const int side, const S_BOARD *pos) {
	int rank = 0;
	int file = 0;
	int sq = 0;

	printf("\n\nSquares attacked by: %c\n", SideChar[side]);
	for(rank = RANK_8; rank >= RANK_1; rank--) {
		for(file = FILE_A; file <= FILE_H; file++) {
			sq = FR2SQ(file, rank);
			if(SqAttacked(sq, side, pos) == TRUE) {
				printf("X");
			}
			else {
				printf("-");
			}
		}
		printf("\n");
	}
	printf("\n");
}
