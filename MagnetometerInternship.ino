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

//set Up SD Card
const int SDChipSelect = 4;
const long maxLog = 100000;
volatile long logged = 0;
unsigned long name = 0;
File dataFile;
String newData;

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
	dataFile = SD.open(String(name)+".txt", FILE_WRITE);

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
		newData = readACM();
		Serial.print(readACM());
		SDActive();
		if(dataFile){
			dataFile.print(newData);
		}
		if(logged == maxLog){
			logged = 0;
			dataFile.close();
			name++;
			dataFile = SD.open(String(name)+".txt", FILE_WRITE);
		}
		USBActive();
		isNewData = false;
	}
}

//Interrupt
ISR(TIMER1_COMPA_vect){
	TCNT1= t1_load;
	//Serial.print("Interrupt");
	//latestOutput = 
	//readACM();
	isNewData = true;
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

String readACM(){
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
			char rcvdData[rcvd];
			for(uint16_t i=0; i < rcvd; i++ ) {
				rcvdData[i] = (char)buf[i]; //printing on the screen
			}

			memset(buf,0,rcvd);
			return String(rcvdData);
		}
		return "";
	}
	return "SomeError \n";
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