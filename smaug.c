#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <time.h>


const char *semName3 = "isThief_signal";
const char *semName4 = "isHunter_signal";
const char *semName1 = "thief_win_signal";
const char *semName2 = "hunter_win_signal";
const char *semName6 = "sure_theif_signal";
const char *semName7 = "get_result_theif_signal";
const char *semName8 = "final_theif_signal";
const char *semName10= "sure_hunter_signal";
const char *semName11= "get_result_hunter_signal";

const char *semName13= "final_hunter_singal";
sem_t *isThief;
sem_t *isHunter; 
sem_t *thief_win;
sem_t *hunter_win;
sem_t *get_result_theif;

sem_t *final_theif;
sem_t *sure_hunter;
sem_t *get_result_hunter;

sem_t *final_hunter;

const char *semName5 = "smaug_trminated_signal";
sem_t *smaug_terminated;


sem_t *smaugWakeup;
sem_t *smaugBreath;
sem_t *something_arrive;
sem_t *sure_theif;



int winprob;

int timeChange( const struct timeval startTime )
{
  	struct timeval nowTime;
	long int elapsed;
	int elapsedTime;
	gettimeofday(&nowTime, NULL);
	elapsed = (nowTime.tv_sec - startTime.tv_sec) * 1000000
	+ (nowTime.tv_usec - startTime.tv_usec);
	elapsedTime = elapsed / 1000;
	return elapsedTime;
}



void *hunter(pid_t *hunterid)
{

    srand(time(NULL));
    pid_t hunter;
    hunter = fork();
    *hunterid = hunter;
    if(hunter == 0)
    {
       
        hunter = getpid();
	
        printf("hunter %d wandering in the valley\n", hunter);
        printf("hunter %d is travelling to the valley\n", hunter);     
        
        sem_post(smaugWakeup);

        sem_wait(smaugBreath);
         //sem_wait(something_arrive);//
             
        
        
        sem_post(something_arrive);
        sem_post(isHunter);
	sem_wait(sure_hunter);
        printf("Hunter %d is playing with Smaug\n", hunter);
        int win = rand()%100;
	//printf("probability: %d\n",win);
        if(win<=winprob)
        {
            printf("hunter %d has won and receives treasure\n", hunter);
            sem_post(hunter_win);
	    sem_post(get_result_hunter);
        }
        else
        {
            printf("hunter %d has been defeated and pays the price\n", hunter);
	    sem_post(get_result_hunter);
	    
        }
	sem_post(final_hunter);
        printf("hunter %d leaves\n", hunter);
	
        
        exit(0);
    }
}


void *thief(pid_t *thiefid)
{
   
    srand(time(NULL));
    pid_t thief;
    thief = fork();
    *thiefid = thief;
    if(thief == 0)
    {
        
        thief = getpid();
	
        printf("Thief %d wandering in the valley\n", thief);
        printf("Thief %d is travelling to the valley\n", thief);     
        
        sem_post(smaugWakeup);

        sem_wait(smaugBreath);
         //sem_wait(something_arrive);//
             
	       int isthief_value;
        sem_getvalue(isThief, &isthief_value);
        
        
        sem_post(something_arrive);
        sem_post(isThief);
	sem_wait(sure_theif);
        printf("Thief %d is playing with Smaug\n", thief);
        int win = rand()%100;
	//printf("probability: %d\n",win);
        if(win<=winprob)
        {
            printf("Thief %d has won and receives treasure\n", thief);
            sem_post(thief_win);
	    sem_post(get_result_theif);
	 
        }
        else
        {
            printf("Thief %d has been defeated and pays the price\n", thief);
	    sem_post(get_result_theif);
	   
        }
	sem_wait(final_theif);
        printf("Thief %d leaves\n", thief);

        exit(0);
    }
}


void *smaug(pid_t *smaugid)
{
    pid_t smaug;
    smaug = fork();
    *smaugid = smaug;
    if(smaug == 0)
    {	
	//*smaugid = getpid();
        int smaug_treasure = 30;

        int win_thief_count=0;
        int win_hunter_count=0;
      
        //printf("Smaug is going to sleep\n");
        
        int isThief_val;
        int isHunter_val;
        int thief_win_val;
        int hunter_win_val;
	int isthief_value;
	int is_hunter_value;
       

        //while true
        

        //semaphore
        while(win_thief_count<3 && win_hunter_count<4 &&smaug_treasure>0 && smaug_treasure<80)
        {
        	   sem_wait(smaugWakeup);
        	   printf("Smaug has been woken up\n");
            	   printf("Smaug takes a deep breath\n");
            sem_post(smaugBreath);
            
 
       	    sem_wait(something_arrive);
	     sem_getvalue(isThief, &isthief_value);
      	  //  printf("isthief2 value is: %i\n", isthief_value); 
	       sem_getvalue(isHunter, &is_hunter_value);
	//printf("isHunter value: %d\n",is_hunter_value);
            sem_getvalue(isThief, &isThief_val);
            sem_getvalue(isHunter, &isHunter_val);
            
        
           
            if(isThief_val == 1)
            {
	      sem_wait(isThief);
              printf("Smaug smells a thief\n");
	      sem_post(sure_theif);
              printf("Smaug is playing with a thief\n");
              
              
              //post smaug playing semaphore
 
              //counter
	      sem_wait(get_result_theif);
              sem_getvalue(thief_win, &thief_win_val);
	      //printf("thief_win_value: %d\n",thief_win_val);
              if(thief_win_val == 2)//thief win
              {
       		  sem_wait(thief_win);
                  smaug_treasure=smaug_treasure-8;
                  printf("Smaug has lost some treasure he now has %i jewels\n",smaug_treasure);
		      sem_post(final_theif);
             	 printf("Smaug has finished a game\n\n");
                  
              }
              else if(thief_win_val == 1)//thief lost
              {
                  smaug_treasure=smaug_treasure+20;
                  win_thief_count++;
		printf("Smaug has added to his treasure he now has %i jewels\n",smaug_treasure);
		
		 sem_post(final_theif);
             	 printf("Smaug has finished a game\n\n");
              }
	
          
                  
             }
         
        
        
        
              //semaphore
           	
              if(isHunter_val==1)
              {	
		sem_wait(isHunter);//clean ishunter signal
                printf("Smaug smells a treasure hunter\n");
		sem_post(sure_hunter);
                printf("Smaug is fighting treasure a hunter\n");
                
                
                //post smaug fighting semaphore
    

                //counter
		sem_wait(get_result_hunter);
                sem_getvalue(hunter_win, &hunter_win_val);
		//printf("hunter_win_value: %d\n",hunter_win_val);
                if(hunter_win_val ==2)//hunter win
                {
                    sem_wait(hunter_win);
                    smaug_treasure=smaug_treasure-10;
                    printf("Smaug has lost some treasure he now has %i jewels\n",smaug_treasure);
                    //clean hunter win signal
		     sem_post(final_hunter);
                 	 printf("Smaug has finished a battle\n");
                }
                else if(hunter_win_val == 1)//hunter lost
                {
                    smaug_treasure=smaug_treasure+5;
                    win_hunter_count++;
                    printf("Smaug has added to his treasure he now has %i jewels\n",smaug_treasure);
		    
		     sem_post(final_hunter);
               		 printf("Smaug has finished a battle\n\n");
                }
              
               }
            
        }
        
        if(smaug_treasure <= 0)
       {
         printf("Smaug has no more treasure\n");
      
       }
       sem_post(smaug_terminated);//smaug trminate
        exit(0);
    }
}


int main(void)
{


    pid_t ProcessIds[500];
    //pid_t thiefProcessIds[100];

    struct timeval startTimeT;
    struct timeval startTimeH;

    int max_hunter_Interval;
    int max_thief_Interval;
    
    printf("enter the max_hunter_Interval:\n");
    scanf("%i", &max_hunter_Interval);
    printf("enter the max_thief_Interval:\n");
    scanf("%i", &max_thief_Interval);
    printf("enter the winprob: \n");
    scanf("%i", &winprob);
    
   
    int hunter_Interval = rand()%max_hunter_Interval+1;
    int thief_Interval = rand()%max_thief_Interval+1;
    
	smaugWakeup = sem_open("/smaugWake_signal", O_CREAT|O_RDWR, 0600,0 );
	smaugBreath = sem_open("/smaugBreath_signal", O_CREAT, 0600,0);
	something_arrive = sem_open("/spmething_arrive_signal",O_CREAT, 0600,0);

    isThief = sem_open("/isThief_signal", O_CREAT, 0600,0);
    isHunter = sem_open("/isHunter_signal", O_CREAT|O_RDWR, 0600,0);

   get_result_theif = sem_open(semName7, O_CREAT|O_RDWR, 0600,0);
	
    thief_win = sem_open("/thief_win_signal", O_CREAT,0600, 1); 
    hunter_win = sem_open("/hunter_win_signal", O_CREAT, 0600, 1);
   
    sure_hunter=sem_open(semName10,O_CREAT|O_RDWR, 0600,0);
    sure_theif= sem_open(semName6, O_CREAT|O_RDWR, 0600,0);
    


     final_theif = sem_open(semName8, O_CREAT,0600, 1); 
     final_hunter=sem_open(semName13,  O_CREAT,0600, 1);
	
     get_result_hunter=sem_open(semName11,O_CREAT,0600, 0);

    smaug_terminated = sem_open("/smaug_terminated_signal", O_CREAT,0600, 0);
    int smaug_terminated_val=0;
    
    
     int counter=0;
     pid_t smaugid;
    smaug(&smaugid);
    ProcessIds[counter]=smaugid;
     counter++;
    gettimeofday(&startTimeH, NULL);
    gettimeofday(&startTimeT, NULL);
 
   //for(int i=0;i<10;)
  while(smaug_terminated_val != 1)// smaug terminate signal
    {
    	
    	if (timeChange(startTimeT)>= hunter_Interval*1000){
	gettimeofday(&startTimeT, NULL);
	 pid_t hunterid;
    	 hunter(&hunterid);
         ProcessIds[counter]=hunterid;
         counter++;
        }
        if(timeChange(startTimeH)>= thief_Interval*1000){

	gettimeofday(&startTimeH, NULL);
	
     	pid_t thiefid;
	thief(&thiefid);
         ProcessIds[counter]=thiefid;
         counter++;
   	}
	sem_getvalue(smaug_terminated, &smaug_terminated_val);
        
    }
       
     int status;
     waitpid(smaugid, &status, 0);
      // printf("the#of count %i \n",counter);
        for (int i = 1; i < counter; i++) {
            //printf("about to kill %i\n", currentChild);
            waitpid(ProcessIds[i], &status, 0);
            kill(ProcessIds[i], SIGKILL);
           // waitpid(ProcessIds[i], &status, 0);
          //   printf("the # of id : %i \n",ProcessIds[i]);
        }

       kill(ProcessIds[0], SIGKILL);

    sem_close(isThief);
    sem_unlink("/isThief_signal");
    
    sem_close(isHunter);
    sem_unlink("/isHunter_signal");
    
    sem_close(thief_win);
    sem_unlink("/thief_win_signal");
    
    sem_close(hunter_win);
    sem_unlink("/hunter_win_signal");
    
    sem_close(smaugWakeup);
    sem_unlink("/smaugWakeup_signal");
    
    sem_close(smaugBreath);
    sem_unlink("/smaugBreath_signal");
    
    sem_close(smaug_terminated);
    sem_unlink("/smaug_terminated_signal");
    
    sem_close(something_arrive);
    sem_unlink("/something_arrive_signal");
    
     sem_close(sure_theif);
    sem_unlink(semName6);

   sem_close(get_result_theif);
    sem_unlink(semName7);

     sem_close(final_theif);
    sem_unlink(semName8);
    

     sem_close(sure_hunter);
     sem_unlink(semName10);

      sem_close( get_result_hunter);
     sem_unlink(semName11);

    

	  sem_close(final_hunter);
     sem_unlink(semName13);

    
    
    
    return 0;
    
}


























