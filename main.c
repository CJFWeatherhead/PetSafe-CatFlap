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
 * time to keep door open
 */
#define OPEN_TIME 5000

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
 * Defines for button handling
 */
#define GREEN_PRESS 1
#define RED_PRESS 2
#define BOTH_PRESS 3

#define CMD_STATE_IDLE 0
#define CMD_STATE_STAT 1
#define CMD_STATE_MODE 2 
#define CMD_STATE_SETTING 3

// Forward declarations for serial command handlers
void listCats(void);
void addCatSerial(void);
void deleteCatSerial(void);
void readCatSerial(void);

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

/**
 * Switch flap operating mode
 * @param mode
 */
void switchMode(uint8_t mode){
    uint8_t prevMode = opMode;
    const char* modeNames[] = {"NORMAL", "VET", "CLOSED", "NIGHT", "LEARN", "CLEAR", "OPEN"};
    
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
    
    // Echo mode change to UART if mode actually changed
    if(prevMode != opMode){
        printf("MODE_CHANGE: %s -> %s (code: %u -> %u)\r\n", 
               modeNames[prevMode], modeNames[opMode],
               (unsigned int)prevMode, (unsigned int)opMode);
    }
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
                //Saved successfully
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
                case 'L':
                    //List all cats
                    listCats();
                    break;
                case 'A':
                    //Add a cat manually
                    addCatSerial();
                    break;
                case 'D':
                    //Delete a cat by slot
                    deleteCatSerial();
                    break;
                case 'R':
                    //Read a specific cat by slot
                    readCatSerial();
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

/**
 * List all cats stored in EEPROM
 */
void listCats(void)
{
    printf("CMD: List cats\r\n");
    printf("Registered cats in EEPROM:\r\n");
    
    uint8_t count = 0;
    for(uint8_t i = 0; i < CAT_SLOTS; i++){
        Cat cat;
        getCat(&cat, i);
        if(cat.crc != 0){
            printf("  Slot %u: ID=%02X%02X%02X%02X%02X%02X CRC=0x%04X\r\n", 
                   (unsigned int)(i + 1),
                   cat.id[0], cat.id[1], cat.id[2], 
                   cat.id[3], cat.id[4], cat.id[5], 
                   (unsigned int)cat.crc);
            count++;
        }
    }
    printf("Total: %u cat(s) registered (max %u)\r\n", (unsigned int)count, (unsigned int)CAT_SLOTS);
}

/**
 * Add a cat manually via serial
 */
void addCatSerial(void)
{
    printf("CMD: Add cat\r\n");
    Cat cat;
    uint8_t b;
    
    // Read 6 bytes for ID
    printf("Reading cat ID (6 bytes)...\r\n");
    for(uint8_t i = 0; i < 6; i++){
        if(getByte(&b) == 0){
            cat.id[i] = b;
        }else{
            printf("ERROR: Timeout reading ID byte %u\r\n", (unsigned int)i);
            return;
        }
    }
    
    // Read 2 bytes for CRC (little-endian)
    uint16_t crc = 0;
    if(getShort(&crc) == 0){
        cat.crc = crc;
    }else{
        printf("ERROR: Timeout reading CRC\r\n");
        return;
    }
    
    // Validate CRC is not zero
    if(cat.crc == 0){
        printf("ERROR: Invalid CRC (0x0000)\r\n");
        return;
    }
    
    // Save the cat
    uint8_t slot = saveCat(&cat);
    if(slot > 0){
        printf("SUCCESS: Cat saved to slot %u\r\n", (unsigned int)slot);
        printf("  ID=%02X%02X%02X%02X%02X%02X CRC=0x%04X\r\n",
               cat.id[0], cat.id[1], cat.id[2], 
               cat.id[3], cat.id[4], cat.id[5], 
               (unsigned int)cat.crc);
    }else{
        printf("ERROR: Failed to save cat (EEPROM full or already exists)\r\n");
    }
}

/**
 * Delete a cat by slot number
 */
void deleteCatSerial(void)
{
    printf("CMD: Delete cat\r\n");
    uint8_t slot;
    
    if(getByte(&slot) == 0){
        // Slot numbers are 1-based for user friendliness
        if(slot < 1 || slot > CAT_SLOTS){
            printf("ERROR: Invalid slot %u (valid range: 1-%u)\r\n", 
                   (unsigned int)slot, (unsigned int)CAT_SLOTS);
            return;
        }
        
        // Check if slot has a cat
        Cat cat;
        getCat(&cat, slot - 1);
        if(cat.crc == 0){
            printf("ERROR: Slot %u is already empty\r\n", (unsigned int)slot);
            return;
        }
        
        // Clear the slot by setting CRC to 0
        uint8_t offset = CAT_OFFSET + (slot - 1) * sizeof(Cat);
        if(eeprom_read(offset) != 0x0){
            eeprom_write(offset, 0x0);
        }
        if(eeprom_read(offset + 1) != 0x0){
            eeprom_write(offset + 1, 0x0);
        }
        
        printf("SUCCESS: Deleted cat from slot %u\r\n", (unsigned int)slot);
        printf("  ID was: %02X%02X%02X%02X%02X%02X CRC was: 0x%04X\r\n",
               cat.id[0], cat.id[1], cat.id[2], 
               cat.id[3], cat.id[4], cat.id[5], 
               (unsigned int)cat.crc);
    }else{
        printf("ERROR: Timeout reading slot number\r\n");
    }
}

/**
 * Read a specific cat by slot number
 */
void readCatSerial(void)
{
    printf("CMD: Read cat\r\n");
    uint8_t slot;
    
    if(getByte(&slot) == 0){
        // Slot numbers are 1-based for user friendliness
        if(slot < 1 || slot > CAT_SLOTS){
            printf("ERROR: Invalid slot %u (valid range: 1-%u)\r\n", 
                   (unsigned int)slot, (unsigned int)CAT_SLOTS);
            return;
        }
        
        Cat cat;
        getCat(&cat, slot - 1);
        if(cat.crc == 0){
            printf("Slot %u: Empty\r\n", (unsigned int)slot);
        }else{
            printf("Slot %u: ID=%02X%02X%02X%02X%02X%02X CRC=0x%04X\r\n",
                   (unsigned int)slot,
                   cat.id[0], cat.id[1], cat.id[2], 
                   cat.id[3], cat.id[4], cat.id[5], 
                   (unsigned int)cat.crc);
        }
    }else{
        printf("ERROR: Timeout reading slot number\r\n");
    }
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
                GREEN_LED = 0;
                //Blink red led
                RED_LED = ((ms>>9) & 0x1);
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
                RED_LED = 1;
                GREEN_LED = 1;
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
                beep();
                inLocked = lockGreenLatch(false);
                printf("DOOR_OPEN: Cat flap unlocked for entry (timeout: %u ms)\r\n", (unsigned int)OPEN_TIME);
                printCat(&c);
                __delay_ms(OPEN_TIME);
                inLocked = lockGreenLatch(true);
                printf("DOOR_CLOSE: Cat flap locked after timeout\r\n");
            }
            c.crc = 0x0;
            //Relax
            __delay_ms(20);
        }
        
        //Handle buttons modes
        switch(handleButtons(&btnPress)){
            case GREEN_PRESS :
                if(btnPress>10000){
                    switchMode(MODE_LEARN);
                }
                break;
            case RED_PRESS :
                if(btnPress>5000){
                    if(opMode == MODE_VET){
                        switchMode(MODE_NORMAL);
                    }else{
                        switchMode(MODE_VET);
                    }
                }else if(btnPress<2000){
                    if(opMode == MODE_NIGHT){
                        switchMode(MODE_NORMAL);
                    }else{
                        switchMode(MODE_NIGHT);                        
                    }
                }
                break;
            case BOTH_PRESS :
                /*if((btnPress>2000) && (btnPress<30000)){
                    //TODO: Extended mode, to be implemented
                }else*/ if(btnPress>30000){
                    switchMode(MODE_CLEAR);                    
                }
                break;              
        }
        
        //Handle serial comm
        handleSerial();
    }
}

