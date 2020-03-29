//--------------------------------------
// このファイルはbintosSeで自動生成しています
//--------------------------------------
#include "mucom88mdz.h"
#include "mucom88mdz_se.h"
//--------------------------------------
// SE ID Table 
const u8 Mucom88mdz_SeIdTbl[MUB_SE_NUM] = {
	MUB_SEID_BOM_D,
	MUB_SEID_CANCEL00_D,
	MUB_SEID_CANCEL01_D,
	MUB_SEID_COIN_D,
	MUB_SEID_CURSOR_D,
	MUB_SEID_OK_D,
	MUB_SEID_PAUSE_D,
	MUB_SEID_SHOT_D,
	MUB_SEID_NOISE0000_F,
};

//--------------------------------------
// SE SET Table 
const SE_REQ_TBL Mucom88mdz_SeSetTbl[MUB_SE_SET_NUM] = {
	{{ MUB_SEID_BOM_D, MUB_SEID_NONE, MUB_SEID_NONE }},
	{{ MUB_SEID_CANCEL00_D, MUB_SEID_NONE, MUB_SEID_NONE }},
	{{ MUB_SEID_CANCEL01_D, MUB_SEID_NONE, MUB_SEID_NONE }},
	{{ MUB_SEID_COIN_D, MUB_SEID_NONE, MUB_SEID_NONE }},
	{{ MUB_SEID_CURSOR_D, MUB_SEID_NONE, MUB_SEID_NONE }},
	{{ MUB_SEID_OK_D, MUB_SEID_NONE, MUB_SEID_NONE }},
	{{ MUB_SEID_PAUSE_D, MUB_SEID_NONE, MUB_SEID_NONE }},
	{{ MUB_SEID_SHOT_D, MUB_SEID_NONE, MUB_SEID_NONE }},
	{{ MUB_SEID_NONE, MUB_SEID_NONE, MUB_SEID_NOISE0000_F }},
};