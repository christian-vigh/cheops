/**************************************************************************/
/*                                                                        */
/*  Icon.h  -  Christian Vigh, 94/10/31.                                  */
/*                                                                        */
/*  D�finition de l'object Icone de fen�tre.                              */
/*                                                                        */
/**************************************************************************/


template  <class WindowObject>  class  TIcon 
   {
	private :
		HICON		HIcon ;
		BOOL		NeedsDelete ;


	public :
		TIcon  ( )   
		  { 
			HIcon 	    = 0 ; 
			NeedsDelete = FALSE 
		   }
		   

	       ~TIcon  ( )   
		  { 
			if  ( NeedsDelete  &&  HIcon )
				:: DestroyIcon ( HIcon ) ;
		   }
		

		void	SetIcon  ( long int  iconid )
		   {
			NeedsDelete 	=  FALSE ;
			HIcon 		=  :: LoadIcon ( NULL, ( LPSTR ) iconid ) ;
		    }


		BOOL	Paint  ( void )
		   {
			