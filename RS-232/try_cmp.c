#include <stdlib.h>
#include <stdio.h>
#include <string.h>
static int arr_search(uint8_t *toSearch, int lenSearch, uint8_t *arr, int lenArr);
int main(int argc, char *argv[])
{

	printf("%d", arr_search(argv[1], strlen(argv[1]), argv[2], strlen(argv[2])));
	return 0;
}

int arr_search(uint8_t *toSearch, int lenSearch, uint8_t *arr, int lenArr)
{
	int pos_search = 0;
	int pos_arr = 0;
	printf("%d\n", lenArr);
	/* Start the loop for search */
	for (pos_arr = 0; pos_arr < lenArr + lenSearch; ++pos_arr) {

		printf("arr[pos_arr] = %c\n", arr[pos_arr]);
		printf("arr[pos_search] = %c\n", arr[pos_search]);

		if (arr[pos_arr] == toSearch[pos_search]) {
			++pos_search;
			if (pos_search == lenSearch) {	/* Match case */
				return pos_arr;
			}
		} else {
			pos_arr -= pos_search;
			pos_search = 0;
		}	/* Non-match case */
	}
	return -1;
}
