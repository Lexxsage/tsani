#include "tsani.h"
#include "avalon.h"

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// Static global variables

//==============================================================================
// Static functions

//==============================================================================
// Global variables

//==============================================================================
// Global functions

void init(){
	portMask(0, 0xFF);
	portOut(0, 0x00);
	portMask(1, 0xFF);
	portOut(1, 0x00);
	portMask(2, 0x07);			 
	portOut(2, 0x06);
}


void add(unsigned char address, unsigned int subaddress, unsigned int* adr){
	*adr = ((unsigned int)address)|(subaddress)<<3;
}

void divide(unsigned int address, unsigned char *adr0, unsigned char* adr1){
	*adr0= (unsigned char)address;
	*adr1 = (unsigned char)(address>>8);
}

void write(unsigned int address, unsigned int data){
	unsigned char w0,w1;
	divide(address,&w0, &w1);
	
	portOut(0, w0);
	portOut(1, w1);
	portOut(2, 0x07);
	portOut(2, 0x06);
	divide(data,&w0, &w1);
	portOut(0, w0);
	portOut(1, w1);
	portOut(2, 0x02);
	portOut(2, 0x06);
}

void read(unsigned int address, unsigned int* data){
	unsigned char r0;
	unsigned char r1;
	divide(address,&r0, &r1);
	portOut(0, r0);
	portOut(1, r1);
	portOut(2, 0x07);
	portOut(2, 0x06);
	portMask(0, 0x00);
	portMask(1, 0x00);
	portOut(2, 0x04);
	
	portIn(0, &r0);		//read data from shina 
	portIn(1, &r1); 	 
	
    //write in read 1 
	portOut(2, 0x06); 
	portMask(0, 0xFF);   //  change in do write 
	portMask(1, 0xFF);
	
	*data = (unsigned int)r0 | (unsigned int)(r1<<8); 
}

void code(unsigned char code){
   unsigned int adr;
   add(0x02, 0x00, &adr);
   write(adr, 0x03);
   add(0x02, 0x02, &adr);
   write(adr, (unsigned int)code);
   add(0x02, 0x03, &adr);
   write(adr, (unsigned int)code);
}

void volt(double v){
	unsigned int adr, n;
	unsigned char code;
	
	if (v<0){
		code = 0;
	}
	else {
		n = (unsigned int)255*v/3.3;
		code = (unsigned char)n;
	}
	
	add(0x02, 0x00, &adr);
    write(adr, 0x03);
	add(0x02, 0x02, &adr);
   write(adr, (unsigned int)code);
   add(0x02, 0x03, &adr);
   write(adr, (unsigned int)code);
}

void acp(double *data){
   unsigned int adr, data1;
   
   add(0x02,0x10,&adr);
   write(adr, 0x05);
   add(2,0x12,&adr);
   write(adr,0x00);
   add(2,0x13,&adr);
   write(adr,0x00);
   add(2,0x14,&adr);
   write(adr,0x00);
   add(2,0x11,&adr);
   write(adr,0x03);
   
   do{read(adr, &data1);
   } while ((data1 & 0x01)==0x01);
   
   add(0x02, 0x16, &adr);
   read(adr, &data1);
   
   *data = data1 * 2.56/1024; 
}
