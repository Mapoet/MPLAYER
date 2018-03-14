

#include "MPLAY.h"
typedef struct _SINFO{
	SOUNDINST	INST;
	DWORD		FREQ;
	DWORD		DURI;
	DWORD		VOLU;
}SINFO;




DWORD SOUND_PRO(LPVOID PARAMENT){
	//mciSendString("", NULL, 0, NULL);
	SINFO*PINFO = (SINFO*)PARAMENT;
	char commond[256] = { 0 }, restr[256] = { 0 };
	long  ilen = 0;
	switch (PINFO->INST->TYPE)
	{
	case SOUNDINST_FREQ:
		Beep(PINFO->FREQ, PINFO->DURI);
		break;
	case SOUNDINST_FUNC:
		if (PINFO->INST->FREQ.FUNC==NULL)return 1;
		Beep(PINFO->INST->FREQ.FUNC(PINFO->FREQ), PINFO->DURI);
		break;
	case SOUNDINST_DIST:
		if (PINFO->INST->FREQ.FREQ.NFREQ <= PINFO->FREQ)return 1;
		Beep(PINFO->INST->FREQ.FREQ.FREQS[PINFO->FREQ], PINFO->DURI);
		break;
	case SOUNDINST_FILE:
		if (PINFO->INST->FREQ.FSET.NFREQ <= PINFO->FREQ)return 1;
		switch (PINFO->INST->FREQ.FSET.FTYPE)
		{
		case SOUNDINST_FILE_MULTIPLY:
			sprintf(restr, PINFO->INST->FREQ.FSET.SFMT,PINFO->FREQ);
			sprintf(commond, "open \"%s\" alis wav", restr);
			mciSendString(commond, restr, sizeof(restr), NULL);
			sprintf(commond, "setaudio wav volume to %ld", PINFO->VOLU);
			mciSendString(commond, restr, sizeof(restr), NULL);
			sprintf(commond, "play wav repeat");
			mciSendString(commond, restr, sizeof(restr), NULL);
			Sleep(PINFO->DURI);
			sprintf(commond, "close wav");
			mciSendString(commond, restr, sizeof(restr), NULL);
		case SOUNDINST_FILE_SINGLE:
			sprintf(commond, "open \"%s\" alis wav", PINFO->INST->FREQ.FSET.SFMT);
			mciSendString(commond, restr, sizeof(restr), NULL);
			sprintf(commond, "setaudio wav volume to %ld",PINFO->VOLU);
			mciSendString(commond, restr, sizeof(restr), NULL);
			sprintf(commond, "Status wav length");
			mciSendString(commond, restr, sizeof(restr), NULL);
			ilen = strtol(restr, NULL, 10);
			ilen = ilen / PINFO->INST->FREQ.FSET.NFREQ;
			sprintf(commond, "play wav from %ld to %ld repeat", ilen*PINFO->FREQ, ilen*(PINFO->FREQ + 1));
			mciSendString(commond, restr, sizeof(restr), NULL);
			Sleep(PINFO->DURI);
			sprintf(commond, "close wav");
			mciSendString(commond, restr, sizeof(restr), NULL);
		default:
			break;
		}
		break;
	default:
		break;
	}
	free(PINFO);
	return 0;
}
INT SOUNDIND(SOUNDINST INST, INT FREQ, INT DURI, INT VOLU){
	SINFO* PINFO = (SINFO*)malloc(sizeof(*PINFO));// { FREQ, DUR };
	DWORD PID = 0;
	PINFO->INST = INST;
	PINFO->FREQ = FREQ;
	PINFO->DURI = DURI;
	PINFO->VOLU = VOLU;
	HANDLE  THANDLE = CreateThread(NULL, 0, (PTHREAD_START_ROUTINE)SOUND_PRO, PINFO, 0, &PID);
	return 0;
}
DWORD PLAYSOUNDROAD(LPVOID PARAMENT){
	SOUNDROAD ROAD = (SOUNDROAD)PARAMENT;
	SOUNDDATA NODE = NULL;
	SOUNDLINE LINE = NULL;
	if (ROAD == NULL || ROAD->INSTR == NULL || ROAD->LINES == NULL)return 1;
	NODE = ROAD->LINES;
	while (NODE != NULL){
		LINE = (SOUNDLINE)NODE->DATA;
		for (UINT i = 0; i < LINE->NSOUND; ++i){
			SOUNDIND(ROAD->INSTR, LINE->FREQS[i], LINE->DURIS[i], LINE->VOLUS[i]);
			Sleep(1000 / ROAD->FREQ);
		}
		NODE = NODE->NEXT;
	}
	return 0;
}



INT	  INSERTDATA(SOUNDDATA*, UINT, PVOID){

}
PVOID REMOVEDATA(SOUNDDATA*, UINT){

}
INT  TRAVELDATA(SOUNDDATA, PVOID(*)(PVOID)){

}
INT  FREEDATA(SOUNDDATA, INT(*)(PVOID)){

}


INT MALLOCSOUNDLINE(SOUNDLINE LINE, INT NS, INT*FREQS, INT*DURIS, INT*VOLUS){
	if (LINE == NULL)return 1;
	memset(LINE, 0, sizeof(*LINE));
	LINE->NSOUND = NS;
	if ((LINE->FREQS = (INT*)malloc(sizeof(INT)*LINE->NSOUND)) == NULL)return 1;
	if ((LINE->DURIS = (INT*)malloc(sizeof(INT)*LINE->NSOUND)) == NULL)return 1;
	if ((LINE->VOLUS = (INT*)malloc(sizeof(INT)*LINE->NSOUND)) == NULL)return 1;
	if (FREQS == NULL)memset(LINE->FREQS, 0, sizeof(INT)*LINE->NSOUND);
	else memcpy(LINE->FREQS, FREQS, sizeof(INT)*LINE->NSOUND);
	if (DURIS == NULL)memset(LINE->DURIS, 0, sizeof(INT)*LINE->NSOUND);
	else memcpy(LINE->DURIS, DURIS, sizeof(INT)*LINE->NSOUND);
	if (VOLUS == NULL)memset(LINE->VOLUS, 0, sizeof(INT)*LINE->NSOUND);
	else memcpy(LINE->VOLUS, VOLUS, sizeof(INT)*LINE->NSOUND);
	return 0;
}
INT FREESOUNDLINE(SOUNDLINE LINE){
	if (LINE == NULL)return 1;
	if(LINE->NSOUND == 0)return 0;
	free(LINE->FREQS);
	free(LINE->DURIS);
	free(LINE->VOLUS);
	LINE->FREQS = LINE->DURIS = LINE->VOLUS = NULL;
	return 0;
}
INT MALLOCSOUNDINST(SOUNDINST INST, CHAR*NAME, INT NFREQ, PVOID DATA){
	if (INST == NULL)return 1;
	memset(INST, 0, sizeof(*INST));
	if ((INST->NAME = malloc(strlen(NAME) + 1)) == NULL)return 1;
	strcpy(INST->NAME, NAME); INST->NAME[strlen(NAME)] = '\0';
	if (NFREQ == 0){ 
		if (DATA == NULL){
			INST->TYPE = SOUNDINST_FREQ;
		}
		else{
			INST->TYPE = SOUNDINST_FUNC;
			INST->FREQ.FUNC = DATA;
		}
	}
	if (NFREQ > 0){
		INST->TYPE = SOUNDINST_DIST;
		INST->FREQ.FREQ.NFREQ = NFREQ;
		if ((INST->FREQ.FREQ.FREQS = malloc(sizeof(INT)* INST->FREQ.FREQ.NFREQ)) == NULL)return 1;
		if (DATA == NULL)memset(INST->FREQ.FREQ.FREQS, 0, sizeof(INT)*INST->FREQ.FREQ.NFREQ);
		else memcpy(INST->FREQ.FREQ.FREQS, DATA, sizeof(INT)*INST->FREQ.FREQ.NFREQ);
	}
	if (NFREQ < 0){
		INST->TYPE = SOUNDINST_FILE;
		INST->FREQ.FSET.NFREQ = NFREQ;
		if ((INST->FREQ.FSET.SFMT = malloc(strlen(DATA) + 1)) == NULL)return 1;
		strcpy(INST->FREQ.FSET.SFMT, DATA); INST->FREQ.FSET.SFMT[strlen(DATA)] = '\0';
		INST->FREQ.FSET.FTYPE = (strchr(DATA, '%') == NULL ? SOUNDINST_FILE_SINGLE : SOUNDINST_FILE_MULTIPLY);
	}
	return 0;
}
INT FREESOUNDINST(SOUNDINST INST){
	if (INST == NULL)return 1;
	if (INST->NAME != NULL)free(INST->NAME);
	INST->NAME = NULL;
	switch (INST->TYPE)
	{
	case SOUNDINST_FREQ:break;
	case SOUNDINST_FILE:
		if (INST->FREQ.FSET.SFMT!=NULL)
			free(INST->FREQ.FSET.SFMT);
		INST->FREQ.FSET.SFMT = NULL;
		break;
	case SOUNDINST_DIST:
		if (INST->FREQ.FREQ.NFREQ != 0)
			free(INST->FREQ.FREQ.FREQS);
		INST->FREQ.FREQ.FREQS = NULL;
		break;
	case SOUNDINST_FUNC:break;
	default:
		break;
	}
	return 0;
}
INT MALLOCSOUNDROAD(SOUNDROAD ROAD, SOUNDINST INST, INT FREQ){
	if (ROAD == NULL)return 1;
	ROAD->INSTR = INST;
	ROAD->FREQ = FREQ;
	ROAD->LINES = NULL;
	return 0;
}

INT FREESOUNDROAD(SOUNDROAD ROAD){
	SOUNDDATA NODE = NULL;
	if (ROAD == NULL)return 1;
	ROAD->FREQ = 0;
	if (ROAD->INSTR != NULL&&FREESOUNDINST(ROAD->INSTR) != 0)return 1;
	free(ROAD->INSTR);
	NODE = ROAD->LINES;
	while (NODE!=NULL)
	{
		if (FREESOUNDLINE(NODE->DATA) != 0)return 1;
		free(NODE->DATA);
		NODE = NODE->NEXT;
	}
	ROAD->LINES = NULL;
	return 0;
}
INT PLAYSOUNDQUES(SOUNDQUES QUES){
	SOUNDDATA NODE = NULL;
	DWORD PID = 0;
	HANDLE  THANDLE = NULL;
	NODE = QUES;
	while (NODE!=NULL){
		THANDLE = CreateThread(NULL, 0, (PTHREAD_START_ROUTINE)PLAYSOUNDROAD, NODE->DATA, 0, &PID);
	}
	return 0;
}
INT FREESOUNDQUES(SOUNDQUES QUES){
	SOUNDDATA NODE = NULL;
	NODE = QUES;
	while (NODE != NULL)
	{
		if (FREESOUNDROAD(NODE->DATA) != 0)return 1;
		free(NODE->DATA);
		NODE = NODE->NEXT;
	}
	QUES = NULL;
	return 0;
}
