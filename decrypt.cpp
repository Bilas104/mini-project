/* decrypt.cpp
 * Performs decryption using AES 128-bit
 */

#include <iostream>
#include <cstring>
#include <fstream>
#include <sstream>
#include "structures.h"

using namespace std;

/* Used in Round() and serves as the final round during decryption
 * SubRoundKey is simply an XOR of a 128-bit block with the 128-bit key.
 * So basically does the same as AddRoundKey in the encryption
 */
void SubRoundKey(unsigned char * state, unsigned char * roundKey)
{
	for (int i = 0; i < 16; i++)
		state[i] ^= roundKey[i];
}

/* InverseMixColumns
 * Uses mul9, mul11, mul13, mul14 look-up tables
 * Multiplies with the matrix

 	0E 0B 0D 09
	09 0E 0B 0D
	0D 09 0E 0B
	0B 0D 09 0E

 * Unmixes the columns by reversing the effect of MixColumns in encryption
 */
void InverseMixColumns(unsigned char * state)
{
	unsigned char temp_state[16];

	temp_state[0] = (unsigned char)mul14[state[0]] ^ mul11[state[1]] ^ mul13[state[2]] ^ mul9[state[3]];
	temp_state[1] = (unsigned char)mul9[state[0]] ^ mul14[state[1]] ^ mul11[state[2]] ^ mul13[state[3]];
	temp_state[2] = (unsigned char)mul13[state[0]] ^ mul9[state[1]] ^ mul14[state[2]] ^ mul11[state[3]];
	temp_state[3] = (unsigned char)mul11[state[0]] ^ mul13[state[1]] ^ mul9[state[2]] ^ mul14[state[3]];

	temp_state[4] = (unsigned char)mul14[state[4]] ^ mul11[state[5]] ^ mul13[state[6]] ^ mul9[state[7]];
	temp_state[5] = (unsigned char)mul9[state[4]] ^ mul14[state[5]] ^ mul11[state[6]] ^ mul13[state[7]];
	temp_state[6] = (unsigned char)mul13[state[4]] ^ mul9[state[5]] ^ mul14[state[6]] ^ mul11[state[7]];
	temp_state[7] = (unsigned char)mul11[state[4]] ^ mul13[state[5]] ^ mul9[state[6]] ^ mul14[state[7]];

	temp_state[8] = (unsigned char)mul14[state[8]] ^ mul11[state[9]] ^ mul13[state[10]] ^ mul9[state[11]];
	temp_state[9] = (unsigned char)mul9[state[8]] ^ mul14[state[9]] ^ mul11[state[10]] ^ mul13[state[11]];
	temp_state[10] = (unsigned char)mul13[state[8]] ^ mul9[state[9]] ^ mul14[state[10]] ^ mul11[state[11]];
	temp_state[11] = (unsigned char)mul11[state[8]] ^ mul13[state[9]] ^ mul9[state[10]] ^ mul14[state[11]];

	temp_state[12] = (unsigned char)mul14[state[12]] ^ mul11[state[13]] ^ mul13[state[14]] ^ mul9[state[15]];
	temp_state[13] = (unsigned char)mul9[state[12]] ^ mul14[state[13]] ^ mul11[state[14]] ^ mul13[state[15]];
	temp_state[14] = (unsigned char)mul13[state[12]] ^ mul9[state[13]] ^ mul14[state[14]] ^ mul11[state[15]];
	temp_state[15] = (unsigned char)mul11[state[12]] ^ mul13[state[13]] ^ mul9[state[14]] ^ mul14[state[15]];

	for (int i = 0; i < 16; i++)
		state[i] = temp_state[i];
}

// Shifts rows right (rather than left; opposite of what we did for encrypting) for decryption
void ShiftRows(unsigned char * state)
{
	unsigned char temp_state[16];

	//Column 1
	temp_state[0] = state[0];
	temp_state[1] = state[13];
	temp_state[2] = state[10];
	temp_state[3] = state[7];

	// Column 2
	temp_state[4] = state[4];
	temp_state[5] = state[1];
	temp_state[6] = state[14];
	temp_state[7] = state[11];

	// Column 3
	temp_state[8] = state[8];
	temp_state[9] = state[5];
	temp_state[10] = state[2];
	temp_state[11] = state[15];

	// Column 4
	temp_state[12] = state[12];
	temp_state[13] = state[9];
	temp_state[14] = state[6];
	temp_state[15] = state[3];

	for (int i = 0; i < 16; i++)
		state[i] = temp_state[i];
}

/* Perform substitution to each of the 16 bytes
 * Uses inverse S-box as lookup table
 */
void SubBytes(unsigned char * state)
{
	for (int i = 0; i < 16; i++)
		state[i] = inv_s[state[i]];
}

/* Each round operates on 128 bits at a time
 * The number of rounds is defined in AESDecrypt()
 * Not surprisingly, the steps are the encryption steps but reversed
 */
void Round(unsigned char * state, unsigned char * key)
{
	SubRoundKey(state, key);
	InverseMixColumns(state);
	ShiftRows(state);
	SubBytes(state);
}

// Same as Round() but no InverseMixColumns
void InitialRound(unsigned char * state, unsigned char * key)
{
	SubRoundKey(state, key);
	ShiftRows(state);
	SubBytes(state);
}

/* The AES decryption function
 * Organizes all the decryption steps into one function
 */
void AESDecrypt(unsigned char * encryptedMessage, unsigned char * expandedKey, unsigned char * decryptedMessage)
{
	unsigned char state[16]; // Stores the first 16 bytes of encrypted message

	for (int i = 0; i < 16; i++) {
		state[i] = encryptedMessage[i];
	}

	InitialRound(state, expandedKey + 160);

	int numberOfRounds = 9;

	for (int i = 8; i >= 0; i--)
		Round(state, expandedKey + (16 * (i + 1)));

	SubRoundKey(state, expandedKey); // Final round

	// Copy decrypted state to buffer
	for (int i = 0; i < 16; i++)
		decryptedMessage[i] = state[i];
}

int main()
{

	// for getting input from input.txt
	freopen("input.txt", "r", stdin);
	// for writing output to output.txt
	freopen("output.txt", "w", stdout);

	cout << "=============================" << endl;
	cout << " 128-bit AES Decryption Tool " << endl;
	cout << "=============================" << endl;

	// Read in the message from message.aes
	string msgstr;
	ifstream infile;
	infile.open("message.aes", ios::in | ios::binary);

	char ch;
	int in;

	if (infile.is_open())
	{
		while (!infile.eof())
		{
			//getline(infile, msgstr); //test for one line
			ch = infile.get();
			msgstr[in++] = ch;
		}

		cout << "Read in encrypted message from message.aes\n";
	}

	else
		cout << "Unable to open file\n";

	char * msg = new char[msgstr.size() + 1];

	strcpy(msg, msgstr.c_str());

	int n = strlen((const char*)msg);

	unsigned char * encryptedMessage = new unsigned char[n];

	for (int i = 0; i < n; i++)
		encryptedMessage[i] = (unsigned char)msg[i];

	// Free memory
	delete[] msg;

	// Read in the key
	string keystr;
	ifstream keyfile;
	keyfile.open("keyfile.txt", ios::in | ios::binary);

	if (keyfile.is_open())
	{
		getline(keyfile, keystr); // The first line of file should be the key
		cout << "Read in the 128-bit key from keyfile\n";
		keyfile.close();
	}

	else
		cout << "Unable to open file\n";

	istringstream hex_chars_stream(keystr);
	unsigned char key[16];
	int i = 0;
	unsigned int c;

	while (hex_chars_stream >> hex >> c)
		key[i++] = c;

	unsigned char expandedKey[176];

	KeyExpansion(key, expandedKey);

	int messageLen = strlen((const char *)encryptedMessage);

	unsigned char * decryptedMessage = new unsigned char[messageLen];

	for (int i = 0; i < messageLen; i += 16)
		AESDecrypt(encryptedMessage + i, expandedKey, decryptedMessage + i);

	cout << "Decrypted message in hex:\n" ;

	for (int i = 0; i < messageLen; i++)
		cout << hex << (int)decryptedMessage[i] << " ";

	cout << endl;

	cout << "Decrypted message: ";

	for (int i = 0; i < messageLen; i++)
	{
		if (decryptedMessage[i] == '\x00')
			break;
		cout << decryptedMessage[i];
	}

	cout << endl;

	return 0;
}