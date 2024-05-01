#include "proj2.h"


void printHelp()
{
    printf("Help for proj2:\n");
    printf("Start with ./proj2 L Z K TL TB \n");
    printf("Where: \n");
    printf("L means amount of skiers, L < 20000 \n");
    printf("Z means amount of bus stops, 0 < Z <= 10 \n");
    printf("K means capacity of bus, 10 <= K <= 100 \n");
    printf("TL is max time in microsec, that skier wait before coming to bus stop, 0 <= TB <= 10000 \n");
    printf("TB is max time between each bus stop, 0 <= TB <= 1000 \n");

    exit(EXIT_SUCCESS);
}


void argsParse(Arguments *args, int argc, char* argv[])
{
    //help option is the only one that can have other amount of arguments than 6
    if (argc == 2 && !strcmp(argv[1], "-h"))
    {
        printHelp();
    }
    else if (argc != 6)
    {
        fprintf(stderr, "Wrong amount of arguments. \n Expected usage:\n ./proj2 L Z K TL TB\n Use -h for more informations.\n");
        exit(EXIT_FAILURE);
    }

    //First argument - L
    if (atoi(argv[1]) > 0 && atoi(argv[1]) < 20000)
    {
        args->L = atoi(argv[1]);
    }
    else
    {
        fprintf(stderr, "Wrong format or value of the first argument (L)\n");
        exit(EXIT_FAILURE);
    }

    //Second argument - Z
    if (atoi(argv[2]) > 0 && atoi(argv[2]) <= 10)
    {
        args->Z = atoi(argv[2]);
    }
    else
    {
        fprintf(stderr, "Wrong format or value of the second argument (Z)\n");
        exit(EXIT_FAILURE);
    }

    //Third argument - K
    if (atoi(argv[3]) >= 10 && atoi(argv[3]) <= 100)
    {
        args->K = atoi(argv[3]);
    }
    else
    {
        fprintf(stderr, "Wrong format or value of the third argument (K)\n");
        exit(EXIT_FAILURE);
    }

    //Forth argument - TL
    if (atoi(argv[4]) >= 0 && atoi(argv[4]) < 10000)
    {
        args->TL = atoi(argv[4]);
    }
    else
    {
        fprintf(stderr, "Wrong format or value of the forth argument (TL)\n");
        exit(EXIT_FAILURE);
    }

    //Fifth argument - TB
    if (atoi(argv[5]) >= 0 && atoi(argv[5]) <= 1000)
    {
        args->TB = atoi(argv[5]);
    }
    else
    {
        fprintf(stderr, "Wrong format or value of the fifth argument (TB)\n");
        exit(EXIT_FAILURE);
    }
}

void semInit(Arguments *args, Semaphores *semaphores)
{
    //Inicialization of write semaphore
    semaphores->write = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (sem_init(semaphores->write, 1, 1) == -1)
    {
        fprintf(stderr, "Error occured in proccess of semaphore inicialization\n");

        munmap(semaphores->write, sizeof(sem_t));

        exit(EXIT_FAILURE);
    }

    // Inicialization semaphores for requied number of bus stops
    for (int i = 0; i < args->Z; i++)
    {
        semaphores->busStops[i] = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
        if (sem_init(semaphores->busStops[i], 1, 0) == -1)
        {
            fprintf(stderr, "Error occured in proccess of semaphore inicialization\n");

            sem_destroy(semaphores->write);
            munmap(semaphores->write, sizeof(sem_t));

            for (int j = i -1; j >= 0; j--)
            {
                sem_destroy(semaphores->busStops[j]);
                munmap(semaphores->busStops[j], sizeof(sem_t));
            }

            munmap(semaphores->busStops[i], sizeof(sem_t));

            exit(EXIT_FAILURE);
        }
    }

    //Inicialization of semaphore for skiers already in bus
    semaphores->skierInBus = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (sem_init(semaphores->skierInBus, 1, 0) == -1)
    {
        fprintf(stderr, "Error occured in proccess of semaphore inicialization\n");

        sem_destroy(semaphores->write);
        munmap(semaphores->write, sizeof(sem_t));

        for (int i = 0; i < args->Z; i++)
        {
            sem_destroy(semaphores->busStops[i]);
            munmap(semaphores->busStops[i], sizeof(sem_t));
        }

        munmap(semaphores->skierInBus, sizeof(sem_t));

        exit(EXIT_FAILURE);
    }

    //Inicialization of semaphore for bus
    semaphores->bus = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (sem_init(semaphores->bus, 1, 0) == -1)
    {
        fprintf(stderr, "Error occured in proccess of semaphore inicialization\n");

        sem_destroy(semaphores->write);
        munmap(semaphores->write, sizeof(sem_t));

        for (int i = 0; i < args->Z; i++)
        {
            sem_destroy(semaphores->busStops[i]);
            munmap(semaphores->busStops[i], sizeof(sem_t));
        }

        sem_destroy(semaphores->skierInBus);
        munmap(semaphores->skierInBus, sizeof(sem_t));

        munmap(semaphores->bus, sizeof(sem_t));

        exit(EXIT_FAILURE);
    }

    // Values has to be in shared memory as well
    values = mmap(NULL, sizeof(Values), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    // Set initial values in values structure
    values->lineCounter = 1;
    values->busSpaces = args->K;
    values->peopleInBus = 0;
    values->pendingSkiers = args->L;
    for (int i = 0; i < 10; i++)
    {
        values->busStopCounters[i] = 0;
    }
}

void semDestruct(Arguments *args, Semaphores *semaphores)
{
    // Clean write semaphore
    sem_destroy(semaphores->write);
    munmap(semaphores->write, sizeof(sem_t));
    
    // Clean bus stops semaphores
    for(int i = 0; i < args->Z; i++)
    {
        sem_destroy(semaphores->busStops[i]);
        munmap(semaphores->busStops[i], sizeof(sem_t));
    }

    // Clean skiers in bus semaphore
    sem_destroy(semaphores->skierInBus);
    munmap(semaphores->skierInBus, sizeof(sem_t));

    // Clean bus semaphore
    sem_destroy(semaphores->bus);
    munmap(semaphores->bus, sizeof(sem_t));

    // Clean values shared memory
    munmap(values, sizeof(Values));
}

void skier(int id, Arguments *args, Semaphores *semaphores)
{
    // Write start line
    sem_wait(semaphores->write);
    fprintf(outFile, "%d: L %d: started\n", values->lineCounter++, id );
    sem_post(semaphores->write);

    // Get random seed for rand
    srand(time(NULL) + id + values->lineCounter + time(NULL));
    // Get random number between zero and TL to sleep process for that time
    int sleepTime = rand() / (RAND_MAX / (args->TL + 1) + 1) * 1000;
    usleep(sleepTime);

    // Get skiers bus stop
    srand(time(NULL) + id + values->lineCounter + time(NULL));
    int busStop = (rand() % (args->Z)) + 1;

    // Go to specific bus stop
    sem_wait(semaphores->write);
    fprintf(outFile, "%d: L %d: arrived to %d\n", values->lineCounter++, id, busStop);
    sem_post(semaphores->write);

    // People on that bus stop increase
    values->busStopCounters[busStop - 1] += 1;

    // Wait until empty bus will post this semaphore
    sem_wait(semaphores->busStops[busStop - 1]);

    // Boarding
    sem_wait(semaphores->write);
    fprintf(outFile, "%d: L %d: boarding\n", values->lineCounter++, id);
    sem_post(semaphores->write);

    // Decrease number of skiers waiting on bus stop
    values->busStopCounters[busStop - 1] -= 1;
    // Increase number of people in bus
    values->peopleInBus++;
    // Skier is succesfully in bus
    sem_post(semaphores->bus);

    // Wait until the bus gets to the final station
    sem_wait(semaphores->skierInBus);

    sem_wait(semaphores->write);
    fprintf(outFile, "%d: L %d: going to ski\n", values->lineCounter++, id);
    sem_post(semaphores->write);

    // Skier is out of the bus
    sem_post(semaphores->bus);

    // Skier went skiing, our work ends here :)
    fclose(outFile);
    return;
}

void bus(Arguments *args, Semaphores *semaphores)
{
    // Information about actual bus stop
    int actualBusStop = 1;

    sem_wait(semaphores->write);
    fprintf(outFile, "%d: BUS: started\n", values->lineCounter++);
    sem_post(semaphores->write);

    // Get random seed for rand
    srand(time(NULL) + values->lineCounter + time(NULL));
    // Get random number between zero and TB to sleep process for that time
    int sleepTime = rand() / (RAND_MAX / (args->TB + 1) + 1) * 1000;
    usleep(sleepTime);

    do
    {
        // Go through every bus stop and pick up people
        for(actualBusStop = 1; actualBusStop < args->Z + 1; actualBusStop++)
        {
            sem_wait(semaphores->write);
            fprintf(outFile, "%d: BUS: arrived to %d\n", values->lineCounter++, actualBusStop);
            sem_post(semaphores->write);

            // If there is someone on the bus stop and bus have free capacity
            if (values->busStopCounters[actualBusStop - 1] > 0 && values->busSpaces > 0)
            {
                // If there is more spaces in bus than people on the bus stop
                if (values->busSpaces > values->busStopCounters[actualBusStop - 1])
                {
                    int peopleAtBusStop = values->busStopCounters[actualBusStop - 1];
                    for (int i = 0; i < peopleAtBusStop; i++)
                    {
                        sem_post(semaphores->busStops[actualBusStop - 1]);
                        // Wait until all skiers get into bus
                        sem_wait(semaphores->bus);
                    }
                }
                // Else get as many people as capacity of the bus will allow
                else
                {
                    for (int i = 0; i < values->busSpaces; i++)
                    {
                        sem_post(semaphores->busStops[actualBusStop - 1]);
                        // Wait until all skiers get into bus
                        sem_wait(semaphores->bus);
                    }
                }
            }
            sem_wait(semaphores->write);
            fprintf(outFile, "%d: BUS: leaving %d\n", values->lineCounter++, actualBusStop);
            sem_post(semaphores->write);
        }

        // Arrive to final destination
        sem_wait(semaphores->write);
        fprintf(outFile, "%d: BUS: arrived to final\n", values->lineCounter++);
        sem_post(semaphores->write);
        // Post semaphore so skiers can get out of the bus
        int peopleInBus = values->peopleInBus;
        for (int i = 0; i < peopleInBus; i++)
        {
            sem_post(semaphores->skierInBus);
            // Decrease number of skiers and bus and skiers to be brought to slope as well
            values->peopleInBus--;
            values->pendingSkiers--;
            // Wait for all people to get out from the bus
            sem_wait(semaphores->bus);
        }

        sem_wait(semaphores->write);
        fprintf(outFile, "%d: BUS: leaving final\n", values->lineCounter++);
        sem_post(semaphores->write);

    //Do that again and again until all skiers were brought to slope successfully
    } while (values->pendingSkiers > 0 );

    sem_wait(semaphores->write);
    fprintf(outFile, "%d: BUS: finish\n", values->lineCounter++);
    sem_post(semaphores->write);
}

int main(int argc, char *argv[])
{
    // Parse arguments and store them into args structure 
    Arguments args = {-1, -1, -1, -1, -1};
    argsParse(&args, argc, argv);

    // Open or create output file
    outFile = fopen("proj2.out", "w");
    if (outFile == NULL)
    {
        fprintf(stderr, "Error occured when opening/creating output file\n");
        exit(EXIT_FAILURE);
    }

    // Get rid of buffering output -> Everything will be written out instantly
    setbuf(outFile, NULL);

    //Semaphores structure create and inicialization after that
    Semaphores semaphores = {NULL, {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}, NULL, NULL};
    semInit(&args, &semaphores);

    //Create all skiers processes
    for (int i = 1; i < args.L + 1; i++)
    {
        pid_t id = fork();
        if (id < 0)
        {
            fprintf(stderr, "Error occured with creating skiers processes\n");

            semDestruct(&args, &semaphores);
            exit(EXIT_FAILURE);
        }
        else if (id == 0)
        {
            // It's child proccess -> skier
            skier(i, &args, &semaphores);
            // Exit the child proccess after success
            exit(0);
        }
    }
    // Create bus as well
    bus(&args, &semaphores);

    // Wait until all child processes finished
    while(wait(NULL) > 0);

    // Clear memory after the bus finishes work
    semDestruct(&args, &semaphores);
    fclose(outFile);

    return 0;
}