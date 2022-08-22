/*
	Crypto.cpp - Library for emulate Enigma cypher.
	Created by Alexandre.M, August 18, 2022.
	Released into the public domain.
*/

#include "enigma.h"

/*
 * Init the class in the sketch
*/
Enigma::Enigma() {
	// Nothing to do
}

/*
 * Find out if a character is a letter (e.g a,A,b,B,...)
 * @param  letter   the character to find out
 * @return either true or false
*/
bool Enigma::isLetter(char letter) {
	if (letter >= 'a' && letter <= 'z') return true;
	if (letter >= 'A' && letter <= 'Z') return true;
	return false; // Other
}

/*
 * Transform an ASCII letter into an integer (0 - 25)
 * @param  letter   the letter to transform
 * @return a integer between 0 and 25
*/
char Enigma::letter2int(char letter) {
	if (letter >= 'a' && letter <= 'z') return letter - 'a'; // 26
	if (letter >= 'A' && letter <= 'Z') return letter - 'A'; // 26
	return 0; // A
}

/*
 * Transform a integer between 0 and 26 into an ASCII letter
 * @param  code   the integer to tranform
 * @return an ASCII letter of '?' if an error occur
*/
char Enigma::int2letter(char code) {
	if (code < 26) return code + 'A';
	return '?';
}

/*
 * Test if a number is in an array
 * @param  val     the value to search
 * @param  array   the array to search for
 * @param  size    the size of the array
 * @return either true if the number is in or false
*/
bool Enigma::valInArray(u8 val, u8* array, u16 size) {
	for (u16 i = 0; i < size; i++) {
		if (val == array[i]) return true;
	}
	return false;
}

/*
 * Create a rotor
 * @param  rotor   the variable to put the key in
*/
void Enigma::createRotor(Rotor* rotor) {
	// Reset the rotor
	rotor->shift = 0;
	// Prepare the arrays
	for (u8 i = 0; i < 26; i++) {
		rotor->normal[i] = 27;
		rotor->invert[i] = 27;
	}
	
	u32 test = 0;
	for (u8 i = 0; i < 26; i++) {
		while (1) {
			u8 val = random(26); // 0 - 25
			// Take the next unused value
			while (1) {
				if (test & (1 << val)) {
					val = (val + 1) % 26;
				} else break;
			}
			// Prevent from transforming a letter to the same
			// (e.g A -> A)
			if (val == i) continue;
			// Trevent from overwritting a value
			test |= 1 << val;
			// Save the result
			rotor->normal[i] = val;
			rotor->invert[val] = i;
			break;
		}
	}
}

/*
 * Create a reflector
 * @param  reflector   an array (size 26) where to put the reflector
*/
void Enigma::createReflector(u8* reflector) {
	// Prepare the array
	for (u8 i = 0; i < 26; i++) reflector[i] = 27;
	
	u32 test = 0;
	for (u8 i = 0; i < 26; i++) {
		// Pass if the value is already saved
		if (reflector[i] != 27) continue;
		while (1) {
			u8 val = random(26); // 0 - 25
			// Take the next unused value
			while (1) {
				if (test & (1 << val)) {
					val = (val + 1) % 26;
				} else break;
			}
			// Prevent from transforming a letter to the same
			// e.g A -> A
			if (val == i) continue;
			// Trevent from overwritting a value
			test |= 1 << val;
			test |= 1 << i;
			// Save the result
			reflector[i] = val;
			reflector[val] = i;
			break;
		}
	}
}

/*
 * Create a rotor from a list of letter
 * @param key     a list of letter
 * @param rotor   a rotor
 * @return false if an error occur
*/
bool Enigma::loadKey(char* key, Rotor* rotor) {
	u32 test = 0;
	rotor->shift = 0;
	for (u8 i = 0; i < 26; i++) {
		if (!isLetter(key[i])) return false;
		u8 val = letter2int(key[i]);
		if (test & (1 << val)) return false;
		test |= 1 << val;
		rotor->normal[i] = val;
		rotor->invert[val] = i;
	}
	return true;
}

/*
 * Create a reflector from a list of letter
 * @param key         a list of letter
 * @param reflector   a reflector
 * @return false if an error occur
*/
bool Enigma::loadKey(char* key, u8* reflector) {
	for (u8 i = 0; i < 26; i++) {
		if (!isLetter(key[i])) return false;
		reflector[i] = letter2int(key[i]);
	}
	for (u8 i = 0; i < 26; i++) {
		if (reflector[reflector[i]] != i) return false;
	}
	return true;
}

/*
 * Transform a rotor into a readable text
 * @param  rotor   the rotor to translate
 * @param  txt     the array where to put the letter
*/
void Enigma::key2char(Rotor rotor, char* txt) {
	for (u8 i = 0; i < 26; i++) txt[i] = rotor.normal[i] + 'A';
}

/*
 * Transform a reflector into a readable text
 * @param  reflector   the reflector to translate
 * @param  txt         the array where to put the letter
*/
void Enigma::key2char(u8* reflector, char* txt) {
	for (u8 i = 0; i < 26; i++) txt[i] = reflector[i] + 'A';
}

/*
 * Encode a message using Enigma's algorithm
 * @param  buf         the buffer where the message is stored
 * @param  size        the buffer' size
 * @param  rotors      an array of rotors
 * @param  nb_rot      the number of rotors
 * @param  reflector   the array where the reflector is stored
*/
void Enigma::cypher(char* buf, u16 size, Rotor* rotors, u8 nb_rot, u8* reflector) {
	for (u16 i = 0; i < size; i++) {
		if (!isLetter(buf[i])) continue;

		// Shift rotors
		for (u8 r = 0; r < nb_rot; r++) {
			rotors[r].shift += 1;
			if (rotors[r].shift >= 26) rotors[r].shift = 0;
			else break;
		}

		// Transform into encodable integer
		u8 trans = letter2int(buf[i]);
		// Forward current
		for (u8 rot = 0; rot < nb_rot; rot++) {
			trans = rotors[rot].normal[(trans+rotors[rot].shift)%26];
		}
		// Reflector
		trans = reflector[trans];
		// Backward current
		for (u8 rot = 0; rot < nb_rot; rot++) {
			u8 roto = nb_rot-rot-1;
			// Reflector transformation
			trans = rotors[roto].invert[trans];
			// Shift transformation
			trans = (trans + 26 - rotors[roto].shift) % 26;
		}
		// ASCII readable
		buf[i] = trans + 'A';
	}
}