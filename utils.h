/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
* and any derivatives exclusively with Microchip products. 
* 
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
* PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
* WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
* IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
* ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*
* MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
* TERMS. 
*/
 
/* 
* File:   
* Author: 
* Comments:
* Revision history: 
*/
 
// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef UTILS_H
#define	UTILS_H
 
#include <xc.h> // include processor files - each processor file is guarded.
 
#define BUFFER_SIZE 10
#define C_BUFFER_SIZE 150
 
void setup_uart();
void send_uart(char toPrint[]);
int16_t spi_comm(unsigned int addr);
void setup_spi();
void setup_ADC1();
void setup_ADC2();
void setup_pwm();
void setMotors_Zero();
void forwardMotion(int d);
void backwardMotion(int d);
void counterclockwiseMotion(int d);
void clockwiseMotion(int d);

//Fifo
 
typedef struct {
    int writeIdx;
    int readIdx;
    int isEmpty;
    int buffer[BUFFER_SIZE];
} fifo;
 
fifo init_fifo();
int push(fifo* f, int num);
int pop(fifo* f);
int isFull(fifo f);
int isEmpty(fifo f);

 
typedef struct {
    int writeIdx;
    int readIdx;
    char buffer[C_BUFFER_SIZE];
    int Empty;
} c_buff;

c_buff cb_init();
char insert_into(c_buff* cb, char msg);
char extract(c_buff* cb);
int Empty(c_buff cb);
int Full(c_buff cb);


// TODO Insert appropriate #include <>
 
// TODO Insert C++ class definitions if appropriate
 
// TODO Insert declarations
 
 
// Comment a function and leverage automatic documentation with slash star star
/**
<p><b>Function prototype:</b></p>
<p><b>Summary:</b></p>
 
    <p><b>Description:</b></p>
 
    <p><b>Precondition:</b></p>
 
    <p><b>Parameters:</b></p>
 
    <p><b>Returns:</b></p>
 
    <p><b>Example:</b></p>
<code>
</code>
 
    <p><b>Remarks:</b></p>
*/
// TODO Insert declarations or function prototypes (right here) to leverage 
// live documentation
 
#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */
 
    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code.
 
#ifdef	__cplusplus
}
#endif /* __cplusplus */
 
#endif	/* XC_HEADER_TEMPLATE_H */