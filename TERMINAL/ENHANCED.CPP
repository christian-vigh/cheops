/**************************************************************************/
/*                                                                        */
/*  Enhanced.cpp  -  Christian Vigh, 94/10/03.                            */
/*                                                                        */
/*  Contient les fonctions avancées du terminal intelligent.              */
/*                                                                        */
/**************************************************************************/

/**************************************************************************
    MODIFICATIONS :                                                       
									  
    02/07/97 (CV)                                                         
	Capture de la souris dans UWFSetCursor si le curseur spécifié est 
	IDC_WAIT ET si gmenu est démarré. La capture est relachée si      
	gmenu est lancé et qu'il capture les mouvements de la souris.     
	Cette modification permet de ne pas perdre le curseur sablier si  
	la souris est déplacée sur une autre fenetre.                     

    03/07/97 (CV)
	Suite à la modification qui consiste à passer les champs EDIT en
	read-only plutot que les rendre disable (ce qui provoque l'affichage
	du texte en grisé), ajout d'un SetFocus dans UWFInitDialog lorsque
	les champs ont été passés en disable, afin que la procédure de
	boite de dialogue ne donne pas le focus au premier champ.
									  
 **************************************************************************/

# include	"AllTerm.h"
# pragma	hdrstop


# include	"UnixApp.h"
# include	"Event.h"
# include	"Icon.h"
# include	"UnixDlg.h"
# include	"Unix.h"

# include	"TextFile.h"
# include	"ChopCfg.h"
# include	"ChopJet.h" 
# include	"MNCommon.h"
# include	"ChopTree.h"
# include	"ChopMenu.h"
# include	"ChopUp.h"
# include 	"EditFile.h"



# if	defined ( CHEOPS )
# include	"Cheops.h"

# define	CHEOPSCHECK(w,type)						\
		if  ( w  !=  NULL  &&					\
			typeid ( * w )  ==  typeid ( TCheopsDialog ) )	\
		   {							\
			TCheopsDialog *	    CheopsDlg	=		\
				dynamic_cast <TCheopsDialog *> ( w ) ;	\
			if  ( CheopsDlg  ==  NULL )			\
				throw  Bad_cast ( ) ;			\
			w = ( type * ) ( TWindow * ) CheopsDlg -> GetDialogBox ( ) ;	\
		    }

# else

# define	CHEOPSCHECK(w)

# endif

# define	LINECHECK(win,line)					\
		if ( win != NULL && typeid(*win) == typeid(TWinLine))	\
		    {							\
		    line = (TWinLine *) dynamic_cast <TWindow *> ( win );\
		    if (line == NULL)					\
				throw Bad_cast ( );			\
		    }							\
		    else line=NULL;					\
		if (line) win = (TWindow *) line;


extern int	errno ;


/**************************************************************************/
/*                                                                        */
/*  Macros d'assistance à l'écriture des fonctions :                      */
/*                                                                        */
/*  HANDLER :                                                             */
/*      Déclare l'en-tête de la fonction, avec les paramètres             */
/*              TUnixApplication *   Application                          */
/*              TUnixMessage *       Message                              */
/*                                                                        */
/*   REPLY :                                                              */
/*      Crée un TOutputMessage à partir de 'msg', des données 'data' et   */
/*      de leur taille 'datasz', puis envoie ce message.                  */
/*                                                                        */
/*   DATA :                                                               */
/*      Accède au champ 'name' de la structure représentée par            */
/*      Message -> Data, qui est du type 'type'.                          */
/*                                                                        */
/**************************************************************************/

# define	HANDLER(name)		 EnhancedFunction ( TUnixApplicationTable :: name )
# define	DATA(name,type)		 ( ( ( type * )  Message -> Data ) -> name )

# define	REPLY(d,ds)		Answer ( Application, Message, d, ds )



/**************************************************************************/
/*                                                                        */
/*   Table de correspondance Numéro de fonction <-> Fonction.             */
/*   Attention !!! L'entrée pour la fonction no x doit ABSOLUMENT être le */
/*   xe élément de ce tableau !!!!                                        */
/*                                                                        */
/**************************************************************************/

# define	BEGIN_DISPATCH						      \
			TUnixApplicationTable :: TDispatchEntry               \
				TUnixApplicationTable :: DispatchTable [ ] =  \
			    {

# define	DISPATCH(b,id,name)	{ b, id, TUnixApplicationTable :: name }
# define	END_DISPATCH		        			      \
			{ FALSE, 0xFFFF, NULL } } ;  			      \
			int  TUnixApplicationTable :: DispatchTableSize =     \
				( sizeof ( DispatchTable ) / 		      \
				   sizeof ( TDispatchEntry ) ) - 1



BEGIN_DISPATCH
	DISPATCH ( FALSE, UWF_NULL_FUNCTION		, UWFNullMessage	   ),
	DISPATCH ( FALSE, UWF_INITIALIZE_APPLICATION	, UWFInitializeApplication ),
	DISPATCH ( FALSE, UWF_END_APPLICATION		, UWFEndApplication 	   ),

	DISPATCH ( FALSE, UWF_OPEN_FILE			, UWFOpenFile		   ),
	DISPATCH ( FALSE, UWF_CLOSE_FILE		, UWFCloseFile		   ),
	DISPATCH ( FALSE, UWF_READ_FILE			, UWFReadFile		   ),
	DISPATCH ( FALSE, UWF_WRITE_FILE		, UWFWriteFile		   ),
	DISPATCH ( FALSE, UWF_CONVERTED_READ_FILE	, UWFReadFile		   ),
	DISPATCH ( FALSE, UWF_CONVERTED_WRITE_FILE	, UWFWriteFile 		   ),
	DISPATCH ( FALSE, UWF_GET_FILE_SIZE		, UWFGetFileSize  	   ),

	DISPATCH ( TRUE , UWF_MESSAGE_BOX		, UWFMessageBox		   ),
	DISPATCH ( FALSE, UWF_CREATE_GAUGE		, UWFCreateGauge	   ),
	DISPATCH ( FALSE, UWF_SET_GAUGE_VALUE		, UWFSetGaugeValue	   ),
	DISPATCH ( FALSE, UWF_CLOSE_GAUGE		, UWFCloseGauge	 	   ),

	DISPATCH ( FALSE, UWF_ECHO			, UWFEcho		   ),
	DISPATCH ( FALSE, UWF_REEMIT			, UWFReemit		   ),

	DISPATCH ( TRUE , UWF_INPUT_BOX			, UWFInputBox		   ),

	DISPATCH ( FALSE, UWF_GET_CAPTION		, UWFGetCaption		   ),
	DISPATCH ( FALSE, UWF_SET_CAPTION		, UWFSetCaption		   ),

	DISPATCH ( FALSE, UWF_DIALOG_BOX		, UWFDialogBox 		   ),
	DISPATCH ( FALSE, UWF_CREATE_WINDOW		, UWFCreateWindow	   ),
	DISPATCH ( FALSE, UWF_CLOSE_WINDOW		, UWFCloseWindow	   ),

	DISPATCH ( FALSE, UWF_NEXT_EVENT		, UWFNextEvent 		   ),

	DISPATCH ( FALSE, UWF_CHECK_DLG_BUTTON		, UWFCheckDlgButton	   ),
	DISPATCH ( FALSE, UWF_CHECK_RADIO_BUTTON	, UWFCheckRadioButton	   ),
	DISPATCH ( FALSE, UWF_GET_DLG_ITEM_TEXT		, UWFGetDlgItemText	   ),
	DISPATCH ( FALSE, UWF_IS_DLG_BUTTON_CHECKED	, UWFIsDlgButtonChecked	   ),
	DISPATCH ( FALSE, UWF_SEND_DLG_ITEM_MESSAGE	, UWFSendDlgItemMessage    ),
	DISPATCH ( FALSE, UWF_SET_DLG_ITEM_TEXT		, UWFSetDlgItemText	   ),

	DISPATCH ( FALSE, UWF_SET_ACTIVE_WINDOW		, UWFSetActiveWindow	   ),
	DISPATCH ( FALSE, UWF_ENABLE_WINDOW		, UWFEnableWindow 	   ),
	DISPATCH ( FALSE, UWF_GET_ACTIVE_WINDOW		, UWFGetActiveWindow 	   ),
	DISPATCH ( FALSE, UWF_ENABLE_CONTROL		, UWFEnableControl	   ),

	DISPATCH ( FALSE, UWF_SET_FOCUS			, UWFSetFocus		   ),
	DISPATCH ( FALSE, UWF_GET_FOCUS			, UWFGetFocus 		   ),
	DISPATCH ( FALSE, UWF_SET_CONTROL_FOCUS		, UWFSetControlFocus	   ),
	DISPATCH ( FALSE, UWF_GET_CONTROL_FOCUS		, UWFGetControlFocus 	   ),

	DISPATCH ( FALSE, UWF_MESSAGE_BEEP		, UWFMessageBeep	   ),
	DISPATCH ( FALSE, UWF_INIT_DIALOG		, UWFInitDialog		   ),

	DISPATCH ( FALSE, UWF_SET_CURSOR		, UWFSetCursor 		   ),
	DISPATCH ( FALSE, UWF_SET_ICON			, UWFSetIcon		   ),
	DISPATCH ( FALSE, UWF_SET_CUSTOM_ICON		, UWFSetCustomIcon	   ),
	DISPATCH ( FALSE, UWF_HELP			, UWFHelp 		   ),

	DISPATCH ( FALSE, UWF_GET_TERMINAL_OBJECT_ID	, UWFGetTerminalObjectId   ),
	DISPATCH ( FALSE, UWF_SHOW_WINDOW		, UWFShowWindow		   ),
	DISPATCH ( FALSE, UWF_QUERY_BAUDRATE		, UWFQueryBaudRate	   ),

	DISPATCH ( FALSE, UWF_RECEIVE_FAILED		, UWFReceiveFailed 	   ),
	DISPATCH ( FALSE, UWF_SUPER_MESSAGE		, UWFSuperMessage	   ),
	DISPATCH ( FALSE, UWF_MENU			, UWFMenu		   ),

	DISPATCH ( TRUE , UWF_EDIT_FILE			, UWFEditFile		   ),

	DISPATCH ( FALSE, UWF_TIMER			, UWFTimer		   ),
	DISPATCH ( FALSE, UWF_INFO_LINE			, UWFInfoLine		   ),

	DISPATCH ( FALSE, UWF_ACCESS_FILE		, UWFAccessFile		   ),
	DISPATCH ( FALSE, UWF_EXECUTE			, UWFExecute		   ),


# if	defined ( CHEOPS )
	DISPATCH ( FALSE, UWF_CHEOPS_DIALOG		, UWFCheopsDialog	   ),
	DISPATCH ( FALSE, UWF_CHEOPS_DIALOG_MODIFY	, UWFCheopsDialogModify	   ),
	DISPATCH ( FALSE, UWF_SET_CHEOPS_FUNCTION_KEYS  , UWFSetCheopsFunctionKeys ),
	DISPATCH ( FALSE, UWF_SET_CHEOPS_HELP		, UWFSetCheopsHelp	   ),
	DISPATCH ( FALSE, UWF_SET_STATUS_BAR_TEXT	, UWFSetStatusBarText	   ),
	DISPATCH ( FALSE, UWF_CHEOPS_MENU		, UWFCheopsMenu		   ),
# endif  /* CHEOPS */


END_DISPATCH ;




/**************************************************************************/
/*                                                                        */
/*  HandleTranslation -                                                   */
/*      Si un message est envoyé à un contrôle, le convertit dans le bon  */
/*      jeu de caractères.                                                */
/*                                                                        */
/**************************************************************************/

void	HandleTranslation  ( uint  message, char *  param )
   {
	switch  ( message )
	   {    case	CB_ADDSTRING :
		case	CB_SELECTSTRING :
		case	CB_INSERTSTRING :
		case	CB_FINDSTRING :
		case	WM_SETTEXT :
			Device -> HostToLocalCharSet ( param ) ;
			break ;
	     }
     }






/**************************************************************************/
/*                                                                        */
/*   TunixApplicationTable :: Reply :                                     */
/*      Envoie la réponse à un message.                                   */
/*                                                                        */
/**************************************************************************/

void	TUnixApplicationTable :: Answer  ( TUnixApplication *	Application,
					   TUnixMessage *	Message,
					   void *  		data,
					   integer16  		data_size )
   {
// Ne rien faire si nous sommes en train de traiter des messages groupés
	if  ( InSuperMessage )
		return ;

// Construire et envoyer la réponse
	LastMessage. Connection = ( Application ) ? Application -> GetOrigin ( ) :
							:: Connection ;

	TOutputMessage 		__Reply  ( Message -> Function,
					   ( char * ) data,
					   data_size,
					   Message -> ProcessId,
					   MessageOrigin ) ;

	__Reply. Send ( ) ;


// Sauvegarde du message
	if  ( LastMessage. Data  !=  NULL )
		delete []  LastMessage. Data ;
	LastMessage. Data = NULL ;
	LastMessage. Message   = * Message ;
	LastMessage. DataSize  =  data_size ;

	if  ( data  !=  NULL  &&  data_size )
	   {
		LastMessage. Data = new char [ ( int ) data_size ] ;
		memcpy ( LastMessage. Data, data, ( int ) data_size ) ;
	    }
     }




/**************************************************************************/
/*                                                                        */
/*  TApplicationTable :: UWFInitializeApplication                         */
/*      Déclare une nouvelle application dans la liste.                   */
/*                                                                        */
/**************************************************************************/

# pragma	argsused

HANDLER ( UWFInitializeApplication )
   {
	WTInitializeApplication		output ;
	integer16			status ;


	if  ( Application  ==  NULL )	// Nouvelle application
	   {
		if  ( ( Application  =  Add ( Message ) )  ==  NULL )
			status = UWS_CONNECTION_REFUSED ;
		else
		   {
			status = UWS_CONNECTION_ACCEPTED ;
			ApplicationCount ++ ;
		    }
	    }
	else				// déjà connecté...
		status = UWS_ALREADY_CONNECTED ;

	I16toCHAR ( status, output. Status ) ;
	REPLY ( & output, sizeof ( output ) ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TUnixApplicationTable :: UWFEndApplication                            */
/*      Termine une application.                                          */
/*                                                                        */
/**************************************************************************/

# pragma	argsused

HANDLER ( UWFEndApplication )
   {
	WTEndApplication        output ;


	if  ( Application  !=  NULL )
	   {
		I16toCHAR ( UWS_CONNECTION_CLOSED, output. Status ) ;
		REPLY ( & output, sizeof ( output ) ) ;
		Remove ( Application ) ;
	    }
	else
	   {
		I16toCHAR ( UWS_NOT_CONNECTED, output. Status ) ;
		REPLY ( & output, sizeof ( output ) ) ;
	    }
   }




/**************************************************************************/
/*                                                                        */
/*  TUnixApplicationTable :: UWFNullMessage                               */
/*      Ne fait que renvoyer un message de réponse vide.                  */
/*                                                                        */
/**************************************************************************/

# pragma	argsused

HANDLER ( UWFNullMessage )
   {
	WTNullFunction		output ;


	if  ( Application  ==  NULL )
		I16toCHAR ( UWS_NOT_CONNECTED, output. Status ) ;
	else
		I16toCHAR ( UWS_OK, output. Status ) ;

	REPLY  ( & output, sizeof ( output ) ) ;
     }





/**************************************************************************/
/*                                                                        */
/*   TUnixApplicationTable :: UWFOpenFile                                 */
/*      Ouvre un fichier et retourne son identificateur.                  */
/*      L'identificateur n'est pas préservé, car c'est au programme Unix  */
/*      de savoir ce qu'il doit faire, non mais des fois.                 */
/*                                                                        */
/**************************************************************************/

# pragma	argsused

HANDLER ( UWFOpenFile )
   {
	WTOpenFile		Status ;
	int			OFlags		=  0,
				OMode 		=  0,
				IFlags,
				IMode,
				status ;


	CHARtoI16 ( IFlags, DATA ( OpenMode  , UTOpenFile ) ) ;
	CHARtoI16 ( IMode , DATA ( CreatFlags, UTOpenFile ) ) ;


// Conversion du mode d'ouverture : c'est un peu plus long, mais ça évite les
// différences entre constantes Unix et Windows...
	CHECKFLAG ( IFlags, UWC_OPEN_RDWR  , OFlags, O_RDWR   ) ;

	if  ( ! OFlags )
	   {
		CHECKFLAG ( IFlags, UWC_OPEN_RDONLY, OFlags, O_RDONLY ) ;
		CHECKFLAG ( IFlags, UWC_OPEN_WRONLY, OFlags, O_WRONLY ) ;
	    }
	    
	CHECKFLAG ( IFlags, UWC_OPEN_APPEND, OFlags, O_APPEND ) ;
	CHECKFLAG ( IFlags, UWC_OPEN_CREAT , OFlags, O_CREAT  ) ;
	CHECKFLAG ( IFlags, UWC_OPEN_TRUNC , OFlags, O_TRUNC  ) ;
	CHECKFLAG ( IFlags, UWC_OPEN_EXCL  , OFlags, O_EXCL   ) ;

// Conversion du mode de création
	CHECKFLAG ( IMode , UWC_OPEN_IREAD , OMode , S_IREAD  ) ;
	CHECKFLAG ( IMode , UWC_OPEN_IWRITE, OMode , S_IWRITE ) ;
	OFlags  |=  O_BINARY ;

// Ouverture...

	status = open ( DATA ( DosFilename, UTOpenFile ), OFlags, OMode ) ;

// Conversion (contraire) du résultat...
	if  ( status  ==  -1 )
	   {
		BEGINCHECK ( errno )
			CHECKVALUE ( ENOENT, status, UWS_ENOENT ) ;
			CHECKVALUE ( EMFILE, status, UWS_EMFILE ) ;
			CHECKVALUE ( EACCES, status, UWS_EACCES ) ;
		ENDCHECK ( status, UWS_EACCES ) ;
	     }
	else
	    {
		integer32	FileId ;


	// Transfert compressé : allouer une entrée dans la table
		if  ( IFlags  &  UWC_OPEN_COMPRESSED ) 
		   {
			register int	i, found ;


			for  ( i = 0, found = -1 ; i < MAX_COMPRESSED_TRANSFERS ; i ++ )
			   {
				if  ( TransferTable [i]. FileId  ==  -1 )
				   {
					found = i ;
					break ;
				    }
			     }


			if  ( found < 0 )
			   {
				close ( status ) ;
				status = UWS_NOT_ENOUGH_MEMORY ;
			    }
			else
			   {
				TransferTable [found]. FileId = status ;
				TransferTable [found]. Uncompressor = new TFileUncompressor ( status ) ;
				FileId = ( long int ) found | 0x80000000L ;
				status = UWS_OK ;
			    }
		     }
		else
		   {
			FileId = status ;		
			status = UWS_OK ;
		    }
			
		I32toCHAR ( FileId, Status. FileId ) ;
	     }


// ... et renvoi (beuuuarrrk!!!)
	I16toCHAR ( status, Status. Status ) ;
	REPLY ( & Status, sizeof ( Status ) ) ;
     }




/**************************************************************************/
/*                                                                        */
/*   TUnixApplicationTable :: UWFCloseFile                                */
/*      Ferme le fichier spécifié.                                        */
/*                                                                        */
/**************************************************************************/

# pragma	argsused

HANDLER ( UWFCloseFile )
   {
	register integer32	fd ;
	WTCloseFile		Status ;
	register int		status ;


	CHARtoI32 ( fd, DATA ( FileId, UTCloseFile ) ) ;

// Fermeture...
	if  ( HIWORD ( fd ) )
	   {
		register int	idx = ( int ) ( fd & 0xFFFF ) ;


		if  ( idx  >=  0  &&  idx  <  MAX_COMPRESSED_TRANSFERS )
		   {
			status = close ( TransferTable [idx]. FileId ) ;
			TransferTable [idx]. FileId = -1 ;

			if  ( TransferTable [idx]. Uncompressor )
			   {
				delete TransferTable [idx]. Uncompressor ;
				TransferTable [idx]. Uncompressor = 0 ;
			    }
		     }
		else
			status = EBADF ;
	    }
	else
		status = close ( ( int ) fd ) ;

// Conversion du résultat
	if  ( status )
	   {
		BEGINCHECK ( errno )
			CHECKVALUE ( EBADF, status, UWS_EBADF ) ;
		ENDCHECK ( status, UWS_EBADF ) ;
	    }
	else
		status = status ;

// Réponse
	I16toCHAR ( status, Status. Status ) ;
	REPLY ( & Status, sizeof ( Status ) ) ;
    }





/**************************************************************************/
/*                                                                        */
/*   TUnixApplication :: UWFReadFile                                      */
/*      Lit des octets dans un fichier.                                   */
/*                                                                        */
/**************************************************************************/

# include <stdio.h>
# pragma	argsused

HANDLER ( UWFReadFile )
   {
	WTReadFile *		reply ;
	register integer32	fd ;
	register int		status ;
	integer16		count,
				bytes_read ;
	integer32		offset ;
	BOOL			binary ;



	CHARtoI32 ( fd    , DATA ( FileId    , UTReadFile ) ) ;
	CHARtoI16 ( count , DATA ( Count     , UTReadFile ) ) ;
	CHARtoI32 ( offset, DATA ( FileOffset, UTReadFile ) ) ;
	binary = ( Message -> Function  ==  UWF_READ_FILE ) ;


// Fichier ouvert avec l'attribut décompression : ne rien faire et renvoyer un
// mauvais statut
	if  ( HIWORD ( fd ) )
	   {
		WTReadFile	reply ;

		I16toCHAR ( UWS_EINVAL, reply. Status ) ;
		I16toCHAR ( 0, reply. BytesRead ) ;
		REPLY ( & reply, sizeof ( WTReadFile ) - 1 ) ;
		return ;
	    }


// Se mettre au bon endroit dans le fichier
	if  ( offset  !=  -1 )
		lseek ( ( int ) fd, offset, 0L ) ;


// Allouer la structure (Taille - 1 car la structure comprend un champ
// char Data[1])
	if  ( count > 0 ) 
	   {
		reply = ( WTReadFile * )
			new char [ sizeof ( WTReadFile ) + count ] ;
	// Lire
		bytes_read = read ( ( int ) fd, reply -> Data, count ) ;

	// Erreur de lecture...
		if  ( bytes_read  ==  -1 )
		   {
			BEGINCHECK ( errno )
				CHECKVALUE ( EBADF, status, UWS_EBADF ) ;
			ENDCHECK ( status, UWS_EACCES ) ;
			bytes_read = 0 ;
		     }
		else
		   {
			if  ( ! binary )
				Device -> LocalToHostCharSet (
					reply -> Data, bytes_read ) ;

			status = UWS_OK ;
		     }

		I16toCHAR ( status    , reply -> Status ) ;
		I16toCHAR ( bytes_read, reply -> BytesRead ) ;

		REPLY ( reply, sizeof ( WTReadFile ) - 1 + bytes_read ) ;
		delete [] ( char * ) reply ;
	     }
	else
	    {
		WTReadFile		reply ;


		I16toCHAR ( UWS_EINVAL, reply. Status ) ;
		I16toCHAR ( 0, reply. BytesRead ) ;
		REPLY ( & reply, sizeof ( WTReadFile ) - 1 ) ;
	     }
      }




/**************************************************************************/
/*                                                                        */
/*   TUnixApplication :: UWFWriteFile                                     */
/*      Ecrit dans un fichier.                                            */
/*                                                                        */
/**************************************************************************/

# pragma	argsused

HANDLER ( UWFWriteFile )
   {
	WTWriteFile		reply ;
	register integer32	fd ;
	register int  		status ;
	integer16		count,
				bytes_written ;
	integer32		offset ;
	BOOL			binary ;
	register char *		p ;


	CHARtoI32 ( fd    , DATA ( FileId    , UTWriteFile ) ) ;
	CHARtoI16 ( count , DATA ( Count     , UTWriteFile ) ) ;
	CHARtoI32 ( offset, DATA ( FileOffset, UTWriteFile ) ) ;
	binary = ( Message -> Function  ==  UWF_WRITE_FILE ) ;

// Fichier à décompresser : on décompresse
	if  ( HIWORD ( fd ) )
	   {
		register int	idx  =  ( int ) ( fd & 0xFFFF ) ;


		if  ( idx >= 0  &&  idx  < MAX_COMPRESSED_TRANSFERS  &&
			TransferTable [idx]. FileId  !=  -1 )
		   {
			register TFileUncompressor *  u = TransferTable [idx]. Uncompressor ;

			u -> Begin ( binary ) ;		
			u -> Uncompress ( DATA ( Data, UTWriteFile ), count ) ;
			u -> End ( bytes_written ) ;
		     }
		else
			bytes_written = -1 ;

		goto  SendReply ;
	     }


// Se mettre au bon endroit dans le fichier 
	if  ( offset  !=  -1 )
		lseek ( ( int ) fd, offset, 0L ) ;


// Ecrire les données...
	if  ( ! binary )
		Device -> HostToLocalCharSet (
			DATA ( Data, UTWriteFile ), count ) ;


	if  ( binary )
		bytes_written = write ( ( int ) fd, DATA ( Data, UTWriteFile ), count ) ;
	else
	   {
		bytes_written = 0 ;
		p = DATA ( Data, UTWriteFile ) ;

		while  ( count -- )
		   {
			if  ( * p  ==  '\n' )
				write ( ( int ) fd, "\r\n", 2 ) ;
			else
				write ( ( int ) fd, p, 1 ) ;

			bytes_written ++ ;
			p ++ ;
		     }
	     }

		

SendReply :
	if  ( bytes_written  ==  -1 )
	   {
		BEGINCHECK ( errno )
			CHECKVALUE ( EBADF, status, UWS_EBADF ) ;
		ENDCHECK ( status, UWS_EACCES ) ;

		bytes_written = 0 ;
	     }
	else
	   {
		status = UWS_OK ;
	     }

	I16toCHAR ( status       , reply. Status ) ;
	I16toCHAR ( bytes_written, reply. BytesWritten ) ;
	REPLY ( & reply, sizeof ( reply ) ) ;
   }





/**************************************************************************/
/*                                                                        */
/*   TUnixApplication :: UWFGetFileSize                                   */
/*      Retourne la taille du fichier                                     */
/*                                                                        */
/**************************************************************************/

# pragma	argsused

HANDLER ( UWFGetFileSize )
   {
	register integer32	fd ;
	register long int	size ;
	register int		status ;
	WTGetFileSize		reply ;


	CHARtoI32 ( fd, DATA ( FileId, UTGetFileSize ) ) ;


// C'est parti...
	size = filelength ( ( int ) fd ) ;

// Erreur ?
	if  ( size  ==  -1L )
	   {
		BEGINCHECK ( errno )
			CHECKVALUE ( EBADF, status, UWS_EBADF ) ;
		ENDCHECK ( status, UWS_EBADF ) ;
	     }
	else
		status = UWS_OK ;

	I16toCHAR ( status, reply. Status ) ;
	I32toCHAR ( size  , reply. FileSize ) ;
	REPLY ( & reply, sizeof ( reply ) ) ;
    }




/**************************************************************************/
/*                                                                        */
/*   TUnixApplicationTable :: UWFAccessFile                               */
/*      Détermine si un fichier est accessible. Le champ AccessMode       */
/*      possède la même valeur que l'argument de la fonction access.      */
/*                                                                        */
/**************************************************************************/

# pragma	argsused

HANDLER ( UWFAccessFile )
   {
	WTAccessFile		Reply ;
	int			IFlags ;
	register char *		Filename ;
	register int		Status ;


	IFlags   = DATA ( AccessMode, UTAccessFile ) ;
	Filename = DATA ( FileName  , UTAccessFile ) ;
	Status = access ( Filename, IFlags ) ;


// Conversion du résultat...
	if  ( Status  ==  -1 )
	   {
		BEGINCHECK ( errno )
			CHECKVALUE ( ENOENT, Status, UWS_ENOENT ) ;
			CHECKVALUE ( EACCES, Status, UWS_EACCES ) ;
		ENDCHECK ( Status, UWS_EACCES ) ;
	     }
	else
		Status = UWS_OK ;

	I16toCHAR ( Status, Reply. Status ) ;
	REPLY ( & Reply, sizeof ( Reply ) ) ;
     }





/**************************************************************************/
/*                                                                        */
/*   TUnixApplicationTable :: UWFMessageBox                               */
/*      Affiche une boîte de messages.                                    */
/*                                                                        */
/**************************************************************************/

# pragma	argsused

HANDLER ( UWFMessageBox )
   {
	integer16		pw,
				opt ;
	TWindow *		ParentWindow ;
	HWND			parent ;
	char *			title,
	     *			message,
	     *			p ;
	register int		status ;
	WTMessageBox		reply ;


// Récupération de la fenêtre parent et des options
	CHARtoI16 ( pw, DATA ( ParentWindow, UTMessageBox ) ) ;
	CHARtoI16 ( opt, DATA ( BoxOptions, UTMessageBox ) ) ;
	p = DATA ( MessageAndTitle, UTMessageBox ) ;

// Si la fenêtre parent est 0, la mère de la boîte sera la fenêtre
// principale de TERMINAL. Si elle est égale à 0xFFFF, la boîte n'aura pas
// de parent
	if  ( pw  ==  0 )
		ParentWindow = TerminalWindow ;
	else if  ( ( unsigned ) pw  ==  UWC_NO_PARENT )
		ParentWindow = NULL ;
	else
		ParentWindow = Application -> GetObject ( pw ) ;

	parent = ( ParentWindow  ==  NULL ) ?
			NULL : ParentWindow -> HWindow ;

// Récupération du message et du titre
	message = p ;
	p      += strlen ( p ) + 1 ;
	title   = p ;

	if  ( ! * title )
		title = NULL ;
	else
		Device -> HostToLocalCharSet ( title ) ;

	Device -> HostToLocalCharSet ( message ) ;

// Affichage de la boiboite; cela sous-entend que l'application Unix a
// effectué un receive bloquant.
// On la fait modale pour l'application...
	TerminalApplication -> PumpWaitingMessages ( ) ;

	status = :: MessageBox ( parent, title, opt | MB_APPLMODAL,
				message ) ;
	EventQueue -> KillFocusMessages ( -1 ) ;

// Construction du résultat
	I16toCHAR ( UWS_OK, reply. Status ) ;
	I16toCHAR ( status, reply. Result ) ;
	REPLY ( & reply, sizeof ( reply ) ) ;
   }




/**************************************************************************/
/*                                                                        */
/*   TUnixApplicationTable :: CreateGauge                                 */
/*      Crée une fenêtre contenant une jauge.                             */
/*                                                                        */
/**************************************************************************/

HANDLER ( UWFCreateGauge )
   {
	TWindow *		ParentWindow ;
	integer16		parent,
				Margin,
				RangeLow,
				RangeHigh,
				X, Y, W, H ;
	char *			Format ;
	char *			Title ;
	int			ObjectId ;
	TUnixGauge *		Gauge ;
	WTCreateGauge		Reply ;



// Récupération des paramètres entiers
	CHARtoI16 ( parent      , DATA ( ParentWindow, UTCreateGauge ) ) ;
	CHARtoI16 ( Margin      , DATA ( Margin      , UTCreateGauge ) ) ;
	CHARtoI16 ( RangeLow    , DATA ( RangeLow    , UTCreateGauge ) ) ;
	CHARtoI16 ( RangeHigh   , DATA ( RangeHigh   , UTCreateGauge ) ) ;
	CHARtoI16 ( X           , DATA ( X           , UTCreateGauge ) ) ;
	CHARtoI16 ( Y		, DATA ( Y           , UTCreateGauge ) ) ;
	CHARtoI16 ( W		, DATA ( W           , UTCreateGauge ) ) ;
	CHARtoI16 ( H		, DATA ( H           , UTCreateGauge ) ) ;


// Interpréter la fenêtre
	if  ( parent  ==  0 )
		ParentWindow = TerminalWindow ;
	else if  ( ( unsigned ) parent  ==  UWC_NO_PARENT )
		ParentWindow = NULL ;
	else
		ParentWindow = Application -> GetObject ( parent ) ;


// Récupération des paramètres chaîne
	Format  = DATA ( FormatAndTitle, UTCreateGauge ) ;
	Title   = Format + strlen ( Format ) + 1 ;

	if  ( ! * Title )
		Title = NULL ;

	Device -> HostToLocalCharSet ( Title ) ;
	Device -> HostToLocalCharSet ( Format ) ;


// Adapter la largeur au titre si elle est nulle
	if  ( ! W )
	   {
		TScreenDC	DC ;


		W = DC. GetTextExtent ( Title, strlen ( Title ) ). cx  +
		    ( 2 * GetSystemMetrics ( SM_CYCAPTION ) ) ;
             }
	


// Création de la fenêtre
	Gauge = new  TUnixGauge ( ParentWindow, Title, Format, X, Y, W, H,
					RangeLow, RangeHigh, Margin ) ;
	ObjectId = Application -> AddObject ( Gauge ) ;

// Construction du résultat
	if  ( ObjectId  ==  -1  ||  Gauge  ==  NULL )
		I16toCHAR ( UWS_NOT_ENOUGH_MEMORY, Reply. Status ) ;
	else
	   {
		I16toCHAR ( UWS_OK, Reply. Status ) ;
		I16toCHAR ( ObjectId, Reply. ObjectId ) ;
		 }

	Gauge -> Create ( ) ;


	REPLY ( & Reply, sizeof ( Reply ) ) ;
    }


/**************************************************************************/
/*                                                                        */
/*   TUnixApplicationTable :: SetGaugeValue                               */
/*      Met à jour la valeur de la jauge.                                 */
/*                                                                        */
/**************************************************************************/

HANDLER ( UWFSetGaugeValue )
   {
	WTSetGaugeValue		Reply ;
	integer16		ObjectId,
				Value ;
	TUnixGauge *		Gauge ;



// Récupération des paramètres
	CHARtoI16 ( ObjectId, DATA ( ObjectId, UTSetGaugeValue ) ) ;
	CHARtoI16 ( Value   , DATA ( Value   , UTSetGaugeValue ) ) ;

	Gauge = ( TUnixGauge * ) Application -> GetObject ( ObjectId ) ;

	if  ( Gauge  !=  NULL )
	   {
		Gauge -> SetValue ( Value ) ;
		I16toCHAR ( UWS_OK, Reply. Status ) ;
	    }
	else
		I16toCHAR ( UWS_UNKNOWN_OBJECT_ID, Reply. Status ) ;

// C'est tout...
	REPLY ( & Reply, sizeof ( Reply ) ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TUnixApplicationTable :: CloseGauge                                   */
/*      Ferme une fenêtre de jauge.                                       */
/*                                                                        */
/**************************************************************************/

HANDLER ( UWFCloseGauge )
	{
	WTCloseGauge 	Reply ;
	integer16	ObjectId ;
	TUnixGauge *	Gauge ;



// Récupération des paramètres
	CHARtoI16 ( ObjectId, DATA ( ObjectId, UTCloseGauge ) ) ;
	Gauge = ( TUnixGauge * ) Application -> GetObject ( ObjectId ) ;


// On ferme
	if  ( Gauge  !=  NULL )
	   {
		Gauge -> Destroy ( ) ;
		Application ->  DeleteObject ( ObjectId ) ;
		I16toCHAR ( UWS_OK, Reply. Status ) ;
	    }
	else
		I16toCHAR ( UWS_UNKNOWN_OBJECT_ID, Reply. Status ) ;


// On répond
	REPLY ( & Reply, sizeof ( Reply ) ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TUnixApplicationTable :: UWFEcho                                      */
/*      Renvoie l'écho du message..                                       */
/*                                                                        */
/**************************************************************************/

# pragma	argsused

HANDLER ( UWFEcho )
   {
	REPLY ( Message -> Data, Message -> DataSize ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TUnixApplicationTable :: UWFReemit                                    */
/*      Renvoie la réponse du précédent message.                          */
/*                                                                        */
/**************************************************************************/

# pragma	argsused

HANDLER ( UWFReemit )
   {
	TOutputMessage 	__Reply  ( LastMessage. Message. Function,
				   LastMessage. Data,
				   LastMessage. DataSize,
				   LastMessage. Message. ProcessId,
				   LastMessage. Connection ) ;

	__Reply. Send ( ) ;
    }





		       
/**************************************************************************/
/*                                                                        */
/*  TUnixApplicationTable :: UWFReceiveFailed                             */
/*      Lorsque Unix ne reçoit pas de réponse à un message, ou bien une   */
/*      réponse incorrecte, il envoie la fonction UWF_RECEIVE_FAILED.     */
/*      Dès réception, on renvoie la réponse du dernier message.          */
/*                                                                        */
/**************************************************************************/

# pragma	argsused

HANDLER ( UWFReceiveFailed )
   {
	UWFReemit ( Application, Message ) ;
    }





		       
/**************************************************************************/
/*                                                                        */
/*  TUnixApplicationTable :: UWFInputBox                                  */
/*      Affiche une boîte de saisie.                                      */
/*                                                                        */
/**************************************************************************/

# pragma	argsused

HANDLER ( UWFInputBox )
   {
	char			buffer [ 512 ] ;
	char			option ;
	TWindow *		parent ;
	char *			initial_value,
	     *			validator_param,
	     * 			title,
	     *			prompt,
	     *			p ;
	integer16		pw ;
	register int		status, size ;
	WTInputBox *		reply ;
	TValidator *		Validator ;


// Récupération de la fenêtre parent et des options
	CHARtoI16 ( pw, DATA ( ParentWindow, UTInputBox ) ) ;
	option = DATA ( ValidatorType, UTInputBox ) ;
	p = DATA ( TitlePromptValueAndParam, UTInputBox ) ;

// Si la fenêtre parent est 0, la mère de la boîte sera la fenêtre
// principale de TERMINAL. Si elle est égale à 0xFFFF, la boîte n'aura pas
// de parent
	if  ( ( unsigned ) pw  ==  UWC_NO_PARENT )
		parent = NULL ;
	else  if  ( ! pw )
		parent = ( TWindow *) TerminalWindow ;
	else
		parent   = Application -> GetObject ( pw ) ;


// Récupération de la valeur initiale et du validateur (if any...)
	title = p ;
	Device -> HostToLocalCharSet ( title ) ;
	p += strlen ( p ) + 1 ;

	prompt = p ;
	Device -> HostToLocalCharSet ( prompt ) ;
	p += strlen ( p ) + 1 ;

	initial_value = p ;
	Device -> HostToLocalCharSet ( initial_value ) ;
	strcpy ( buffer, initial_value ) ;

	if  ( option  !=  UWC_NO_VALIDATOR )
	   {
		p               += strlen ( p ) + 1 ;
		validator_param  = p ;
		Device -> HostToLocalCharSet ( validator_param ) ;
	    }
	else
		validator_param = NULL ;


// Création du validateur
	if  ( validator_param  !=  NULL )
	    {
		switch ( option )
		   {
			case	UWC_FILTER_VALIDATOR :
				Validator = new TFilterValidator ( validator_param ) ;
				break ;

			case	UWC_PICTURE_VALIDATOR :
				Validator = new TPXPictureValidator ( validator_param, TRUE ) ;
				break ;

			case	UWC_NO_VALIDATOR :
			default :
				Validator = NULL ;
				break ;
		    }
	      }
	else
		Validator = NULL ;


// Affichage de la boiboite; cela sous-entend que l'application Unix a
// effectué un receive bloquant
	status = TInputDialog ( parent, title, prompt, buffer,
			sizeof ( buffer ), 0, Validator ). Execute ( ) ;

// Allocation du message de retour
	size  =  sizeof ( WTInputBox ) - 1 + strlen ( buffer ) + 1 ;
	reply = ( WTInputBox * ) new char [ size ] ;


// Construction du résultat
	I16toCHAR ( UWS_OK, reply -> Status ) ;
	I16toCHAR ( status, reply -> Result ) ;
	Device -> LocalToHostCharSet ( buffer ) ;
	strcpy ( reply -> Value, buffer ) ;
	REPLY ( reply, size ) ;


// on fait le ménage  (TInputDialog détruit le TValidator pour nous...)
	delete [] ( char * )  reply ;
   }





/**************************************************************************/
/*                                                                        */
/*   UWFSetCaption, UWFGetCaption :                                       */
/*      Change / retourne le titre d'une fenêtre.                         */
/*                                                                        */
/**************************************************************************/

HANDLER ( UWFGetCaption )
   {
	char			buffer [ 512 ] ;
	WTGetCaption *		reply ;
	WTGetCaption		StaticReply ;
	integer16		object_id ;
	int			size ;
	TWindow *		Window ;



// Récupération du handle de fenêtre
	CHARtoI16 ( object_id, DATA ( ObjectId, UTGetCaption ) ) ;

	if  ( object_id  ==  0 )
		Window = TerminalWindow ;
	else
		Window = Application -> GetObject ( object_id ) ;


	if  ( Window ) 
	   {
	// On récupère le titre
		* buffer = 0 ;
		Window -> GetWindowText ( buffer, sizeof ( buffer ) ) ;
		size = sizeof ( WTGetCaption ) - 1 + strlen ( buffer ) + 1 ;
		reply = ( WTGetCaption * ) new char [ size ] ;

	// Initialisation de la réponse
		I16toCHAR ( UWS_OK, reply -> Status ) ;
		Device -> LocalToHostCharSet ( buffer ) ;
		strcpy ( reply -> Caption, buffer ) ;

	// On balance tout
		REPLY ( reply, size ) ;
		delete []  ( char * ) reply ;
	     }
	else
	    {
		I16toCHAR ( UWS_UNKNOWN_OBJECT_ID, StaticReply. Status ) ;
		StaticReply. Caption [0] = 0 ;


		REPLY ( & StaticReply, sizeof ( StaticReply ) ) ;
	     }
    }




HANDLER ( UWFSetCaption )
   {
	WTSetCaption		reply ;
	integer16		object_id ;
	TWindow *		Window ;
	char * 			p ;


// Récupération des paramètres
	CHARtoI16 ( object_id, DATA ( ObjectId, UTSetCaption ) ) ;
	p = DATA ( Caption, UTSetCaption ) ;


	if  ( object_id  ==  0 )
		Window = TerminalWindow ;
	else
		Window = Application -> GetObject ( object_id ) ;


	if  ( Window )
	   {
	// On change le titre
		Device -> HostToLocalCharSet ( p ) ;
		Window -> SetCaption ( p ) ;

	// On envoie la réponse
		I16toCHAR ( UWS_OK, reply. Status ) ;
	     }
	else
		I16toCHAR ( UWS_UNKNOWN_OBJECT_ID, reply. Status ) ;

	REPLY ( & reply, sizeof ( reply ) ) ;
    }





/**************************************************************************/
/*                                                                        */
/*  UWFDialogBox :                                                        */
/*      Crée une boîte de dialogue.                                       */
/*                                                                        */
/**************************************************************************/

# pragma	argsused

HANDLER ( UWFDialogBox )
   {
	TWindow *		ParentWindow ;
	integer16		parent ;
	int			ObjectId ;
	TUnixDialogBox *	DialogBox ;
	WTDialogBox		Reply ;
	UTCheopsGroupe		*InfoBloc=(UTCheopsGroupe *)(Message -> Data +
						 sizeof(UTCheopsDialog));
	unsigned char		*Data;
	int			DataSize,offset;

	CHARtoI16(offset,InfoBloc->offset);
	Data = (unsigned char *) (Message -> Data + offset);
	CHARtoI16(DataSize, InfoBloc-> size);


// Récupération des paramètres entiers
	CHARtoI16 ( parent, DATA ( ParentWindow, UTDialogBox ) ) ;


// Interpréter la fenêtre
	if  ( parent  ==  0 )
		ParentWindow = TerminalWindow ;
	else if  ( ( unsigned ) parent  ==  UWC_NO_PARENT )
		ParentWindow = NULL ;
	else
		ParentWindow = Application -> GetObject ( parent ) ;


// Création de la fenêtre
	DialogBox = new TUnixDialogBox ( Application, ParentWindow,
				Data,DataSize,Message->ProcessId, FALSE ) ;
	ObjectId  = Application -> AddObject ( DialogBox ) ;
	DialogBox -> ObjectId = ObjectId ;

// Construction du résultat
	if  ( ObjectId  ==  -1  ||  DialogBox  ==  NULL )
		I16toCHAR ( UWS_NOT_ENOUGH_MEMORY, Reply. Status ) ;
	else
	   {
		I16toCHAR ( UWS_OK, Reply. Status ) ;
		I16toCHAR ( ObjectId, Reply. ObjectId ) ;
		DialogBox -> Create ( ) ;
		DialogBox -> Show ( SW_SHOW ) ;
	    }

	REPLY ( & Reply, sizeof ( Reply ) ) ;
    }







/**************************************************************************/
/*                                                                        */
/*  UWFCreateWindow :                                                     */
/*      Crée une fenêtre.                                                 */
/*                                                                        */
/**************************************************************************/

# pragma	argsused

HANDLER ( UWFCreateWindow )
   {
	TWindow *		ParentWindow ;
	integer16		parent ;
	int			ObjectId ;
	TUnixWindow * 		Window ;
	WTCreateWindow		Reply ;
	integer32		window_style ;
	integer16		X, Y, CX, CY,
				show_mode ;


// Récupération des paramètres entiers
	CHARtoI32 ( window_style, DATA ( Style       , UTCreateWindow ) ) ;
	CHARtoI16 ( parent      , DATA ( ParentWindow, UTCreateWindow ) ) ;
	CHARtoI16 ( X           , DATA ( X           , UTCreateWindow ) ) ;
	CHARtoI16 ( Y           , DATA ( Y           , UTCreateWindow ) ) ;
	CHARtoI16 ( CX          , DATA ( CX          , UTCreateWindow ) ) ;
	CHARtoI16 ( CY          , DATA ( CY          , UTCreateWindow ) ) ;
	CHARtoI16 ( show_mode   , DATA ( ShowMode    , UTCreateWindow ) ) ;


// Interpréter la fenêtre
	if  ( parent  ==  0 )
		ParentWindow = TerminalWindow ;
	else if  ( ( unsigned ) parent  ==  UWC_NO_PARENT )
		ParentWindow = NULL ;
	else
		ParentWindow = Application -> GetObject ( parent ) ;


// Création de la fenêtre
	Device -> HostToLocalCharSet ( DATA ( Caption, UTCreateWindow ) ) ;

	Window = new  TUnixWindow  ( Application, Message, ParentWindow,
				     DATA ( Caption, UTCreateWindow ),
				     window_style,
				     X, Y, CX, CY ) ;
	ObjectId = Application -> AddObject ( Window ) ;
	Window -> ObjectId = ObjectId ;


// Construction du résultat
	if  ( ObjectId  ==  -1  ||  Window  ==  NULL )
		I16toCHAR ( UWS_NOT_ENOUGH_MEMORY, Reply. Status ) ;
	else
	   {
		I16toCHAR ( UWS_OK, Reply. Status ) ;
		I16toCHAR ( ObjectId, Reply. ObjectId ) ;
		Window -> Create ( ) ;
		Window -> Show ( show_mode ) ;
	    }

	REPLY ( & Reply, sizeof ( Reply ) ) ;
    }





/**************************************************************************/
/*                                                                        */
/*  TUnixApplicationTable :: CloseWindow :                                */
/*      Ferme une fenêtre ou une boîte de dialogue.                       */
/*                                                                        */
/**************************************************************************/

HANDLER ( UWFCloseWindow )
   {
	WTCloseWindow 	Reply ;
	integer16	ObjectId ;
	TWindow *	Window ;



// Récupération des paramètres
	CHARtoI16 ( ObjectId, DATA ( ObjectId, UTCloseWindow ) ) ;
	Window = Application -> GetObject ( ObjectId ) ;


// On ferme
	if  ( Window  !=  NULL )
	   {
	// Le delete object fera juste un PostMessage ( WM_CLOSE ).
	// cela suffit à détruire la fenêtre ainsi que l'objet
		Application ->  DeleteObject ( ObjectId ) ;
		I16toCHAR ( UWS_OK, Reply. Status ) ;
		 }
	else
		I16toCHAR ( UWS_UNKNOWN_OBJECT_ID, Reply. Status ) ;


// On répond
	REPLY ( & Reply, sizeof ( Reply ) ) ;
    }





/**************************************************************************/
/*                                                                        */
/*  TUnixApplicationTable :: UWFCheckDlgButton :                          */
/*      Change l'état d'un bouton.                                        */
/*                                                                        */
/**************************************************************************/

HANDLER ( UWFCheckDlgButton )
   {
	WTCheckDlgButton 	Reply ;
	integer16		ObjectId,
				ControlId ;
	char			ButtonState ;
	TWindow *		Window ;



// Récupération des paramètres
	CHARtoI16 ( ObjectId , DATA ( ObjectId , UTCheckDlgButton ) ) ;
	CHARtoI16 ( ControlId, DATA ( ControlId, UTCheckDlgButton ) ) ;
	ButtonState = DATA ( CheckType, UTCheckDlgButton ) ;
	Window = ( TWindow * ) Application -> GetObject ( ObjectId ) ;


// On ferme
	if  ( Window  !=  NULL )
	   {
		CHEOPSCHECK ( Window, TWindow ) ;
		Window -> CheckDlgButton ( ControlId, ButtonState ) ;
		I16toCHAR ( UWS_OK, Reply. Status ) ;
	    }
	else
		I16toCHAR ( UWS_UNKNOWN_OBJECT_ID, Reply. Status ) ;


// On répond
	REPLY ( & Reply, sizeof ( Reply ) ) ;
    }







/**************************************************************************/
/*                                                                        */
/*  TUnixApplicationTable :: UWFCheckRadioButton :                        */
/*      Change l'état d'un bouton radio.                                  */
/*                                                                        */
/**************************************************************************/

HANDLER ( UWFCheckRadioButton )
   {
	WTCheckRadioButton 	Reply ;
	integer16		ObjectId,
				ControlId ;
	integer16		FirstControl,
				LastControl ;
	TWindow *		Window ;



// Récupération des paramètres
	CHARtoI16 ( ObjectId    , DATA ( ObjectId    , UTCheckRadioButton ) ) ;
	CHARtoI16 ( ControlId   , DATA ( ControlId   , UTCheckRadioButton ) ) ;
	CHARtoI16 ( FirstControl, DATA ( FirstControl, UTCheckRadioButton ) ) ;
	CHARtoI16 ( LastControl , DATA ( LastControl , UTCheckRadioButton ) ) ;

	Window = ( TWindow * ) Application -> GetObject ( ObjectId ) ;

// On ferme
	if  ( Window  !=  NULL )
	   {
		CHEOPSCHECK ( Window, TWindow ) ;

		Window -> CheckRadioButton ( FirstControl, LastControl,
				ControlId ) ;
		I16toCHAR ( UWS_OK, Reply. Status ) ;
	    }
	else
		I16toCHAR ( UWS_UNKNOWN_OBJECT_ID, Reply. Status ) ;


// On répond
	REPLY ( & Reply, sizeof ( Reply ) ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TUnixApplicationTable :: UWFIsDlgButtonChecked :                      */
/*      Retourne TRUE si le bouton est enfoncé.                           */
/*                                                                        */
/**************************************************************************/

HANDLER ( UWFIsDlgButtonChecked )
   {
	WTIsDlgButtonChecked 	Reply ;
	integer16		ObjectId,
				ControlId ;
	integer16		status ;
	TWindow *		Window ;



// Récupération des paramètres
	CHARtoI16 ( ObjectId , DATA ( ObjectId , UTIsDlgButtonChecked ) ) ;
	CHARtoI16 ( ControlId, DATA ( ControlId, UTIsDlgButtonChecked ) ) ;
	Window = ( TWindow * ) Application -> GetObject ( ObjectId ) ;

// On ferme
	if  ( Window  !=  NULL )
	   {
		CHEOPSCHECK ( Window, TWindow ) ;

		status = Window -> IsDlgButtonChecked ( ControlId ) ;
		I16toCHAR ( UWS_OK, Reply. Status ) ;
		Reply. IsButtonChecked = status ;
	    }
	else
		I16toCHAR ( UWS_UNKNOWN_OBJECT_ID, Reply. Status ) ;


// On répond
	REPLY ( & Reply, sizeof ( Reply ) ) ;
    }





/**************************************************************************/
/*                                                                        */
/*  TUnixApplicationTable :: UWFGetDlgItemText :                          */
/*      Retourne le texte d'un contrôle.                                  */
/*                                                                        */
/**************************************************************************/

HANDLER ( UWFGetDlgItemText )
   {
	char			text [ 1024 ] ;
	WTGetDlgItemText * 	Reply ;
	integer16		ObjectId,
				ControlId ;
	register int		size, status ;
	TWindow *		Window ;
	TCheopsDialog *		Cheopsdlg ;
	TWinLine		*WinL;



// Récupération des paramètres
	CHARtoI16 ( ObjectId , DATA ( ObjectId , UTCheckDlgButton ) ) ;
	CHARtoI16 ( ControlId, DATA ( ControlId, UTCheckDlgButton ) ) ;
	Window = ( TWindow * ) Application -> GetObject ( ObjectId ) ;

	if  ( Window )
		Cheopsdlg = ( TCheopsDialog * ) dynamic_cast <TDecoratedFrame *> ( Window );


// La réponse...
	if  ( Window  !=  NULL )
	   {
		CHEOPSCHECK ( Window, TWindow ) ;
		LINECHECK( Window, WinL);

		if  ( WinL )
			WinL -> GetDlgItemText ( ControlId, text, sizeof ( text ) ) ;
		else if  ( Cheopsdlg  &&  Cheopsdlg -> Support -> GetNbBox ( ) > 1 )
			Cheopsdlg -> Support -> GetDlgItemText ( ControlId, text, sizeof ( text ) ) ;
		else
			Window -> GetDlgItemText ( ControlId, text, sizeof ( text ) ) ;
		Device -> LocalToHostCharSet ( text ) ;
		status = UWS_OK ;
	    }
	else
		status = UWS_UNKNOWN_OBJECT_ID ;


	if  ( status  !=  UWS_OK )
		size = sizeof ( WTGetDlgItemText ) - 1 ;
	else
		size = sizeof ( WTGetDlgItemText ) - 1 + strlen ( text ) + 1 ;

	Reply = ( WTGetDlgItemText * ) new  char [ size ] ;
	I16toCHAR ( status, Reply -> Status ) ;

	if  ( status  ==  UWS_OK )
		strcpy ( Reply -> Text, text ) ;



// On répond
	REPLY ( Reply, size ) ;
	delete [] ( char * )  Reply ;
    }






/**************************************************************************/
/*                                                                        */
/*  TUnixApplicationTable :: UWFSetDlgItemText :	                  */
/*      Initialise le contenu d'un champ d'une boite de dialog.           */
/*                                                                        */
/**************************************************************************/

HANDLER ( UWFSetDlgItemText )
   {
	WTSetDlgItemText	Reply ;
	integer16		ObjectId,
				ControlId ;
	TWindow *		Window ;
	TWinLine		*WinL;



// Récupération des paramètres
	CHARtoI16 ( ObjectId , DATA ( ObjectId , UTSetDlgItemText ) ) ;
	CHARtoI16 ( ControlId, DATA ( ControlId, UTSetDlgItemText ) ) ;
	Window = ( TWindow * ) Application -> GetObject ( ObjectId ) ;

// On ferme
	if  ( Window  !=  NULL )
	   {
		CHEOPSCHECK ( Window, TWindow ) ;
		LINECHECK( Window, WinL);


		Device -> HostToLocalCharSet (
			DATA ( Text, UTSetDlgItemText ) ) ;

		if  ( WinL )
			WinL->InitTab( ControlId, DATA ( Text, UTSetDlgItemText ));
		Window -> SetDlgItemText ( ControlId,
			DATA ( Text, UTSetDlgItemText ) ) ;

		I16toCHAR ( UWS_OK, Reply. Status ) ;
	    }
	else
		I16toCHAR ( UWS_UNKNOWN_OBJECT_ID, Reply. Status ) ;


// On répond
	REPLY ( & Reply, sizeof ( Reply ) ) ;
    }



/**************************************************************************/
/*                                                                        */
/*  TUnixApplicationTable ::  UWFSendDlgItemMessage                       */
/*      Envoie le message spécifié à un contrôle.                         */
/*                                                                        */
/**************************************************************************/

HANDLER ( UWFSendDlgItemMessage )
   {
	WTSendDlgItemMessage 	Reply ;
	integer16		ObjectId,
				ControlId,
				MessageId,
				WParam,
				LParamSize ;
	integer32		LParam ;
	DWORD			Result ;
	TWindow *		Window ;
	TCheopsDialog *		Cheopsdlg;
	TWinLine		*WinL;


// Récupération des paramètres
	CHARtoI16 ( ObjectId , DATA ( ObjectId , UTSendDlgItemMessage ) ) ;
	CHARtoI16 ( ControlId, DATA ( ControlId, UTSendDlgItemMessage ) ) ;

	CHARtoI16 ( MessageId , DATA ( Message. Message   , UTSendDlgItemMessage ) ) ;
	CHARtoI16 ( WParam    , DATA ( Message. WParam    , UTSendDlgItemMessage ) ) ;
	CHARtoI16 ( LParamSize, DATA ( Message. LParamSize, UTSendDlgItemMessage ) ) ;

	if  ( ( int ) LParamSize  ==  UWC_LPARAM_NOT_POINTER )
		CHARtoI32 ( LParam, DATA ( Message. LParam, UTSendDlgItemMessage ) ) ;
	else
	    {
		LParam = ( integer32 ) DATA ( Message. LParam, UTSendDlgItemMessage ) ;
		HandleTranslation ( MessageId, ( char * ) LParam ) ;
	     }

	Window = ( TWindow * ) Application -> GetObject ( ObjectId ) ;


// On envoie le message
	if  ( Window  !=  NULL )
	   {
		Cheopsdlg = ( TCheopsDialog * ) dynamic_cast <TDecoratedFrame *> ( Window );

		CHEOPSCHECK ( Window, TWindow ) ;
		LINECHECK (Window , WinL);

		if  ( WinL )
		   {
			Result = WinL -> SendDlgItemMessage ( ControlId,
				MessageId, WParam, LParam ) ;
			// AZE !!!
			// On ne sait pas à quoi ça sert mais dans certains
			// cas, l'écran de saisie se retrouve bloqué (il
			// attend une réponse qui ne vient pas)
			//EventQueue -> Kill ( Message -> ProcessId ) ;
		    }
		else
		   {
			if  ( Cheopsdlg  ==  NULL )
				throw  Bad_cast ( ) ;

			if  ( Cheopsdlg -> Support -> GetNbBox () > 1 )
				Result = Cheopsdlg -> Support -> SendDlgItemMessage ( ControlId,
					MessageId, WParam, LParam ) ;
			else
				Result = Window -> SendDlgItemMessage ( ControlId,
					MessageId, WParam, LParam ) ;
		    }

		I16toCHAR ( UWS_OK, Reply. Status ) ;
		I32toCHAR ( Result, Reply. ReturnValue ) ;
	    }
	else
		I16toCHAR ( UWS_UNKNOWN_OBJECT_ID, Reply. Status ) ;


// On répond
	REPLY ( & Reply, sizeof ( Reply ) ) ;
    }






/**************************************************************************/
/*                                                                        */
/*  TUnixApplicationTable :: UWFNextEvent :                               */
/*      Renvoie le prochain évènement disponible.                         */
/*                                                                        */
/**************************************************************************/

# pragma	argsused

HANDLER ( UWFNextEvent )
   {
	EventQueue -> Request ( Application -> ApplicationId, 
					Message -> ProcessId ) ;
    }






/**************************************************************************/
/*                                                                        */
/*  TUnixApplicationTable :: UWFSetActiveWindow :                         */
/*      Rend une fenêtre active.                                          */
/*                                                                        */
/**************************************************************************/

HANDLER ( UWFSetActiveWindow )
   {
	WTSetActiveWindow	Reply ;
	integer16		ObjectId,
				Index ;
	HWND			previous ;
	TWindow *		Window ;



// Récupération des paramètres
	CHARtoI16 ( ObjectId , DATA ( ObjectId , UTSetActiveWindow ) ) ;
	Window = ( TWindow * ) Application -> GetObject ( ObjectId ) ;


// On ferme
	if  ( Window  !=  NULL )
	   {
		CHEOPSCHECK ( Window, TWindow ) ;
		previous = Window -> SetActiveWindow ( ) ;
		I16toCHAR ( UWS_OK, Reply. Status ) ;
		Index = Application -> GetObjectIndex ( previous ) ;
		I16toCHAR ( Index, Reply. PreviousActiveWindow ) ;
	    }
	else
		I16toCHAR ( UWS_UNKNOWN_OBJECT_ID, Reply. Status ) ;


// On répond
	REPLY ( & Reply, sizeof ( Reply ) ) ;
    }





/**************************************************************************/
/*                                                                        */
/*  TUnixApplicationTable :: UWFEnableWindow :                            */
/*      Change l'état (actif/inactif) d'une fenêtre.                      */
/*                                                                        */
/**************************************************************************/

HANDLER ( UWFEnableWindow )
	{
	WTEnableWindow		Reply ;
	integer16		ObjectId ;
	BOOL			Enabled ;
	TWindow *		Window ;



// Récupération des paramètres
	CHARtoI16 ( ObjectId , DATA ( ObjectId , UTEnableWindow ) ) ;
	Enabled = DATA ( Enabled, UTEnableWindow ) ;
	Window = ( TWindow * ) Application -> GetObject ( ObjectId ) ;


// On ferme
	if  ( Window  !=  NULL )
	   {
		CHEOPSCHECK ( Window, TWindow ) ;
		Enabled = :: EnableWindow ( Window -> HWindow, Enabled ) ;
		I16toCHAR ( UWS_OK, Reply. Status ) ;
		Reply. WasDisabled = Enabled ;
	    }
	else
		I16toCHAR ( UWS_UNKNOWN_OBJECT_ID, Reply. Status ) ;


// On répond
	REPLY ( & Reply, sizeof ( Reply ) ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TUnixApplicationTable :: UWFGetActiveWindow :                         */
/*      Retourne la fenêtre active, ou -1 si aucune fenêtre n'est active. */
/*                                                                        */
/**************************************************************************/

# pragma	argused

HANDLER ( UWFGetActiveWindow )
   {
	WTGetActiveWindow  	Reply ;
	HWND			Current ;
	integer16		ObjectId ;



// Récupération des paramètres
	Current  =  :: GetActiveWindow ( ) ;
	ObjectId =  Application -> GetObjectIndex ( Current ) ;


// On initialise la réponse
	I16toCHAR ( UWS_OK, Reply. Status ) ;
	I16toCHAR ( ObjectId, Reply. ObjectId ) ;


// On répond
	REPLY ( & Reply, sizeof ( Reply ) ) ;
    }





/**************************************************************************/
/*                                                                        */
/*  TUnixApplicationTable :: UWFEnableControl :                           */
/*      Change l'état (actif/inactif) d'un contrôle.                      */
/*                                                                        */
/**************************************************************************/

HANDLER ( UWFEnableControl )
   {
	WTEnableControl		Reply ;
	integer16		ObjectId,
				ControlId ;
	BOOL			Enabled ;
	TWindow *		Window ;
	TWinLine		*WinL;



// Récupération des paramètres
	CHARtoI16 ( ObjectId , DATA ( ObjectId , UTEnableControl ) ) ;
	CHARtoI16 ( ControlId, DATA ( ControlId, UTEnableControl ) ) ;
	Enabled = DATA ( Enabled, UTEnableControl ) ;
	Window = ( TWindow * ) Application -> GetObject ( ObjectId ) ;

// On ferme
	if  ( Window  !=  NULL )
	   {
		CHEOPSCHECK ( Window, TWindow ) ;
		LINECHECK ( Window, WinL);

		Enabled = :: EnableWindow ( Window -> GetDlgItem ( ControlId ),
					Enabled ) ;
		I16toCHAR ( UWS_OK, Reply. Status ) ;
		Reply. WasDisabled = Enabled ;
	    }
	else
		I16toCHAR ( UWS_UNKNOWN_OBJECT_ID, Reply. Status ) ;


// On répond
	REPLY ( & Reply, sizeof ( Reply ) ) ;
    }





/**************************************************************************/
/*                                                                        */
/*  TUnixApplicationTable :: UWFSetFocus :                                */
/*      Donne le focus à la fenêtre spécifiée.                            */
/*                                                                        */
/**************************************************************************/

HANDLER ( UWFSetFocus )
   {
	WTSetFocus		Reply ;
	integer16		ObjectId ;
	TUnixWindow *		Window ;
	HWND			Previous ;



// Récupération des paramètres
	CHARtoI16 ( ObjectId , DATA ( ObjectId , UTSetFocus ) ) ;
	Window = ( TUnixWindow * ) Application -> GetObject ( ObjectId ) ;


// On ferme
	if  ( Window  !=  NULL )
	   {
		Previous = :: SetFocus ( Window -> HWindow ) ;
		ObjectId = Application -> GetObjectIndex ( Previous ) ;
		I16toCHAR ( UWS_OK  , Reply. Status ) ;
		I16toCHAR ( ObjectId, Reply. PreviousFocusedWindow ) ;
	    }
	else
		I16toCHAR ( UWS_UNKNOWN_OBJECT_ID, Reply. Status ) ;


// On répond
	REPLY ( & Reply, sizeof ( Reply ) ) ;
    }






/**************************************************************************/
/*                                                                        */
/*  TUnixApplicationTable :: UWFSetControlFocus :                         */
/*      Donne le focus au contrôle spécifié.                              */
/*                                                                        */
/**************************************************************************/

HANDLER ( UWFSetControlFocus )
   {
	WTSetControlFocus	Reply ;
	integer16		ObjectId,
				ControlId ;
	TWindow *		Window ;
	HWND			Current, Previous ;
	TCheopsDialog *		Cheopsdlg;
	TWinLine		*WinLine;


// Récupération des paramètres
	CHARtoI16 ( ObjectId , DATA ( ObjectId , UTSetControlFocus ) ) ;
	CHARtoI16 ( ControlId, DATA ( ControlId, UTSetControlFocus ) ) ;
	Window = ( TWindow * ) Application -> GetObject ( ObjectId ) ;

// On ferme
	if  ( Window  !=  NULL )
	   {
		Cheopsdlg = ( TCheopsDialog * ) dynamic_cast <TDecoratedFrame *> ( Window );

		CHEOPSCHECK ( Window, TWindow ) ;
		LINECHECK( Window , WinLine );

		if  ( WinLine )
			Current = WinLine -> DemandeFocus ( ControlId ) ;
		else if  ( Cheopsdlg  &&  Cheopsdlg -> Support -> GetNbBox () > 1 )
			Current =Cheopsdlg -> Support -> GetDlgItem ( ControlId ) ;
		else
			Current = Window -> GetDlgItem ( ControlId ) ;

/***
	!!! Ce bout de code pose un problème pour les écrans qui entrent
	directement en saisie dans un mode (modif, interro, etc.) : aucun
	controle n'a le focus.
	Ceci est du au fait que lorsqu'on rentre directement dans un mode,
	il n'y a pas de wgetch, on entre directement dans windows_input.
	windows_input fait un SetFocus, mais avant d'appeler enterpoint qui
	va recevoir INITDIALOG et les messages de notification xxN_SETFOCUS.
	Cela suffit pour que le focus parte je ne sais où...

		if  ( ! EventQueue -> IsSetFocus () )	// teste la necesite du setfocus.
		   {
			Previous = :: SetFocus ( Current ) ;
			ControlId = GetDlgCtrlID ( Previous ) ;
		    }
		else
			ControlId = 0 ;
 ***/
		if  ( :: GetFocus ( )  !=  Current )
		   {
			Previous = :: SetFocus ( Current ) ;
			ControlId = GetDlgCtrlID ( Previous ) ;
		     }
		else
			ControlId = 0 ;

		I16toCHAR ( UWS_OK  , Reply. Status ) ;
		I16toCHAR ( ControlId, Reply. PreviousControlId ) ;
	    }
	else
		I16toCHAR ( UWS_UNKNOWN_OBJECT_ID, Reply. Status ) ;


// On répond
	REPLY ( & Reply, sizeof ( Reply ) ) ;
    }





/**************************************************************************/
/*                                                                        */
/*  TUnixApplicationTable :: UWFGetFocus :                                */
/*      Retourne le handle de la fenêtre qui a le focus.                  */
/*                                                                        */
/**************************************************************************/

HANDLER ( UWFGetFocus )
   {
	WTGetFocus		Reply ;
	integer16		ObjectId ;
	HWND			FocusWindow ;



// On ferme
	FocusWindow = :: GetFocus ( ) ;
	ObjectId = Application -> GetObjectIndex ( FocusWindow ) ;
	I16toCHAR ( UWS_OK  , Reply. Status ) ;
	I16toCHAR ( ObjectId, Reply. ObjectId ) ;


// On répond
	REPLY ( & Reply, sizeof ( Reply ) ) ;
    }





/**************************************************************************/
/*                                                                        */
/*  TUnixApplicationTable :: UWFGetControlFocus :                         */
/*      Donne l'identificateur du contrôle ayant le focus.                */
/*                                                                        */
/**************************************************************************/

HANDLER ( UWFGetControlFocus )
   {
	WTGetControlFocus	Reply ;
	integer16		ObjectId,
				ControlId ;
	TWindow *		Window ;
	HWND			ControlWindow ;
	TCheopsDialog *		Cheopsdlg;


// Récupération des paramètres
	CHARtoI16 ( ObjectId , DATA ( ObjectId , UTGetControlFocus ) ) ;
	Window = ( TWindow * ) Application -> GetObject ( ObjectId ) ;

// On ferme
	if  ( Window  !=  NULL )
	   {
		Cheopsdlg = ( TCheopsDialog * ) dynamic_cast <TDecoratedFrame *> ( Window );

		CHEOPSCHECK ( Window, TWindow ) ;

	// Si une COMBOBOX a le focus, c'est en fait son contrôle EDIT. Dans
	// ce cas, il faut récupérer le HWND du parent de ce contrôle (la
	// combo box)
		char  ClassName [ 16 ] ;

		ControlWindow = :: GetFocus (  ) ;
		GetClassName ( GetParent ( ControlWindow ), ClassName, sizeof ( ClassName ) ) ;

		if  ( ! stricmp ( ClassName, "COMBOBOX" ) )
			ControlWindow = GetParent ( ControlWindow ) ;

		ControlId = GetDlgCtrlID ( ControlWindow ) ;

		if  ( Cheopsdlg && Cheopsdlg -> Support -> GetNbBox () > 1 )
			Cheopsdlg -> Support -> Onglet -> VirtualClick ( 0, ControlId ) ;

		I16toCHAR ( UWS_OK  , Reply. Status ) ;
		I16toCHAR ( ControlId, Reply. ControlId ) ;
	    }
	else
		I16toCHAR ( UWS_UNKNOWN_OBJECT_ID, Reply. Status ) ;

// On répond
	REPLY ( & Reply, sizeof ( Reply ) ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TUnixApplicationTable :: UWFMessageBeep :                             */
/*      Emet un beep.                                                     */
/*                                                                        */
/**************************************************************************/

# pragma	argsused

HANDLER ( UWFMessageBeep )
   {
	WTMessageBeep    	Reply ;
	integer16		AlertType ;


// Récupération des paramètres
	CHARtoI16 ( AlertType, DATA ( AlertType, UTMessageBeep ) ) ;

// Go
	:: MessageBeep ( AlertType ) ;


// On répond
	I16toCHAR ( UWS_OK, Reply. Status ) ;
	REPLY ( & Reply, sizeof ( Reply ) ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TUnixApplicationTable :: UWFInitDialog                                */
/*      Structure d'initialisation d'une boîte de dialogue.               */
/*                                                                        */
/**************************************************************************/


# pragma	argsused

HANDLER ( UWFInitDialog )
   {
	WTInitDialog    	Reply ;
	UTInitDialogEntry *	DialogEntry ;
	TWindow *		Window ;
	WINDOWMESSAGE *		WindowMessage ;
	UINT			ObjectId,
				MessageId,
				LParamSize,
				ControlId ;
	WPARAM			WParam ;
	LPARAM			LParam ;
	int			ItemCount, size ;
	TCheopsDialog *		Cheopsdlg ;
	TWinLine *		WinL ;




// Récupération
	CHARtoI16 ( ObjectId , DATA ( ObjectId , UTInitDialog ) ) ;
	Window = Application -> GetObject ( ObjectId ) ;

	if ( Window  !=  NULL )
	   {
		Cheopsdlg = ( TCheopsDialog * ) dynamic_cast <TDecoratedFrame *> ( Window );

		CHEOPSCHECK ( Window, TWindow ) ;
		LINECHECK ( Window , WinL);

		ItemCount     = DATA ( ItemCount, UTInitDialog ) ;
		DialogEntry   = DATA ( Items    , UTInitDialog ) ;

		while  ( ItemCount -- )
		   {
			WindowMessage = & DialogEntry -> Message ;
			CHARtoI16 ( ControlId, DialogEntry -> ControlId ) ;

			CHARtoI16 ( MessageId , WindowMessage -> Message ) ;
			CHARtoI16 ( WParam    , WindowMessage -> WParam ) ;
			CHARtoI16 ( LParamSize, WindowMessage -> LParamSize ) ;

			if  ( ( int ) LParamSize  ==  UWC_LPARAM_NOT_POINTER )
			   {
				CHARtoI32 ( LParam, WindowMessage -> LParam ) ;
				size = sizeof ( INTEGER32 ) ;
			    }
			else
			   {
				LParam = ( LPARAM ) WindowMessage -> LParam ;
				size   = LParamSize ;
				HandleTranslation ( MessageId, ( char * ) LParam ) ;
			    }

		// Harcodé : pour certains messages, on est obligés d'en faire
		// plus...
			switch  ( MessageId )
			   {
				case	WM_ENABLE :
					if (WinL)
						WinL->EnableColonne(ControlId,WParam);
					else  if  ( Cheopsdlg  &&  Cheopsdlg -> Support -> GetNbBox () > 1 )
						Cheopsdlg -> Support -> SendDlgItemMessage ( ControlId,	MessageId, WParam, LParam ) ;
					else
						EnableTextItem ( Window -> GetDlgItem ( ControlId ), WParam ) ;

					if  ( ! WParam )
						Window -> SetFocus ( ) ;					
					break ;

				default :

					if (WinL)
						WinL->SendDlgItemMessage ( ControlId,MessageId, WParam, LParam );
					else  if  ( Cheopsdlg  &&  Cheopsdlg -> Support -> GetNbBox () > 1 )
						Cheopsdlg -> Support -> SendDlgItemMessage ( ControlId,	MessageId, WParam, LParam ) ;
					else
						Window -> SendDlgItemMessage ( ControlId, MessageId, WParam, LParam ) ;
			     }


		// Message suivant
			DialogEntry = ( UTInitDialogEntry * ) (
					( ( char * ) DialogEntry ) +
					   sizeof ( UTInitDialogEntry ) +
					   size -
					   sizeof ( WindowMessage -> LParam ) ) ;
		     }

		I16toCHAR ( UWS_OK, Reply. Status ) ;
	    }
	else
		I16toCHAR ( UWS_UNKNOWN_OBJECT_ID, Reply. Status ) ;


// On répond
	REPLY ( & Reply, sizeof ( Reply ) ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TUnixApplicationTable :: UWFSetCursor :                               */
/*      Change le curseur en l'un des curseurs prédéfinis.                */
/*      Afin que le curseur ne soit pas changé lors du mouvement de la    */
/*      souris, on capture la souris et on relâche la souris lorsque le   */
/*      curseur d'origine est restauré.                                   */
/*                                                                        */
/**************************************************************************/

# pragma	argsused

HANDLER ( UWFSetCursor )
   {
	WTSetCursor      	Reply ;
	integer32		CursorId ;
	HCURSOR			OldCursor ;


// Récupération des paramètres
	CHARtoI32 ( CursorId , DATA ( CursorId , UTSetCursor ) ) ;

// Changement du curseur

	OldCursor = LoadCursor ( NULL, ( LPCSTR ) CursorId ) ;
	SetCursor ( OldCursor ) ;

	if  ( TheCheopsMenuWindowObject )
	   {
		if  ( CursorId  ==  ( integer32 ) IDC_WAIT )
			TheCheopsMenuWindowObject -> SetCapture ( ) ;
		else if  ( GetCapture ( )  ==  TheCheopsMenuWindowObject -> HWindow )
			TheCheopsMenuWindowObject -> ReleaseCapture ( ) ;
	     }
			


// On répond
	I16toCHAR  ( UWS_OK, Reply. Status ) ;
	REPLY ( & Reply, sizeof ( Reply ) ) ;
	 }





/**************************************************************************/
/*                                                                        */
/*  TUnixApplicationTable :: UWFSetIcon   :                               */
/*      Change le curseur en l'un des curseurs prédéfinis.                */
/*                                                                        */
/**************************************************************************/

# pragma	argsused

HANDLER ( UWFSetIcon   )
   {
	WTSetIcon        	Reply ;
	integer16		ObjectId ;
	TWindow *		Window ;
	integer32		IconId ;


// Récupération des paramètres
	CHARtoI16 ( ObjectId, DATA ( ObjectId, UTSetIcon ) ) ;
	CHARtoI32 ( IconId  , DATA ( IconId  , UTSetIcon ) ) ;

	Window = Application -> GetObject ( ObjectId ) ;

// Changement du curseur
	if  ( Window  !=  NULL )
	   {
		CHEOPSCHECK ( Window, TUnixWindow ) ;

		Window -> HandleMessage ( WM_SETICON, 0, IconId ) ;
		I16toCHAR ( UWS_OK, Reply. Status ) ;
	    }
	else
		I16toCHAR ( UWS_UNKNOWN_OBJECT_ID, Reply. Status ) ;


// On répond
	REPLY ( & Reply, sizeof ( Reply ) ) ;
    }





/**************************************************************************/
/*                                                                        */
/*  TUnixApplicationTable :: UWFSetCustomIcon :                           */
/*      Change l'icône de la fenêtre.                                     */
/*                                                                        */
/**************************************************************************/

# pragma	argsused

HANDLER ( UWFSetCustomIcon )
   {
	integer16		ObjectId ;
	integer32		IconFileSize ;
	TWindow *		Window ;
	char far *		IconFile ;
	WTSetCustomIcon      	Reply ;


// Récupération des paramètres
	CHARtoI16 ( ObjectId      , DATA ( ObjectId    , UTSetCustomIcon ) ) ;
	CHARtoI32 ( IconFileSize  , DATA ( IconFileSize, UTSetCustomIcon ) ) ;
	IconFile = DATA ( IconFile, UTSetCustomIcon ) ;
	Window   = Application -> GetObject ( ObjectId ) ;


// Changement du curseur
	if  ( Window  !=  NULL )
	   {
		CHEOPSCHECK ( Window, TWindow ) ;
		Window -> HandleMessage ( WM_SETICON, ( WPARAM ) IconFileSize,
				( LPARAM ) IconFile ) ;
		I16toCHAR ( UWS_OK, Reply. Status ) ;
		 }
	else
		I16toCHAR ( UWS_UNKNOWN_OBJECT_ID, Reply. Status ) ;


// On répond
	REPLY ( & Reply, sizeof ( Reply ) ) ;
    }





/**************************************************************************/
/*                                                                        */
/*  TUnixApplicationTable :: UWFHelp                                      */
/*      Appelle l'aide en ligne de Windows.                               */
/*                                                                        */
/**************************************************************************/

# pragma	argsused


HANDLER ( UWFHelp )
   {
	WTHelp			reply ;
	integer16		object_id ;
	TWindow *		Window ;
	char *			Filename,
	     *			Keyname ;
	DWORD			HelpParam ;
	WORD			HelpCommand ;


// Récupération des paramètres
	CHARtoI16 ( object_id  , DATA ( ParentWindow, UTHelp ) ) ;
	CHARtoI16 ( HelpCommand, DATA ( HelpCommand , UTHelp ) ) ;
	Filename = DATA ( FilenameAndHelpKey, UTHelp ) ;
	Keyname  = Filename + strlen ( Filename ) + 1 ;


	if  ( object_id  ==  0 )
		Window = TerminalWindow ;
	else if  ( ( unsigned int ) object_id  ==  UWC_NO_PARENT )
		Window = NULL ;
	else
		Window = Application -> GetObject ( object_id ) ;

// On vérifie si la commande est supportée
	switch ( HelpCommand )
	   {
		case	HELP_CONTEXT :
			CHARtoI32 ( HelpParam, Keyname ) ;
			break ;

		case	HELP_HELPONHELP :
			break ;

		case	HELP_INDEX :
			break ;

		case	HELP_KEY :
			HelpParam = ( DWORD ) ( LPSTR ) Keyname ;
			Device -> HostToLocalCharSet ( Keyname ) ;
			break ;

		case	HELP_QUIT :
			break ;

		case	HELP_SETINDEX :
			CHARtoI32 ( HelpParam, Keyname ) ;
			break ;

		default :	// HELP_MULTIKEY,...
			I16toCHAR ( UWS_EINVAL, reply. Status ) ;
			goto  TheEnd ;
	     }

// Et ZOU
	Device -> HostToLocalCharSet ( Filename ) ;
	:: WinHelp ( ( Window  ==  NULL ) ?  NULL : Window -> HWindow,
			Filename, HelpCommand, HelpParam ) ;

// On envoie la réponse
	I16toCHAR ( UWS_OK, reply. Status ) ;

TheEnd :
	REPLY ( & reply, sizeof ( reply ) ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TUnixApplicationTable :: GetTerminalObjectId                          */
/*      Retourne l'identificateur de l'object TERMINALWINDOW.             */
/*      Cet identificateur est toujours 0 (sauf erreur interne).          */
/*                                                                        */
/**************************************************************************/

# pragma	argsused


HANDLER ( UWFGetTerminalObjectId )
   {
	WTGetTerminalObjectId	reply ;


	if  ( Application -> GetObject ( 0 )  !=  NULL )
	   {
		I16toCHAR ( UWS_OK, reply. Status ) ;
		I16toCHAR ( 0     , reply. ObjectId ) ;
	    }
	else			// Paranoïa
		I16toCHAR ( UWS_UNKNOWN_OBJECT_ID, reply. Status ) ;


	REPLY  ( & reply, sizeof ( reply ) ) ;
     }




/**************************************************************************/
/*                                                                        */
/*  TUnixApplicationTable :: UWFShowWindow                                */
/*      Appelle la fonction ShowWindow pour l'objet spécifié.             */
/*                                                                        */
/**************************************************************************/

# pragma	argsused

HANDLER ( UWFShowWindow )
   {
	WTShowWindow		reply ;
	integer16		ObjectId ;
	integer16		ShowMode ;
	TWindow *		Window ;



// Récupération des paramètres
	CHARtoI16 ( ObjectId, DATA ( ObjectId, UTShowWindow ) ) ;
	CHARtoI16 ( ShowMode, DATA ( ShowMode, UTShowWindow ) ) ;



// Récupération du pointeur sur l'objet
	if  ( ( Window = Application -> GetObject ( ObjectId ) )  !=  NULL )
	   {
		Window -> Show ( ShowMode ) ;
		I16toCHAR ( UWS_OK, reply. Status ) ;
	    }
	else			// Paranoïa
		I16toCHAR ( UWS_UNKNOWN_OBJECT_ID, reply. Status ) ;


// Réponse
	REPLY  ( & reply, sizeof ( reply ) ) ;
     }




/**************************************************************************/
/*                                                                        */
/*  TUnixApplicationTable :: UWFQueryBaudRate                             */
/*      Retourne la vitesse de la transmission (0xFFFFFFFF pour le réseau)*/
/*                                                                        */
/**************************************************************************/

# pragma	argsused

HANDLER ( UWFQueryBaudRate )
   {
	WTQueryBaudRate		Reply ;


	I16toCHAR ( UWS_OK, Reply. Status ) ;

	if  ( ConnectionParameters -> NetworkLink )  
		I32toCHAR ( 0L, Reply. BaudRate ) ;
	else
		I32toCHAR ( ConnectionParameters -> Serial. RealSpeed, Reply. BaudRate ) ;


// Réponse
	REPLY  ( & Reply, sizeof ( Reply ) ) ;
	  }





/**************************************************************************/
/*                                                                        */
/*  TUnixApplicationTable :: UWFSuperMessage                              */
/*      Gestion d'un message comportant une suite de messages.            */
/*                                                                        */
/**************************************************************************/

# pragma	argsused

HANDLER ( UWFSuperMessage )
   {
	TInputMessage		InputMessage ;
	WTSuperMessage		Reply ;
	register integer16	MessageCount, Remains ;
	register integer16	DataSize ;
	register int		Processed	=  0 ;
	register char *		p ;
	int			Status 		=  UWS_OK ;



//  Récupération des paramètres
	CHARtoI16 ( MessageCount, DATA ( MessageCount, UTSuperMessage ) ) ;
	p = DATA ( MessageData, UTSuperMessage ) ;
	DataSize = Message -> DataSize ;
	Remains  = MessageCount ;

// Indiquer de ne pas envoyer de réponse
	InSuperMessage = TRUE ;

// Tant qu'il y a des caractères
	while  ( DataSize  &&  Remains )
	   {
		InputMessage. Add ( * p ) ;

		if  ( InputMessage. GetState ( )  ==  TInputMessage :: Complete )
		   {
			:: ApplicationTable -> HandleMessage ( & InputMessage. Message,
				MessageOrigin ) ;
			InputMessage. Zero ( ) ;
			Processed ++ ;
			Remains -- ;
		    }

		p ++, DataSize -- ;
	    }


// Rétablir l'envoi de réponse aux messages
	InSuperMessage = FALSE ;

// Et envoyer une réponse globale
	if  ( Processed  !=  MessageCount  ||  DataSize )
		Status = UWS_BAD_MESSAGE_DATA ;

	I16toCHAR ( Status   , Reply. Status ) ;
	I16toCHAR ( Processed, Reply. ProcessedMessages ) ;
	REPLY  ( & Reply, sizeof ( Reply ) ) ;
     }







/**************************************************************************/
/*                                                                        */
/*   UWFExecute :                                                         */
/*      Execute la commande spécifiée.                                    */
/*                                                                        */
/**************************************************************************/

# pragma	argsused

HANDLER ( UWFExecute )
   {
	WTExecute 	Reply ;
	char *		Command ;
	HINSTANCE 	Instance ;


// Récupération des paramètres
	Command = DATA ( PathAndArgs, UTExecute ) ;
	Device -> HostToLocalCharSet ( Command ) ;


// Exécution
	Instance = ( HINSTANCE ) WinExec ( Command, SW_SHOW ) ;


	if  ( Instance  < ( HINSTANCE ) 32 )
		I16toCHAR ( UWS_FAILED, Reply. Status ) ;
	else
		I16toCHAR ( UWS_OK, Reply. Status ) ;


// Réponse
	REPLY  ( & Reply, sizeof ( Reply ) ) ;
   }









# if	defined ( CHEOPS )


/**************************************************************************/
/*                                                                        */
/*  UWFCheopsDialog :                                                     */
/*      Crée une boîte de dialogue Cheops.                                */
/*                                                                        */
/**************************************************************************/

# pragma	argsused

HANDLER ( UWFCheopsDialog )
   {
	TWindow *		ParentWindow ;
	integer16		parent ;
	int			ObjectId ;
	char			Style ;
	TCheopsDialog *		DialogBox ;
	WTCheopsDialog		Reply ;
	BOOL			IsModal		=  FALSE ;


// Récupération des paramètres entiers
	CHARtoI16 ( parent, DATA ( ParentWindow, UTCheopsDialog ) ) ;
	Style = DATA ( DialogType, UTCheopsDialog ) ;

// Interpréter la fenêtre
	if  ( parent  ==  0 )
	   {
		if  ( TheCheopsMenuWindowObject )
			ParentWindow = TheCheopsMenuWindowObject -> GetClientWindow ( ) ;
		else
			ParentWindow = TerminalWindow ;
	    }
	else if  ( ( unsigned ) parent  ==  UWC_NO_PARENT )
		ParentWindow = NULL ;
	else
	   {
		ParentWindow = Application -> GetObject ( parent ) ;
		IsModal = TRUE ;	// Fenetre lancée par un autre programme :
					// elle doit etre modale
	    }


// Création de la fenêtre
	switch ( Style )
	   {
		case	UWC_INVISIBLE_CHEOPS_DIALOG :
	// Ecran de saisie Cheops "simple"
		case	UWC_REGULAR_CHEOPS_DIALOG :
			DialogBox = new TCheopsDialog ( Application,
						ParentWindow, Message, IsModal ) ;
			ObjectId  = Application -> AddObject ( DialogBox ) ;


		// Construction du résultat
			if  ( ObjectId  ==  -1  ||  DialogBox  ==  NULL )
				I16toCHAR ( UWS_NOT_ENOUGH_MEMORY, Reply. Status ) ;
			else
			   {
				DialogBox -> SetObjectId ( ObjectId ) ;
				I16toCHAR ( UWS_OK, Reply. Status ) ;
				I16toCHAR ( ObjectId, Reply. ObjectId ) ;
				DialogBox -> Create ( ) ;
			    }
			break ;

	// Ecran de saisie Cheops "entête-lignes"
		case UWC_SPLITTED_CHEOPS_DIALOG :
			TWinLine *Lignes;

			DialogBox = ( TCheopsDialog * ) dynamic_cast <TDecoratedFrame *> (ParentWindow);
			if  ( DialogBox  ==  NULL )
				throw  Bad_cast ( ) ;

			if (!DialogBox->Support->WinLine)
				Lignes=DialogBox->Support->AddWinLignes(Message);

			ObjectId  = Application -> AddObject ( Lignes ) ;

			if  ( ObjectId  ==  -1  ||  Lignes ==  NULL )
				I16toCHAR ( UWS_NOT_ENOUGH_MEMORY, Reply. Status ) ;
			else
			   {
				Lignes -> SetObjectId ( ObjectId ) ;
				I16toCHAR ( UWS_OK, Reply. Status ) ;
				I16toCHAR ( ObjectId, Reply. ObjectId ) ;
			    }
			break ;

	// Type d'écran invalide ou pas encore supporté :
		default :
			I16toCHAR ( UWS_EINVAL, Reply. Status ) ;
	     }


	REPLY ( & Reply, sizeof ( Reply ) ) ;
    }


/**************************************************************************/
/*                                                                        */
/*  UWFCheopsDialogModify :                                               */
/*      Crée une boîte de dialogue Cheops et remplace un ancienne.        */
/*	Rappel : il n'y a pas destruction des lignes car c'est le	  */
/*		programme unix qui doit detruire explicitement les lignes */
/*		avant de demander une fenetre invisible			  */
/*                                                                        */
/**************************************************************************/

# pragma	argsused

HANDLER ( UWFCheopsDialogModify )
   {
	TWindow *		ReplaceWindow,
		*		ParentWindow ;
	integer16		parent,
				replace ;
	int			ObjectId ;
	char			Style ;
	TCheopsDialog *		DialogBox ;
	WTCheopsDialogModify	Reply ;


// Récupération des paramètres entiers
	CHARtoI16 ( parent, DATA ( ParentWindow, UTCheopsDialogModify ) ) ;
	CHARtoI16 ( replace, DATA ( ReplaceWindow, UTCheopsDialogModify ) ) ;
	Style = DATA ( DialogType, UTCheopsDialogModify ) ;

// Interpréter la fenêtre
	ReplaceWindow = Application -> GetObject ( replace ) ;

	if  ( ReplaceWindow  ==  NULL )
	   {
		I16toCHAR ( UWS_UNKNOWN_OBJECT_ID, Reply. Status ) ;
		REPLY ( & Reply, sizeof ( Reply ) ) ;
		return ;
	    }

	TCheopsDialog * swapdialog = ( TCheopsDialog * ) dynamic_cast <TDecoratedFrame *> ( ReplaceWindow ) ;
	ParentWindow = Application -> GetObject ( parent ) ;
	TCheopsDialog * swapparent = ( TCheopsDialog * ) dynamic_cast <TDecoratedFrame *> ( ParentWindow ) ;

	if  ( swapdialog  ==  NULL )
		throw  Bad_cast ( ) ;


// Création de la fenêtre
	switch ( Style )
	   {
	// Ecran invisible pour l'utilisateur
		case	UWC_INVISIBLE_CHEOPS_DIALOG :
	// Ecran de saisie Cheops "simple"
		case	UWC_REGULAR_CHEOPS_DIALOG :
			DialogBox = new TCheopsDialog ( Application,
						ReplaceWindow -> Parent, Message ) ;
			ObjectId  = Application -> AddObject ( DialogBox ) ;

		// Construction du résultat
			if  ( ObjectId  ==  -1  ||  DialogBox  ==  NULL )
				I16toCHAR ( UWS_NOT_ENOUGH_MEMORY, Reply. Status ) ;
			else
			   {
				Application -> SwapObject ( ObjectId, replace ) ;
				DialogBox -> SetObjectId ( replace ) ;
				swapdialog -> SetObjectId ( ObjectId ) ;
				Application -> DeleteObject ( ObjectId ) ;
				I16toCHAR ( UWS_OK, Reply. Status ) ;
				DialogBox -> Create ( ) ;
			    }
			I16toCHAR ( replace, Reply. ObjectId ) ;
			break ;

	// Ecran de saisie Cheops "entête-lignes"
		case UWC_SPLITTED_CHEOPS_DIALOG :
			TWinLine *		Lignes ;

			DialogBox = swapparent ;
			if  ( DialogBox  ==  NULL )
				throw  Bad_cast ( ) ;

			Lignes = DialogBox -> Support -> AddWinLignes ( Message ) ;
			ObjectId  = Application -> AddObject ( Lignes ) ;

			if  ( ObjectId  ==  -1  ||  Lignes ==  NULL )
				I16toCHAR ( UWS_NOT_ENOUGH_MEMORY, Reply. Status ) ;
			else
				{
				Application -> SwapObject ( ObjectId, replace ) ;
				Lignes -> SetObjectId ( replace ) ;
				Application -> DeleteObject ( ObjectId ) ;

				I16toCHAR ( UWS_OK, Reply. Status ) ;
				 }
			I16toCHAR ( replace, Reply. ObjectId ) ;
			break ;

	// Type d'écran invalide ou pas encore supporté :
		default :
			I16toCHAR ( UWS_EINVAL, Reply. Status ) ;
		  }

	REPLY ( & Reply, sizeof ( Reply ) ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TUnixDialogBox :: UWFSetCheopsFunctionKeys                            */
/*      Change les labels de touche de fonction d'un écran Cheops.        */
/*                                                                        */
/**************************************************************************/

# pragma	argsused


HANDLER ( UWFSetCheopsFunctionKeys )
   {
	WTSetCheopsFunctionKeys		reply ;
	integer16			object_id ;
	TWinLine			*WinLine;
	TCheopsDialog *			Window 	=  NULL ;
	TWindow *			W ;
	UTFunctionKeyDefinition *	defp ;
	integer16			KeyCount ;
	register char * 		p ;
	


// Récupération des paramètres
	CHARtoI16 ( object_id, DATA ( ObjectId, UTSetCheopsFunctionKeys ) ) ;
	KeyCount = DATA ( KeyCount   , UTSetCheopsFunctionKeys ) ;
	defp     = DATA ( Definitions, UTSetCheopsFunctionKeys ) ;

// Récupération de la fenêtre
	W   = Application -> GetObject ( object_id ) ;

	if  ( W )
	   {
		if  ( typeid ( * W )  ==  typeid ( TWinLine ) )
		   {
			WinLine = ( TWinLine * ) dynamic_cast <TWindow *> ( W ) ;
			if  ( WinLine  ==  NULL )
				throw  Bad_cast ( ) ;
			Window = WinLine -> Support;
		    }
		else
		   {
			Window = ( TCheopsDialog * ) dynamic_cast <TDecoratedFrame *> ( W ) ;
			
			if  ( Window  ==  NULL )
				throw  Bad_cast ( ) ;
		    }
	      }


// Redéfinition des labels
	if  ( Window  ==  NULL )
		I16toCHAR ( UWS_UNKNOWN_OBJECT_ID, reply. Status ) ;
	else
	   {
		register BOOL		OldKeys	=  Window -> OldFunctionKeys ;


		if  ( ! OldKeys )
			Window -> ControlBar -> BeginUpdate ( ) ;

		for  ( int  i = 0 ; i < KeyCount ; i ++ )
		   {
			if  ( defp -> KeyNumber > 0  &&
					defp -> KeyNumber  <=  8 )
			   {
				p = defp -> KeyLabelAndHelp +
					strlen ( defp -> KeyLabelAndHelp ) + 1 ;
				Device -> HostToLocalCharSet ( defp -> KeyLabelAndHelp ) ;
				Device -> HostToLocalCharSet ( p ) ;

				if  ( OldKeys )
					Window	-> SetFunctionKey ( defp -> KeyNumber, 
						defp -> KeyLabelAndHelp, p ) ;
				else
					Window -> ControlBar -> SetFunctionKey ( defp -> KeyNumber, 
						defp -> KeyLabelAndHelp, p ) ;
			    }

			defp = ( UTFunctionKeyDefinition * ) 
				  ( ( char * ) defp + sizeof ( UTFunctionKeyDefinition ) +
					strlen ( defp -> KeyLabelAndHelp ) + strlen ( p ) + 1 ) ;
		     }


		if  ( ! OldKeys )
			Window -> EndFunctionKeyUpdate ( ) ;
 
		if  ( ! Window -> HasBeenDisplayed )
		   {
			TerminalWindow -> ComputeNextPosition ( Window ) ;
			:: LockWindowUpdate ( Window -> HWindow ) ;
			Window -> Show ( SW_SHOW ) ;
			Window -> HasBeenDisplayed = TRUE ;
			:: LockWindowUpdate ( NULL ) ;
		    }

		I16toCHAR ( UWS_OK, reply. Status ) ;
	     }


// On envoie la réponse
	REPLY ( & reply, sizeof ( reply ) ) ;
    }





/**************************************************************************/
/*                                                                        */
/*  TUnixApplicationTable :: UWFSetCheopsHelp                             */
/*      Associe une table de contextes à une fenêtre (voir Help.h pour    */
/*      plus de renseignements).                                          */
/*                                                                        */
/**************************************************************************/

# pragma	argsused

HANDLER ( UWFSetCheopsHelp )
   {
	WTSetCheopsHelp		reply ;
	integer16		ObjectId ;
	integer16		ResourceId ;
	TWindow *		Window ;
	TCheopsDialog *		CheopsDialog 	=  NULL ;
	register integer16	i ;



// Récupération des paramètres
	CHARtoI16 ( ObjectId, DATA ( ObjectId, UTSetCheopsHelp ) ) ;
	CHARtoI16 ( ResourceId, DATA ( ResourceId, UTSetCheopsHelp ) ) ;


	Window = ( TWindow * ) Application -> GetObject ( ObjectId ) ;

	if  ( Window )
		CheopsDialog = ( TCheopsDialog * ) dynamic_cast <TDecoratedFrame *> ( Window );

// Récupération du pointeur sur l'objet
	if  ( CheopsDialog  !=  NULL )
	   {
		i = CheopsDialog -> SetHelp ( ResourceId ) ;
		I16toCHAR ( i, reply. HelpId ) ;


		if  ( i < 0 )
			I16toCHAR ( UWS_FAILED, reply. Status ) ;
		else
			I16toCHAR ( UWS_OK, reply. Status ) ;
	    }
	else			// Paranoïa
		I16toCHAR ( UWS_UNKNOWN_OBJECT_ID, reply. Status ) ;


// Réponse
	REPLY  ( & reply, sizeof ( reply ) ) ;
     }




# endif 	/* CHEOPS */


/**************************************************************************/
/*                                                                        */
/*  TUnixApplicationTable :: UWFMenu                            	  */
/*      Gestion des menus des fenêtres.                  		  */
/*                                                                        */
/**************************************************************************/

# pragma	argsused

HANDLER ( UWFMenu )
   {
	TCheopsDialog *	window 		=  NULL ;
	TWindow * 	W ;
	integer16	object_id,
			count ;
	char *		body ;
	WTMenu		output ;
	TPopupMenu *	submenu ;

	CHARtoI16 ( object_id, DATA ( Id_win, UTMenu ) ) ;

// Récupération de la fenêtre
	W   = Application -> GetObject ( object_id ) ;

	if  ( ! W )
	   {
		I16toCHAR ( UWS_UNKNOWN_OBJECT_ID, output. Status ) ;
		REPLY  ( & output, sizeof ( output ) ) ;
        	return ;
	    }

	window = ( TCheopsDialog * ) dynamic_cast <TDecoratedFrame *> ( W ) ;

	if  ( window  ==  NULL )
		throw  Bad_cast ( ) ;

// Création du PopupMenu
	if  ( ! window -> menu )
	{
		window -> menu = new TMenu ( ) ;
	}

	CHARtoI16 ( object_id, DATA ( Id_menu, UTMenu ) ) ;
	while  ( window -> menu -> GetSubMenu ( object_id ) )
		window -> menu -> DeleteMenu ( object_id, MF_BYPOSITION ) ;

	submenu = new TPopupMenu ( ) ;
	window -> menu -> AppendMenu ( MF_STRING | MF_ENABLED | MF_POPUP, (uint) submenu -> GetHandle ( ) ,DATA ( text, UTMenu ) ) ;

	body = Message -> Data + sizeof ( UTMenu ) ;
	body += strlen ( DATA ( text, UTMenu ) ) ;
	CHARtoI16 ( count, DATA ( Nb_elt, UTMenu ) ) ;
	for  ( ; count ; count -- )
	   {
		CHARtoI16 ( object_id, body ) ;
		body += sizeof ( INTEGER16 ) ;
		while ( * body  ==  ' '  ||  * body  ==  '\t' )
			body ++ ;
		submenu -> AppendMenu ( MF_STRING | MF_ENABLED, - object_id, body ) ;
		while  ( * body )
			body ++ ;
		body ++ ;
	    }

	W -> SetMenu ( window -> menu ->GetHandle ( ) ) ;
	W -> DrawMenuBar ( ) ;
	W -> Attr. H += 40 ;
	W -> MoveWindow ( W -> Attr. X, W -> Attr. Y, W -> Attr. W, W -> Attr. H+20, SWP_NOMOVE | SWP_NOZORDER ) ;
	W -> UpdateWindow ( ) ;

	I16toCHAR ( UWS_OK, output. Status ) ;

	REPLY  ( & output, sizeof ( output ) ) ;
     }

/**************************************************************************/
/*                                                                        */
/*  TUnixApplicationTable :: UWFTimer                               	  */
/*      Gestion des signaux de reveil de tache.                  	  */
/*                                                                        */
/**************************************************************************/

# pragma	argsused

HANDLER ( UWFTimer )
	{
	TCheopsDialog *	window 	=  NULL ;
	TWindow * 	W ;
	integer16	object_id,
			count ;
//	char *		body ;
	WTTimer		output ;

	CHARtoI16 ( object_id, DATA ( Id_win, UTTimer ) ) ;
// Récupération de la fenêtre
	W   = Application -> GetObject ( object_id ) ;

	if  ( ! W )
	   {
		I16toCHAR ( UWS_UNKNOWN_OBJECT_ID, output. Status ) ;
		goto  TheEnd ;
	    }

	window = ( TCheopsDialog * ) dynamic_cast <TDecoratedFrame *> ( W ) ;

	if  ( window  ==  NULL )
		throw  Bad_cast ( ) ;

	W -> KillTimer ( 1 ) ;		// 1 par exemple
	CHARtoI16 ( count, DATA ( temporisation, UTTimer ) ) ;
	W -> SetTimer ( 1, 1000* count ) ;  // 1 par exemple

	I16toCHAR ( UWS_OK, output. Status ) ;

TheEnd :
	REPLY  ( & output, sizeof ( output ) ) ;
	  }


/**************************************************************************/
/*                                                                        */
/*  TUnixApplicationTable :: UWFInfoLine                               	  */
/*      Gestion des caracteristique d'une fenêtre ligne.               	  */
/*                                                                        */
/**************************************************************************/

# pragma	argsused

HANDLER ( UWFInfoLine )
	{
	TWinLine *			window ;
	TWindow * 			W ;
	integer16			object_id ;
	WTInfoLine			output ;
	integer16			operation;
	integer16			NbLigne;
	integer16			position;

	CHARtoI16 ( object_id, DATA ( Id_win, UTInfoLine ) ) ;
	CHARtoI16 ( operation, DATA ( Type_operation , UTInfoLine ) );
	CHARtoI16 ( NbLigne, DATA ( Nb_ligne , UTInfoLine ) );
	CHARtoI16 ( position, DATA ( Y_position , UTInfoLine ) );

// Récupération de la fenêtre
	W   = Application -> GetObject ( object_id ) ;

	if  ( ! W )
	   {
		I16toCHAR ( UWS_UNKNOWN_OBJECT_ID, output. Status ) ;
		goto  TheEnd ;
	    }


	window = dynamic_cast <TWinLine *> ( W ) ;

	if  ( window  ==  NULL )
		throw  Bad_cast ( ) ;

	switch (operation) {
		case UWC_IL_NORMAL  :
			window -> RecevoirLignes(NbLigne);
			break;

		case UWC_IL_INSERT  :
			window -> InsererLigne(NbLigne,position);
			break;

		case UWC_IL_DELETE  :
			window -> SupprimerLigne(NbLigne,position);
			break;

		case UWC_IL_REWRITE :
			window -> EffacerLigne(NbLigne, position);
			break;

		case UWC_IL_UNKNOWN :

		default		    :
			MessageBox(0,"Partie ligne-UWFInfoLine",
				"Operation inconnue ou pas encore traitée",MB_OK);

		}


	I16toCHAR ( UWS_OK, output. Status ) ;

TheEnd :
	REPLY  ( & output, sizeof ( output ) ) ;
	  }









/*=====================================================================*/
/*=                 Decompression-edition                             =*/
/*=====================================================================*/

/*
 *	Cette fonction recoit des blocs de données compressées.
 *	Elle décompresse ces données et ouvre une fenetre sous Windows
 *	pour que l'utilisateur puisse les visualiser.
 *
 *	L'utilisateur peut commencer à visualiser dès la reception du premier
 *	bloc. Les données sont accessibles au fur et à mesure qu'elles arrivent.
 *
 *  	Note (CV) : je vais gueuler ! qu'est-ce que c'était que l'indentation
 *                  qu'il y avait avant ?
 */

HANDLER ( UWFEditFile )
   {
	WTEditFile  	Reply;
	UTEditFile 	*UData;
	int        	erreur;
	char       	*pData;         // ptr vers le bloc de donnée à décompresser
	int	   	size;           // taille du bloc de donnée
	TEditWindow	*window;
	TWindow	   	*W;
	integer16  	ObjectId;
	long int	FileSize ;



//  on charge le sablier pour l'afficher pendant la décompression
// "CURSOR_WAIT" est identique à IDC_WAIT, sauf que le nôtre possède un hot spot
// au milieu du sablier qui permet à l'utilisateur de cliquer ailleurs pendant
// ce temps
	HCURSOR	   hrcWait=::LoadCursor( * TerminalWindow -> GetModule ( ), "CURSOR_WAIT" ) ;
	HCURSOR    hrcOld;


// on recupere le message provenant d'UNIX
	    UData=(UTEditFile *)Message->Data;


// si on recoit le premier bloc d'un fichier, on crée un nouvel objet
	if (UData->nbBloc==1) 
	  {
		CHARtoI32 ( FileSize, UData -> FileSize ) ;
		window=new TEditWindow ( Application, ( TheCheopsMenuWindowObject ) ?
			TheCheopsMenuWindowObject : 0,
				FileSize ) ;
		ObjectId=Application->AddObject(window);
		window->ObjectId=ObjectId;
	    }
//  sinon on recherche le pointeur de la fenetre correspondante
	else 
	   {
		CHARtoI16(ObjectId,UData->nbFen);
		W=Application->GetObject(ObjectId);

		if  ( ! W )
		   {
			Reply. code = ERR_CLOSED ;
			REPLY ( & Reply, sizeof ( Reply ) ) ;
			return ;
		    }

		window= dynamic_cast <TEditWindow *> ( W );
	     }



// on vérifie que le nouvel objet a bien été crée ou si le pointeur
// sur la fenetre a bien pu être récupérer
	if (ObjectId==-1)
		erreur=ERR_ALLOC;
	else if ( window  ==  NULL )
		erreur = ERR_OPEN ;
	else 
	   {

	// la fonction InitDecomp a pour role d'initialiser la decompression lorsque
	// l'on recoit le premier bloc, ou bien de controler que le bloc reçu correspond
	// bien au bloc attendu
	// elle effectue en plus quelques initialisations
		if ((erreur=window->InitializeDecompression(Message,&pData,&size))==0)
		   {

		// on met le sablier pendant la décompression
			hrcOld=::SetCursor(hrcWait);

		// si l'initialisation s'est bien passée alors on decompresse
			erreur=window->Uncompressor->Uncompress(pData,size);

		// on restaure la souris d'origine
			::SetCursor(hrcOld);
			::DestroyCursor ( hrcWait ) ;
		      }


	// eventuellement on traite les erreurs
		if (erreur)   
			window->Uncompressor->CheckError(erreur);


	// si on vient de decompresser le premier bloc et qu'aucune erreur n'est
	// survenue alors on crée la fentre et on l'affiche
	// la mise a jour sert à actualiser le nombre de lignes disponibles dans le
	// fichier pour l'utilisateur. Cela lui permet de pouvoir commencer à
	// visualiser le texte dès la reception du premier bloc
		if ( ! erreur  ||  erreur  ==  ERR_EOF ) 
		   {
			window -> UpdateText ( ) ;
	
			if  ( ! window -> HWindow ) 
			   {
				window->Create() ;
				window -> Show ( SW_SHOW ) ;
			     }


		// Tant qu'on n'est pas en fin de fichier, on envoie dans la file
		// d'évènements à destination d'Unix un message qui sera dispatché
		// à la fonction Transmission() par UWFNextEvent().
			if  ( erreur  !=  ERR_EOF )
				EventQueue -> Enqueue ( Application -> ApplicationId,
					ObjectId, Message -> ProcessId,
					  0, 0, 0, UWC_LPARAM_NOT_POINTER,
						UWF_EDIT_FILE ) ;
		// Sinon on supprime cet object de la liste des objects de 
		// l'application afin de le laisser ouvert jusqu'à la sortie
		// de Terminal
			else
				Application -> DeleteObject ( ObjectId, FALSE ) ;

		     }
	    }


 // on initialise le status de retour
	Reply.code=erreur;
	I16toCHAR(ObjectId,Reply.ObjectId);


// Il arrive parfois que la fonction soit appelée deux fois alors qu'UNIX n'a
// emis qu'un seul message. La fonction ayant déja renvoye un message à UNIX
// pour lui signaler qu'elle a bien recu le bloc, elle ne renvoie aucun message
// la deuxieme fois.
//  Cela peut paraitre bizarre mais tout rentre dans l'ordre !!!!!!!!!!!
	if (erreur!=ERR_RECEIVED)
		REPLY ( & Reply, sizeof( Reply ) );


// on avertit l'utilisateur si la transmission du fichier ne peut pas être
// éffectué dans sa totalité
	if  ( erreur  !=  ERR_ALLOC  &&  erreur  !=  ERR_OPEN )
	   {
		char Titre[ MAX_PARAMETER_LENGTH ];

		if (window->IsWindow()) 
		   {
			window->GetWindowText(Titre, sizeof ( Titre ) );
			strcat(Titre,"-Erreur");
		     }
		else 
			strcpy(Titre,"Erreur");

		if (erreur && (erreur!=ERR_CLOSED) && (erreur!=ERR_EOF)&&(erreur!=ERR_RECEIVED))
			MessageBox(0,"La transmission du fichier a échoué.\n\n"
				"Le fichier est donc incomplet. Si vous souhaitez"
				" le consulter entièrement, veuillez fermer\n"
				"la fenêtre et recommencer l'édition",Titre,
				MB_OK|MB_ICONSTOP);
	    }
    }




/**************************************************************************/
/*                                                                        */
/*  UWFSetStatusBarText                                                   */
/*      Permet à une application Unix de redéfinir le texte d'un gadget   */
/*      de la barre de statut.                                            */
/*      Le champ TextIndex de la structure UT indique le numéro du        */
/*      gadget texte, et le champ Text son nouveau contenu.               */
/*      Un contenu vide entraîne la suppression du gadget.                */
/*                                                                        */
/**************************************************************************/

HANDLER ( UWFSetStatusBarText )
   {
	WTSetStatusBarText	reply ;
	integer16		ObjectId ;
	int			Index, i ;
	char *			Text ;
	TWindow *		Window ;
	TCheopsDialog *		CheopsDialog 	=  NULL ;
	TMultiFontTextGadget * 	TextGadget ;
	TStatusBar *		StatusBar ;
	int			Status 		=  UWS_OK ;



// Récupération des paramètres
	CHARtoI16 ( ObjectId, DATA ( ObjectId, UTSetStatusBarText ) ) ;
	Index = DATA ( TextIndex, UTSetStatusBarText ) ;
	Text  = DATA ( Text     , UTSetStatusBarText ) ;
	Device -> HostToLocalCharSet ( Text ) ;
	Beautify ( Text ) ;

	
	Window = ( TWindow * ) Application -> GetObject ( ObjectId ) ;

	if  ( Window )
		CheopsDialog = ( TCheopsDialog * ) dynamic_cast <TDecoratedFrame *> ( Window );

// Récupération du pointeur sur l'objet
	if  ( CheopsDialog  ==  NULL )
	   {
		TWinLine *	WinLine = dynamic_cast <TWinLine *> ( Window ) ;


		if  ( WinLine )
			CheopsDialog = WinLine -> Support ;
	     }

	if  ( CheopsDialog  !=  NULL )
	   {
		StatusBar = CheopsDialog -> StatusBar ;

	// Quand on demande l'élément x d'une status bar, OWL fait x fois
	// g = g -> NextGadget ( ), et nous donne une belle GPF si la status bar
	// contient moins de x éléments
		i = Index ;

		for  ( TextGadget = ( TMultiFontTextGadget * ) StatusBar -> FirstGadget ( ) ;
				i > 0  &&  TextGadget ; i -- )
			TextGadget = ( TMultiFontTextGadget * ) TextGadget -> NextGadget ( ) ;


	// Si le gadget existe...
		if  ( ! Index )
			StatusBar -> SetText ( Text ) ;
		else if  ( TextGadget  !=  NULL )
		   {

		// Si le texte n'est pas vide
			if  ( * Text )
				TextGadget -> SetText ( Text ) ;	// Changer
			else
			   {
				StatusBar -> Remove ( * TextGadget ) ;	// sinon supprimer
				delete  TextGadget ;
				StatusBar -> LayoutSession ( ) ;
			    }
		     }
	// Sinon, si le texte n'est pas vide, cela signifie qu'il faut
	// rajouter un gadget texte (à la condition que le gadget d'index
	// Index-1 existe)
		else if  ( * Text )
		   {
			TextGadget = ( TMultiFontTextGadget * ) ( * StatusBar ) [ Index - 1 ] ;

			if  ( TextGadget  !=  NULL )
			   {
				StatusBar -> Insert ( * new  TMultiFontTextGadget ( ) ) ;
				( ( TMultiFontTextGadget * ) ( * StatusBar ) [ Index ] ) -> SetText ( Text ) ;

			// Si on n'appelle pas cette fonction, le nouveau gadget 
			// n'est affiché que lorsque la largeur de la fenêtre
			// est modifiée : merci, OWL
				StatusBar -> LayoutSession ( ) ;
			    }
			else
				Status = UWS_EINVAL ;
		     }
		else
			Status = UWS_UNKNOWN_OBJECT_ID ;
	     }
	else
		Status = UWS_UNKNOWN_OBJECT_ID ;
	
 
// Renvoi de la	 réponse
	I16toCHAR ( Status, reply. Status ) ;
	REPLY ( & reply, sizeof ( reply ) ) ;
     }
			







/**************************************************************************/
/*                                                                        */
/*  UWFCheopsMenu -                                                       */
/*      Gestion du menu Cheops (inclut un ensemble de sous-fonctions      */
/*      spécifiques).                                                     */
/*                                                                        */
/**************************************************************************/

# pragma	argsused

HANDLER ( UWFCheopsMenu )
   {
	integer16		MenuSubFunction ;


// Récupération des paramètres
	CHARtoI16 ( MenuSubFunction, DATA ( MenuFunction, UTCheopsMenu ) ) ;


// Vérifier si le menu a été créé
	if  ( TheCheopsMenuWindowObject  &&  
		MenuSubFunction  ==  MNF_INITIALIZE )
	   {
		WTCheopsMenu		Reply ;
	    
	
		I16toCHAR ( UWS_UNKNOWN_OBJECT_ID, Reply. Status ) ;
		REPLY ( & Reply, sizeof ( Reply ) ) ;
	     }
	else if  ( MenuSubFunction  ==  MNF_INITIALIZE )
	   {
		WTCheopsMenu *		Reply  =  ( WTCheopsMenu * )
						new  char [ sizeof ( WTCheopsMenu ) - 1 +
							    sizeof ( integer16 ) ] ;
		integer16		ObjectId ;
		TCheopsMenuWindow *	MNW ;


		Device -> HostToLocalCharSet ( DATA ( MenuParams, UTCheopsMenu ) ) ;

		MNW = new  TCheopsMenuWindow ( TerminalWindow, 
				DATA ( MenuParams, UTCheopsMenu ) ) ;
		ObjectId = Application -> AddObject ( MNW ) ;
		MNW -> SetIds ( Application, ObjectId ) ;

		
		I16toCHAR ( UWS_OK, Reply -> Status ) ;
		I16toCHAR ( ObjectId, Reply -> Data ) ;
		REPLY ( Reply, sizeof ( WTCheopsMenu ) - 1 + sizeof ( integer16 ) ) ;
		delete  []  ( char * )  Reply ;
	     }
	else if  ( ! TheCheopsMenuWindowObject )
	   {
		WTCheopsMenu		Reply ;
	    
	
		I16toCHAR ( UWS_UNKNOWN_OBJECT_ID, Reply. Status ) ;
		REPLY ( & Reply, sizeof ( Reply ) ) ;
	     }
	else
	   {
		WTCheopsMenu *		Reply ;
		int			Size ;
		char *			NewData ;
		int			RepSize ;
		int			Status ;


		NewData = TheCheopsMenuWindowObject -> ProcessMenuFunction ( 
				MenuSubFunction, 
				DATA ( MenuParams, UTCheopsMenu ),
				Status, Size ) ;

		RepSize = sizeof ( WTCheopsMenu ) + Size - 1 ;

		Reply = ( WTCheopsMenu * ) new  char [ RepSize ] ;
		I16toCHAR ( MenuSubFunction, Reply -> MenuFunction ) ;
		I16toCHAR ( Status, Reply -> Status ) ;

		if  ( Size  &&  NewData )
		   {
			memcpy ( Reply -> Data, NewData, Size ) ;
			delete []  NewData ;
		    }
		
		REPLY ( Reply, RepSize ) ;
		delete []  ( char * ) Reply ;
	     }
     }
			
