/**************************************************************************/
/*                                                                        */
/*  ShrinkSP.h  -  Christian Vigh, 95/11.                                 */
/*                                                                        */
/*  Gadget s�parateur "WideAsPossible" avec limite inf�rieure et          */
/*  sup�rieure pour la taille.                                            */
/*                                                                        */
/**************************************************************************/



# if	! defined ( __SHRINKSP_H__ )
#	  define    __SHRINKSP_H__




class	TShrinkSeparatorGadget  :  public  TSeparatorGadget
   {
	public :
		TShrinkSeparatorGadget  ( int		minwidth = -1,
					  int		maxwidth = -1 ) ;


	// Limites du gadget
		void	 SetLimits  ( int  minw, int maxw ) ;


		int	GetLowerLimit  ( void )
		   {
			return  ( MinWidth ) ;
		    }

		int	GetUpperLimit  ( void ) 
		   {
			return  ( MaxWidth ) ;
		    }



	// Contr�le 
		void	SetBounds  ( TRect &  bounds ) ;



	private :
		int		MinWidth,
				MaxWidth ;
    } ;



# endif		/*  __SHRINKSP_H__  */
