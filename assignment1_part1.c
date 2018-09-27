#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int S;					// Number of train stations
char **L;				// Name list of all stations
int M[100][100];					// Adjacency Matrix
float *P;				// Popularity of each station
int *G, *Y, *B;			// List of stations serviced by green/yellow/blue line
int N;					// Number of time ticks (time-units) in the simulation
int g, y, b;			// Number of trains per each line (green, yellow, blue)


void initData() {
	int i, j;
	char inputStr[1000], tempChar;
	const char delim[2] = ",";
	char *tempStr;
	char *nameList[S];
	float p[S];
	
	// L
	scanf("%s", inputStr);
	tempStr = strtok(inputStr, delim);
	for (i = 0; i < S && tempStr != NULL; i++) {
		nameList[i] = tempStr;
		tempStr = strtok(NULL, delim);
	}
	L = nameList;
	// M
	for (i = 0; i < S; i++)
		for (j = 0; j < S; j++)
			scanf("%d", &M[i][j]);
	// P
	for (i = 0; i < S; i++) {
		scanf("%f", &p[i]);
		scanf("%c", &tempChar);
	}
	P = p;
	// G
	// scanf("%s", inputStr);
	// tempStr = strtok(inputStr, delim);
	// while (tempStr != NULL) {

	// }
}


int main(int argc, char const *argv[]) {
	int i;
	scanf("%d\n", &S);
	initData();
	for (i = 0; i < S; i++)
		printf("%s\n", L[i]);
	return 0;
}
