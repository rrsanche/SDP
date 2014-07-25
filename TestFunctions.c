#include <xc.h>
#include <pwm.h>
#include <stdio.h>
#include <BOARD.h>
#include <AD.h>
#include <IO_Ports.h>
#include <serial.h>
#include <timers.h>
#include <pwm.h>

//Pins to transistors for battery banks
#define packAIn PORTY11_LAT
#define packAOut PORTZ03_LAT
#define packBIn PORTZ04_LAT
#define packBOut PORTZ05_LAT
#define batteryAGround PORTZ11_LAT
#define batteryBGround PORTY07_LAT
#define loadAGround PORTZ09_LAT
#define loadBGround PORTY07_LAT

//Variable for Voltage levels of battery packs
float packA = 0;
float packB = 0;
//Solar Sensor Variables
float solarSensorNorth = 0;
float solarSensorSouth = 0;
float solarSensorEast = 0;
float solarSensorWest = 0;
float batteryAFlag = 1;
//Flags for if the battery is discharging
float batteryBFlag = 0;
//hard coded values for battery charge levels
float lowBattery1 = 7;
float lowBattery2 = 6;
float highBattery = 13;
//Variable for system being on or off
int systemOn = 1;
//variable for storing hall effect sensor value
int hall = 0;

//Function calls all needed INIT fucntions
//Sets PWM settings
//Sets Pins to be used
void Init(void){
    //Init functions
    BOARD_Init();
    SERIAL_Init();
    TIMERS_Init();
    AD_Init();
    PWM_Init();
    //Setting PWM
    PWM_AddPins(PWM_PORTZ06|PWM_PORTY12);
    //Setting Interrupts
    INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
    INTEnableInterrupts();
    PWM_SetFrequency(PWM_5KHZ);
    //PWM_SetDutyCycle(PWM_PORTZ06, 500);
   // PWM_SetDutyCycle(PWM_PORTY12, 1000);
    //define switches as outputs
    PORTY11_TRIS = 0;
    PORTZ03_TRIS = 0;
    PORTZ04_TRIS = 0;
    PORTZ05_TRIS = 0;
    PORTZ11_TRIS = 0;
    PORTY07_TRIS = 0;
    PORTZ09_TRIS = 0;
    PORTY07_TRIS = 0;
    //Add AD pins to read from
    AD_AddPins(AD_PORTV3|AD_PORTV4|AD_PORTV5|AD_PORTV6|AD_PORTV7|AD_PORTV8);

    //Set Pins to give signals for solar sensor and turn pin on
    PORTY03_TRIS = 0;
    PORTY03_LAT = 1;

    PORTY04_TRIS = 0;
    PORTY04_LAT = 1;

    PORTY05_TRIS = 0;
    PORTY05_LAT = 1;

    PORTY06_TRIS = 0;
    PORTY06_LAT = 1;
}


//Function to real all pins
void ReadSensors(void){
    //read pin and convert it to a number between 0-12
    packA = AD_ReadADPin(AD_PORTV3);
    packA = (packA/1023)*3.3;
    packA = packA*1.3/(.3);
    //printf("\n Pack A %f", packA);
    
    packB = AD_ReadADPin(AD_PORTV4);
    packB = (packB/1023)*3.3;
    packB = (packB*1.3)/(.3);
    //printf("\n Pack B %f", packB);
    
    solarSensorNorth = AD_ReadADPin(AD_PORTV5);
    solarSensorNorth = (solarSensorNorth/1023)*3.3;
    //printf("\n North %f", solarSensorNorth);
    
    solarSensorSouth = AD_ReadADPin(AD_PORTV6);
    solarSensorSouth = (solarSensorSouth/1023)*3.3;
    //printf("\n South %f", solarSensorSouth);

    solarSensorEast = AD_ReadADPin(AD_PORTV7);
    solarSensorEast = (solarSensorEast/1023)*3.3;
    //printf("\n East %f", solarSensorEast);

    solarSensorWest = AD_ReadADPin(AD_PORTV8);
    solarSensorWest = (solarSensorWest/1023)*3.3;
    //printf("\n West %f", solarSensorWest);
}

//Function to controll the battery switches
void Switches(void){
    if(systemOn){

            //Using packA
            if(batteryAFlag){
                //check if other pack is charged
                if(packB >= highBattery){
                    packBIn = 0;
                }
                else{
                    packBIn = 1;
                }
                //turn switches on
                packAIn = 0;
                packAOut = 1;
                packBOut = 0;
                batteryBGround = 1;
                loadAGround = 1;
                loadBGround = 0;
                //check for switching to other pack
                if((packA <= lowBattery2) && (packB >= lowBattery1)){
                    packAOut = 0;
                    batteryAFlag = 0;
                    batteryBFlag = 1;
                }
                //check to see if both batteries are too low
                else if((packA <= lowBattery2) && (packB < lowBattery1)){
                    packAOut = 0;
                    batteryAFlag = 0;
                    batteryBFlag = 0;
                }
            }
            //using packB
            if(batteryBFlag){
                //check if pack A is charged up
                if(packA >= highBattery){
                    packAIn = 0;
                }
                else{
                    packAIn = 1;
                }
                //turn switches on
                packBIn = 0;
                packBOut = 1;
                packAOut = 0;
                batteryAGround = 1;
                loadAGround = 0;
                loadBGround = 1;
                //check for switching to other pack
                if((packB <= lowBattery2) && (packA >= lowBattery1)){
                    packBOut = 0;
                    batteryAFlag = 1;
                    batteryBFlag = 0;
                }
                //check to se eif both batteries are too low
                else if((packB <= lowBattery2) && (packA < lowBattery1)){
                    packBOut = 0;
                    batteryAFlag = 0;
                    batteryBFlag = 0;
                }
            }

            //if packA is higher and about threshold for turning on
            if(!systemOn){
                if((packA > packB) && (packA >= lowBattery1) && (batteryBFlag = 0)){
                    batteryAFlag = 1;
                    batteryBFlag = 0;
                }
                //if packB is higher and about threshold for turning on
                if((packB > packA) && (packB >= lowBattery1) && (batteryAFlag = 0)){
                    batteryAFlag = 0;
                    batteryBFlag = 1;
                }
            }

        }
}


//Function for sending data over UART
void Uart(void){
    //variable needed to use PutChar
    UINT8 send;
    int i = 0;
    int count = 0;
    for(count = 0; count<6;count++){
        if(count == 0){
            send = 97;
            PutChar(send);
            count++;
        }
        if(count == 1){
            //Delay for UART
            for(i = 0;i<30000;i++){}
            send = packA * 10;
            send = send % 10;
            send = packA - (send/10);
            //Addition done at this end because of Raspberry Pi interface, subtracted at other end
            send = send + 40;
            PutChar(send);
            count++;
        }
        if(count == 2){
            //Delay for UART
            for(i = 0;i<30000;i++){}
            send = 98;
            PutChar(send);
            count++;
        }
        if(count == 3){
            //Delay for UART
            for(i = 0;i<30000;i++){}
            send = packB * 10;
            send = send % 10;
            send = packB - (send/10);
            send = send + 40;
            PutChar(send);
            count++;
        }
        if(count == 4){
            //Delay for UART
            for(i = 0;i<30000;i++){}
            send = 104;
            PutChar(send);
            count++;
        }
        if(count == 5){
            //Delay for UART
            for(i = 0;i<30000;i++){}
            send = hall+40;
            PutChar(send);
            count++;
        }
        if(count == 6){
            //Delay for UART
            for(i = 0;i<30000;i++){}
            //Sends a number based off of the state of the battery switches to allow GUI to update properly
            if(batteryAFlag){
                if(packBIn == 1){
                    send = 121;
                    PutChar(send);
                }
                else if(packBIn == 0){
                    send = 120;
                    PutChar(send);
                }
            }
            else if(batteryBFlag){
                if(packAIn == 1){
                    send = 123;
                    PutChar(send);
                }
                else if(packBIn == 0){
                    send = 122;
                    PutChar(send);
                }
            }
            else if((packAIn == 1) && (packBIn == 0)){
                send = 124;
                PutChar(send);
            }
            else if((packAIn == 0) && (packBIn ==1)){
                send = 125;
                PutChar(send);
            }
            else if((packAIn == 0) && (packBIn == 0)){
                send = 126;
                PutChar(send);
            }
        }

    }
}
//Solar sensor control, motor movement needs to be added
void SolarSensor(void){
    if((solarSensorNorth - solarSensorSouth >= .1) || (solarSensorSouth - solarSensorNorth >= .1)){
        if((solarSensorEast - solarSensorWest >= .1) || (solarSensorWest - solarSensorEast >= .1)){
            printf("Sun is directly overhead");
        }
        else if(solarSensorWest > solarSensorEast){
            printf("Move West side down and East side up");
        }
        else if(solarSensorEast > solarSensorWest){
            printf("Move east side down and West side up");
        }
    }
    else if((solarSensorEast - solarSensorWest >= .1) || (solarSensorWest - solarSensorEast >= .1)){
        if((solarSensorNorth - solarSensorSouth >= .1) || (solarSensorSouth - solarSensorNorth >= .1)){
            printf("Sun is directly overhead");
        }
        else {
            printf("Do not know what to do");
        }
    }
    else if(solarSensorEast > solarSensorWest){
        if(solarSensorNorth > solarSensorSouth){
            printf("Move from East to North. Then move East side down and West side up");
        }
        else if(solarSensorSouth > solarSensorNorth){
            printf("Move from East to South. Then move East side down and West side up");
        }
    }
    else if(solarSensorWest > solarSensorEast){
        if(solarSensorNorth > solarSensorSouth){
            printf("Move from West to North. Then move West side down and East side up");
        }
        else if(solarSensorSouth > solarSensorNorth){
            printf("Move from West to South. Then move West side down and East side up");
        }
    }


}




//Function to make motors turn based off of PWM 2 seconds in each direction. Not implinented in final code.
void Motors(void){
    int x = 0;
    InitTimer(0,2000);
    SetTimer(0,2000);
    StartTimer(0);
    while(x<2){
        if ((IsTimerExpired(0) == TIMER_EXPIRED) && x == 0){
            PWM_SetDutyCycle(PWM_PORTZ06, 500);
            PWM_SetDutyCycle(PWM_PORTY12, 1000);
            SetTimer(0,2000);
            StartTimer(0);
            x = 1;
            ClearTimerExpired(0);
        }
        else if ((IsTimerExpired(0) == TIMER_EXPIRED) && (x == 1)){;
            PWM_SetDutyCycle(PWM_PORTY12, 500);
            PWM_SetDutyCycle(PWM_PORTZ06, 1000);
            SetTimer(0,2000);
            StartTimer(0);
            x = 2;
            ClearTimerExpired(0);
        }
    }
}
