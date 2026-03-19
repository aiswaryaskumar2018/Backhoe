#include <gui/screen_pcodes_screen/Screen_pcodesView.hpp>
#include "Screen.h"
#include "POPUPMessages.h"

#define MAX_MSGS_ON_ROW 10
#define SPN_TXT_SIZE SPN1_SIZE
#define FMI_TXT_SIZE FMI1_SIZE
#define PCODE_TXT_SIZE PCODE1_SIZE

uint8_t PcodeScreenEnterStatus = FALSE;
DTCLookupTable PreLookUpptrPCODES;
DTCLookupTable *PcodeStartingAddr = NULL;
uint16_t* FaultArrayIndexSpnFmiAddr;
static uint8_t FirstEntry = TRUE;

/*************************************************/
typedef struct
{
	touchgfx::TextAreaWithOneWildcard *SPNPointer;
	touchgfx::TextAreaWithOneWildcard *FMIPointer;
	touchgfx::TextAreaWithOneWildcard *PCODEPointer;
}MyInvalidateType;

typedef struct
{
	touchgfx::Unicode::UnicodeChar *SPNPointer;
	touchgfx::Unicode::UnicodeChar *FMIPointer;
	touchgfx::Unicode::UnicodeChar *PCODEPointer;

}MyPrintType;
/***************************************************/


Screen_pcodesView::Screen_pcodesView()
{

}

void Screen_pcodesView::setupScreen()
{
    Screen_pcodesViewBase::setupScreen();
    PcodeScreenEnterStatus = TRUE;  // Use for only exit to home screen
    FirstEntry = TRUE;
    PcodeStartingAddr = GetPcodeSpnFmiChildScreen();
    FaultArrayIndexSpnFmiAddr = GetFaultArrayIndexSpnFmiIndexNo();

	Pcode_screen();  // Show PCODEs only once when entering

}

void Screen_pcodesView::tearDownScreen()
{
    Screen_pcodesViewBase::tearDownScreen();
}

void Screen_pcodesView::handleTickEvent()
{
	if(++Processounter >= 30u)
	{
		Processounter = 0;
		if( GetScreenStatus() == eHomeScreen )
		{
			exit_from_PCODES();
		}
	}
}

void Screen_pcodesView::Pcode_screen()
{

	static uint16_t OldFaultNumber = 0,NewFaultNumber = 0;
	int NoOfMsgs = 0;


	static MyInvalidateType MyInvalidateVar[MAX_MSGS_ON_ROW]={{&SPN1,&FMI1,&PCODE1},{&SPN2,&FMI2,&PCODE2},{&SPN3,&FMI3,&PCODE3},{&SPN4,&FMI4,&PCODE4},{&SPN5,&FMI5,&PCODE5},
             {&SPN6,&FMI6,&PCODE6},{&SPN7,&FMI7,&PCODE7},{&SPN8,&FMI8,&PCODE8},{&SPN9,&FMI9,&PCODE9},{&SPN10,&FMI10,&PCODE10}};

	static MyPrintType MyPrintVar[MAX_MSGS_ON_ROW] = {{&SPN1Buffer[0],&FMI1Buffer[0],&PCODE1Buffer[0]},{&SPN2Buffer[0],&FMI2Buffer[0],&PCODE2Buffer[0]},
	                                                  {&SPN3Buffer[0],&FMI3Buffer[0],&PCODE3Buffer[0]},{&SPN4Buffer[0],&FMI4Buffer[0],&PCODE4Buffer[0]},
													  {&SPN5Buffer[0],&FMI5Buffer[0],&PCODE5Buffer[0]},{&SPN6Buffer[0],&FMI6Buffer[0],&PCODE6Buffer[0]},
													  {&SPN7Buffer[0],&FMI7Buffer[0],&PCODE7Buffer[0]},{&SPN8Buffer[0],&FMI8Buffer[0],&PCODE8Buffer[0]},
													  {&SPN9Buffer[0],&FMI9Buffer[0],&PCODE9Buffer[0]},{&SPN10Buffer[0],&FMI10Buffer[0],&PCODE10Buffer[0]},
	                                                  };

	pcode_up_arrow.setVisible(false);
	pcode_up_arrow.invalidate();
	pcode_down_arrow.setVisible(false);
	pcode_down_arrow.invalidate();

	NewFaultNumber = getFaultNumber();
	if(NewFaultNumber > 10u)
	{
		NewFaultNumber = 10u;
	}

	if (FirstEntry == TRUE)
	{
		if( ((PcodeStartingAddr + (*(FaultArrayIndexSpnFmiAddr + NoOfMsgs)))->SPN) == 0u
		    				&& ((PcodeStartingAddr + (*(FaultArrayIndexSpnFmiAddr + NoOfMsgs)))->FMI) == 0u)
		{
			for (int idx = 0u; idx < NewFaultNumber; idx++)
			{
				MyInvalidateVar[idx].SPNPointer->setVisible(false);
				MyInvalidateVar[idx].SPNPointer->invalidate();
				MyInvalidateVar[idx].FMIPointer->setVisible(false);
				MyInvalidateVar[idx].FMIPointer->invalidate();
				MyInvalidateVar[idx].PCODEPointer->setVisible(false);
				MyInvalidateVar[idx].PCODEPointer->invalidate();
			}
		}
		else
		{
			for (int idx = 0u; idx < NewFaultNumber; idx++)
			{
				MyInvalidateVar[idx].SPNPointer->setVisible(true);
				MyInvalidateVar[idx].SPNPointer->invalidate();
				MyInvalidateVar[idx].FMIPointer->setVisible(true);
				MyInvalidateVar[idx].FMIPointer->invalidate();
				MyInvalidateVar[idx].PCODEPointer->setVisible(true);
				MyInvalidateVar[idx].PCODEPointer->invalidate();
			}
			Unicode::snprintf((MyPrintVar[NoOfMsgs].SPNPointer), SPN_TXT_SIZE, "%u", ((PcodeStartingAddr + (*(FaultArrayIndexSpnFmiAddr + NoOfMsgs)))->SPN));
			MyInvalidateVar[NoOfMsgs].SPNPointer->invalidate();
			Unicode::snprintf((MyPrintVar[NoOfMsgs].FMIPointer), FMI_TXT_SIZE, "%u",((PcodeStartingAddr + (*(FaultArrayIndexSpnFmiAddr + NoOfMsgs)))->FMI));
			MyInvalidateVar[NoOfMsgs].FMIPointer->invalidate();
			Unicode::strncpy((MyPrintVar[NoOfMsgs].PCODEPointer),(const char *)((PcodeStartingAddr + (*(FaultArrayIndexSpnFmiAddr + NoOfMsgs)))->DTCTxts), PCODE_TXT_SIZE);
			MyInvalidateVar[NoOfMsgs].PCODEPointer->invalidate();
		}
		FirstEntry = FALSE;
	}


	if((NewFaultNumber > 0u) || GetSinglePacketPcodeMatchingStatus())
	{
		for(NoOfMsgs = 0u; ( (NoOfMsgs < NewFaultNumber)  &&  (NoOfMsgs < 10)); NoOfMsgs++)
		{
    		if( ((PcodeStartingAddr + (*(FaultArrayIndexSpnFmiAddr + NoOfMsgs)))->SPN) == 0u
    				&& ((PcodeStartingAddr + (*(FaultArrayIndexSpnFmiAddr + NoOfMsgs)))->FMI) == 0u)
    		{
    			for (int idx = 0u; idx < NewFaultNumber; idx++)
    			{
					MyInvalidateVar[idx].SPNPointer->setVisible(false);
					MyInvalidateVar[idx].SPNPointer->invalidate();
					MyInvalidateVar[idx].FMIPointer->setVisible(false);
					MyInvalidateVar[idx].FMIPointer->invalidate();
					MyInvalidateVar[idx].PCODEPointer->setVisible(false);
					MyInvalidateVar[idx].PCODEPointer->invalidate();
    			}
    		}
    		else
    		{
    			for (int idx = 0u; idx < NewFaultNumber; idx++)
    			{
    				MyInvalidateVar[idx].SPNPointer->setVisible(true);
    				MyInvalidateVar[idx].SPNPointer->invalidate();
    				MyInvalidateVar[idx].FMIPointer->setVisible(true);
    				MyInvalidateVar[idx].FMIPointer->invalidate();
    				MyInvalidateVar[idx].PCODEPointer->setVisible(true);
    				MyInvalidateVar[idx].PCODEPointer->invalidate();
    			}
    			Unicode::snprintf((MyPrintVar[NoOfMsgs].SPNPointer), SPN_TXT_SIZE, "%u", ((PcodeStartingAddr + (*(FaultArrayIndexSpnFmiAddr + NoOfMsgs)))->SPN));
    			MyInvalidateVar[NoOfMsgs].SPNPointer->invalidate();
    			Unicode::snprintf((MyPrintVar[NoOfMsgs].FMIPointer), FMI_TXT_SIZE, "%u",((PcodeStartingAddr + (*(FaultArrayIndexSpnFmiAddr + NoOfMsgs)))->FMI));
    			MyInvalidateVar[NoOfMsgs].FMIPointer->invalidate();
    			Unicode::strncpy((MyPrintVar[NoOfMsgs].PCODEPointer),(const char *)((PcodeStartingAddr + (*(FaultArrayIndexSpnFmiAddr + NoOfMsgs)))->DTCTxts), PCODE_TXT_SIZE);
    			MyInvalidateVar[NoOfMsgs].PCODEPointer->invalidate();
    		}
		}
	}
	else
	{
		for (int idx = 0u; idx < 10u; idx++)
		{
			MyInvalidateVar[idx].SPNPointer->setVisible(false);
			MyInvalidateVar[idx].SPNPointer->invalidate();
			MyInvalidateVar[idx].FMIPointer->setVisible(false);
			MyInvalidateVar[idx].FMIPointer->invalidate();
			MyInvalidateVar[idx].PCODEPointer->setVisible(false);
			MyInvalidateVar[idx].PCODEPointer->invalidate();
		}
	}

	if( GetScreenStatus() == eHomeScreen )
	{
		exit_from_PCODES();
	}

}

uint8_t GetPcodeScreenEnterStatus(void)
{
	return  PcodeScreenEnterStatus;
}

void SetPcodeScreenEnterStatus(uint8_t Status)
{
	PcodeScreenEnterStatus = Status;
}

