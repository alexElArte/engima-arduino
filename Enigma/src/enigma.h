/*
	Enigma.h - Library for emulate Enigma cypher.
	Created by Alexandre.M, August 18, 2022.
	Released into the public domain.
*/

#ifndef ENIGMA_H
#define ENIGMA_H

#if ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif


#define u8 byte
#define u16 unsigned int
#define u32 unsigned long


class Enigma {
	public:
		typedef struct {
			u8 normal[26];
			u8 invert[26];
			u8 shift;
		} Rotor;

		Enigma(); // Use ths to init the library
		
		bool isLetter(char letter);
		char letter2int(char letter);
		char int2letter(char code);
		
		bool loadKey(char* key, Rotor* rotor);
		bool loadKey(char* key, u8* reflector);
		void key2char(Rotor rotor, char* txt);
		void key2char(u8* reflector, char* txt);
		
		bool valInArray(u8 val, u8* array, u16 size);
		
		void createRotor(Rotor* rotor);
		void createReflector(u8* reflector);
		void cypher(char* buf, u16 size, Rotor* rotors, u8 nb_rot, u8* reflector);
};

#endif













