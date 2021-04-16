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
#include <unistd.h>

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

//Condition variables
pthread_cond_t eastQueue;
pthread_cond_t southQueue;
pthread_cond_t westQueue;
pthread_cond_t northQueue;

pthread_cond_t eastFirst;
pthread_cond_t southFirst;
pthread_cond_t westFirst;
pthread_cond_t northFirst;

//Function Declerations
void *goBatGo(bat b);
void *doTheRest(bat b);
void *arrive(bat b);
void *cross(bat b);
void *leave(bat b);
void *check(bat b);

int main(int argc, char **argv)
{
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
		if (temp.direct == 'w')
			temp.right = 's';
		else if (temp.direct == 'n')
			temp.right = 'w';
		else if (temp.direct == 'e')
			temp.right = 'n';
		else if (temp.direct == 's')
			temp.right = 'e';
		bats[i] = temp;
	}
	
	
	//2) loop through bats now and dispatch all of
	//   the threads 
	// maybe send to a function that takes in a 
	// bat struct... 
	for(int i = 0; i < (sizeof(bats) / sizeof(bats[0])); i++)
    {
        goBatGo(bats[i]);
    }
	return 0; 
}

void *goBatGo(bat b)
{
	check(b);
	doTheRest(b);
}

void *doTheRest(bat b)
{
	arrive(b);
	cross(b);
	leave(b);
}

/**
 * This function handles the bats arriving and should casue them to wait at the crossroads but they move on imediately instead
 **/
void *arrive(bat b)
{
	pthread_mutex_lock(&mutex);
	if (b.direct == 'n') 
	{ 
        printf("BAT %d", b.id_num);
        printf(" from North arrives at crossing\n");
		northBats++;
	    if (northBats > 1)
	       pthread_cond_wait(&northQueue, &mutex);
	}
	else if (b.direct == 'e')
	{
        printf("BAT %d", b.id_num);
        printf(" from East arrives at crossing\n");
		eastBats++;
		if (eastBats > 1)
			pthread_cond_wait(&eastQueue, &mutex);
	}
	else if (b.direct == 's')
	{
        printf("BAT %d", b.id_num);
        printf(" from South arrives at crossing\n");
		southBats++;
		if (southBats > 1)
			pthread_cond_wait(&southQueue, &mutex);
	}
	else if (b.direct == 'w')
	{
        printf("BAT %d", b.id_num);
        printf(" from West arrives at crossing\n");
		westBats++;
		if (westBats > 1)
			pthread_cond_wait(&westQueue, &mutex);
	}

	total++;
	pthread_mutex_unlock(&mutex);
}

/**
 * I beleive that the issue with my code is in this function. 
 * I am not able to figure out how to get the bats to wait at
 * the crossroads and they are instead leaving as soon as they arrive.
**/
void *cross(bat b)
{
    sleep(1);
	pthread_mutex_lock(&mutex);
	if (b.right == 'n' && northBats > 0) 
    {
	    pthread_cond_wait(&northFirst, &mutex); 
    }
	else if (b.right == 'e' && eastBats > 0)
    {
		pthread_cond_wait(&eastFirst, &mutex);
    }
	else if (b.right == 's' && southBats > 0)
    {
		pthread_cond_wait(&southFirst, &mutex);
    }
	else if (b.right == 'w' && westBats > 0)
    {
		pthread_cond_wait(&westFirst, &mutex);
    }

	if (b.direct == 'n')
	{
		northBats--;
		pthread_cond_signal(&northQueue);
	}
	else if (b.direct == 'e')
	{
		eastBats--;
		pthread_cond_signal(&eastQueue);
	}
	else if (b.direct == 's')
	{
		southBats--;
		pthread_cond_signal(&southQueue);
	}
	else if (b.direct == 'w')
	{
		westBats--;
		pthread_cond_signal(&westQueue);
	}
	total--; 
	pthread_mutex_unlock(&mutex);
}

/**
 * This function is responsible for making the bats leave the crossroad when it is their time
 **/
void *leave(bat b)
{
	pthread_mutex_lock(&mutex);
	if (b.direct == 'n')
    {
        pthread_cond_signal(&northFirst);
        printf("BAT %d", b.id_num);
        printf(" from North leaves the crossing\n");
    }   
	else if (b.direct == 'e')
    {
        pthread_cond_signal(&eastFirst);
        printf("BAT %d", b.id_num);
        printf(" from East leaves the crossing\n");
    }
	else if (b.direct == 's')
    {
        pthread_cond_signal(&southFirst);
        printf("BAT %d", b.id_num);
        printf(" from South leaves the crossing\n");
    }
	else if (b.direct == 'w')
    {
        pthread_cond_signal(&westFirst);
        printf("BAT %d", b.id_num);
        printf(" from West leaves the crossing\n");
    }
	pthread_mutex_unlock(&mutex);
}

/**
 * This function is responsible for checking for deadlock, I am not sure if this one works since I can not get the bats to wait
 **/
void *check(bat b)
{
	pthread_mutex_lock(&mutex);
	if (total == 4)
	{
        printf("BAT jam detected,");
		if (b.direct == 'n')
        {
            pthread_cond_signal(&northFirst);
            printf(" signaling north to go\n");
        }	
		else if (b.direct == 'e')
        {
            pthread_cond_signal(&eastFirst);
            printf(" signaling east to go\n");
        }
		else if (b.direct == 's')
        {
            pthread_cond_signal(&southFirst);
            printf(" signaling south to go\n");
        }
			
		else if (b.direct == 'w')
        {
            pthread_cond_signal(&westFirst);
            printf(" singaling west to go\n");
        }
	}
	pthread_mutex_unlock(&mutex);
}
