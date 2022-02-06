// A Multithreaded Program that implements the banker's algorithm.
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>



int nrcs, nprs, *rcs, **alloctd, **maxreq,**need, *safeSequence, procDone = 0;
int processes_executed,pres_proc;


FILE* out;
void GenerateSafeSeq();

void* runThreads(void* );
//function to read the input file
void read_input(char  *fname){
	
	FILE *fptr=fopen(fname,"r");
	if(!fptr)
	{
		printf("Error reading file %s. Input file name again\n",fname);
		exit(0);
	}
	fscanf(fptr,"%d %d",&nprs,&nrcs);
	rcs = (int *)malloc(nrcs * sizeof(*rcs));
    for(int i=0; i<nrcs; i++)
        fscanf(fptr,"%d", &rcs[i]);
    alloctd = (int **)  malloc(nprs * sizeof(*alloctd));
    for(int i=0; i<nprs; i++)
            alloctd[i] = (int *)malloc(nrcs * sizeof(**alloctd));
    maxreq = (int **)malloc(nprs * sizeof(*maxreq));
    for(int i=0; i<nprs; i++)
    	maxreq[i] = (int *)malloc(nrcs * sizeof(**maxreq));
    for(int i=0; i<nprs; i++) {
            for(int j=0; j<nrcs; j++)
                    fscanf(fptr,"%d", &alloctd[i][j]);
    }
    for(int i=0; i<nprs; i++) {
        for(int j=0; j<nrcs; j++)
                fscanf(fptr,"%d", &maxreq[i][j]);
	}
}
void calc_req(){
	 need = (int **)malloc(nprs * sizeof(*need));
    for(int i=0; i<nprs; i++)
            need[i] = (int *)malloc(nrcs * sizeof(**need));

    for(int i=0; i<nprs; i++)
            for(int j=0; j<nrcs; j++)
                    need[i][j] = maxreq[i][j] - alloctd[i][j];


	safeSequence = (int *)malloc(nprs * sizeof(*safeSequence));
}
int main(int argc, char** argv) {
	srand(time(NULL));
		printf("Enter the input file name\n");//take input file name
		char fname[100];
		scanf("%s",&fname);
		read_input(fname);
       	calc_req();//calculate needed matrix
		GenerateSafeSeq();//generate the safe sequence
		out=fopen("output.txt","w");
        fprintf(out,"\n\nPrinting safe sequence : ");
        for(int i=0; i<nprs; i++) {
                fprintf(out,"%-3d", safeSequence[i]+1);
        }
        fprintf(out,"\nExecuting Processes...\n\n");
		if(nprs==0)exit(0);
		pres_proc=safeSequence[0];
		pthread_t processes[nprs];
		
        for(int i=0; i<nprs; i++){
			int *arg = (int*) malloc(sizeof(&arg));
			if ( arg == NULL ) {
		        fprintf(out, "Couldn't allocate memory for thread arg.\n");
		        exit(EXIT_FAILURE);
        	}
        	*arg=i;
			pthread_create(&processes[i], NULL, runThreads, arg);
		}
                

        for(int i=0; i<nprs; i++)
                pthread_join(processes[i], NULL);

       fprintf(out,"\nAll Processes Finished\n");	
       printf("Output is saved in file named output.txt\n");
}


void  GenerateSafeSeq() {
		if(nprs==0)return ;
        int presAvailRes[nrcs],possible[nprs]={0};
        for(int i=0; i<nrcs; i++) presAvailRes[i] = rcs[i];

        bool finished[nprs]={0};
        int nfinished=0;
        int procrem=nprs;
        
        for(int run=0;run<nprs;run++) {
        	//run this for max nprs runs to check if any process can be performed next
                bool safe = false;
				
                for(int proc_num=0; proc_num<nprs; proc_num++) {
                	//check for all processes , if they can be scheduled and they are not finished yet
                        if(!finished[proc_num]) {
                        	
//                        		printf("checking for prc %d\n",proc_num);
                                possible[proc_num] = 1;

                                for(int rsc_num=0; rsc_num<nrcs; rsc_num++)
                                		//check if pres avail resources are enough to run the process
                                        if(need[proc_num][rsc_num] > presAvailRes[rsc_num]) {
                                                possible[proc_num] = 0;
                                        }
//                            	printf("possible  is %d\n",possible[proc_num]);      
                        }
                }
                for(int proc_num=0;proc_num<nprs;proc_num++){
                	if(!finished[proc_num] and possible[proc_num]){
                		//if process if not finished and is possible to run then run it now
                        for(int rsc_num=0; rsc_num<nrcs; rsc_num++)
                        		//free the resources used by present process
                                presAvailRes[rsc_num] += alloctd[proc_num][rsc_num];
                        safeSequence[nfinished] = proc_num;
                        finished[proc_num] = true;
                        safe = true;
                        possible[proc_num]=0;
                        break;
					}
				}
				if(safe){
					
					++nfinished;
					procrem--;
				
				}
                else {
                	//if no process can be run next, then exit application and print unsafe state
                		fprintf(out,"Unsafe state ,exiting application\n");
                   	  	exit(-1);
                }
        }
        return;
}

// function to run all process in multithreading
void* runThreads(void *arg) {
        int p = *((int * ) arg);
        //keep waiting until its turn of the process
       	while(p != pres_proc);
        fprintf(out,"\n-->Runinig Process %d", p+1);
        fprintf(out,"\n\tAllocated : ");
        for(int i=0; i<nrcs; i++)
                fprintf(out,"%d ", alloctd[p][i]);

        fprintf(out,"\n\tNeeded    : ");
        for(int i=0; i<nrcs; i++)
                fprintf(out,"%d ", need[p][i]);

        fprintf(out,"\n\tAvailable : ");
        for(int i=0; i<nrcs; i++)
                fprintf(out,"%d ", rcs[i]);
        fprintf(out,"\n");
        fprintf(out,"\tResource Allocated!\n");
        fprintf(out,"\tProcess Code Running...\n");
        fprintf(out,"\tProcess Code Completed...\n");
       	fprintf(out,"\tProcess Releasing Resource...\n");
        fprintf(out,"\tResource Released!\n");

		for(int i=0; i<nrcs; i++)
                rcs[i] += alloctd[p][i];

        fprintf(out,"\tNow Available : ");
        for(int i=0; i<nrcs; i++)
                fprintf(out,"%d ", rcs[i]);

        procDone++;
        if(procDone<nprs)
    	//make pres proc to next process after present process finishes
        	pres_proc=safeSequence[procDone];
	pthread_exit(NULL);
}
