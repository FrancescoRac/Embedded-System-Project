/* 
 *  Authors: Nicolas Bravi 6402442, Enrico Piacenza 4878469, Francesco Rachiglia 6478122.
*/


#include "xc.h"
#include "timer.h"
#include "utils.h"
#include "parser.h"
#include "scheduler.h"
#include "string.h"
#include <math.h>


#define START_STATE 1 
#define EXECUTE_STATE 2
#define  nTasks 6

// FIFO Structure to store the instructions
typedef struct {
    fifo x;
    fifo t; 
} control_data;

// Initial state when the code is being executed
int state = START_STATE;

// Scheduler structure inizialization
heartbeat schedInfo[nTasks];

// Parser iniziatization
parser_state pstate;

// Circular buffers
c_buff cb_recive;
c_buff cb_send;

// Global variable to keep track of the kind of motion it is being executed
int instruction = 0;

// Global variable that keeps track of the last IR sensor read
int dist = 0;

// UART1 Transmitter Interrupt Service Routine
void __attribute__((interrupt, no_auto_psv)) _U1TXInterrupt(void) {
    
    // The function sends char to the Uart until either
    // The transmitt buffer is full or the circular buffer is empty
    
    IFS0bits.U1TXIF = 0;
    
    if (!Empty(cb_send)) {
        while(!U1STAbits.UTXBF && !Empty(cb_send))
            U1TXREG = extract(&cb_send);
    } else {
        IEC0bits.U1TXIE = 0;
    }
}

// UART receive interrupt service routine
void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt(void) {
    
    // The function clear the interrupt flag
    // and read the received characters until the buffer is empty
    
    IFS0bits.U1RXIF = 0;
    
    while(U1STAbits.URXDA) // data available
    {
        char receivedChar = U1RXREG;
        insert_into(&cb_recive, receivedChar);
    }
}
 // Interrupt service routine binded to button E8
void __attribute__((__interrupt__, auto_psv))_INT1Interrupt() {

    IFS1bits.INT1IF = 0;
    tmr_setup_period(TIMER2, 20);
    IEC0bits.T2IE = 1;
    IFS0bits.T2IF = 0;
}

void __attribute__((__interrupt__, auto_psv))_T2Interrupt() {
    
    // Timer 2 interrupt to manage bouncing problem of the button E8
    // if the button was pressed, it changes state.
    
    IFS0bits.T2IF = 0;
    IEC0bits.T2IE = 0;
    T2CONbits.TON = 0;
    TMR2 = 0;

    if (PORTREbits.RE8 == 1){

        if (state == START_STATE) {
            state = EXECUTE_STATE;
            LATBbits.LATB8 = 0;
            LATFbits.LATF1 = 0;
            schedInfo[0].enable = 0;
            schedInfo[4].enable = 1;
        } else {
            state = START_STATE;
            schedInfo[0].enable = 1;
            schedInfo[4].enable = 0;
            setMotors_Zero();
            schedInfo[4].N = 1;
        }
        
    }
}

void blinkLedsWaitState() {
    LATBbits.LATB8 = !LATBbits.LATB8;
    LATFbits.LATF1 = !LATFbits.LATF1;
}

void blinkLedA0() {
    LATAbits.LATA0 = !LATAbits.LATA0;
}

void batt_read() {
    
    // Function to read battery voltage from ADC1
    // Value is added to the circular buffer to be sent
    // To UART
    
    while (!AD1CON1bits.DONE); 
    AD1CON1bits.DONE = 0;
    int value = ADC1BUF0;
    float value_f = ((float) value * 3.3) / 1024;
    value_f *= 3;
    char message[20];
    sprintf(message, "$BATT,%.2f*\n", value_f);
    insert_string(message);
}
void sens_read() {
    
    // Function to read the IR value from ADC2
    // it saves the value in the global variable "dist"
    
    while (!AD2CON1bits.DONE); 
    AD2CON1bits.DONE = 0;
    int value = ADC2BUF0;
    float value_f = ((float) value * 3.3) / 1024; // to sample analog value 
    value_f = 2.34 - 4.74 * value_f + 4.06 * pow(value_f, 2) - 1.60 * pow(value_f, 3) + 0.24 * pow(value_f, 4); // end of conversion from analog to digital
    dist = (int) (value_f * 100);
}
void send_IR() {
    
    // Function to add the IR value read
    // to the circular buffer to be sent to
    // the UART
    
    char message[20];
    sprintf(message, "$DIST,%d*\n", dist);
    insert_string(message);
}
int isCloseToObstacle() {
    
    // Function to check if an obstacle is closer than 20 cm
    
    return (dist <= 20);
}
void execute_task(void* param) {
    
    // The function exctracts one instruction to the FIFO queue
    // and execute it, x_new represents the type of motion
    // t_new represents its duration, if the FIFO is empty
    // it stops the motors.
    
    control_data *cd = (control_data*) param;
    
    if(!isEmpty(cd->x)){
        
        int x_new = pop(&cd->x);
        int t_new = pop(&cd->t);    
        
        instruction = x_new;
        schedInfo[4].N = t_new;
        
        switch (x_new) {
        case 1:
            if(!isCloseToObstacle())
                forwardMotion(7200);
            break;
        case 2:
            counterclockwiseMotion(7200);
            break;
        case 3:
            clockwiseMotion(7200);
            break;
        case 4:
            backwardMotion(7200);
            break;
    }
    }else{
        setMotors_Zero();
        instruction = 0;
        schedInfo[4].N = 1;
    }

 
}

void insert_string(char* message)
{
    // Function to insert a string in the circular buffer
    
    IEC0bits.U1TXIE = 0;
     for(int i = 0; message[i] != '\0';i++)
        insert_into(&cb_send, message[i]);
    IEC0bits.U1TXIE = 1;
    
    // If the buffer isn't full, 1 char is sent
    if (!U1STAbits.UTXBF) 
        U1TXREG = extract(&cb_send);
}

int main(void) {
    
    // Main function of the code
    
    // Set analog pins to digital
    ANSELA = ANSELC = ANSELD = ANSELE = ANSELG = 0x0000;
    
    // Pin config
    ANSELBbits.ANSB0 = 1;
    TRISEbits.TRISE8 = 1; 
    TRISAbits.TRISA0 = 0; 
    TRISBbits.TRISB8 = 0; 
    TRISFbits.TRISF1 = 0; 
    TRISBbits.TRISB9 = 0;
    TRISGbits.TRISG9 = 0;
    
    // ADC setups
    setup_ADC1();
    setup_ADC2();
    
    // PWM setup
    setup_pwm();
    
    // UART setup
    setup_uart();
    
    // Set up the interrupt for the button E8
    RPINR0bits.INT1R = 0x58;
    INTCON2bits.GIE = 1;
    IFS1bits.INT1IF = 0;
    IEC1bits.INT1IE = 1;
    
    // Uart interrupt service routine
    IFS0bits.U1RXIF = 0;
    
    // Setup deadline for the main loop
    tmr_setup_period(TIMER1, 1);
    
    // Parser init
    pstate.state = STATE_DOLLAR;
    pstate.index_type = 0;
    pstate.index_payload = 0;
    
    // Instruction FIFO init
    control_data cd;
    cd.x = init_fifo();
    cd.t = init_fifo();
    
    // Circular buffers for data transmission and recieve
    cb_recive = cb_init();
    cb_send = cb_init();

    // Scheduler functions setup
    schedInfo[0].n = 0; // Numbers used to count till N 
    schedInfo[0].N = 1000; // Numbers until we have to count to execute the function (1000ms --> 1Hz)
    schedInfo[0].enable = 1;
    schedInfo[0].f = &blinkLedsWaitState; // Function activated when n >= N
    
    schedInfo[1].n = 0;
    schedInfo[1].N = 1000; // (1000ms --> 1Hz)
    schedInfo[1].enable = 1;
    schedInfo[1].f = &batt_read;
    
    schedInfo[2].n = 0;
    schedInfo[2].N = 1; // (1ms --> 1KHz)
    schedInfo[2].enable = 1;
    schedInfo[2].f = &sens_read;
    
    schedInfo[5].n = 0;
    schedInfo[5].N = 100; // (100ms --> 10Hz)
    schedInfo[5].enable = 1;
    schedInfo[5].f = &send_IR;
    
    schedInfo[3].n = 0;
    schedInfo[3].N = 1000; // (1000ms --> 1Hz)
    schedInfo[3].enable = 1;
    schedInfo[3].f = &blinkLedA0;
    
    schedInfo[4].n = 0;
    schedInfo[4].N = 1; // (1ms --> 1KHz)
    schedInfo[4].enable = 0;
    schedInfo[4].f = execute_task;
    schedInfo[4].params = (void*)(&cd);
    
    
    LATBbits.LATB9 = 1; // enable IR sensor

    int newMessage_flag = 0;
    
    while (1) {
        
        if(instruction == 1 && isCloseToObstacle())
            setMotors_Zero();   
        
        // While to empty the cb_recive circular buffer
        while (!Empty(cb_recive)){
            int parser_state = parse_byte(&pstate,extract(&cb_recive));
            if (parser_state == NEW_MESSAGE)
            {
                newMessage_flag = 1;
            }
        }
        
        // If a new message is parsed we check it correctness
        if (newMessage_flag) {
            if (isFull(cd.x) || strcmp(pstate.msg_type, "PCCMD")) {
                insert_string("$MACK,0*\n");
            } else {
                int x_new = extract_integer(pstate.msg_payload);
                int t_new = extract_integer(pstate.msg_payload + next_value(pstate.msg_payload, 0));
                if (x_new < 0 || x_new > 4) {
                    insert_string("$MACK,0*\n");
                } else {
                    push(&cd.x, x_new);
                    push(&cd.t, t_new);
                    insert_string("$MACK,1*\n");
                }
            }
            newMessage_flag = 0;
        }
        
        // Scheduler function execution
        scheduler(schedInfo, nTasks);
        
        // Check on the deadline for TIMER1
        int ret = tmr_wait_period(TIMER1);
        
        if (ret){
            LATGbits.LATG9 = 1;
        }
    }
    return 0;
}