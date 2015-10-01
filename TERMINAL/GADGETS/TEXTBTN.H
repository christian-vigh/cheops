/**************************************************************************/
/*                                                                        */
/*  TextBtn.h  -  CHristian Vigh, 95/10.                                  */
/*                                                                        */
/*  Classe similaire à TButtonGadget, permettant de base la gestion de    */
/*  bouton avec du texte uniquement.                                      */
/*                                                                        */
/**************************************************************************/



# if	! defined ( __TEXTBTN_H__ )
#	  define    __TEXTBTN_H__



class	TTextButtonGadget	:  public  TGadget 
   {
	public :
		enum  TState  		// Etat du bouton
		  {
			Up, 
			Down, 
			Indeterminate
		    } ;

		enum  TType		// Type du bouton
		   {
			Exclusive,	// Un seul bouton down
			NonExclusive,	// Flip flop
			Command		// bouton normal
		    } ;


	public :
	// Constructeur et destructeur
	//
		TTextButtonGadget  ( int		id,
				     TType		type	   = Command,
				     bool		enabled    = true,
				     int		numchar    = 10 ) ;


	       ~TTextButtonGadget ( ) ;


	// Etat du bouton
	//
		virtual void   SetButtonState		( TState 	state ) ;

		virtual TState GetButtonState		( void ) 
		   {
			return ( State ) ; 
		    }


	// Texte du bouton
	//
		void		SetText			( char *	text ) ;

		char *		GetText			( void )
		   {
			return  ( Text ) ;
		    }


	//  Taille
	//
		void          GetDesiredSize   	( TSize & 	size ) ;


		//  Divers
		//
		void          CommandEnable 	( void ) ;
		void          SysColorChange	( void ) ;


		// Fonctions de base de TGadget
		//
		virtual void  Paint		( TDC &  	dc ) ;
		virtual void  Invalidate	( void ) ;
		virtual void  PaintContents	( TDC &    	dc,
						  TRect &	rect ) ;


		// Changement d'état du bouton
		//
		void          LButtonDown	( uint 		modKeys, 
						  TPoint & 	p ) ;
		void          MouseMove		( uint 		modKeys, 
						  TPoint & 	p ) ;
		void          MouseEnter	( uint 		modKeys, 
						  TPoint &  	p ) ;
		void          MouseLeave	( uint 		modKeys, 
						  TPoint & 	p ) ;
		void          LButtonUp		( uint 		modKeys, 
						  TPoint & 	p ) ;

		virtual void  BeginPressed	( TPoint & 	p ) ;
		virtual void  CancelPressed	( TPoint & 	p ) ;
		virtual void  Activate		( TPoint & 	p ) ;




	protected :
		char *		Text ;
		int		CharacterCount ;
		BOOL		Pressed ;
		TType		Type ;
		TState		State ;
		int		HiliteUnits,
				ShadowUnits ;



	private:
		void          CheckExclusively	( void ) ;

    } ;




# endif		/*  __TEXTBTN_H__  */