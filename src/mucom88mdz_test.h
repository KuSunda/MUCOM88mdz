//*****************************************************************************
//*****************************************************************************
#ifndef _MUCOM88MDZ_TEST_H_
#define _MUCOM88MDZ_TEST_H_
#include <genesis.h>

//*****************************************************************************
// Defein
//*****************************************************************************
// Test モードを使うか？
#define	USE_MUCOM_TESTMODE			(1)

#define JOY_PORT_NUM    (1)
#define MD_JOY_NUM      (JOY_NUM)

#define P1 (0)
#define P2 (1)
#define P3 (2)
#define P4 (3)


#define JOY_PORT_1		(PORT_1)
#define JOY_PORT_2		(PORT_2)

// 簡易定義
#define UP		(BUTTON_UP)
#define DOWN	(BUTTON_DOWN)
#define LEFT	(BUTTON_LEFT)
#define RIGHT	(BUTTON_RIGHT)
#define SHOT_A	(BUTTON_A)
#define SHOT_B	(BUTTON_B)
#define SHOT_C	(BUTTON_C)
#define START	(BUTTON_START)
#define SHOT_X	(BUTTON_X)
#define SHOT_Y	(BUTTON_Y)
#define SHOT_Z	(BUTTON_Z)
#define MODE	(BUTTON_MODE)

//*****************************************************************************
// Struct
//*****************************************************************************
#define CAN_USE_JOYPORT_NUM (2)

typedef struct {
    u16 joytype;
    u16 porttype;
    u16 old;
    u16 now;
    u16 on;
}JOY_WORK;

//*****************************************************************************
// Extern
//*****************************************************************************
extern void Mucom88mdz_TestInit();
extern void Mucom88mdz_TestUpdate();

extern void JoySystemInitilize();
extern void JoyDataUpdate();

#endif