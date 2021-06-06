#ifndef READ_FILE_H
#define READ_FILE_H

#include <iostream>
#include <fstream>
#include <string.h>

using namespace std;

void read_txt(unsigned char * str)
{
	//for getting input from input.txt
 	//freopen("input.txt", "r", stdin);
	//for writing output to output.txt
	//freopen("output.txt", "w", stdout);

	FILE * f = fopen("message.aes", "rb");

	fseek(f, 0, SEEK_END);

	long fsize = ftell(f);

	fseek(f, 0, SEEK_SET);

	//str = new char [fsize + 1];
	fread(str, fsize, 1, f);
	fclose(f);

	str[fsize] = '\0';
	
	return;
}

#endif
