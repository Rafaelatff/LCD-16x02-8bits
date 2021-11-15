//Using Code Composer Studio - Version: 10.2.0.00009
#include <msp430.h> 
#include <string.h> //to use strlen() function
//#include <stdio.h> //to use printf() function



int ACLK_clock;
char text1[15]="LCD 8 bits";
char text2[15]="Rafaela Freitas";



void delay_ms(int time);
void start_clock_sys(void);
void start_LCD_8bit_pins(void);
void send_LCD_byte(char RS, char data);
void send_LCD_text(char text[], char line, char pos);

/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;       //to apply the GPIO configurations disable the LOCKLPM5 bit in PM5CTL0.

    //Set LED1 for test routines
    // Set P1.0 as output direction [J7 needs to be connected]
    // LED White: Port P1 / Pin 0
    P1SEL0 &= ~BIT0;
    P1SEL1 &= ~BIT0;
    P1DIR |= BIT0; // Set LED1 as Output
    P1OUT &= ~BIT0; // turn off LED1

    start_clock_sys();
	start_LCD_8bit_pins();
	
	send_LCD_text(text1,1,3);
	send_LCD_text(text2,2,0);

	while(1){
	    P1OUT ^= BIT0;
	    delay_ms(2);
	}


	return 0;
}
// Timer A0 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) Timer_A (void)
#else
#error Compiler not supported!
#endif
{
    ACLK_clock++;

    //P1OUT ^= BIT0;
    TA0CCR0 +=1;                             // Add Offset to TACCR0
}

void start_clock_sys(void){
    //Clock configured as counter mode
    //Timer configuration: 16 bits = 65.536 - 1 (0 até 65535/ 0x0 até 0xFFFF)
    //Timer A Instance 0 (TA0) ->  Compare/Capture Register 0 (CCR0)
    //1 - To set Enable bit:
       TA0CCTL0 |= CCIE;
    //Now the MSP430 will interrupt once the timer reaches TA0CCR0.
    //2 - Choose value for TA0CCR0:
       TA0CCR0 = 1;
       //3 - Choose clock source for Timer_A0 (SMCLK, ) and also choose Continuous Mode:
       //The default for the SMCLK is the internal DCO which is set to 1 MHz on reset.
       //SMCLK = 1 MHz, so one timer count is 1/(1 MHz) = 1 us
       //Since Timer_A is only 16 bits, our max count is 65,535
       //ACLK = 32678 Hz
       TA0CTL |= TASSEL__ACLK | MC__CONTINUOUS;
       //It will enter ISR of timer A each 1/32678 seconds -> 30.6 us
       //1ms = 32678 clocks

    //4 - Enable global interrupts in the MSP430 so that we can perform an action once TA0R = TA0CCR0.
    //It`s also possible to send to an LPMx mode by adding: LPM0_bits |
       __bis_SR_register(GIE);
}

void delay_ms(int time){
    //Time isn`t very accurate, but enought for applications
    int count;
    for (count=0;count<=time;count++){
        ACLK_clock=0;
        while(ACLK_clock<=30);
    }
    //place following lines in main to test
    /*  while(1){
        P1OUT ^= BIT0;
        delay_ms(10);
    }*/
}

void send_LCD_text(char text[], char line, char pos){
    char i;
    char text_size = strlen(text); //read string size

    if(line==1)line=0x80;   //Set line 1 of LCD = 0x8*
        else if (line==2)line=0xc0;  //Set line 2 of LCD = 0xc*

    send_LCD_byte(0,line+pos); //Send line and position command

    for(i=0;i<text_size;i++){    //Stay in loop while send all the texts
        send_LCD_byte(1,text[i]);     //Envia caracter por caracter o texto ao LCD
        }
}

void send_LCD_byte(char RS, char data){
    if (RS==0) P4OUT &= ~BIT1; // RS pin -> Text = 1 and Command = 0
        else P4OUT |= BIT1;
    //set EN low
    P3OUT &= ~BIT1;

    //set data from D0 to D7
    if(data & 0x01) P5OUT |= BIT2;//Compare D0 (LSB)
        else P5OUT &= ~BIT2;
    if(data & 0x02) P5OUT |= BIT1; //Compare D1
        else P5OUT &= ~BIT1;
    if(data & 0x04) P5OUT |= BIT0; //Compare D2
        else P5OUT &= ~BIT0;
    if(data & 0x08) P4OUT |= BIT7; //Compare D3
        else P4OUT &= ~BIT7;
    if(data & 0x10) P6OUT |= BIT0; //Compare D4
        else P6OUT &= ~BIT0;
    if(data & 0x20) P3OUT |= BIT3; //Compare D5
        else P3OUT &= ~BIT3;
    if(data & 0x40) P6OUT |= BIT1; //Compare D6
        else P6OUT &= ~BIT1;
    if(data & 0x80) P6OUT |= BIT2; //Compare D7 (MSB)
        else P6OUT &= ~BIT2;
    //Send an enable signal - Falling edge
    P3OUT |= BIT1;
    delay_ms(2);
    P3OUT &= ~BIT1;
    delay_ms(2);
}

void start_LCD_8bit_pins(void){

    // Configure all LCD pins

    // VSS = GND
    // VDD = 5V
    // V0 = trimpot (3V3 - V0 - GND)
    // A = 5V
    // K = GND

    // SELx = 00 to work as GPIOS
    // DIR = 1 to work as output
    // OUT = 0 Set pin to start as low

    // EN (LCD) = P3.1 (MSP)
    P3SEL0 &= ~BIT1;
    P3SEL1 &= ~BIT1;
    P3DIR |= BIT1;
    P3OUT &= ~BIT1;

    // RS (LCD) = P4.1 (MSP) // Text = 1 and Command = 0
    P4SEL0 &= ~BIT1;
    P4SEL1 &= ~BIT1;
    P4DIR |= BIT1;
    P4OUT &= ~BIT1;

    // RW (LCD) = GND (MSP) // Read = 1 and Write = 0

    // D0 (LCD) = P5.2 (MSP)
    P5SEL0 &= ~BIT2;
    P5SEL1 &= ~BIT2;
    P5DIR |= BIT2;
    P5OUT &= ~BIT2;
    // D1 (LCD) = P5.1 (MSP)
    P5SEL0 &= ~BIT1;
    P5SEL1 &= ~BIT1;
    P5DIR |= BIT1;
    P5OUT &= ~BIT1;
    // D2 (LCD) = P5.0 (MSP)
    P5SEL0 &= ~BIT0;
    P5SEL1 &= ~BIT0;
    P5DIR |= BIT0;
    P5OUT &= ~BIT0;
    // D3 (LCD) = P4.7 (MSP)
    P4SEL0 &= ~BIT7;
    P4SEL1 &= ~BIT7;
    P4DIR |= BIT7;
    P4OUT &= ~BIT7;
    // D4 (LCD) = P6.0 (MSP)
    P6SEL0 &= ~BIT0;
    P6SEL1 &= ~BIT0;
    P6DIR |= BIT0;
    P6OUT &= ~BIT0;
    // D5 (LCD) = P3.3 (MSP)
    P3SEL0 &= ~BIT3;
    P3SEL1 &= ~BIT3;
    P3DIR |= BIT3;
    P3OUT &= ~BIT3;
    // D6 (LCD) = P6.1 (MSP)
    P6SEL0 &= ~BIT1;
    P6SEL1 &= ~BIT1;
    P6DIR |= BIT1;
    P6OUT &= ~BIT1;
    // D7 (LCD) = P6.2 (MSP)
    P6SEL0 &= ~BIT2;
    P6SEL1 &= ~BIT2;
    P6DIR |= BIT2;
    P6OUT &= ~BIT2;

    //Send an enable signal - Falling edge
    P3OUT |= BIT1;
    delay_ms(2);
    P3OUT &= ~BIT1;
    delay_ms(2);

    //Set operating mode (4 or 8 bits)
    send_LCD_byte(0,0x38); // 0x0011.1000 FUNCTION SET - 4 bits, 2 lines, 5x7 dots
    send_LCD_byte(0,0x0F); // 0x0000.1111 DISPLAY CONTROL - Display on, no cursor
    send_LCD_byte(0,0x01); // CLEAN LCD
    send_LCD_byte(0,0x06); // ENTRY MODE SET - Shift cursor to the right each written character
    delay_ms(20);

}
