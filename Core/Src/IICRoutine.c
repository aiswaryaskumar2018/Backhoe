/**************************************************************************/
/*   IIC RTC and EEPROM Interface with RL78).	  */
/*				  					  */
/*   Routines for Reading and Writing to IIC Bus 			  */
/*									  */
/*   Developed by: Frontline Electronics Pvt Ltd., India.	          */
/*                 www.MightyMicons.com				          */
/**************************************************************************/
//#include "Header.h"



#include "IICRoutine.h"


#define		WRITE_C		0xA0 		//Command for write
#define		READ_C		0xA1		//Command for read
#define		ACK_READ	0xA1		//Command to get acknowledge







/* Write One Byte to EEPROM */
void WriteByteEEPROM(unsigned int Address, unsigned char Da)
{
	unsigned char	ID;
	unsigned char ADDR_LSB, ADDR_MSB;
	unsigned char *ptr;
        unsigned int a=0;
	//ptr = &Address;
	ADDR_LSB = Address;
	//ptr++;
	//ADDR_MSB = *ptr;
	
	if(Address>=256)
	ID = WRITE_C | 0x02;		//Command byte (ID)	
	else
	ID = WRITE_C;
	
	Address=Address-256;
	
	StartBit();					//Give start bit
	SendOneByteIIC(ID);			//Send the write command
	SlaveAck();					//Get the acknowledge from slave
//	SendOneByteIIC(ADDR_MSB);	//Send the address
//	SlaveAck();					//Get the acknowledge from slave
	SendOneByteIIC(ADDR_LSB);	//Send the address
	SlaveAck();					//Get the acknowledge from slave
	SendOneByteIIC(Da);			//Send the Data
	SlaveAck();					//Get the acknowledge from slave
	StopBit();					//Give stop bit
	PollAck();
        for(a=0;a<10000;a++)
	 {
        	HAL_IWDG_Refresh(&hiwdg);                                    // Give bite to dog
	 }
	 
}



/* Read One Byte from EEPROM */
unsigned char	ReadByteEEPROM(unsigned int Address)
{
	unsigned char	a;
	unsigned char	ID;
	unsigned char ADDR_LSB, ADDR_MSB;
	unsigned char *ptr;

	/*ptr = &Address;
	ADDR_LSB = *ptr;
	ptr++;
	ADDR_MSB = *ptr;
        */
	//ADDR_LSB=(unsigned char)Address;
	//ADDR_MSB=Address>>8;
	//ID = WRITE_C;	//Command byte (ID) 
	
	
        if(Address>=256)
	ID = WRITE_C | 0x02;		//Command byte (ID)	
	else
	ID = WRITE_C;
	
	//Address=Address-256;
	
	
	//Write Address
	StartBit();			//Give start bit
	SendOneByteIIC(ID);		//Send the write command
	SlaveAck();			//Get the acknowledge from slave
	//SendOneByteIIC(ADDR_MSB);	//Send the address
	//SlaveAck();			//Get the acknowledge from slave
	if(Address>=256)
	 SendOneByteIIC(Address-256);	//Send the address
	else
	 SendOneByteIIC(Address);	//Send the address
	
	SlaveAck();			//Get the acknowledge from slave
	
        if(Address>=256)
	ID = READ_C | 0x02;		//Command byte (ID)	
	else
	ID = READ_C;	
	//Read a byte
	ID = READ_C;		//Command byte (ID) for 24C04
	StartBit();			//Give start bit
	SendOneByteIIC(ID);		//Send the read command
	SlaveAck();			//Get the acknowledge from slave
	a=ReadOneByteIIC();		//Read the byte
	SlaveNoAck();
	StopBit();			//Give stop bit
	return (a);
}


/********************************************************************************
## Module:   	EEPROM.c
## Function: 	write_int()
## Description: Write two bytes data to AT24c02
## Parameter:	Add: the address to be written ;
				Data: 	 the pointer of datas to be written ;
				add:	 the address where data to be write
## Return��  	none
********************************************************************************/
 
void write_int(unsigned int add,unsigned int data)
{
unsigned char status=0;

//        status = WriteByteTo24c02(add+1,data);
//	   status = WriteByteTo24c02(add,data>>8);
         WriteByteEEPROM(add+1, data);
	 WriteByteEEPROM(add, data>>8);
        
}
/********************************************************************************
## Module:   	EEPROM.c
## Function: 	read_int()
## Description: Read two bytes data from AT24c02
## Parameter:	Add: the address to be read;
				
## Return��  	operation result
				(integer value) data on the eeprom at that address
********************************************************************************/
 
unsigned int read_int(unsigned int add)
{
unsigned char temp,status=0;
//unsigned long temp1; 
unsigned int hr_temp;
			//status = ReadByteFrom24c02(add,&temp);
			//unsigned char	ReadByteEEPROM(unsigned int Address)
			temp = ReadByteEEPROM(add);

                          hr_temp=temp;
                          hr_temp=hr_temp<<8;
                          //status = ReadByteFrom24c02(add+1,&temp);
			  temp = ReadByteEEPROM(add+1);
                          hr_temp=hr_temp | temp;
                       
           return(hr_temp);
}
/********************************************************************************
## Module:   	EEPROM.c
## Function: 	write_long()
## Description: Write three bytes data to AT24c02
## Parameter:	Add: the address where data to be write ;
		data:three bytes data to be write		
## Return��  	none
********************************************************************************/

void write_long(unsigned int add,unsigned long data)
{
unsigned char status=0;

       // status = WriteByteTo24c02(add+2,data);
	 //   status = WriteByteTo24c02(add+1,data>>8);
	//	 status = WriteByteTo24c02(add,data>>16);
	WriteByteEEPROM(add+2, data);
	WriteByteEEPROM(add+1, data>>8);
	WriteByteEEPROM(add, data>>16);
	         
        



   
 
 }
/********************************************************************************
## Module:   	EEPROM.c
## Function: 	read_long()
## Description: Read three bytes data from AT24c02
## Parameter:	Add: the address to be read;
				
## Return��  	operation result
				(long value) three bytes data on the eeprom at that address
********************************************************************************/

unsigned long read_long(unsigned int add)
{
unsigned char temp,status=0;
//unsigned long temp1; 
unsigned long hr_temp;
		//ReadByteFrom24c02(add,&temp);
		temp = ReadByteEEPROM(add);
                hr_temp=temp;
                hr_temp=hr_temp<<8;
               // ReadByteFrom24c02(add+1,&temp);
	       temp = ReadByteEEPROM(add+1);
		hr_temp=hr_temp | temp;
                                  hr_temp=hr_temp<<8;
                                  //status = ReadByteFrom24c02(add+2,&temp);
				  temp = ReadByteEEPROM(add+2);
			          hr_temp=hr_temp | temp;
                
                   return(hr_temp);
}






/* Write One Byte to RTC */
void WriteByteRTC(unsigned char A, unsigned char D)
{
	StartBit();			//Give start bit
	SendOneByteIIC(WRITE_C);	//Send the write command
	SlaveAck();			//Get the acknowledge from slave
	SendOneByteIIC(A);		//Send the address
	SlaveAck();			//Get the acknowledge from slave
	SendOneByteIIC(D);		//Send the Data
	SlaveAck();			//Get the acknowledge from slave
	StopBit();			//Give stop bit
	PollAck();
}

/* Read One Byte from RTC */
unsigned char	ReadByteRTC(unsigned char Address)
{
	unsigned char	a;
	//Write Address
	StartBit();				//Give start bit
	SendOneByteIIC(WRITE_C);		//Send the write command
	SlaveAck();				//Get the acknowledge from slave
	SendOneByteIIC(Address);		//Send the address
	SlaveAck();				//Get the acknowledge from slave
	//Read a byte
	StartBit();				//Give start bit
	SendOneByteIIC(READ_C);			//Send the read command
	SlaveAck();				//Get the acknowledge from slave
	a=ReadOneByteIIC();			//Read the byte
	SlaveNoAck();
	StopBit();				//Give stop bit
	return (a);
}


void	PollAck(void)
{
	char	a;
	for(a=0;a<0x40;a++)
	{
		StartBit();			//Give start bit
		SendOneByteIIC(ACK_READ);	//Send the read Acknowledgement command
		if(SlaveAck())			//Get the acknowledge from slave
		{	//acknowledge received
			break;
		}
	}
}


/* Send One Byte to RTc */
void SendOneByteIIC(unsigned char d)
{
	char	a;
	Set_SDA_As_Output();
	for(a=0;a<8;a++)
	{	//send 8 bits 
		//		IICClockDelay();		//delay
		if(d & (0x80 >> a))
		{
			HAL_GPIO_WritePin(GPIOD, I2C_SDA_PIN_Pin, GPIO_PIN_SET);	// SDA = 1;		//set SDA
		}
		else
		{
			HAL_GPIO_WritePin(GPIOD, I2C_SDA_PIN_Pin, GPIO_PIN_RESET);	//SDA = 0;		//clear SDA
		}
			
		HAL_GPIO_WritePin(GPIOB, I2C_SCL_PIN_Pin, GPIO_PIN_SET);	//SSCL	= 1;			//set SCL
		IICClockDelay();
		HAL_GPIO_WritePin(GPIOB, I2C_SCL_PIN_Pin, GPIO_PIN_RESET);	//SCL = 0;			//clear SCL
		IICClockDelay();			

	}
}


/* Send a start bit */
void StartBit(void)
{
	Set_SDA_As_Output();
	HAL_GPIO_WritePin(GPIOB, I2C_SCL_PIN_Pin, GPIO_PIN_SET);	//SCL = 1;
	IICClockDelay();
	HAL_GPIO_WritePin(GPIOD, I2C_SDA_PIN_Pin, GPIO_PIN_SET);	//SDA	= 1;
	IICClockDelay();
	HAL_GPIO_WritePin(GPIOD, I2C_SDA_PIN_Pin, GPIO_PIN_RESET);	//SDA	= 0;
	IICClockDelay();
	HAL_GPIO_WritePin(GPIOB, I2C_SCL_PIN_Pin, GPIO_PIN_RESET);	//SCL = 0;
	IICClockDelay();
}

/* Send a stop bit */
void StopBit(void)
{
	Set_SDA_As_Output();
	HAL_GPIO_WritePin(GPIOD, I2C_SDA_PIN_Pin, GPIO_PIN_RESET);	//SDA	= 0;
	IICClockDelay();
	HAL_GPIO_WritePin(GPIOB, I2C_SCL_PIN_Pin, GPIO_PIN_SET);	//SCL	= 1;
	IICClockDelay();
	HAL_GPIO_WritePin(GPIOD, I2C_SDA_PIN_Pin, GPIO_PIN_SET);	//SDA = 1;
}

/* Get acknowledge from slave */
char SlaveAck(void)
{
	char	a;
	Set_SDA_As_Output();
//	IICClockDelay();
	HAL_GPIO_WritePin(GPIOB, I2C_SCL_PIN_Pin, GPIO_PIN_RESET);	//SCL	= 0;
//	IICClockDelay();
	Set_SDA_As_Input();
	IICClockDelay();
	HAL_GPIO_WritePin(GPIOB, I2C_SCL_PIN_Pin, GPIO_PIN_SET);	//SCL = 1;
	IICClockDelay();

//	Set_SDA_As_Input();
//	IICClockDelay();
	a   = HAL_GPIO_ReadPin(GPIOD, I2C_SDA_PIN_Pin);	//;
//	IICClockDelay();
	HAL_GPIO_WritePin(GPIOB, I2C_SCL_PIN_Pin, GPIO_PIN_RESET);	//SCL = 0;
	IICClockDelay();
	return(a);
}


/* No acknowledge from slave */
void SlaveNoAck(void)
{
	Set_SDA_As_Output();
	HAL_GPIO_WritePin(GPIOD, I2C_SDA_PIN_Pin, GPIO_PIN_SET);	//SDA	= 1;
	IICClockDelay();
	HAL_GPIO_WritePin(GPIOB, I2C_SCL_PIN_Pin, GPIO_PIN_SET);	//SCL	= 1;
	IICClockDelay();
	HAL_GPIO_WritePin(GPIOB, I2C_SCL_PIN_Pin, GPIO_PIN_RESET);	//SCL = 0;
	IICClockDelay();
}



/* Delay */
void IICClockDelay(void)
{
	unsigned int	j;
	//for(a=0;a<3;a++);
	for(j=0;j<40;j++)
	{
		HAL_IWDG_Refresh(&hiwdg); // 1.5 sec WDT time();                                    // Give bite to dog
	}
}


/* Sets output mode to SDA line */
void Set_SDA_As_Output(void)
{
	SDA_PinChangeModeType(eSDA_OutPutMode);
	//SDA_Direction = 0;	//Set as output
	//SCL_Direction = 0;	//Set as output
}


/* Sets input mode to SDA line */
void Set_SDA_As_Input(void)
{
	SDA_PinChangeModeType(eSDA_inputMode);
	//SDA_Direction = 1;	//Set as input//
	//SCL_Direction = 0;	//Set as output
}

/* Read One Byte (bit by bit) from RTC */
unsigned char ReadOneByteIIC()
{
	char	a;
	unsigned char b;
	b=0;
	Set_SDA_As_Input();	//Set SDA line as I/P line
	for(a=0;a<8;a++)
	{
		HAL_GPIO_WritePin(GPIOB, I2C_SCL_PIN_Pin, GPIO_PIN_SET);	//SCL = 1;		// Set clock to one level
		IICClockDelay();
		IICClockDelay();
		
		if(HAL_GPIO_ReadPin(GPIOD, I2C_SDA_PIN_Pin) == 1u)		// if(SDA)input read
		{
			b |= (0x80 >> a);	//Set the data bit on SDA line
		}
//		IICClockDelay();
		HAL_GPIO_WritePin(GPIOB, I2C_SCL_PIN_Pin, GPIO_PIN_RESET);	//SCL	= 0;		// Set clock to zero level
		IICClockDelay();
	        IICClockDelay();
		
	}
	return(b);
}
