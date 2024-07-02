#include "xc.h"
#include "timer.h"
#include "utils.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
 
///////////////////////////////////UART/////////////////////////////////////////
 
void setup_uart(){
    // Set up the UART
    RPOR0bits.RP64R = 1;
    RPINR18bits.U1RXR = 0x4b;    

    U1BRG = 468; // Baud rate for 9600 bps at 72 MHz
    
    U1MODEbits.UARTEN = 1; // Enable UART
    U1STAbits.UTXEN = 1; // Enable UART TX
    
    
    IFS0bits.U1RXIF = 0; // Clear the U1RX interrupt flag
    IEC0bits.U1RXIE = 1; // Enable the U1RX interrupt
}
 
void send_uart(char toPrint[]){
    for (int i = 0;toPrint[i] != '\0'; i++)
    {
        while(U1STAbits.UTXBF);                
        U1TXREG = toPrint[i];
    }
}
 
/////////////////////////////////SPI////////////////////////////////////////////
 
int16_t spi_comm(unsigned int addr){
    int16_t value;
    while (SPI1STATbits.SPITBF == 1);
    SPI1BUF = addr; // setting the MSB to 1
    while (SPI1STATbits.SPIRBF == 0);
    value = SPI1BUF; // read to prevent buffer overrun
    return value;
}
 
void setup_spi(){
    
    TRISDbits.TRISD6 = 0;
    TRISBbits.TRISB3 = 0; 
    TRISBbits.TRISB4 = 0;

    LATBbits.LATB3 = 1; 
    LATBbits.LATB4 = 1; 
    LATDbits.LATD6 = 1;

    // Setup the SPI
    SPI1CON1bits.MSTEN = 1; // master mode
    SPI1CON1bits.MODE16 = 0; // 8bit mode
    SPI1CON1bits.PPRE = 1; //  16:1 primary prescaler
    SPI1CON1bits.SPRE = 7; //  1:1 secondary prescaler    
    SPI1STATbits.SPIEN = 1; // enable SPI
 
    TRISAbits.TRISA1 = 1; // RAI-RP117 MISO
    TRISFbits.TRISF12 = 0; //RF12-RP108 SCK
    TRISFbits.TRISF13 = 0; // RF13-RP109 MOSI
    RPINR20bits.SDI1R = 0b0010001; // MISO (SDII) - RP117
    RPOR12bits.RP109R = 0b000101; // MOSI (SDOI) - RF13
    RPOR11bits.RP108R = 0b000110; // SCKI;
    
    // Switch to Sleep mode
    LATDbits.LATD6 = 0;
    spi_comm(0x4B); 
    spi_comm(0x01);
    LATDbits.LATD6 = 1;
    tmr_wait_ms(TIMER1, 2);
    // Switch to Active mode
    LATDbits.LATD6 = 0;
    spi_comm(0x4C);
    spi_comm(0x00);
    LATDbits.LATD6 = 1;
}

//////////////////////////////ADC///////////////////////////////////////////////

void setup_ADC1(){

    // Automatic Sampling, Automatic Conversion
    AD1CON1bits.ADON = 0;
    AD1CON3bits.ADCS = 8; // Tad = 8*(ADCS + 1)*(Tpb)
    AD1CON1bits.ASAM = 1; // Enable Automatic sampling
    AD1CON1bits.SSRC = 7; // Enable Automatic conversion
    AD2CON3bits.SAMC = 16; // Set sampling time (adjust as needed)
    AD1CON2bits.CHPS = 0; // Select CH0 for sampling
    AD1CHS0bits.CH0SA = 0b1011; // Select AN11 for CH0 positive input
    AD1CON1bits.ADON = 1; // Turn on ADC module
}

void setup_ADC2(){
    // Automatic Sampling, Automatic Conversion
    AD2CON1bits.ADON = 0;
    AD2CON3bits.ADCS = 8; // Tad = 8*(ADCS + 1)*(Tpb)
    AD2CON1bits.ASAM = 1; // Enable Automatic sampling
    AD2CON1bits.SSRC = 7; // Enable Automatic conversion
    AD2CON3bits.SAMC = 16; // Set sampling time (adjust as needed)
    AD2CON2bits.CHPS = 0; // Select CH0 for sampling
    AD2CHS0bits.CH0SA = 0b1110; // Select AN14 for CH0 positive input
    AD2CON1bits.ADON = 1; // Turn on ADC module
}


    
//////////////////////////////PWM///////////////////////////////////////////////
 
void setup_pwm(){
    // Motors pins set as output
    TRISDbits.TRISD1 = 0;
    TRISDbits.TRISD2 = 0;
    TRISDbits.TRISD3 = 0;
    TRISDbits.TRISD4 = 0;
    // remapping of the outputcomares to the motors
    RPOR0bits.RP65R = 0b010000;
    RPOR1bits.RP66R = 0b010001;
    RPOR1bits.RP67R = 0b010010;
    RPOR2bits.RP68R = 0b010011;
    
    // Setup Output compare 1,2,3,4
    OC1CON1 = 0; // 
    OC1CON2 = 0; // 
    OC1CON1bits.OCTSEL = 0x07; // source clock internal peripheral
    OC1R = 0; // value to that the timer must count to 
    OC1RS = 7200; // period 
    OC1CON2bits.SYNCSEL = 0x1F; //    
    OC1CON1bits.OCM = 6; // 
    
    OC2CON1 = 0;
    OC2CON2 = 0; 
    OC2CON1bits.OCTSEL = 0x07;
    OC2R = 0;
    OC2RS = 7200; 
    OC2CON2bits.SYNCSEL = 0x1F; 
    OC2CON1bits.OCM = 6;
    
    OC3CON1 = 0;
    OC3CON2 = 0; 
    OC3CON1bits.OCTSEL = 0x07;
    OC3R = 0;
    OC3RS = 7200; 
    OC3CON2bits.SYNCSEL = 0x1F; 
    OC3CON1bits.OCM = 6;
    
    OC4CON1 = 0;
    OC4CON2 = 0; 
    OC4CON1bits.OCTSEL = 0x07;
    OC4R = 0;
    OC4RS = 7200; 
    OC4CON2bits.SYNCSEL = 0x1F; 
    OC4CON1bits.OCM = 6;
}

void setMotors_Zero() {
    OC1R = 0;
    OC2R = 0;
    OC3R = 0;
    OC4R = 0;
}
void forwardMotion(int d) {
    OC1R = 0;
    OC2R = d;
    OC3R = 0;
    OC4R = d;
}
void backwardMotion(int d) {
    OC1R = d;
    OC2R = 0;
    OC3R = d;
    OC4R = 0;
}
void counterclockwiseMotion(int d) {
    OC1R = d;
    OC2R = 0;
    OC3R = 0;
    OC4R = d;
}
void clockwiseMotion(int d) {
    OC1R = 0;
    OC2R = d;
    OC3R = d;
    OC4R = 0;
}
 
 
/////////////////FIFO///////////////////////////////////////////////////////////
 
fifo init_fifo()
{
	fifo f;
	f.readIdx = 0;
    f.writeIdx = 0;
    f.isEmpty = 1;
    return f;
}
int push(fifo* f, int num) {
    int status = 1;
    if (isFull(*f)) {
        status = 0; // Buffer is full
    } else {
        f->buffer[f->writeIdx] = num;
        f->writeIdx = (f->writeIdx + 1) % BUFFER_SIZE;
        f->isEmpty = 0;
    }
    
    return status;
}

int pop(fifo* f) {
    int value = -1; // Default return value for an empty buffer
    //if (f->writeIdx != f->readIdx) {
    if (!isEmpty(*f)) {
        value = f->buffer[f->readIdx];
        f->readIdx = (f->readIdx + 1) % BUFFER_SIZE;
        
        if (f->readIdx == f->writeIdx)
            f->isEmpty = 1; // Buffer becomes empty
       
    }
    
    return value;
}


int isFull(fifo f) {
    //return ((f.writeIdx + 1) % BUFFER_SIZE == f.readIdx );
    return (f.writeIdx == f.readIdx && !f.isEmpty);
}
 
int isEmpty(fifo f) {
    return f.isEmpty;
}

 
 
//////////////////////Circular Buffer///////////////////////////////////////////

c_buff cb_init()
{
	c_buff cb;
	cb.readIdx = 0;
    cb.writeIdx = 0;
    cb.Empty = 1;
    cb.buffer[0] = 0;
    return cb;
}

char insert_into(c_buff* cb, char msg) {
    if (Full(*cb)) {
        return -1; // Buffer is full
    }
    
    cb->buffer[cb->writeIdx] = msg;
    cb->writeIdx = (cb->writeIdx + 1) % C_BUFFER_SIZE;
    cb->Empty = 0;
    return 0; // Success
}


char extract(c_buff* cb) {    
    
    if (Empty(*cb)) {
        return -1; // Buffer is empty
    }
    char msg = cb->buffer[cb->readIdx];
    cb->readIdx = (cb->readIdx + 1) % C_BUFFER_SIZE;
    if (cb->readIdx == cb->writeIdx) {
        cb->Empty = 1; // Buffer is now empty
    }
    return msg;
}

int Full(c_buff cb) {
    return (cb.writeIdx == cb.readIdx && !cb.Empty);
}

int Empty(c_buff cb) {
    return cb.Empty;
}