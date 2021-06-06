/* enc.h
 * Performs encryption using AES 128-bit
 */

#ifndef ENC_H
#define ENC_H

#include <iostream>
#include <cstring>
#include <fstream>
#include <sstream>

#include "structures.h"

using namespace std;

/* Serves as the initial round during encryption
 * AddRoundKey is simply an XOR of a 128-bit block with the 128-bit key.
 */
void AddRoundKey(unsigned char * state, unsigned char * roundKey)
{
	for (int i = 0; i < 16; i++)
		state[i] ^= roundKey[i];

}

/* Perform substitution to each of the 16 bytes
 * Uses S-box as lookup table
 */
void SubBytes(unsigned char * state)
{
	for (int i = 0; i < 16; i++)
		state[i] = s[state[i]];
}

// Shift left, adds diffusion
void ShiftRows(unsigned char * state)
{
	unsigned char temp_state[16];

	// Given below is an example of this function
	/*
	0 4 8  12	-> 0  4  8  12
	1 5 9  13	-> 5  9  13 1
	2 6 10 14	-> 10 14 2  6
	3 7 11 15	-> 15 3  7  11
	*/

	// Column 1
	temp_state[0] = state[0];
	temp_state[1] = state[5];
	temp_state[2] = state[10];
	temp_state[3] = state[15];

	// Column 2
	temp_state[4] = state[4];
	temp_state[5] = state[9];
	temp_state[6] = state[14];
	temp_state[7] = state[3];

	// Column 3
	temp_state[8] = state[8];
	temp_state[9] = state[13];
	temp_state[10] = state[2];
	temp_state[11] = state[7];

	// Column 4
	temp_state[12] = state[12];
	temp_state[13] = state[1];
	temp_state[14] = state[6];
	temp_state[15] = state[11];

	for (int i = 0; i < 16; i++)
		state[i] = temp_state[i];
}

/* MixColumns multipies the state table with the standard matrix
 * Using mul2, mul3 lookup tables

	02 03 01 01
	01 02 03 01
	01 01 02 03
	03 01 01 02

  * Source of diffusion
  */
void MixColumns(unsigned char* state)
{
	unsigned char temp_state[16];

	temp_state[0] = (unsigned char) mul2[state[0]] ^ mul3[state[1]] ^ state[2] ^ state[3];
	temp_state[1] = (unsigned char) state[0] ^ mul2[state[1]] ^ mul3[state[2]] ^ state[3];
	temp_state[2] = (unsigned char) state[0] ^ state[1] ^ mul2[state[2]] ^ mul3[state[3]];
	temp_state[3] = (unsigned char) mul3[state[0]] ^ state[1] ^ state[2] ^ mul2[state[3]];

	temp_state[4] = (unsigned char)mul2[state[4]] ^ mul3[state[5]] ^ state[6] ^ state[7];
	temp_state[5] = (unsigned char)state[4] ^ mul2[state[5]] ^ mul3[state[6]] ^ state[7];
	temp_state[6] = (unsigned char)state[4] ^ state[5] ^ mul2[state[6]] ^ mul3[state[7]];
	temp_state[7] = (unsigned char)mul3[state[4]] ^ state[5] ^ state[6] ^ mul2[state[7]];

	temp_state[8] = (unsigned char)mul2[state[8]] ^ mul3[state[9]] ^ state[10] ^ state[11];
	temp_state[9] = (unsigned char)state[8] ^ mul2[state[9]] ^ mul3[state[10]] ^ state[11];
	temp_state[10] = (unsigned char)state[8] ^ state[9] ^ mul2[state[10]] ^ mul3[state[11]];
	temp_state[11] = (unsigned char)mul3[state[8]] ^ state[9] ^ state[10] ^ mul2[state[11]];

	temp_state[12] = (unsigned char)mul2[state[12]] ^ mul3[state[13]] ^ state[14] ^ state[15];
	temp_state[13] = (unsigned char)state[12] ^ mul2[state[13]] ^ mul3[state[14]] ^ state[15];
	temp_state[14] = (unsigned char)state[12] ^ state[13] ^ mul2[state[14]] ^ mul3[state[15]];
	temp_state[15] = (unsigned char)mul3[state[12]] ^ state[13] ^ state[14] ^ mul2[state[15]];

	for (int i = 0; i < 16; i++)
		state[i] = temp_state[i];
}

/* Each round operates on 128 bits at a time
 * The number of rounds is defined in AESEncrypt()
 */
void Round(unsigned char * state, unsigned char * key)
{
	SubBytes(state);
	ShiftRows(state);
	MixColumns(state);
	AddRoundKey(state, key);
}

// Same as Round() except it doesn't mix columns
void FinalRound(unsigned char * state, unsigned char * key)
{
	SubBytes(state);
	ShiftRows(state);
	AddRoundKey(state, key);
}

/* The AES encryption function
 * Organizes the confusion and diffusion steps into one function
 */
void AESEncrypt(unsigned char * message, unsigned char * expandedKey, unsigned char * encryptedMessage)
{
	unsigned char state[16]; // Stores the first 16 bytes of original message

	for (int i = 0; i < 16; i++)
		state[i] = message[i];


	// Here the number of rounds is excluding the final round
	int numberOfRounds = 9; // Since the key is 128 bits

	AddRoundKey(state, expandedKey); // Initial round

	for (int i = 0; i < numberOfRounds; i++)
		Round(state, expandedKey + (16 * (i + 1)));


	FinalRound(state, expandedKey + 160);

	// Copy encrypted state to buffer
	for (int i = 0; i < 16; i++)
		encryptedMessage[i] = state[i];
}

int encrypt()
{
	// for getting input from input.txt
	//freopen("input.txt", "r", stdin);
	// for writing output to output.txt
	//freopen("output.txt", "w", stdout);


	cout << "\n=============================" << endl;
	cout << " 128-bit AES Encryption Tool   " << endl;
	cout << "=============================" << endl;

	cout << "\nEnter the message to encrypt: \n";
	FILE * f = fopen("rand_txt.txt", "rb");
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	char * message = new char[fsize + 1];
	fread(message, fsize, 1, f);
	fclose(f);

	message[fsize] = '\0';

	cout << message << endl;

	// Pad message to 16 bytes
	int originalLen = strlen((const char *)message);

	int paddedMessageLen = originalLen;

	if ((paddedMessageLen % 16) != 0)
		paddedMessageLen = (paddedMessageLen / 16 + 1) * 16;

	unsigned char * paddedMessage = new unsigned char[paddedMessageLen];

	for (int i = 0; i < paddedMessageLen; i++)
	{
		if (i >= originalLen)
			paddedMessage[i] = 0;

		else
			paddedMessage[i] = message[i];
	}

	unsigned char * encryptedMessage = new unsigned char[paddedMessageLen];

	string str;
	ifstream infile;
	infile.open("keyfile.txt", ios::in | ios::binary);

	if (infile.is_open())
	{
		getline(infile, str); // The first line of file should be the key
		infile.close();
	}

	else
		cout << "Unable to open file\n";

	istringstream hex_chars_stream(str);
	unsigned char key[16];
	int i = 0;
	unsigned int c;

	while (hex_chars_stream >> hex >> c)
		key[i++] = c;

	unsigned char expandedKey[176];

	KeyExpansion(key, expandedKey);

	for (int i = 0; i < paddedMessageLen; i += 16)
		AESEncrypt(paddedMessage + i, expandedKey, encryptedMessage + i);

	cout << "\nEncrypted message in hex:\n";

	for (int i = 0; i < paddedMessageLen; i++)
		cout << hex << (int) encryptedMessage[i] << " ";

	cout << endl;

	// Write the encrypted string out to file "message.aes"
	ofstream outfile;
	outfile.open("message.aes", ios::out | ios::binary);

	if (outfile.is_open())
	{
		outfile << encryptedMessage;
		outfile.close();
		cout << "Wrote encrypted message to file message.aes\n";
	}

	else
		cout << "Unable to open file\n";
	
	// Free memory
	delete[] message;
	delete[] paddedMessage;
	delete[] encryptedMessage;

	return paddedMessageLen;
}

#endif
