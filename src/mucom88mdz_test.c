//*****************************************************************************
// Mucom88のテスト(使い方のひな形)
//*****************************************************************************
#include <genesis.h>

#include "mucom88mdz.h"
#include "mucom88mdz_test.h"
#include "mucom88mdz_drv.h"
#include "mucom88mdz_mub.h"
#include "mucom88mdz_se.h"
#include "mucom88mdz_pcm.h"

//*****************************************************************************
// Define
//*****************************************************************************
#define	USE_MUCOM_TESTMODE			(1)

//-----------------------------------------------------------------------------
// for Test Mode
enum
{
	BGM_TEST = 0,
	SE_TEST,
	PCM_TEST,
	MUCOM_TEST_NUM
};

typedef struct _MUCOM88_TEST_WORK
{
	u8 rno[4];
	s8 ctr[4];
	s16 sel_no[MUCOM_TEST_NUM];
	s16 sel_no_max[MUCOM_TEST_NUM];

	s16 SoundTestCursor;
	s16 SoundTestCursorOld;
} MUCOM88_TEST_WORK;

static MUCOM88_TEST_WORK Mucom88TestWk;
static MUCOM88_TEST_WORK *Mwk;


//*****************************************************************************
// Work
//*****************************************************************************
// Z80側のステータス
extern MUCOM_Z80STATE Mucom88mdz_Z80State;
//extern MUC88_CHDAT Mucom88mdz_Z80State.chdat_wak[CHANNEL_WK_NUM];

// Mucomm88mdz SE System
u8 Mucom88mdz_SeReqArray[SE_REQ_ARRAY_NUM][SE_CHANNEL_NUM];

// テストモード用
static JOY_WORK    JoyData[MD_JOY_NUM];

//*****************************************************************************
// Program 
//*****************************************************************************
// TestMode
void Mucom88mdz_TestInit();
void Mucom88mdz_TestUpdate();
void Mucom88mdZ_TestMain();

void Mucom88mdz_Init();
void Mucom88mdz_FrameInit();

void Mucom88mdz_CopyStatus68kToZ80();
//void Mucom88mdz_CopyStatusZ80To68k();

void Mucom88mdz_SeReqArrayClear();
void Mucom88mdz_SetSeReq(u8 se_no, u8 channel);
void Mucom88mdz_SetSePackReq(u8 set_no);

//static void se_set_request();

static void musom88_test_menu_disp();
static void mucom88_test_cursor_disp();
static void mucom88_test_statebase_disp();
static void mucom88_test_chdat_disp();

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Debug
void Dbg_SetPrintPos(u16 x, u16 y);
void Dbg_PrintChar(char c, u16 state);
void Dbg_Print8(u8 val, u16 state);
void Dbg_Print16(u16 val, u16 state);
void Dbg_Print32(u32 valo, u16 state);
void Dbg_PrintString(const char *str, u16 state);


//=============================================================================
// Mucom Test
//=============================================================================
#if USE_MUCOM_TESTMODE
//=============================================================================
// 初期化
//=============================================================================
void Mucom88mdz_TestInit()
{
	memset(&Mucom88TestWk, 0, sizeof(MUCOM88_TEST_WORK));
	Mwk = &Mucom88TestWk;

	Mwk->sel_no_max[0] = MUB_NUM;
	Mwk->sel_no_max[1] = MUB_SE_SET_NUM;
	Mwk->sel_no_max[2] = PCM_ID_NUM;

	Mucom88mdz_Driverload();	// Mucom88mdz Z80サウンドドライバーの転送
}

//=============================================================================
// テスト本体
//=============================================================================
void Mucom88mdz_TestUpdate()
{
	switch (Mwk->rno[0])
	{
	case 0:
		Mwk->rno[0]++;				// Z80側のドライバー初期化待ち

		break;
	case 1:
		Mwk->rno[0]++;

		Mucom88mdZ_SetSeBank();		// SEデータのアドレスをZ80にセット

		mucom88_test_statebase_disp();
		mucom88_test_cursor_disp();

		break;
	case 2:
		Mucom88mdz_FrameInit();		// リクエスト用のワークをクリア
									// Z80側のステータス取得
		Mucom88mdZ_TestMain();

		mucom88_test_chdat_disp();
		musom88_test_menu_disp();

		Mucom88mdz_CopyStatus68kToZ80();	// リクエストをZ80に転送

		break;
	}
}

//-----------------------------------------------------------------------------
// テストメイン
//-----------------------------------------------------------------------------
void Mucom88mdZ_TestMain()
{
	Mwk->SoundTestCursorOld = Mwk->SoundTestCursor;

	if (JoyData[P1].on & UP)
	{
		Mwk->SoundTestCursor--;
		if (Mwk->SoundTestCursor < 0)
		{
			Mwk->SoundTestCursor = MUCOM_TEST_NUM - 1;
		}
		mucom88_test_cursor_disp();
	}
	else if (JoyData[P1].on & DOWN)
	{
		Mwk->SoundTestCursor++;
		if (Mwk->SoundTestCursor >= MUCOM_TEST_NUM)
		{
			Mwk->SoundTestCursor = 0;
		}
		mucom88_test_cursor_disp();
	}

	if (JoyData[P1].on & LEFT)
	{
		//        sel_no[MUCOM_TEST_NUM];
		Mwk->sel_no[Mwk->SoundTestCursor]--;
		if (Mwk->sel_no[Mwk->SoundTestCursor] < 0)
		{
			Mwk->sel_no[Mwk->SoundTestCursor] = Mwk->sel_no_max[Mwk->SoundTestCursor] - 1;
		}
	}
	else if (JoyData[P1].on & RIGHT)
	{
		Mwk->sel_no[Mwk->SoundTestCursor]++;
		if (Mwk->sel_no[Mwk->SoundTestCursor] >= Mwk->sel_no_max[Mwk->SoundTestCursor])
		{
			Mwk->sel_no[Mwk->SoundTestCursor] = 0;
		}
	}


	// 演奏
	if (JoyData[P1].on & SHOT_A)
	{
		switch (Mwk->SoundTestCursor)
		{
		case 0:
			// BGM
			Mucom88mdZ_SetMubReq(Mwk->sel_no[0]);
			break;
		case 1:
//			se_set_request();

			Mucom88mdz_SetSePackReq(Mwk->sel_no[1]);

			break;
		case 2:
			Mucom88mdZ_SetPcmReq(Mwk->sel_no[2]);
			break;
		}
	}
	else if (JoyData[P1].on & SHOT_B)
	{
		// 停止
		switch (Mwk->SoundTestCursor)
		{
		case 0:
			Mucom88mdZ_SetMubStop();
			break;
		case 1:
			Mucom88mdz_SetSeAllStop();
			break;
		case 2:
			Mucom88mdZ_SetPcmStop();
			break;
		}
	}
	else if (JoyData[P1].on & SHOT_C)
	{
		// FADE(BGM Only)
		switch (Mwk->SoundTestCursor)
		{
		case 0:
//			Mucom88mdZ_FadeOut();
			Mucom88mdZ_SetMubFadeOut();
			break;
		case 1:
			break;
		case 2:
			break;
		}
	}
	
	// Debug
	if (JoyData[P1].now & SHOT_Z)
	{
		// FADE(BGM Only)
		switch (Mwk->SoundTestCursor)
		{
		case 0:
			Mucom88mdZ_DebugQueReq(1);
			break;
		case 1:
			break;
		case 2:
			break;
		}
	}else{
		Mucom88mdZ_DebugQueReq(0);
	}

}

#endif


//=============================================================================
// 各Channelのステータース表示
//=============================================================================
static void mucom88_test_statebase_disp()
{
	static char *channnel_name_tbl[CHANNEL_WK_NUM] = {
		"FM1 :",
		"FM2 :",
		"FM3 :",

		"PSG1:",
		"PSG2:",
		"PSG3:",

		"RYTHM",

		"FM4 :",
		"FM5 :",
		"FM6 :",

		"PCM",

		"SE1 :",
		"SE2 :",
		"SE3 :"};

	u16 x = 1;
	u16 y = 11;

	//    Dbg_SetPrintPos( x,  y);

	for (int i = 0; i < CHANNEL_WK_NUM; i++)
	{
		if ((i == CH_RYTM) || (i == CH_PCM))
		{
			continue;
		}

		Dbg_SetPrintPos(x, y);
		Dbg_PrintString(channnel_name_tbl[i], 0);

		y += 1;
	}

	Dbg_SetPrintPos(1+7, 3);
	Dbg_PrintString("SE1:", 0);
	Dbg_SetPrintPos(10+7, 3);
	Dbg_PrintString("SE2:", 0);
	Dbg_SetPrintPos(19+7, 3);
	Dbg_PrintString("SE3:", 0);

	x = 1;
	y = 0;

	Dbg_SetPrintPos(x, y);
	Dbg_PrintString("MUCOM88mdz TEST", 0);

	x = 1;
	y = 24;

	Dbg_SetPrintPos(x, y);
	Dbg_PrintString("UD:SEL BGM/SE LR:SEL No", 0);

	y = 25;
	Dbg_SetPrintPos(x, y);
	Dbg_PrintString("A:PLAY B:STOP C:FADE(Mub)", 0);
}

//=============================================================================
//! @fn
//! @brief z80側の演奏状態を確認する
//=============================================================================
static void musom88_test_menu_disp()
{
	static char *snd_state_tbl[] = {
		"STOP",
		"PLAY",};

	static char *loop_mub_tbl[] = {
		"LoopOff", // Loop有り
		"LoopOn ", // Loop無し
	};

	static char *looped_tbl[] = {
		"------",
		"Looped",
	};

	SYS_disableInts();

	Dbg_SetPrintPos(1, 2);
	Dbg_PrintString("Mub:", 0);
	Dbg_SetPrintPos(5, 2);
	Dbg_PrintString("                ", 0); //　消す
	Dbg_SetPrintPos(5, 2);
	Dbg_PrintString(Mucom88mdz_mub_name_tbl[Mwk->sel_no[0]], 0);

	Dbg_SetPrintPos(1, 3);
	Dbg_PrintString("Se :", 0);

	Dbg_SetPrintPos(5, 3);
	Dbg_Print8(Mwk->sel_no[1], 0);

	Dbg_SetPrintPos(1, 4);
	Dbg_PrintString("Pcm:", 0);

	Dbg_SetPrintPos(5, 4);
	Dbg_Print8(Mwk->sel_no[2], 0);

	Dbg_SetPrintPos(8, 4);		// STOP PLAY
	Dbg_PrintString(snd_state_tbl[Mucom88mdz_Z80State.z80_PcmState], 0);

	u16 x;

	x = 25;
	Dbg_SetPrintPos(x, 22);
	Dbg_PrintString("OCT:", 0);
	Dbg_Print8(Mucom88mdz_Z80State.z80_OctUnderNum, 0);
//	Dbg_SetPrintPos(x, 22);
//	Dbg_PrintString("[w]:", 0);
//	Dbg_Print8(Mucom88mdz_Z80State.z80_NoisewNum, 0);

	Dbg_SetPrintPos(15, 2);		// MUB STOP PLAY
	Dbg_PrintString(snd_state_tbl[Mucom88mdz_GetPlayState()], 0);

	// LOOP曲か？
	Dbg_SetPrintPos(1, 6);
	Dbg_PrintString(loop_mub_tbl[Mucom88mdz_GetLoopMubFlag()], 0);

	// 1回ループしたか？
	Dbg_SetPrintPos(1, 7);
	Dbg_PrintString(looped_tbl[Mucom88mdz_GetLooped()], 0);

	// FLAGSETの値
	Dbg_SetPrintPos(1, 8);
	Dbg_PrintString("FLAGSET:", 0);
	Dbg_SetPrintPos(9, 8);
	Dbg_Print8(Mucom88mdz_GetFlag(), 0);

	// SEの状態 STOP PLAY
	u8 se_state = Mucom88mdz_GetSeState();
	Dbg_SetPrintPos(5+7, 3);
	Dbg_PrintString(snd_state_tbl[((se_state & 0x01) != 0)], 0);
	Dbg_SetPrintPos(14+7, 3);
	Dbg_PrintString(snd_state_tbl[((se_state & 0x02) != 0)], 0);
	Dbg_SetPrintPos(23+7, 3);
	Dbg_PrintString(snd_state_tbl[((se_state & 0x04) != 0)], 0);

	SYS_enableInts();
}

//=============================================================================
//=============================================================================
static void mucom88_test_cursor_disp()
{
	// 曲/SEの選択
	Dbg_SetPrintPos(0, 2 + Mwk->SoundTestCursorOld);
	Dbg_PrintString(" ", 0);

	Dbg_SetPrintPos(0, 2 + Mwk->SoundTestCursor);
	Dbg_PrintString(">", 0);
}


//------------------------------------------------------------------
// CHDATの状態を表示
static void mucom88_test_chdat_disp()
{
	static char *tone_name_tbl[CHANNEL_WK_NUM] = {
		"c ",
		"c#",
		"d ",
		"d#",
		"e ",
		"f ",
		"f#",
		"g ",
		"g#",
		"a ",
		"a#",
		"b ",
	};

	static char *octave_name_tbl[9] = {
		"o1", // 0
		"o2", // 1
		"o3", // 2
		"o4", // 3
		"o5", // 4
		"o6", // 5
		"o7", // 6
		"o8", // 7
		"o9", // 8
	};

	u16 x = 6;
	u16 y = 11;

	SYS_disableInts();

	for (int i = 0; i < CHANNEL_WK_NUM; i++)
	{
		if ((i == CH_RYTM) || (i == CH_PCM))
		{
			continue;
		}

		// オクターブ
		Dbg_SetPrintPos(x, y);
		Dbg_PrintString(octave_name_tbl[(Mucom88mdz_Z80State.chdat_wak[i].ftune >> 4)], 0);

		// 音符
		Dbg_SetPrintPos(x + 3, y);

		if( (Mucom88mdz_Z80State.chdat_wak[i].state & 0x40) != 0 /*6Bit*/ ){
			Dbg_PrintString(tone_name_tbl[(Mucom88mdz_Z80State.chdat_wak[i].ftune & 0x0f)], 0);
		}else{
			Dbg_PrintString("  ", 0);		// Key Off
		}

		// Volume
		Dbg_SetPrintPos(x + 6, y);
		Dbg_Print8(Mucom88mdz_Z80State.chdat_wak[i].volume, 0);

		if( (i == CH_SSG3) || (i == CH_SE_SSG3) ){
			Dbg_SetPrintPos(x + 9, y);
			Dbg_PrintString("P:", 0);		// Key Off
			Dbg_Print8(Mucom88mdz_Z80State.chdat_wak[i].mix_mode, 0);

			Dbg_PrintString(" ", 0);		// Key Off
			Dbg_PrintString("w:", 0);		// Key Off
			Dbg_Print8(Mucom88mdz_Z80State.chdat_wak[i].w_param, 0);
		}

		y += 1;
	}
	SYS_enableInts();
}

//*****************************************************************************
//*****************************************************************************
// Subroutine
//*****************************************************************************
//*****************************************************************************

//=============================================================================
//
//=============================================================================
static u16 posX;
static u16 posY;
static char *hex = "0123456789ABCDEF";
//=============================================================================
//! @fn void void Dbg_SetPrintPos(u16 x, u16 y)
//! @brief Plan_Aへの文字列表示　座標セット
//! @param[in] u16 pos_x
//! @param[in] u16 pos_y
//! @note 基本的にアプリからはマクロを使う
//=============================================================================
void Dbg_SetPrintPos(u16 x, u16 y)
{
	posX = x;
	posY = y;
}

//=============================================================================
//! @fn void Dbg_PrintChar(char c, u16 state)
//! @brief Plan_Aへの文字列表示　座標セット
//! @param[in] char c 文字コード
//! @param[in] u16 state 0:PAL0 other:PAL1
//! @note 基本的にアプリからはマクロを使う
//=============================================================================
void Dbg_PrintChar(char c, u16 state)
{
	char temp[2];
	temp[0] = c;
	temp[1] = 0;
	VDP_setTextPalette(state ? 1 : 0);
	VDP_drawText(temp, posX, posY);
	posX += 2;
}

//=============================================================================
//! @fn void Dbg_Print8(u8 val, u16 state)
//! @brief Plan_Aへの　1Byteの文字列表示
//! @param[in] u8 表示数値
//! @param[in] u16 state 0:PAL0 other:PAL1
//! @note 基本的にアプリからはマクロを使う
//=============================================================================
void Dbg_Print8(u8 val, u16 state)
{
	char temp[8];
	temp[0] = hex[(val >> 4) & 15];
	temp[1] = hex[val & 15];
	temp[2] = 0;
	VDP_setTextPalette(state ? 1 : 0);
	VDP_drawText(temp, posX, posY);
	//    posX += 8;
	posX += 2;
}

//=============================================================================
//! @fn void Dbg_Print16(u8 val, u16 state)
//! @brief Plan_Aへの　2Byteの文字列表示
//! @param[in] u16 表示数値
//! @param[in] u16 state 0:PAL0 other:PAL1
//! @note 基本的にアプリからはマクロを使う
//=============================================================================
void Dbg_Print16(u16 val, u16 state)
{
	char temp[8];

	temp[0] = hex[val >> 12];
	temp[1] = hex[(val >> 8) & 15];
	temp[2] = hex[(val >> 4) & 15];
	temp[3] = hex[val & 15];
	temp[4] = 0;
	VDP_setTextPalette(state ? 1 : 0);
	VDP_drawText(temp, posX, posY);
	//    posX += 8;
	posX += 4;
}

//=============================================================================
//! @fn void Dbg_Print32(u8 val, u16 state)
//! @brief Plan_Aへの　4Byteの文字列表示
//! @param[in] u32 表示数値
//! @param[in] u16 state 0:PAL0 other:PAL1
//! @note 基本的にアプリからはマクロを使う
//=============================================================================
void Dbg_Print32(u32 valo, u16 state)
{
	char temp[16];
	u16 val;

	val = 0;
	val = valo >> 16;

	temp[0] = hex[val >> 12];
	temp[1] = hex[(val >> 8) & 15];
	temp[2] = hex[(val >> 4) & 15];
	temp[3] = hex[val & 15];

	val = valo & 0xffff;

	temp[4] = hex[val >> 12];
	temp[5] = hex[(val >> 8) & 15];
	temp[6] = hex[(val >> 4) & 15];
	temp[7] = hex[val & 15];

	temp[8] = 0;
	VDP_setTextPalette(state ? 1 : 0);
	VDP_drawText(temp, posX, posY);

	posX += 8;
}

//=============================================================================
//! @fn void Dg_PrintString(const char *str, u16 state)
//! @brief Plan_Aへの文字列表示
//! @param[in] const char *str 表示文字列
//! @param[in] u16 state 0:PAL0 other:PAL1
//! @note 基本的にアプリからはマクロを使う
//=============================================================================
void Dbg_PrintString(const char *str, u16 state)
{
	u16 len;

	len = strlen(str);

	VDP_setTextPalette(state ? 1 : 0);
	if (state == 0)
	{
		VDP_drawText(str, posX, posY);
	}
	else
	{
		VDP_clearText(posX, posY, len);
	}
	posX += len;
}


/******************************************************************************
 * Work
 ******************************************************************************/

/******************************************************************************
 * Forword
 ******************************************************************************/
void JoySystemInitilize();
void JoyDataUpdate();

static void set_joydata(u16 port, u16 joytype);

//=============================================================================
void JoySystemInitilize()
{
    u16 i;

    for(i = 0 ; i < JOY_PORT_NUM  ; i++)
    {
        JoyData[i].porttype = JOY_getPortType(i);
        if( JoyData[i].porttype == PORT_TYPE_PAD ){
            JoyData[i].joytype = JOY_getJoypadType(i);
        }
    }
}

//=============================================================================
void JoyDataUpdate()
{
    u16  i;

	// イニシャライズを持ってこないと、途中の抜き差しに対応できない

    for(i = 0 ; i < JOY_PORT_NUM /*CAN_USE_JOYPORT_NUM*/ ; i++){

        if(JoyData[i].porttype == PORT_TYPE_PAD ){
            // Support
            switch(JoyData[i].joytype){
                case JOY_TYPE_PAD3:
                case JOY_TYPE_PAD6:
                    set_joydata(i,JoyData[i].joytype);
                    break;
                default:
                    JoyData[i].old = JoyData[i].now = JoyData[i].on = 0x0000;
                    break;
            };
        } else {
            // NO Support
            JoyData[i].old = JoyData[i].now = JoyData[i].on = 0x0000;
        }
    }
}

//=============================================================================
// パッドのデータを取得
//=============================================================================
static void set_joydata(u16 port, u16 joytype)
{
    if( JoyData[port].joytype != joytype ){
        JoyData[port].now = JOY_readJoypad(port);
        JoyData[port].old = 0x0000;
        JoyData[port].on = 0x0000;
        return;
    }
    // old
    JoyData[port].old = JoyData[port].now;
    // new
    JoyData[port].now = JOY_readJoypad(port);

    // trg
    JoyData[port].on = JoyData[port].now ^ JoyData[port].old;
    JoyData[port].on &= JoyData[port].now;

    // rep
}

//*****************************************************************************
// E n d o f F i l e
//*****************************************************************************
