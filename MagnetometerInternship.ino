//Lots of Code from https://github.com/felis/USB_Host_Shield_2.0/blob/master/examples/acm/acm_terminal/acm_terminal.ino
#include <cdcacm.h>
#include <usbhub.h>

#include "pgmstrings.h"

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>
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

USB     Usb;
//USBHub     Hub(&Usb);
ACMAsyncOper  AsyncOper;
ACM           Acm(&Usb, &AsyncOper);

void setup()
{
  Serial.begin(115200);
  Serial.println("Start");

  if (Usb.Init() == -1)
	  Serial.println("OSCOKIRQ failed to assert");

  delay( 200 );
}

void loop(){
  Serial.println(readACM());
  delay(50);
}

String readACM(){
  Usb.Task();

  if( Acm.isReady()) {
	   uint8_t rcode;

		/* reading the phone */
		/* buffer size must be greater or equal to max.packet size */
		/* it it set to 64 (largest possible max.packet size) here, can be tuned down
		for particular endpoint */
		uint8_t  buf[64];
		uint16_t rcvd = 64;
		rcode = Acm.RcvData(&rcvd, buf);
		 if (rcode && rcode != hrNAK)
			ErrorMessage<uint8_t>(PSTR("Ret"), rcode);

		if( rcvd ) { //more than zero bytes received
			char rcvdData[rcvd];
			for(uint16_t i=0; i < rcvd; i++ ) {
				rcvdData[i] = (char)buf[i]; //printing on the screen
			}
			return String(rcvdData);
		}
	}
	return String("ACM Not Ready");
}
