/**************************************************************************/
/*                                                                        */
/*  ShapeBtn.h  -  Christian Vigh, 95/10.                                 */
/*                                                                        */
/*  Classe abstraite permettant de gérer des boutons de n'importe quelle  */
/*  forme.                                                                */
/*                                                                        */
/**************************************************************************/


# if	! defined ( __SHAPEBTN_H__ )
#         define    __SHAPEBTN_H__



class	TShapeButtonGadget	:  public  TTextButtonGadget 
   {
	public :
	// Constructeur et destructeur
	//
		TShapeButtonGadget ( int		id,
				     TType		type	= Command,
				     bool		enabled = true,
				     int		numchar = 10 ) ;

	       ~TShapeButtonGadget ( ) ;	


		virtual void  GetDesiredSize	( TSize &	size )
		   {
			TTextButtonGadget :: GetDesiredSize ( size ) ;
		    }


		void          Paint		( TDC &  	dc ) ;
		virtual BOOL  PtIn		( TPoint & 	p ) ;
		virtual void  Invalidate	( void ) ;	

		


	protected :
		enum  TRegionType             	// Partie du gadget :
		   {
				OuterRegion,		// Partie englobante du gadget (VisibleRegion plus les régions non visibles)
				VisibleRegion,  	// Partie non recouverte par un autre gadget
				BorderRegion,		// Tout le gadget, bord compris
				InnerRegion,		// Intérieur (avec ou sans ombrage)
				ShadowRegion,		// Partie ombrée
				HiliteRegion,		// Partie éclairée
				DrawableRegion,		// Partie dessinable
				OtherShadowRegion,      // Ombre (optionnelle) portée par d'autres gadgets
		     } ;



		int		cxBorder,
				cyBorder ;	// Taille d'une bordure


	// Cette fonction doit retourner une région correspondant à l'élément
	// indiqué, et retourner TRUE si la région en question n'est pas vide
		virtual BOOL		GetRegion  		( TRegion &  	r, 
								  TRegionType	type ) 	=  0 ;
		virtual COLORREF 	GetRegionColor		( TRegionType	type ) ;



	// Gestion des régions à updater
		TRegion	*	IntersectionRegion ;
		BOOL		MustIntersect ;


		void	SetIntersectionRegion ( TRegion *  r )
		   {
			IntersectionRegion = r ;
			MustIntersect = TRUE ;
		    }

    } ;



# endif		/*  __SHAPEBTN_H__  */