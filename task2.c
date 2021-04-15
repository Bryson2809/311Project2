/**
 * @file task2.c
 * @author James Carroll (jbc2@email.sc.edu)
 * @brief Uses mutex locking in order to schedule the movement of BATS at an intersection
 * @version 0.1
 * @date 2021-04-13
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <stdio.h>
#include <pthread.h>
#include <string.h>

typedef struct {
	int id_num; //id
	char direct; // direction they are coming from 
	char right; // which is to the right
} bat;

//One mutex to get into ALL functions 
pthread_mutex_t mutex;

//Some counters: 
int total = 0; //total at intersections
int northBats = 0; 
int southBats = 0;
int westBats = 0;
int eastBats = 0;

pthread_cond_t eastQueue;
pthread_cond_t southQueue;
pthread_cond_t westQueue;
pthread_cond_t northQueue;

pthread_cond_t eastFirst;
pthread_cond_t southFirst;
pthread_cond_t westFirst;
pthread_cond_t northFirst;

void *goBatGo(void *arg);
void doTheRest(void *arg);
void *arrive(void *arg);
void *cross(void *arg);
void *leave(void *arg);
void *check(void *arg);

int main(int argc, char **argv){
	//Let's safely assume that we will at most have 
	//100 BATs
	char directions[100]; 
	strcpy(directions, argv[1]);
	printf("Argc: %i and directions %s\n", argc, directions);
	bat bats[strlen(directions)];
	//1) loop through input (strlen(directions)) and 
	//   create array of bat structs
	for(int i = 0; i < strlen(directions); i++)
	{
		bat temp;
		temp.id_num = i;
		temp.direct = directions[i];
		if (temp.direct = 'w')
			temp.right = 's';
		else if (temp.direct = 'n')
			temp.right = 'w';
		else if (temp.direct = 'e')
			temp.right = 'n';
		else if (temp.direct = 's')
			temp.right = 'e';
		bats[i] = temp;
	}
	
	
	//2) loop through bats now and dispatch all of
	//   the threads 
	// maybe send to a function that takes in a 
	// bat struct... 
	for(int i = 0; i < (sizeof(bats) / sizeof(bats[0])); i++)
		printf("bats\n");
	
	return 0; 
}

void *goBatGo(void *arg){
	printf("goBatGoRuns");
	bat *b = (bat *) arg; 
	check(b);
	doTheRest(b);
}

void doTheRest(void *arg){
	printf("DTR runs");
	bat *b = (bat *) arg;
	arrive(b);
	cross(b);
	leave(b);
}

void *arrive(void *arg)
{
	printf("arrive runs");
	bat *b = (bat *) arg; 
	pthread_mutex_lock(&mutex);
	if (b->direct == 'n') 
	{ 
		northBats++;
	    if (northBats > 1)
	       pthread_cond_wait(&northQueue, &mutex);
	}
	else if (b->direct == 'e')
	{
		eastBats++;
		if (eastBats > 1)
			pthread_cond_wait(&eastQueue, &mutex);
	}
	else if (b->direct == 's')
	{
		southBats++;
		if (southBats > 1)
			pthread_cond_wait(&southQueue, &mutex);
	}
	else if (b->direct == 'w')
	{
		westBats++;
		if (westBats > 1)
			pthread_cond_wait(&westQueue, &mutex);
	}

	total++;
	pthread_mutex_unlock(&mutex);
}

void *cross(void *arg)
{
	printf("cross runs");
	bat *b = (bat *) arg; 
	pthread_mutex_lock(&mutex);
	if (b->right == 'n' && northBats > 0) 
	    pthread_cond_wait(&northFirst, &mutex); 
	else if (b->right == 'e' && eastBats > 0)
		pthread_cond_wait(&eastFirst, &mutex);
	else if (b->right == 's' && southBats > 0)
		pthread_cond_wait(&southFirst, &mutex);
	else if (b->right == 'w' && westBats > 0)
		pthread_cond_wait(&westFirst, &mutex);
	
	if (b->direct == 'n')
	{
		northBats--;
		pthread_cond_signal(&northQueue);
	}
	else if (b->direct == 'e')
	{
		eastBats--;
		pthread_cond_signal(&eastQueue);
	}
	else if (b->direct == 's')
	{
		southBats--;
		pthread_cond_signal(&southQueue);
	}
	else if (b->direct == 'w')
	{
		westBats--;
		pthread_cond_signal(&westQueue);
	}

	total--; 
	pthread_mutex_unlock(&mutex);
}

void *leave(void *arg)
{
	printf("leave runs");
	bat *b = (bat *) arg;
	pthread_mutex_lock(&mutex);
	if (b->direct == 'n')
	    pthread_cond_signal(&northFirst);
	else if (b->direct == 'e')
		pthread_cond_signal(&eastFirst);
	else if (b->direct == 's')
		pthread_cond_signal(&southFirst);
	else if (b->direct == 'w')
		pthread_cond_signal(&westFirst);
	
	pthread_mutex_unlock(&mutex);
}
void *check(void *arg)
{
	printf("check runs");
	pthread_mutex_lock(&mutex);
	bat *b = (bat *) arg;
	if (total == 4)
	{
		if (b->direct == 'n')
			pthread_cond_signal(&northFirst);
		else if (b->direct == 'e')
			pthread_cond_signal(&eastFirst);
		else if (b->direct == 's')
			pthread_cond_signal(&southFirst);
		else if (b->direct == 'w')
			pthread_cond_signal(&westFirst);
	}
	pthread_mutex_unlock(&mutex);
}



