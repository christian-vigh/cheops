/**************************************************************************/
/*                                                                        */
/*  Unix.cpp  -  Christian Vigh, 94/11/16.                                */
/*                                                                        */
/*  Implémentation des classes proposées à Unix.                          */
/*                                                                        */
/**************************************************************************/


# include	"AllTerm.h"
# pragma	hdrstop

# include	"UnixApp.h"
# include	"Event.h"
# include	"Icon.h"
# include	"UnixDlg.h"
# include	"Unix.h"



/**************************************************************************/
/**************************************************************************/
/*****                                                                *****/
/*****   Classe  TUnixGauge :                                         *****/
/*****                                                                *****/
/**************************************************************************/
/**************************************************************************/

TUnixGauge :: TUnixGauge 	( TWindow *	parent,
				  char *        title,
				  char *	format,
				  int		x,
				  int		y,
				  int		w,
				  int		h,
				  int		low,
				  int		high,
				  int		margin )
	: TWindow ( parent, title )
   {
	TRect		Rect ;


	Parent -> GetClientRect ( Rect ) ;


	if  ( x  ==  CW_USEDEFAULT  &&  w  !=  CW_USEDEFAULT )
		Attr. X = ( Rect. right - Rect. left - w ) / 2 ;
	else
		Attr. X = x ;

	if  ( y  ==  CW_USEDEFAULT   &&  h  !=  CW_USEDEFAULT )
		Attr. Y = ( Rect. bottom - Rect. top - h ) / 2 ;
	else
		Attr. Y = y ;

	Attr. H = h + GetSystemMetrics ( SM_CYCAPTION ) ;
	Attr. W = w ;
	Attr. Style  =  WS_POPUP | WS_BORDER | WS_CAPTION |
			WS_VISIBLE ;

	RangeLow  = low ;
	RangeHigh = high ;

	Gauge = new TGauge ( this, format, 0, 0, 0, w, h, TRUE, margin ) ;
    }






/**************************************************************************/
/**************************************************************************/
/*****                                                                *****/
/*****   Classe  TUnixWindow                                          *****/
/*****                                                                *****/
/**************************************************************************/
/**************************************************************************/
		    


DEFINE_RESPONSE_TABLE1  ( TUnixWindow, TWindow )
	EV_WM_PAINT,
	EV_MESSAGE ( WM_SETICON	, EvSetIcon ),
END_RESPONSE_TABLE ;