#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<omp.h>

#define MAX_S	100

int S;								// Number of train stations
char *L[MAX_S];						// Name list of all stations
int M[MAX_S][MAX_S];				// Adjacency Matrix
float P[MAX_S];						// Popularity of each station
int G[MAX_S], Y[MAX_S], B[MAX_S];	// List of stations serviced by green/yellow/blue line
int G_size, Y_size, B_size;			// List size for G, Y, B
int N;								// Number of time ticks (time-units) in the simulation
int g, y, b;						// Number of trains per each line (green, yellow, blue)


void initData() {
	int i, j;
	char inputStr[MAX_S * 20], tempChar;
	char inputG[MAX_S * 20], inputY[MAX_S * 20], inputB[MAX_S * 20];
	const char delim[2] = ",";
	char *tempStr;
	
	// L
	scanf("%s", inputStr);
	L[0] = strtok(inputStr, delim);
	for (i = 1; i < S && L[0] != NULL; i++) {
		L[i] = strtok(NULL, delim);
	}
	// M
	for (i = 0; i < S; i++)
		for (j = 0; j < S; j++)
			scanf("%d", &M[i][j]);
	// P
	for (i = 0; i < S; i++) {
		scanf("%f", &P[i]);
		scanf("%c", &tempChar);
	}
	// G
	scanf("%s", inputG);
	tempStr = strtok(inputG, delim);
	while (tempStr != NULL) {
		for (i = 0; i < S; i++) {
			if (strcmp(tempStr, L[i]) == 0)
				G[G_size++] = i;
		}
		tempStr = strtok(NULL, delim);
	}
	// Y
	scanf("%s", inputY);
	tempStr = strtok(inputY, delim);
	while (tempStr != NULL) {
		for (i = 0; i < S; i++) {
			if (strcmp(tempStr, L[i]) == 0)
				Y[Y_size++] = i;
		}
		tempStr = strtok(NULL, delim);
	}
	// B
	scanf("%s", inputB);
	tempStr = strtok(inputB, delim);
	while (tempStr != NULL) {
		for (i = 0; i < S; i++) {
			if (strcmp(tempStr, L[i]) == 0)
				B[B_size++] = i;
		}
		tempStr = strtok(NULL, delim);
	}
	// N, g, y, b
	scanf("%d\n%d,%d,%d", &N, &g, &y, &b);
}

void testInput() {
	int i, j;
	printf("S: %d\n", S);
	for (i = 0; i < S; i++)
		printf("%s,", L[i]);
	printf("\n");
	for (i = 0; i < S; i++) {
		for (j = 0; j < S; j++)
			printf("%d ", M[i][j]);
		printf("\n");
	}
	for (i = 0; i < S; i++)
		printf("%.1f,", P[i]);
	printf("\n");
	for (i = 0; i < G_size; i++)
		printf("%s,", L[G[i]]);
	printf("\n");
	for (i = 0; i < Y_size; i++)
		printf("%s,", L[Y[i]]);
	printf("\n");
	for (i = 0; i < B_size; i++)
		printf("%s,", L[B[i]]);
	printf("\n");
	printf("%d\n%d,%d,%d\n", N, g, y, b);
}

int main(int argc, char const *argv[]) {
	scanf("%d\n", &S);
	initData();
	testInput();
	return 0;
}
