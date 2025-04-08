
#include <pthread.h>		//Create POSIX threads.
#include <time.h>			//Wait for a random time.
#include <unistd.h>			//Thread calls sleep for specified number of seconds.
#include <semaphore.h>		//To create semaphores
#include <stdlib.h>			
#include <stdio.h>			//Input Output

pthread_t *Students;		//N threads running as Students.
pthread_t TA;				//Separate Thread for TA.

int ChairsCount = 0;
int CurrentIndex = 0;

/*Should be complete now, please check the code below and make sure it is correct.
 
//Declaration of Semaphores and Mutex Lock.
//Semaphores used:
//A semaphore to signal and wait TA's sleep.
//An array of 3 semaphores to signal and wait chair to wait for the TA.
//A semaphore to signal and wait for TA's next student.
 
 //Mutex Lock used:
//To lock and unlock variable ChairsCount to increment and decrement its value.
 
 //hint: use sem_t and pthread_mutex_t
 
 
 */
sem_t TASemaphore;
sem_t ChairsSemaphores[3];
sem_t NextStudentSemaphore;

pthread_mutex_t ChairsCountMutex;


//Declared Functions
void *TA_Activity();
void *Student_Activity(void *threadID);

int main(int argc, char* argv[])
{
	int number_of_students;		//a variable taken from the user to create student threads.	Default is 5 student threads.
	int id;
	srand(time(NULL));

    /*Please check code below and make sure it is correct.
	//Initializing Mutex Lock and Semaphores.
	
     //hint: use sem_init() and pthread_mutex_init()
     
     */
	
	if (sem_init(&TASemaphore, 0, 0) != 0)
	{
		perror("Failed to initialize TA Semaphore");
		exit(EXIT_FAILURE);
	}
	for(int i=0; i<3; i++)
	{
		if (sem_init(&ChairsSemaphores[i], 0, 0) != 0)
		{
			printf("Failed to initialize Chair Semaphore number %d\n", i);
			perror("Failed to initialize Chair Semaphore");
			exit(EXIT_FAILURE);
		}
	}
	if (sem_init(&NextStudentSemaphore, 0, 0) != 0)
	{
		perror("Failed to initialize Next Student Semaphore");
		exit(EXIT_FAILURE);
	}

	if (pthread_mutex_init(&ChairsCountMutex, NULL) != 0)
	{
		perror("Failed to initialize Mutex Lock");
		exit(EXIT_FAILURE);
	}

	if(argc<2)
	{
		printf("Number of Students not specified. Using default (5) students.\n");
		number_of_students = 5;
	}
	else
	{
		printf("Number of Students specified. Creating %d threads.\n", number_of_students);
		number_of_students = atoi(argv[1]);
	}
		
	//Allocate memory for Students
	Students = (pthread_t*) malloc(sizeof(pthread_t)*number_of_students);

    /*TODO
	//Creating one TA thread and N Student threads.
     //hint: use pthread_create

	//Waiting for TA thread and N Student threads.
     //hint: use pthread_join
     
     */
	
	// create TA thread 
	if (pthread_create(&TA, NULL, TA_Activity, NULL) != 0) {
		perror("Failed to create TA thread");
		exit(EXIT_FAILURE);
	}

	// create student threads
	for (int i = 0; i < number_of_students; i++) {
		if (pthread_create(&Students[i], NULL, Student_Activity, (void*)(long)i) !=0) {
			perror("Failed to create Student thread");
            exit(EXIT_FAILURE);
		} 
	}

	// join TA thread
	if (pthread_join(TA, NULL) !=0) {
		perror("Failed to join TA thread");
		exit(EXIT_FAILURE);
	}

	// join student threads
	for (int i = 0; i < number_of_students; i++) {
		if (pthread_join(Students[i], NULL) != 0) {
			perror("Failed to join Student thread");
            exit(EXIT_FAILURE);
		}
	}
	

	//Free allocated memory
	free(Students); 
	return 0;
}

void *TA_Activity()
{
    /* I think this is done, The ta might not be "sleeping" properly though, so please double check this.
	//TA is currently sleeping.

    // lock chairs count mutex
    
    //if chairs are empty, break the loop.

	//TA gets next student on chair.

    //unlock

	//TA is currently helping the student
     
     //hint: use sem_wait(); sem_post(); pthread_mutex_lock(); pthread_mutex_unlock()

*/
	while (1)
	{
		printf("TA is sleeping.\n");
		sem_wait(&TASemaphore);
		printf("TA is awake.\n");
		while(ChairsCount>0)
		{
		pthread_mutex_lock(&ChairsCountMutex);
		if (ChairsCount == 0)
		{
			pthread_mutex_unlock(&ChairsCountMutex);
			break;
		}
		ChairsCount--;
		pthread_mutex_unlock(&ChairsCountMutex);
		sem_post(&NextStudentSemaphore);
		printf("TA is helping a student.\n");
		sleep(1); // Simulate time taken to help the student
		printf("TA finished helping a student.\n");
		}
	}
	
	

}

void *Student_Activity(void *threadID) 
{
    /*TODO
     
	//Student  needs help from the TA
	//Student tried to sit on a chair.
	//wake up the TA.
    // lock
     
	// unlock
    //Student leaves his/her chair.
	//Student  is getting help from the TA
	//Student waits to go next.
	//Student left TA room
    
    //If student didn't find any chair to sit on.
    //Student will return at another time
     
     //hint: use sem_wait(); sem_post(); pthread_mutex_lock(); pthread_mutex_unlock()
			
	*/

	long id = (long)threadID;
    while(1) {
        // random delay simulating arrival
        sleep(rand() % 5);

        // attempt to sit in a chair
        pthread_mutex_lock(&ChairsCountMutex);
        if (ChairsCount < 3) {
            ChairsCount++;
            printf("Student %ld sat on a chair. Chairs in use: %d\n", id, ChairsCount);
            pthread_mutex_unlock(&ChairsCountMutex);

            // wake up the TA
            sem_post(&TASemaphore);

            // wait for your turn to be helped
            sem_wait(&NextStudentSemaphore);

            // student is now being helped
            printf("Student %ld is getting help from the TA.\n", id);
            // simulate the student finishing help
            printf("Student %ld left the TA room.\n", id);
            break; // done
        } else {
            // no chairs left
            printf("Student %ld found no empty chair, will try again later.\n", id);
            pthread_mutex_unlock(&ChairsCountMutex);
        }
    }
    return NULL;
}