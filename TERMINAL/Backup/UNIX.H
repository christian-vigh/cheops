/**************************************************************************/
/*                                                                        */
/*  Unix.h  -  Christian Vigh, 94/11/16.                                  */
/*                                                                        */
/*  Classes proposées à Unix :                                            */
/*                                                                        */
/*  TUnixGauge -                                                          */
/*      Jauge.                                                            */
/*                                                                        */
/*  TUnixWindow -                                                         */
/*      Fenêtre normale.                                                  */
/*                                                                        */
/**************************************************************************/



/**************************************************************************/
/**************************************************************************/
/*****                                                                *****/
/*****   Classe  TUnixGauge :                                         *****/
/*****                                                                *****/
/**************************************************************************/
/**************************************************************************/

# define	LEFT_MARGIN			20
# define	TOP_MARGIN			20


class	TUnixGauge	:  public  TWindow
    {
	private :
		TGauge *		Gauge ;
		int			RangeLow,
					RangeHigh ;


	public :
		TUnixGauge 	( TWindow *	parent,
				  char *        title,
				  char *	format,
				  int		x,
				  int		y,
				  int		w,
				  int		h,
				  int		low,
				  int		high,
				  int		margin ) ;


	       ~TUnixGauge ( )
		  {
			Gauge -> Destroy ( ) ;
			delete  Gauge ;
		   }



		void	SetValue  ( int	  value )
		   {
			Gauge -> SetValue ( value ) ;
		    }


		void	SetupWindow  ( void )
		   {
			TWindow :: SetupWindow ( ) ;
			Gauge -> SetRange ( RangeLow, RangeHigh ) ;
			Gauge -> SetValue ( 0 ) ;
		    }
	} ;




/**************************************************************************/
/**************************************************************************/
/*****                                                                *****/
/*****   Classe  TUnixGaugeWindow :                                   *****/
/*****                                                                *****/
/**************************************************************************/
/**************************************************************************/

class	TUnixWindow :  	public  TWindow
   {
	private :
		TUnixApplication *	UnixApplication ;
		TWindow *		ParentWindow ;
		int			ProcessId ;

	public :
		integer16		ObjectId ;
		TUnixIcon		Icon ;
		
		
	public :
		TUnixWindow ( TUnixApplication *	app,
			      TUnixMessage *		msg,
			      TWindow *			parent,
			      char *			caption,
			      long int			style,
			      int			X,
			      int			Y,
			      int			CX,
			      int			CY )
			: TWindow ( parent, caption )
		   {
			UnixApplication 	=  app ;
			ParentWindow		=  parent ;
			ProcessId		=  msg -> ProcessId ;

			Attr. Style 	= style ;
			Attr. X	   	= X ;
			Attr. Y		= Y ;
			Attr. W		= CX ;
			Attr. H		= CY ;
		     }



		void	EvPaint ( void )
		   {
			if  ( ! Icon. Paint ( HWindow ) )
				TWindow :: EvPaint ( ) ;
		    }


		LRESULT	 EvSetIcon  ( WPARAM  predefined, LPARAM  lparam )
		   {
			Icon. SetIcon ( predefined, lparam ) ;
			return ( 0L ) ;
		    }

	DECLARE_RESPONSE_TABLE  ( TUnixWindow ) ;
	
   } ;
			      

