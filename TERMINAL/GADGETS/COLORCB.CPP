/**************************************************************************/
/*                                                                        */
/*  ColorCB.cpp -  Christian Vigh, 95/11.                                 */
/*                                                                        */
/*  Implémentation de la classe TColorControlBar.                         */
/*                                                                        */
/**************************************************************************/



# pragma	option -k- -N- -r- -v- -vi- -y-
# include	<owl/owlall.h>
# pragma	hdrstop
# pragma	option -k. -N. -r. -v. -vi. -y.


# include	"ChopJet.h"




// Table de réponses
//
DEFINE_RESPONSE_TABLE1 ( TColorControlBar, TControlBar ) 
	EV_WM_CTLCOLOR,
	EV_WM_SYSCOLORCHANGE,
END_RESPONSE_TABLE ;




/**************************************************************************/
/*                                                                        */
/*  TColorControlBar :: EvSysColorChange -                                */
/*      Synchronise la couleur de la barre avec les nouvelles couleurs du */
/*      système.                                                          */
/*                                                                        */
/**************************************************************************/

void	TColorControlBar :: EvSysColorChange ( void )
   {
	SetColor ( ) ;
	TControlBar :: EvSysColorChange ( ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TColorControlBar :: EvCtlColor -                                      */
/*      Répond à la demande des contrôles de définir les couleurs de      */
/*      texte et de fond.                                                 */
/*                                                                        */
/**************************************************************************/

HBRUSH  TColorControlBar :: EvCtlColor  ( HDC  hdc, HWND   /* childwnd */, 
						    uint  /* ctltype */ )
   {
	:: SetBkColor   ( hdc, ControlBackground ) ;
	:: SetTextColor ( hdc, ControlForeground ) ;
	return  ( * ControlBrush ) ;
    }



/**************************************************************************/
/*                                                                        */
/*  TColorControlBar :: SetColor -                                        */
/*      Change les couleurs des contrôles, sans effectuer de remise à     */
/*      jour à l'écran.                                                   */
/*                                                                        */
/**************************************************************************/

void	TColorControlBar :: SetColor ( COLORREF  fore, COLORREF  back )
   {
	ControlBackground = back ;
	ControlForeground = fore ;

	if  ( ControlBrush ) 
		delete ControlBrush ;
	ControlBrush = new  TBrush ( ControlBackground ) ;
     }


	



