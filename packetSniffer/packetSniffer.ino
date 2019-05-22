byte incomingByte; // for incoming serial data
String command = "";

void setup() {
  Serial.begin(115200); // opens serial port, sets data rate to 9600 bps
}

void loop() {
	// send data only when you receive data:
	if (Serial.available() > 0) {
		// read the incoming byte:
		incomingByte = (byte) Serial.read();
		command = command + " " + incomingByte;
	}
	Serial.println(command);
}
