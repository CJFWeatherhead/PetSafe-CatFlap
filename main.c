#include <xc.h>         /* XC8 General Include File */

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

#include "interrupts.h"
#include "peripherials.h"
#include "user.h"          /* User funct/params, such as InitApp */
#include "serial.h"
#include <stdio.h>
#include "rfid.h"
#include "cat.h"

/**
 * time to keep door open (default 4 seconds per manual)
 */
#define OPEN_TIME 4000

/**
 * Number of milliseconds
 * between light sensor read
 */
#define LIGHT_READ_PERIOD 5000



/**
 * Operating mode of flap
 */
#define MODE_NORMAL 0
#define MODE_VET 1
#define MODE_CLOSED 2
#define MODE_NIGHT 3
#define MODE_LEARN 4
#define MODE_CLEAR 5
#define MODE_OPEN 6

/**
 * Extended modes (accessed via both buttons > 2s)
 */
#define EXT_MODE_EXIT 1
#define EXT_MODE_OPEN 2
#define EXT_MODE_SILENT 3
#define EXT_MODE_LIGHT_LEVEL 4
#define EXT_MODE_LOCK_TIME 5
#define EXT_MODE_LOW_BATTERY 6
#define EXT_MODE_TIMER 7

/**
 * Defines for button handling
 */
#define GREEN_PRESS 1
#define RED_PRESS 2
#define BOTH_PRESS 3

#define CMD_STATE_IDLE 0
#define CMD_STATE_STAT 1
#define CMD_STATE_MODE 2 
#define CMD_STATE_SETTING 3

//Operation mode
static uint8_t opMode = MODE_NORMAL;    
//Is the out locked?
static bool outLocked = false;
//Is the in locked
static bool inLocked = false;
//Light sensor value
static uint16_t light = 0;
//Light sensor threshold
static uint16_t lightThd = 0;
//Extended modes state
static bool inExtendedMode = false;
static uint8_t currentExtendedMode = EXT_MODE_EXIT;
//Silent mode state (disables beeps when cat detected)
static bool silentMode = false;
//Lock return time in milliseconds (default 4 seconds per manual)
static uint16_t lockReturnTime = OPEN_TIME;
//Key pad lock state (prevents button presses)
static bool keyPadLocked = false;

/**
 * Switch flap operating mode
 * @param mode
 */
void switchMode(uint8_t mode){    
    switch(mode){
        case MODE_NIGHT:
        case MODE_NORMAL:
        case MODE_LEARN:
        case MODE_CLEAR:
            //Cat is allowed to go out
            outLocked = lockRedLatch(false);
            inLocked = lockGreenLatch(true);
            break;
        case MODE_VET:
        case MODE_CLOSED:
            //Cat cannot go out
            outLocked = lockRedLatch(true);
            inLocked = lockGreenLatch(true);
            break;
        case MODE_OPEN:
            //Free party mode
            outLocked = lockRedLatch(false);
            inLocked = lockGreenLatch(false);
            break;
        default:
            //Cat is allowed to go out
            outLocked = lockRedLatch(false);
            inLocked = lockGreenLatch(true);
            mode = MODE_NORMAL;
            break;
    }
    opMode = mode;
}

/**
 * Handles button press
 * @param time Time of press
 * @return Button status
 */
uint8_t handleButtons(ms_t *time){
    static bool greenPrev = true;
    static bool redPrev = true;
    static ms_t start = 0;
    static bool bothPressed = false;
    uint8_t ret = 0;
    bool green = GREEN_BTN;
    bool red = RED_BTN;
    ms_t now = millis();
    ms_t elapsed = now-start;
    if(!green && greenPrev){
        //Rising edge on green
        start = millis();
        bothPressed = false;
    }else if(green && !greenPrev){
        //Falling edge on green
        //Is the red button pressed?
        if(bothPressed){
            ret = BOTH_PRESS;
        }else{
            ret = GREEN_PRESS;
        }
    }else if(!green && !greenPrev){
        //Button pressed
        if(!red){
            bothPressed = true;
        }
    }else if(!red && redPrev){
        //Rising edge on red only
        start = millis();
        bothPressed = false;
    }else if(red && !redPrev){
        //Falling edge on red
        //Is the red button pressed?
        if(!green){
            ret = BOTH_PRESS;
        }else{
            ret = RED_PRESS;
        }
    }else if(!red && !redPrev){
        //Button pressed
        if(!green){
            bothPressed = true;
        }
    }
    *time = elapsed;
    greenPrev = green;
    redPrev = red;
    return ret;
}

/**
 * Enter extended modes menu
 * Shows green LED and plays long beep
 */
void enterExtendedMode(void){
    inExtendedMode = true;
    currentExtendedMode = EXT_MODE_EXIT;
    beepLong();
    GREEN_LED = 1;
}

/**
 * Navigate extended modes (Red=up, Green=down)
 * Plays beeps to indicate current mode number
 */
void navigateExtendedMode(bool up){
    if(!inExtendedMode) return;
    
    if(up){
        // Red button - go up
        currentExtendedMode++;
        if(currentExtendedMode > EXT_MODE_TIMER){
            currentExtendedMode = EXT_MODE_EXIT;
        }
    }else{
        // Green button - go down
        if(currentExtendedMode <= EXT_MODE_EXIT){
            currentExtendedMode = EXT_MODE_TIMER;
        }else{
            currentExtendedMode--;
        }
    }
    
    // Play beeps to indicate current mode
    beepSeries(currentExtendedMode);
}

/**
 * Activate the currently selected extended mode
 */
void activateExtendedMode(void){
    if(!inExtendedMode) return;
    
    switch(currentExtendedMode){
        case EXT_MODE_EXIT:
            // Exit extended modes - return to normal operation
            inExtendedMode = false;
            GREEN_LED = 0;
            beepShort();
            break;
            
        case EXT_MODE_OPEN:
            // Enter Open Mode (free access)
            inExtendedMode = false;
            GREEN_LED = 0;
            switchMode(MODE_OPEN);
            beepShort();
            break;
            
        case EXT_MODE_SILENT:
            // Toggle silent mode
            silentMode = !silentMode;
            inExtendedMode = false;
            GREEN_LED = 0;
            beepShort();
            break;
            
        case EXT_MODE_LIGHT_LEVEL:
            // Set current light level as threshold
            lightThd = light;
            setConfiguration(LIGHT_CFG, lightThd);
            inExtendedMode = false;
            GREEN_LED = 0;
            beepShort();
            break;
            
        case EXT_MODE_LOCK_TIME:
            // Enter lock return time adjustment mode
            // Green LED stays on, Red LED flashes
            // User holds green button to set time (1-25 seconds)
            {
                ms_t startTime = millis();
                bool buttonHeld = false;
                RED_LED = 1; // Indicate we're in adjustment mode
                
                // Wait for user to press and hold green button
                while(inExtendedMode){
                    ms_t elapsed = millis() - startTime;
                    
                    // Flash red LED while waiting
                    RED_LED = ((millis()>>8) & 0x1);
                    
                    if(!GREEN_BTN){
                        // Green button pressed - start timing
                        if(!buttonHeld){
                            buttonHeld = true;
                            startTime = millis();
                            beepLong(); // Continuous beep while held
                        }
                        // Update time while button held (max 25 seconds)
                        elapsed = millis() - startTime;
                        if(elapsed > 25000){
                            elapsed = 25000;
                        }
                        lockReturnTime = elapsed > 1000 ? elapsed : 1000;
                    }else if(buttonHeld){
                        // Button released - save setting
                        inExtendedMode = false;
                        GREEN_LED = 0;
                        RED_LED = 0;
                        break;
                    }
                    
                    // Timeout after 30 seconds of inactivity
                    if(elapsed > 30000 && !buttonHeld){
                        inExtendedMode = false;
                        GREEN_LED = 0;
                        RED_LED = 0;
                        break;
                    }
                }
            }
            break;
            
        case EXT_MODE_LOW_BATTERY:
            // Configure low battery lock state
            // Not fully implemented - requires battery monitoring hardware
            // For now, just exit with beep
            inExtendedMode = false;
            GREEN_LED = 0;
            beepShort();
            break;
            
        case EXT_MODE_TIMER:
            // Timer mode - set lock/unlock times
            // Not fully implemented - requires RTC or time tracking
            // For now, just exit with beep
            inExtendedMode = false;
            GREEN_LED = 0;
            beepShort();
            break;
            
        default:
            // Unknown mode - exit
            inExtendedMode = false;
            GREEN_LED = 0;
            break;
    }
}

/**
 * Learn a new cat and save it to eeprom
 */
void learnCat(void)
{
    uint16_t crcRead;    
    ms_t t = millis();
    ms_t now = 0;
    uint16_t i =0;
    Cat cat;
    do{
        uint8_t r = readRFID(&cat.id[0], 6, &cat.crc, &crcRead);
        if((r ==0) && (cat.crc == crcRead) && (crcRead != 0)){        
            uint8_t slot = saveCat(&cat);
            if(slot>0){
                //Saved successfully - beep three times per manual page 7
                beep();
                __delay_ms(150);
                beep();
                __delay_ms(150);
                beep();
                break;
            }
        }
        ++i;
        if(i>9){
            i = 0;
            GREEN_LED = !GREEN_LED;
        }
        __delay_ms(20);
        now = millis();
    }while((now-t)<30000);
    GREEN_LED = 0;
}

/**
 * Build a bit pattern containing all status
 * Bit 0 : In lock (1 means locked)
 * Bit 1 : Out locked
 * Bit 2 : Flap open to go in (based on pot)
 * Bit 3 : Flap open to go out (based on pot) 
 * @return A bit pattern 
 */
uint16_t buildStatusBits(){
    uint16_t ret = 0;
    if(inLocked){ ret = 0x1; }
    if(outLocked){ ret |= 0x2; }
    return ret;
}

void printStatus(){
    // Verbose human-readable status output
    printf("STATUS: Mode=%u Light=%u Pos=%u Status=0x%04X InLocked=%u OutLocked=%u\r\n", 
           (unsigned int)opMode, (unsigned int)light, (unsigned int)0, (unsigned int)buildStatusBits(), 
           inLocked ? 1U : 0U, outLocked ? 1U : 0U);
}

/**
 * Handle all serial communication with external
 */
void handleSerial(){
    uint8_t b = 0;    
    //Do we received a command?
    if(byteAvail()){
        uint8_t c = 0;
        if(getByte(&c) == 0){
            // Echo the received character for debugging
            printf("RX: '%c' (0x%02X)\r\n", (c >= 32 && c < 127) ? c : '.', c);
            
            switch(c){
                case 'S':
                    //Get status
                    printf("CMD: Status request\r\n");
                    printStatus();
                    break;
                case 'C':
                    //Change/read a configuration
                    printf("CMD: Configuration\r\n");
                    //read/write?
                    if(getByte(&b) == 0){                
                        uint8_t index = 0;
                        //Get parameter index
                        if(getByte(&index) == 0){
                            if(b == 'S'){
                                //Set the configuration                    
                                uint16_t value = 0;
                                if(getShort(&value) == 0){
                                    setConfiguration(index, value);
                                    printf("CONFIG: Set index=%u value=%u\r\n", index, value);
                                    switch(index){
                                        case LIGHT_CFG:
                                            lightThd = value;
                                            break;
                                        default:
                                            ;
                                    }
                                }else{
                                    printf("ERROR: Timeout reading value\r\n");
                                }
                            }else{
                                //Read the configuration
                                uint16_t configValue = getConfiguration(index);
                                printf("CONFIG: Read index=%u value=%u\r\n", index, configValue);
                            }
                        }else{
                            printf("ERROR: Timeout reading index\r\n");
                        }
                    }else{
                        printf("ERROR: Timeout reading R/S parameter\r\n");
                    }
                    break;
                case 'M':
                    //Change mode
                    printf("CMD: Mode change\r\n");
                    if(getByte(&b) == 0){
                        if(b<=MODE_OPEN){
                            switchMode(b);
                            printf("MODE: Changed to %u\r\n", opMode);
                        }else{
                            printf("ERROR: Invalid mode %u (max=%u)\r\n", (unsigned int)b, (unsigned int)MODE_OPEN);
                        }
                    }else{
                        printf("ERROR: Timeout reading mode value\r\n");
                    }
                    break;
                default:
                    //Not handled, ignore it
                    printf("WARN: Unknown command '%c' (0x%02X)\r\n", 
                           (c >= 32 && c < 127) ? c : '.', c);
                    return;
            }
        }else{
            printf("ERROR: Failed to read command byte\r\n");
        }
    }
}

/**
 * Send cat ID by serial
 * @param c
 */
void printCat(const Cat* c)
{
    // Verbose human-readable cat detection output
    printf("CAT_DETECTED: ID=%02X%02X%02X%02X%02X%02X CRC=0x%04X\r\n",
           c->id[0], c->id[1], c->id[2], c->id[3], c->id[4], c->id[5], c->crc);
}

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/
void main(void)
{
    uint8_t r = 0;
    Cat c;
    uint16_t crcRead;
    ms_t btnPress = 0;    
    /* Initialize I/O and Peripherals for application */
    InitApp();
    
    // Startup sequence: visual and audio feedback to confirm boot
    // LED: Both LEDs solid for 1 second, then 2 alternating flashes (Green, Red, Green, Red)
    RED_LED = 1;
    GREEN_LED = 1;
    __delay_ms(1000);
    RED_LED = 0;
    GREEN_LED = 0;
    __delay_ms(100);
    
    // Two alternating flashes: Green, Red, Green, Red (200ms each)
    for(uint8_t i=0; i<2; ++i){
        GREEN_LED = 1;
        __delay_ms(200);
        GREEN_LED = 0;
        __delay_ms(50);
        RED_LED = 1;
        __delay_ms(200);
        RED_LED = 0;
        __delay_ms(50);
    }
    
    // Startup beep sequence: two short, one long
    beepShort();
    __delay_ms(150);
    beepShort();
    __delay_ms(150);
    beepLong();
    
    // Check if any cats are programmed - flash both LEDs if not (per manual page 18)
    if(!anyCatsProgrammed()){
        __delay_ms(500);
        for(uint8_t i=0; i<10; ++i){
            RED_LED = 1;
            GREEN_LED = 1;
            __delay_ms(250);
            RED_LED = 0;
            GREEN_LED = 0;
            __delay_ms(250);
        }
    }
    
    lightThd = getConfiguration(LIGHT_CFG);    
    // Initialize EEPROM if unprogrammed or invalid (0xFFFF=unprogrammed, >1023=out of range)
    if(lightThd > 1023){
        lightThd = 512;
        setConfiguration(LIGHT_CFG, lightThd);
    }
    switchMode(MODE_NORMAL);
    ms_t lastLightRead = millis();
    while(1)
    {   
        ms_t ms = millis();
        if((ms-lastLightRead)>LIGHT_READ_PERIOD){
            light = getLightSensor();           
            lastLightRead = ms;
        }
        bool doOpen = false;
        switch(opMode){
            case MODE_NORMAL:             
                doOpen = true;
                RED_LED = 0;
                GREEN_LED = 0;
                break;
            case MODE_VET:
                RED_LED = 0;
                //Blink green led (per manual page 18: Off, Flash)
                GREEN_LED = ((ms>>9) & 0x1);
                doOpen = true;
                break;
            case MODE_CLOSED:
                doOpen = false;
                //Blink both leds
                RED_LED = ((ms>>9) & 0x1);;
                GREEN_LED = ((ms>>9) & 0x1);
                break;
            case MODE_LEARN:
                learnCat();
                switchMode(MODE_NORMAL);
                break;
            case MODE_CLEAR:
                clearCats();
                switchMode(MODE_NORMAL);
                break;
            case MODE_OPEN:
                //Blink red led only (per manual page 18: Flash, Off)
                RED_LED = ((ms>>9) & 0x1);
                GREEN_LED = 0;
                doOpen = false;
                break;
            case MODE_NIGHT:
                //Tests if light is not enough
                //More is darker
                if((light>lightThd) && !outLocked){
                    outLocked = lockRedLatch(true);
                    lockGreenLatch(true);
                }else if((light<(lightThd-5)) && outLocked){
                    outLocked = lockRedLatch(false);
                    lockGreenLatch(true);
                }
                GREEN_LED = outLocked;
                RED_LED = 1;
                doOpen = true;
                break;
            default:
                switchMode(MODE_NORMAL);
                doOpen = false;
                break;
        }
        //If open is allowed
        if(doOpen){
            //Read RFID chip
            r = readRFID(&c.id[0], 6, &c.crc, &crcRead);
            if(r == 0 && catExists(&c, &crcRead)){
                //Read ok and found in EEPROM
                if(!silentMode){
                    beep(); // Only beep if not in silent mode
                }
                inLocked = lockGreenLatch(false);
                printCat(&c);
                __delay_ms(lockReturnTime); // Use configurable lock return time
                inLocked = lockGreenLatch(true);                
            }
            c.crc = 0x0;
            //Relax
            __delay_ms(20);
        }
        
        //Handle buttons modes
        switch(handleButtons(&btnPress)){
            case GREEN_PRESS :
                if(keyPadLocked){
                    // Buttons locked - ignore
                    break;
                }
                if(btnPress>30000){
                    // Reset to Normal Mode (per manual: Green button > 30s)
                    beepSeries(5); // Series of beeps
                    beepLong();    // Followed by long beep
                    switchMode(MODE_NORMAL);
                    // Turn off all LEDs to confirm
                    RED_LED = 0;
                    GREEN_LED = 0;
                }else if(inExtendedMode){
                    // In extended mode - navigate down
                    navigateExtendedMode(false);
                }else if(btnPress>10000){
                    // Normal mode - enter learn mode
                    switchMode(MODE_LEARN);
                }
                break;
            case RED_PRESS :
                if(btnPress>30000){
                    // Toggle Key Pad Lock (per manual: Red button > 30s)
                    keyPadLocked = !keyPadLocked;
                    beepSeries(5); // Series of beeps to confirm
                    break;
                }
                if(keyPadLocked){
                    // Buttons locked - ignore (except for unlock above)
                    break;
                }
                if(inExtendedMode){
                    // In extended mode - navigate up
                    navigateExtendedMode(true);
                }else if(btnPress>5000){
                    // Toggle Vet mode (per manual: long press >5s)
                    if(opMode == MODE_VET){
                        switchMode(MODE_NORMAL);
                        beepShort(); // Beep on exit
                    }else{
                        switchMode(MODE_VET);
                        beepShort(); // Beep on entry
                    }
                }else if(btnPress<2000){
                    // Toggle Night mode (per manual: press <2s)
                    if(opMode == MODE_NIGHT){
                        switchMode(MODE_NORMAL);
                        beepShort(); // Beep on exit
                    }else{
                        switchMode(MODE_NIGHT);
                        beepShort(); // Beep on entry
                    }
                }
                break;
            case BOTH_PRESS :
                if(keyPadLocked && btnPress<30000){
                    // Buttons locked - ignore (except for long operations)
                    break;
                }
                if(btnPress>30000){
                    // Clear all cats (per manual: both buttons > 30s)
                    switchMode(MODE_CLEAR);                    
                }else if(inExtendedMode){
                    // In extended mode - activate current mode
                    activateExtendedMode();
                }else if(btnPress>2000){
                    // Enter Extended Modes (per manual: both buttons > 2s)
                    enterExtendedMode();
                }
                break;              
        }
        
        //Handle serial comm
        handleSerial();
    }
}

