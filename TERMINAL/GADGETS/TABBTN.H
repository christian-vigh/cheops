/**************************************************************************/
/*                                                                        */
/*  TabBtn.h  -  Christian Vigh, 95/10.                                   */
/*                                                                        */
/*  Classe permettant de gérer des boutons sous forme d'onglet, avec      */
/*  ou sans chevauchement.                                                */
/*                                                                        */
/**************************************************************************/



# if 	! defined ( __TABBTN_H__ )
#	  define    __TABBTN_H__ 



class  TTabButtonGadget 	:  public  TShapeButtonGadget
   {
	public :
	// Constructeur et destructeur
	//
		TTabButtonGadget   ( int		id,
				     bool		enabled   = true,
				     bool		overlap   = false,
				     int		numchar   = 10,
				     bool		shadowing = true ) ;



		virtual void  GetDesiredSize	( TSize &	Size ) ;
		virtual void  SetButtonState    ( TState 	newstate ) ;



	protected :
		virtual BOOL		GetRegion  	( TRegion &		region,
							  TRegionType		type ) ;
		virtual COLORREF 	GetRegionColor	( TRegionType		type ) ;
		virtual void		Paint		( TDC &			dc ) ;



	private :
		BOOL		Overlap ;
		BOOL		ShadowingEnabled ;
		int		dX ;		// Valeur à ajouter à la largeur
		int		cySoftCorner ;	// Hauteur d'un coin "adouci"

		void		ComputePoints		( TPoint *	points ) ;
		void		CheckOverlappedRegion   ( TRegion &	region ) ;
      } ;




# endif		/*  __TABBTN_H__  */