#include <stdint.h>
#include "tm4c123gh6pm.h"

volatile int start=0;
void I2C_Init(void) {
    SYSCTL_RCGCI2C_R |= 0x01;            // Enable clock to I2C0 module
    SYSCTL_RCGCGPIO_R |= 0x02;           // Enable clock to Port B

    GPIO_PORTB_AFSEL_R |= 0x0C;          // Enable alternate function on PB2 and PB3
    GPIO_PORTB_ODR_R |= 0x08;            // Enable open drain on PB3 (SDA)
    GPIO_PORTB_DEN_R |= 0x0C;            // Enable digital I/O on PB2 and PB3
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xFFFF00FF) | 0x00003300; // Assign I2C signals to PB2 and PB3

    I2C0_MCR_R = 0x10;                   // Initialize I2C master function
    I2C0_MTPR_R = 0x09;                  // Set SCL clock speed (based on 16 MHz clock for 100 kbps)
}

int main(void) {
    I2C_Init();

    int slaveAddr = 0x61;
    int data[1] = {0x0000};    // Transmit data
    int datasent[2];    // Data to send

    // Populate datasent with 16-bit data, handling only available data elements
    datasent[0] = (data[0] >> 8) & 0xFF;
    datasent[1] = data[0] & 0xFF;
    int i=0;
    I2C0_MSA_R = (slaveAddr << 1) & 0xFE;  // Set slave address for write


    while(1){


        I2C0_MDR_R = datasent[i%2];              // Load the byte to transmit

        if (i ==0 && start==0) {
            I2C0_MCS_R |= 0x03;                 // First byte: START and RUN
            start=1;
        }
         else {
            I2C0_MCS_R = 0x01;                 // Middle bytes: RUN only
        }

        while (I2C0_MCS_R & 0x01);             // Wait until BUSY clears

        if (I2C0_MCS_R & 0x02) {               // Check for errors
            I2C0_MCS_R = 0x04;                 // Send STOP to recover if error
            break;
        }
        if(data[0]<4090&&i%3==2){
        data[0]=data[0]+4096/1000;
        }
        else if(data[0]>=4090)
        {
            data[0]=0x0000;
        }
        datasent[0] = (data[0] >> 8) & 0xFF;
        datasent[1] = data[0] & 0xFF;
        i=i+1;

    }



}

