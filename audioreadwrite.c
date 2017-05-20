#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <string.h>
#include "udpclient.h"

#define AXI_REG_0   ((unsigned *)(ptr0 + 0))

//functions
void *AudioRecieveAndWrite(void *);
  
  
int main(int argc, char *argv[]) {
    if (*argv[1] == 'p') {
        printf("::::START_USAGE::::\n");
        printf("EXAMPLE : %s 2 4 \n", argv[0]);
        printf("::::END_USAGE::::\n");
    } else {
		printf("::::START_USAGE::::\n");
        // open dev/uio0
        int fd = open ("/dev/uio0", O_RDWR);
        if (fd < 1) { perror(argv[0]); return -1; }
   
        //Redirect stdout/printf into /dev/kmsg file (so it will be printed using printk)
        freopen ("/dev/kmsg","w",stdout);
  
        //get architecture specific page size
        unsigned pageSize = sysconf(_SC_PAGESIZE);
  
        //Map virtual memory to physical memory
        void *ptr0;
        ptr0 = mmap(NULL, pageSize, (PROT_READ |PROT_WRITE), MAP_SHARED, fd, 0);
          
		//seting up udp client
        if (udp_client_setup ("10.255.255.255", 7891) == 1){
			perror("Error while setting up UDP client\n");
			exit(-1);
		}
		
		//Creating FIFO
		const char * temp_fifo = "/tmp/audio.fifo";
		if(mkfifo(temp_fifo, S_IWUSR | S_IRUSR) != 0) {
			perror("Error while creation temp FIFO file\n");
			exit (-1);
		}
        //thread to get audio from linux and write it into the FIFO
		pthread_t audioRecieveThread;
		int ar = pthread_create(&audioRecieveThread, NULL, AudioRecieveAndWrite,(void *)temp_fifo);
		if (ar){
		 perror("Error while creating a thread to get audio from the network from Linux\n");
		 exit(-1);
		}
		 
		//Reading from the FIFO file and continously writing on the AXI register
		int writeAxi = open(temp_fifo, O_RDONLY);
		while(true){
			if((read(writeAxi, AXI_REG_0, 4)) == -1){//Write 4  bytes to REG_AXI_0
				perror("Error while Reading from FIFO file\n");
				close(writeAxi);
			}
		}
				 
		pthread_join(audioRecieveThread, NULL);
		//unmapping
        munmap(ptr0, pageSize);
        
        fclose(stdout);
        // Unlinking temp FIFO
        unlink(temp_fifo);
        //Exiting the pthread
        pthread_exit(NULL);
        }
	return 0;
}

void *AudioRecieveAndWrite (void *tmpfifo) {
	const char *audioFifo;
	audioFifo = (char *) tmpfifo;
	int fifoFile;
	unsigned buffer[256];
	fifoFile = open (audioFifo, O_WRONLY);
	if (fifoFile < 0){
			perror("Error while opening FIFO file\n");
			return;
	}
	while (true){
		if (udp_client_recv (buffer, 256) == 1){
			perror("Error while receiving a audio from the network from Linux\n");
			close(fifoFile);
			return;
		}
		if ((write (fifoFile, buffer, 256) )== -1){ 
			perror("Error while writing to the FIFO file\n");
			close(fifoFile);
			return;
		}
	}
	close(fifoFile);
	pthread_exit(NULL);
	return;
}
