/**************************************************************************/
/*                                                                        */
/*  Gombo.h  -  Christian Vigh, 95/10.                                    */
/*           -  Jean-Luc Lebresne 95/11.											  */
/*                                                                        */
/*  ComboBox générique permettant entre autres, le dimensionnement        */
/*  automatique de la listbox en fonction de son contenu.                 */
/*                                                                        */
/*  ListBox générique permettant entre autre, le dimensionnement		     */
/*  automatique en fonction de son contenu.									     */
/**************************************************************************/



# if	! defined ( __GOMBO_H__ )
#         define    __GOMBO_H__

# define	Nb_display_max	6

/**************************************************************************/
/*  Outil de gestion des liste dans la partie entete des écrans de saisie.*/
/*		Ce differenci par la touche de visualisation [down].					  */
/**************************************************************************/
class	TEnhancedComboBox	:  	public  TComboBox
	{
	public :
		HWND		wpopup ;					/* identificateur de la fenetre de selection	*/
		bool		boolpopup ;	   		/*	informaion de remise à jour	*/
		char **	memo ;					/* table des texte à gerer	*/
		int		indexmax ;	   		/* index maximum en court : NB -1	*/
		int		x1, x2 ;	   		/*	distances d'alignement pour plus propre	*/
		int		base_display,			/*	premiere ligne affichée dans la memo	*/
				Nb_display ;			/*	nombre de lignes affichées	*/

		TEnhancedComboBox  (  TWindow * parent, int resId ) : TComboBox  ( parent, resId )
			{
			indexmax = -1 ;
			memo = ( char ** ) malloc ( 0 ) ;
			x1 = x2 = 0 ;
			wpopup = NULL ;
			boolpopup = FALSE ;
			base_display = 0 ;
			Nb_display = 0;
  			 }

# pragma	warn	-inl
		~TEnhancedComboBox  ( )
			{
			for  ( ; indexmax  >=  0; indexmax -- )
				free ( memo [ indexmax ] ) ;
			free ( memo ) ;
			 }
# pragma	warn	.inl

		void	SetupWindow   ( void ) ;
		void    Paint         ( TDC &, BOOL, TRect & ) ;
		BOOL	PreProcessMsg ( MSG &  msg ) ;

		LRESULT DefWindowProc ( uint msg, WPARAM wparam, LPARAM lparam ) ;
		virtual void EvMeasureItem ( uint ctrlid, MEASUREITEMSTRUCT far& meas );
		virtual void DrawItem     ( DRAWITEMSTRUCT far & drawInfo ) ;

		void	MovePopup	   ( HWND hwpopup, int large ) ;

		LRESULT EvAddString  	   ( WPARAM  key, LPARAM  param ) ;
		LRESULT EvInsertString 	   ( WPARAM  key, LPARAM  param ) ;
		LRESULT EvDeleteString 	   ( WPARAM  key, LPARAM  param ) ;
		LRESULT EvResetContent 	   ( WPARAM  key, LPARAM  param ) ;
		LRESULT EvGetCurSel    	   ( WPARAM  key, LPARAM  param ) ;
		LRESULT EvShowdropdown     ( WPARAM  key, LPARAM  param ) ;

		DECLARE_RESPONSE_TABLE     ( TEnhancedComboBox ) ;
	 } ;








/**************************************************************************/
/*  Outil de gestion des liste dans la partie ligne des écrans de saisie. */
/**************************************************************************/

class	TEnhancedListBox	:  	public  TListBox
	{
	public :
		TRect		EditRect ;
		HWND		wpopup ;
		bool		boolpopup ;
		char **		memo ;
		int		indexmax ;
		int		x1, x2 ;
		int		base_display,			/*	premiere ligne affichée dans la memo	*/
				Nb_display ;			/*	nombre de lignes affichées	*/


		TEnhancedListBox ( TWindow * 		Parent, 
				   int 			Id, int x, int y, int w, int h ) 
				: TListBox( Parent, Id, x, y, w, h )
			{
			Attr. Style |= LBS_OWNERDRAWFIXED | LBS_HASSTRINGS | LBS_DISABLENOSCROLL ;

			indexmax = -1 ;
			memo = ( char ** ) malloc ( 0 ) ;
			x1 = x2 = 0 ;
			wpopup = NULL ;
			boolpopup = FALSE ;
			base_display = 0 ;
			Nb_display = 0;
			 }


# pragma	warn	-inl
		~TEnhancedListBox  ( )
			{
			for  ( ; indexmax  >=  0; indexmax -- )
				free ( memo [ indexmax ] ) ;
			free ( memo ) ;
			 }
# pragma	warn	.inl

		LRESULT DefWindowProc ( uint msg, WPARAM wparam, LPARAM lparam ) ;
		virtual void DrawItem     ( DRAWITEMSTRUCT far & drawInfo ) ;

		void	MovePopup	   ( HWND hwpopup, int large ) ;

                void	Showdropdown	   ( void ) ;
		LRESULT EvAddString  	   ( WPARAM  key, LPARAM  param ) ;
		LRESULT EvInsertString 	   ( WPARAM  key, LPARAM  param ) ;
		LRESULT EvDeleteString 	   ( WPARAM  key, LPARAM  param ) ;
		LRESULT EvResetContent 	   ( WPARAM  key, LPARAM  param ) ;
		LRESULT EvGetCurSel    	   ( WPARAM  key, LPARAM  param ) ;

		DECLARE_RESPONSE_TABLE     ( TEnhancedListBox ) ;
    } ;






# endif		/*  __GOMBO_H__  */