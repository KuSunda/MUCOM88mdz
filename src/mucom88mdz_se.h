//--------------------------------------
// このファイルはbintosSeで自動生成しています
//--------------------------------------
#ifndef _MUCOM88MDZ_SE_H_
#define _MUCOM88MDZ_SE_H_
#include <genesis.h>
#include "mucom88mdz.h"
enum{
	MUB_SEID_BOM_D,
	MUB_SEID_CANCEL00_D,
	MUB_SEID_CANCEL01_D,
	MUB_SEID_COIN_D,
	MUB_SEID_CURSOR_D,
	MUB_SEID_OK_D,
	MUB_SEID_PAUSE_D,
	MUB_SEID_SHOT_D,
	MUB_SEID_NUM
};

//--------------------------------------

//--------------------------------------
#define MUB_SE_NUM	(8)
#define MUB_SE_SET_NUM	(8)

//--------------------------------------
extern const u16 Mucom88mdzSeTbl;
extern const u8 Mucom88mdz_SeIdTbl[MUB_SE_NUM];
extern const SE_REQ_TBL Mucom88mdz_SeSetTbl[MUB_SE_SET_NUM];
#endif /* _MUCOM88MDZ_SE_H_*/
