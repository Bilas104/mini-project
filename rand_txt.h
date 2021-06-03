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

void rand_txt(char * str)
{
	// for getting input from input.txt
	freopen("input.txt", "r", stdin);
	// for writing output to output.txt
	freopen("output.txt", "w", stdout);

	srand((unsigned) time(0));

	int n = 500; // Number of elements in the text file

	int l = 33, u = 126; // Lower and Upper bound for printable ASCII characters

	int t = u - l + 1;

	int i;

	for (i = 0; i < n; ++i)
		str[i] = char((rand() % t) + l);

	str[i] = '\0';

	return;
}

#endif