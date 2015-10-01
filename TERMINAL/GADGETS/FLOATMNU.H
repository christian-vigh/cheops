/**************************************************************************/
/*                                                                        */
/*   FloatMnu.h -  Christian Vigh, 95/10.                                 */
/*                                                                        */
/*   Fenêtre de type floating frame ayant une TGadgetMenuWindow dans sa   */
/*   partie cliente.                                                      */
/*                                                                        */
/**************************************************************************/


# if	! defined ( __FLOATMNU_H__ )
#	  define    __FLOATMNU_H__ 



# define	CM_CLOSE_POPUP		1000



class	TFloatingGadgetWindow	:  public TFloatingFrame
   {
	private :
		TGadgetMenuWindow *	GadgetMenuWindow ;
		TRect			ClientRect ;


	public :
		TFloatingGadgetWindow  ( TWindow *		owner,
					 char *			title,
					 TGadgetMenuWindow *	client,
					 BOOL			shrink		=  TRUE,
					 int			capheight       =  75,
					 BOOL			popuppal	=  TRUE ) ;




		BOOL		Create 		( void ) ;



	// Ferme le menu
		void	CmClosePopup  ( void )
		   {
			CloseWindow ( 0 ) ;
		    }



		BOOL	IsMinimized  ( void )
		   {
			return  ( Minimized ) ;
		    }


	protected :
		BOOL			Minimized ;



		void	EvSysCommand   ( uint			cmdtype,
					 TPoint	&		position ) ;	

		
		void	EvSetFocus     ( HWND  			lost ) ;
		void	EvKillFocus    ( HWND  			gained ) ;

		bool	PreProcessMsg  ( MSG &                  msg ) ;



		void	Paint  		( TDC &  		dc, 
					  BOOL  		erase, 
					  TRect &  		rect ) ;


	DECLARE_RESPONSE_TABLE ( TFloatingGadgetWindow ) ;
     } ;





# endif		/*  __FLIATMNU_H__  */