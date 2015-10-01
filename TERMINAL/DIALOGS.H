/**************************************************************************/
/*                                                                        */
/*  Dialogs.h  -  Christian Vigh, 94/09/19.                               */
/*                                                                        */
/*  Classes pour la gestion des boîtes de dialogue de l'application.      */
/*                                                                        */
/**************************************************************************/

# define	Dialog_Base  			( Resource_Base + 1000 )


/***
	Constantes.
 ***/

# define	IDD_FUNCTION_KEYS      		( Dialog_Base +   2 )	
# define	IDD_REDO	       		( Dialog_Base +   3 )
# define	IDD_APPLICATION_LIST		( Dialog_Base +   4 )
# define	IDD_DOSUNIX_TRANSFER		( Dialog_Base +   5 )
# define	IDD_UNIXDOS_TRANSFER		( Dialog_Base +   6 )
# define	IDD_OPEN_CONFIGURATION		( Dialog_Base +   7 )
# define	IDD_MODIFY_CONFIGURATION	( Dialog_Base +   8 )
# define	IDD_MODIFY_SESSIONS		( Dialog_Base +   9 ) 
# define	IDD_SESSION_PARAMETERS		( Dialog_Base +  10 )
# define	IDD_SESSION_PARAMETERS_NETWORK	( Dialog_Base +  11 )
# define	IDD_SESSION_PARAMETERS_SERIAL	( Dialog_Base +  12 )
# define	IDD_PREFERENCES			( Dialog_Base +  13 )
# define	IDD_ABOUT			( Dialog_Base +  14 )
# define	IDD_SESSION_LIST		( Dialog_Base +  15 )
# define	IDD_GENERAL			( Dialog_Base +  16 )
# define	IDD_OBJECT_LIST			( Dialog_Base +  17 )
# define	IDD_PRINTING			( Dialog_Base +  18 )


# define	IDC_KEY_F1			( Dialog_Base + 100 )	
# define	IDC_KEY_F2			( Dialog_Base + 101 )
# define	IDC_KEY_F3			( Dialog_Base + 102 )
# define	IDC_KEY_F4			( Dialog_Base + 103 )
# define	IDC_KEY_F5			( Dialog_Base + 104 )
# define	IDC_KEY_F6			( Dialog_Base + 105 )
# define	IDC_KEY_F7			( Dialog_Base + 106 )
# define	IDC_KEY_F8			( Dialog_Base + 107 )
# define	IDC_KEY_F9			( Dialog_Base + 108 )
# define	IDC_KEY_F10			( Dialog_Base + 109 )
# define	IDC_KEY_F11			( Dialog_Base + 110 )
# define	IDC_KEY_F12			( Dialog_Base + 111 )


# define	IDC_REDO_LIST			( Dialog_Base +   0 )	

# define	IDC_APPLICATION_LIST		( Dialog_Base +   0 )	
# define	IDC_APP_KILL			( Dialog_Base +   2 )

# define	IDC_TRANSFER_CHOOSE		( Dialog_Base +   0 )	
# define	IDC_DOS_FILE			( Dialog_Base +   1 )
# define	IDC_UNIX_FILE			( Dialog_Base +   2 )
# define	IDC_BINARY_TRANSFER		( Dialog_Base +   3 )


# define	IDC_CONFIGURATION_LIST		( Dialog_Base +   0 ) 	
# define	IDC_MODIFY			( Dialog_Base +   1 )
# define	IDC_CREATE			( Dialog_Base +   2 )
# define	IDC_DELETE			( Dialog_Base +   3 )
# define	IDC_DEFAULT			( Dialog_Base +   4 )
# define	IDC_LISTBOX_TITLE		( Dialog_Base +   5 ) 

# define	IDC_SESSION_NAME		( Dialog_Base +   0 )
# define	IDC_SERVER_NAME			( Dialog_Base +   1 )
# define	IDC_LOCAL_ECHO			( Dialog_Base +   2 )
# define	IDC_PORT			( Dialog_Base +   3 )
# define	IDC_SPEED			( Dialog_Base +   4 )
# define	IDC_PARITY_NONE			( Dialog_Base +   5 )
# define	IDC_PARITY_EVEN			( Dialog_Base +   6 )
# define	IDC_PARITY_ODD			( Dialog_Base +   7 )
# define	IDC_DATA_BITS_8			( Dialog_Base +   8 )
# define	IDC_DATA_BITS_6			( Dialog_Base +   9 )
# define	IDC_DATA_BITS_7			( Dialog_Base +  10 )
# define	IDC_DATA_BITS_5			( Dialog_Base +  11 ) 
# define	IDC_STOP_BIT_1			( Dialog_Base +  12 )
# define	IDC_STOP_BIT_15			( Dialog_Base +  13 )
# define	IDC_STOP_BIT_2			( Dialog_Base +  14 )
# define	IDC_DELAY			( Dialog_Base +  15 )
# define	IDC_NETWORK_TYPE		( Dialog_Base +  16 ) 
# define	IDC_SERIAL_TYPE			( Dialog_Base +  17 ) 
# define	IDC_PREFERENCES			( Dialog_Base +  18 )


# define	IDC_WINDOW_TITLE		( Dialog_Base +   0 )
# define	IDC_FONT_NAME			( Dialog_Base +   1 )
# define	IDC_FONT_HEIGHT			( Dialog_Base +   2 )
# define	IDC_BACKGROUND_COLOR		( Dialog_Base +   3 )
# define	IDC_FOREGROUND_COLOR		( Dialog_Base +   4 )
# define	IDC_EXAMPLE			( Dialog_Base +   5 )
# define	IDC_CHOOSE_FONT			( Dialog_Base +   6 )
# define	IDC_EMULATION			( Dialog_Base +   7 )
# define	IDC_CHOOSE_EMULATION		( Dialog_Base +   8 )
# define	IDC_OEM_CHARSET			( Dialog_Base +   9 )
# define	IDC_ANSI_CHARSET		( Dialog_Base +  10 )

# define	IDC_VERSION			( Dialog_Base +   0 )
# define	IDC_COPYRIGHT			( Dialog_Base +   1 )
# define	IDC_DEBUG			( Dialog_Base +   2 )
# define	IDC_DATE			( Dialog_Base +   3 )

# define	IDC_SESSION_LIST		( Dialog_Base +   0 ) 
# define	IDC_CHOOSE_SESSION		( Dialog_Base +   1 )
# define	IDC_OPEN_SESSION		( Dialog_Base +   2 )
# define	IDC_CLOSE_SESSION		( Dialog_Base +   3 ) 


# define	IDC_ALARM_BEEP			( Dialog_Base +   0 ) 
# define	IDC_MIN_REDO_SIZE    		( Dialog_Base +   1 )
# define	IDC_AUTO_OPEN			( Dialog_Base +   2 )
# define	IDC_CONFIRM_ON_EXIT		( Dialog_Base +   3 )
# define	IDC_FF_ON_HARDCOPY		( Dialog_Base +   4 )
# define	IDC_FRAME_HARDCOPY		( Dialog_Base +   5 )

# if	defined ( CHEOPS )
#    define	IDC_OLD_FUNCTION_KEYS		( Dialog_Base +   6 )
#    define     IDC_NEW_FUNCTION_KEYS		( Dialog_Base +   7 )
#	define 	IDC_AUTOOPEN_DELAY		( Dialog_Base +   8 )
# endif


# define	IDC_OBJECT_LIST			( Dialog_Base +   0 )
# define	IDC_OBJECT_COUNT		( Dialog_Base +   1 )

# define	IDC_PRINTING_SLIDER		( Dialog_Base +   0 )



/**************************************************************************/
/*                                                                        */
/*   Classe  TRedoDialog                                                  */
/*      Permet de choisir une commande parmi celles déjà saisies.         */
/*                                                                        */
/**************************************************************************/

class	TRedoDialog 	:  public  TDialog
   {
	private :
		char *			OutputBuffer ;


	public	:
		TRedoDialog     ( TTerminalWindow *	window,
				  char *		output )
			: TDialog ( window, IDD_REDO )
		   {
			OutputBuffer  = output ;
		    }


		virtual BOOL	EvInitDialog    ( HWND 	hwnd ) ;
		void		CmOk 		( void ) ;

		void		CmCancel	( void )
		   {
			TDialog :: CmCancel ( ) ;
		    }


	DECLARE_RESPONSE_TABLE  ( TRedoDialog ) ;
    } ;




/**************************************************************************/
/*                                                                        */
/*  Classe TApplicationListDialog :                                       */
/*      Permet de choisir, à la manière du gestionnaire de tâches, une    */
/*      des applications Unix.                                            */
/*                                                                        */
/**************************************************************************/

class TUnixApplication ;

class TApplicationTableDialog	:  public  TDialog
    {
	private :
		TUnixApplication **	Applications ;
		int			ApplicationCount ;
		int *			UserChoice ;
		BOOL *			Kill ;


	public :
		TApplicationTableDialog  ( TTerminalWindow *	parent,
					   TUnixApplication **	table,
					   int			table_size,
					   int *		chosen,
					   BOOL *		kill )
			: TDialog ( parent, IDD_APPLICATION_LIST )
			{
			Applications 		=  table ;
			ApplicationCount	=  table_size ;
			UserChoice		=  chosen ;
		    }


		int	GetApplicationIndex     ( int	selected ) ;
		BOOL	EvInitDialog 		( HWND ) ;
		void	CmOk            	( void ) ;
		void	CmKill 			( void ) ;

		void 	CmCancel		( void )
		  {
			TDialog :: CmCancel ( ) ;
			}  ;


	DECLARE_RESPONSE_TABLE ( TApplicationTableDialog ) ;
       } ;




/**************************************************************************/
/*                                                                        */
/*  Classe TDosUnixTransferDialog :                                       */
/*      Initialise un transfert de fichier de Dos vers Unix.              */
/*                                                                        */
/**************************************************************************/


class TDosUnixTransferDialog	:  public  TDialog
    {
	public :
		typedef enum
		   {
				Send,
				Receive
		    }  TTransferDirection ;


	private :
		char			DosFile   [ MAX_PARAMETER_LENGTH ],
					UnixFile  [ MAX_PARAMETER_LENGTH ] ;
		BOOL			Binary ;
		TTransferDirection      Direction ;


	public :
		TDosUnixTransferDialog ( TTerminalWindow *  window,
					 TTransferDirection dir )
			: TDialog ( window, ( dir  ==  Send ) ?
						IDD_DOSUNIX_TRANSFER :
						IDD_UNIXDOS_TRANSFER )
		   {
			* DosFile  = * UnixFile = 0 ;
			Binary     = FALSE ;
			Direction  = dir ;
		    }


		BOOL		EvInitDialog		( HWND ) ;
		virtual void 	CmOk            	( void ) ;
		virtual void   	CmChoose		( void ) ;

		void 		CmCancel		( void )
		   {
			TDialog :: CmCancel ( ) ;
		    }  ;

	DECLARE_RESPONSE_TABLE ( TDosUnixTransferDialog ) ;
       } ;






/**************************************************************************/
/*                                                                        */
/*   Classe TExtendedListBox -                                            */
/*      Gère une listbox où l'on peut déplacer les élements en les faisant*/       
/*      glisser avec la souris.                                           */
/*                                                                        */
/**************************************************************************/

class	TExtendedListBox  	: public  TListBox
   {
	private :
		BOOL			ExtendedInterfaceEnabled ;
		BOOL			ButtonPressed ;
		HCURSOR			SavedCursor,
					ListBoxCursor ;
		int			SelectedElement ;


	public :
		TConfiguration *        DialogConfiguration ;



	public :
		TExtendedListBox  ( TWindow *  parent, int  id )
			: TListBox ( parent, id ) 
		   {
			
			ExtendedInterfaceEnabled = FALSE ;
			ButtonPressed		 = FALSE ;
		    }




		void	EnableExtendedInterface ( BOOL  state  =  TRUE )
		   {
			ExtendedInterfaceEnabled = state ;
		    }


		
		
		void	EvLButtonUp   ( uint  flags, TPoint &  position ) ;
		void	EvLButtonDown ( uint  flags, TPoint &  position ) ;


	DECLARE_RESPONSE_TABLE ( TExtendedListBox ) ;
    } ;




/**************************************************************************/
/*                                                                        */
/*  TOpenConfigurationDialog :                                            */
/*      Choix d'une session à ouvrir.                                     */
/*                                                                        */
/**************************************************************************/

class	TOpenConfigurationDialog  	:  public  TDialog
   {
	protected :
		char *			ConfigurationName ;
		TExtendedListBox *	ListBox ;
	

	public :
		TOpenConfigurationDialog    ( TWindow *   parent,
					      char *	  name,
					      int	  dialogid  =  IDD_OPEN_CONFIGURATION )
			: TDialog ( parent, dialogid )
		   {
			ConfigurationName = name ;
			* ConfigurationName = 0 ;
			ListBox = new  TExtendedListBox ( this, IDC_CONFIGURATION_LIST ) ;
		    }


	       ~TOpenConfigurationDialog ( )
		   {
			delete  ListBox ;
		    }
	

		virtual void	CmOk  			( void ) ;
		virtual BOOL	EvInitDialog		( HWND ) ;
		void		UpdateListBox		( void ) ;



	// Retourne la chaîne actuellement sélectionnée dans la boiboite
		BOOL	GetCurrentSelection  ( char *   buffer )
		   {
			register int		i  =  ListBox -> GetSelIndex ( ) ;


			if  ( i < 0 )
				return ( FALSE ) ;
			else
			   {
				ListBox -> GetSelString ( buffer, MAX_PARAMETER_LENGTH ) ;
				return ( TRUE ) ;
			    }
		     }


	
	// Retourne le préfixe utilisé dans le nom de section des configurations.
	// La fonction est 'virtual' pour permettre à TSessionsDialog de retourner un
	// prefixe différent
	// Cette fonction est utilisée dans UpdateListBox.
		virtual char *	SectionPrefix ( void )
		   {
			return ( CONFIGURATION_SECTION ) ;
		    }


	// Permet d'effectuer des traitements selon le nombre d'éléments trouvés
	// pour les configurations ou les sessions. Typiquement, si aucun élément
	// n'a été trouvé, il faudra mettre disable les boutons "Modifier" et
	// "Supprimer"
		virtual void	ElementsFound  ( int  count )
		   {
			register BOOL		Enable = ( count != 0 ) ;
			register HWND           ButtonHwnd ;
			register BOOL		State ;


			if  ( ( ButtonHwnd = GetDlgItem ( IDC_MODIFY ) )  !=  NULL )
				:: EnableWindow ( ButtonHwnd, Enable ) ;

			if  ( ( ButtonHwnd = GetDlgItem ( IDC_DELETE ) )  !=  NULL )
				:: EnableWindow ( ButtonHwnd, Enable ) ;

			if  ( ( ButtonHwnd = GetDlgItem ( IDC_DEFAULT ) )  !=  NULL )
			   {
				if  ( ! Configuration -> HasAccess ( ) )
					State = FALSE ;
				else
					State = ( ListBox -> GetCount ( )  <=  0 ) ?  
							FALSE : TRUE ;
				:: EnableWindow ( ButtonHwnd, State ) ;
			     }
		    }			
	

	DECLARE_RESPONSE_TABLE ( TOpenConfigurationDialog ) ;
     } ;





/**************************************************************************/
/*                                                                        */
/*  TConfigurationDialog :                                                */
/*      Gestion des configurations.                                       */
/*                                                                        */
/**************************************************************************/

class	TConfigurationDialog  	:  public  TOpenConfigurationDialog
   {
	private :
		char 			ConfigurationName [ MAX_PARAMETER_LENGTH ] ;
		TConfiguration *        DialogConfiguration ;
		BOOL			CurrentWasModified ;
	

	public :
		TConfigurationDialog    ( TWindow *   parent )
			: TOpenConfigurationDialog ( parent, ConfigurationName, 
				IDD_MODIFY_CONFIGURATION )
		   {
			DialogConfiguration =  new  TConfiguration ;
			DialogConfiguration -> SetPreventAutoOpen ( TRUE ) ;
			CurrentWasModified  =  FALSE ;
		    }


	       ~TConfigurationDialog ( )
		   {
			delete  DialogConfiguration ;
		    }	


		void		CmOk  		( void ) ;
		void		CmCreate	( void ) ;
		void		CmDelete	( void ) ;
		void 		CmModify	( void ) ;
		void		CmDefault 	( void ) ;


	// Retourne true si 'name' est le nom de la configuration en cours		
		BOOL	IsCurrentConfiguration  ( char *  name )
		   {
			return  ( ! stricmp ( name, Configuration -> Terminal ( ). Configuration ) ) ;
		    }


	DECLARE_RESPONSE_TABLE ( TConfigurationDialog ) ;
     } ;






/**************************************************************************/
/*                                                                        */
/*  TSessionsDialog :   i                                                 */
/*      Modification des sessions d'une configuration.                    */
/*                                                                        */
/**************************************************************************/

class	TSessionsDialog  	:  public  TOpenConfigurationDialog
   {
	private :
		char			SessionName [ MAX_PARAMETER_LENGTH ] ;
		TConfiguration *	DialogConfiguration ;
	

	public :
		TSessionsDialog    ( TWindow *   	parent,
				     TConfiguration *	config ) 
			: TOpenConfigurationDialog ( parent, SessionName, IDD_MODIFY_SESSIONS )
		   {
			DialogConfiguration		= config ;
			ListBox -> DialogConfiguration  = config ;
			ListBox -> EnableExtendedInterface ( ) ;
		    }           


		void	CmOk  			( void ) ;
		void	CmCreate		( void ) ;
		void	CmDelete		( void ) ;
		void	CmModify		( void ) ;
		BOOL	EvInitDialog		( HWND ) ;
		void	UpdateListBox 		( void ) ;

		int	SessionIndex		( char *  name ) ;


	// Le prefixe des noms de section pour les sessions est différent de celui des
	// configurations
		virtual char *		SectionPrefix ( void )
		   {
			return  ( SESSION_SECTION ) ;
		    }
	

	DECLARE_RESPONSE_TABLE ( TSessionsDialog ) ;
     } ;





/**************************************************************************/
/*                                                                        */
/*  TSessionParametersDialog :                                            */
/*      Classe pour la gestion de la boîte de dialogue "Paramètres de     */
/*      session".                                                         */
/*      Il y a en réalité trois boîtes de dialogue :                      */
/*                                                                        */
/*      . La première, accessible en création uniquement, permet de       */
/*        spécifier le type de liaison.                                   */
/*      . Les deux autres la remplacent selon le type de liaison choisie  */
/*        (réseau ou série).                                              */
/*                                                                        */
/**************************************************************************/

class	TSessionParametersDialog :  public  TDialog
   {
	private :
		TConfiguration *        DialogConfiguration ;
		TConnectionParameters * LocalConnectionParameters ;
		TSessionParameters *	LocalSessionParameters ;
		int			SessionIndex ;
		int			DialogId ;
		char *			SessionName ;
		TEdit *			NetworkDelay ;
		TListBox *		ParentListBox ;
		TComboBox *		Port,
			  *		Speed ;



	public :
		TSessionParametersDialog  ( TWindow *			parent,
					    int				dialog_id,
					    TConfiguration *		config,
					    TConnectionParameters *     cp,
					    TSessionParameters *	sp,
					    int				session_index,
					    char *			session_name,
					    TListBox *			lb  =  NULL )
			:  DialogConfiguration       ( config ),
			   DialogId		     ( dialog_id ),
			   LocalConnectionParameters ( cp ),
			   LocalSessionParameters    ( sp ),
			   SessionIndex		     ( session_index ),
			   SessionName		     ( session_name ),
			   NetworkDelay		     ( NULL ),
			   Port			     ( NULL ),
			   Speed 		     ( NULL ),
			   TDialog ( parent, dialog_id )
		   { 
			ParentListBox 	= lb ;

			if  ( DialogId  !=  IDD_SESSION_PARAMETERS )
			   {
				if  ( LocalConnectionParameters -> NetworkLink )
				   {
					NetworkDelay = new  TEdit ( this, IDC_DELAY ) ;
					NetworkDelay -> SetValidator ( new  TFilterValidator ( "0-9" ) ) ;
				     }
				else
				   {
					Port   = new  TComboBox ( this, IDC_PORT ) ;
					Speed  = new  TComboBox ( this, IDC_SPEED ) ;
				    }
			     }
		    }
		


	       ~TSessionParametersDialog ( )
		   {
			if  ( NetworkDelay  !=  NULL )
				delete  NetworkDelay ;

			if  ( Port  !=  NULL )
				delete  Port ;

			if  ( Speed  !=  NULL )
				delete  Speed ;
		    }	
						

		void	CmLinkType 		( WPARAM  id ) ;
		void	CmChanged		( void ) ;
		void	CmOk			( void ) ;
		void	CmPreferences		( void ) ;
		BOOL	EvInitDialog		( HWND  focus ) ;


		void	EnableControl	( int  id, BOOL  state )
		   {
			register HWND	hwnd = GetDlgItem ( id ) ;

			if  ( hwnd  !=  NULL )
				:: EnableWindow ( hwnd, state ) ;
		    }

	DECLARE_RESPONSE_TABLE ( TSessionParametersDialog ) ;
    } ;





/**************************************************************************/
/*                                                                        */
/*  TPreferencesDialog :                                                  */
/*      Gère la boîte de dialogue "Préférences" pour une session.         */
/*                                                                        */
/**************************************************************************/

class	TPreferencesDialog 	:  public  TDialog
   {
	private :
		TSessionParameters *	ReturnParameters ;
		TSessionParameters	Parameters ;
		HFONT			ExampleFont ;


	public :
		TPreferencesDialog  ( TWindow *			parent,
				      TSessionParameters *      params )
			: TDialog ( parent, IDD_PREFERENCES )
		   {
			ReturnParameters = params ;
			Parameters = * ReturnParameters ;
			ExampleFont = NULL ;
		    }


	       ~TPreferencesDialog  ( )
		   {
			if  ( ExampleFont  !=  NULL )
				DeleteObject ( ExampleFont ) ;
		    }	


		BOOL	EvInitDialog 		( HWND		focus ) ;
		void	CmOk			( void ) ;
		void	CmChooseFont		( void ) ;
		void	CmChooseEmulation       ( void ) ;
		void	CmColor			( WPARAM	id ) ;
		HBRUSH	EvCtlColor		( HDC		hDC,
						  HWND		hwndChild,
						  UINT		type ) ;

		void	ResetFont		( void ) ;


	// Il aurait été plus simple d'utiliser EV_COMMAND_AND_ID avec CmColor,
	// mais ça ne marche apparemment qu'avec des menus...
		void	CmBackground  ( void )
		   {
			CmColor ( IDC_BACKGROUND_COLOR ) ;
		    }

		void	CmForeground  ( void ) 
		   {
			CmColor ( IDC_FOREGROUND_COLOR ) ;
		    }


	DECLARE_RESPONSE_TABLE ( TPreferencesDialog ) ;
    } ;





/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/******                                                              ******/
/******   TAboutDialog :                                             ******/
/******         Classe pour la gestion de la boîte "A propos de..."  ******/
/******                                                              ******/
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/

class 	TAboutDialog 	: 	public TDialog 
   {
	public:
		TAboutDialog ( TWindow *	parent ) :
			TDialog ( parent, IDD_ABOUT )  { }

		void	SetupWindow ( void ) ;
     } ;




/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/******                                                              ******/
/******   TChooseSessionDialog :                                     ******/
/******         Permet d'activer une session.                        ******/
/******                                                              ******/
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/

class 	TChooseSessionDialog  :  public TDialog 
   {
	private :
		TListBox *	ListBox ;
		int *		SessionId ;


	public:
		TChooseSessionDialog ( TWindow *	parent,
				       int *		session_id ) :
			TDialog ( parent, IDD_SESSION_LIST )  
		  { 
			ListBox = new  TListBox ( this, IDC_SESSION_LIST ) ;
			SessionId = session_id ;
			* SessionId = 0 ;
		    }


	       ~TChooseSessionDialog  ( )
		   {
			delete  ListBox ;
		    }
	

		BOOL	EvInitDialog	( HWND ) ;
		void	CmOk		( void ) ;
		void    CmClose		( void ) ;
		void	CmOpen		( void ) ;
		void	CmSelChange	( void ) ;


		void	CmCancel	( void )
		   {
			TDialog :: CmCancel ( ) ;
		    }

	DECLARE_RESPONSE_TABLE ( TChooseSessionDialog ) ;
    } ;






/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/******                                                              ******/
/******   TGeneralOptionsDialog                                      ******/
/******         Permet de définir les paramètres généraux.           ******/
/******                                                              ******/
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/

class   TGeneralOptionsDialog  :  public  TDialog
   {
	private :
		TEdit *		MinRedoSize ;


	public :
		TGeneralOptionsDialog ( TWindow *  parent )
			: TDialog ( parent, IDD_GENERAL ) 
		   {
			MinRedoSize =  new  TEdit ( this, IDC_MIN_REDO_SIZE ) ;
			MinRedoSize -> SetValidator ( new  TFilterValidator ( "0-9" ) ) ;
		    }


	       ~TGeneralOptionsDialog ( )
		   {
			delete  MinRedoSize ;
		    }


		BOOL	EvInitDialog  	( HWND ) ;
		void	CmOk 		( void ) ;


		void	CmCancel	( void )
		   {
			TDialog :: CmCancel ( ) ;
		    }

	DECLARE_RESPONSE_TABLE  ( TGeneralOptionsDialog ) ;
      } ;







/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/******                                                              ******/
/******   TGeneralOptionsDialog                                      ******/
/******         Permet de définir les paramètres généraux.           ******/
/******                                                              ******/
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/

class   TObjectListDialog  :  public  TDialog
   {
	private :
		TListBox *	ObjectList ;


	public :
		TObjectListDialog ( TWindow *  parent )
			: TDialog ( parent, IDD_OBJECT_LIST ) 
		   {
			ObjectList = new  TListBox ( this, IDC_OBJECT_LIST ) ;
		    }


	       ~TObjectListDialog ( )
		   {
			delete  ObjectList ;
		    }


		BOOL	EvInitDialog  	( HWND ) ;
		void	CmOk 		( void ) ;


		void	CmCancel	( void )
		   {
			TDialog :: CmCancel ( ) ;
		    }

	DECLARE_RESPONSE_TABLE  ( TObjectListDialog ) ;
      } ;







/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/******                                                              ******/
/******   TPrintingDialog -                                          ******/
/******         Boite de dialogue affichée durant une impression     ******/
/******         Unix en mode texte.                                  ******/
/******                                                              ******/
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/

class   TPrintingDialog  :  public  TDialog
   {
	private :
		BOOL *		PrintMode ;
		TGauge *	Slider ;
		int		AnimateEvery ;
		int		CurrentValue ;
		TWindow *	ParentWindow ;


	public :
		TPrintingDialog ( TWindow *  	parent, 
				  BOOL *  	print_mode,
				  int		animate_every )
			: TDialog ( parent, IDD_PRINTING ) 
		   {
			PrintMode 	= print_mode ;
			AnimateEvery    = animate_every ;
			CurrentValue	= 0 ;
			ParentWindow 	= parent ;
			Slider 		= 0 ;
		    }


	       ~TPrintingDialog ( )
		   {
			if  ( Slider )
				delete  Slider ;
			if  ( ParentWindow )
				ParentWindow -> SetFocus ( ) ;
		     }	


		BOOL	EvInitDialog  	( HWND ) ;
		void	AnimateSlider	( ) ;


		void	CmCancel	( void )
		   {
			* PrintMode = FALSE ;
			TDialog :: CmCancel ( ) ;
		    }

	DECLARE_RESPONSE_TABLE  ( TPrintingDialog ) ;
      } ;

