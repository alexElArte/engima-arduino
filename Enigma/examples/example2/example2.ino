#include "enigma.h"
#include "enigmaKeys.h"


#define BUF_SIZE 32
Enigma enigma;

#define NB_ROTORS 3
Enigma::Rotor rotors[NB_ROTORS];
u8 reflector[26];


void setup() {
	delay(1000);
	Serial.begin(115200);
	Serial.println("Example: enigma serial using custom keys");
	Serial.println("Send 'R' to create keys or 'C' to load them");
	
	while (!Serial.available()) delay(100);
	
	char txt[32];
	char letter = Serial.read();
	Serial.readBytes(txt, 32);
	if (letter == 'R') {                   // Random
		// Reflector
		enigma.createReflector(reflector);
		
		// Rotors
		for (u8 i = 0; i < NB_ROTORS; i++) {
			enigma.createRotor(&rotors[i]);
		}
	} else if (letter == 'C') {            // Custom
		// Reflector
		Serial.println("Send the reflector");
		while (Serial.available() < 26) delay(100);
		u8 ct = Serial.readBytes(txt, 26);
		if(ct != 26 || !enigma.loadKey(txt, reflector)) {
			Serial.println("Error loading the reflector");
			Serial.println("while(1)");
			while(1) delay(1000);
		}
		if (Serial.peek() == '\n') Serial.read();
		
		// Rotors
		for (u8 i = 0; i < NB_ROTORS; i++) {
			Serial.print("Send rotor ");
			Serial.println(i + 1);
			while (Serial.available() < 26) delay(100);
			u8 ct = Serial.readBytes(txt, 26);
			if(ct != 26 || !enigma.loadKey(txt, &rotors[i])) {
				Serial.println("Error loading the rotor");
				Serial.println("while(1)");
				while(1) delay(1000);
			}
			if (Serial.peek() == '\n') Serial.read();
		}

	} else {
		Serial.println("Wrong command, infinity loop");
		while (1) delay(1000);
	}

	// Reflector
	enigma.key2char(reflector, txt);
	Serial.print("Reflector:  ");
	Serial.write(txt, 26);
	Serial.println();
	// Rotors
	for (u8 i = 0; i < NB_ROTORS; i++) {
		enigma.key2char(rotors[i], txt);
		Serial.print("Rotor ");
		Serial.print(i+1);
		Serial.print("  :  ");
		Serial.write(txt, 26);
		Serial.println();
	}

}

void loop() {
	if (Serial.available()) {
		char txt[BUF_SIZE];
		u8 i = Serial.readBytes(txt, BUF_SIZE);

		enigma.cypher(txt, i, rotors, NB_ROTORS, reflector);
		Serial.write(txt, i);
		
	}
	delay(100);
}