/**************************************************************************/
/*                                                                        */
/*   UWCommon.h  -  Christian Vigh, 94/09/26.                             */
/*                                                                        */
/*   Définitions communes à Unix et Windows.                              */
/*   (ce fichier réside dans les deux environnements à la fois)           */
/*                                                                        */
/**************************************************************************/


# if	! defined ( __UWCOMMON_H__ )
#	  define    __UWCOMMON_H__


# define	MAX_MESSAGE_SIZE		32768U



/**************************************************************************/
/*                                                                        */
/*  Gestion des caractères à échapper.                                    */
/*                                                                        */
/**************************************************************************/

# define	ESCAPED_CHARACTER		( 0xFF - ' ' )
# define	ESCAPED_BASE			' '

# define	isescaped(c)			( ( unsigned char ) c < ' '  &&  EscapeTable [c] )

extern char 	EscapeTable [ ] ;



/**************************************************************************/
/*                                                                        */
/*	 Gestion du caractère 255 interprété par TELNET.                  */
/*                                                                        */
/**************************************************************************/


# define        TELNET_CHARACTER                0xFE
# define        IAC                             0xFF




/**************************************************************************/
/*                                                                        */
/*  Les séquences d'échappement envoyées par un programme Unix (une       */
/*  édition) sont remplacées par les codes suivants.                      */
/*                                                                        */
/**************************************************************************/

# define	LIST_BEGIN			0x01
# define	LIST_END			0x02
# define	LIST_OEM			0x03
# define	LIST_PORTRAIT			0x04
# define	LIST_LANDSCAPE			0x05

# define	LIST_10_CPI			'A'
# define	LIST_12_CPI			'B'
# define	LIST_15_CPI			'C'
# define	LIST_17_CPI			'D'
# define	LIST_6_LPI			'E'
# define	LIST_8_LPI			'F'
# define	LIST_BOLD_ON			'H'
# define	LIST_BOLD_OFF			'I' 
# define	LIST_ITALIC_ON			'J'
# define	LIST_ITALIC_OFF			'K'
# define	LIST_UNDERLINE_ON		'L'
# define	LIST_UNDERLINE_OFF		'M'





/**************************************************************************/
/*                                                                        */
/*  Taille des buffers pour les communications.                           */
/*                                                                        */
/**************************************************************************/

# define	RECEIVE_BUFFER_SIZE_SERIAL   	1024
# define	SEND_BUFFER_SIZE_SERIAL		1024

# define	RECEIVE_BUFFER_SIZE_NETWORK 	8192
# define	SEND_BUFFER_SIZE_NETWORK  	8192



/**************************************************************************/
/*                                                                        */
/*   Caractère de synchronisation.                                        */
/*                                                                        */
/**************************************************************************/

# define	SYNC_CHAR			0x16
# define	SYNC_SIZE			10
# define	SYNC_INTERVAL			25 	/* ms */




/**************************************************************************/
/*                                                                        */
/*  Macros de conversion d'entiers 16 et 32 bits vers tableau de          */
/*  caractères (et vice-versa).                                           */
/*                                                                        */
/**************************************************************************/

typedef		unsigned char		INTEGER16 [2] ;
typedef		unsigned char		INTEGER32 [4] ;


# define	UNSIGNED(x)		( ( unsigned integer16 ) (x) )
# define	LUNSIGNED(x)		( ( unsigned integer32 ) (x) )


# define	I16toCHAR(v,t)					  \
	(t) [1] = ( unsigned char ) ( ( UNSIGNED ( v )      ) & 0xFF ),    \
	(t) [0] = ( unsigned char ) ( ( UNSIGNED ( v ) >> 8 ) & 0xFF )


# define	I32toCHAR(v,t)						\
	(t) [3] = ( unsigned char ) ( ( LUNSIGNED ( v )       ) & 0xFF ),  	\
	(t) [2] = ( unsigned char ) ( ( LUNSIGNED ( v ) >>  8 ) & 0xFF ),   	\
	(t) [1] = ( unsigned char ) ( ( LUNSIGNED ( v ) >> 16 ) & 0xFF ),   	\
	(t) [0] = ( unsigned char ) ( ( LUNSIGNED ( v ) >> 24 ) & 0xFF )


# define	CHARtoI16(v,t)						\
	v   =  ( unsigned integer16 ) ( ( unsigned char ) (t) [1] ) |		\
	       ( unsigned integer16 ) ( ( ( unsigned char ) (t) [0] ) << 8 )


# define	CHARtoI32(v,t)						\
	v   =  ( ( ( unsigned integer32 ) ( unsigned char ) (t) [3] ) )	    	|	\
	       ( ( ( unsigned integer32 ) ( unsigned char ) (t) [2] ) <<  8 )  	| 	\
	       ( ( ( unsigned integer32 ) ( unsigned char ) (t) [1] ) << 16 )  	|	\
	       ( ( ( unsigned integer32 ) ( unsigned char ) (t) [0] ) << 24 )



# define	SUM16(v)					\
			( ( ( (v)       )  &  0xFF ) +		\
			  ( ( (v) >>  8 )  &  0xFF ) )
# define	SUM32(v)					\
			( ( ( (v)       )  &  0xFF ) +		\
			  ( ( (v) >>  8 )  &  0xFF ) +		\
			  ( ( (v) >> 16 )  &  0xFF ) +		\
			  ( ( (v) >> 24 )  &  0xFF ) )





/**************************************************************************/
/*                                                                        */
/*  Structure d'un message :                                              */
/*                                                                        */
/*       Checksum :                                                       */
/*              Checksum du message (somme de tous les octets constituant */
/*              le message).                                              */
/*                                                                        */
/*      ProcessId -                                                       */
/*              Numéro du processus Unix ayant émis le message.           */
/*                                                                        */
/*      MessageTime, MessageClock :                                       */
/*              Heure (à la seconde et à la microseconde) d'émission du   */
/*              message.                                                  */
/*                                                                        */
/*                                                                        */
/*      Function :                                                        */
/*              Numéro de la fonction.                                    */
/*                                                                        */
/*      Data, DataSize :                                                  */
/*              Données du message (avec taille)                          */
/*                                                                        */
/**************************************************************************/

# define	MESSAGE_HEADER          	"\033!"
# define	MESSAGE_HEADER_SIZE		2

# define	MESSAGE_TRAILER			"\033*"
# define	MESSAGE_TRAILER_SIZE		2


typedef struct
   {
		integer16		Checksum ;
		integer16		ProcessId ;
		integer16		Function ;
		integer16		DataSize ;
		char *			Data ;
     }  TUnixMessage  ;



/**************************************************************************/
/*                                                                        */
/*   Fonctions d'assistance à la récupération des valeurs d'un message :  */
/*                                                                        */
/*   DATA :                                                               */
/*      Déclare une variable 'name' pointant sur le champ 'Data' du       */
/*      message, et ayant le type 'type'                                  */
/*                                                                        */
/*   CHECKFLAG :                                                          */
/*      Si IFlag a le bit 'Iname' défini, le bit 'Oname' est positionné   */
/*      dans OFlag.                                                       */
/*                                                                        */
/*   BEGINCHECK :                                                         */
/*      Commence un 'switch' selon la valeur de 'iname'.                  */
/*                                                                        */
/*   CHECKVALUE :                                                         */
/*      Si 'Iname' est égal à 'Ivalue', la valeur 'OValue' est assignée   */
/*      à Oname.                                                          */
/*                                                                        */
/*   ENDCHECK :                                                           */
/*      Termine un 'switch' et assigne la valeur par défaut 'defval' à    */
/*      oname.                                                            */
/*                                                                        */
/**************************************************************************/

# define	CHECKFLAG(iflag,iname,oflag,oname)			    \
			if  ( ((iflag) & (iname))  ==  (iname) ) oflag |= (oname)

# define	BEGINCHECK(var)			switch ( var ) {
# define	CHECKVALUE(ival,oname,oval)			    	    \
			case  ival : oname = oval ; break
# define	ENDCHECK(oname,oval)		default : oname = oval  ; }






/**************************************************************************/
/*                                                                        */
/*   Définition d'une boîte de dialogue.                                  */
/*                                                                        */
/**************************************************************************/

# define	MAX_DIALOG_FIELDS		255


typedef struct
    {
		integer16	 X,  Y,
				CX, CY ;
     }  TDialogDimension ;


typedef struct
    {
		integer16	UnixDataSize,
				RemoteDataSize ;
		void *		UnixData,
		     *		RemoteData ;
      }  TDialogData ;


typedef struct
    {
		integer16		FieldId ;
		TDialogDimension	Text,
					Graphic ;
		char *			Title ;
		char *			ClassName ;
		integer32		Style ;
		TDialogData		Data ;
      }  TDialogEntry ;


typedef struct
    {
		integer16		DialogId ;
		TDialogDimension	Text,
					Graphic ;
		char *			MenuName ;
		char *			ClassName ;
		char *			FontName ;
		integer16		FontSize ;
		char *			Title ;
		integer32		Style ;
		integer16		FieldCount ;
		TDialogEntry *		Fields ;
		TDialogData		Data ;
      }  TDialogDefinition ;





/**************************************************************************/
/*                                                                        */
/*  Données supplémentaires associées aux champs d'une boîte de dialogue. */
/*                                                                        */
/**************************************************************************/

typedef struct 
   {
		char		Format [10],
				Ctl    [10],
				CpCtl  [10] ;
     }  TDialogEntryData ;





/**************************************************************************/
/*                                                                        */
/*   Identificateurs de fonctions et structures de données associées      */
/*   pour le champ Data.                                                  */
/*   Chaque fonction est définie par les éléments suivants :              */
/*                                                                        */
/*   . Une constante identifiant la fonction, de la forme WUF_xxx         */
/*   . Une structure de données, émise par Unix, contenant les paramètres */
/*     de cette fonction. Le nom de cette structure est de la forme       */
/*     UTxxx                                                              */
/*   . Une structure de données, émise par Windows, contenant le résultat */
/*     de la fonction demandée. Le nom de cette structure est de la forme */
/*     WTxxx.                                                             */
/*                                                                        */
/**************************************************************************/


/*=========================================================================*/
/*=                                                                        */
/*=  Le champ codant le numéro de fonction est composé de deux parties :   */
/*=     . Le numéro de la fonction sur les 13 premiers bits.               */
/*=     . Les 3 bits de poids fort restants indiquent la méthode utilisée  */
/*=       pour compresser les données du messages.                         */
/*=                                                                        */
/*=========================================================================*/

# define	COMPRESSION_MASK		0xE000
# define	COMPRESSION_NONE		0x0000


/*=========================================================================*/
/*=   Codes des fonctions.                                                 */
/*=========================================================================*/

# define	UWF_NULL_FUNCTION			0x0000
# define	UWF_INITIALIZE_APPLICATION		0x0001
# define	UWF_END_APPLICATION			0x0002

# define	UWF_OPEN_FILE				0x0003
# define	UWF_CLOSE_FILE				0x0004
# define	UWF_READ_FILE				0x0005
# define	UWF_WRITE_FILE				0x0006
# define	UWF_CONVERTED_READ_FILE			0x0007
# define	UWF_CONVERTED_WRITE_FILE		0x0008
# define	UWF_GET_FILE_SIZE			0x0009

# define	UWF_MESSAGE_BOX				0x000A
# define	UWF_CREATE_GAUGE			0x000B
# define	UWF_SET_GAUGE_VALUE			0x000C
# define	UWF_CLOSE_GAUGE				0x000D

# define	UWF_ECHO				0x000E
# define	UWF_REEMIT				0x000F

# define	UWF_INPUT_BOX				0x0010

# define	UWF_GET_CAPTION				0x0011
# define	UWF_SET_CAPTION				0x0012

# define	UWF_DIALOG_BOX				0x0013
# define	UWF_CREATE_WINDOW			0x0014
# define	UWF_CLOSE_WINDOW			0x0015

# define	UWF_NEXT_EVENT				0x0016

# define	UWF_CHECK_DLG_BUTTON			0x0017
# define	UWF_CHECK_RADIO_BUTTON			0x0018
# define	UWF_GET_DLG_ITEM_TEXT			0x0019
# define	UWF_IS_DLG_BUTTON_CHECKED		0x001A
# define	UWF_SEND_DLG_ITEM_MESSAGE		0x001B
# define	UWF_SET_DLG_ITEM_TEXT			0x001C

# define	UWF_SET_ACTIVE_WINDOW			0x001D
# define	UWF_ENABLE_WINDOW			0x001E
# define	UWF_GET_ACTIVE_WINDOW			0x001F
# define	UWF_ENABLE_CONTROL			0x0020

# define	UWF_SET_FOCUS				0x0021
# define	UWF_GET_FOCUS				0x0022
# define        UWF_SET_CONTROL_FOCUS                   0x0023
# define	UWF_GET_CONTROL_FOCUS			0x0024

# define	UWF_MESSAGE_BEEP			0x0025
# define	UWF_INIT_DIALOG				0x0026

# define	UWF_SET_CURSOR				0x0027
# define	UWF_SET_ICON				0x0028
# define	UWF_SET_CUSTOM_ICON			0x0029

# define	UWF_HELP				0x002A

# define	UWF_GET_TERMINAL_OBJECT_ID		0x002B
# define	UWF_SHOW_WINDOW				0x002C
# define	UWF_QUERY_BAUDRATE			0x002D
# define	UWF_RECEIVE_FAILED			0x002E
# define	UWF_SUPER_MESSAGE			0x002F

# define	UWF_MENU				0x0030

# define 	UWF_EDIT_FILE				0x0031

# define	UWF_TIMER				0x0032
# define	UWF_INFO_LINE				0x0033

# define	UWF_ACCESS_FILE				0x0034
# define	UWF_EXECUTE				0x0035


# define	UWF_FUNCTION_MAX			UWF_EXECUTE


# if	defined ( CHEOPS )
#   define	UWF_CHEOPS_DIALOG			( UWF_FUNCTION_MAX + 1 )
#   define	UWF_CHEOPS_DIALOG_MODIFY		( UWF_FUNCTION_MAX + 2 )
#   define	UWF_SET_CHEOPS_FUNCTION_KEYS		( UWF_FUNCTION_MAX + 3 )
#   define	UWF_SET_CHEOPS_HELP			( UWF_FUNCTION_MAX + 4 )
#   define	UWF_SET_STATUS_BAR_TEXT			( UWF_FUNCTION_MAX + 5 )
#   define	UWF_CHEOPS_MENU				( UWF_FUNCTION_MAX + 6 )
# endif		/* CHEOPS */



/*=========================================================================*/
/*=   Toutes les fonctions renvoient au moins un statut.                   */
/*=========================================================================*/

# define	UWS_OK					0x0000
# define	UWS_CONNECTION_ACCEPTED  		UWS_OK
# define	UWS_CONNECTION_CLOSED 			UWS_OK

# define	UWS_CONNECTION_REFUSED   		0x0001
# define	UWS_ALREADY_CONNECTED    		0x0002
# define	UWS_NOT_CONNECTED	  		0x0003
# define	UWS_ENOENT				0x0004
# define	UWS_EMFILE				0x0005
# define	UWS_EACCES				0x0006
# define	UWS_EBADF				0x0007
# define        UWS_EINVAL				0x0008
# define	UWS_UNKNOWN_OBJECT_ID			0x0009
# define	UWS_NOT_ENOUGH_MEMORY			0x000A
# define	UWS_BAD_MESSAGE_DATA			0x000B
# define	UWS_FAILED				0x000C







typedef struct
   {
		INTEGER16	Status ;
    }  WTStatus ;




/*=========================================================================*/
/*=   Certaines fonctions doivent gérer des messages Windows               */
/*=   ( Msg, wparam, lparam ).                                             */
/*=========================================================================*/

# define	UWC_LPARAM_NOT_POINTER		(-1)


typedef struct
   {
		INTEGER16	Message ;
		INTEGER16	WParam ;
		INTEGER16	LParamSize ;
		char		LParam [1] ;
    }  WINDOWMESSAGE ;


typedef struct
	{
		integer16	ApplicationId,
				ObjectId ;
		integer16	Message ;
		integer16	WParam ;
		integer16	LParamSize ;
		integer32	LParam ;
    }  WindowMessage ;

    
/*=========================================================================*/
/*=   UWF_NULL_MESSAGE :                                                   */
/*=   Message nul (pas de structure Unix ni Windows).                      */
/*=========================================================================*/

typedef  	WTStatus	WTNullFunction ;



/*=========================================================================*/
/*=   UWF_INITIALIZE_APPLICATION :                                         */
/*=   Initialisation de l'application Unix.                                */
/*=========================================================================*/

# define	MAX_APPLICATION_NAME_LENGTH		32


typedef struct
	{
		char		ApplicationName [ MAX_APPLICATION_NAME_LENGTH ] ;
    }  UTInitializeApplication ;


typedef 	WTStatus	WTInitializeApplication ;




/*=========================================================================*/
/*=   UWF_END_APPLICATION :                                                */
/*=   Fin de l'application.                                                */
/*=========================================================================*/

typedef		WTStatus		WTEndApplication ;




/*=========================================================================*/
/*=   UWF_OPEN_FILE                                                        */
/*=   Ouverture d'un fichier DOS.                                          */
/*=========================================================================*/

# define	UWC_OPEN_RDONLY			0x0001	/* 2e paramètre de open() */
# define	UWC_OPEN_WRONLY			0x0002
# define	UWC_OPEN_RDWR			( UWC_OPEN_RDONLY | UWC_OPEN_WRONLY )
# define	UWC_OPEN_NDELAY			0x0008
# define	UWC_OPEN_APPEND			0x0010
# define	UWC_OPEN_CREAT			0x0020
# define	UWC_OPEN_TRUNC			0x0040
# define	UWC_OPEN_EXCL			0x0080
# define	UWC_OPEN_COMPRESSED		0x0100	/* Flag spécial à nous pour décompresser */
						 	/* à l'écriture */

# define	UWC_OPEN_IREAD			0x0001  /* et 3e */
# define	UWC_OPEN_IWRITE			0x0002



typedef struct
   {
		INTEGER16	OpenMode ;
		INTEGER16	CreatFlags ;
		char 		DosFilename [1] ;
     }   UTOpenFile ;



typedef struct
   {
		INTEGER16	Status ;
		INTEGER32	FileId ;
     }   WTOpenFile ;



/*=========================================================================*/
/*=   UWF_CLOSE_FILE :                                                     */
/*=   Fermeture d'un fichier DOS.                                          */
/*=========================================================================*/


typedef struct
    {
		INTEGER32	FileId ;
     }  UTCloseFile ;


typedef  	WTStatus	WTCloseFile ;




/*=========================================================================*/
/*=   UWF_READ_FILE :                                                      */
/*=   Lecture d'un fichier DOS.                                            */
/*=========================================================================*/


/***
   On doit obligatoirement inclure l'offset où la lecture doit être effectuée.
   Pourquoi ? A cause d'un éventuel problème de communication. Si le message
   est traité par Windows, mais qu'il y a un problème lors de l'envoi, ce
   message est susceptible d'être réémis plusieurs fois, faisant à chaque
   fois avancer le pointeur du fichier...
 ***/

typedef struct
   {
		INTEGER32	FileId ;
		INTEGER32	FileOffset ;
		INTEGER16	Count ;
		char		BinaryRead ;
    }   UTReadFile ;



typedef struct
   {
		INTEGER16	Status ;
		INTEGER16	BytesRead ;
		char		Data [1] ;
    }   WTReadFile ;




/*=========================================================================*/
/*=   UWF_WRITE_FILE :                                                     */
/*=   Ecriture d'un fichier DOS.                                           */
/*=========================================================================*/


typedef struct
   {
		INTEGER32	FileId ;
		INTEGER32	FileOffset ;
		INTEGER16	Count ;
		char		BinaryWrite ;
		char		Data [ 1 ] ;
    }  UTWriteFile ;


typedef struct
   {
		INTEGER16	Status ;
		INTEGER16	BytesWritten ;
    }  WTWriteFile ;



/*=========================================================================*/
/*=   UWF_GET_FILE_SIZE :                                                  */
/*=   Obtenir la taille d'un fichier.                                      */
/*=========================================================================*/

typedef struct
   {
		INTEGER32	FileId ;
	 }  UTGetFileSize ;



typedef struct
   {
		INTEGER16	Status ;
		INTEGER32	FileSize ;
    }  WTGetFileSize ;



/*=========================================================================*/
/*=   UWF_ACCESS_FILE :                                                    */
/*=   Indique si un fichier est accessible.                                */
/*=========================================================================*/

typedef struct
   {
		char		AccessMode ;
		char		FileName [1] ;
    }  UTAccessFile ;



typedef WTStatus		WTAccessFile ;
   


/*=========================================================================*/
/*=   UWF_MESSAGE_BOX :                                                    */
/*=   Affiche une boîte de message.                                        */
/*=========================================================================*/

# define	UWC_NO_PARENT			0xFFFF

typedef struct
    {
		INTEGER16	ParentWindow ;
		INTEGER16	BoxOptions ;
		char		MessageAndTitle [1] ;
      }   UTMessageBox ;



typedef struct
   {
		INTEGER16	Status ;
		INTEGER16	Result ;
     }   WTMessageBox ;
     



/*=========================================================================*/
/*=   UWF_CREATE_GAUGE :                                                   */
/*=   Affiche une fenêtre contenant une jauge.                             */
/*=========================================================================*/

typedef struct
   {
		INTEGER16	ParentWindow ;
		INTEGER16	X, Y, W, H ;
		INTEGER16	Margin ;
		INTEGER16	RangeLow,
				RangeHigh ;
		char		FormatAndTitle [1] ;
     }  UTCreateGauge ;


typedef struct
	{
		INTEGER16	Status ;
		INTEGER16	ObjectId ;
     }  WTCreateGauge ;



/*=========================================================================*/
/*=   UWF_SET_GAUGE_VALUE :                                                */
/*=   Met à jour le pourcentage de la jauge.                               */
/*=========================================================================*/

typedef struct
   {
		INTEGER16	ObjectId ;
		INTEGER16	Value ;
    }  UTSetGaugeValue ;


typedef		WTStatus	WTSetGaugeValue ;




/*=========================================================================*/
/*=   UWF_CLOSE_GAUGE :                                                    */
/*=   Ferme la fenêtre contenant la jauge.                                 */
/*=========================================================================*/

typedef struct
   {
		INTEGER16 	ObjectId ;
    }  UTCloseGauge ;


typedef 	WTStatus	WTCloseGauge ;




/*=========================================================================*/
/*=   UWF_INPUT_BOX :                                                      */
/*=   Affiche une boîte de dialogue avec un champ de saisie.               */
/*=========================================================================*/

# define	UWC_NO_VALIDATOR		0
# define	UWC_PICTURE_VALIDATOR		1
# define	UWC_FILTER_VALIDATOR		2



typedef struct
   {
   		INTEGER16	ParentWindow ;
		char		ValidatorType ;
		char		TitlePromptValueAndParam [1] ;
     }  UTInputBox ;


typedef struct
    {
		INTEGER16	Status ;
		INTEGER16	Result ;
		char		Value [1] ;
     }  WTInputBox ;




/*=========================================================================*/
/*=   UWF_GET_CAPTION :                                                    */
/*=   Retourne le titre d'une fenêtre.                                     */
/*=========================================================================*/

typedef struct
   {
		INTEGER16	ObjectId ;
    }  UTGetCaption ;


typedef struct
	{
		INTEGER16	Status ;
		char		Caption [1] ;
    }  WTGetCaption ;



/*=========================================================================*/
/*=   UWF_SET_CAPTION :                                                    */
/*=   Change le titre d'une fenêtre.                                       */
/*=========================================================================*/

typedef struct
   {
		INTEGER16	ObjectId ;
		char		Caption [1] ;
    }  UTSetCaption ;


typedef   WTStatus		WTSetCaption ;



/*=========================================================================*/
/*=   UWF_DIALOG_BOX :                                                     */
/*=   Affiche une boîte de dialog modale ou amodale.                       */
/*=========================================================================*/

typedef struct
   {
		INTEGER16	ParentWindow ;
    }  UTDialogBox ;


typedef struct
   {
		INTEGER16	Status ;
			INTEGER16	ObjectId ;
    }  WTDialogBox ;
    


/*=========================================================================*/
/*=   UWF_CREATE_WINDOW :                                                  */
/*=   Crée une fenêtre.                                                    */
/*=========================================================================*/

typedef struct
   {
		INTEGER32	Style ;
		INTEGER16	X, Y, CX, CY ;
		INTEGER16	ParentWindow ;
		INTEGER16	ShowMode ;
		char		Caption [1] ;
	 }  UTCreateWindow ;


typedef struct
   {
		INTEGER16	Status ;
		INTEGER16	ObjectId ;
    }  WTCreateWindow ;
    

/*=========================================================================*/
/*=   UWF_CLOSE_WINDOW :                                                   */
/*=   Ferme une fenêtre.                                                   */
/*=========================================================================*/

typedef struct
   {
		INTEGER16	ObjectId ;
    }  UTCloseWindow ;


typedef		WTStatus	WTCloseWindow ;



/*=========================================================================*/
/*=   UWF_NEXT_EVENT :                                                     */
/*=   Envoie le prochain &vènement disponible.                             */
/*=========================================================================*/

typedef struct
   {
		INTEGER16	Status ;
		INTEGER16	ApplicationId,
				ObjectId ;
		WINDOWMESSAGE	Message ;
     }  WTNextEvent ;




/*=========================================================================*/
/*=   UWF_CHECK_DLG_BUTTON :                                               */
/*=   Enfonce un bouton.                                                   */
/*=========================================================================*/

# define	UWC_UNCHECKED		0
# define	UWC_CHECKED		1
# define	UWC_GRAYED		2

typedef struct 
   {
		INTEGER16	ObjectId ;
		INTEGER16	ControlId ;
		char		CheckType ;
    }  UTCheckDlgButton ;



typedef  WTStatus 	WTCheckDlgButton ;



/*=========================================================================*/
/*=   UWF_CHECK_RADIO_BUTTON :                                             */
/*=   Enfonce un bouton.                                                   */
/*=========================================================================*/

typedef struct
   {
		INTEGER16	ObjectId ;
		INTEGER16	FirstControl,
				LastControl,
				ControlId ;
     }  UTCheckRadioButton ;


typedef   WTStatus		WTCheckRadioButton ;




/*=========================================================================*/
/*=   UWF_GET_DLG_ITEM_TEXT :                                              */
/*=   Retourne le texte contenu dans un controle.                          */
/*=========================================================================*/

typedef struct
   {
		INTEGER16	ObjectId ;
		INTEGER16	ControlId ;
    }  UTGetDlgItemText ;



typedef struct 
   {
		INTEGER16	Status ;
		char		Text [1] ;
    }  WTGetDlgItemText ;



/*=========================================================================*/
/*=   UWF_IS_DLG_BUTTON_CHECKED :                                          */
/*=   Indique si un bouton est enfoncé ou non.                             */
/*=========================================================================*/

typedef struct
   {
		INTEGER16	ObjectId ;
		INTEGER16	ControlId ;
    }  UTIsDlgButtonChecked ;



typedef struct
   {
		INTEGER16	Status ;
		char		IsButtonChecked ;
    }  WTIsDlgButtonChecked ;




/*=========================================================================*/
/*=   UWF_SEND_DLG_ITEM_MESSAGE :                                          */
/*=   Envoie un message à un controle.                                     */
/*=========================================================================*/

typedef struct
   {
		INTEGER16	ObjectId ;
		INTEGER16	ControlId ;
		WINDOWMESSAGE	Message ;
	 }  UTSendDlgItemMessage ;



typedef struct
   {
		INTEGER16	Status ;
		INTEGER32	ReturnValue ;
    }  WTSendDlgItemMessage ;




/*=========================================================================*/
/*=   UWF_SET_DLG_ITEM_TEXT :                                              */
/*=   Change le texte d'un controle.                                       */
/*=========================================================================*/

typedef struct
   {
		INTEGER16	ObjectId ;
		INTEGER16	ControlId ;
		char		Text [1] ;
    }  UTSetDlgItemText ;


typedef  WTStatus  	WTSetDlgItemText ;



/*=========================================================================*/
/*=   UWF_SET_ACTIVE_WINDOW :                                              */
/*=   Rend une fenêtre active.                                             */
/*=========================================================================*/

typedef struct
   {
		INTEGER16	ObjectId ;
    }  UTSetActiveWindow ;


typedef struct
   {
		INTEGER16	Status ;
		INTEGER16	PreviousActiveWindow ;
    }  WTSetActiveWindow ;



/*=========================================================================*/
/*=   UWF_ENABLE_WINDOW :                                                  */
/*=   Change l'état (actif/inactif) d'une fenêtre.                         */
/*=========================================================================*/

typedef struct
   {
		INTEGER16	ObjectId ;
		char		Enabled ;
    }  UTEnableWindow ;


typedef struct 
   {
		INTEGER16	Status ;
		char		WasDisabled ;
    }  WTEnableWindow ;
    


/*=========================================================================*/
/*=   UWF_ENABLE_CONTROL :                                                 */
/*=   Change l'état (actif/inactif) d'un controle.                         */
/*=========================================================================*/

typedef struct
   {
		INTEGER16	ObjectId ;
		INTEGER16	ControlId ;
		char		Enabled ;
    }  UTEnableControl ;


typedef struct 
   {
		INTEGER16	Status ;
		char		WasDisabled ;
    }  WTEnableControl ;



/*=========================================================================*/
/*=   UWF_GET_ACTIVE_WINDOW :                                              */
/*=   Retourne la fenêtre active (ou -1 si aucune fenêtre Unix ne l'est)   */
/*=========================================================================*/

typedef struct
   {
		INTEGER16	Status ;
		INTEGER16	ObjectId ;
    }  WTGetActiveWindow ;		      



/*=========================================================================*/
/*=   UWF_GET_FOCUS                                                        */
/*=   Retourne la fenêtre focusée(ou -1 si aucune fenêtre Unix ne l'est)   */
/*=========================================================================*/

typedef struct
   {
		INTEGER16	Status ;
		INTEGER16	ObjectId ;
    }  WTGetFocus ;		      


/*=========================================================================*/
/*=   UWF_SET_FOCUS :                                                      */
/*=   Donne le focus à la fenêtre spécifiée.                               */
/*=========================================================================*/

typedef struct
   {
		INTEGER16	ObjectId ;
    }  UTSetFocus ;


typedef struct
   {
		INTEGER16	Status ;
		INTEGER16	PreviousFocusedWindow ;
    }  WTSetFocus ;



/*=========================================================================*/
/*=   UWF_GET_CONTROL_FOCUS :                                              */
/*=   Retourne l'identificateur du controle ayant le focus.                */
/*=========================================================================*/

typedef struct
   {
		INTEGER16	ObjectId ;
    }  UTGetControlFocus ;


typedef struct
   {
		INTEGER16	Status ;
		INTEGER16	ControlId ;
    }  WTGetControlFocus ;



/*=========================================================================*/
/*=   UWF_SET_CONTROL_FOCUS :                                              */
/*=   Donne le focus au controle spécifié.                                 */
/*=========================================================================*/

typedef struct
   {
		INTEGER16	ObjectId ;
		INTEGER16	ControlId ;
	 }  UTSetControlFocus ;


typedef struct
   {
		INTEGER16	Status ;
		INTEGER16	PreviousControlId ;
    }  WTSetControlFocus ;


/*=========================================================================*/
/*=   UWF_MESSAGE_BEEP :                                                   */
/*=   Emet un beep.                                                        */
/*=========================================================================*/

typedef struct
   {
		INTEGER16	AlertType ;
    }  UTMessageBeep ;


typedef  WTStatus		WTMessageBeep ;



/*=========================================================================*/
/*=   UWF_INIT_DIALOG :                                                    */
/*=   Structure d'initialisation des champs d'une boîte de dialogue.       */
/*=========================================================================*/

typedef struct
   {
		INTEGER16	ControlId ;
		WINDOWMESSAGE 	Message ;
    }  UTInitDialogEntry ;


typedef struct
   {
		INTEGER16		ObjectId ;
		char			ItemCount ;
		UTInitDialogEntry 	Items [1] ;
    }  UTInitDialog ;


typedef  WTStatus 		WTInitDialog ;



/*=========================================================================*/
/*=   UWF_SET_CURSOR :                                                     */
/*=   Change le curseur actuel par un des curseurs prédéfinis.             */
/*=========================================================================*/

typedef struct
   {
		INTEGER32	CursorId ;
    }  UTSetCursor ;


typedef  WTStatus  	WTSetCursor ;



/*=========================================================================*/
/*=   UWF_SET_ICON :                                                       */
/*=   Change l'icône actuelle de la fenêtre par une de celles prédéfinies. */
/*=========================================================================*/

typedef struct
   {
		INTEGER16	ObjectId ;
		INTEGER32	IconId ;
    }  UTSetIcon ;


typedef  WTStatus  	WTSetIcon ;



/*=========================================================================*/
/*=   UWF_SET_CUSTOM_ICON :                                                */
/*=   Change l'icône actuelle de la fenêtre par l'icône passée en param.   */
/*=========================================================================*/

typedef struct
   {
		INTEGER16	ObjectId ;
		INTEGER32	IconFileSize ;
		char		IconFile [1] ;
    }  UTSetCustomIcon ;


typedef  WTStatus  	WTSetCustomIcon ;




/*=========================================================================*/
/*=   UWF_HELP :                                                           */
/*=   Appelle l'aide en ligne.                                             */
/*=========================================================================*/

typedef struct
   {
		INTEGER16	ParentWindow ;
		INTEGER16	HelpCommand ;
		char		FilenameAndHelpKey [1] ;
	 }  UTHelp ;


typedef WTStatus		WTHelp ;



/*=========================================================================*/
/*=   UWF_GET_TERMINAL_OBJECT_ID :                                         */
/*=   Retourne l'identificateur de l'application.                          */
/*=========================================================================*/

/* Pas de structure UT */

typedef struct
   {
		INTEGER16	Status ;
		INTEGER16	ObjectId ;
    }  WTGetTerminalObjectId ;



/*=========================================================================*/
/*=   UWF_SHOW_WINDOW :                                                    */
/*=   Montre une fenêtre.                                                  */
/*=========================================================================*/

typedef struct
   {
		INTEGER16	ObjectId ;
		INTEGER16	ShowMode ; 	/* Constantes SW_xxx  */
    }  UTShowWindow ;


typedef WTStatus		WTShowWindow ;



/*=========================================================================*/
/*=   UWF_QUERY_BAUDRATE :                                                 */
/*=   Retourne la vitesse de la transmission.                              */
/*=========================================================================*/

typedef struct			/* Pas de structure UT */
   {
		INTEGER16	Status ;
		INTEGER32	BaudRate ;
    }  WTQueryBaudRate ;




/*=========================================================================*/
/*=   UWF_SUPER_MESSAGE  :                                                 */
/*=   Envoi global d'une série de messages.                                */
/*=========================================================================*/

typedef struct	
   {
		INTEGER16	MessageCount ;
		char		MessageData [1] ;
    }  UTSuperMessage ;


typedef  struct
   {
		INTEGER16	Status ;
		INTEGER16	ProcessedMessages ;
    }  WTSuperMessage ;



/*=========================================================================*/
/*=    UWF_EDIT_FILE							  =*/
/*=    compression-decompression de fichier                               =*/
/*=========================================================================*/

typedef struct
    {
		INTEGER16	nbFen;
		INTEGER32	FileSize ;
		unsigned char	nbBloc;
		unsigned char	fin;
		char		data[1];
    }	UTEditFile;

typedef struct
    {
		unsigned char	code;
		INTEGER16	ObjectId ;
    }	WTEditFile;





/*=========================================================================*/
/*=    UWF_EXECUTE  							  =*/
/*=    Exécute une commande                                               =*/
/*=========================================================================*/

typedef struct 
   {
		char		Wait ;
		char		PathAndArgs [1] ;
    }  UTExecute ;




typedef WTStatus 	WTExecute ;




# if	defined ( CHEOPS )

/*=========================================================================*/
/*=   UWF_CHEOPS_DIALOG_BOX :                                              */
/*=   Crée un écran de saisie Cheops.                                      */
/*=========================================================================*/

# define	UWC_INVISIBLE_CHEOPS_DIALOG		0x00
# define	UWC_REGULAR_CHEOPS_DIALOG		0x01
# define	UWC_SPLITTED_CHEOPS_DIALOG		0x02
# define	UWC_COMPLEX_CHEOPS_DIALOG		0x03


typedef struct
   {
		INTEGER16	offset ;
		INTEGER16	size ;
		char		intitule [1] ;
    }  UTCheopsGroupe ;


typedef struct
   {
		INTEGER16	ParentWindow ;
		INTEGER16	ReplaceWindow ;
		char		DialogType ;
		INTEGER16	NbBox ;
    }  UTCheopsDialog ;


typedef struct
   {
		INTEGER16	Status ;
		INTEGER16	ObjectId ;
    }  WTCheopsDialog ;


typedef struct
   {
		INTEGER16	ParentWindow ;
		INTEGER16	ReplaceWindow ;
		char		DialogType ;
		INTEGER16	NbBox ;
    }  UTCheopsDialogModify ;


typedef WTCheopsDialog	WTCheopsDialogModify ;

/*=========================================================================*/
/*=   UWF_SET_CHEOPS_FUNCTION_KEYS :                                       */
/*=   Définit le label des touches de fonction.                            */
/*=========================================================================*/

typedef struct
   {
		char		KeyNumber ;
		char		KeyLabelAndHelp [1] ;
    }  UTFunctionKeyDefinition ;


typedef struct
	{
		INTEGER16		ObjectId ;
		char			KeyCount ;
		UTFunctionKeyDefinition	Definitions [1] ;
    }  UTSetCheopsFunctionKeys ;


typedef WTStatus	WTSetCheopsFunctionKeys ;

/*=========================================================================*/
/*=   UWF_Menu :  	                                                   */
/*=   Définit les menus popup.		                                   */
/*=========================================================================*/


typedef struct
    {
		INTEGER16	Size,
				Nb_elt,
				Id_win,
				Id_menu ;
		char		text [1] ;
     }  UTMenu ;


typedef  	WTStatus	WTMenu ;

/*=========================================================================*/
/*=   UWF_Timer :  	                                                   */
/*=   Définit les menus popup.		                                   */
/*=========================================================================*/


typedef struct
    {
		INTEGER16	Id_win,
				temporisation ;
     }  UTTimer ;


typedef  	WTStatus	WTTimer ;


/*=========================================================================*/
/*=   UWF_Info_Line :  	                                                   */
/*=   Ensemble d'information sur l'ecran ligne.		                   */
/*=========================================================================*/

# define	UWC_IL_NORMAL		0x01
# define	UWC_IL_INSERT		0x02
# define	UWC_IL_DELETE		0x03
# define	UWC_IL_REWRITE		0x04
# define	UWC_IL_UNKNOWN		0x05

typedef struct
	 {
		INTEGER16	Id_win,
				Nb_ligne,
				Type_operation,
				Y_position ;
	  }  UTInfoLine ;


typedef  	WTStatus	WTInfoLine ;




/*=========================================================================*/
/*=    UWF_SET_CHEOPS_HELP   						  =*/
/*=    Associe une table de contexte d'aide à une fenêtre.                =*/
/*=========================================================================*/

typedef struct
   {
		INTEGER16	ObjectId ;
		INTEGER16	ResourceId ;
     }  UTSetCheopsHelp ;


typedef struct
   {
		INTEGER16	Status ;
		INTEGER16	HelpId ;
    }   WTSetCheopsHelp ;



/*=========================================================================*/
/*=    UWF_SET_STATUS_BAR_TEXT   					  =*/
/*=    Redéfinit le texte d'un gadget de la barre d'état.		  =*/
/*=========================================================================*/


typedef struct
   {
		INTEGER16	ObjectId ;
		char		TextIndex ;
		char		Text [ 1 ] ;
    }  UTSetStatusBarText ;


typedef  WTStatus		WTSetStatusBarText ;





/*=========================================================================*/
/*=    UWF_CHEOPS_MENU           					  =*/
/*=    Gestion du menu Cheops                                             =*/
/*=========================================================================*/

typedef struct
   {
		INTEGER16	MenuFunction ;
		char		MenuParams [1] ;
    }  UTCheopsMenu ;


typedef  struct
   {
		INTEGER16	Status ;
		INTEGER16	MenuFunction ;
		char		Data [1] ;
    }  WTCheopsMenu ;


# endif		/* CHEOPS */


# endif		/* __UWCOMMON_H__ */
