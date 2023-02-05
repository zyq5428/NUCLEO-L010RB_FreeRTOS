/*
 * type_to_ascii.c
 *
 *  Created on: Jan 18, 2023
 *
 */

/*
 * size <= 12
 */

int int_to_ascii( int i, char *out, unsigned char *size, unsigned char need_size, char ascii)
{
	int t;
	unsigned char m, count = 0;
	unsigned int u = i;
	char s[12];

	while (u) {
		t = u % 10;
		s[count] = t + '0';
		count++;
		u /= 10;
	}
	while ( (need_size != 0) && (count < need_size)) {
		s[count] = ascii;
		count++;
	}

	*size = count;

	for ( m = 0; m < count; m++) {
		out[count - 1 - m] = s[m];
	}
	return 0;
}



