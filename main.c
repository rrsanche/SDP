/*
 * File:   main.c
 * Author: Rudy
 *
 * Created on February 12, 2014, 10:35 PM
 */


main(void) {
    Init();
    while(1){
        ReadSensors();
        //Switches();
        //SolarSensor();
        Motors();
        Uart();
    }

}
