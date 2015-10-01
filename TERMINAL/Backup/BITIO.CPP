#include <stdio.h>
#include <stdlib.h>
#include "decomp.h"


BIT_FILE *OpenOutputBitFile(char *name )
{
    BIT_FILE *bit_file;

    bit_file = (BIT_FILE *) calloc( 1, sizeof( BIT_FILE ) );
    if ( bit_file == NULL )
        return( bit_file );
    bit_file->file = fopen( name, "wb" );
    bit_file->rack = 0;
    bit_file->mask = 0x80;
    bit_file->pacifier_counter = 0;
    return( bit_file );
}


int CloseOutputBitFile(BIT_FILE *bit_file )
{
    if ( bit_file->mask != 0x80 )
        if ( putc( bit_file->rack, bit_file->file ) != bit_file->rack )
            return ERR_ECRIT;
    fclose( bit_file->file );
    free( (char *) bit_file );
    return OK;
}


int OutputBits(BIT_FILE *bit_file,unsigned long code,int count )
{
    unsigned long mask;

    mask = 1L << ( count - 1 );
    while ( mask != 0) {
        if ( mask & code )
            bit_file->rack |= bit_file->mask;
        bit_file->mask >>= 1;
        if ( bit_file->mask == 0 ) {
	    if ( putc( bit_file->rack, bit_file->file ) != bit_file->rack )
		return ERR_ECRIT;
	    bit_file->pacifier_counter++;
	    bit_file->rack = 0;
            bit_file->mask = 0x80;
        }
        mask >>= 1;
    }
   return OK;
}

int InputBits(BIT_INPUT *bit_file,int bit_count,unsigned int *val_ret )
{
    unsigned long mask;
    unsigned long return_value;

    mask = 1L << ( bit_count - 1 );
    return_value = 0;
    while ( mask != 0) {
	if ( bit_file->mask == 0x80 ) {
	   if ( bit_file->size--)
	             bit_file->rack = *(bit_file->data++);
		else return ERR_DCOMP;
	    }
	if ( bit_file->rack & bit_file->mask )
            return_value |= mask;
        mask >>= 1;
        bit_file->mask >>= 1;
        if ( bit_file->mask == 0 )
            bit_file->mask = 0x80;
    }
    *val_ret=(unsigned int) return_value ;
    
    return OK;
}
