#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#define SLEEP_TIME 10			//sleep value in microseconds.. can change it to your need
#define MEGA 1.0     //do not change
int smallRunways[11][3]={{1,1,2},{2,3,4},{3,5,6},{4,1,4},{5,2,3},{6,3,5},{7,4,6},{8,4,5},{9,3,6},{10,1,3},{11,2,4}};//paths for small airplanes where the first value in each array is id of runway
int largeRunways[2][4]={{12,1,4,6},{13,2,3,5}};		//paths for large airplanes where the first value in each array is id of runway

pthread_mutex_t locks[13];
sem_t regionLock[6];			//semaphore variables for each region so that only one plane can use a region at a time

void *largePlane(void *vargp){
    int planeId = *((int *) vargp);				//get plane id from where thread was created in loop
    
    while(1){				//infinite loop where a plane comes back into idle at terminal after landing
        //idle
        int idleTime=rand()%SLEEP_TIME;
        float idleTime1=idleTime/MEGA;		//randomize idle time 
        printf("\n(State: Idle at the terminal) Large Plane Thread %d idle at terminal for %.3f seconds",planeId,idleTime1);
        sleep(idleTime);			//sleep for randomized idle time

        //takeoff
        int runwayId=(rand()%2)+12;				//randomize a runway id from 11-12(for large runways)
        int path[3];								//path array for the plane to choose a 3 region path
        for(int i=0;i<2;i++){	
            if(largeRunways[i][0]==runwayId){		//get path of randomized id generated 
                path[0]=largeRunways[i][1];
                path[1]=largeRunways[i][2];
                path[2]=largeRunways[i][3];
            }
        }
        
         int decision=(rand()%2);				//randomize forward or backward decision of path to take
        int forward=0;
        if(decision==0){
            printf("\n(State: Awaiting Takeoff) Large Plane Thread %d waiting for runway clear %d,%d,%d",planeId,path[0],path[1],path[2]);
            forward=1;			//if forward print in order otherwise print in reverse
        }
        else{
            printf("\n(State: Awaiting Takeoff) Large Plane Thread %d waiting for runway clear %d,%d,%d",planeId,path[2],path[1],path[0]);
        }
        
        //locks implementation
        sem_wait(&regionLock[path[0]-1]);
        sem_wait(&regionLock[path[1]-1]);
        sem_wait(&regionLock[path[2]-1]);			//acquire semaphore for regions being used in current plane's path so other planes cant use it
        
       //runway clear
       if(forward){			//if path is forward print and proceed forward
            int waitTime=rand()%SLEEP_TIME;
            float waitTime1=waitTime/MEGA;			//randomize sleep time for each region the plane arrives at
            printf("\n(State: Taking Off) Large Plane Thread %d at region %d idle for %.3f seconds",planeId,path[0],waitTime1);
            sleep(waitTime);
            waitTime=rand()%SLEEP_TIME;
            printf("\n(State: Taking Off) Large Plane Thread %d at region %d idle for %.3f seconds",planeId,path[1],waitTime1);
            sleep(waitTime);
            waitTime=rand()%SLEEP_TIME;
            printf("\n(State: Taking Off) Large Plane Thread %d at region %d idle for %.3f seconds",planeId,path[2],waitTime1);
            sleep(waitTime);
            sem_post(&regionLock[path[0]-1]);
            sem_post(&regionLock[path[1]-1]);
            sem_post(&regionLock[path[2]-1]);			//after the plane takes off give semaphore signals that the regions are clear to use
            //release lock
       }
       else{			//same thing as above but for reverse path
            int waitTime=rand()%SLEEP_TIME;
            float waitTime1=waitTime/MEGA;	
            printf("\n(State: Taking Off) Large Plane Thread %d at region %d idle for %.3f seconds",planeId,path[2],waitTime1);
            sleep(waitTime);
            waitTime=rand()%SLEEP_TIME;
            printf("\n(State: Taking Off) Large Plane Thread %d at region %d idle for %.3f seconds",planeId,path[1],waitTime1);
            sleep(waitTime);
            waitTime=rand()%SLEEP_TIME;
            printf("\n(State: Taking Off) Large Plane Thread %d at region %d idle for %.3f seconds",planeId,path[0],waitTime1);
            sleep(waitTime);
            sem_post(&regionLock[path[0]-1]);
            sem_post(&regionLock[path[1]-1]);
            sem_post(&regionLock[path[2]-1]);
            //release lock
       }

       //flying start
       int flyingTime=rand()%SLEEP_TIME;
       float flyingTime1=flyingTime/MEGA;			//randomize a flying time and sleep for the time
       printf("\n(State: Flying) Large Plane Thread %d flying for %.3f seconds",planeId,flyingTime1);
       sleep(flyingTime);

       //awaiting landing
       runwayId=(rand()%2)+12;					//choose from 2 runways again after randomizing to land
        for(int i=0;i<2;i++){
            if(largeRunways[i][0]==runwayId){			//get the path of randomized runway
                path[0]=largeRunways[i][1];
                path[1]=largeRunways[i][2];
                path[2]=largeRunways[i][3];
            }
        }

        decision=(rand()%2);			//randomize decision of forward or backward
        forward=0;
        if(decision==0){			//print path and await landing
            printf("\n(State: Awaiting Landing) Large Plane Thread %d waiting for landing runway clear %d,%d,%d",planeId,path[0],path[1],path[2]);
            forward=1;
        }
        else{
            printf("\n(State: Awaiting Landing) Large Plane Thread %d waiting for landing runway clear %d,%d,%d",planeId,path[2],path[1],path[0]);
        }

        sem_wait(&regionLock[path[0]-1]);
        sem_wait(&regionLock[path[1]-1]);
        sem_wait(&regionLock[path[2]-1]);			//wait for semaphore signals so that the regions are not in use by other planes

       
        //wait for locks

        if(forward){			//if forward path randomize sleep time for each region it arrives on and sleep for duration
            int waitTime=rand()%SLEEP_TIME;
            float waitTime1=waitTime/MEGA;	
            printf("\n(State: Landing) Large Plane Thread %d at region %d idle for %.3f seconds",planeId,path[0],waitTime1);
            sleep(waitTime);
            waitTime=rand()%SLEEP_TIME;
            printf("\n(State: Landing) Large Plane Thread %d at region %d idle for %.3f seconds",planeId,path[1],waitTime1);
            sleep(waitTime);
            waitTime=rand()%SLEEP_TIME;
            printf("\n(State: Landing) Large Plane Thread %d at region %d idle for %.3f seconds",planeId,path[2],waitTime1);
            sleep(waitTime);
            sem_post(&regionLock[path[0]-1]);			//release semaphore signals to notify the regions are clear for use
            sem_post(&regionLock[path[1]-1]);
            sem_post(&regionLock[path[2]-1]);
            //release lock
       }
       else{			//same thing as above but for backward path
            int waitTime=rand()%SLEEP_TIME;
            float waitTime1=waitTime/MEGA;	
            printf("\n(State: Landing) Large Plane Thread %d at region %d idle for %.3f seconds",planeId,path[2],waitTime1);
            sleep(waitTime);
            waitTime=rand()%SLEEP_TIME;
            printf("\n(State: Landing) Large Plane Thread %d at region %d idle for %.3f seconds",planeId,path[1],waitTime1);
            sleep(waitTime);
            waitTime=rand()%SLEEP_TIME;
            printf("\n(State: Landing) Large Plane Thread %d at region %d idle for %.3f seconds",planeId,path[0],waitTime1);
            sleep(waitTime);
            sem_post(&regionLock[path[0]-1]);
            sem_post(&regionLock[path[1]-1]);
            sem_post(&regionLock[path[2]-1]);
            //release lock
       }



}}


void *smallPlane(void *vargp){
    int planeId = *((int *) vargp);					//get plane id from main function where thread was created in loop
    
    while(1){			//infinite loop where the plan returns to idle at terminal after each cycle
        //idle
        int idleTime=rand()%SLEEP_TIME;
        float idleTime1=idleTime/MEGA;			//sleep for a random time in idle and announce
        printf("\n(State: Idle at the terminal) Small Plane Thread %d idle at terminal for %.3f seconds",planeId,idleTime1);
        sleep(idleTime);

        //takeoff
        int runwayId=(rand()%11)+1;				//choose from 11 possible runways for small airplanes
        int path[2];
        for(int i=0;i<11;i++){					//copy path of the chosen runway through randomize
            if(smallRunways[i][0]==runwayId){
                path[0]=smallRunways[i][1];
                path[1]=smallRunways[i][2];
                
            }
        }
        
        int decision=(rand()%2);			//randomize forward and backward decision
        int forward=0;
        if(decision==0){			//print path backward or forward based on decision
            printf("\n(State: Awaiting Takeoff) Small Plane Thread %d waiting for runway clear %d,%d",planeId,path[0],path[1]);
            forward=1;
        }
        else{
            printf("\n(State: Awaiting Takeoff) Small Plane Thread %d waiting for runway clear %d,%d",planeId,path[1],path[0]);
        }
        
        //locks implementation
        sem_wait(&regionLock[path[0]-1]);
        sem_wait(&regionLock[path[1]-1]);			//check if the both the regions are clear for use through semaphore signals
        
        
       //runway clear
       if(forward){			//if forward decision then randomize sleep time on each region and move forward
            int waitTime=rand()%SLEEP_TIME;
            float waitTime1=waitTime/MEGA;	
            printf("\n(State: Taking Off) Small Plane Thread %d at region %d idle for %.3f seconds",planeId,path[0],waitTime1);
            sleep(waitTime);
            waitTime=rand()%SLEEP_TIME;
            printf("\n(State: Taking Off) Small Plane Thread %d at region %d idle for %.3f seconds",planeId,path[1],waitTime1);
            sleep(waitTime);
            sem_post(&regionLock[path[0]-1]);			//release semaphore signals that regions are clear for use
            sem_post(&regionLock[path[1]-1]);
            
        }
        
            //release lock
       
       else{ //same thing as above but for backwards decision
            int waitTime=rand()%SLEEP_TIME;
            float waitTime1=waitTime/MEGA;	
            printf("\n(State: Taking Off) Small Plane Thread %d at region %d idle for %.3f seconds",planeId,path[1],waitTime1);
            sleep(waitTime);
            waitTime=rand()%SLEEP_TIME;
            printf("\n(State: Taking Off) Small Plane Thread %d at region %d idle for %.3f seconds",planeId,path[0],waitTime1);
            sleep(waitTime);
           
            sem_post(&regionLock[path[0]-1]);
            sem_post(&regionLock[path[1]-1]);
            //release lock
       }

       //flying start
       int flyingTime=rand()%SLEEP_TIME;
       float flyingTime1=flyingTime/MEGA;		//randomize a flying time and fly for duration 
       printf("\n(State: Flying) Small Plane Thread %d flying for %.3f seconds",planeId,flyingTime1);
       sleep(flyingTime);

       //awaiting landing
       runwayId=(rand()%11)+1;				//choose from 11 possible runways for landing 
        for(int i=0;i<11;i++){
            if(smallRunways[i][0]==runwayId){		//copy path of chosen runway
                path[0]=smallRunways[i][1];
                path[1]=smallRunways[i][2];
                
            }
        }

        decision=(rand()%2);			//randomize decision of forward or backward path
        forward=0;
        if(decision==0){			//print path backwards or forwards
            printf("\n(State: Awaiting Landing) Small Plane Thread %d waiting for landing runway clear %d,%d",planeId,path[0],path[1]);
            forward=1;
        }
        else{
            printf("\n(State: Awaiting Landing) Small Plane Thread %d waiting for landing runway clear %d,%d",planeId,path[1],path[0]);
        }

       sem_wait(&regionLock[path[0]-1]);
       sem_wait(&regionLock[path[1]-1]);			//await landing through semaphore signals if runways are clear
        
        
        
        //wait for locks

        if(forward){		//if forward then sleep for each region and move forward 
            int waitTime=rand()%SLEEP_TIME;
            float waitTime1=waitTime/MEGA;	
            printf("\n(State: Landing) Small Plane Thread %d at region %d idle for %.3f seconds",planeId,path[0],waitTime1);
            sleep(waitTime);
            waitTime=rand()%SLEEP_TIME;
            printf("\n(State: Landing) Small Plane Thread %d at region %d idle for %.3f seconds",planeId,path[1],waitTime1);
            sleep(waitTime);
            
            sem_post(&regionLock[path[0]-1]);			//release semaphore signal after reaching terminal to announce regions are clear for use
            sem_post(&regionLock[path[1]-1]);
            
            //release lock
       }
       else{		//same thing but for backward
            int waitTime=rand()%SLEEP_TIME;
            float waitTime1=waitTime/MEGA;	
            printf("\n(State: Landing) Small Plane Thread %d at region %d idle for %.3f seconds",planeId,path[1],waitTime1);
            sleep(waitTime);
            waitTime=rand()%SLEEP_TIME;
            printf("\n(State: Landing) Small Plane Thread %d at region %d idle for %.3f seconds",planeId,path[0],waitTime1);
            sleep(waitTime);
            
           sem_post(&regionLock[path[0]-1]);
           sem_post(&regionLock[path[1]-1]);
            //release lock
       }



}
}



int main(){
	sem_init(&regionLock[0],0,1);		//initializing the semaphore
    sem_init(&regionLock[1],0,1);
    sem_init(&regionLock[2],0,1);
    sem_init(&regionLock[3],0,1);
    sem_init(&regionLock[4],0,1);
    sem_init(&regionLock[5],0,1);
    pthread_t tid;					//initialize pthread variable
	FILE* file=fopen("seed.txt","r");
	int seed_file;
	fscanf(file,"%d",&seed_file);
    srand(seed_file);			    //read seed value from seed.txt file
	sem_post(&regionLock[0]);
	sem_post(&regionLock[1]);
	sem_post(&regionLock[2]);
	sem_post(&regionLock[3]);
	sem_post(&regionLock[4]);
	sem_post(&regionLock[5]);		//release all semaphores so they are available for use
    for(int i=1;i<=15;i++){
        int *arg = malloc(sizeof(*arg));
         *arg = i;
        pthread_create(&tid,NULL,largePlane,arg);
    }								//create 15 large plane threads

    for(int i=16;i<=45;i++){
          int *arg = malloc(sizeof(*arg));
         *arg = i;
        pthread_create(&tid,NULL,smallPlane,arg);
    }								//create 30 small plane threads
	
    pthread_exit(NULL);
    return 0;
}