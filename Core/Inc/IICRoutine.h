/*
 * IICRutine.h
 *
 *  Created on: Feb 14, 2025
 *      Author: dharmendra.singh
 */

#ifndef INC_IICROUTINE_H_
#define INC_IICROUTINE_H_


#include "main.h"

extern IWDG_HandleTypeDef hiwdg;


void WriteByteEEPROM(unsigned int Address, unsigned char Da);
void WritePageEEPROM(unsigned int Address, unsigned char *Da, char NBytes);

void write_long(unsigned int add,unsigned long data);
unsigned long read_long(unsigned int add);
unsigned int read_int(unsigned int add);
void write_int(unsigned int add,unsigned int data);
unsigned char	ReadByteEEPROM(unsigned int Address);
void ReadPageEEPROM(unsigned int Address, unsigned char *Da, char NBytes);
void WriteByteRTC(unsigned char A, unsigned char D);
unsigned char	ReadByteRTC(unsigned char Address);

void		PollAck(void);
void 		SendOneByteIIC(unsigned char d);
void 		StartBit(void);
void 		StopBit(void);
char 		SlaveAck(void);
void 		SlaveNoAck(void);
void 		IICClockDelay(void);
void 		Set_SDA_As_Output(void);
void 		Set_SDA_As_Input(void);
unsigned char 	ReadOneByteIIC(void);


#endif /* INC_IICROUTINE_H_ */
