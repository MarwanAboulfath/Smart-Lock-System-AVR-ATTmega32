
/*****************LIB************************/
#include "LIB/BIT_MATH.h"
#include "LIB/STD_TYPES.h"
/*****************MCAL***********************/
#include "MCAL/GIE/GIE_Interface.h"
#include "MCAL/EXTI/EXTI_Interface.h"
#include "MCAL/TWI/TWI_Interface.h"
/*****************HAL************************/
#include "HAL/LCD/LCD_Interface.h"
#include "HAL/KPD/KPD_Interface.h"
#include "HAL/EEPROM/EEPROM_Interface.h"
/****************delay***********************/
#include "util/delay.h"

/********************************************************/
/*					GENERAL MACROS						*/
/********************************************************/
#define NOT_SET			0xFF
#define SET				1
#define MAX_TRIALS		3
#define CLOSED			FALSE
#define OPENED			TRUE

/**************************************************************************/
/*							 GLOBAL VARIABLES							  */
/**************************************************************************/
u8 Global_u8ChangePassword = FALSE;

/**************************************************************************/
/*							 INTERRUPT HANDLER							  */
/**************************************************************************/
void EXTI0_Handler_ChangePassword(void)
{
	Global_u8ChangePassword = TRUE;
}


int main(void)
{
	/**************************************************************************/
	/*							 LOCAL VARIABLES							  */
	/**************************************************************************/
	u8  Local_u8PressedKey 				= NOTPRESSED;	/* Store pressed keypad key value */
	u8  Local_u8HouseOpenFlag 			= CLOSED;		/* House open or closed status */
	u16 Local_u16EnteredCode 			= 0;			/* Stores users password entries */
	u8  Local_u8Password_Set 			= NOT_SET;		/* Flag stores status of password set flag */
	u8	Local_u8EntriesCounter 	= 0;					/* Stores number of password entries */
	u16 Local_u16Password		= 0;					/* Stores and reads password from EEPROM */


	/**************************************************************************/
	/*					   MODULES INITIALIZATIONS							  */
    /**************************************************************************/
	/* Initialize KEYPAD, LCD, EEPROM, GIE, and EXTI0 */
	LCD_voidInit();
	KPD_Init();
	EEPROM_voidInit();
	GIE_voidEnable();
	EXTI0_voidInit();

	/* Set notification function for EXTI0 (Change password) */
	EXTI_u8Int0SetCallBack(EXTI0_Handler_ChangePassword);

	/* Used to delete set flag variable from EEPROM in development process only */
	//EEPROM_voidSendDataByte(0xFF,2);


	/**************************************************************************/
	/*				 			   APPLICATION								  */
	/**************************************************************************/
	/* Read password status flag from EEPROM */
	Local_u8Password_Set = EEPROM_u8ReadDataByte(2);
	_delay_ms(20);

	while(1)
	{
		/************** First time create password ***************/
		/* Check password status flag */
		if((SET != Local_u8Password_Set) || \
		  ((TRUE == Global_u8ChangePassword) && (SET == Local_u8Password_Set) && (OPENED == Local_u8HouseOpenFlag)))
		{
			LCD_voidClearScreen();
			LCD_voidSetPosition(0,0);
			if(SET != Local_u8Password_Set)
			{
				LCD_voidSendString((u8*)"Create passcode:");
			}
			else if(TRUE == Global_u8ChangePassword)
			{
				LCD_voidSendString((u8*)"Change passcode:");
			}
			LCD_voidSetPosition(LCD_ROW1,LCD_COLUMN0);

			/* While loop until password is set */
			while((Local_u8Password_Set != SET) || (TRUE == Global_u8ChangePassword))
			{
				/* Get pressed Key value */
				Local_u8PressedKey = KPD_u8GetPressed();
				/* Check if pressed value is a number */
				if (Local_u8PressedKey >= '0' && Local_u8PressedKey <= '9')
				{
					/* Store it in Entered Code variable */
					Local_u16EnteredCode = (Local_u16EnteredCode * 10) + Local_u8PressedKey - '0';
					/* Print '*' to hide password */
					LCD_voidSendData('*');
				}
				/* Check if pressed key value is '?'(CLEAR PASSWORD) */
				else if ('?' == Local_u8PressedKey)
				{
					/* Reset entered code variable */
					Local_u16EnteredCode = 0;
					/* Clear entered password from LCD */
					LCD_voidSetPosition(LCD_ROW1,LCD_COLUMN0);
					LCD_voidSendString((u8*)"      ");
					LCD_voidSetPosition(LCD_ROW1,LCD_COLUMN0);
				}
				/* Check if pressed key value is '='(ENTER)/(SAVE PASSWORD) */
				else if('=' == Local_u8PressedKey)
				{
					/* Store first byte of password in EEPROM */
					EEPROM_voidSendDataByte((u8)Local_u16EnteredCode,0);
					_delay_ms(20);
					/* Store second byte of password in EEPROM */
					EEPROM_voidSendDataByte(((u8)(Local_u16EnteredCode>>8)),1);
					_delay_ms(20);

					/* Clear entered code variable to be ready for next entry */
					Local_u16EnteredCode = 0;

					/* Set the Set Password flag and store the value in EEPROM */
					Local_u8Password_Set = SET;
					EEPROM_voidSendDataByte(Local_u8Password_Set,2);
					_delay_ms(20);
					if(TRUE == Global_u8ChangePassword)
					{
						Global_u8ChangePassword = FALSE;
						Local_u8HouseOpenFlag 	= CLOSED;
						Local_u16EnteredCode 	= 0;
						Local_u8EntriesCounter 	= 0;
					}
					else
					{
						/* Do nothing */
					}


					LCD_voidClearScreen();
				}

			}

		}
		/************** Request password from user ***************/
		/* Check if password is already set  and House flag is not already open and number of pasword entries is less than MAX_TRIALS (3) */
		else if ((SET == Local_u8Password_Set) && (CLOSED == Local_u8HouseOpenFlag) && (MAX_TRIALS > Local_u8EntriesCounter))
		{
			LCD_voidSetPosition(0,0);
			LCD_voidSendString((u8*)"Enter passcode:");
			LCD_voidSetPosition(LCD_ROW1,LCD_COLUMN0);

			/* Set pressed key value to not pressed */
			Local_u8PressedKey = NOTPRESSED;

			/* While loop until user enters right password or exceeds number of wrong entries */
			while((MAX_TRIALS > Local_u8EntriesCounter) && (CLOSED == Local_u8HouseOpenFlag))
			{
				/* Get pressed key value */
				Local_u8PressedKey = KPD_u8GetPressed();

				/* Check if pressed value is a number */
				if (Local_u8PressedKey >= '0' && Local_u8PressedKey <= '9')
				{
					/* Store it in Entered Code variable */
					Local_u16EnteredCode = (Local_u16EnteredCode * 10) + Local_u8PressedKey - '0';
					/* Print '*' to hide password */
					LCD_voidSendData('*');
				}
				/* Check if pressed key value is '?'(CLEAR PASSWORD) */
				else if (Local_u8PressedKey == '?')
				{
					/* Reset entered code variable */
					Local_u16EnteredCode = 0;
					/* Clear entered password from LCD */
					LCD_voidSetPosition(LCD_ROW1,LCD_COLUMN0);
					LCD_voidSendString((u8*)"      ");
					LCD_voidSetPosition(LCD_ROW1,LCD_COLUMN0);
				}
				/* Check if pressed key value is '='(ENTER) */
				else if(Local_u8PressedKey == '=')
				{
					/* Increment Password number of entries counter */
					Local_u8EntriesCounter++;

					/* Read password form EEPROM */
					/* Read first byte */
					Local_u16Password = EEPROM_u8ReadDataByte(0);
					/* Read second byte */
					Local_u16Password |= (EEPROM_u8ReadDataByte(1)<<8);

					/* Compare Entered password with stored password */
					/* Right password entered */
					if(Local_u16EnteredCode == Local_u16Password)
					{
						/* Set House flag to open */
						Local_u8HouseOpenFlag = OPENED;
						/* Reset password entries counter */
						Local_u8EntriesCounter = 0;
						/* Clear entered password variable */
						Local_u16EnteredCode = 0;

					}
					/* Wrong password entered */
					else
					{
						/* Set House flag to closed */
						Local_u8HouseOpenFlag = CLOSED;
						/* Clear entered password variable */
						Local_u16EnteredCode = 0;
						/* Print Wrong password to user */
						LCD_voidClearScreen();
						LCD_voidSendString((u8*)"Wrong Password");

						/* Check if number of entries is less than MAX_TRIALS (3) */
						if(MAX_TRIALS > Local_u8EntriesCounter)
						{
							/* Print try again for 0.5s */
							LCD_voidSetPosition(LCD_ROW1,LCD_COLUMN0);
							LCD_voidSendString((u8*)"Try Again");
							_delay_ms(500);
							/* Print enter password */
							LCD_voidClearScreen();
							LCD_voidSetPosition(0,0);
							LCD_voidSendString((u8*)"Enter passcode:");
							LCD_voidSetPosition(LCD_ROW1,LCD_COLUMN0);
						}
					}
					/* Reset Pressed key value */
					Local_u8PressedKey = NOTPRESSED;
				}
			}
		}
		/************** Check if entered password is right and password entries are less than MAX_TRIALS (3) ***************/
		else if ((OPENED == Local_u8HouseOpenFlag) && (MAX_TRIALS > Local_u8EntriesCounter))
		{
			/* Reset number of entries counter */
			Local_u8EntriesCounter = 0;
			/* Print Welcome message */
			LCD_voidClearScreen();
			LCD_voidSendString((u8*)"Welcome Marwan");
			_delay_ms(20);

			/********* SEND OPEN CONTROLS TO SLAVE **********/
//			while(1)
//			{
//
//			}
		}
		/************** Check if password entries are more than or equal MAX_TRIALS (3) ***************/
		else if (MAX_TRIALS <= Local_u8EntriesCounter)
		{
			/* Print Banned message and enter banned state */
			LCD_voidClearScreen();
			LCD_voidSendString((u8*)"BANNED");
			_delay_ms(20);

			/********* SEND CLOSE CONTROLS TO SLAVE **********/
			while(1)
			{

			}
		}
	}
	return 0;
}
