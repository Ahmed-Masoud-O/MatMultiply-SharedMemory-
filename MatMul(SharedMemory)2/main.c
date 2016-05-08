//
//  main.c
//  MatMul(SharedMemory)2
//
//  Created by Ahmed masoud on 5/5/16.
//  Copyright © 2016 Ahmed masoud. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>

int* creatSharedMemory(int size){
    int shm_fd;
    int* shared_memory;
    int memory_size = size; // the size (in bytes) of the shared memory segment
    const char *name = "Product";
    
    // open the memory
    shm_unlink(name);
    shm_fd = shm_open (name, O_CREAT | O_EXCL | O_RDWR, S_IRWXU | S_IRWXG); // ask about last 2 arguments
    if (shm_fd < 0)
    {
        fprintf(stderr,"Error in shm_open()");
        return NULL;
    }
    
    printf("Created shared memory object %s\n", name);
    
    // attach the shared memory segment
    ftruncate(shm_fd, memory_size);
    printf("shmat returned\n");
    
    // allocating the shared memory
    shared_memory = (int *) mmap(NULL, memory_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_memory == NULL)
    {
        fprintf(stderr,"Error in mmap()");
        return NULL;
    }
    
    printf("Shared memory segment allocated correctly (%d bytes).\n", memory_size);
    return shared_memory;

}

int main(int argc, const char * argv[]) {
    pid_t childPID;
    int iCount,jCount,i;
    int MAT1[10][10];
    int MAT2[10][10];
    int MAT3[10][10];
    int r1,c1,r2,c2;
    printf("Enter Number of Rows For Matrix 1 :");
    scanf("%d",&r1);
    pthread_t tid[r1];
    
    printf("Enter Number of Columns For Matrix 1 :");
    scanf("%d",&c1);
    
    for(iCount=0;iCount<r1;iCount++)
    {
        for(jCount=0;jCount<c1;jCount++)
        {
            printf("Enter Mat1[%d][%d] :",iCount,jCount);
            scanf("%d",&MAT1[iCount][jCount]);
        }
    }
    
    printf("\n");
    
    printf("Enter Numer of Rows For Matrix 2 :");
    scanf("%d",&r2);
    
    printf("Enter Number of Columns For Matrix 2 :");
    scanf("%d",&c2);
    
    for(iCount=0;iCount<r2;iCount++)
    {
        for(jCount=0;jCount<c2;jCount++)
        {
            printf("Enter Mat2[%d][%d] :",iCount,jCount);
            scanf("%d",&MAT2[iCount][jCount]);
        }
    }
    
    if(c1!=r2)
    {
        printf("Multipication of Matrix not Possible !!!");
        return 0;
    }
    int* shared_memory = creatSharedMemory((r1*c2 + 2)*sizeof(int));
    shared_memory[r1*c2 + 1] = 0;
    shared_memory[r1*c2 + 2] = 0;
    for (i=0; i<r1; i++) {
        childPID = fork();
        if(childPID < 0){
            fprintf(stderr, "Fork Failed");
            return 1;
        }//error
        //child process
        else if(childPID==0){
            int jCount,kCount;
            int row = shared_memory[r1*c2 + 1];
            int temp = shared_memory[r1*c2 + 2];
            printf("row %d before loop \n",row);
            for(jCount=0;jCount<c2;jCount++)
            {
                for(kCount=0;kCount<c1;kCount++)
                {
                    MAT3[row][jCount]+=MAT1[row][kCount] * MAT2[kCount][jCount];
                }
                shared_memory[temp] = MAT3[row][jCount];
                
                //printf("temp : %d \n %d\t",temp,shared_memory[temp]);
                temp++;shared_memory[r1*c2 + 2]++;
                
                
            }
            printf("after loop\n");
            //row++;
            shared_memory[r1*c2 + 1]++;
        }
        //parent process
        else
        {
            
            
            wait(NULL);
            int bibo[100];
            int k;
            for (k=0; k<100; k++) {
                bibo[k] = shared_memory[k];
            }
            for (i=0; i<r1*c2; i++) {
                printf("%d\t",shared_memory[i]);
            }
            printf("\n");
        }

    }


        return 0;
}
