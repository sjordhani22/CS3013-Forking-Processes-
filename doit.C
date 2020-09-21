/* doit.C */

#include <iostream>
using namespace std;
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <string.h>

void printStats(struct timeval s, struct timeval e){
	int x = RUSAGE_CHILDREN; 
	struct rusage usage; 
	struct timeval utv = usage.ru_utime; //User CPU Time 
	struct timeval stv = usage.ru_stime; //System CPU Time
	uint64_t uMilliSec = (usage.ru_utime.tv_sec * 1000.0) + (usage.ru_utime.tv_usec / 1000.0);
	uint64_t sMilliSec = (usage.ru_stime.tv_sec * 1000.0) + (usage.ru_stime.tv_usec / 1000.0);
	uint64_t elapsed = ((e.tv_sec - s.tv_sec) * 1000.0) + 		((e.tv_usec - s.tv_usec) / 1000.0);
	int val; 
	val = getrusage(x, &usage);

	cout << "The amount of user CPU time used is: " << (usage.ru_utime.tv_sec * 1000.0) + (usage.ru_utime.tv_usec / 1000.0) <<  		" ms" << "\n";
	cout << "The amount of system CPU time used is: " << (usage.ru_stime.tv_sec * 1000.0) + (usage.ru_stime.tv_usec / 1000.0) <<  		" ms" << "\n";
	cout << "The elapsed time for the execution of the command is: " << 		elapsed << " ms" << "\n";
	cout << "The number of major page faults was: " << usage.ru_majflt 		<< "\n";
	cout << "The number of minor page faults was: " << usage.ru_minflt 		<< "\n";
	cout << "The number of voluntary context switches was: " << 		usage.ru_nvcsw << "\n";
	cout << "The number of involuntary context switches was: " << 	usage.ru_nivcsw << "\n";
}


extern char **environ;		

main(int argc, char **argv){
     /* argc -- number of arguments */
     /* argv -- an array of arguments as strings */

    char *argvNew[32];  
    int pid;
    bool background = false;
    struct timeval start, end;
    /*We must store prompt as a global variable so that we are 
       able to set the prompt to whatever the user chooses inside 
       the while loop */ 
    string prompt = "==>"; 
    
    if(argc == 1){
    	while(1){ 
    	cout << prompt; //Will use "==>" as prompt unless changed by user
    	char* s = (char*)malloc(128*sizeof(char)); 
    	//We are assuming line of input is no more than 128 characters 
    	// so we will malloc enough space for 128 characters 
    	int z = 1;
    	char* shellArgvNew[32];
    	cin.getline(s, 128); 
    	char* buffer;
    	buffer = strtok(s, " "); 
    	shellArgvNew[0] = buffer; //set the first argument to index 0
    	while((buffer = strtok(NULL, " "))){
    	//This while loop will go through the arguments until 
    	// there is no more arguments left to store
    	
    	shellArgvNew[z] = buffer; 
    	z++;
    	}
    	
    	shellArgvNew[z] = NULL; // adding a string terminator
    	
    	if(strncmp(shellArgvNew[0], "exit", 4) == 0){
    	// exits the shell
    	exit(0);
    	}
    
    	else if(strncmp(shellArgvNew[0],"cd", 2) ==0){
    	// change directory to second argument in array
    	chdir(shellArgvNew[1]);
       cout << "The directory has been changed to " << shellArgvNew[1] << endl;
    	}
    	
    	else if((strncmp(shellArgvNew[0], "set", 3) == 0) && 
	    (strncmp(shellArgvNew[1], "prompt", 6) == 0)){
	  // set prompt to whatever the user inputs after the "="
	   prompt = shellArgvNew[3];
	   cout << "The prompt has been changed to " << shellArgvNew[3] << endl;
	   }
  
  else{  
    	struct timeval startShell, endShell;
    	//start the time right before forking 
    	
    	gettimeofday(&startShell, NULL);
        if ((pid = fork()) < 0) {
	cerr << "Fork error\n";
        exit(1);
    }
    
    
    else if (pid == 0) {
    /*Child Process Begins Here*/ 
                                                                        
    
    	
    	if(strncmp(shellArgvNew[z-1], "&", 1) == 0){
		cout << "This is a background process." << endl;
		cout << "Background process functionality is not complete." << endl;
	  //This is the progress I made on part 3 
		char* temp[32];
		background = true; 
		for(int i = 0; i < z-1; i++){
		     temp[i] = shellArgvNew[i];
		}
		temp[z - 1] = NULL;
		
		if ((pid = fork()) < 0) {
	cerr << "Fork error\n";
        exit(1);
    }
            else if (pid == 0) {
		if(execvp(temp[0], temp) < 0){
		   cerr << "Execvp error\n";
		   exit(1);
		}
	}
	else{
	struct timeval bgStart;
	gettimeofday(&bgStart, NULL); 
	wait(0);
	gettimeofday(&endShell, NULL); //end time here
	printStats(bgStart, endShell);
	}
    }
	
        
  /*if execvp succeeds it will not return but instead it will execute the command and print out the statistics in the  parent below */
  
        else if(execvp(shellArgvNew[0], shellArgvNew) < 0){
	    cerr << "Execvp error\n";
            exit(1);
          }
   }

    	 else{ 
    	 /*parent process starts here*/
	 if(background){ 
	
	 }
	 else{
	 wait(0);
	gettimeofday(&endShell, NULL); //end time here
	printStats(startShell, endShell);
	 }  
	}
       }
     }	
   }

 	/* Regular execution not in shell starts here*/ 
    gettimeofday(&start, NULL);
    if ((pid = fork()) < 0) {
	cerr << "Fork error\n";
        exit(1);
    }
    else if (pid == 0) {
        /* child process */
        
// We need to loop through the whole array and grab each argument from argv
        for(int a = 0; a < argc; a++){
        argvNew[a] = argv[a + 1];
        }
        if (execvp(argvNew[0], argvNew) < 0) {
	    cerr << "Execve error\n";
            exit(1);
        }
      }
        
    else {
        /* parent process starts here */
	wait(0);	//wait for the child to finish 
	gettimeofday(&end, NULL);
	printStats(start, end);
	
    }
}
