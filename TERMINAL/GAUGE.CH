/**************************************************************************/
/*                                                                        */
/*  Gauge.ch  -  Christian Vigh, 94/10/04.                                */
/*                                                                        */
/*  Gestion d'une fenêtre contenant une gauge.                            */
/*                                                                        */
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
