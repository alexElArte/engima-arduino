#include "enigma.h"
#include "enigmaKeys.h"


#define BUF_SIZE 32
Enigma enigma;

#define NB_ROTOR 3
Enigma::Rotor rotors[NB_ROTOR];
u8 reflector[26];


void setup() {
	delay(1000);
	Serial.begin(115200);
	Serial.println("Example: enigma serial using preload keys");

	enigma.loadKey(I, &rotors[0]);   // Fastest
	enigma.loadKey(II, &rotors[1]);
	enigma.loadKey(III, &rotors[2]); // Slowest
	enigma.loadKey(A, reflector);
}

void loop() {
	if (Serial.available()) {
		char txt[BUF_SIZE];
		u8 i = Serial.readBytes(txt, BUF_SIZE);
		enigma.cypher(txt, i, rotors, NB_ROTOR, reflector);
		Serial.write(txt, i);
	}
	delay(100);
}