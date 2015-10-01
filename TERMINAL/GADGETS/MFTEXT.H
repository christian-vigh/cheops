/**************************************************************************/
/*                                                                        */
/*  MFText.h  -  Christian Vigh, 95/11.                                   */
/*                                                                        */
/*  Object Textgadget permettant l'utilisation de plusieurs polices de    */
/*  caractères et de plusieurs couleurs.                                  */
/*                                                                        */
/**************************************************************************/



# if	! defined ( __MFTEXT_H__ )
# 	  define    __MFTEXT_H__



class	TMultiFontTextGadget :  public  TAutoTextGadget
   {
	public :
		TMultiFontTextGadget  ( int		id 	   =  0,
					TBorderStyle 	style      =  Recessed,
					uint		numchar    =  10,
					char *		text       =  0,
					int		margins    =  4,
					BOOL		clickable  =  FALSE ) ;

	       ~TMultiFontTextGadget  ( ) ;



		void	SetText  	( char  *	text ) ;
		void	GetDesiredSize	( TSize &	size ) ;


	protected :
		void	Paint		( TDC &		dc ) ;



	protected :
	// Chaque portion de texte ayant des attributs spécifiques se voit
	// associée une structure TTextEntry, qui contient un pointeur sur
	// la police correspondante
	//
		struct TTextEntry 
		   {
			TFont *			Font ;
			COLORREF		BackgroundColor,	
						ForegroundColor ;
			char *			Text ;
			int			TextLength ;
			struct  TTextEntry *	Next ;
		    } ;		


	protected :
		TTextEntry *	TextEntries,
			   * 	LastTextEntry ;
		char *		Text ;


	private :
		char *		GetTextElement ( char * 	text,
						 int &		size,
						 LOGFONT &	original,
						 LOGFONT &	newone,
						 COLORREF	oldfore,
						 COLORREF	oldback,
						 COLORREF &	newfore,
						 COLORREF &	newback ) ;
		
		char *		GetTextElement ( char * 	input,
						 char *		format,
						 int &		text_size ) ;

		void		DeleteEntries  ( void ) ;
   } ;




# endif		/*  __MFTEXT_H__  */
