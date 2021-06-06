/* "rand_txt.h" contains the function rand_txt()
 * generates random stream of characters and stores them
 * in the string given as function parameter.
 */

#ifndef RAND_TXT_H
#define RAND_TXT_H

#include <cstdlib>
#include <ctime>
#include <iostream>
using namespace std;

void rand_txt(int n)
{
	srand((unsigned) time(0));

	//int n = 500; // Number of elements in the text file

	int l = 33, u = 126; // Lower and Upper bound for printable ASCII characters

	int t = u - l + 1;

	int i;

	unsigned char * str;
	str = new unsigned char [n];

	for (i = 0; i < n; ++i)
		str[i] = char((rand() % t) + l);

	FILE * fp = fopen("rand_txt.txt", "w");
	fprintf(fp, "%s", str);

	fclose(fp);

	cout << endl;

	return;
}

#endif
