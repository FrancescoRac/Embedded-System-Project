/*
 * File:   timer1.c
 * Author: enricopaicenza
 *
 * Created on March 6, 2024, 4:39 PM
 */


#include "xc.h"
#include "timer.h"

long Fcy = 72000000;
int prescaler = 256;

void tmr_setup_period(int timer, int ms) {

    int tcount;
    tcount = ((Fcy / prescaler) / 1000.0) * ms; // Assuming Fosc = 144MHz

    switch (timer) {
        case TIMER1:
            T1CONbits.TON = 0;
            TMR1 = 0;
            T1CONbits.TCKPS = 0b11; // Prescaler 256
            PR1 = tcount;
            T1CONbits.TON = 1;
            break;
            
        case TIMER2:
            T2CONbits.TON = 0;
            TMR2 = 0;
            T2CONbits.TCKPS = 0b11; // Prescaler 256
            PR2 = tcount;
            T2CONbits.TON = 1;
            break;
            
        case TIMER3:
            T3CONbits.TON = 0;
            TMR3 = 0;
            T3CONbits.TCKPS = 0b11; // Prescaler 256
            PR3 = tcount;
            T3CONbits.TON = 1;
            break;
            
        case TIMER4:
            T4CONbits.TON = 0;
            TMR4 = 0;
            T4CONbits.TCKPS = 0b11; // Prescaler 256
            PR4 = tcount;
            T4CONbits.TON = 1;
            break;
            
        default:
            break;
    }
}


int tmr_wait_period(int timer) {

    switch (timer) {

        case TIMER1:

            if (IFS0bits.T1IF) {
                IFS0bits.T1IF = 0;
                return 1;
            }
            while (!IFS0bits.T1IF) {
            }
            IFS0bits.T1IF = 0;
            return 0;

        case TIMER2:

            if (IFS0bits.T2IF) {
                IFS0bits.T2IF = 0;
                return 1;
            }
            while (!IFS0bits.T2IF) {
            }
            IFS0bits.T2IF = 0;
            return 0;

        case TIMER3:

            if (IFS0bits.T3IF) {
                IFS0bits.T3IF = 0;
                return 1;
            }
            while (!IFS0bits.T3IF) {
            }
            IFS0bits.T3IF = 0;
            return 0;
            
        case TIMER4:

            if (IFS1bits.T4IF) {
                IFS1bits.T4IF = 0;
                return 1;
            }
            while (!IFS1bits.T4IF) {
            }
            IFS1bits.T4IF = 0;
            return 0;
        default:
            break;
    }
}

void tmr_wait_ms(int timer, int ms) {

    if (ms > 200) {

        tmr_setup_period(timer, 200);

        while (ms > 0) {
            tmr_wait_period(timer);
            ms -= 200;
        }

        int tcount;
        tcount = ((Fcy / prescaler) / 1000.0) * ms; // Assuming Fosc = 144MHz

        switch (timer) {

            case TIMER1:
                PR1 = tcount;
                if (TMR1 < tcount)
                    tmr_wait_period(timer);
                break;

            case TIMER2:
                PR2 = tcount;
                if (TMR2 < tcount)
                    tmr_wait_period(timer);
                break;

            case TIMER3:
                PR3 = tcount;
                if (TMR3 < tcount)
                    tmr_wait_period(timer);
                break;
                
            case TIMER4:
                PR4 = tcount;
                if (TMR4 < tcount)
                    tmr_wait_period(timer);
                break;
            default:
                break;
        }
    } else {
        tmr_setup_period(timer, ms);
        tmr_wait_period(timer);
    }

    switch (timer) {

        case TIMER1:
            T1CONbits.TON = 0;
            TMR1 = 0;
            break;

        case TIMER2:
            T2CONbits.TON = 0;
            TMR2 = 0;
            break;

        case TIMER3:
            T3CONbits.TON = 0;
            TMR3 = 0;
            break;

        case TIMER4:
            T4CONbits.TON = 0;
            TMR4 = 0;
            break;

        default:
            break;
    }
}