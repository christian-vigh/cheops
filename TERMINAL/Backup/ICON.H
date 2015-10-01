/**************************************************************************/
/*                                                                        */
/*  Icon.h  -  Christian Vigh, 94/10/31.                                  */
/*                                                                        */
/*  Gestion de l'icône d'une fenêtre d'application.                       */
/*                                                                        */
/**************************************************************************/


class  TUnixIcon 
   {
	private :
		HICON		HIcon ;
		BOOL		NeedsDelete ;


	public :
		TUnixIcon  ( void )   
		  { 
			HIcon 	    = 0 ; 
			NeedsDelete = FALSE ;
		   }
		   

	       ~TUnixIcon  ( )   
		  { 
			if  ( NeedsDelete  &&  HIcon )
				:: DestroyIcon ( HIcon ) ;
		   }
		

		void	SetIcon  ( int	size, long int  iconid )
		   {
			if  ( ! size )
			   {
				NeedsDelete 	=  FALSE ;
				HIcon 		=  :: LoadIcon ( NULL, ( LPSTR ) iconid ) ;
			    }
			else
			   {
				NeedsDelete 	=  TRUE ;
				HIcon		=  CreateIconFromTemplate ( ( char far * )  iconid, size ) ;
			    }
		    }


		BOOL	Paint  ( HWND   hwnd )
		   {
			if  ( IsIconic ( hwnd )  &&  HIcon )
			   {
				TPaintDC	DC ( hwnd ) ;
				:: DrawIcon ( DC, 0, 0, HIcon ) ;
				return ( TRUE ) ;
			     }
			else
				return ( FALSE ) ;
		      }
    } ;