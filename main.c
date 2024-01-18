//main.c
#include <SI_EFM8BB1_Register_Enums.h>

void InitDevice(void)
{
	
//Disable Watchdog Timer with key sequence
WDTCN = 0xDE; //First key
WDTCN = 0xAD; //Second key

//Setup Defaults for P1
P1MDOUT = P1MDOUT_B0__OPEN_DRAIN | P1MDOUT_B1__OPEN_DRAIN
| P1MDOUT_B2__OPEN_DRAIN | P1MDOUT_B3__OPEN_DRAIN
| P1MDOUT_B4__PUSH_PULL | P1MDOUT_B5__OPEN_DRAIN
| P1MDOUT_B6__OPEN_DRAIN | P1MDOUT_B7__OPEN_DRAIN;
//Enable Weak Pullups
XBR2 = XBR2_WEAKPUD__PULL_UPS_ENABLED | XBR2_XBARE__ENABLED;
//Disable Interrupts
IE = IE_EA__DISABLED | IE_EX0__DISABLED | IE_EX1__DISABLED
| IE_ESPI0__DISABLED | IE_ET0__DISABLED | IE_ET1__DISABLED
| IE_ET2__ENABLED | IE_ES0__DISABLED;
}
unsigned int o = 0;
void DelayMs(unsigned int x){
	o = 0;
	while(o < x);
}
void OVERFLOW_ISR(void) interrupt 1{
	TCON &= ~(1<<4);
	TH0 = 0x9C;
	TL0 = 0x4F;
	TCON |= (1<<4);
	o++;
}

void main(void){
	InitDevice();
	IE = 0x82;//enable inter
	TMOD = 0x01;
	TH0 = 0x9C;//100ms timer overflow
	TL0 = 0x4F;
	TCON |= (1<<4); //start the timer
	while(1){
		P0 = 0xF6;//red light
		P1 = 0xF6;//red light on for west and east
		DelayMs(20);//2sec so switch isnt instant
		P0 = 0xDB;//green light			
		DelayMs(300);//delays by 30s
		P0 = 0xED;//yellow light
		DelayMs(60);//delays by 6s
		P0 = 0xF6;//stays red until next time
		DelayMs(20);//2sec
			
		//west light with left turn
		P1 = 0xB3;//green light and left turn light
		DelayMs(70);//7sec left turn is on
		P2 &= ~(1<<0);//turns left turn yellow on
		P1 = 0xF3;// keeps green on other side on
		DelayMs(60);//6sec
		P2 |= (1<<0);//turn of yellow
		DelayMs(10);
		P1 = 0xDB;//keeps green light on
		DelayMs(170);//17sec
		P1 = 0xED;//yellow
		DelayMs(60);//6sec
		P1 = 0xF6;//red
		DelayMs(20);
			
		//north south lights
		P1 = 0xF6;//red light on for west and east
		DelayMs(20);//delay by 2s
		P0 = 0xDB;//green light
		DelayMs(300);//delays by 30s
		P0 = 0xED;//yellow light
		DelayMs(60);//delays by 6s
		P0 = 0xF6;//stays red until next time
		DelayMs(20);//2second delay from when it switches so its not instant
			
		//east light with protected left turn
		P1 = 0x5E;//green light and left turn
		DelayMs(70);//7sec delay
		P2 &= ~(1<<1);//tells left turn yellow to be on
		P1 = 0xDE;//keeps green on but tells other west light to stay red
		DelayMs(60);
		P2 |= (1<<1);//turns left yellow led off
		DelayMs(10);
		P1 = 0xDB;//turns both east and west green so cars can go straight on opposite lanes
		DelayMs(170);//17s
		P1 = 0xED;//yellow
		DelayMs(60);//6sec
		P1 = 0xF6;//red
	}
}
