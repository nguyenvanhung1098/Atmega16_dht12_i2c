/*
 * i2c_with_dht12.c
 *
 * Created: 6/18/2020 4:06:05 PM
 * Author: ASUS
 */

#include <io.h>
#include <delay.h>  

#define BITRATE(TWSR)	((F_CPU/SCL_CLK)-16)/(2*pow(4,(TWSR&((1<<TWPS0)|(1<<TWPS1)))))
#define F_CPU 1000000UL
#define dht_addr 0xB8
char value[5];
value[4] = ' ';

void init()
{
   TWBR = BITRATE(TWSR=0x00);	/* Get bit rate register value by formula */
}

//--- i2c start condition
void I2C_Start(char write_address)/* I2C start function */
{
    TWCR = (1<<TWSTA)|(1<<TWEN)|(1<<TWINT); /* Enable TWI, generate START */
    while(!(TWCR&(1<<TWINT)));    /* Wait until TWI finish its current job */

    TWDR = write_address;        /* Write SLA+W in TWI data register */
    TWCR = (1<<TWEN)|(1<<TWINT);    /* Enable TWI & clear interrupt flag */
        while(!(TWCR&(1<<TWINT)));    /* Wait until TWI finish its current job */
}

void I2C_Repeated_Start(char read_address) /* I2C repeated start function */
{
    TWCR=(1<<TWSTA)|(1<<TWEN)|(1<<TWINT);/* Enable TWI, generate start */
    while(!(TWCR&(1<<TWINT)));	/* Wait until TWI finish its current job */

    TWDR=read_address;		/* Write SLA+R in TWI data register */
    TWCR=(1<<TWEN)|(1<<TWINT);	/* Enable TWI and clear interrupt flag */
    while(!(TWCR&(1<<TWINT)));	/* Wait until TWI finish its current job */
}

void I2C_Write(char data)	/* I2C write function */
{
    TWDR=data;			/* Copy data in TWI data register */
    TWCR=(1<<TWEN)|(1<<TWINT);	/* Enable TWI and clear interrupt flag */
    while(!(TWCR&(1<<TWINT)));	/* Wait until TWI finish its current job */
}

char I2C_Read_Ack()		/* I2C read ack function */
{
    TWCR=(1<<TWEN)|(1<<TWINT)|(1<<TWEA); /* Enable TWI, generation of ack */
    while(!(TWCR&(1<<TWINT)));	/* Wait until TWI finish its current job */
    return TWDR;			/* Return received data */
}

char I2C_Read_Nack()		/* I2C read nack function */
{
    TWCR=(1<<TWEN)|(1<<TWINT);	/* Enable TWI and clear interrupt flag */
    while(!(TWCR&(1<<TWINT)));	/* Wait until TWI finish its current job */
    return TWDR;		/* Return received data */
}

void I2C_Stop()			/* I2C stop function */
{
    TWCR=(1<<TWSTO)|(1<<TWINT)|(1<<TWEN);/* Enable TWI, generate stop */
    //while(TWCR&(1<<TWSTO));	/* Wait until stop condition execution */
}


// read data from sensor dht12
void read_dht12(){
   /*-------------------------------- Doc do am  ------------------------------------------*/
    I2C_Start(dht_addr); /* Start I2C with device write address */
    I2C_Write(0x00);		/* Write start memory address */ 
    I2C_Repeated_Start(dht_addr | 1); /* Repeat start I2C SLA+R */ 
    value[0] = I2C_Read_Ack();        /* read humidity */
    value[1] = I2C_Read_Nack();       /* read humidity scale */
    I2C_Stop();
    
    /*------------------------------- Doc nhiet do -----------------------------------------*/
    I2C_Start(dht_addr); /* Start I2C with device write address */
    I2C_Write(0x02);		/* Write start memory address */    
    I2C_Repeated_Start(dht_addr | 1);/* Repeat start I2C SLA+R */ 
    value[2] = I2C_Read_Ack();    /* read Temperature */
    value[3] = I2C_Read_Nack();   /* read Temperature scale */   
    I2C_Stop();
}


void main(void)
{
    init(); 
    while (1)
    {                
        delay_ms(100);
        read_dht12(); 
    }
}
