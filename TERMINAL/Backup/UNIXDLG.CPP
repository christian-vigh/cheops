/**************************************************************************/
/*                                                                        */
/*  UnixDlg.cpp  -  Christian Vigh, 94/11/16.                             */
/*                                                                        */
/*  Impl�mentation de la classe TUnixDialogBox.                           */
/*                                                                        */
/**************************************************************************/


# include	"AllTerm.h"
# pragma	hdrstop

# include	"UnixApp.h"
# include	"Event.h"
# include	"Icon.h"
# include	"UnixDlg.h"
# include	"ChopJet.h"

# if	defined ( CHEOPS )
#    include	"Cheops.h"
# endif

# include	"Gadgets/DlgObjs.h"


extern BOOL FAR PASCAL _export  StdDlgProc ( HWND, UINT, WPARAM, LPARAM ) ;

TProcInstance 	DialogProcInstance ( ( FARPROC ) StdDlgProc ) ;



DEFINE_RESPONSE_TABLE1 ( TUnixDialogBox, TDialog )
	EV_COMMAND ( IDOK       , CmOk ),
	EV_COMMAND ( IDCANCEL   , CmCancel ),
	EV_WM_PAINT,
	EV_MESSAGE ( WM_SETICON , EvSetIcon ),
END_RESPONSE_TABLE ;


/**************************************************************************/
/*                                                                        */
/*  TUnixDialogBox :: TUnixDialogBox                                      */
/*      Constructeur de la classe.                                        */
/*      Cr�e le template pour la bo�te de dialogue.                       */
/*                                                                        */
/**************************************************************************/

# define	CHECKSTYLE(style)					\
			if  ( dlgtemplate -> dtStyle & (style) )        \
			   {                                            \
				dlgtemplate -> dtStyle  &=  ~(style) ;  \
				UnusedStyle             |=    style ;   \
			    }




TUnixDialogBox :: TUnixDialogBox  ( TUnixApplication *	app,
				    TWindow *		parent,
				    unsigned char*	Data,
				    int			DataSize,
				    int			PID,
				    BOOL		isclient,
				    TWindow*		EnvMsg )
		: TDialog ( ( isclient ) ? 0 : parent, 0 )
    {
	char far *			globalp ;
	DLGTEMPLATE far *		dlgtemplate ;
	int				combolist [ 100 ], combolistpos ;


// Initialisation des donn�es membres
	UnixApplication		=  app ;
	ParentWindow		=  parent ;
	ProcessId		=  PID ;
	MyId			=  app -> ApplicationId ;
	LastFocusHwnd		=  0 ;
	IsClientWindow		=  isclient ;
	UnusedStyle		=  0L ;
	ResponsableMsg		= EnvMsg;


// Cr�ation du template en m�moire
	CreateDialogBoxTemplate ( Data, DataSize, DialogBoxTemplate ) ;

// R�cup�ration du style de la bo�te de dialogue
	if  ( ! stricmp ( DialogBoxTemplate. DialogTemplate. dtClassName, "BorDlg" ) )
		ControlStyle = Borland ;
	else
		ControlStyle = Microsoft ;

// Transformation des ComboBox en UnixComboBox pour mieux contr�ler ( BigBrother )
	memset ( combolist, 0, sizeof ( combolist ) ) ;
	combolistpos = 0 ;
	for  ( int i = 0 ; i < DialogBoxTemplate. DialogTemplate. dtItemCount ; i ++ )
	   {
		if  ( ! stricmp ( DialogBoxTemplate. DialogItemTemplates [i]. dtilClass, "ComboBox" ) )
		   {
			DialogBoxTemplate. DialogItemTemplates [i].dtilStyle |= CBS_OWNERDRAWFIXED | CBS_HASSTRINGS ;
			DialogBoxTemplate. DialogItemTemplates [i].dtilStyle &= ( ~ WS_EX_NOPARENTNOTIFY ) ;
			combolist [ combolistpos ++ ] = DialogBoxTemplate. DialogItemTemplates [i]. dtilID ;
			new TEnhancedComboBox ( this, DialogBoxTemplate. DialogItemTemplates [i]. dtilID ) ;
		    }
		else if  ( ! stricmp ( DialogBoxTemplate. DialogItemTemplates [i]. dtilClass, "BorCheck" )   ||
			   ! stricmp ( DialogBoxTemplate. DialogItemTemplates [i]. dtilClass, "CHECKBOX" ) )
			new  TDialogCheckBox ( this, DialogBoxTemplate. DialogItemTemplates [i]. dtilID,
				DialogBoxTemplate. DialogEntryData [i],
				DialogBoxTemplate. DialogItemTemplates [i] ) ;
		else if  ( ! stricmp ( DialogBoxTemplate. DialogItemTemplates [i]. dtilClass, "GROUPBOX" )   ||
			   ( ! stricmp ( DialogBoxTemplate. DialogItemTemplates [i]. dtilClass, "BorShade" )  &&
				DialogBoxTemplate. DialogItemTemplates [i]. dtilID  !=  - 1 ) )
			new  TDialogGroupBox ( this, DialogBoxTemplate. DialogItemTemplates [i]. dtilID,
				DialogBoxTemplate. DialogEntryData [i],
				DialogBoxTemplate. DialogItemTemplates + i + 1,
				DialogBoxTemplate. DialogItemTemplates +
					DialogBoxTemplate. DialogTemplate. dtItemCount - 1 ) ;
		else if  ( ! stricmp ( DialogBoxTemplate. DialogItemTemplates [i]. dtilClass, "EDIT" ) )
			new  TDialogEdit ( this, DialogBoxTemplate. DialogItemTemplates [i]. dtilID,
				DialogBoxTemplate. DialogEntryData [i],
				DialogBoxTemplate. DialogItemTemplates [i] ) ;
	    }

	MemoryHandle = CreateDialogBoxHandle ( DialogBoxTemplate ) ;


// R�cup�ration du pointeur vers le template
	globalp      = ( char far * ) GlobalLock  ( MemoryHandle ) ;
	dlgtemplate  = ( DLGTEMPLATE * ) globalp ;


// Si la bo�te doit �tre dans la partie cliente d'une fen�tre, on enl�ve
// les styles non utilisables
// Attention : le code suivant ne marche que si on ne cherche pas �
// acc�der aux cha�nes de caract�res du template (ne pas oublier que le
// template donn� � CreateDialogBoxIndirect est un DLGTEMPLATE "mis � plat")
	if  ( IsClientWindow )
	   {
		CHECKSTYLE ( DS_MODALFRAME  ) ;
		CHECKSTYLE ( WS_BORDER      ) ;
		CHECKSTYLE ( WS_CAPTION     ) ;
		CHECKSTYLE ( WS_DLGFRAME    ) ;
		CHECKSTYLE ( WS_ICONIC      ) ;
		CHECKSTYLE ( WS_MINIMIZE    ) ;
		CHECKSTYLE ( WS_MINIMIZEBOX ) ;
		CHECKSTYLE ( WS_MAXIMIZE    ) ;
		CHECKSTYLE ( WS_MAXIMIZEBOX ) ;
		CHECKSTYLE ( WS_OVERLAPPED  ) ;
		CHECKSTYLE ( WS_POPUP       ) ;
		CHECKSTYLE ( WS_SYSMENU     ) ;
		CHECKSTYLE ( WS_THICKFRAME  ) ;

		dlgtemplate -> dtStyle  |=  WS_CHILD ;
		dlgtemplate -> dtStyle  &= ~( WS_VISIBLE ) ;
	     }
	else
		dlgtemplate -> dtStyle  |=  WS_VISIBLE ;


// D�verrouillage du template
	GlobalUnlock ( MemoryHandle ) ;
    }



/**************************************************************************/
/*                                                                        */
/*   TUnixDialogBox :: DoCreate, DoExecute -                              */
/*      Cr�e la bo�te de dialogue � partir de sa d�finition contenue dans */
/*      Data.                                                             */
/*      Lorsque la bo�te de dialogue doit �tre dans la partie cliente d'  */
/*      une fen�tre, les styles concernant la bordure de la bo�te sont    */
/*      r�cup�r�s dans UnusedStyle et supprim� de la d�finition.          */
/*                                                                        */
/**************************************************************************/

HWND	TUnixDialogBox ::  DoCreate  ( void )
   {
	char far *			globalp ;


// R�cup�ration du pointeur vers le template
	globalp      = ( char far * ) GlobalLock  ( MemoryHandle ) ;

// Cr�ation de la boiboite
	DialogHandle = CreateDialogIndirectParam (
				* GetModule ( ), globalp,
				  ( ParentWindow  !=  NULL ) ?
					ParentWindow -> HWindow : NULL,
				     ( DLGPROC ) ( FARPROC ) DialogProcInstance,
					 Attr. Param ) ;

// D�verrouillage du template
	GlobalUnlock ( MemoryHandle ) ;


// C'est bon
	return ( DialogHandle ) ;
    }



int	TUnixDialogBox ::  DoExecute  ( void )
   {
	DialogBoxIndirectParam (
				* GetModule ( ), MemoryHandle,
				  ( ParentWindow  !=  NULL ) ?
					ParentWindow -> HWindow : NULL,
				     ( DLGPROC ) ( FARPROC ) DialogProcInstance,
					 Attr. Param ) ;

// C'est bon
	return ( 0 ) ;
    }



/**************************************************************************/
/*                                                                        */
/*   TUnixDialogBox :: CmOk                                               */
/*      Simule l'appui sur la touche TAB lorsque la touche Entr�e est     */
/*      press�e.                                                          */
/*                                                                        */
/**************************************************************************/

void  TUnixDialogBox :: CmOk ( void )
   {
	EventQueue -> Enqueue (
		UnixApplication -> ApplicationId,
			ObjectId,
			UnixApplication -> ProcessId,
			WM_CHAR,
			VK_RETURN,
			0L,
			UWC_LPARAM_NOT_POINTER ) ;
    }

void  TUnixDialogBox :: CmCancel ( void )
   {
	DialogFunction ( WM_COMMAND, IDCANCEL, 0L ) ;
    }




/**************************************************************************/
/*                                                                        */
/*   TUnixDialogBox :: DialogFunction                                     */
/*      Se charge de g�rer les �v�nements utilisateur survenant de la     */
/*      bo�te de dialogue, puis les place dans la file de message �       */
/*      destination d'Unix.                                               */
/*                                                                        */
/**************************************************************************/

BOOL	TUnixDialogBox :: DialogFunction ( UINT  	msg,
					   WPARAM  	wparam,
					   LPARAM	lparam )
   {
	BOOL		Status ;



	Status = TDialog :: DialogFunction ( msg, wparam, lparam ) ;


	switch ( msg )
	   {
	// InitDialog : il faut envoyer ce message vers Unix
		case	WM_INITDIALOG :
			EventQueue -> Enqueue (
				UnixApplication -> ApplicationId,
				ObjectId,
				UnixApplication -> ProcessId,
				WM_INITDIALOG,
				wparam,
				lparam,
				UWC_LPARAM_NOT_POINTER ) ;
		       break;


		case	WM_COMMAND :
			if  ( ProcessCommand ( wparam, LOWORD ( lparam ),
						HIWORD ( lparam ), wparam, lparam ) )
			    {
				EventQueue -> Enqueue (
					UnixApplication -> ApplicationId,
					ObjectId,
					UnixApplication -> ProcessId,
					WM_COMMAND,
					wparam,
					lparam,
					UWC_LPARAM_NOT_POINTER ) ;
			      }
			break ;

		case	WM_DESTROY :
			EventQueue -> Enqueue (
				UnixApplication -> ApplicationId,
				ObjectId,
				UnixApplication -> ProcessId,
				WM_DESTROY,
				wparam,
				lparam,
				UWC_LPARAM_NOT_POINTER ) ;
			break ;
		case  WM_KEYDOWN :
			switch  ( wparam )
				{
					case  VK_DOWN :
					case  VK_UP :
					case  VK_PRIOR :
				       case  VK_NEXT :
					case  VK_ESCAPE :
					case  VK_RETURN :
							EventQueue -> Enqueue (
								UnixApplication -> ApplicationId,
								ObjectId,
								UnixApplication -> ProcessId,
								WM_KEYDOWN,
								wparam,
								lparam,
								UWC_LPARAM_NOT_POINTER ) ;

				 }
				break ;
		case	WM_TIMER :
			if  ( EventQueue -> vide ( ) )
			   {
				EventQueue -> Enqueue (
					UnixApplication -> ApplicationId,
					ObjectId,
					UnixApplication -> ProcessId,
					WM_TIMER,
					wparam,
					lparam,
					UWC_LPARAM_NOT_POINTER ) ;
			       }
			break ;
	     }


	return ( Status ) ;
     }


/**************************************************************************/
/*                                                                        */
/*   TUnixDialogBox :: IsNotFunctionKey                                   */
/*      Retourne TRUE si la fen�tre ayant le focus n'est pas une touche   */
/*      de fonction.                                                      */
/*                                                                        */
/**************************************************************************/

BOOL	TUnixDialogBox :: IsNotFunctionKey  ( HWND  hwnd )
   {

# if	defined ( CHEOPS )
	register int	Id 	=  :: GetDlgCtrlID ( hwnd ) ;

	if  ( Id  <  START_ID  ||  Id  >  START_ID + 7 )
		return ( TRUE ) ;
	else
		return ( FALSE ) ;
# else
	return ( FALSE ) ;
# endif
    }



/**************************************************************************/
/*                                                                        */
/*   TUnixDialogBox :: ProcessCommand                                     */
/*      Filtre les messages WM_COMMAND.                                   */
/*                                                                        */
/**************************************************************************/

# pragma 	argsused


BOOL  TUnixDialogBox :: ProcessCommand  ( UINT		ControlId,
					  UINT 		ControlHwnd,
					  UINT		NotifyCode,
					  WPARAM	wparam,
					  LPARAM	lparam )
    {
	char		ClassName [ MAX_PARAMETER_LENGTH ] ;
	HWND		FocusHwnd	=  :: GetFocus ( ) ;



	:: GetClassName ( ( HWND ) ControlHwnd, ClassName,
				sizeof ( ClassName ) ) ;


// Si HWND est NULL, le message provient de CmCancel...
	if  ( ! ControlHwnd )
		return ( FALSE ) ;


// Sinon, on r�agit selon la classe du contr�le
	if  ( ! stricmp ( ClassName, "Button" )  ||
	      ! stricmp ( ClassName, "BorBtn" )  ||
	      ! stricmp ( ClassName, "BorCheck" )  ||
	      ! stricmp ( ClassName, "CHECKBOX" )  ||
	      ! stricmp ( ClassName, "RADIOBUTTON" )  ||
	      ! stricmp ( ClassName, "BorRadio" ) )
	   {
		switch ( NotifyCode )
		   {
			case	BN_SETFOCUS :
				LastFocusHwnd = ( HWND ) ControlHwnd ;
				if  ( EventQueue -> SetFocus ( UnixApplication -> ProcessId,
							wparam,lparam ) )
					return  ( FALSE ) ;
				if  ( IsChild  ( ( HWND ) ControlHwnd )  &&
					IsNotFunctionKey ( FocusHwnd ) )
					return ( TRUE ) ;
				break ;

			case	BN_CLICKED :
				return ( TRUE ) ;
		     }
	     }
	else if  ( ! stricmp ( ClassName, "EDIT" )  ||
		   ! stricmp ( ClassName, "LZCColor" )  ||
		   ! stricmp ( ClassName, "GROUPBOX" )  ||
		   ( ! stricmp ( ClassName, "BorShade" )  &&
			( :: GetWindowLong ( ( HWND ) ControlHwnd, GWL_STYLE ) & WS_GROUP ) ) )
	   {
		switch ( NotifyCode )
		   {
			case	EN_SETFOCUS :
				LastFocusHwnd = ( HWND ) ControlHwnd ;
				if  ( EventQueue -> SetFocus ( UnixApplication -> ProcessId,
							wparam,lparam ) )
					return  ( FALSE ) ;
				if  ( IsChild  ( ( HWND ) ControlHwnd )  &&
					IsNotFunctionKey ( FocusHwnd ) )
					return ( TRUE ) ;
				break ;
		    }
	      }

	else if ( ! stricmp ( ClassName, "COMBOBOX" ) )
	   {
		switch ( NotifyCode )
			{
			case  CBN_DROPDOWN :
			case  CBN_CLOSEUP :
				return  ( TRUE ) ;
			case	CBN_SETFOCUS :
				LastFocusHwnd = ( HWND ) ControlHwnd ;
				if  ( EventQueue -> SetFocus ( UnixApplication -> ProcessId,
							wparam,lparam ) )
					return ( FALSE ) ;
				if  ( IsChild  ( :: GetFocus ( ) ) )
					return ( TRUE ) ;
				break ;
		     }
	     }
	else if  ( ! stricmp ( ClassName, "LISTBOX" ) )
	   {
		switch ( NotifyCode )
		   {
			case	LBN_SETFOCUS :
				LastFocusHwnd = ( HWND ) ControlHwnd ;
				if  ( EventQueue -> SetFocus ( UnixApplication -> ProcessId,
							wparam,lparam ) )
					return ( FALSE ) ;
				if  ( IsChild  ( :: GetFocus ( ) ) )
					return ( TRUE ) ;
				break ;
		     }
	      }


// Autre message de notification : il ne faut pas pr�venir Unix
	return ( FALSE ) ;
    }




/**************************************************************************/
/*                                                                        */
/*   TUnixDialogBox :: PreProcessMsg :                                    */
/*      Filtre l'appui sur les touches de fonction.                       */
/*                                                                        */
/**************************************************************************/

BOOL  TUnixDialogBox :: PreProcessMsg  ( MSG &  msg )
   {
# if  	defined ( CHEOPS )

// Rechercher d'abord si c'est un message WM_KEYDOWN : dans ce cas, fastoche
	if  ( msg. message  ==  WM_KEYDOWN ||  msg. message  ==  WM_SYSKEYDOWN )
	   {
		if  ( msg. wParam  >=  VK_F1  &&  msg. wParam  <=  VK_F8 )
		   {
			:: SendMessage (
				( ResponsableMsg ) ?  ResponsableMsg -> HWindow :
						      GetParent (),
					WM_FUNCTIONKEY,
					msg. wParam - VK_F1,
					BN_CLICKED ) ;
			return ( TRUE ) ;
		     }
		else if  ( msg. wParam  ==  VK_DOWN  ||  msg. wParam  ==  VK_UP  ||
			   msg. wParam  ==  VK_PRIOR  ||  msg. wParam  ==  VK_NEXT )
		   {
			char		buffer [ 256 ] ;


			:: GetClassName ( msg. hwnd, buffer, sizeof ( buffer ) ) ;


			if  ( ! stricmp ( buffer, "EDIT" ) )
			   {
				register HWND  	ParentHwnd = :: GetParent ( msg. hwnd ) ;


				:: GetClassName ( ParentHwnd, buffer, sizeof ( buffer ) ) ;

			// N'envoyer l'�v�nement que si le parent n'est pas une
			// combo OU que la liste n'est pas ouverte
				if  ( stricmp ( buffer, "COMBOBOX" )  ||
					! :: SendMessage ( ParentHwnd, CB_GETDROPPEDSTATE, 0, 0L ) )
				   {
					EventQueue -> Enqueue (
						UnixApplication -> ApplicationId,
						ObjectId,
						UnixApplication -> ProcessId,
						WM_KEYDOWN,
						msg. wParam,
						msg. lParam,
						UWC_LPARAM_NOT_POINTER ) ;

					return ( TRUE ) ;
				    }
			     }
			else
				EventQueue -> Enqueue (
					UnixApplication -> ApplicationId,
					ObjectId,
					UnixApplication -> ProcessId,
					WM_KEYDOWN,
					msg. wParam,
					msg. lParam,
					UWC_LPARAM_NOT_POINTER ) ;
		     }     
	      }

# endif

	return ( TDialog :: PreProcessMsg ( msg ) ) ;
     }


