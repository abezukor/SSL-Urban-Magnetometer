//Lots of Code from https://github.com/felis/USB_Host_Shield_2.0/blob/master/examples/acm/acm_terminal/acm_terminal.ino
#include <cdcacm.h>
#include <usbhub.h>

#include "pgmstrings.h"

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif

#include <SPI.h>
#include <SD.h>

//https://github.com/felis/USB_Host_Shield_2.0.git
class ACMAsyncOper : public CDCAsyncOper{
public:
	uint8_t OnInit(ACM *pacm);
};

uint8_t ACMAsyncOper::OnInit(ACM *pacm){
	uint8_t rcode;
	// Set DTR = 1 RTS=1
	rcode = pacm->SetControlLineState(3);

	if (rcode)
	{
		ErrorMessage<uint8_t>(PSTR("SetControlLineState"), rcode);
		return rcode;
	}

	LINE_CODING	lc;
	lc.dwDTERate	= 115200;
	lc.bCharFormat	= 0;
	lc.bParityType	= 0;
	lc.bDataBits	= 8;

	rcode = pacm->SetLineCoding(&lc);

	if (rcode)
		ErrorMessage<uint8_t>(PSTR("SetLineCoding"), rcode);

	return rcode;
}

//Set up USB
const int USBChipSelect = 10;
USB     Usb;
//USBHub     Hub(&Usb);
ACMAsyncOper  AsyncOper;
ACM           Acm(&Usb, &AsyncOper);
volatile char rcvdData[127];
char nonVolatileData[127];

//set Up SD Card
const int SDChipSelect = 4;
const long maxLog = 500;
volatile long logged = 0;
unsigned long name = 0;
File dataFile;

//Set Up timers
const uint16_t t1_load = 0;
const uint16_t t1_comp = 15624;
volatile bool isNewData = false;


void setup(){
	//Set up Serial
	Serial.begin(115200);
	Serial.println("Start");

	//Set up SPI
	pinMode(USBChipSelect, OUTPUT); // Sets USB CS pin output
	pinMode(SDChipSelect, OUTPUT); //Sets SD CS pin output
	digitalWrite(SDChipSelect,HIGH);
	digitalWrite(USBChipSelect,HIGH);

	//Set Up SD
	SDActive();
	SD.begin(SDChipSelect);
	dataFile = SD.open(String(name), FILE_WRITE);

	//Set up USB
	USBActive();

	if (Usb.Init() == -1){
	Serial.println("OSCOKIRQ failed to assert");
	}
	delay(500);

	//set up timers

	noInterrupts();
	// Clear registers
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1 = 0;

	// 16 Hz (16000000/((15624+1)*64))
	OCR1A = t1_comp;
	// CTC
	TCCR1B |= (1 << WGM12);
	// Prescaler 64
	TCCR1B |= (1 << CS11) | (1 << CS10);
	// Output Compare Match A Interrupt Enable
	TIMSK1 |= (1 << OCIE1A);
	interrupts();

}

void loop(){
	
	if(isNewData){
		for(int i = 0; i<127; i++){
			nonVolatileData[i] = rcvdData[i];
		}
		Serial.print(logged);Serial.println(name);
		if(dataFile){
			SDActive();
			dataFile.print(nonVolatileData);
		}
		memset(nonVolatileData,(char)0,127);
		memset(rcvdData,(char)0,127);
		isNewData = false;
	}

	if(logged >= maxLog){
		noInterrupts();
		//Serial.println("Changing name");
		logged = 0;
		SDActive();
		//Serial.println("Closing File");
		dataFile.close();
		name++;
		SDActive();
		//Serial.println("Opening New File ");
		dataFile = SD.open(String(name), FILE_WRITE);
		interrupts();
	}
}

//Interrupt
ISR(TIMER1_COMPA_vect){
	TCNT1= t1_load;
	//Serial.print("Interrupt");
	//latestOutput = 
	//readACM();
	isNewData = readACM();
	logged++;
}

//Activates USB and Deactivates SD
void USBActive(){
	digitalWrite(SDChipSelect,HIGH);
	digitalWrite(USBChipSelect,LOW);
}

//Activates SD and Deactiavates USB
void SDActive(){
	digitalWrite(USBChipSelect,true);
	digitalWrite(SDChipSelect,false);
}

bool readACM(){
	USBActive();
	Usb.Task();

	if( Acm.isReady()) {
		uint8_t rcode;

		/* buffer size must be greater or equal to max.packet size */
		/* it it set to 64 (largest possible max.packet size) here, can be tuned down
		for particular endpoint */
		uint8_t buf[127];
		uint16_t rcvd = 127;
		rcode = Acm.RcvData(&rcvd, buf);
			if (rcode && rcode != hrNAK)
			ErrorMessage<uint8_t>(PSTR("Ret"), rcode);

		if( rcvd ) { //more than zero bytes received
			for(uint16_t i=0; i < rcvd; i++ ) {
				rcvdData[i] = (char)buf[i]; //printing on the screen
			}

			memset(buf,0,rcvd);
			SDActive();
			return true;
		}
		return false;
	}
	return false;
}

bool sndData(byte data[]){
	Usb.Task();

	if(Acm.isReady()){
		uint8_t rcode;

		rcode = Acm.SndData(1, data);
		if (rcode){
			ErrorMessage<uint8_t>(PSTR("SndData"), rcode);
			return false;
		}
		return true;
	}else{
		Serial.print("ACM Not Ready");
		return false;
	}
}