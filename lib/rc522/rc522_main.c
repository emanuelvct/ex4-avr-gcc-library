/*****************************************************
This program was produced by the
CodeWizardAVR V2.05.6 
Automatic Program Generator
� Copyright 1998-2012 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : RC522
Version : 1
Date    : 2/27/2015
Author  : EX4
Company : EX4 TECH
Comments: 
RC522 RFID READER WRITER


Chip type               : ATmega32
Program type            : Application
AVR Core Clock frequency: 12.000000 MHz
Memory model            : Small
External RAM size       : 0
Data Stack size         : 512
*****************************************************/

// avr lib
#include <mega32.h>
#include <delay.h>

// Standard Input/Output functions
#include <stdio.h>
#include <string.h>

// spi lib
#include <spix.h>

// rc522 lib
#include <mfrc522.h>

// mode debug
#define DEBUG_ON


/*
usart setup for debugging at 8-N-1-9600bps-No Parity
*/
void usart_init(void)
{
    UCSRA=0x00;
    UCSRB=0x18;
    UCSRC=0x86;
    UBRRH=0x00;
    UBRRL=0x4D;
}

/*
check rc522 rfid reader writer presence   
*/
void rc522_check_reader()
{
	uint8_t curr_read_status = mfrc522_get_version();
	if (curr_read_status<0x90 || curr_read_status>0x92)
	{         
        #if defined(DEBUG_ON)
		    printf("NO READER\n");
        #endif
		for(;;);
	}
}

/*
read card serial id
*/
uint8_t rc522_read_card_id(uint8_t *card_id, uint16_t *card_type)
{
	uint8_t status, resx = 0;
	uint8_t buff_data[MAX_LEN]; 

    *card_type = 0;	
	if(mfrc522_is_card(card_type))
	{
		status = mfrc522_get_card_serial(buff_data);
		if (status==CARD_FOUND)
		{
			memcpy(card_id,buff_data,5);//kopi id and checksum at last byte (5th)
			resx = 1;
		}
		else
		{
			resx = 0;
		}
	}
	else
	{
		resx = 0;
	}
	
	return resx;
}

/*
  main program
*/
void main(void)
{
    //local var
    uint8_t curr_id[5]; 
    uint16_t card_tipe;    
    uint8_t str[MAX_LEN]; 
    uint8_t status; 
    
    //start up delay
    delay_ms(1000);
    
    //init usart
    usart_init(); 
    
    #if defined(DEBUG_ON)
    printf("RC522 TEST\n"); 
    #endif

    // init spi as master
    spi_init();
    
    // init rc522
    mfrc522_init();
    
    // check reader attached???
    rc522_check_reader();
    
    #if defined(DEBUG_ON)
    printf("RC522 DETECTED\n"); 
    #endif

    while (1)
    {
        // read id card
        if (rc522_read_card_id(curr_id, &card_tipe))
        {
            printf("%.2X%.2X%.2X%.2X%.2X %.4X\n", curr_id[0], curr_id[1], curr_id[2], curr_id[3], curr_id[4], card_tipe);  
            
            // select the tag
            printf("select = %.2X\n", mfrc522_select_tag(curr_id)); 
            
            //login to block 4 
            printf("login = %d\n", mfrc522_auth(PICC_AUTHENT1A, 4, keyA_default, curr_id)); 
            
            //read block 4 
            status = mfrc522_read_block(4, str);
            printf("read = %d  %.2X %.2X %.2X\n", status, str[0], str[1], str[2]); 
            
//            delay_ms(1000); 
//            
//            //login to block 4 
//            printf("login = %d\n", mfrc522_auth(PICC_AUTHENT1A, 4, keyA_default, curr_id));
//            
//            //write block 4
//            str[0] = 1;
//            str[1] = 2;
//            str[2] = 3;
//            status = mfrc522_write_block(4, str);  
//            
//            delay_ms(1000);
//            
//            //login to block 4 
//            printf("login = %d\n", mfrc522_auth(PICC_AUTHENT1A, 4, keyA_default, curr_id)); 
//            
//            //read block 4 
//            status = mfrc522_read_block(4, str);
//            printf("read = %d  %.2X %.2X %.2X\n", status, str[0], str[1], str[2]); 
        } 
        
        //halt the card
        mfrc522_halt();
        
        //delay
        delay_ms(100);  
                
        //eternal loop
//        for(;;);      
    }
}
