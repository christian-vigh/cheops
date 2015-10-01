/**************************************************************************/
/*                                                                        */
/*   Terminal.h  -  Christian Vigh, 94/09/12.                             */
/*                  (d'après Sébastien George et Christophe Després).     */
/*                                                                        */
/*   Définition des classes application et fenêtre principale.            */
/*                                                                        */
/**************************************************************************/



# define	GetMyApplication()	( ( TTerminalApplication * ) GetApplication ( ) )
# define	XON			17
# define	XOFF			19



/**************************************************************************/
/*                                                                        */
/*  Constantes utilisées pour la compilation :                            */
/*                                                                        */
/*  STATIC_COLOR_DLL :                                                    */
/*      Doit être défini à une valeur non-nulle si la DLL de contrôles    */
/*      statiques en couleur est utilisée.                                */
/*                                                                        */
/**************************************************************************/

# define	STATIC_COLOR_DLL		0




/**************************************************************************/
/*                                                                        */
/*  Constantes relatives aux ressources.                                  */
/*                                                                        */
/**************************************************************************/

# define	TERMINAL_INI			"Terminal.Ini"
# define	TERMINAL_HLP			"Terminal.Hlp"
# define	CHEOPS_HLP			"Cheops.Hlp"



# define	Resource_Base			100

# define	CURSOR_TERMINAL			( Resource_Base +   1 )
# define	CURSOR_LISTBOX			( Resource_Base +   2 )

# define	ICON_TERMINAL			( Resource_Base +   1 )
# define	MENU_TERMINAL			( Resource_Base +   1 )
# define	ACCELERATORS_TERMINAL		( Resource_Base +   1 )


# if	STANDALONE
# define	IDM_File_Open			( Resource_Base +   0 )
# endif

# define	IDM_File_Connect		( Resource_Base +   1 )
# define	IDM_File_Disconnect		( Resource_Base +   2 )
# define	IDM_File_Quit			( Resource_Base +   4 )
# define	IDM_File_Log			( Resource_Base +   5 )
# define	IDM_File_Transfer		( Resource_Base +   6 )
# define	IDM_File_Receive		( Resource_Base +   7 )
# define	IDM_File_Save_Configuration	( Resource_Base +   8 )
# define	IDM_File_Printer_Config		( Resource_Base +   9 )
# define	IDM_File_Print_Page		( Resource_Base +  10 )
# define	IDM_File_Print_Continuous	( Resource_Base +  11 )
# define	IDM_File_Add_Session		( Resource_Base +  12 )
# define	IDM_File_Remove_Session		( Resource_Base +  13 )


# define	IDM_Edit_Copy			( Resource_Base +  20 )
# define	IDM_Edit_Paste			( Resource_Base +  22 )
# define	IDM_Edit_Redo			( Resource_Base +  23 )
# define	IDM_Edit_Select_All		( Resource_Base +  24 )


# define	IDM_Options_Display_All		( Resource_Base +  40 )
# define	IDM_Options_Function_Keys	( Resource_Base +  41 )
# define	IDM_Options_Preferences		( Resource_Base +  42 )
# define	IDM_Options_Configuration	( Resource_Base +  43 )
# define	IDM_Options_Session_Defaults	( Resource_Base +  44 )
# define	IDM_Options_Serial_Defaults	( Resource_Base +  45 )
# define	IDM_Options_Network_Defaults	( Resource_Base +  46 )
# define	IDM_Options_Global		( Resource_Base +  47 )

# define	IDM_Window_Next_Session		( Resource_Base +  60 ) 
# define	IDM_Window_Previous_Session	( Resource_Base +  61 ) 
# define	IDM_Window_Session_List		( Resource_Base +  62 )
# define	IDM_Window_Session_Base		( Resource_Base + 200 )

# define	IDM_Maintenance_Unfreeze	( Resource_Base + 300 )
# define	IDM_Maintenance_Objects		( Resource_Base + 350 )

# define	IDM_Help_About			( Resource_Base + 500 )
# define	IDM_Help_Index			( Resource_Base + 501 )
# define	IDM_Help_Context		( Resource_Base + 502 )

# define	IDM_Lambda_Shortcut		( Resource_Base + 600 )


/**************************************************************************/
/*                                                                        */
/*  Autres constantes :                                                   */
/*                                                                        */
/*  WM_DATARECEIVED :                                                     */
/*      Message envoyé à la fenêtre principale lorsque des caractères ont */
/*      été reçus.                                                        */
/*                                                                        */
/*  WM_SENDDATA :                                                         */
/*      Lorsque des données trop importantes diovent être envoyées, elles */
/*      sont bufferisées et, à intervalles réguliers, un message de ce    */
/*      type est émis par TConnection vers la fenêtre de destination pour */
/*      que cette dernière envoie une partie de ces données.              */
/*                                                                        */
/*  WM_SETICON :                                                          */
/*      Indique à une fenêtre qu'elle doit modifier son icône standard.   */
/*                                                                        */
/*  WM_QUERYHELPCONTEXT :                                                 */
/*      Envoyé à une fenêtre lorsqu'une aide contextuelle a été demandée, */
/*      mais qu'aucune entrée dans la table des chaînes de contexte n'a   */
/*      été trouvée. LParam pointe alors sur une structure THelpEntry. .  */
/*      La fonction doit alors renseigner le champ correspondant à la     */
/*      chaîne contextuelle, et retourner le groupe auquel il appartient. */
/*                                                                        */
/**************************************************************************/

# define	WM_DATARECEIVED			( WM_USER + 100 )
# define	WM_SENDDATA			( WM_USER + 101 )
# define	WM_USERIDLE			( WM_USER + 102 )
# define	WM_UNIXMESSAGE			( WM_USER + 103 )
# define	WM_SETICON			( WM_USER + 104 )
# define	WM_QUERYHELPCONTEXT		( WM_USER + 105 )



/**************************************************************************/
/*                                                                        */
/*  Variables globales.                                                   */
/*                                                                        */
/**************************************************************************/

class	TTerminalApplication ;
extern  TTerminalApplication *		TerminalApplication ;

class   TTerminalWindow	;
extern  TTerminalWindow *		TerminalWindow ;

class	TDisplayDevice ;
extern  TDisplayDevice *		Device ;

class	TConnection ;
extern  TConnection *			Connection ;

class	TUnixApplicationTable ;
extern  TUnixApplicationTable *		ApplicationTable ;

class	TConfiguration ;
extern	TConfiguration *		Configuration ;

class	TCheopsConfiguration ;
extern	TCheopsConfiguration *		CheopsConfiguration ;


class	TConnection ;
extern	TConnection *			Connection ;

class	THelp ;
extern	THelp *				Help ;

extern  TPrinter *	  		Printer ;

extern  char				TerminalIniFileName [] ;


/**************************************************************************/
/*                                                                        */
/*   TxAbort -                                                            */
/*      Classe utilisée uniquement pour lancer une exception qui          */
/*      permettra de sortir du programme en catastrophe, même si          */
/*      certaines variables globales ne sont pas encore initialisées.     */
/*                                                                        */
/**************************************************************************/

class	TXAbort   :  public  TXOwl
   {
	public :
		typedef enum
		   {
				NormalExit,
				NoIniFile,
				NoConfiguration,
				NoSession
		     }  TXAbortReason ;


	private :
		TXAbortReason		Reason ;
		char *			Message ;


	public :
		TXAbort ( TXAbortReason		reason,
			  char *		msg 	=  NULL )
				: TXOwl ( string ( msg ) )
		   {
			Message =  msg ;
			Reason 	=  reason ;
		    }


		TXAbortReason	GetReason ( void )
		   {
			return ( Reason ) ;
		    }


		virtual void	Throw  ( void )
		   {
			THROW ( * this ) ;
		    }


		virtual int	Unhandled ( TModule *, unsigned int )
		   {
			:: MessageBox ( NULL, Message, "Erreur de configuration",
				MB_OK | MB_TASKMODAL ) ;
			return ( -1 ) ;
		    }


		virtual TXOwl *	 Clone  ( void )
		   {
			return  ( new  TXAbort ( * this ) ) ;
		    }
      } ;







/**************************************************************************/
/*                                                                        */
/*  Définition de la classe Application.                                  */
/*                                                                        */
/**************************************************************************/


class	TTerminalApplication	:  public  TApplication
    {
	public :
		TTerminalApplication	( LPSTR  	name ) ;

		virtual void	InitMainWindow		( void ) ;
		virtual int	Run			( void ) ;

		virtual int	IdleAction		( long int ) ;

		void		ScanParameterList	( int  		argc,
							  char **	argv ) ;
     } ;





/**************************************************************************/
/*                                                                        */
/*  Constantes pour la classe TConfiguration.                             */
/*                                                                        */
/**************************************************************************/


# define	MAX_SESSIONS			16

# define	SETTINGS_SECTION		"Settings"
# define	DEFAULTS_SECTION		"Defaults"
# define	CONFIGURATION_SECTION		"(Configuration) "
# define	SESSION_SECTION			"(Session) "
# define	TERMINAL_INI			"Terminal.Ini"


class		TConnection ;



typedef enum			// Pour les conversions ascii d'Unix vers Windows
	{
		AnsiCharSet,
		OemCharSet
	 }  THostCharSet ;



typedef enum			// Type des contrôles utilisés dans les boîtes de dialogue
   {
		Borland,
		Microsoft,
		Microsoft3D
     }  TControlStyle ;
		



/**************************************************************************/
/*                                                                        */
/*  Les classes TNetworkParameters et TSerial renferment les    	  */
/*  paramètres relatifs à une liaison réseau ou série.                    */
/*                                                                        */
/**************************************************************************/

typedef struct
   {
		char		ServerName [ MAX_PARAMETER_LENGTH ] ;
		int		Delay ;
    }  TNetworkParameters ;


typedef struct
   {
		char		PortName   [ MAX_PARAMETER_LENGTH ] ;
		long int	Speed,
				RealSpeed ;
		int		Parity,
				DataBits,
				StopBits ;
    }  TSerialParameters ;



class	TTerminalWindowClient ;

typedef struct 
   {
		BOOL				NetworkLink ;
		TConnection *			Connection ;
		TDisplayDevice *        	Device ;
		TTerminalWindowClient *		TerminalWindowClient ;
		char				UserName     [ MAX_PARAMETER_LENGTH ],
						UserPassword [ MAX_PARAMETER_LENGTH ] ;
		
		union
		   {
			TNetworkParameters	Network ;
			TSerialParameters	Serial ;
		    } ;

     }  TConnectionParameters ;





/**************************************************************************/
/*                                                                        */
/*   La structure  TSessionParameters contient l'ensemble des paramètres  */
/*   (autres que ceux définissant la connexion) relatifs à une session.   */
/*                                                                        */
/**************************************************************************/

typedef struct
   {
		char		WindowTitle     [ MAX_PARAMETER_LENGTH ] ;
		char		FontName  	[ MAX_PARAMETER_LENGTH ] ;
		int		FontWidth,
				FontHeight ;
		COLORREF	BackgroundColor,
				ForegroundColor ;
		char		Emulation	[ MAX_PARAMETER_LENGTH ] ;
		BOOL		LocalEcho,
				TransparentMode ;
		THostCharSet	HostCharSet ;
		BOOL		Opened ;
		BOOL		LocalSession ; 	// TRUE si la session a été ajoutée
     }  TSessionParameters ;




/**************************************************************************/
/*                                                                        */
/*  La structure TTerminalParameters contient les paramètres relatifs à   */
/*  l'application en général.                                             */
/*                                                                        */
/**************************************************************************/

# define	IDR_DEFAULT_CONFIGURATION		100
# define	IDR_DEFAULT_EMULATION			101
# define	IDT_PARAMETERS				"PARAMETERS"



typedef struct
   {
		char		ApplicationDirectory [ MAX_PARAMETER_LENGTH ] ;
		char		IniFileName	     [ MAX_PARAMETER_LENGTH ] ;
		char		Configuration	     [ MAX_PARAMETER_LENGTH ] ;
		char		SessionSections	     [ MAX_SESSIONS ] [ MAX_PARAMETER_LENGTH ] ;
		BOOL		LambdaUser ;
		BOOL		LambdaUserShortcut ;
		BOOL		AlarmBeep ;
		int		MinRedoSize ;
		int		MenuAutoOpenDelay ;
		BOOL		AutoOpen ;
		BOOL		ConfirmOnExit ;
		BOOL		FormFeedOnHardcopy ;
		BOOL		FrameHardcopy ;
		BOOL		LastExitWasOk ;
		int		LastX, LastY ;

# if  defined ( CHEOPS )
		BOOL		OldFunctionKeys ;
# endif

    }   TTerminalParameters ;
		





/**************************************************************************/
/*                                                                        */
/*  La classe TConfiguration gère la mise à jour du fichier .INI,         */
/*  ainsi que le paramétrage des sessions.                                */
/*                                                                        */
/**************************************************************************/

class	 TConfiguration
   {
	public :
		typedef enum			// Etat d'une session
		   {
				Free,		// L'élément du tableau est libre
				Created,	// La session a été créée
				Opened		// La session est ouverte
		    }  TSessionState ;


		typedef enum       		// Statut retourné par Read
		   {
				Ok,		// Tout s'est bien passé
				NoIniFile,	// Fichier .INI non trouvé
				NoConfiguration,// Pas de configuration par défaut
				NoSession	// Pas de sessions pour cette configuration
		    }  TReadStatus ;

		


	private :
		TTerminalParameters 		TerminalParameters ;
		TSessionParameters 		DefaultSessionParameters ;
		TConnectionParameters 		DefaultNetworkParameters,
						DefaultSerialParameters ;
		BOOL				DefaultNetworkLink ;

		TSessionParameters  		Sessions       [ MAX_SESSIONS ] ;
		TConnectionParameters 		Connections    [ MAX_SESSIONS ] ;
		TSessionState 			SessionStates  [ MAX_SESSIONS ] ;
		int				CurrentSession ;

		BOOL				Loaded ;
		BOOL				Modified ;
		BOOL				PreventAutoOpen ;

	public :
		FILE *				LogFileFP ;
		char				LogFileName  [ MAX_PARAMETER_LENGTH ] ;
		char				HelpFileName [ MAX_PARAMETER_LENGTH ] ;

# if	defined ( CHEOPS )
		char				CheopsHelpFileName [ MAX_PARAMETER_LENGTH ] ;
# endif



	private :
		void		CreateConnection ( int	session ) ;


	public :

	// Constructeur par défaut pour permettre la déclaration en global
		TConfiguration  ( )
		   {
			Initialize ( ) ;
		     }


	// Libération éventuelle des connexions et devices par le destructeur
	       ~TConfiguration  ( ) 
		   {
			FreeResources ( ) ;
		    }	


		void	Initialize  	( void ) ;
		void	FreeResources   ( void ) ;


	// Accès aux paramètres du .INI
		void		GetApplicationDirectory  ( char *  appdir ) ;
		BOOL		GetConfigurationPath	 ( char *  path ) ;
		void		ConvertEmulationPath     ( char *  path,
							   BOOL    absolute ) ;


		void		GetSectionList  ( char *   	buffer,
						  int	        buffer_size ) ;
		void		GetSectionItems ( char *	section,
						  char *	buffer,
						  int		buffer_size ) ;
		BOOL		GetBoolean	( char *	section,
						  char *	key,
						  BOOL		defval ) ; 

		BOOL		IsNetworkLink	( char *	section ) ;

		int		ReadString	( char *	section,
						  char *	key,
						  char *	defval,
						  char *	buffer,
						  int		buffer_size,
						  BOOL		is_default_entry 	=  TRUE ) ;
						
		int		WriteString	( char *	section,
						  char *	key,
						  char *	buffer ) ;

		BOOL		CreateDefaultINIFile   ( void ) ;
		BOOL		CreateDefaultEmulation ( void ) ;

						

	// Chargement/écriture des paramètres du .INI
		void		SetDirty	( BOOL    dirty		 =  TRUE )
		   {
			Modified = dirty ;
		    }

		BOOL		HasAccess ( void )
		   {
			return  ( ! TerminalParameters. LambdaUser   ||
				  TerminalParameters. LambdaUserShortcut ) ;
		    }

		BOOL		IsDirty   ( void ) 
		   {
			return  ( Modified ) ;
		    }


		BOOL		IsLoaded  ( void )
		  {
			return ( Loaded ) ;
		   }


		void		SetPreventAutoOpen  ( BOOL   value )
		   {
			PreventAutoOpen = value ;
		    }

		BOOL		GetPreventAutoOpen  ( void )
		   {
			return ( PreventAutoOpen ) ;
		    }


		TReadStatus	Read   			 ( char *  configname     =  NULL ) ;
		void		Write  			 ( BOOL    forced	  =  FALSE ) ;

		void		ReadConnectionParameters ( char *  		     section,
							   TConnectionParameters &   p,
							   BOOL	   	   	     network_link,
							   BOOL			     is_default ) ;
		void		ReadSessionParameters    ( char *		     section,
							   TSessionParameters &	     s,
							   BOOL			     is_default ) ;

		void		WriteConnectionParameters( char *		     section,
							   TConnectionParameters &   p,
							   BOOL			     network_link,
							   BOOL			     isdef  ) ;
		void		WriteSessionParameters   ( char *		     section,
							   TSessionParameters &      s,
							   BOOL			     isdef  ) ;

							  


	// Gestion des ajouts
		int		FindFreeSlot	( void ) ;
		int		AddSession  	( char *		   name,
						  TConnectionParameters &  cp,
						  TSessionParameters *     sp  =  NULL ) ;
		int		RemoveSession   ( char *		   session_name ) ;
		int		RemoveSession   ( int			   session_index ) ;
		void		SwapSessions    ( int			   sess1,
						  int			   sess2 ) ;



	// Accès aux paramètres des sessions
		int	GetSessionCount    	( void ) ;
		int	GetOpenedSessionCount   ( void ) ;
		int	GetCurrentSession  	( void )
		   {
			return  ( CurrentSession ) ;
		    }

		char *		SessionName  ( int  index  =  -1 )
		   {
			if  ( index  ==  -1 )
				index = CurrentSession ;
			return  ( TerminalParameters. SessionSections [ index ] ) ;
		     }


		char *		SessionTitle  ( int  index  =  -1 )
		   {
			if  ( index  ==  -1 )
				index = CurrentSession ;
			if  ( Sessions [ index ]. WindowTitle [0] )
				return ( Sessions [ index ]. WindowTitle ) ;
			else
				return ( TerminalParameters. SessionSections [ index ] ) ;
		    }


		TSessionParameters &	Session  ( int   index   =  -1 )
		   {
			if  ( index  ==  -1 )
				index = CurrentSession ;
			return  ( Sessions [ index ] ) ;
		    }


		TConnectionParameters &  Connection  ( int  index  =  -1 )
		   {
			if  ( index  ==  -1 )
				index = CurrentSession ;
			return ( Connections [ index ] ) ;
		    }



		TSessionParameters &	DefaultSession  ( void )
		   {
			return  ( DefaultSessionParameters ) ;
		    }

		TConnectionParameters &  DefaultConnection  ( BOOL   network_link = FALSE )
		   {
			return ( ( network_link ) ?
					DefaultNetworkParameters : DefaultSerialParameters ) ;
		    }


		TTerminalParameters &   Terminal  ( void )
		   {
			return  ( TerminalParameters ) ;
		    }


	// Construction des noms de configuration et de session
		char *		ConfigurationSectionName  ( char *  name ) ;
		char *		SessionSectionName        ( char *  name ) ;


	// Changement de session courante
		void		SetCurrentSession  ( int  session, BOOL  update = TRUE ) ;
		void		SetNextSession	   ( void ) ;
		void		SetPreviousSession ( void ) ;


	// Connexion / déconnexion
		int		Open  		 ( int 	session  =  -1 ) ;
		int		Close 		 ( int	session  =  -1 ) ;
		void		CloseAll	 ( void ) ;

		BOOL		IsOpened	 ( int  session  =  -1 ) 
		   {
			if  ( session  ==  -1 )
				session = CurrentSession ;
			return  ( SessionStates [ session ]  ==  Opened ) ;
		    }


		TSessionState	SessionState ( int  session  =  -1 ) 
		   {
			if  ( session  ==  -1 )
				session = CurrentSession ;
			return  ( SessionStates [ session ] ) ;
		    }
    } ;



extern 	TSessionParameters *		SessionParameters ;
extern	TConnectionParameters *		ConnectionParameters ;


