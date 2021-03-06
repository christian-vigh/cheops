/**************************************************************************/
/*                                                                        */
/*   Util.h  -  Christian Vigh, 95/11.                                    */
/*                                                                        */
/*   Fonctions utilitaires.                                               */
/*                                                                        */
/**************************************************************************/



# if	! defined ( __UTIL_H__ )
#	  define    __UTIL_H__ 



# define	MAX_PARAMETER_LENGTH		256



/**************************************************************************/
/*                                                                        */
/*   Structure d�finissant les bo�tes de dialogue Windows.                */
/*                                                                        */
/**************************************************************************/

typedef struct           	// En-t�te
   {			
		long int	dtStyle;         // Style de la bo�te
		BYTE 		dtItemCount;     // Nombre de contr�les
		int 		dtX,             // Dimension de la bo�te
				dtY,
				dtCX,
				dtCY ;           
		char *		dtMenuName;	// Menu associ�
		char *		dtClassName;	// Nom de la classe
		char *		dtCaptionText;	// Titre de la fen�tre
    }  DLGTEMPLATE ;


typedef struct 					// Information sur la police
   {
		short int 	PointSize;	// Taille de la police
		char *		szTypeFace;	// Nom de la police
    }  FONTINFO ;


typedef struct 					// D�finition d'un contr�le
   { 						// du dialogue
											 // des contr�les d'une Boite de Dialogue
		int 		dtilX,  	// Dimension du contr�le
				dtilY,
				dtilCX,
				dtilCY ;
		int		dtilID ;	// ID du contr�le
		long int	dtilStyle ;	// Style du contr�le
		char *		dtilClass ; 	// Classe du contr�le
		char *		dtilText ; 	// Texte du contr�le
		BYTE 		dtilInfo ; 	// Nombre d'info suppl�mentaires
		LPSTR 		dtilData ;	// Informations suppl�mentaires
     } DLGITEMTEMPLATE ;



typedef struct					// Par commodit�
   {
		DLGTEMPLATE		DialogTemplate ;
		FONTINFO		FontInfo ;
		DLGITEMTEMPLATE *	DialogItemTemplates ;
		TDialogEntryData *	DialogEntryData ;
    }  DIALOGBOX ;





/**************************************************************************/
/*                                                                        */
/*   Prototypes de fonctions utiles.                                      */
/*                                                                        */
/**************************************************************************/

char *		SpeedToString		( long int  speed ) ;
long int	StringToSpeed		( char *    speed ) ;
char *		ParityToString		( UINT	    speed ) ;
UINT		StringToParity		( char *    speed ) ;
long int	StringToLong		( char *    str ) ;
char *		DelayToString		( int	    delai ) ;
int		StringToDelai		( char *    delai ) ;

void		DelayMin			  	( DWORD Delay ) ;

void		CreateDialogBoxTemplate ( unsigned char *	unixdata,
					  int			datasize,
					  DIALOGBOX &		dlgbox ) ;
HANDLE		CreateDialogBoxHandle	( DIALOGBOX &		dlgbox ) ;


void		FreeDialogBoxTemplate   ( DIALOGBOX &		dlgbox ) ;

HICON		CreateIconFromTemplate	( char far *		defstr,
					  int			size ) ;

void		GetDefaultWindowPosition( int &			x,
					  int &			y,
					  int			w,
					  int                   h ) ;


int		MessageBox  		( HWND			parent,
					  char *		title,
					  UINT			flags,
					  char *		format,
					  ... ) ;


void		InitializeHelp		( void ) ;

void		Beautify		( char *		text ) ;

void		SetWindowVisible	( TWindow *		win,
					  BOOL			visible   =  TRUE ) ;

BOOL		IsFocusMessage		( UINT			message,
					  WPARAM		wparam,
					  LPARAM		lparam ) ;

BOOL		RestoreFocusToChild 	( HWND			parent_window,
					  HWND			avoid_this_one ) ;

void		EnableTextItem		( HWND			control_hwnd,
					  BOOL			state ) ;


# endif		/*  __UTIL_H__  */
