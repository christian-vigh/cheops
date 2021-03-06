/**************************************************************************/
/*                                                                        */
/*  AutoText.h  -  Christian Vigh, 95/10.                                 */
/*                                                                        */
/*  Gadget texte avec :                                                   */
/*      . Ajustement automatique de la largeur                            */
/*      . Possibilit� de changer de fonte � la vol�e (en surchargeant     */
/*        GetFont dans une classe d�riv�e).                               */
/*      . Possibilit� d'envoyer une commande sur simple clic du gadget    */
/*  Si l'ajustement automatique n'est pas demand�, se comporte comme      */
/*  TTextGadget.                                                          */
/*                                                                        */
/**************************************************************************/


# if	! defined ( __AUTOTEXT_H__ )
# 	  define    __AUTOTEXT_H__



class	TAutoTextGadget 	:  public  TTextGadget
   {
	public :
		enum  TAlign
		   {
			Left, Center, Right, AutoAdjust 
		    } ;
			
			
	public :
		TAutoTextGadget ( int		id	  =  0,
				  TBorderStyle  style     =  Recessed,
				  TAlign		  =  AutoAdjust,
				  int		numchar   =  10,
				  char *	text 	  =  0,
				  int		margins   =  4,
				  BOOL		clickable =  FALSE ) ;


		virtual TFont &	GetFont  	( void ) ;
		virtual void    GetDesiredSize	( TSize &	sz ) ;
		void		SetText		( const char *	text ) ;
		void		CommandEnable   ( void ) ;


	protected :
		void		LButtonDown	( uint  	modkeys,
						  TPoint &	pos ) ;
		void		LButtonUp	( uint		modkeys,
						  TPoint & 	pos ) ;


	private :
		TAlign		Align ;
		BOOL		Clickable ;
		BOOL		ButtonPressed ;
		

		void	GetTextSize  ( TSize &	sz ) ;		
     } ;



# endif	 	/*  __AUTOTEXT_H__  */		
