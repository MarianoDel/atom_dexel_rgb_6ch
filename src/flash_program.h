#ifndef FLASH_PROGRAM_H_
#define FLASH_PROGRAM_H_

//#include "main.h"
//typedef unsigned int uint32_t;
#include <stdint.h>
#include "stm32f0xx.h"

// Define the STM32F10x FLASH Page Size depending on the used STM32 device
// si es mayor a 128K la pagina es de 2KB, sino 1KB

//#define FLASH_PAGE_SIZE    ((uint16_t)0x400)
#define FLASH_PAGE_SIZE			1024
#define FLASH_PAGE_SIZE_DIV2	512
#define FLASH_PAGE_SIZE_DIV4	256

#define PAGE28_START	0
#define PAGE29_START	256
#define PAGE30_START	512
#define PAGE31_START	768
#define PAGE31_END		1023

#define PAGE27			((uint32_t)0x08006C00)
#define PAGE28			((uint32_t)0x08007000)
#define PAGE29			((uint32_t)0x08007400)
#define PAGE30			((uint32_t)0x08007800)
#define PAGE31			((uint32_t)0x08007C00)

typedef union mem_bkp {
		unsigned int v_bkp [FLASH_PAGE_SIZE_DIV4];
		unsigned char v_bkp_8u [FLASH_PAGE_SIZE];
		unsigned short v_bkp_16u [FLASH_PAGE_SIZE_DIV2];
} mem_bkp_typedef;

typedef struct parameters {

	unsigned char last_function_in_flash;

	unsigned char last_program_in_flash;
	unsigned char last_program_deep_in_flash;
	unsigned char dummy;	//corrige desplazamiento de memoria

	unsigned short last_channel_in_flash;
	unsigned char dummy1;	//corrige desplazamiento de memoria
	unsigned char dummy2;	//corrige desplazamiento de memoria

	//agregados para RGB_FOR_CAT
	unsigned char pwm_channel_1;
	unsigned char pwm_channel_2;
	unsigned char pwm_channel_3;
	unsigned char pwm_channel_4;

} parameters_typedef;

typedef struct filesystem {

	//para num0
	unsigned int posi0;
	unsigned int lenght0;
	//para num1
	unsigned int posi1;
	unsigned int lenght1;
	//para num2
	unsigned int posi2;
	unsigned int lenght2;
	//para num3
	unsigned int posi3;
	unsigned int lenght3;
	//para num4
	unsigned int posi4;
	unsigned int lenght4;
	//para num5
	unsigned int posi5;
	unsigned int lenght5;
	//para num6
	unsigned int posi6;
	unsigned int lenght6;
	//para num7
	unsigned int posi7;
	unsigned int lenght7;
	//para num8
	unsigned int posi8;
	unsigned int lenght8;
	//para num9
	unsigned int posi9;
	unsigned int lenght9;

} filesystem_typedef;


typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

typedef enum
{
  FLASH_BUSY = 1,
  FLASH_ERROR_WRP,
  FLASH_ERROR_PROGRAM,
  FLASH_COMPLETE,
  FLASH_TIMEOUT
}FLASH_Status;

#define FLASH_ER_PRG_TIMEOUT         ((uint32_t)0x000B0000)
#define FLASH_FLAG_BSY                 FLASH_SR_BSY     /*!< FLASH Busy flag */
#define FLASH_FLAG_WRPERR              FLASH_SR_WRPERR  /*!< FLASH Write protected error flag */

//-------- Exported Functions -------------
unsigned char WriteConfigurations (void);
unsigned char WriteFlash(unsigned int * p, uint32_t p_addr, unsigned char with_lock, unsigned char len_in_4);

//-------- Private functions --------------
// void ErasePage(uint32_t , unsigned char );
//
//
// unsigned char ReadMem (void);
// unsigned char EraseAllMemory_FLASH(void);
// void BackupPage(unsigned int *, unsigned int *);
// void ErasePage(uint32_t , unsigned char );
// unsigned char UpdateNewCode(unsigned int *, unsigned short, unsigned int);
// unsigned char WritePage(unsigned int *, uint32_t, unsigned char);
// unsigned char Write_Code_To_Memory_FLASH(unsigned short, unsigned int);
//
// void LoadFilesIndex (void);
// void UpdateFileIndex (unsigned char, unsigned int, unsigned int);
// void SaveFilesIndex (void);
// void Load16SamplesShort (unsigned short *, unsigned int);
// void Load16SamplesChar (unsigned char *, unsigned int);
// void ShowFileSystem(void);
// void LoadConfiguration (void);
// void ShowConfiguration (void);



#endif
