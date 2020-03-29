//*****************************************************************************
//! @file mucom88mdz.h
//! @author KuSunda
//*****************************************************************************
#ifndef _MUCOM88MDZ_H_
#define _MUCOM88MDZ_H_

#include <genesis.h>

//*****************************************************************************
// Defeine
//*****************************************************************************

// ---------------------------------------------------------------------------
// Z80のRAMをアクセするアドレス
// ---------------------------------------------------------------------------
// z80_ctrl_h
#define Z80_WORK_RAM_ADDR (0x1e00)
#define WORK_RAM_ADDR (Z80_RAM_START | Z80_WORK_RAM_ADDR) //    ; WORK（64Kと通信できるようにアドレスを管理）

// ---------------------------------------------------------------------------
// MD Work
// 64Kとの通信
// ---------------------------------------------------------------------------
#define REQ_CMMAND (WORK_RAM_ADDR)		   // 1byte 68k との通信 $1a00
#define REQ_PARAM (WORK_RAM_ADDR + 1)	  //  68k との通信 $1a01

#define REQ_EFCT1 (WORK_RAM_ADDR + 2)	   	// 
#define REQ_EFCT_PARAM1 (WORK_RAM_ADDR + 3) // 
#define REQ_EFCT2 (WORK_RAM_ADDR + 4)	   	// 
#define REQ_EFCT_PARAM2 (WORK_RAM_ADDR + 5) // 
#define REQ_EFCT3 (WORK_RAM_ADDR + 6)	   	// 
#define REQ_EFCT_PARAM3 (WORK_RAM_ADDR + 7) // 


#define VSYNC_CNT (WORK_RAM_ADDR + 0x10)   //      ; 1 byte VSyncの数(Z80が動いているか？)
#define SND_STATUS (WORK_RAM_ADDR + 0x11) //  ; 演奏状況
#define TIMER_B_CNT (WORK_RAM_ADDR + 0x12) //       ; 1 byte Timer-Bの処理回数(PL_SNDが動いているか？)

#define PCM_REQ_CMMAND	(WORK_RAM_ADDR+0x13)	// PCMリクエスト
#define PCM_REQ_PARAM	(WORK_RAM_ADDR+0x14)	// 


// バンク切り替え
#define MUB_LOW_ADDR (WORK_RAM_ADDR + 0x1d) // .mub + data_offsetのアドレス
#define MUB_MID_ADDR (WORK_RAM_ADDR + 0x1e)
#define MUB_HIGH_ADDR (WORK_RAM_ADDR + 0x1f)

#define SE_LOW_ADDR (WORK_RAM_ADDR + 0x20) // SE
#define SE_MID_ADDR (WORK_RAM_ADDR + 0x21)
#define SE_HIGH_ADDR (WORK_RAM_ADDR + 0x22)

#define PCM_LOW_ADDR (WORK_RAM_ADDR + 0x23) // PCM
#define PCM_MID_ADDR (WORK_RAM_ADDR + 0x24)
#define PCM_HIGH_ADDR (WORK_RAM_ADDR + 0x25)

// PCMリクエスト情報
#define PCM_START_LOW_ADDR (WORK_RAM_ADDR + 0x26) // PCM
#define PCM_START_HIGH_ADDR (WORK_RAM_ADDR + 0x27)

#define PCM_END_LOW_ADDR (WORK_RAM_ADDR + 0x28) // PCM
#define PCM_END_HIGH_ADDR (WORK_RAM_ADDR + 0x29)

#define	PCM_STATUS	(WORK_RAM_ADDR + 0x2a)

// CUE
#define	CUE_REQ_RCV	(WORK_RAM_ADDR + 0x2b)

// .mubに埋め込むフラグの検出
#define FLG_ADR			(WORK_RAM_ADDR + 0x40)		// ; F9  サブコマンドで使用(曲からフラグを立てる)

#define LOOP_MUB		(WORK_RAM_ADDR + 0x41)		// != 0 ならループの曲(リクエストの次のフレームから有効)
#define MUB_LOOPED		(WORK_RAM_ADDR + 0x42)		// 1回LOOPした
#define SE_STATE_BIT	(WORK_RAM_ADDR + 0x43)		// 1byte SSG3

#define DCSG_OCT_UNDER	(WORK_RAM_ADDR + 0x44)
#define DCSG_NOISEP		(WORK_RAM_ADDR + 0x45)
#define DCSG_NOISEW		(WORK_RAM_ADDR + 0x46)

// ---------------------------------------------------------------------------
// チャンネルの情報 Channelワーク
enum{
	CH_FM1 = 0,
	CH_FM2,
	CH_FM3,
	CH_SSG1,
	CH_SSG2,
	CH_SSG3,
	CH_RYTM,
	CH_FM4,
	CH_FM5,
	CH_FM6,
	CH_PCM,

	CH_SE_SSG1,
	CH_SE_SSG2,
	CH_SE_SSG3,
	
	CHANNEL_WK_NUM
};

// チャンネル毎の情報
typedef struct muc88_chdat
{
	u8 ctr;
	u8 volume;
	u8 ftune;

	u8 state;		// IX+31 bit6 keyoff

	u8 mix_mode;	// DCSG Pコマンド(F Ch)
	u8 w_param;		// DCSG wコマンド(F Ch)

	/* data */
}MUC88_CHDAT;

#define CHDAT_A    (WORK_RAM_ADDR+0x60)          // 1byte FM1
#define CHDAT_B    (WORK_RAM_ADDR+0x62)          // 1byte FM2
#define CHDAT_C    (WORK_RAM_ADDR+0x64)          // 1byte FM3

#define CHDAT_D    (WORK_RAM_ADDR+0x66)          // 1byte SSG1
#define CHDAT_E    (WORK_RAM_ADDR+0x68)          // 1byte SSG2
#define CHDAT_F    (WORK_RAM_ADDR+0x6a)          // 1byte SSG3

#define CHDAT_G    (WORK_RAM_ADDR+0x6c)          // 1byte FM4

#define CHDAT_H    (WORK_RAM_ADDR+0x6e)          // 1byte FM4
#define CHDAT_I    (WORK_RAM_ADDR+0x70)          // 1byte FM5
#define CHDAT_J    (WORK_RAM_ADDR+0x72)          // 1byte FM6

#define CHDAT_K    (WORK_RAM_ADDR+0x74)          // 1byte FM6

#define CHDAT_D2   (WORK_RAM_ADDR+0x76)          // 1byte SSG1
#define CHDAT_E2   (WORK_RAM_ADDR+0x78)          // 1byte SSG2
#define CHDAT_F2   (WORK_RAM_ADDR+0x7a)          // 1byte SSG3

// ---------------------------------------------------------------------------
// Command
// ---------------------------------------------------------------------------

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Musicリクエスト
#define MUB_REQ_NONE	(0x00)
#define MUB_REQ_START	(0x01)
#define MUB_REQ_STOP	(0x02)
#define MUB_REQ_FADEOUT	(0x03)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// SE リクエスト
#define SE_REQ_NONE		(0x00)
#define SE_REQ_NML		(0x01)
#define SE_REQ_TYPE1	(0x02)		// 未使用
#define SE_REQ_TYPE2	(0x03)		// 未使用

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define PCM_NONE	(0x00)
#define PCM_START	(0x01)
#define PCM_STOP	(0x02)

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
#define MUCOM_FMVOICE_MAX (32)

// ---------------------------------------------------------------------------
// .mubファイルのヘッダー
// @note mubはリトルエンディアンのデータなので、ワードデータの扱いに注意
// ---------------------------------------------------------------------------
typedef struct _MUBHED
{
	//	Memory Data structure
	u8 magic[4];	//  = new byte[4]; // magic number
	u32 dataoffset; // DATA offset
	u32 datasize;   // DATA size
	u32 tagdata;	// TAG data file offset (0=none)
	u32 tagsize;	// TAG data size
	u32 pcmdata;	// PCM data offset (0=none)
	u32 pcmsize;	// PCM data size
	u32 jumpcount;  // Jump count (for skip)
	u32 jumpline;   // Jump line number (for skip)

	//	Extend data (2.0 Header)
	//
	u16 ext_flags;		 // Option flags ( MUCOM_FLAG_* )
	u8 ext_system;		 // build system ( MUCOM_SYSTEM_* )
	u8 ext_target;		 // playback target ( MUCOM_TARGET_* )
	u16 ext_channel_num; // Max channel table num
	u16 ext_fmvoice_num; // internal FM voice table num

	u32 ext_player;					   // external player option
	u32 pad1;						   // not use (reserved)
	u8 ext_fmvoice[MUCOM_FMVOICE_MAX]; // FM voice no.(orginal) table
} MUBHED;

// ---------------------------------------------------------------------------
// 演奏状態のワーク
typedef struct _MUCOM_Z80STATE
{
	u8	z80_TimerBCnt;		// (u8 *)TIMER_B_CNT;
	u8	z80_VSyncCnt;		// (u8 *)VSYNC_CNT;
	u8	z80_SndStatus;		// (u8 *)SND_STATUS;	STOP/PLAY
	u8	z80_SCmdFLG;		// (u8 *)FLG_ADR;		サブコマンドのFLAG
	u8	z80_LoopMubFlag;	// (u8 *)LoopMubFlag;	!= 0 でLoop曲
	u8	z80_T_FLAG;			// (u8 *)T_FLAG;		1回ループした
	u8	z80_SeStateBit;		// (u8 *)SE_STATE_BIT;	SEの演奏状態

	u8	z80_OctUnderNum;	// (u8 *)DCSG_OCT_UNDER;
	u8	z80_NoiseNum;		// (u8 *)DCSG_NOISE;
	u8	z80_NoisewNum;		// (u8 *)DCSG_NOISEW
	u8	z80_PcmState;		// (u8 *)

	MUC88_CHDAT chdat_wak[CHANNEL_WK_NUM];
}MUCOM_Z80STATE;

// ---------------------------------------------------------------------------
// SE関係
enum{
	SE_REQ_COMMAND = 0,
	SE_REQ_NO,

	SE_REQ_ARRAY_NUM,
};
enum{
	SE_CHANNEL_ID1 = 0,
	SE_CHANNEL_ID2,
	SE_CHANNEL_ID3,

	SE_CHANNEL_NUM
};

#define MUB_SEID_NONE    (0xff)
#define MUB_SEID_STOP    (0xff)


typedef struct _SE_REQ_TBL{
//	u8	type[SE_CHANNEL_NUM];		// SE_REQ_NML / SE_REQ_SYS
	u8	req_no[SE_CHANNEL_NUM];	// 0xff none
}SE_REQ_TBL;

// ---------------------------------------------------------------------------
// リクエスト管理
// Z80に転送するリクエストデータを1つの構造体で管理して纏めて転送する
// ---------------------------------------------------------------------------
typedef struct _MUCOM88MDZ_REQ_WORK{
	// mub
	u8	mub_req_flag;
	u8	mub_req_no;
	u32	mub_bank;
	u8	mub_req_command;
	u8	mub_req_param;

	// SE
	u32	se_bank;
	u8	se_req_flag[SE_CHANNEL_NUM];
	u8	Mucom88mdz_SeReqArray[SE_REQ_ARRAY_NUM][SE_CHANNEL_NUM];

	// pcm
	u8	pcm_req_flag;
	u8	pcm_req_no;

	u32	pcm_bank;
	u8	pcm_req_command;			// Play/Stop
	u8	pcm_req_param;
	u16	start_adrs;
	u16	pcm_size;

	// Debug
	u8	cue;

}MUCOM88MDZ_REQ_WORK;


//*****************************************************************************
// Extern 
//*****************************************************************************
extern void Mucom88mdz_Driverload();
extern void Mucom88mdz_Init();
extern void Mucom88mdz_FrameInit();
extern void Mucom88mdz_CopyStatus68kToZ80();
extern void Mucom88mdz_CopyStatusZ80To68k();
extern void Mucom88mdZ_SetMubReq(u8 mub_no);
extern void Mucom88mdZ_SetMubStop();
extern void Mucom88mdZ_SetMubFadeOut();
extern void Mucom88mdZ_SetSeBank();
extern void Mucom88mdz_SetSeReq(u8 se_no, u8 channel);
extern void Mucom88mdz_SetSePackReq(u8 set_no);
extern void Mucom88mdz_SetSeStop(u8 channel);
extern void Mucom88mdz_SetSeAllStop();
extern void Mucom88mdZ_SetPcmReq(u8 no);
extern void Mucom88mdZ_SetPcmStop();
extern void Mucom88mdZ_DebugQueReq(u8 param);
extern u8 Mucom88mdz_GetPlayState();
extern u8 Mucom88mdz_GetLoopMubFlag();
extern u8 Mucom88mdz_GetLooped();
extern u8 Mucom88mdz_GetFlag();
extern u8 Mucom88mdz_GetSeState();
extern u8 Mucom88mdz_GetPcmState();

//*****************************************************************************
// E n d o f F i l e
//*****************************************************************************
#endif