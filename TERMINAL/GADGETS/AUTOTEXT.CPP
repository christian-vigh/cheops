/**************************************************************************/
/*                                                                        */
/*  AutoText.cpp  -  Christian Vigh, 95/11.                               */
/*                                                                        */
/*  Gadget texte avec ajustement automatique.                             */
/*                                                                        */
/**************************************************************************/



# pragma	option -k- -N- -r- -v- -vi- -y-
# include	<owl/owlall.h>
# pragma	hdrstop
# pragma	option -k. -N. -r. -v. -vi. -y.


# include	"ChopJet.h"




/**************************************************************************/
/*                                                                        */
/*  CommandEnabler pour ce gadget.                                        */
/*                                                                        */
/**************************************************************************/

class 	TAutoTextGadgetEnabler : 	public TCommandEnabler 
   {
	public :
		TAutoTextGadgetEnabler   ( HWND  		hWndReceiver, 
					   TAutoTextGadget * 	g )
			: TCommandEnabler ( g -> GetId (), hWndReceiver ) 
		   {
		      Gadget = g ;
		    }



		void  Enable	( bool  enable )
		   {
			TCommandEnabler :: Enable ( enable ) ;
			Gadget -> SetEnabled ( enable ) ;
		     }


		void  SetText ( const char *  text )
		   { 
			Gadget -> SetText ( text ) ;
		    }


		void  SetCheck ( int )
		   { 
		     }


	protected :
		TAutoTextGadget *  Gadget ;
    } ;









/**************************************************************************/
/*                                                                        */
/*  Constructeur.                                                         */
/*                                                                        */
/**************************************************************************/

TAutoTextGadget :: TAutoTextGadget  ( int			id,
				      TBorderStyle		border_style,
				      TAlign			align,
				      int			numchars,
				      char *			text,
				      int			margins,
				      BOOL			clickable )
	: TTextGadget ( id, border_style,  TTextGadget :: Left, numchars, text ) 
   {
	Margins. Left  = 
	Margins. Right = margins ;
	Align 	       = align ;
	ButtonPressed  = FALSE ;
	Clickable      = clickable ;	

	switch  ( Align )
	   {
		case	Right :
			TTextGadget :: Align = TTextGadget :: Right ;
			break ;

		case	Center :
			TTextGadget :: Align = TTextGadget :: Center ;
			break ;
	     }
    }






/**************************************************************************/
/*                                                                        */
/*  TAutoTextGadget :: CommandEnable -                                    */
/*      Envoie un message Command enable à la fenêtre gadget.             */
/*                                                                        */
/**************************************************************************/


void	TAutoTextGadget :: CommandEnable ( void )
   {
	if  ( Clickable )
		Window -> Parent -> HandleMessage ( WM_COMMAND_ENABLE, 0,
			( LPARAM ) & TAutoTextGadgetEnabler ( * Window -> Parent, this ) ) ;
    }







/**************************************************************************/
/*                                                                        */
/*  TAutoTextGadget :: SetText -                                          */
/*      Redéfinit le texte du gadget.                                     */
/*                                                                        */
/**************************************************************************/

void	TAutoTextGadget :: SetText  ( const char *	text )
   {
	char *		oldtext = Text ;


	TTextGadget :: SetText ( text ) ;


// Indiquer à la fenêtre gadget qu'on a changé de taille
	if  ( Window  &&  oldtext  !=  Text )
		Window -> GadgetChangedSize ( * this ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TAutoTextGadget :: GetDesiredSize -                                   */
/*      Retourne la taille désirée.                                       */
/*                                                                        */
/**************************************************************************/

void	TAutoTextGadget :: GetDesiredSize  ( TSize &  Size )
   {
	if  ( Align  !=  AutoAdjust )
		TTextGadget :: GetDesiredSize ( Size ) ;
	else
	   {
		TSize		TextSize ;


		TGadget :: GetDesiredSize  ( Size ) ;
		GetTextSize ( TextSize ) ;
		Size. cx  +=  TextSize. cx ;
		Size. cy  +=  TextSize. cy ;
	     }
    }




/**************************************************************************/
/*                                                                        */
/*  TAutoTextGadget :: GetTextSize -                                      */
/*      Retourne la taille du texte.                                      */
/*                                                                        */
/**************************************************************************/

void  TAutoTextGadget :: GetTextSize  ( TSize &  Size )
   {
	if  ( Window  &&  Text  &&  TextLen )
	   {
		TScreenDC	DC ;


		DC. SelectObject ( GetFont ( ) ) ;
		Size = DC. GetTextExtent ( Text, TextLen ) ;
	     }
	else
		Size. cx = Size. cy = 0 ;
     }






/**************************************************************************/
/*                                                                        */
/*  TAutoTextGadget :: GetFont -                                          */
/*      Retourne la police pour ce gadget.                                */
/*                                                                        */
/**************************************************************************/

TFont &	  TAutoTextGadget :: GetFont  ( void )
   {
	return  ( Window -> GetFont ( ) ) ;
    }






/**************************************************************************/
/*                                                                        */
/*  TAutoTextGadget :: LButtonDown, LButtonUp -                           */
/*      Gestion de l'envoi d'un message WM_COMMAND si le gadget texte est */
/*      cliquable.                                                        */
/*                                                                        */
/**************************************************************************/

void	TAutoTextGadget :: LButtonDown ( uint  modkeys, TPoint &  pos )
  {
	TTextGadget :: LButtonDown ( modkeys, pos ) ;

	if  ( Clickable )
		ButtonPressed = TRUE ;
   }




void	TAutoTextGadget :: LButtonUp  ( uint  modkeys, TPoint &  pos )
   {
	TTextGadget :: LButtonUp  ( modkeys, pos ) ;


	if  ( Clickable  &&  ButtonPressed )
		Window -> Parent -> PostMessage ( WM_COMMAND, GetId ( ) ) ;
	ButtonPressed = FALSE ;
    }
