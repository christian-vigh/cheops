/**************************************************************************/
/*                                                                        */
/*  ColorCB.cpp -  Christian Vigh, 95/11.                                 */
/*                                                                        */
/*  Impl�mentation de la classe TColorControlBar.                         */
/*                                                                        */
/**************************************************************************/



# pragma	option -k- -N- -r- -v- -vi- -y-
# include	<owl/owlall.h>
# pragma	hdrstop
# pragma	option -k. -N. -r. -v. -vi. -y.


# include	"ChopJet.h"




// Table de r�ponses
//
DEFINE_RESPONSE_TABLE1 ( TColorControlBar, TControlBar ) 
	EV_WM_CTLCOLOR,
	EV_WM_SYSCOLORCHANGE,
END_RESPONSE_TABLE ;




/**************************************************************************/
/*                                                                        */
/*  TColorControlBar :: EvSysColorChange -                                */
/*      Synchronise la couleur de la barre avec les nouvelles couleurs du */
/*      syst�me.                                                          */
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
/*      R�pond � la demande des contr�les de d�finir les couleurs de      */
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
/*      Change les couleurs des contr�les, sans effectuer de remise �     */
/*      jour � l'�cran.                                                   */
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


	



