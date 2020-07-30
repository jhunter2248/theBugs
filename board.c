// board.c

#include "stdio.h"
#include "defs.h"

int CheckBoard(const S_BOARD *pos) {
	int t_pceNum[13] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	int t_bigPce[2] = { 0, 0 };
	int t_majPce[2] = { 0, 0 };
	int t_minPce[2] = { 0, 0 };
	int t_material[2] = { 0, 0 };

	int sq64, t_piece, t_pce_num, sq120, color, pcount;

	U64 t_pawns[3] = { 0ULL, 0ULL, 0ULL };

	t_pawns[WHITE] = pos->pawns[WHITE];
	t_pawns[BLACK] = pos->pawns[BLACK];
	t_pawns[BOTH] = pos->pawns[BOTH];

	//check piece lists
	for(t_piece = wP; t_piece <= bK; t_piece++) {
		for(t_pce_num = 0; t_pce_num < pos->pceNum[t_piece]; t_pce_num++) {
			sq120 = pos->pList[t_piece][t_pce_num];
			ASSERT(pos->pieces[sq120] == t_piece);
		}
	}

	// check piece count and other counters
	for(sq64 = 0; sq64 < 64; sq64++) {
		sq120 = SQ120(sq64);
		t_piece = pos->pieces[sq120];
		t_pceNum[t_piece]++;
		color = PieceCol[t_piece];
		if(PieceBig[t_piece] == TRUE) t_bigPce[color]++;
		if(PieceMaj[t_piece] == TRUE) t_majPce[color]++;
		if(PieceMin[t_piece] == TRUE) t_minPce[color]++;

		t_material[color] += PieceVal[t_piece];
	}

	for(t_piece = wP; t_piece <= bK; t_piece++) {
		ASSERT(t_pceNum[t_piece] == pos->pceNum[t_piece]);
	}

	// check bitboard pawn counts
	pcount = CNT(t_pawns[WHITE]);
	ASSERT(pcount == pos->pceNum[wP]);
	pcount = CNT(t_pawns[BLACK]);
	ASSERT(pcount == pos->pceNum[bP]);
	pcount = CNT(t_pawns[BOTH]);
	ASSERT(pcount == (pos->pceNum[wP] + pos->pceNum[bP]));

	//check bitboard squares
	while(t_pawns[WHITE]) {
		sq64 = POP(&t_pawns[WHITE]);
		ASSERT(pos->pieces[SQ120(sq64)] == wP);
	}

	while(t_pawns[BLACK]) {
		sq64 = POP(&t_pawns[BLACK]);
		ASSERT(pos->pieces[SQ120(sq64)] == bP);
	}

	while(t_pawns[BOTH]) {
		sq64 = POP(&t_pawns[BOTH]);
		ASSERT( (pos->pieces[SQ120(sq64)] == wP) || (pos->pieces[SQ120(sq64)] == bP) );
	}

	ASSERT(t_material[WHITE] == pos->material[WHITE] && t_material[BLACK] == pos->material[BLACK]);
	ASSERT(t_minPce[WHITE] == pos->minPce[WHITE] && t_minPce[BLACK] == pos->minPce[BLACK]);
	ASSERT(t_majPce[WHITE] == pos->majPce[WHITE] && t_majPce[BLACK] == pos->majPce[BLACK]);
	ASSERT(t_bigPce[WHITE] == pos->bigPce[WHITE] && t_bigPce[BLACK] == pos->bigPce[BLACK]);

	ASSERT(pos->side == WHITE || pos->side == BLACK);
	ASSERT(GeneratePosKey(pos) == pos->posKey);
	ASSERT(pos->enPas == NO_SQ || (RanksBrd[pos->enPas] == RANK_6 && pos->side == WHITE)
		|| (RanksBrd[pos->enPas] == RANK_3 && pos->side == BLACK));

	ASSERT(pos->pieces[pos->KingSq[WHITE]] == wK);
	ASSERT(pos->pieces[pos->KingSq[BLACK]] == bK);

	return TRUE;
}

int ParseFen(char *fen, S_BOARD *pos) {
	ASSERT(fen!=NULL);
	ASSERT(pos!=NULL);

	int rank = RANK_8;
	int file = FILE_A;
	int piece = 0;
	int count = 0;
	int i = 0;
	int sq64 = 0;
	int sq120 = 0;

	ResetBoard(pos);

	while ((rank >= RANK_1) && *fen) {
		count = 1;

		switch (*fen) {
			case 'p': piece = bP; break;
			case 'r': piece = bR; break;
			case 'n': piece = bN; break;
			case 'b': piece = bB; break;
			case 'k': piece = bK; break;
			case 'q': piece = bQ; break;
			case 'P': piece = wP; break;
			case 'R': piece = wR; break;
			case 'N': piece = wN; break;
			case 'B': piece = wB; break;
			case 'K': piece = wK; break;
			case 'Q': piece = wQ; break;

			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
				piece = EMPTY;
				count = *fen - '0';
				break;

			case '/':
			case ' ':
				rank--;
				file = FILE_A;
				fen++;
				continue;

			default:
				printf("FEN error \n");
				return -1;
		}


		if(piece != EMPTY) {
			pos->pieces[FR2SQ(file, rank)] = piece;
		}
		//printf("Count: %d\n", count);
		//printf("file: %d\n", file);
		file += count;

		/*
		for(i = 0; i < count; i++) {
			sq64 = rank * 8 + file;
			sq120 = SQ120(sq64);
			if(piece != EMPTY) {
				pos->pieces[sq120] = piece;
			}
			file++;
		}
		*/

		fen++; //increment pointer
	} //Should have reached the end of the first part of the fen

	ASSERT(*fen == 'w' || *fen == 'b');

	pos->side = (*fen == 'w') ? WHITE : BLACK; //determine if white or black move (shortened if statement)
	fen += 2;

	//get castle permissions
	for(i = 0; i < 4; i++) {
		if(*fen == ' ') {
			break;
		}
		switch(*fen) {
			case 'K': pos->castlePerm |= WKCA; break;
			case 'Q': pos->castlePerm |= WQCA; break;
			case 'k': pos->castlePerm |= BKCA; break;
			case 'q': pos->castlePerm |= BQCA; break;
			default: break;
		}
		fen++;
	}
	fen++;

	ASSERT(pos->castlePerm >= 0 && pos->castlePerm <= 15);

	//get enpassant square
	if(*fen != '-') {
		file = fen[0] - 'a';
		rank = fen[1] - '1';

		ASSERT(file >= FILE_A && file <= FILE_H);
		ASSERT(rank >= RANK_1 && rank <= RANK_8);

		pos->enPas = FR2SQ(file,rank);
		fen++;
	}
	fen += 2;

	//get fiftymove number
	if(fen[1] == ' ') {
		pos->fiftyMove = fen[0] - '0';
	}
	else {
		pos->fiftyMove = (fen[0] - '0')*10 + (fen[1] - '0');
		fen++;
	}
	fen += 2;

	ASSERT(pos->fiftyMove >= 0 && pos->fiftyMove <= 100);

	// get number of plys, equal to double the full move count plus 1 if its black move
	if(pos->side == WHITE) {
		if(fen[1] == ' ') {
			pos->gamePly = ((int)(fen[0] - '0') - 1) * 2;
		} else {
			pos->gamePly = ((int)(fen[0] - '0') * 10 + (int)(fen[1] - '0') - 1) * 2;
		}
	}
	else {
		if(fen[1] == ' ') {
			pos->gamePly = ((int)(fen[0] - '0') - 1) * 2 + 1;
		} else {
			pos->gamePly = ((int)(fen[0] - '0') * 10 + (int)(fen[1] - '0') - 1) * 2 + 1;
		}
	}

	printf("game ply:%d\n", pos->gamePly);

	pos->posKey = GeneratePosKey(pos);
	UpdateListsMaterial(pos);

	//InitHashTable(pos->HashTable);

	return 0;
}

void UpdateListsMaterial(S_BOARD *pos) {
	int piece, index, color;

	for(index = 0; index < BRD_SQ_NUM; index++) {
		piece = pos->pieces[index];
		if(piece != NO_SQ && piece != EMPTY) {
			color = PieceCol[piece];

			if(PieceBig[piece] == TRUE) pos->bigPce[color]++;
			if(PieceMin[piece] == TRUE) pos->minPce[color]++;
			if(PieceMaj[piece] == TRUE) pos->majPce[color]++;

			pos->material[color] += PieceVal[piece];

			pos->pList[piece][pos->pceNum[piece]] = index;
			pos->pceNum[piece]++;

			if(piece == wK) pos->KingSq[WHITE] = index;
			if(piece == bK) pos->KingSq[BLACK] = index;

			if(piece == wP) {
				SETBIT(pos->pawns[WHITE], SQ64(index));
				SETBIT(pos->pawns[BOTH], SQ64(index));
			}
			else if(piece == bP) {
				SETBIT(pos->pawns[BLACK], SQ64(index));
				SETBIT(pos->pawns[BOTH], SQ64(index));
			}
		}
	}
}


void ResetBoard(S_BOARD *pos) {
	int index = 0;

	for(index = 0; index < BRD_SQ_NUM; ++index) {
		pos->pieces[index] = NO_SQ;
	}

	for(index = 0; index < 64; ++index) {
		pos->pieces[SQ120(index)] = EMPTY;
	}

	for(index = 0; index < 2; ++index) {
		pos->bigPce[index] = 0;
		pos->majPce[index] = 0;
		pos->minPce[index] = 0;
		pos->material[index] = 0;
	}

	for(index = 0; index < 3; index++) {
		pos->pawns[index] = 0ULL;
	}

	for(index = 0; index < 13; index++) {
		pos->pceNum[index] = 0;
	}

	pos->side = BOTH;
	pos->enPas = NO_SQ;
	pos->fiftyMove = 0;

	pos->gamePly = 0;
	pos->ply = 0;
	pos->hisPly = 0;

	pos->posKey = 0ULL;

	pos->KingSq[WHITE] = NO_SQ;
	pos->KingSq[BLACK] = NO_SQ;

	pos->castlePerm = 0;

	//reset plist
	for(int i = 1; i < 13; i++) {
		for(int j = 0; j < 10; j++) {
			pos->pList[i][j] = EMPTY;
		}
	}

	//pos->HashTable->numEntries = 0;
}

void MirrorBoard(S_BOARD *pos) {
	int tempPiecesArray[64];
	int tempSide = pos->side^1;
	int SwapPiece[13] = { EMPTY, bP, bN, bB, bR, bQ, bK, wP, wN, wB, wR, wQ, wK };
	int tempCastlePerm = 0;
	int tempEnPas = NO_SQ;
	int tempFiftyMove = pos->fiftyMove;
	int tempGamePly = pos->gamePly;
	int tempHisPly = pos->hisPly;
	//int tempPly = pos->ply;

	int sq;
	int tp;

	if(pos->castlePerm & WKCA) tempCastlePerm |= BKCA;
	if(pos->castlePerm & WQCA) tempCastlePerm |= BQCA;

	if(pos->castlePerm & BKCA) tempCastlePerm |= WKCA;
	if(pos->castlePerm & BQCA) tempCastlePerm |= WQCA;

	if(pos->enPas != NO_SQ) {
		tempEnPas = SQ120(Mirror64[SQ64(pos->enPas)]);
	}

	for(sq = 0; sq < 64; sq++) {
		tempPiecesArray[sq] = pos->pieces[SQ120(Mirror64[sq])];
	}

	ResetBoard(pos);

	for(sq = 0; sq < 64; sq++) {
		pos->pieces[SQ120(sq)] = SwapPiece[tempPiecesArray[sq]];
	}

	pos->side = tempSide;
	pos->castlePerm = tempCastlePerm;
	pos->enPas = tempEnPas;
	pos->fiftyMove = tempFiftyMove;
	pos->gamePly = tempGamePly;
	pos->hisPly = tempHisPly;
	//pos->ply = tempPly;


	pos->posKey = GeneratePosKey(pos);

	UpdateListsMaterial(pos);

	ASSERT(CheckBoard(pos));
}

void PrintBoard(const S_BOARD *pos) {
	int sq, file, rank, piece;

	printf("\nGame Board:\n\n");

	for(rank = RANK_8; rank >= RANK_1; rank--) {
		printf("%d   ", rank+1);
		for(file = FILE_A; file <= FILE_H; file++) {
			sq = FR2SQ(file, rank);
			piece = pos->pieces[sq];
			printf("%3c", PceChar[piece]);
		}
		printf("\n");
	}

	printf("\n    ");
	for(file = FILE_A; file <= FILE_H; file++) {
		printf("%3c", 'a' + file);
	}
	printf("\n");
	printf("side: %c\n", SideChar[pos->side]);
	printf("enPas: %d\n", pos->enPas);
	printf("castle: %c%c%c%c\n",
			pos->castlePerm & WKCA ? 'K' : '-',
			pos->castlePerm & WQCA ? 'Q' : '-',
			pos->castlePerm & BKCA ? 'k' : '-',
			pos->castlePerm & BQCA ? 'q' : '-'
			);
	printf("PosKey: %llX\n", pos->posKey);

	printf("fiftyMove: %d\n", pos->fiftyMove);
	printf("Plys: %d\n", pos->gamePly);
}

void PrintFBoard(const S_BOARD *pos) {
	int sq, file, rank, piece;

	printf("\nGame Board:\n\n");

	for(rank = RANK_8; rank >= RANK_1; rank--) {
		printf("%d   ", rank+1);
		for(file = FILE_A; file <= FILE_H; file++) {
			sq = FR2SQ(file, rank);
			piece = pos->pieces[sq];
			printf("%3c", PceChar[piece]);
		}
		printf("\n");
	}

	printf("\n    ");
	for(file = FILE_A; file <= FILE_H; file++) {
		printf("%3c", 'a' + file);
	}
	printf("\n");
	printf("side: %c\n", SideChar[pos->side]);
	printf("enPas: %d\n", pos->enPas);
	printf("castle: %c%c%c%c\n",
			pos->castlePerm & WKCA ? 'K' : '-',
			pos->castlePerm & WQCA ? 'Q' : '-',
			pos->castlePerm & BKCA ? 'k' : '-',
			pos->castlePerm & BQCA ? 'q' : '-'
			);
	printf("PosKey: %llX\n", pos->posKey);

	printf("fiftyMove: %d\n", pos->fiftyMove);
	printf("Plys: %d\n", pos->gamePly);
}

void BoardDump(const S_BOARD *pos) {
	printf("\nBoard Dump:\n");

	//print piece lists
	for(int i = 1; i < 13; i++) {
		printf("pList %d: ", i);
		for(int j = 0; j < 10; j++) {
			printf("%d ", pos->pList[i][j]);
		}
		printf("\n");
	}

	//print
	int index = 0;
	for(index = 0; index < BRD_SQ_NUM; ++index) {
		if(index%10 == 0) printf("\n");
		printf("%5d", pos->pieces[index]);
	}

	printf("\n\n");
}

