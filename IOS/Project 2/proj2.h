#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <pthread.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/wait.h>

// Global variable with is linked to outputFile
FILE *outFile;

/**
 * Sturcture Arguments used for storing parsed arguments
 * @param int Number of skiers
 * @param int Number of bus stops
 * @param int Capacity of ski bus
 * @param int Time before skier get to bus stop
 * @param int Time that bus takes to travel between bus stops
*/
typedef struct
{
    int L;
    int Z;
    int K;
    int TL;
    int TB;

} Arguments;

/**
 * Structure Semaphores used for easier semaphores passing to functions
 * @param sem_t* Semaphore providing unique access to output file writing
 * @param sem_t** Array of semaphores, which is used for people queing on each bus stop
 * @param sem_t* Semaphore used to let skiers in bus wait until they get to final bus stop
 * @param sem_t* Semaphore used to stop the bus, until all skiers get in or out
*/

typedef struct
{
    sem_t *write;
    sem_t *busStops[10];
    sem_t *skierInBus;
    sem_t *bus;

} Semaphores;

/**
 * Structure with counters used for correct program running
 * @param int Current line, incremented after every write to outFile
 * @param int* Counters of how many people is on each bus stop
 * @param int Variable used for holding information about free spaces in bus
 * @param int Variable used for holding information about how many people is currently in bus
 * @param int Variable, which counts how many skiers need to be brought to the ski slope before the bus finish
*/

typedef struct 
{
    int lineCounter;
    int busStopCounters[10];
    int busSpaces;
    int peopleInBus; // Can be calculated from busSpaces and bus capacity, but i am to lazy I guess
    int pendingSkiers;
} Values;

//Global variable because it contains just counters -> nothing so important
Values *values;

/**
 * Function write out help to STDOUT and exit with success exit code
*/
void printHelp();

/**
 * Function parse arguments given by user and stores them into structure given via arguments
 * @param Arguments* Struckture made in main, in which the argumnts will store into
 * @param int Amount of arguments
 * @param char** Array of arguments
*/
void argsParse( Arguments* , int , char** );

/**
 * Function semInit will inicialize all semaphores and other shared memory which is needed
 * @param Arguments* Structure in which are parsed arguments stored
 * @param Semaphores* Structure of semaphores, which have to be inicalized
*/
void semInit( Arguments* , Semaphores* );

/**
 * Function semDestruct will clean all memory which semaphores used
 * @param Arguments* Structure in which are parsed arguments stored
 * @param Semaphores* Structure of semaphores which have to be destructed
*/
void semDestruct( Arguments* , Semaphores* );

/**
 * Function which will make all skier work. Every child proccess will get into this function
 * @param int ID of skier
 * @param Arguments* Parsed arguments
 * @param Semaphores* Inicialized semaphores
*/
void skier( int , Arguments* , Semaphores* );

/**
 * Function will make all bus work. There is just one bus which is created after creating all child processes
 * @param Arguments* Parsed arguments
 * @param Semaphores* Inicialized semaphores
*/
void bus( Arguments*, Semaphores* );