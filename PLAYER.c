
#include"MPLAY.h"
static UINT stdfreq[] = { 400, 440, 480, 520, 560, 600, 640 };
CRITICAL_SECTION cs;//定义临界区全局变量  
int randrun(){
	long pt = clock(), pt0 = 0,
		freq0 = 500, dfreq = 1000;
	int isound = 0, ilong = 0, ivolu = 0;
	int freq = 10, idur = 0, during = 1000;
	SOUNDINST INST = (SOUNDINST)malloc(sizeof(*INST));
	memset(INST, 0, sizeof(*INST));
	INST->NAME = NULL;
	INST->TYPE = SOUNDINST_DIST;
	INST->FREQ.FREQ.NFREQ = 7;
	INST->FREQ.FREQ.FREQS = (UINT*)malloc(sizeof(UINT)*INST->FREQ.FREQ.NFREQ);
	memcpy(INST->FREQ.FREQ.FREQS, stdfreq, sizeof(UINT)*INST->FREQ.FREQ.NFREQ);
	printf("%d\n", INST->TYPE);
	printf("%d\n", INST->FREQ.FREQ.NFREQ);
	for (UINT i = 0; i < INST->FREQ.FREQ.NFREQ;++i)
		printf(" %d", INST->FREQ.FREQ.FREQS[i]);
	printf("\n");
	printf("%d\n", freq);
	while (1){
		pt0 = pt;
		Sleep((INT)(1000 / freq));
		pt = clock();
		//if ((ch = getc(stdin)) == 'q')break;
		//if (ch < '1' || ch>'7')continue;
		if (idur++ > during)break;
		isound = rand() % 7;
		ilong = rand() % 500;
		ivolu = rand() % 500;
		SOUNDIND(INST, isound, ilong,ivolu);
		printf("%4d %4d %4d\n", isound, ilong, ivolu);
	}
	free(INST->FREQ.FREQ.FREQS);
	free(INST);
	return 0;
}
int readrun(char*music){
	FILE*fid = fopen(music, "r");
	if (fid == NULL)return 1;
	long freq = 0, isound = 0, ilong = 0, ivolu = 0;
	SOUNDINST INST = (SOUNDINST)malloc(sizeof(*INST));
	memset(INST, 0, sizeof(*INST));
	fscanf(fid, "%d\n", &INST->TYPE);
	fscanf(fid, "%d\n", &INST->FREQ.FREQ.NFREQ);
	INST->FREQ.FREQ.FREQS = malloc(sizeof(INT)*INST->FREQ.FREQ.NFREQ);
	for (UINT i = 0; i < INST->FREQ.FREQ.NFREQ; ++i)
		fscanf(fid, "%d", &INST->FREQ.FREQ.FREQS[i]);
	fscanf(fid, "\n");
	if (fscanf(fid, "%d\n", &freq) != 1)return 2;
	while (fscanf(fid, "%d %d %d\n", &isound, &ilong,&ivolu) == 3){
		Sleep((INT)(1000 / freq));
		SOUNDIND(INST,isound, ilong,ivolu);
	}
	fclose(fid);
	free(INST);
	return 0;
}
int main(int argc,char**argv){
	if (argc < 2)
		return randrun();
	else
		return readrun(argv[1]);
}
