/**************************************************************************/
/*                                                                        */
/*  ChopMenu.h  -  Christian Vigh, 95/10/19.                              */
/*                                                                        */
/*  Classes du menu Cheops.                                               */
/*                                                                        */
/**************************************************************************/


# if	! defined ( __CHOPMENU_H__ )
#	  define    __CHOPMENU_H__ 


# include	"MNCommon.h"



/**************************************************************************/
/*                                                                        */
/*  Commandes de menu.                                                    */
/*                                                                        */
/**************************************************************************/

# define	Cheops_Module_Base		10000

# define	IDD_CHEOPS_CONFIG  		900
# define	IDD_CHEOPS_SUCONFIG		901
# define	IDD_SENDMAIL			902

# define	IDC_USER_LIST			100
# define	IDC_MAIL_TEXT			101

# define	IDC_YEAR			100
# define	IDC_MONTH			101
# define	IDC_SITE_NAME			102
# define	IDC_PRINTER			103
# define	IDC_PROGS			104
# define	IDC_DESCRIP			105
# define	IDC_DATA			106
# define	IDC_PROT			107


# define	CM_FILE_QUIT			1100
# define	CM_FILE_CONFIGURE		1101
# define	CM_FILE_LOG			1102
# define	CM_FILE_READMAIL		1103
# define	CM_FILE_SENDMAIL		1104
# define	CM_YOUR_CHOICE			1105
# define	CM_ENTER_COMMAND		1106
# define	CM_CONFIG_MONTH			1107
# define	CM_CONFIG_YEAR			1108
# define	CM_CONFIG_PRINTER		1109
# define	CM_WINDOW_ORDER_MENUS		1110
# define	CM_WINDOW_ORDER_ICONS		1111
# define	CM_WINDOW_ICONIFY_SCREENS	1112
# define	CM_WINDOW_ICONIFY_LISTS		1113
# define	CM_WINDOW_ICONIFY_ALL		1114
# define	CM_UNHIDE_COMMAND		1115



/**************************************************************************/
/*                                                                        */
/*  Classe  TCheopsWindowFont -                                           */
/*      Police par défaut pour les éléments du menu Cheops.               */
/*                                                                        */
/**************************************************************************/

class	TCheopsWindowFont 	:  public  TFont 
   {
	public :
		TCheopsWindowFont ( int  height  =  8 ) ;
     } ;




/**************************************************************************/
/*                                                                        */
/*  Classe  TCheopsMenu -                                                 */
/*      Fenêtre du menu Cheops.                                           */
/*                                                                        */
/**************************************************************************/

class	TCheopsMenuWindowClient ;
class	TCheopsTabButtonGadget ;
class	TCheopsGadgetMenuWindow ;
class	TCheopsFloatingMenu ;
class	TArrowWindow ;
class	TIntroWindow ;
class   TCheopsReadMailDialog ;
class   TCheopsSendMailDialog ;


class 	TCheopsMenuWindow  : public  TDecoratedMDIFrame
   {
	public :
		int				ObjectId ;
		TUnixApplication *		Application ;
		TCheopsMenuTree *		MenuTree ;	// Arborescence



	private :
		char 			ApplicationName [ MAX_PARAMETER_LENGTH ] ;

		TControlBar *		MainMenuControlBar ;	// Menu principal
		TControlBar *		ControlBar ;
		TStatusBar *		StatusBar ;		// Barre d'état

		TCheopsMenuWindowClient *	
					CheopsMenuWindowClient ;// Fenêtre cliente

		TWindow *		MenuParent ;		// Parent (Terminal)

		TCheopsUserConfig    	CheopsUserConfig ;   	// Configuration
		TCheopsUserInfo 	CheopsUserInfo ;    	// Infos user Unix

		TCheopsModule *		CheopsModules ;		// Modules Cheops
		int			CheopsModuleCount ;
		TCheopsTabButtonGadget **	
					CheopsMenuGadgets ;	
		TArrowWindow *		ArrowWindow ;
		TIntroWindow *		IntroWindow ;

		TMultiFontTextGadget *	MonthTextGadget,	// Gadgets de la barre d'état
				*	YearTextGadget,
				*	PrinterTextGadget,
				*	UserTextGadget ;
		TEdit		*	EditControlGadget ;

		TCheopsFloatingMenu * 	FloatingMenus ;		// Menus flottants ouverts

		TGadgetStyle		InitialMenuStyle,
					InitialSelectionStyle ;

		int			LastOpenedModule ;
		BOOL			ShutDownRequested ;
		BOOL			MailAvailable ;


	public :
		TCheopsReadMailDialog *	ReadMailWindow ;
		TCheopsSendMailDialog * SendMailWindow ;



	public :
		TCheopsMenuWindow  ( TWindow *	parent,
				     char *  	title ) ;
	       ~TCheopsMenuWindow  ( ) ;	


		virtual BOOL	Create  		( void ) ;
		void		CreateFloatingMenu	( TCheopsGadgetMenuWindow *  w,
							  TPoint &		     pos,
							  bool			     minimized = false ) ;
		void		FloatingMenusAutoCreate ( void ) ;


		void		CommandEnable		( TCommandEnabler &  ce ) ;
		void		CmEnterCommand		( void ) ;
		void		CmFileQuit		( void ) ;
		void		CmFileLog		( void ) ;
		void		CmFileReadMail		( void ) ;
		void		CmFileSendMail		( void ) ;
		void		CmYourChoice		( void ) ;

		void		CmFileConfigure         ( int	give_focus_to  =  -1 ) ;
		void		CmConfig		( void ) ;
		void		CmConfigYear		( void ) ;
		void		CmConfigMonth 		( void ) ;
		void		CmConfigPrinter		( void ) ;

		void		CmWindowOrderMenus	( void ) ;
		void		CmWindowOrderIcons 	( void ) ;
		void		CmWindowIconifyScreens	( void ) ;
		void		CmWindowIconifyLists	( void ) ;

		void		CmUnhideCommand		( void ) ;

		void	CmWindowIconifyAll  ( void )
		   {
			CmWindowIconifyScreens ( ) ;
			CmWindowIconifyLists   ( ) ;
		    }



		BOOL		CanClose		( void ) ;


	// Fenêtres Cheops
		void		IconifyMe 		( TWindow *	dlg,
							  TResId &	icon ) ;

		void		RemoveFloatingMenu	( TCheopsFloatingMenu *  m ) ;

	// Gestion des menus
		void		EvMenuDropped   ( TCheopsGadgetMenuWindow *	win ) ;
		void		EvMenuSelected  ( int				id,
						  TCheopsGadgetMenuWindow * 	gadget ) ;
		void		EvMenuClosed	( TCheopsGadgetMenuWindow *	win ) ;

	// Focus des fenetres
		void		EvSetFocus	( HWND  	lost ) ;
		void		EvKillFocus	( HWND		gained ) ;


	// Environnement de l'utilisateur
		void		SetUserInfo   	( TCheopsUserInfo &  	 info,
						  TCheopsUserConfig &    config ) ;
		TCheopsUserInfo &
				GetUserInfo	( void ) ;
		TCheopsUserConfig &
				GetConfiguration( void ) ;
		


	// Modules
		void		SetModules	( TCheopsModule *	mods,
						  int			modcount ) ;

		TCheopsModule *	GetModules	( int &  		count ) ;


	// Gestion des sous-fonctions du menu
		char *        	ProcessMenuFunction ( int		subfunction,
						      char *		data,
						      int &		status,
						      int &		reply_size ) ;

		void		BuildMenuTree	    ( FILE *		fp )
		   {
			MenuTree = new  TCheopsMenuTree ( fp ) ;
		    }


	// Définit les identificateurs de l'application et de cet objet
# if  ! STANDALONE 
		void	SetIds ( TUnixApplication *  app, integer16  objectid )
		   {
			Application = app ;
			ObjectId    = objectid ;
			CheopsConfiguration -> ApplicationId = app -> ApplicationId ;
			CheopsConfiguration -> ProcessId     = app -> ProcessId ;
			CheopsConfiguration -> ObjectId      = objectid ;
		     }
# endif



	// Cache le parent
		void	Show  ( uint  cmd )
		   {
			TDecoratedFrame :: Show ( cmd ) ;

			if  ( MenuParent )
				MenuParent -> Show ( SW_HIDE ) ;
		    }



	// Récupération de la couleur d'un module 
		COLORREF  GetLastModuleColor ( void )
		   {
			if  ( LastOpenedModule  ==  -1  ||
				CheopsModules [ LastOpenedModule ]. ModuleColor  ==  0xFFFFFFFFL )
				return  ( GetSysColor ( COLOR_BTNFACE ) ) ;
			else
				return  ( CheopsModules [ LastOpenedModule ]. ModuleColor ) ;
		    }

			
	// PreProcessMsg : Appelle IdleAction car cette dernière n'est appelée 
	// que pour la fenêtre principale...
		virtual BOOL	PreProcessMsg ( MSG &  msg ) ;


	

	protected :
		void		UpdateInformation  ( void ) ;		
		void		EvModuleCommand	   ( uint	id ) ;	

		virtual void	EvInitMenuPopup	   ( HMENU	menu,
						     uint	index,
						     BOOL	issysmenu ) ;


		void		EvTimer  ( uint ) ;
		void		EvClose  ( void ) ;



	// Empêche le dessin d'une bordure inactive si le focus est donné à
	// une TGadgetMenuWindow
		LRESULT		EvMyNCActivate    ( WPARAM		wparam,
						    LPARAM		lparam )
		   {
			if  ( :: GetWindowTask ( ( HWND ) LOWORD ( lparam ) )  ==
					:: GetCurrentTask ( ) )
				return  ( DefWindowProc ( WM_NCACTIVATE, 1,
						( LPARAM ) HWindow ) ) ;
			else
				return  ( DefWindowProc ( WM_NCACTIVATE, wparam, lparam ) ) ;
		     }



	private :
		LRESULT 	WmMenuDropped ( WPARAM, LPARAM  win )
		   {
			EvMenuDropped ( ( TCheopsGadgetMenuWindow * ) win ) ;
			return  ( 0 ) ;
		    }


		LRESULT 	WmMenuSelected ( WPARAM  id, LPARAM  gadget )
		   {
			EvMenuSelected ( id, ( TCheopsGadgetMenuWindow * ) gadget ) ;
			return  ( 0 ) ;
		    }



		LRESULT 	WmMenuClosed  ( WPARAM, LPARAM  window )
		   {
			EvMenuClosed ( ( TCheopsGadgetMenuWindow * ) window ) ;
			return ( 0 ) ;
		    }


# if	! STANDALONE
		LRESULT		WmEndApplication  ( WPARAM, LPARAM )
		   {
			ShutDownRequested = TRUE ;
			return  ( 0L ) ;
		    }
# endif



	// Retourne la valeur d'un mot-clé décrivant un style de gadget
		TGadgetStyle   GetProfileStyle ( char *  section, char *  item )
		   {
			char		Buffer [ MAX_PARAMETER_LENGTH ] ;


			if  ( ! CheopsConfiguration )
				return  ( RaisedGadget ) ;

			CheopsConfiguration -> Get ( section, item, Buffer ) ;


			if  ( ! stricmp ( Buffer, "Normal" ) )
				return  ( NormalGadget ) ;
			else if  ( ! stricmp ( Buffer, "Raised" ) )
				return  ( RaisedGadget ) ;
			else if  ( ! stricmp ( Buffer, "Recessed" ) )
				return  ( RecessedGadget ) ;
			else
				return  ( RaisedGadget ) ;
		      }

 

	DECLARE_RESPONSE_TABLE ( TCheopsMenuWindow ) ;
     } ;



extern	TCheopsMenuWindow *	TheCheopsMenuWindowObject ;



/**************************************************************************/
/*                                                                        */
/*  Classe  TCheopsMenuClient -                                           */
/*      Fenêtre cliente de la classe TCheopsMenu.                         */
/*                                                                        */
/**************************************************************************/

class	TBitmapWindow ;


class	TCheopsMenuWindowClient	: public  TMDIClient
   {
	private :
		TCheopsMenuWindow *	CheopsMenuWindow ;
		TMemoryDC *		MemDC ;
		TBitmap *		MemDCBitmap ;
		int			BitmapWidth,
					BitmapHeight ;



	public :
		TCheopsMenuWindowClient  ( TCheopsMenuWindow *	  parent ) ;
	       ~TCheopsMenuWindowClient  ( ) ;	


		BOOL		Create		( void ) ;
		void		Paint   	( TDC &, BOOL, TRect & ) ;
		void		EvHScroll	( uint, uint, HWND ) ;
		void		EvVScroll	( uint, uint, HWND ) ;


	DECLARE_RESPONSE_TABLE ( TCheopsMenuWindowClient ) ;
    } ;




/**************************************************************************/
/*                                                                        */
/*  TCheopsTabButtonGadget -                                              */
/*      Classe onglet, avec un rectangle de couleur à gauche du texte.    */
/*                                                                        */
/**************************************************************************/

class	TCheopsTabButtonGadget :  public  TTabButtonGadget
    {
	private :
		TSize		SquareSize ;
		COLORREF	Color ;

	public :
	// Constructeur et destructeur
	//
		TCheopsTabButtonGadget   ( int		id,
					   COLORREF     color,
					   bool		enabled = true,
					   int		numchar = 10 ) ;


		virtual void	GetDesiredSize  ( TSize &  size ) ;
		virtual void	PaintContents   ( TDC &    dc,
						  TRect &  rect ) ;



	// Sert à changer l'épaisseur du bouton lorsqu'il est enfoncé
		virtual BOOL	GetRegion	( TRegion &  	region,
						  TRegionType   type )
		   {
			if  ( State  ==  Up )
			   {
				HiliteUnits = GetSystemMetrics ( SM_CXBORDER ) ;
				ShadowUnits = GetSystemMetrics ( SM_CYBORDER ) ;
			     }
			else
			   {
				HiliteUnits = 2 * GetSystemMetrics ( SM_CXBORDER ) ;
				ShadowUnits = 2 * GetSystemMetrics ( SM_CYBORDER ) ;
			     }

			return  ( TTabButtonGadget :: GetRegion ( region, type ) ) ;
		     }
       } ;                  





/**************************************************************************/
/*                                                                        */
/*  TCheopsConfigurationDialog -                                          */
/*      Affiche le F5-config de l'utilisateur.                            */
/*                                                                        */
/**************************************************************************/

class	TCheopsConfigurationDialog  :  public  TDialog 
   {
	private :
		TCheopsUserInfo *	UserInfo ;
		TCheopsUserConfig *	UserConfig ;
		TComboBox *		PrinterControl ;
		TEdit *			YearControl,
		      *			MonthControl,
		      *			SiteNameControl,
		      *			ProgsControl,
		      *			DescripControl,
		      *			DataControl,
		      *			ProtectionControl ;
		int			GiveFocusTo ;


	public :
		TCheopsConfigurationDialog  ( TWindow *			parent,
					      TCheopsUserConfig & 	user_info,
					      TCheopsUserInfo &		user_config,
					      int			give_focus_to  =  -1 ) ;
	       ~TCheopsConfigurationDialog ( ) ;



		BOOL	EvInitDialog  ( HWND  focus ) ;
		void	CmOk 	      ( void ) ;



	DECLARE_RESPONSE_TABLE ( TCheopsConfigurationDialog ) ;
     } ;		




# endif		/*  __CHOPMENU_H__  */




