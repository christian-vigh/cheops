/**************************************************************************/
/*                                                                        */
/*  TermWin.h  -  Christian Vigh, 94/09/12.                               */
/*                (d'après Sébastien George et Christophe Després)        */
/*                                                                        */
/*  Définition de la classe TTerminalWindow (fenêtre principale).         */
/*                                                                        */
/**************************************************************************/


class	TConnection ;
class	TDisplayDevice ;
class	TUnixApplicationTable ;





class	TTerminalWindow		:  public  TDecoratedFrame
	 {
	private :
		BOOL			InSelection ;
		TPoint			LastPosition ;

	public :
		TStatusBar *		StatusBar ;


	public :
		TTerminalWindow  	( char *			Title ) ;
	       ~TTerminalWindow  ( ) ;


		virtual BOOL	CanClose  	( void ) ;
		virtual BOOL	Create		( void ) ;
		virtual void	SetupWindow     ( void ) ;
		virtual void   	CleanupWindow	( void ) ;
		void		Destroy		( int ) ;


# if	STANDALONE
		void		IDMFileOpen			( void ) ;
# endif

		void		IDMFileConnect			( void ) ;
		void		IDMFileDisconnect		( void ) ;
		void		IDMFileAddSession		( void ) ;
		void		IDMFileRemoveSession		( void ) ;
		void		IDMFileTransfer			( void ) ;
		void		IDMFileReceive			( void ) ;
		void		IDMFilePrinterConfig		( void ) ;
		void		IDMFilePrintPage		( void ) ;
		void		IDMFilePrintContinuous		( void ) ;
		void		IDMFileLog			( void ) ;
		void		IDMFileSaveConfiguration	( void ) ;
		void		IDMFileQuit			( void ) ;
		void		IDMEditCopy			( void ) ;
		void		IDMEditPaste			( void ) ;
		void		IDMEditRedo			( void ) ;
		void		IDMEditSelectAll		( void ) ;
		void		IDMOptionsDisplayAll		( void ) ;
		void		IDMOptionsDisplayFunctionKeys	( void ) ;
		void		IDMOptionsGlobal 		( void ) ;
		void		IDMOptionsConfiguration		( void ) ;
		void		IDMOptionsSessionDefaults	( void ) ;
		void		IDMOptionsConnectionDefaults	( WPARAM  id ) ;
		void		IDMWindowNextSession		( void ) ;
		void		IDMWindowPreviousSession	( void ) ;
		void		IDMWindowSetCurrentSession	( WPARAM  id ) ;
		void		IDMWindowSessionList		( void ) ;
		void		IDMMaintenanceUnfreeze		( void ) ;
		void		IDMMaintenanceObjects		( void ) ;
		void		IDMHelpAbout  			( void ) ;
		void		IDMHelpIndex  			( void ) ;

		void		EvSessionCommand		( unsigned int	id ) ;

		void		IDMLambdaShortcut  ( void )
		   {
			Configuration -> Terminal ( ). LambdaUserShortcut = 
				! Configuration -> Terminal ( ). LambdaUserShortcut ;
		    }

		void		SetStatusBarText  	( int		index,
							  char *	message, 
							  ... ) ;
		void		UpdateConnectionText    ( TConfiguration :: TSessionState  state ) ;

		
		void		EvChar			( UINT		c,
							  UINT,
							  UINT ) ;
		void		EvSetFocus		( HWND  hwnd ) ;
		void		EvKillFocus		( HWND  hwnd ) ;
		void		EvKeyDown		( UINT	virtkey,
							  UINT	repeat_count,
							  UINT	flags ) ;
		void		EvSysKeyDown		( UINT	virtkey,
							  UINT	repeat_count,
							  UINT	flags ) ;
		void		EvLButtonDblClk		( UINT,
							  TPoint & ) ;
		void		EvLButtonDown		( UINT,
							  TPoint & ) ;
		void		EvRButtonDown		( UINT,
							  TPoint & ) ;
		void		EvLButtonUp		( UINT,
							  TPoint & ) ;

		void		EvSysColorChange	( void ) ;


		void		EvInitMenuPopup		( HMENU		menu,
							  uint		index,
							  BOOL		sysmenu ) ;
						
		void		CommandEnable		( TCommandEnabler &  ce ) ;

		void		EvMouseMove		( UINT,
							  TPoint & ) ;
		void		GotKey	   		( BOOL		syskey,
							  UINT		virtkey,
							  UINT		repeat_count,
							  UINT		flags ) ;

		virtual void	Paint			( TDC &, BOOL,
							  TRect &  	rect ) ;


		void		EvMenuSelect		( uint, uint, HMENU ) ;
		void		EvNCLButtonDblClk	( uint, TPoint & ) ;



	// Gestion de la position par  défaut des boîtes Cheops
		void	GetLastPosition ( TPoint &  p )
		   {
			p = LastPosition ;
		    }

		void	SetLastPosition ( TPoint &  p )
		   {
			LastPosition = p ;
		    }

		void	ComputeNextPosition ( TWindow *  win ) ;


	// Change le titre de la fenêtre
		void		SetText  ( void )
		   {
			char	buffer [ MAX_PARAMETER_LENGTH ] ;

			if  ( SessionParameters -> WindowTitle [0] )
				strcpy ( buffer, SessionParameters -> WindowTitle ) ;
			else
				wsprintf ( buffer, "Terminal  -  %s", Configuration -> SessionName ( ) ) ;
			SetWindowText ( buffer ) ;
		    }



	// GetWindowClass va permettre de spécifier que les fenêtres appartenant 
	// à cette classe doivent avoir un display context privé.
		void	GetWindowClass ( WNDCLASS &  WindowClass )
		   {
			TDecoratedFrame :: GetWindowClass ( WindowClass ) ;
			WindowClass. style  |=  CS_OWNDC ;
		    }



	DECLARE_RESPONSE_TABLE ( TTerminalWindow ) ;
     } ;





/**************************************************************************/
/*                                                                        */
/*  Classe  TTerminalWindowClient :                                       */
/*      Une TDecoratedFrame possède toujours une fenêtre cliente. Par     */
/*      défaut, lorsqu'aucune fenêtre n'est indiquée lors de sa création, */
/*      OWL crée une fenêtre client de type TWindow. Tout ceci fonctionne */
/*      très bien, mis à part le fait que les messages clavier ne sont pas*/
/*      envoyés à la fenêtre principale... Cette classe à donc pour but   */
/*      de relayer ces messages.                                          */
/*                                                                        */
/**************************************************************************/

# define	RelayMessage(func,msg)					  \
			LRESULT  func  ( WPARAM  wparam, LPARAM  lparam ) \
			   {						  \
				TWindow :: DefWindowProc ( msg, wparam,	  \
					lparam ) ;			  \
				return ( Parent -> SendMessage ( msg,     \
					wparam, lparam ) ) ;		  \
			    }

			    
class	TTerminalWindowClient	:  public  TWindow
   {
	private :
		int			MySession ;
		TDisplayDevice *        MyDevice ;
		TConnection *		MyConnection ;


	public :
		TTerminalWindowClient  ( TWindow *	parent,
					 int		session )
			: TWindow ( parent, "", 0 )  
		   {
			Attr. Style  =  WS_CLIPCHILDREN |  WS_CHILD ;
			MySession    = session ;
			MyDevice     = 0 ;
			MyConnection = 0 ;
		    }

	       ~TTerminalWindowClient ( ) ;	


		void		EvSetFocus  		( HWND  hwnd )   ;
		void		EvKillFocus  		( HWND  hwnd )  ;

		LRESULT		EvDataReceived		( WPARAM	wparam,
							  LPARAM	lparam ) ;
		LRESULT		EvSendData		( WPARAM	wparam,
							  LPARAM	lparam ) ;
		LRESULT		EvUnixMessage		( WPARAM	wparam,
							  LPARAM 	lparam ) ;

		void 		Paint			( TDC &, BOOL, TRect & ) ;

		void		Assign			( TDisplayDevice *  dev,
							  TConnection *     conn )
		   {
			MyDevice = dev ;
			MyConnection = conn ;
		    }


		RelayMessage  ( EvChar		, WM_CHAR		)
		RelayMessage  ( EvSysKeyDown    , WM_SYSKEYDOWN		)
		RelayMessage  ( EvKeyDown       , WM_KEYDOWN		)
		RelayMessage  ( EvLButtonDblClk , WM_LBUTTONDBLCLK 	)
		RelayMessage  ( EvLButtonDown   , WM_LBUTTONDOWN	)
		RelayMessage  ( EvLButtonUp     , WM_LBUTTONUP		)
		RelayMessage  ( EvRButtonDown   , WM_RBUTTONDOWN	)		
		RelayMessage  ( EvMouseMove	, WM_MOUSEMOVE		)

	DECLARE_RESPONSE_TABLE ( TTerminalWindowClient ) ;
    } ;

