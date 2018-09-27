#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <omp.h>

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
	const char delim[2] = ",";
	char *tempStr;
	
	// L
	scanf("%s", inputStr);
	tempStr = strtok(inputStr, delim);
	for (i = 0; i < S && tempStr != NULL; i++) {
		L[i] = tempStr;
		tempStr = strtok(NULL, delim);
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
	scanf("%s", inputStr);
	tempStr = strtok(inputStr, delim);
	while (tempStr != NULL) {
		for (i = 0; i < S; i++) {
			if (strcmp(tempStr, L[i]) == 0)
				G[G_size++] = i;
		}
	}
	// Y
	scanf("%s", inputStr);
	tempStr = strtok(inputStr, delim);
	while (tempStr != NULL) {
		for (i = 0; i < S; i++) {
			if (strcmp(tempStr, L[i]) == 0)
				Y[Y_size++] = i;
		}
	}
	// B
	scanf("%s", inputStr);
	tempStr = strtok(inputStr, delim);
	while (tempStr != NULL) {
		for (i = 0; i < S; i++) {
			if (strcmp(tempStr, L[i]) == 0)
				B[B_size++] = i;
		}
	}
	// N, g, y, b
	scanf("%d\n%d,%d,%d", &N, &g, &y, &b);
}


void simulate() {
	// set the number of threads
	omp_set_num_threads(g+y+b);

 	#pragma omp parallel private(tid)
 	{
 		tid = omp_get_thread_num();
 		printf("Hello world from thread %d", tid);
 	}

}

int main(int argc, char const *argv[]) {
	scanf("%d\n", &S);
	initData();
	return 0;
}
