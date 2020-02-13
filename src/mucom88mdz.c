//*****************************************************************************
//! @file mucom88mdz.c
//! @brief mucom88mdzの68k側
//! @author KuSunda
//*****************************************************************************
#include <genesis.h>

#include "mucom88mdz.h"
#include "mucom88mdz_drv.h"

#include "mucom88mdz_mub.h"
#include "mucom88mdz_se.h"
#include "mucom88mdz_pcm.h"

//*****************************************************************************
// Work
//*****************************************************************************

//-----------------------------------------------------------------------------
// リクエスト管理
MUCOM88MDZ_REQ_WORK Mucom88mdz_ReqWork;

//-----------------------------------------------------------------------------
// Z80側のステータス
MUCOM_Z80STATE Mucom88mdz_Z80State;
//MUC88_CHDAT Mucom88mdz_Z80State.chdat_wak[CHANNEL_WK_NUM];

//-----------------------------------------------------------------------------
// Mucomm88mdz SE System
u8 Mucom88mdz_SeReqArray[SE_REQ_ARRAY_NUM][SE_CHANNEL_NUM];

//*****************************************************************************
// Forword
//*****************************************************************************
void Mucom88mdz_Driverload();

void Mucom88mdz_Init();
void Mucom88mdz_FrameInit();

void Mucom88mdz_CopyStatus68kToZ80();
void Mucom88mdz_CopyStatusZ80To68k();

void Mucom88mdZ_SetMubReq(u8 mub_no);
void Mucom88mdZ_SetMubStop();
void Mucom88mdZ_SetMubFadeOut();

void Mucom88mdZ_SetSeBank();

void Mucom88mdz_SetSeReq(u8 se_no, u8 channel);
void Mucom88mdz_SetSePackReq(u8 set_no);
void Mucom88mdz_SetSeStop(u8 channel);
void Mucom88mdz_SetSeAllStop();

void Mucom88mdZ_SetPcmReq(u8 no);
void Mucom88mdZ_SetPcmStop();

u8 Mucom88mdz_GetPlayState();
u8 Mucom88mdz_GetLoopMubLoag();
u8 Mucom88mdz_GetLooped();
u8 Mucom88mdz_GetFlag();
u8 Mucom88mdz_GetSeState();
u8 Mucom88mdz_GetPcmState();

static void mub_struct_flip(MUBHED *src, MUBHED *dst);
static u16 mub_flip16(u16 num);
static u32 mub_flip32(u32 num);
inline static void z80_BusRequest();
inline static void z80_BusRequestWait();
inline static void z80_BusRelease();

//*****************************************************************************
//*****************************************************************************
// Driver管理
//*****************************************************************************
//*****************************************************************************

//=============================================================================
//!@fn void Mucom88mdz_Driverload()
//!@brief Mucom88 for MD Z80 Sound DriverをZ80に読み込んで起動する
//!@details この関数でドライバー転送後のドライバー操作が2フレーム待ってください。（初期化済みフラグを持つべき）
//=============================================================================
void Mucom88mdz_Driverload()
{
	u8 *ramZ80;
	const u8 *source;
	u32 length;
	u32 cnt;

	// Z80 停止
	z80_BusRequest(); // HALT

	z80_BusRequestWait();

	length = sizeof(mucom88mdz_drv);
	source = mucom88mdz_drv;

	/* 転送 */
	ramZ80 = (u8 *)0xA00000;
	for (cnt = length; cnt != 0; cnt--)
		*ramZ80++ = *source++;

	// z80 稼働
	z80_BusRelease();
}

//*****************************************************************************
//*****************************************************************************
// システム
//*****************************************************************************
//*****************************************************************************

//=============================================================================
//!@fn void Mucom88mdz_FrameInit()
//!@brief Mucom88 for MD 初期化
//!@details 68k側のmucom88mdz関係の初期化
//=============================================================================
void Mucom88mdz_Init()
{
	memset(&Mucom88mdz_ReqWork, 0, sizeof(MUCOM88MDZ_REQ_WORK));
}

//=============================================================================
//!@fn void Mucom88mdz_FrameInit()
//!@brief 毎フレームの初期化
//!@details フレーム内のサウンドリクエストが登録される前に1回呼ぶ事。
//=============================================================================
void Mucom88mdz_FrameInit()
{
	memset(&Mucom88mdz_ReqWork, 0, sizeof(MUCOM88MDZ_REQ_WORK));
	Mucom88mdz_CopyStatusZ80To68k();
}

//=============================================================================
//!@fn void Mucom88mdz_CopyStatus68kToZ80()
//!@brief 68Kから、.mub/SE/PCMのリクエストをZ80に転送する
//!@details .Mub/SEのリクエストは Z80転送後に次のVSync頭で実際のワークにコピーして使う。
//!@details PCMはコピーせずに使います。
//!@details この関数は1フレームに1回呼ぶ事。
//=============================================================================
void Mucom88mdz_CopyStatus68kToZ80()
{
	u8 *ap;

	// 前処理

	// Z80停止
	z80_BusRequest(); // HALT
	z80_BusRequestWait();

	// mub
	if (Mucom88mdz_ReqWork.mub_req_flag)
	{
		ap = (u8 *)REQ_CMMAND;
		*ap = Mucom88mdz_ReqWork.mub_req_command;
		ap = (u8 *)REQ_PARAM;
		*ap = Mucom88mdz_ReqWork.mub_req_param;

		if (Mucom88mdz_ReqWork.mub_req_command == MUB_REQ_START)
		{
			u32 mub_bank = Mucom88mdz_ReqWork.mub_bank;

			// mubリクエスト
			ap = (u8 *)MUB_LOW_ADDR;
			*ap = (u8)(mub_bank & 0x000000ff);

			ap = (u8 *)MUB_MID_ADDR;
			*ap = (u8)((mub_bank & 0x0000ff00) >> 8);

			ap = (u8 *)MUB_HIGH_ADDR;
			*ap = (u8)((mub_bank & 0x00ff0000) >> 16);
		}

		// MubがリクエストされたらPCMを止める。

		// MubとPCMが同時にリクエストされたらPCMリクエストをキャンセル
//		Mucom88mdz_ReqWork.pcm_req_flag = 0;
		Mucom88mdZ_SetPcmStop();

	}

	// SE
	ap = (u8 *)REQ_EFCT1;
	int i;
	for (i = 0; i < SE_CHANNEL_NUM; i++) // 3Chループ
	{
		*ap++ = Mucom88mdz_ReqWork.Mucom88mdz_SeReqArray[SE_REQ_COMMAND][i];
		*ap++ = Mucom88mdz_ReqWork.Mucom88mdz_SeReqArray[SE_REQ_NO][i];
	}

	// PCM
	if (Mucom88mdz_ReqWork.pcm_req_flag != 0)
	{
		*ap = Mucom88mdz_ReqWork.pcm_req_command;

		// /	Mucom88mdz_ReqWork.pcm_req_no = no;
		u32 adrs = (u32)Mucom88mdz_pcm_tbl[Mucom88mdz_ReqWork.pcm_req_no];

		// top adrs    u32 mub_adrs = 0; // mubのアドレス
		ap = (u8 *)PCM_LOW_ADDR;
		*ap = (u8)((u32)(adrs)&0x000000ff);

		ap = (u8 *)PCM_MID_ADDR;
		*ap = (u8)(((u32)(adrs)&0x0000ff00) >> 8);

		ap = (u8 *)PCM_HIGH_ADDR;
		*ap = (u8)(((u32)(adrs)&0x00ff0000) >> 16);

		// size
		//	u16 adrs2 = (u16)(adrs & 0xffff);
		u16 pcm_size = Mucom88mdz_pcm_size_tbl[Mucom88mdz_ReqWork.pcm_req_no];

		ap = (u8 *)PCM_END_LOW_ADDR;
		*ap = (u8)(((pcm_size)&0x00ff));

		ap = (u8 *)PCM_END_HIGH_ADDR;
		*ap = (u8)((pcm_size & 0xff00) >> 8);

		// リクエスト
		u8 *ap = (u8 *)PCM_REQ_CMMAND;
		*ap = Mucom88mdz_ReqWork.pcm_req_command;

		ap = (u8 *)PCM_REQ_PARAM;
		*ap = (u8)0;
	}

	// z80 稼働
	z80_BusRelease();
}

//=============================================================================
//!@fn static void Mucom88mdz_CopyStatusZ80To68k()
//!@brief Z80から.mub/SE/PCMの状態を取得する
//!@todo 負荷軽減のためmasterに不要なものをDEBUGで囲む
//=============================================================================
void Mucom88mdz_CopyStatusZ80To68k()
{
	// Z80 停止
	z80_BusRequest();

	z80_BusRequestWait();

	u8 *ap = (u8 *)TIMER_B_CNT;
	Mucom88mdz_Z80State.z80_TimerBCnt = *ap;

	ap = (u8 *)VSYNC_CNT;
	Mucom88mdz_Z80State.z80_VSyncCnt = *ap;

	ap = (u8 *)SND_STATUS;
	Mucom88mdz_Z80State.z80_SndStatus = *ap;

	ap = (u8 *)FLG_ADR;
	Mucom88mdz_Z80State.z80_SCmdFLG = *ap;

	ap = (u8 *)LOOP_MUB;
	Mucom88mdz_Z80State.z80_LoopMubFlag = *ap;

	ap = (u8 *)MUB_LOOPED;
	Mucom88mdz_Z80State.z80_T_FLAG = *ap;

	// SEの演奏状態
	ap = (u8 *)SE_STATE_BIT;
	Mucom88mdz_Z80State.z80_SeStateBit = *ap;

	ap = (u8 *)DCSG_OCT_UNDER;
	Mucom88mdz_Z80State.z80_OctUnderNum = *ap;
	ap = (u8 *)DCSG_NOISEP;
	Mucom88mdz_Z80State.z80_NoiseNum = *ap;
	ap = (u8 *)DCSG_NOISEW;
	Mucom88mdz_Z80State.z80_NoisewNum = *ap;

	u32 tmp;

	u8 *dat_adr;
	dat_adr = (u8 *)CHDAT_A;

	for (int i = 0; i < CHANNEL_WK_NUM; i++)
	{

		tmp = *dat_adr;
		dat_adr++;
		tmp |= ((u32)(*dat_adr)) << 8;
		tmp |= Z80_RAM_START;
		ap = (u8 *)tmp;
		dat_adr++;

		Mucom88mdz_Z80State.chdat_wak[i].ctr = *(ap + 0);
		Mucom88mdz_Z80State.chdat_wak[i].volume = (*(ap + 6)) & 0x0f;
		Mucom88mdz_Z80State.chdat_wak[i].ftune = *(ap + 32);
		Mucom88mdz_Z80State.chdat_wak[i].state = *(ap + 31);
	}

	// PCM
	ap = (u8 *)PCM_STATUS;
	Mucom88mdz_Z80State.z80_PcmState = *ap;

	z80_BusRelease();
}

//*****************************************************************************
//*****************************************************************************
// Mub
//*****************************************************************************
//*****************************************************************************

//=============================================================================
//!@fn void Mucom88mdZ_SetMubReq(u8 mub_no)
//!@brief Mubの演奏リクエスト登録
//!@param[in] mub_no リクエストmubのID
//!@details .mubファイルから譜面の本体のアドレスと演奏リクエストを登録する
//=============================================================================
void Mucom88mdZ_SetMubReq(u8 mub_no)
{
	MUBHED mc68k_mub_head;

	// リクエストを登録
	Mucom88mdz_ReqWork.mub_req_flag = 1;
	Mucom88mdz_ReqWork.mub_req_no = mub_no;

	// 譜面本体のアドレスを取得
	MUBHED *mubhead = (MUBHED *)Mucom88mdz_mub_tbl[mub_no];
	mub_struct_flip(mubhead, &mc68k_mub_head);
	u8 *mubp = (u8 *)mubhead;
	mubp = mubp + mc68k_mub_head.dataoffset;

	Mucom88mdz_ReqWork.mub_bank = (u32)mubp;
	Mucom88mdz_ReqWork.mub_req_command = MUB_REQ_START;
	Mucom88mdz_ReqWork.mub_req_param = 0; // 用途未定
}

//=============================================================================
//!@fn void Mucom88mdZ_SetMubStop()
//!@brief Mubの演奏停止クエスト登録
//!@details 
//=============================================================================
void Mucom88mdZ_SetMubStop()
{
	// リクエストを登録
	Mucom88mdz_ReqWork.mub_req_flag = 1;
	Mucom88mdz_ReqWork.mub_req_command = MUB_REQ_STOP;
}

//=============================================================================
// Mubのリクエスト登録
//!@fn void Mucom88mdZ_SetMubFadeOut()
//!@brief BGM(.mub)のフェードアウトリクエスト
//!@details 
//=============================================================================
void Mucom88mdZ_SetMubFadeOut()
{
	// リクエストを登録
	Mucom88mdz_ReqWork.mub_req_flag = 1;

	Mucom88mdz_ReqWork.mub_req_command = MUB_REQ_FADEOUT;
}


//*****************************************************************************
//*****************************************************************************
// SE
//*****************************************************************************
//*****************************************************************************

//=============================================================================
//!@fn void Mucom88mdZ_SetSeBank()
//!@brief SEデータのバンクをセットします。
//!@details Z80のSoundDriverをLoadしてから、1フレーム以上後に呼ぶこと
//!@details Z80のDriverが起動してメモリの初期化が終わってから呼ぶ必要がある
//=============================================================================
void Mucom88mdZ_SetSeBank()
{
	u8 *ap;

	// Z80停止
	Z80_requestBus(TRUE);
	z80_BusRequestWait();

	//    u32 mub_adrs = 0; // mubのアドレス
	ap = (u8 *)SE_LOW_ADDR;
	*ap = (u8)((u32)(&Mucom88mdzSeTbl) & 0x000000ff);

	ap = (u8 *)SE_MID_ADDR;
	*ap = (u8)(((u32)(&Mucom88mdzSeTbl) & 0x0000ff00) >> 8);

	ap = (u8 *)SE_HIGH_ADDR;
	*ap = (u8)(((u32)(&Mucom88mdzSeTbl) & 0x00ff0000) >> 16);

	Z80_releaseBus();
}

//=============================================================================
//!@fn void Mucom88mdz_SetSeReq(u8 se_no, u8 channel)
//!@brief 指定のSEを指定のCHANNELにリクエストする
//!@param[in] se_no リクエストSEのID
//!@param[in] channnel 0-2のDCSGチャンネルID
//!@details 
//=============================================================================
void Mucom88mdz_SetSeReq(u8 se_no, u8 channel)
{
	// エラーチェックを挟む
	Mucom88mdz_ReqWork.Mucom88mdz_SeReqArray[SE_REQ_COMMAND][channel] = SE_REQ_NML;
	Mucom88mdz_ReqWork.Mucom88mdz_SeReqArray[SE_REQ_NO][channel] = Mucom88mdz_SeIdTbl[se_no];
}

//=============================================================================
//!@fn void Mucom88mdz_SetSePackReq(u8 set_no)
//!@brief 指定のSEセットをまとめてリクエストする
//!@param[in] set_no リクエストSEセットのID
//!@details 3チャンネル分のリクエストSE_IDテーブルからリクエストの有るテーブルのリクエストを行う
//=============================================================================
void Mucom88mdz_SetSePackReq(u8 set_no)
{
	int i;
	for (i = 0; i < SE_CHANNEL_NUM; i++) // 3Chループ
	{
		if (Mucom88mdz_SeSetTbl[set_no].req_no[i] != MUB_SEID_NONE)
		{
			Mucom88mdz_SetSeReq(Mucom88mdz_SeSetTbl[set_no].req_no[i], i);
		}
	}
}

//=============================================================================
//!@fn void Mucom88mdz_SetSeStop(u8 channel)
//!@brief 指定のチャンネルのSEを止める
//!@param[in] channel 演奏を停止するSEチャンネル
//!@details 指定のチャンネルのSEを止める
//=============================================================================
void Mucom88mdz_SetSeStop(u8 channel)
{
	// エラーチェックを挟む
	Mucom88mdz_ReqWork.Mucom88mdz_SeReqArray[SE_REQ_COMMAND][channel] = SE_REQ_NML;
	Mucom88mdz_ReqWork.Mucom88mdz_SeReqArray[SE_REQ_NO][channel] = MUB_SEID_STOP;
}

//=============================================================================
//!@fn void Mucom88mdz_SetSeAllStop()
//!@brief 全てのチャンネルのSEを止める
//!@details 3チャンネルの停止リクエストを行う
//=============================================================================
void Mucom88mdz_SetSeAllStop()
{
	Mucom88mdz_SetSeStop(0);
	Mucom88mdz_SetSeStop(1);
	Mucom88mdz_SetSeStop(2);
}

//*****************************************************************************
//*****************************************************************************
// PCM
//*****************************************************************************
//*****************************************************************************

//=============================================================================
//!@fn void Mucom88mdZ_SetPcmReq(u8 no)
//!@brief PCM演奏リクエストをセットする
//!@param[in] no 演奏するPCM番号
//!@details 
//=============================================================================
void Mucom88mdZ_SetPcmReq(u8 no)
{
	Mucom88mdz_ReqWork.pcm_req_flag = 1;
	Mucom88mdz_ReqWork.pcm_req_no = no;
	Mucom88mdz_ReqWork.pcm_req_command = PCM_START;
}

//=============================================================================
//!@fn void Mucom88mdZ_SetPcmStop()
//!@brief PCM停止リクエストをセットする
//!@details 
//=============================================================================
void Mucom88mdZ_SetPcmStop()
{
	Mucom88mdz_ReqWork.pcm_req_flag = 1;
	Mucom88mdz_ReqWork.pcm_req_no = 0;
	Mucom88mdz_ReqWork.pcm_req_command = PCM_STOP;
}

//*****************************************************************************
//*****************************************************************************
// Subroutine
//*****************************************************************************
//*****************************************************************************

//=============================================================================
// Music演奏状態取得
// 0:STOP
// 1:PLAY
//=============================================================================
u8 Mucom88mdz_GetPlayState()
{
	return (Mucom88mdz_Z80State.z80_SndStatus);
}

//=============================================================================
//=============================================================================
u8 Mucom88mdz_GetLoopMubFlag()
{
	return( Mucom88mdz_Z80State.z80_LoopMubFlag);
}
//=============================================================================
// 曲がループしたら 1を返す
//=============================================================================
u8 Mucom88mdz_GetLooped()
{
	return (Mucom88mdz_Z80State.z80_T_FLAG);
}

//=============================================================================
// サブコマンド FLAGSET F09(FM/DCSG) の値を返す
//=============================================================================
u8 Mucom88mdz_GetFlag()
{
	return (Mucom88mdz_Z80State.z80_SCmdFLG);
}

//=============================================================================
// 各SEチャンネルの演奏状態を返す
// Ret:
//  演奏中なら1を返す
//=============================================================================
u8 Mucom88mdz_GetSeState()
{
	return (Mucom88mdz_Z80State.z80_SeStateBit);
}

//=============================================================================
// PCM演奏中なら1を返す
//=============================================================================
u8 Mucom88mdz_GetPcmState()
{
	return(Mucom88mdz_Z80State.z80_PcmState);
}

//-----------------------------------------------------------------------------
// Z80のエンディアンを反転させる
//-----------------------------------------------------------------------------
static void mub_struct_flip(MUBHED *src, MUBHED *dst)
{
	//	Memory Data structure
	dst->magic[0] = src->magic[0];
	dst->magic[1] = src->magic[1];
	dst->magic[2] = src->magic[2];
	dst->magic[3] = src->magic[3];

	dst->dataoffset = mub_flip32(src->dataoffset); // DATA offset
	dst->datasize = mub_flip32(src->datasize);	 // DATA size
	dst->tagdata = mub_flip32(src->tagdata);	   // TAG data file offset (0=none)
	dst->tagsize = mub_flip32(src->tagsize);	   // TAG data size
	dst->pcmdata = mub_flip32(src->pcmdata);	   // PCM data offset (0=none)
	dst->pcmsize = mub_flip32(src->pcmsize);	   // PCM data size
	dst->jumpcount = mub_flip32(src->jumpcount);   // Jump count (for skip)
	dst->jumpline = mub_flip32(src->jumpline);	 // Jump line number (for skip)

	//	Extend data (2.0 Header)
	//
	dst->ext_flags = mub_flip16(src->ext_flags); // Option flags ( MUCOM_FLAG_* )

	dst->ext_system = src->ext_system;						 // build system ( MUCOM_SYSTEM_* )
	dst->ext_target = src->ext_target;						 // playback target ( MUCOM_TARGET_* )
	dst->ext_channel_num = mub_flip16(src->ext_channel_num); // Max channel table num
	dst->ext_fmvoice_num = mub_flip16(src->ext_fmvoice_num); // internal FM voice table num

	dst->ext_player = mub_flip32(src->ext_player); // external player option
	dst->pad1 = mub_flip32(src->pad1);			   // not use (reserved)

	int i;
	for (i = 0; i < MUCOM_FMVOICE_MAX; i++)
	{
		dst->ext_fmvoice[i] = src->ext_fmvoice[i]; // FM voice no.(orginal) table
	}
}

//-----------------------------------------------------------------------------
// エンディアンの変換 16
static u16 mub_flip16(u16 num)
{
	u16 tmp;
	tmp = (num & 0x00ff) << 8;
	tmp |= (num & 0xff00) >> 8;
	return (tmp);
}

//-----------------------------------------------------------------------------
// エンディアンの変換 32
static u32 mub_flip32(u32 num)
{
	u32 param;

	param = (num & 0x000000ff) << 24;
	param |= (num & 0x0000ff00) << 8;
	param |= (num & 0x00ff0000) >> 8;
	param |= (num & 0xff000000) >> 24;
	return (param);
}

//-----------------------------------------------------------------------------
// Z80を停止する
//-----------------------------------------------------------------------------
inline static void z80_BusRequest()
{
	vu16 *pw_bus;
	// request bus (need to end reset)
	pw_bus = (u16 *)Z80_HALT_PORT;

	// take bus
	*pw_bus = 0x0100;
	// wait for bus taken
	while (*pw_bus & 0x0100);
}

//-----------------------------------------------------------------------------
// Z80の停止を確認
//-----------------------------------------------------------------------------
inline static void z80_BusRequestWait()
{
	u16 bus_taken;

	bus_taken = Z80_isBusTaken();
	while (!bus_taken)
	{
		bus_taken = Z80_isBusTaken();
	}
}

//-----------------------------------------------------------------------------
// Z80を再稼働
//-----------------------------------------------------------------------------
inline static void z80_BusRelease()
{
	vu16 *pw_bus;
	// request bus (need to end reset)
	pw_bus = (u16 *)Z80_HALT_PORT;

	// release bus
	*pw_bus = 0x0000;
}
