#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <omp.h>

#define MAX_S		100
#define MAX_LENGTH  100	

int S;								// Number of train stations
char L[MAX_S][MAX_LENGTH];			// Name list of all stations
int M[MAX_S][MAX_S];				// Adjacency Matrix
float P[MAX_S];						// Popularity of each station
int G[MAX_S], Y[MAX_S], B[MAX_S];	// List of stations serviced by green/yellow/blue line
int G_size, Y_size, B_size;			// List size for G, Y, B
int N;								// Number of time ticks (time-units) in the simulation
int g, y, b;						// Number of trains per each line (green, yellow, blue)


#define WAITING_TO_LOAD 0
#define LOADING 1
#define WAITING_TO_LEAVE 2
#define RUNNING 3


struct Status {
	int status;
	int from;
	int to;
	int direction;
};

void initData() {
	int i, j;
	char inputStr[MAX_S * 20], tempChar;
	char inputG[MAX_S * 20], inputY[MAX_S * 20], inputB[MAX_S * 20];
	const char delim[2] = ",";
	char *tempStr;
	
	// L
	scanf("%s", inputStr);

	strcpy(L[0], strtok(inputStr, delim));
	// L[0] = strtok(inputStr, delim);
	for (i = 1; i < S && L[0] != NULL; i++) {
		strcpy(L[i], strtok(NULL, delim));
		// L[i] = strtok(NULL, delim);
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


void simulate() {
	// set the number of threads
	omp_set_num_threads(g+y+b+1);
	const int num_trains = g + y + b;
	int tid, i, j, k;
	int cur_time = 0;
	int count = 0;
	int flag = 0;
	int active = 0;

	int stations[S], waitingTime[S], edges[S][S], visited[S];
	omp_lock_t sem1, sem2;
	int loadRemain = 0, runRemain = 0;

	for (i = 0; i < S; i++) {
		stations[i] = 0;
		waitingTime[i] = 0;
		visited[i] = 0;
		for (j = 0; j < S; j++) {
			edges[i][j] = 0;
		}
	}

	struct Status status[num_trains+1];
	for (i = 0; i < num_trains + 1; i++)
		status[i].status = WAITING_TO_LOAD;

	omp_init_lock(&sem1);
	omp_init_lock(&sem2);

	omp_set_lock(&sem1);

 	#pragma omp parallel private(tid, i, j, k, active, loadRemain, runRemain)
 	{
 		int X_Size, trainIndex;
 		int *X_Arr;

 		active = 0;
 		tid = omp_get_thread_num();

 		if (tid != 0) {
			if (tid <= g && tid > 0) {
				X_Size = G_size;
				X_Arr = G;
				trainIndex = tid - 1;
			} else if (tid > g && tid <= g+y) {
				X_Size = Y_size;
				X_Arr = Y;
				trainIndex = tid - 1 - g;
			} else if (tid > g+y && tid < g+y+b+1){
				X_Size = B_size;
				X_Arr = B;
				trainIndex = tid - 1 - g - y;
			} 

	 		for (i = 0; i < N * 10; i++) {
				if (active == 0) {
					int c = (int)(i/10);
					if (trainIndex == c * 2) {
						active = 1;
						status[tid].from = X_Arr[0];
						status[tid].to = X_Arr[0];
						status[tid].direction = 0;
					} else if (trainIndex == c * 2 + 1) {
						active = 1;
						status[tid].from = X_Arr[X_Size - 1];
						status[tid].to = X_Arr[X_Size - 1];
						status[tid].direction = 1;
					}
				}
				if (active == 1) {
					#pragma omp critical
					{
						if (flag == 0) {
							// if (i % 10 == 0) {
			 			// 		for (j = 0; j < S; j++) {
			 			// 			printf("%d ", stations[j]);
			 			// 		}
			 			// 		printf("\n");
		 				// 	}
		 					for (j = 0; j < S; j++) {
		 						if (stations[j] > 0) {
		 							stations[j] -= 1;
		 						} else {
		 							if (visited[j] != 0) {
		 								waitingTime[j] += 1;
		 							}
		 						}
		 						for (k = 0; k < S; k++) {
		 							if (edges[j][k] > 0) {
		 								edges[j][k] -= 1;
		 							}
		 						}
		 					}
	 					}
	 					flag = 1;
					}
					if (status[tid].status == WAITING_TO_LOAD) {
						// check if can load.
						# pragma omp critical
						{
		 					if (stations[status[tid].from] == 0) {
		 						// printf("wait_to_load=>load\n");
								stations[status[tid].from] = (rand() % 10 + 1) * P[status[tid].from] * 10;
								status[tid].status = LOADING;
								loadRemain = stations[status[tid].from];
		 					}
						}
					} else if (status[tid].status == LOADING) {
						visited[status[tid].from] = 1;
						loadRemain -= 1;
						// printf("current load remain is %d\n", loadRemain);
						if (loadRemain == 0) {
							int curPosIndex;
							for (j = 0; j < X_Size; j++) {
								if (status[tid].from == X_Arr[j]) {
									curPosIndex = j;
									break;
								}
							}
							if (status[tid].direction == 0) {
								if (curPosIndex == (X_Size - 1)) { // reach the end
									status[tid].to = X_Arr[X_Size-2];
									status[tid].direction = 1;
								} else {
									status[tid].to = X_Arr[curPosIndex+1];
								}
							} else {
								if (curPosIndex == 0) {
									status[tid].to = X_Arr[1];
									status[tid].direction = 0;
								} else {
									status[tid].to = X_Arr[curPosIndex-1];
								}
							}
							#pragma omp critical
							{
								if (edges[status[tid].from][status[tid].to] == 0) {
									edges[status[tid].from][status[tid].to] = M[status[tid].from][status[tid].to] * 10;
									status[tid].status = RUNNING;
									// printf("loading => run\n");
									runRemain = edges[status[tid].from][status[tid].to];
								} else {
									// printf("loading => wait_to_leave\n");
									status[tid].status = WAITING_TO_LEAVE;
								}
							}
						}
					} else if (status[tid].status == WAITING_TO_LEAVE)  {
						#pragma omp critical
						{
							if (edges[status[tid].from][status[tid].to] == 0) {
								edges[status[tid].from][status[tid].to] = M[status[tid].from][status[tid].to] * 10;
								status[tid].status = RUNNING;
								// printf("waiting_to_leave => run\n");
							}
						}
					} else if (status[tid].status == RUNNING) {
						runRemain -= 1;
						if (runRemain == 0) {
							status[tid].from = status[tid].to;

							#pragma omp critical
							{
								if (stations[status[tid].from] == 0) {
				 					stations[status[tid].from] = (rand() % 10 + 1) * P[status[tid].from] * 10;
									status[tid].status = LOADING;
									// printf("run => load\n");
									loadRemain = stations[status[tid].from];
								} else {
									// printf("run => wait_to_load\n");
									status[tid].status = WAITING_TO_LOAD;
								}
							}
						}
					} 
				}
				#pragma omp critical
				{
					count++;
					if (count == num_trains) {
						omp_set_lock(&sem2);
						omp_unset_lock(&sem1);
					}
				}

				omp_set_lock(&sem1);
				omp_unset_lock(&sem1);

				#pragma omp critical 
				{
					count--;
					if (count == 0) {
						omp_set_lock(&sem1);
						omp_unset_lock(&sem2);
						flag = 0;

						// if (i % 10 == 0) {
						// 	printf("%d:  ", i / 10);
						// 	int c1 = (int)(i/10);
						// 	for (j = 1; j <= num_trains; j++) {
						// 		char color;
						// 		int index;

						// 		if (j <= g && j > 0) {
						// 			color = 'g';
						// 			index = j - 1;
						// 		} else if (j > g && j <= g+y) {
						// 			color = 'y';
						// 			index = j - 1 - g;
						// 		} else if (j > g+y && j < g+y+b+1){
						// 			color = 'b';
						// 			index = j - 1 - g - y;
						// 		} 


						// 		if (index <= c1 * 2 + 1) {
						// 			printf("%c%d-", color, index);
						// 			if (status[j].status != RUNNING) {
						// 				printf("s%d, ", status[j].from);
						// 			} else {
						// 				printf("s%d->s%d, ", status[j].from, status[j].to);
						// 			}
						// 		}
						// 	}
						// 	printf("\n");	
						// }
					}
				}

				// if (tid == 1 && i % 10 == 0) {
				// 	int n;
				// 	for (n = 1; n <= g; n++) {
				// 		printf("%d => %d  ", status[n+g].from, status[n+g].to);
				// 	}
				// 	printf("\n");
				// }
				omp_set_lock(&sem2);
				omp_unset_lock(&sem2);
			}
		}
 	}
 	float g_sum = 0.0, y_sum = 0.0, b_sum = 0.0;
 	printf("\n\nAverage waiting time:\n");
 	for (i = 0; i < G_size; i++) {
 		g_sum += waitingTime[G[i]] / 10.0;
 	}
 	for (i = 0; i < Y_size; i++) {
 		y_sum += waitingTime[Y[i]] / 10.0;
 	}
 	for (i = 0; i < B_size; i++) {
 		b_sum += waitingTime[B[i]] / 10.0;
 	}

 	printf("=========================\n");

 	for (i = 0; i < S; i++) {
 		printf("%d ", waitingTime[i]);
 	}

 	printf("\n=========================\n");

	printf("green: %d trains -> %f\n", g, g_sum/G_size);
	printf("yellow: %d trains -> %f\n", y, y_sum/Y_size);
	printf("blue: %d trains -> %f\n", b, b_sum/B_size);
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
		printf("%d,", G[i]);
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
	srand((unsigned)time(NULL));
	initData();
	// testInput();
	simulate();
	return 0;
}
