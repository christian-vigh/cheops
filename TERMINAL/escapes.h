/**************************************************************************/
/*                                                                        */
/*  Escapes.h  -  Christian Vigh, 95/03/15.                               */
/*                                                                        */
/*  D�finition des caract�res ascii � "�chapper".                         */
/*                                                                        */
/**************************************************************************/


/***
   La table EscapeTable comprend une entr�e par caract�re ascii.
   Lorsque le contenu de cette entr�e est non nul, cela signifie que le
   caract�re correspondant doit �tre "�chapp�" durant les transmissions
   s�rie.
***/


char	EscapeTable [ 32 ]	=
   {
	1,		/* 000: NUL, intercept� par je ne sais pas qui */
	0,		/* 001: SOH */
	0, 	/* 002: STX */
	0, 	/* 003: ETX */
	0, 	/* 004: EOT */
	0, 	/* 005: ENQ */
	0, 	/* 006: ACK */
	0, 	/* 007: BEL */
	0, 	/* 008: BS  */
	0, 	/* 009: HT  */
	0, 	/* 010: LF  */
	0, 	/* 011: VT  */
	0, 	/* 012: FF  */
	1, 	/* 013: CR  */
	0, 	/* 014: SO  */
	0, 	/* 015: SI  */
	0, 	/* 016: DLE */
	1, 	/* 017: DC1, XON intercept� par le mat�riel */
	0, 	/* 018: DC2 */
	1, 	/* 019: DC3, XOFF intercept� par le mat�riel */
	0, 	/* 020: DC4 */
	0, 	/* 021: NAK */
	1, 	/* 022: SYN, caract�re de synchronisation en cas de pb */
	0, 	/* 023: ETB */
	0, 	/* 024: CAN */
	0, 	/* 025: EM  */
	0, 	/* 026: SUB */
	1, 	/* 027: ESC, d�but de l'en-t�te et de la fin d'un message */
	0, 	/* 028: FS  */
	0, 	/* 029: GS  */
	0, 	/* 030: RS  */
	0, 	/* 031: US  */
    } ;
