/*
* Simple app to read/write into custom IP in PL via /dev/uoi0 interface
* To compile for arm: make ARCH=arm CROSS_COMPILE=arm-xilinx-linux-gnueabi-
* Author: Tsotnep, Kjans
*/
  
//C
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdint.h>
#include "ZedboardOLED.h"
 
//Filter for Network

#define REG_FILTER_NETWORK_0   *((unsigned *)(ptr4 + 0))
#define REG_FILTER_NETWORK_1   *((unsigned *)(ptr4 + 4))
#define REG_FILTER_NETWORK_2   *((unsigned *)(ptr4 + 8))
#define REG_FILTER_NETWORK_3   *((unsigned *)(ptr4 + 12)) 
#define REG_FILTER_NETWORK_4   *((unsigned *)(ptr4 + 16))
#define REG_FILTER_NETWORK_5   *((unsigned *)(ptr4 + 20))
#define REG_FILTER_NETWORK_6   *((unsigned *)(ptr4 + 24)) 
#define REG_FILTER_NETWORK_7   *((unsigned *)(ptr4 + 28))
#define REG_FILTER_NETWORK_8   *((unsigned *)(ptr4 + 32))
#define REG_FILTER_NETWORK_9   *((unsigned *)(ptr4 + 36)) 
#define REG_FILTER_NETWORK_10  *((unsigned *)(ptr4 + 40))
#define REG_FILTER_NETWORK_11  *((unsigned *)(ptr4 + 44))
#define REG_FILTER_NETWORK_12  *((unsigned *)(ptr4 + 48)) 
#define REG_FILTER_NETWORK_13  *((unsigned *)(ptr4 + 52))
#define REG_FILTER_NETWORK_14  *((unsigned *)(ptr4 + 56))
#define REG_FILTER_NETWORK_15  *((unsigned *)(ptr4 + 60)) 
#define REG_FILTER_NETWORK_16  *((unsigned *)(ptr4 + 64))
#define REG_FILTER_NETWORK_17  *((unsigned *)(ptr4 + 68))
#define REG_FILTER_NETWORK_18  *((unsigned *)(ptr4 + 72)) 
#define REG_FILTER_NETWORK_19  *((unsigned *)(ptr4 + 76))

//Filter for Line In
#define REG_FILTER_LINE_IN_0   *((unsigned *)(ptr1 + 0))
#define REG_FILTER_LINE_IN_1   *((unsigned *)(ptr1 + 4))
#define REG_FILTER_LINE_IN_2   *((unsigned *)(ptr1 + 8))
#define REG_FILTER_LINE_IN_3   *((unsigned *)(ptr1 + 12)) 
#define REG_FILTER_LINE_IN_4   *((unsigned *)(ptr1 + 16))
#define REG_FILTER_LINE_IN_5   *((unsigned *)(ptr1 + 20))
#define REG_FILTER_LINE_IN_6   *((unsigned *)(ptr1 + 24)) 
#define REG_FILTER_LINE_IN_7   *((unsigned *)(ptr1 + 28))
#define REG_FILTER_LINE_IN_8   *((unsigned *)(ptr1 + 32))
#define REG_FILTER_LINE_IN_9   *((unsigned *)(ptr1 + 36)) 
#define REG_FILTER_LINE_IN_10  *((unsigned *)(ptr1 + 40))
#define REG_FILTER_LINE_IN_11  *((unsigned *)(ptr1 + 44))
#define REG_FILTER_LINE_IN_12  *((unsigned *)(ptr1 + 48)) 
#define REG_FILTER_LINE_IN_13  *((unsigned *)(ptr1 + 52))
#define REG_FILTER_LINE_IN_14  *((unsigned *)(ptr1 + 56))
#define REG_FILTER_LINE_IN_15  *((unsigned *)(ptr1 + 60)) 
#define REG_FILTER_LINE_IN_16  *((unsigned *)(ptr1 + 64))
#define REG_FILTER_LINE_IN_17  *((unsigned *)(ptr1 + 68))
#define REG_FILTER_LINE_IN_18  *((unsigned *)(ptr1+ 72)) 
#define REG_FILTER_LINE_IN_19  *((unsigned *)(ptr1 + 76))

// Volume for Line In
#define REG_VOLUME_LINE_IN_0  *((unsigned *)(ptr2 + 0))
#define REG_VOLUME_LINE_IN_1  *((unsigned *)(ptr2 + 4))

// Volume for Network
#define REG_VOLUME_NETWORK_0  *((unsigned *)(ptr3 + 0))
#define REG_VOLUME_NETWORK_1  *((unsigned *)(ptr3 + 4))


int main(int argc, char *argv[]) {
    if (*argv[1] == 'p') {
        printf("::::START_USAGE::::\n\n");
        printf("::::::Volume Control:::::::\n");
        printf("EXAMPLE: %s l v 2 3\n", argv[0]);
        printf("Where \"l\" and \"v\" represent Line In and volume\n");
        printf("Where \"2\" and \"3\" represent Right and left volume gain values repectively for Line In\n");
        printf("EXAMPLE: %s n v 2 3\n", argv[0]);
        printf("Where \"n\" and \"v\" represent Network and volume\n");
        printf("Where \"2\" and \"3\" represent Right and left volume gain values repectively for Network\n");
        printf("NB: To decrease volume, enter a negative number\n");
		printf("Amplification values must be between -16 and 16\n\n");
        printf("::::::Filter Control:::::::\n");
        printf("EXAMPLE: %s l f 1 0 0\n", argv[0]);
        printf("Where \"l\" and \"f\" represent Line In and Filter\n");
        printf("Where \"1 0 0\" represent HP, BP & LP respectively for Line In\n");
        printf("EXAMPLE: %s n f 1 0 0\n", argv[0]);
        printf("Where \"n\" and \"f\" represent Network and Filter\n");
        printf("Where \"1 0 0\" represent HP, BP & LP respectively for Network\n");
        printf("NB: Any number greater or less than 0 will default to 1\n\n");
        printf("::::END_USAGE::::\n");
    }else {
  
		printf("Start to control volume and filter for Line In or Network\n");
		//take inputs from user
		unsigned controlInput1;
		unsigned controlInput2;
		unsigned controlInput3;
		signed controlInput4;
		signed controlInput5;
		/*For LED's which blow as per user input*/
		FILE * fled0, * fled7;
		//OLED dev/uio5
        int fd5 = open ("/dev/uio5", O_RDWR);
        if (fd5 < 1) { perror(argv[0]); return -1; }
        
		if (*argv[1] == 'f'){
		controlInput1 = atoi(argv[2]);
		controlInput2 = atoi(argv[3]);
		controlInput3 = atoi(argv[4]);
		}
		if (*argv[1] == 'v'){
		controlInput4 = atoi(argv[2]);
		controlInput5 = atoi(argv[3]);
		}

		//open dev/uio1 & dev/uio2
		int fd1 = open ("/dev/uio1", O_RDWR);
		int fd2 = open ("/dev/uio2", O_RDWR);
		int fd3 = open ("/dev/uio3", O_RDWR);
		int fd4 = open ("/dev/uio4", O_RDWR);
		
		if (fd1 < 1) { perror(argv[0]); return -1; }
		if (fd2 < 1) { perror(argv[0]); return -1; }
		if (fd3 < 1) { perror(argv[0]); return -1; }
		if (fd4 < 1) { perror(argv[0]); return -1; }
		
		printf("opened user space dev/uio1 & dev/uio2\n");
		//Redirect stdout/printf into /dev/kmsg file (so it will be printed using printk)
		freopen ("/dev/kmsg","w",stdout);

		//get architecture specific page size
		unsigned pageSize = sysconf(_SC_PAGESIZE);

		void *ptr1;
		void *ptr2;
		void *ptr3;
		void *ptr4;
		void *ptr5;
        ptr5 = mmap(NULL, pageSize, (PROT_READ |PROT_WRITE), MAP_SHARED, fd5, 0);
		ptr1 = mmap(NULL, pageSize, (PROT_READ |PROT_WRITE), MAP_SHARED, fd1, 0);
		ptr2 = mmap(NULL, pageSize, (PROT_READ |PROT_WRITE), MAP_SHARED, fd2, 0);
		ptr3 = mmap(NULL, pageSize, (PROT_READ |PROT_WRITE), MAP_SHARED, fd3, 0);
		ptr4 = mmap(NULL, pageSize, (PROT_READ |PROT_WRITE), MAP_SHARED, fd4, 0);
		printf("mmap is okay \n");
				
		//write coefficients into registers for Line In
		REG_FILTER_LINE_IN_0 = 0x00002CB6;
		REG_FILTER_LINE_IN_1 = 0x0000596C;
		REG_FILTER_LINE_IN_2 = 0x00002CB6;
		REG_FILTER_LINE_IN_3 = 0x8097A63A;
		REG_FILTER_LINE_IN_4 = 0x3F690C9D;
		REG_FILTER_LINE_IN_5 = 0x074D9236;
		REG_FILTER_LINE_IN_6 = 0x00000000;
		REG_FILTER_LINE_IN_7 = 0xF8B26DCA;
		REG_FILTER_LINE_IN_8 = 0x9464B81B;
		REG_FILTER_LINE_IN_9 = 0x3164DB93;
		REG_FILTER_LINE_IN_10 = 0x12BEC333;
		REG_FILTER_LINE_IN_11 = 0xDA82799A;
		REG_FILTER_LINE_IN_12 = 0x12BEC333;
		REG_FILTER_LINE_IN_13 = 0x00000000;
		REG_FILTER_LINE_IN_14 = 0x0AFB0CCC;
		printf("filt coeffs is okay\n");

		//set reset for filter device to zero
		REG_FILTER_LINE_IN_15 = 0;
		//set REG16 to constant 1
		REG_FILTER_LINE_IN_16 = 1;
		printf("enable sample and disable reset\n");
		
		//write coefficients into registers for Network
		
		REG_FILTER_NETWORK_0 = 0x00002CB6;
		REG_FILTER_NETWORK_1 = 0x0000596C;
		REG_FILTER_NETWORK_2 = 0x00002CB6;
		REG_FILTER_NETWORK_3 = 0x8097A63A;
		REG_FILTER_NETWORK_4 = 0x3F690C9D;
		REG_FILTER_NETWORK_5 = 0x074D9236;
		REG_FILTER_NETWORK_6 = 0x00000000;
		REG_FILTER_NETWORK_7 = 0xF8B26DCA;
		REG_FILTER_NETWORK_8 = 0x9464B81B;
		REG_FILTER_NETWORK_9 = 0x3164DB93;
		REG_FILTER_NETWORK_10 = 0x12BEC333;
		REG_FILTER_NETWORK_11 = 0xDA82799A;
		REG_FILTER_NETWORK_12 = 0x12BEC333;
		REG_FILTER_NETWORK_13 = 0x00000000;
		REG_FILTER_NETWORK_14 = 0x0AFB0CCC;
		printf("filt coeffs is okay\n");

		//set reset for filter device to zero
		REG_FILTER_NETWORK_15 = 0;
		//set REG16 to constant 1
		REG_FILTER_NETWORK_16 = 1;
		printf("enable sample and disable reset\n");
		
		//Writing into OLED
		oled_clear(ptr5);
		if (!oled_print_message("SoC Audio Mixer Project",0, ptr5)){
			perror("Error:Not able to write\n");
			exit(-1);
		}
		//User input logic for filter and volume devices
		switch (*argv[1]){
		case 'v': //Volume control
			if (!oled_print_message("Volume change ",1, ptr5)){
				perror("Error:Not able to write\n");
				exit(-1);
			}
			if(controlInput4 >= 0 && controlInput4 <=16) 
				controlInput4 = controlInput4 * 256;
			else if(controlInput4 < 0 && controlInput4 >= -16)
				controlInput4 = 256/abs(controlInput4);
			else{
				printf("Wrong input value specified\n");
				printf("Run <%s p> to see usage\n", argv[0]);
				break;
			}
			if(controlInput5 >= 0 && controlInput5 <=16) 
				controlInput5 = controlInput5 * 256;
			else if(controlInput5 < 0 && controlInput5 >= -16)
				controlInput5 = 256/abs(controlInput5);
			else{
				printf("Wrong input value specified\n");
				printf("Run <%s p> to see usage\n", argv[0]);
				break;
			}
			printf("before assigning multiplication values\n");
			if (*argv[4] == 'l'){
				REG_VOLUME_LINE_IN_0 = controlInput4;
				REG_VOLUME_LINE_IN_1 = controlInput5;
				fled0 = fopen ("/sys/class/gpio/gpio224/value", "w");
				fprintf(fled0 ,"%d", 1);
				fled7 = fopen ("/sys/class/gpio/gpio231/value", "w");
				fprintf(fled7 ,"%d", 0);
				        //Writing into OLED
				if (!oled_print_message("for Line In",2, ptr5)){
					perror("Error:Not able to write\n");
					exit(-1);
				}
				break;
			}else if(*argv[4] == 'n'){
				REG_VOLUME_NETWORK_0 = controlInput4;
				REG_VOLUME_NETWORK_1 = controlInput5;
				fled0 = fopen ("/sys/class/gpio/gpio224/value", "w");
				fprintf(fled0 ,"%d", 0);
				fled7 = fopen ("/sys/class/gpio/gpio231/value", "w");
				fprintf(fled7 ,"%d", 1);
				if (!oled_print_message("for Network",2, ptr5)){
					perror("Error:Not able to write\n");
					exit(-1);
				}
				break;
			}else{
				printf("Wrong input value specified\n");
				printf("Run <%s p> to see usage\n", argv[0]);
				break;				
			}
		case 'f': //Filter control
			if (!oled_print_message("FRQ(Hz) change",1, ptr5)){
				perror("Error:Not able to write\n");
				exit(-1);
			}
			if(controlInput1 > 1 || controlInput1 < 0)
				controlInput1 = 1;
			if(controlInput2 > 1 || controlInput2 < 0)
				controlInput2 = 1;
			if(controlInput3 > 1 || controlInput3 < 0)
				controlInput3 = 1;
			if (*argv[5] == 'l'){
				REG_FILTER_LINE_IN_17 = controlInput1;
				REG_FILTER_LINE_IN_18 = controlInput2;
				REG_FILTER_LINE_IN_19 = controlInput3;
				fled0 = fopen ("/sys/class/gpio/gpio224/value", "w");
				fprintf(fled0 ,"%d", 1);
				fled7 = fopen ("/sys/class/gpio/gpio231/value", "w");
				fprintf(fled7 ,"%d", 0);
				if (!oled_print_message(" for Line In",2, ptr5)){
					perror("Error:Not able to write\n");
					exit(-1);
				}
				break;
			}else if(*argv[5] == 'n'){
				REG_FILTER_NETWORK_17 = controlInput1;
				REG_FILTER_NETWORK_18 = controlInput2;
				REG_FILTER_NETWORK_19 = controlInput3;
				fled0 = fopen ("/sys/class/gpio/gpio224/value", "w");
				fprintf(fled0 ,"%d", 0);
				fled7 = fopen ("/sys/class/gpio/gpio231/value", "w");
				fprintf(fled7 ,"%d", 1);
				if (!oled_print_message(" for Network",2, ptr5)){
					perror("Error:Not able to write\n");
					exit(-1);
				}
				break;
			}else{
				printf("Wrong input value specified\n");
				printf("Run <%s p> to see usage\n", argv[0]);
				break;				
			}
		default: //Notify unsupported control
			printf("Unsupported control specified\n");
			printf("Run <%s p> to see usage\n", argv[0]);
			break;
			
		}
		//unmap
		munmap(ptr1, pageSize);
		munmap(ptr2, pageSize);
		munmap(ptr3, pageSize);
		munmap(ptr4, pageSize);
		fclose(fled0);
		fclose(fled7);
		//close
		fclose(stdout);
	}
	return 0;
}
