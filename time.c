//time.c

#include "stdio.h"
#include "defs.h"

int getMoveTime(S_BOARD *pos, int totTime) {
	int totMaterial = 0;
	int time = 0;
	int openingCoEff = 1.3;

	for(int i = 0; i < 13; i++) {
		for(int j = 0; j < pos->pceNum[i]; j++) {
			totMaterial += DefaultMatVal[i];
		}
	}

	if(totMaterial > 60) {
		time = totTime/(5 / 4 * totMaterial - 30) * openingCoEff;
	} else if(totMaterial <=60 && totMaterial >= 20) {
		time = totTime/(3 / 8 * totMaterial + 22);
	} else if(totMaterial < 20 && totMaterial > 0) {
		time = totTime/(totMaterial + 10);
	} else {
		printf("DEBUG: totMaterial error - %d\n", totMaterial);
	}

	if(time <= 50) {
		return 50;
	}

	printf("DEBUG: totMaterial - %d\n", totMaterial);

	if(time == 0) {
		printf("DEBUG: time error - %d\n", time);
	}

	return time;
}
