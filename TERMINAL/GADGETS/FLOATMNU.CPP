/**************************************************************************/
/*                                                                        */
/*  FloatMnu.cpp  -  Christian Vigh, 95/10.                               */
/*                                                                        */
/*  Impl�mentation de la classe TFloatingGadgetWindow.                    */
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

// Attention !!  Cette fen�tre doit �tre popup, sinon elle ne re�oit jamais de
// focus
	Attr. Style = WS_POPUP | WS_BORDER |  
		      WS_MINIMIZEBOX | WS_CLIPCHILDREN | WS_CLIPSIBLINGS ;
	Minimized = FALSE ;
	Attr. AccelTable = TResId ( "POPUPMENU_ACCEL" ) ;
     }







/**************************************************************************/
/*                                                                        */
/*  TFloatingGadgetWindow :: Create -                                     */
/*      Cr�e la fen�tre et lui donne le focus.                            */
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
/*      "Enroule" ou "d�roule" la fen�tre s'il s'agit d'une commande      */
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
// Lors d'un SetWindowPos, TFrameWindow :: ResizeClientWindow est appel�e.
// Si le flag wfShrinkToClient est mis, cette andouille de fonction fait un
// MoveWindow, exprim� en coordonn�es relatives � la partie cliente du parent
// Or, pour les fen�tres popup (comme celle-ci), les coordonn�es du MoveWindow
// doivent �tre exprim�es en coordonn�es �cran. 
// Cons�quence : � chaque SetWindowPos, la fen�tre se d�place d'une distance
// �quivalente � la diff�rence entre le point (0,0) de la fen�tre cliente de
// TCheopsMenuWindow et le coin sup�rieur gauche de l'�cran. 
// Pour parer � ceci, un seul moyen : mettre le membre Parent � 0 le temps du
// move pour que ResizeClientWindow ne fasse pas de conversion de coordonn�es.
	TWindow *	OldParent = Parent ;


	if  ( ! ( GetWindowLong ( GWL_STYLE )  &  WS_CHILD ) )
		Parent = 0 ;


	int		MoveDelta	=  GetSystemMetrics ( SM_CYMENU ) ;
	TWindow *	Client		=  GetClientWindow ( ) ;
	register int	Height		=  ClientRect. Height ( ),
			Width		=  ClientRect. Width ( ) ;



//  La fen�tre n'est pas minimis�e : la r�duire
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
/*      Force la fen�tre cliente � se redessiner.                         */
/*                                                                        */
/**************************************************************************/

void	TFloatingGadgetWindow :: Paint  ( TDC &  dc, BOOL  erase, TRect &  rect )
   {
	TFloatingFrame :: Paint ( dc, erase, rect ) ;

// Forwarder le paint � la partie client
	GadgetMenuWindow -> Invalidate ( ) ;
	GadgetMenuWindow -> UpdateWindow ( ) ;
    }



