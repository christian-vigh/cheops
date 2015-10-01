/**************************************************************************/
/*                                                                        */
/*   UnixApp.h  -  Christian Vigh, 94/010/03.                             */
/*                                                                        */
/*   Gestion des applications Unix.                                       */
/*                                                                        */
/**************************************************************************/



# define	MAX_UNIX_APPLICATIONS		64


// Message envoyé pour signaler que la fermeture de l'objet fenêtre est due
// à un UWFEndApplication
# define	WM_ENDAPPLICATION		( WM_USER + 1000 )



/**************************************************************************/
/*                                                                        */
/*   Classe TUnixApplication :                                            */
/*      Gère les évènements propres à une application.                    */
/*                                                                        */
/**************************************************************************/

class	TDisplayDevice ;
class	TConnection ;
class	TMessageQueue ;
class	TEventQueue ;

extern TEventQueue *	EventQueue ;


# define	MAX_APPLICATION_OBJECTS			256
# define	MAX_COMPRESSED_TRANSFERS		32


class	TFileUncompressor ;


class	TUnixApplication
   {
	private :
		char			ApplicationName    [ MAX_APPLICATION_NAME_LENGTH ] ;
		TWindow *		ApplicationObjects [ MAX_APPLICATION_OBJECTS ] ;
		TConnection *		Connection ;


	public :
		integer16		ApplicationId ;
		integer16		ProcessId ;


	public :
		TUnixApplication	( char *		name,
					  int			appid,
					  integer16		pid,
					  TConnection *		connection = :: Connection ) ;

	       ~TUnixApplication  ( ) ;


		char *		GetName  ( void )
		   {
			return ( ApplicationName ) ;
		    }


		TConnection *	GetOrigin  ( void )
		   {
			return  ( Connection ) ;
	 	    }


		int		AddObject  	( TWindow *	object ) ;
		void		SwapObject	( int 		source_id, 
						  int 		dest_id ) ;
		TWindow *	GetObject	( int		object_id ) ;
		int		GetObjectIndex	( HWND		hwnd ) ;
		void		DeleteObject	( int		object_id,
						  BOOL 		erase  =  TRUE ) ;
		void		DeleteObject    ( TWindow *	diedchild ) ;

	friend class  TObjectListDialog ;
	
       } ;






/**************************************************************************/
/*                                                                        */
/*  La classe TUnixApplicationTable gère les applications Unix.           */
/*                                                                        */
/**************************************************************************/

# define	EnhancedFunction(name)					\
			void  name  ( TUnixApplication *  Application,	\
				      TUnixMessage *      Message )

class	TUnixApplicationTable
   {
	private :
		typedef  void  ( TUnixApplicationTable :: * TUnixMessageHandler )
					( TUnixApplication *  Application,
					  TUnixMessage *      Message ) ;


		typedef struct
		   {
				BOOL			Blocks ;
				integer16		FunctionId ;
				TUnixMessageHandler     Handler ;
		     }  TDispatchEntry ;


		typedef struct
		   {
				TUnixMessage 		Message ;
				char *			Data ;
				integer16		DataSize ;
				TConnection *		Connection ;
		     }  TLastMessage ;



		struct	 TCompressedTransfer 
		   {
			int			FileId ;
			TFileUncompressor *	Uncompressor ;
		    } ;


			


		static TDispatchEntry		DispatchTable [ ] ;
		static int			DispatchTableSize ;
		
		TCompressedTransfer 		TransferTable [ MAX_COMPRESSED_TRANSFERS ] ;

		TUnixApplication **		ApplicationTable ;
		int				ApplicationCount,
						ApplicationTableSize ;
		TLastMessage			LastMessage ;
		BOOL				InSuperMessage ;
		TConnection *			MessageOrigin ;



	public :
		TUnixApplicationTable ( int 	table_size  =  MAX_UNIX_APPLICATIONS ) ;

	       ~TUnixApplicationTable  ( ) ;


		void	Answer			( TUnixApplication *	app,
						  TUnixMessage *	Message,
						  void *		data,
						  integer16		data_size ) ;
		void 	HandleMessage  		( TUnixMessage *  	msg,	
						  TConnection *		origin ) ;
		void	ApplicationListBox	( void ) ;

		TUnixApplication *
			Add			( TUnixMessage *        msg ) ;
		void	Remove			( TUnixApplication *	app ) ;
		void	RemoveObject 		( int			appid,
						  TWindow *		win ) ;

	
		TUnixApplication *	Get  ( int  appid )
		   {
			if  ( appid  >  0  &&  appid  <  ApplicationTableSize )
				return  ( ApplicationTable [ appid ] ) ;
			else
				return  ( 0 ) ;
		    }



	// Retourne la connexion d'origine de l'application
		TConnection *	GetApplicationOrigin  ( integer16  appid )
		   {
			if  ( ApplicationTable [ appid ] )
				return  ( ApplicationTable [ appid ] -> GetOrigin ( ) ) ;
			else
				return ( 0 ) ;
		     }



	private :
		int			Find	 ( integer16		pid ) ;






	/*******************************************************************/
	/*                                                                 */
	/*  Fonctions "intelligentes" :                                    */
	/*                                                                 */
	/*******************************************************************/

	protected :
		EnhancedFunction ( UWFInitializeApplication 	) ;
		EnhancedFunction ( UWFEndApplication 		) ;
		EnhancedFunction ( UWFNullMessage 		) ;

		EnhancedFunction ( UWFOpenFile 			) ;
		EnhancedFunction ( UWFReadFile			) ;
		EnhancedFunction ( UWFWriteFile			) ;
		EnhancedFunction ( UWFCloseFile			) ;
		EnhancedFunction ( UWFGetFileSize    		) ;

		EnhancedFunction ( UWFMessageBox		) ;
		EnhancedFunction ( UWFCreateGauge		) ;
		EnhancedFunction ( UWFSetGaugeValue		) ;
		EnhancedFunction ( UWFCloseGauge		) ;

		EnhancedFunction ( UWFEcho			) ;
		EnhancedFunction ( UWFReemit			) ;

		EnhancedFunction ( UWFInputBox			) ;

		EnhancedFunction ( UWFGetCaption		) ;
		EnhancedFunction ( UWFSetCaption		) ;

		EnhancedFunction ( UWFDialogBox			) ;	
		EnhancedFunction ( UWFCreateWindow		) ;
		EnhancedFunction ( UWFCloseWindow		) ;

		EnhancedFunction ( UWFNextEvent 		) ;
		
		EnhancedFunction ( UWFCheckDlgButton		) ;
		EnhancedFunction ( UWFCheckRadioButton		) ;
		EnhancedFunction ( UWFGetDlgItemText		) ;
		EnhancedFunction ( UWFIsDlgButtonChecked	) ;
		EnhancedFunction ( UWFSendDlgItemMessage	) ;
		EnhancedFunction ( UWFSetDlgItemText 		) ;

		EnhancedFunction ( UWFSetActiveWindow		) ;
		EnhancedFunction ( UWFEnableWindow		) ;
		EnhancedFunction ( UWFGetActiveWindow		) ;
		EnhancedFunction ( UWFEnableControl		) ;

		EnhancedFunction ( UWFSetFocus			) ;
		EnhancedFunction ( UWFGetFocus			) ;
		EnhancedFunction ( UWFSetControlFocus 		) ;
		EnhancedFunction ( UWFGetControlFocus		) ;

		EnhancedFunction ( UWFMessageBeep		) ;
		EnhancedFunction ( UWFInitDialog		) ;

		EnhancedFunction ( UWFSetCursor 		) ;
		EnhancedFunction ( UWFSetIcon			) ;
		EnhancedFunction ( UWFSetCustomIcon		) ;
		EnhancedFunction ( UWFHelp			) ;

                EnhancedFunction ( UWFGetTerminalObjectId 	) ;
		EnhancedFunction ( UWFShowWindow		) ;
		EnhancedFunction ( UWFQueryBaudRate		) ;
		EnhancedFunction ( UWFReceiveFailed		) ;

		EnhancedFunction ( UWFSuperMessage		) ;
		EnhancedFunction ( UWFMenu			) ;

		EnhancedFunction ( UWFEditFile			) ;

		EnhancedFunction ( UWFTimer			) ;
		EnhancedFunction ( UWFInfoLine			) ;

		EnhancedFunction ( UWFAccessFile		) ;
		EnhancedFunction ( UWFExecute			) ;


# if	defined ( CHEOPS )
		EnhancedFunction ( UWFCheopsDialog 		) ;
		EnhancedFunction ( UWFCheopsDialogModify	) ;
		EnhancedFunction ( UWFSetCheopsFunctionKeys	) ;
		EnhancedFunction ( UWFSetCheopsHelp 		) ;
		EnhancedFunction ( UWFSetStatusBarText		) ;
		EnhancedFunction ( UWFCheopsMenu		) ;
# endif	  /* CHEOPS */


	friend class  TObjectListDialog ;

     } ;
