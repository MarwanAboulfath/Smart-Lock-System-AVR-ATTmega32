# Smart-Lock-System-AVR-ATTmega32
Smart Lock System on AVR ATmega32
This project is a smart lock system implemented on an AVR ATmega32 microcontroller. It uses a keypad for user input, an EEPROM for storing the passcode, and an LCD display for user interaction. The system employs the TWI (I2C) communication protocol to ensure the passcode is not lost in case of power loss. The lock system only unlocks when the correct passcode is entered, with additional features to enhance usability and security.

Features
Password Entry and Storage: Users can set and enter a password using a keypad. The password is stored in EEPROM to prevent data loss on power failure.
Password Protection: Users can only make three incorrect password attempts before being locked out.
Password Masking: Entered passwords are displayed as '*' on the LCD.
Password Reset: Users can clear the entered password if a mistake is made.
Password Change: The password can be changed only after the correct password is entered.
LCD Display: Provides clear instructions and feedback to the user.
Interrupt Handling: External interrupts are used for password change functionality.
Hardware Abstraction Layer (HAL) Modules
LCD: For displaying messages and instructions to the user.
Keypad: For entering the password.
EEPROM: For storing and retrieving the password.
Microcontroller Abstraction Layer (MCAL) Modules
DIO: Digital Input/Output operations.
EXTI: External Interrupt handling for password change.
TWI (I2C): Two-Wire Interface for communication with EEPROM.
System Operation
Initial Setup:

When powered on for the first time, the user is prompted to create a passcode.
The passcode is stored in the EEPROM.
Normal Operation:

The user is prompted to enter the passcode to unlock the system.
The system allows three attempts to enter the correct passcode.
On entering the correct passcode, the system displays a welcome message and unlocks.
On exceeding the allowed attempts, the system displays a "BANNED" message and locks indefinitely.
Password Change:

If the user wishes to change the password, an external interrupt (triggered by a specific action) allows the password change functionality.
The user must enter the current password to set a new one.
Getting Started
Prerequisites
AVR ATmega32 microcontroller
Keypad
16x2 LCD display
EEPROM (e.g., AT24C16)
External interrupt setup (e.g., button for triggering EXTI0)
Power supply and necessary wiring components

Usage
Set the initial password:

Follow the on-screen prompts to create and save your password.
Unlock the system:

Enter the saved password to unlock the system.
If the password is incorrect, you can clear the entry using the clear button ('?').
Change the password:

Trigger the external interrupt to enable password change mode.
Enter the current password and follow the prompts to set a new password.
Authors
Marwan Khaled Aboulfath - Initial work - MarwanKAboulfath
