
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "PCIE.h"


#define DEMO_PCIE_USER_BAR			PCIE_BAR0
#define DEMO_PCIE_IO_LED_ADDR		0x00
/*#define DEMO_PCIE_IO_BUTTON_ADDR	0x20
#define DEMO_PCIE_FIFO_WRITE_ADDR	0x40
#define DEMO_PCIE_FIFO_STATUS_ADDR	0x60
#define DEMO_PCIE_FIFO_READ_ADDR	0x80*/
#define DEMO_PCIE_IO_SWITCH_ADDR	0xA0
//#define DEMO_PCIE_IO_SPI_ADDR	0xC0
#define DEMO_PCIE_MEM_ADDR			0x20000

//#define MEM_SIZE			(128*1024) //128KB
//#define FIFO_SIZE			(16*1024) // 2KBx8



typedef enum{
	MENU_LED = 0,
	//MENU_BUTTON,
	MENU_SWITCH,
	//MENU_SPI,
	//MENU_DMA_MEMORY,
	//MENU_DMA_FIFO,
	MENU_QUIT = 99
}MENU_ID;

void UI_ShowMenu(void){
	printf("===TEST===========================\r\n");
	printf("[%d]: Led control\r\n", MENU_LED);
	//printf("[%d]: Button Status Read\r\n", MENU_BUTTON);
	printf("[%d]: Switch Status Read\r\n", MENU_SWITCH);
	//printf("[%d]: SPI Status Read\r\n", MENU_SPI);
	//printf("[%d]: DMA Memory Test\r\n", MENU_DMA_MEMORY);
	//printf("[%d]: DMA Fifo Test\r\n", MENU_DMA_FIFO);
	printf("[%d]: Quit\r\n", MENU_QUIT);
	printf("Please input your selection:");
}

int UI_UserSelect(void){
	int nSel;
	scanf("%d",&nSel);
	return nSel;
}


BOOL TEST_LED(PCIE_HANDLE hPCIe){
	BOOL bPass;
	int	Mask;
	
	printf("Please input led conrol mask:");
	scanf("%d", &Mask);

	bPass = PCIE_Write32(hPCIe, DEMO_PCIE_USER_BAR, DEMO_PCIE_IO_LED_ADDR, (DWORD)Mask);
	if (bPass)
		printf("Led control success, mask=%xh\r\n", Mask);
	else
		printf("Led conrol failed\r\n");

	
	return bPass;
}
/*
BOOL TEST_BUTTON(PCIE_HANDLE hPCIe){
	BOOL bPass = TRUE;
	DWORD Status;

	bPass = PCIE_Read32(hPCIe, DEMO_PCIE_USER_BAR, DEMO_PCIE_IO_BUTTON_ADDR, &Status);
	if (bPass)
		printf("Button status mask=%xh\r\n", Status);
	else
		printf("Failed to read button status\r\n");

	
	return bPass;
}
*/
BOOL TEST_SWITCH(PCIE_HANDLE hPCIe){
	BOOL bPass = TRUE;
	DWORD Status;

	bPass = PCIE_Read32(hPCIe, DEMO_PCIE_USER_BAR, DEMO_PCIE_IO_SWITCH_ADDR, &Status);
	if (bPass)
		printf("Switch status mask=%xh\r\n", Status);
	else
		printf("Failed to read switch status\r\n");

	
	return bPass;
}
/*
BOOL TEST_SPI(PCIE_HANDLE hPCIe){
	BOOL bPass;
	int	Mask;
	
	printf("Please input SPI conrol mask:");
	scanf("%d", &Mask);

	bPass = PCIE_Write32(hPCIe, DEMO_PCIE_USER_BAR, DEMO_PCIE_IO_SPI_ADDR, (DWORD)Mask);
	if (bPass)
		printf("SPI control success, mask=%xh\r\n", Mask);
	else
		printf("SPI conrol failed\r\n");

	
	return bPass;
}
*/
/*
char PAT_GEN(int nIndex){
	char Data;
	Data = nIndex & 0xFF;
	return Data;
}

BOOL TEST_DMA_MEMORY(PCIE_HANDLE hPCIe){
	BOOL bPass=TRUE;
	int i;
	const int nTestSize = MEM_SIZE;
	const PCIE_LOCAL_ADDRESS LocalAddr = DEMO_PCIE_MEM_ADDR;
	char *pWrite;
	char *pRead;
	char szError[256];


	pWrite = (char *)malloc(nTestSize);
	pRead = (char *)malloc(nTestSize);
	if (!pWrite || !pRead){
		bPass = FALSE;
		sprintf(szError, "DMA Memory:malloc failed\r\n");
	}
	

	// init test pattern
	for(i=0;i<nTestSize && bPass;i++)
		*(pWrite+i) = PAT_GEN(i);

	// write test pattern
	if (bPass){
		bPass = PCIE_DmaWrite(hPCIe, LocalAddr, pWrite, nTestSize);
		if (!bPass)
			sprintf(szError, "DMA Memory:PCIE_DmaWrite failed\r\n");
	}		

	// read back test pattern and verify
	if (bPass){
		bPass = PCIE_DmaRead(hPCIe, LocalAddr, pRead, nTestSize);

		if (!bPass){
			sprintf(szError, "DMA Memory:PCIE_DmaRead failed\r\n");
		}else{
			for(i=0;i<nTestSize && bPass;i++){
				if (*(pRead+i) != PAT_GEN(i)){
					bPass = FALSE;
					sprintf(szError, "DMA Memory:Read-back verify unmatch, index = %d, read=%xh, expected=%xh\r\n", i, *(pRead+i), PAT_GEN(i));
				}
			}
		}
	}


	// free resource
	if (pWrite)
		free(pWrite);
	if (pRead)
		free(pRead);
	
	if (!bPass)
		printf("%s", szError);
	else
		printf("DMA-Memory (Size = %d byes) pass\r\n", nTestSize);


	return bPass;
}

BOOL TEST_DMA_FIFO(PCIE_HANDLE hPCIe){
	BOOL bPass=TRUE;
	int i;
	const int nTestSize = FIFO_SIZE;
	const PCIE_LOCAL_ADDRESS FifoID_Write = DEMO_PCIE_FIFO_WRITE_ADDR;
	const PCIE_LOCAL_ADDRESS FifoID_Read = DEMO_PCIE_FIFO_READ_ADDR;
	char *pBuff;
	char szError[256];


	pBuff = (char *)malloc(nTestSize);
	if (!pBuff){
		bPass = FALSE;
		sprintf(szError, "DMA Fifo: malloc failed\r\n");
	}
	

	// init test pattern
	if (bPass){
		for(i=0;i<nTestSize;i++)
			*(pBuff+i) = PAT_GEN(i);
	}

	// write test pattern into fifo
	if (bPass){
		bPass = PCIE_DmaFifoWrite(hPCIe, FifoID_Write, pBuff, nTestSize);
		if (!bPass)
			sprintf(szError, "DMA Fifo: PCIE_DmaFifoWrite failed\r\n");
	}		

	// read back test pattern and verify
	if (bPass){
		memset(pBuff, 0, nTestSize); // reset buffer content
		bPass = PCIE_DmaFifoRead(hPCIe, FifoID_Read, pBuff, nTestSize);

		if (!bPass){
			sprintf(szError, "DMA Fifo: PCIE_DmaFifoRead failed\r\n");
		}else{
			for(i=0;i<nTestSize && bPass;i++){
				if (*(pBuff+i) != PAT_GEN(i)){
					bPass = FALSE;
					sprintf(szError, "DMA Fifo: Read-back verify unmatch, index = %d, read=%xh, expected=%xh\r\n", i, *(pBuff+i), PAT_GEN(i));
				}
			}
		}
	}


	// free resource
	if (pBuff)
		free(pBuff);
	
	if (!bPass)
		printf("%s", szError);
	else
		printf("DMA-Fifo (Size = %d byes) pass\r\n", nTestSize);


	return bPass;
}

*/

int main(void)
{
	void *lib_handle;
	PCIE_HANDLE hPCIE;
	BOOL bQuit = FALSE;
	int nSel;

	printf("== Terasic: PCIe Demo Program ==\r\n");

	lib_handle = PCIE_Load();
	if (!lib_handle){
		printf("PCIE_Load failed!\r\n");
		return 0;
	}

	hPCIE = PCIE_Open(0,0,0);
	if (!hPCIE){
		printf("PCIE_Open failed\r\n");
	}else{
		while(!bQuit){
			UI_ShowMenu();
			nSel = UI_UserSelect();
			switch(nSel){	
				case MENU_LED:
					TEST_LED(hPCIE);
					break;
				/*case MENU_BUTTON:
					TEST_BUTTON(hPCIE);
					break;*/
				case MENU_SWITCH:
					TEST_SWITCH(hPCIE);
					break;
				/*case MENU_SPI:
					TEST_SPI(hPCIE);
					break;
				case MENU_DMA_MEMORY:
					TEST_DMA_MEMORY(hPCIE);
					break;
				case MENU_DMA_FIFO:
					TEST_DMA_FIFO(hPCIE);
					break;*/
				case MENU_QUIT:
					bQuit = TRUE;
					printf("Bye!\r\n");
					break;
				default:
					printf("Invalid selection\r\n");
			} // switch

		}// while

		PCIE_Close(hPCIE);

	}


	PCIE_Unload(lib_handle);
	return 0;
}
