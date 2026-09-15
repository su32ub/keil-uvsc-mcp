/* DemoC51 - minimal 8051 program for UVSOCK read verification.
 * Exposes easy-to-observe globals:
 *   g_tick  : incremented in the Timer0 ISR (liveness proof)
 *   g_led   : mirrored onto P1 (port read cross-check)
 * Target: Simulator (no hardware required).
 */
#include <REG52.H>

volatile unsigned int  g_tick = 0;   /* 16-bit counter, bumped by T0 ISR */
volatile unsigned char g_led  = 0xAA;/* driven onto P1 so it is visible in the Memory window */

void timer0_isr(void) interrupt 1
{
    g_tick++;
    g_led = (unsigned char)(g_tick & 0xFF);
    P1 = g_led;
}

void main(void)
{
    /* Timer0, mode 1 (16-bit), fastest practical tick for the simulator */
    TMOD = (TMOD & 0xF0) | 0x01;
    TH0  = 0x00;
    TL0  = 0x00;
    ET0  = 1;
    EA   = 1;
    TR0  = 1;

    P1 = g_led;

    for (;;)
    {
        /* idle: all activity is in the ISR so g_tick keeps moving */
    }
}
