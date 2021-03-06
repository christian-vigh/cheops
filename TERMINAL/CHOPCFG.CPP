/**************************************************************************/
/*                                                                        */
/*  ChopCfg.cpp  -  Christian Vigh, 95/10/13.                             */
/*                                                                        */
/*  Impl�mentation de la classe TCheopsConfiguration.                     */
/*                                                                        */
/**************************************************************************/


# include	"AllTerm.h"
# pragma	hdrstop


# include	"MNCommon.h"
# include	"ChopCfg.h"
# include	"UnixApp.h"
# include	"Event.h"

# define	DEFAULTS_SECTION		"Defaults"
# define	DEFAULT_FILE			"Cheops.ini"



TCheopsConfiguration *	CheopsConfiguration	=  NULL ;





/**************************************************************************/
/*                                                                        */
/*  TCheopsConfiguration :: SetFileName -                                 */
/*      D�finit le nom du fichier .ini � utiliser.                        */
/*                                                                        */
/**************************************************************************/

void	TCheopsConfiguration :: SetFileName  ( char *	filename )
   {
	char			buffer [ MAX_PARAMETER_LENGTH ] ;
	register char *         p ;



	if  ( ! filename )
	   {
		filename = DEFAULT_FILE ;


	// R�cup�ration du r�pertoire de l'application
		GetApplicationObject ( )  -> GetModuleFileName ( buffer, MAX_PARAMETER_LENGTH ) ;
		p = buffer + strlen ( buffer ) - 1 ;

		while  ( p >= buffer )
		   {
			if  ( strchr ( "\\/:", * p )  !=  NULL )
				break ;
			p -- ;
		    }

		p ++ ;
		* p = 0 ;
		strcat ( buffer, filename ) ;
		filename = buffer ;
	     }


	if  ( FileName )
		free ( FileName ) ;

	FileName = strdup ( filename ) ;
    }








/**************************************************************************/
/*                                                                        */
/*  TCheopsConfiguration :: Get -                                         */
/*      Lit une cha�ne, un bool�en, un int ou un long.                    */
/*      Si la section sp�cifi�e ne contient pas l'�l�ment, il sera        */
/*      cherch� dans la section "Defaults".                               */
/*                                                                        */
/**************************************************************************/


//
//
// Version Cha�ne de caract�res, utilis�e par toutes les autres
// Pour v�rifier que la Set sp�cifi�e existe, on appelle GetProfileString
// avec une valeur par d�faut unique.
//
//
# define	UNIQUE		"!!&!@!#!"

void	TCheopsConfiguration :: Get  ( const char * 	section,
				       const char *  	item,
				       char *		output,
				       char *		defval )
   {
	GetPrivateProfileString ( section, item, UNIQUE, output,
		MAX_PARAMETER_LENGTH, FileName ) ;

	if  ( ! strcmp ( output, UNIQUE ) )
	   {
		GetPrivateProfileString ( DEFAULTS_SECTION, item, UNIQUE, output,
			MAX_PARAMETER_LENGTH, FileName ) ;

		if  ( ! strcmp ( output, UNIQUE ) )
			strcpy ( output, ( defval ) ?  defval : "" ) ;
	     }
     }



//
//
//  Version  Bool�en
//
//
void	TCheopsConfiguration :: Get  ( const char * 	section,
				       const char * 	item,
				       boolean &	output,
				       boolean		defval )
   {
	char		Buffer [ MAX_PARAMETER_LENGTH ] ;


	Get ( section, item, Buffer, ( defval ) ?  "True" : "False" ) ;


	if  ( ! stricmp ( Buffer, "True" )  || 
	      ! stricmp ( Buffer, "On" )    ||
	      ! stricmp ( Buffer, "Yes" )   ||
	      ! strcmp  ( Buffer, "1" ) )
		output = True ;
	else
		output = False ;
      }		
				




//
//
//  Version int
//
//
void	TCheopsConfiguration :: Get  ( const char *	section,
				       const char *	item,
				       int &		output,
				       int		defval )
   {
	long int	value ;


	Get ( section, item, value, ( long int )  defval ) ;
	output = ( int ) value ;
    }




//
//
//  Version long int
//
//
//
void	TCheopsConfiguration :: Get  ( const char *	section,
				       const char *	item,
				       long int	&	output,
				       long int		defval )
   {
	char		Buffer [ MAX_PARAMETER_LENGTH ],
			def    [ 128 ] ;


// R�cup�ration de la valeur
	wsprintf ( def, "%ld", defval ) ;
	Get ( section, item, Buffer, def ) ;


// Analyse
	register char *		p	=  Buffer ;
	register int		c,
				base 	=  10 ;
	long int		value   =  0 ;


// Sauter les blancs
	while  ( isspace ( * p ) )
		p ++ ;


// Si on arrive sur un z�ro, il y a peut-�tre une base qui suit
	if  ( * p  ==  '0' )  
	   {
		switch ( toupper ( * ( p + 1 ) ) )
		   {
			case 'X' : base = 16 ; p += 2 ; break ;
			case 'D' : base = 10 ; p += 2 ; break ;
			case 'O' : base =  8 ; p += 2 ; break ;
			case 'B' : base =  2 ; p += 2 ; break ;
			default  : p ++ ;	// Sauter le z�ro
		     }
	     }


// La base a �t� d�termin�e (si elle �tait sp�cifi�e) -> Analyse des
// chiffres suivants
	value = 0 ;

	while  ( isxdigit ( * p ) ) 
	  {
		if  ( isdigit ( * p ) ) 
			c  =  * p - '0' ;
		else
			c  =  toupper ( * p ) - 'A' + 10 ;

		if  ( c  >=  base )
			break ;

		value = ( value * base ) + c ;
		p ++ ;
	     }

// Yabon
	output = value ;
    }




/**************************************************************************/
/*                                                                        */
/*  TCheopsConfiguration :: Set -                                         */
/*      Ecrit une cha�ne, un bool�en, un int ou un long.                  */
/*      Si "section" est nul, le param�tre est �crit dans celle par defaut*/
/*                                                                        */
/**************************************************************************/

void	TCheopsConfiguration :: Set  ( const char *	section,
				       const char *	item,
				       char *		buffer )
   {
	char			Buffer [ 512 ] ;
	register char *		p	=  Buffer ;	


	if  ( ! section )	
		section = DEFAULTS_SECTION ;

	WritePrivateProfileString ( section, item, buffer, FileName ) ;


	strcpy ( p, section ) ;
	p += strlen ( p ) + 1 ;

	strcpy ( p, ( item ) ?  item : "" ) ;
	p += strlen ( p ) + 1 ;

	strcpy ( p, ( buffer ) ?  buffer : "" ) ;
	p += strlen ( p ) + 1 ;


# if	! STANDALONE	
	EventQueue -> Enqueue ( ApplicationId, ObjectId, ProcessId, 
		WM_MENU_EVENT, MNF_INI_FILE,
			( LPARAM ) Buffer, ( int ) ( p - Buffer ) ) ;
# endif
    }




void	TCheopsConfiguration :: Set  ( const char *	section,
				       const char *	item,
				       boolean		val )
   {
	Set ( section, item, ( val ) ? "True" : "False" ) ;
    }



void	TCheopsConfiguration :: Set  ( const char *	section,
				       const char *	item,
				       int		value ) 
   {
	char		buffer [32] ;


	wsprintf ( buffer, "%d", value ) ;
	Set ( section, item, buffer ) ;
    }




void	TCheopsConfiguration :: Set  ( const char *	section,
				       const char *	item,
				       long int		value ) 
   {
	char		buffer [32] ;


	wsprintf ( buffer, "%ld", value ) ;
	Set ( section, item, buffer ) ;
    }

