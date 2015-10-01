/**************************************************************************/
/*                                                                        */
/*  FloatMnu.cpp  -  Christian Vigh, 95/10.                               */
/*                                                                        */
/*  Implémentation de la classe TFloatingGadgetWindow.                    */
/*                                                                        */
/**************************************************************************/



# pragma	option -k- -N- -r- -v- -vi- -y-
# include	<owl/owlall.h>
# pragma	hdrstop
# pragma	option -k. -N. -r. -v. -vi. -y.


# include	"ChopJet.h"



DEFINE_RESPONSE_TABLE1 ( TFloatingGadgetWindow, TFloatingFrame )
	EV_COMMAND ( CM_CLOSE_POPUP, CmClosePopup ),
	EV_WM_SYSCOMMAND,
	EV_WM_SETFOCUS,
	EV_WM_KILLFOCUS,
END_RESPONSE_TABLE ;




/**************************************************************************/
/*                                                                        */
/*  Constructeur.                                                         */
/*                                                                        */
/**************************************************************************/

TFloatingGadgetWindow :: TFloatingGadgetWindow  ( TWindow *		owner,
						  char *		title,
						  TGadgetMenuWindow *	client,
						  BOOL			shrink,
						  int			capheight,
						  BOOL			popuppal )
	: TFloatingFrame ( owner, title, client, shrink, capheight, popuppal )
    {
	GadgetMenuWindow = client ;

// Attention !!  Cette fenêtre doit être popup, sinon elle ne reçoit jamais de
// focus
	Attr. Style = WS_POPUP | WS_BORDER |  
		      WS_MINIMIZEBOX | WS_CLIPCHILDREN | WS_CLIPSIBLINGS ;
	Minimized = FALSE ;
	Attr. AccelTable = TResId ( "POPUPMENU_ACCEL" ) ;
     }







/**************************************************************************/
/*                                                                        */
/*  TFloatingGadgetWindow :: Create -                                     */
/*      Crée la fenêtre et lui donne le focus.                            */
/*                                                                        */
/**************************************************************************/

BOOL	TFloatingGadgetWindow :: Create ( void )
   {
	int			X	=  Attr. X,
				Y	=  Attr. Y ;
	register int		Status 	=  TFloatingFrame :: Create ( ) ;


	ClientRect = GetClientWindow ( ) -> GetWindowRect ( ) ;	// Conserver la taille du menu


// Voir  EvSysCommand pour une explication du code qui suit
	TWindow *		OldParent = Parent ;


	if  ( ! ( GetWindowLong ( GWL_STYLE )  &  WS_CHILD ) )
		Parent = 0 ;
	SetWindowPos ( 0, X, Y, 0, 0, SWP_NOSIZE | SWP_NOZORDER ) ;
	Parent = OldParent ;


	GetClientWindow ( ) -> SetFocus ( ) ;
	GetClientWindow ( ) -> EnableWindow ( TRUE ) ;

	return  ( Status ) ;
     }




/**************************************************************************/
/*                                                                        */
/*  TFloatingGadgetWindow :: EvsysCommand -                               */
/*      "Enroule" ou "déroule" la fenêtre s'il s'agit d'une commande      */
/*      SC_MINIMIZE.                                                      */
/*                                                                        */
/**************************************************************************/

 void	TFloatingGadgetWindow :: EvSysCommand  ( uint		cmdtype,
						 TPoint &        p )
   {
// Laisser faire la classe de base pour tout ce qui n'est pas SC_MINIMIZE
	if  ( cmdtype  !=  SC_MINIMIZE )
	   {
		if  ( cmdtype  ==  SC_CLOSE  &&  ! p. x  &&  ! p. y )
			Parent -> HandleMessage ( WM_SYSCOMMAND, SC_CLOSE, 0L ) ;
		else
			TFloatingFrame :: EvSysCommand ( cmdtype, p ) ;
		return ;
	    }



// ***   OWLBUG !!!   ***
//
// Lors d'un SetWindowPos, TFrameWindow :: ResizeClientWindow est appelée.
// Si le flag wfShrinkToClient est mis, cette andouille de fonction fait un
// MoveWindow, exprimé en coordonnées relatives à la partie cliente du parent
// Or, pour les fenêtres popup (comme celle-ci), les coordonnées du MoveWindow
// doivent être exprimées en coordonnées écran. 
// Conséquence : à chaque SetWindowPos, la fenêtre se déplace d'une distance
// équivalente à la différence entre le point (0,0) de la fenêtre cliente de
// TCheopsMenuWindow et le coin supérieur gauche de l'écran. 
// Pour parer à ceci, un seul moyen : mettre le membre Parent à 0 le temps du
// move pour que ResizeClientWindow ne fasse pas de conversion de coordonnées.
	TWindow *	OldParent = Parent ;


	if  ( ! ( GetWindowLong ( GWL_STYLE )  &  WS_CHILD ) )
		Parent = 0 ;


	int		MoveDelta	=  GetSystemMetrics ( SM_CYMENU ) ;
	TWindow *	Client		=  GetClientWindow ( ) ;
	register int	Height		=  ClientRect. Height ( ),
			Width		=  ClientRect. Width ( ) ;



//  La fenêtre n'est pas minimisée : la réduire
	if  ( ! Minimized )
	   {
		while  ( Height )
		   {
			if  ( Height  <  MoveDelta )
				Height = 0 ;
			else
				Height -= MoveDelta ;

			Client -> SetWindowPos ( 0, 0, 0, Width, Height,
				SWP_NOMOVE | SWP_NOZORDER ) ;
		     }

		Minimized = TRUE ;
	     }
	else 
	   {
		Client -> SetWindowPos ( 0, 0, 0, Width, Height,
			SWP_NOMOVE | SWP_NOZORDER ) ;
		Minimized = FALSE ;
	     }

	Parent = OldParent ;
     }







/**************************************************************************/
/*                                                                        */
/*  TFloatingGadgetWindow :: SetFocus, KillFocus -                        */
/*      Gestion du focus.                                                 */
/*                                                                        */
/**************************************************************************/

void	TFloatingGadgetWindow :: EvSetFocus  ( HWND  lost )
   {
	BringWindowToTop ( ) ;
	TFloatingFrame :: EvSetFocus ( lost ) ;
	GetClientWindow ( ) -> SetFocus ( ) ;
	GetClientWindow ( ) -> EnableWindow ( TRUE ) ;
    }



void	TFloatingGadgetWindow :: EvKillFocus  ( HWND  gained )
   {
	TFloatingFrame :: EvKillFocus ( gained ) ;
    }                          





/**************************************************************************/
/*                                                                        */
/*  TFloatingGadgetWindow :: PreProcessMsg -                              */
/*      Ne fait pas grand-chose.                                          */
/*                                                                        */
/**************************************************************************/

bool	TFloatingGadgetWindow :: PreProcessMsg ( MSG &  msg )
   {
	return  ( TFloatingFrame :: PreProcessMsg ( msg ) ) ;
     }







/**************************************************************************/
/*                                                                        */
/*  TFloatingGadgetWindow :: Paint -                                      */
/*      Force la fenêtre cliente à se redessiner.                         */
/*                                                                        */
/**************************************************************************/

void	TFloatingGadgetWindow :: Paint  ( TDC &  dc, BOOL  erase, TRect &  rect )
   {
	TFloatingFrame :: Paint ( dc, erase, rect ) ;

// Forwarder le paint à la partie client
	GadgetMenuWindow -> Invalidate ( ) ;
	GadgetMenuWindow -> UpdateWindow ( ) ;
    }



