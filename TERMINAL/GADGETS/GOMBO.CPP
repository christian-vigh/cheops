/**************************************************************************/
/*                                                                        */
/*  Gombo.cpp  -  Christian Vigh, 95/10.                                  */
/*                                                                        */
/*  Implémentation de la classe  TEnhancedComboBox.                       */
/*                                                                        */
/**************************************************************************/
/**************************************************************************

  MODIFICATIONS :

  10/12/97 (CV)
	. Modification de TEnhancedComboBox :: SetupWindow afin de dimensionner
	  correctement le controle EDIT de la combo lorsque cette dernière 
	  possède le style CBS_NOBUTTON.
	. Modification de TEnhancedListBox :: MovePopup afin de positionner
	  correctement la fenetre de liste si cette dernière dépasse le bord
	  inférieur de l'écran.

 **************************************************************************/

# pragma	option -k- -N- -r- -v- -vi- -y-
# include	<owl/owlall.h>
# pragma	hdrstop
# pragma	option -k. -N. -r. -v. -vi. -y.


# include	"../ExStyles.h"
# include	"ChopJet.h"


/**************************************************************************/
/*                                                                        */
/*  Table de réponse.                                                     */
/*                                                                        */
/**************************************************************************/


DEFINE_RESPONSE_TABLE1 ( TEnhancedComboBox, TComboBox )
	EV_MESSAGE ( CB_ADDSTRING, EvAddString ),
	EV_MESSAGE ( CB_INSERTSTRING, EvInsertString ),
	EV_MESSAGE ( CB_DELETESTRING, EvDeleteString ),
	EV_MESSAGE ( CB_RESETCONTENT, EvResetContent ),
	EV_MESSAGE ( CB_GETCURSEL, EvGetCurSel ),
	EV_MESSAGE ( CB_SHOWDROPDOWN, EvShowdropdown ),
	EV_WM_MEASUREITEM,
END_RESPONSE_TABLE ;

DEFINE_RESPONSE_TABLE1 ( TEnhancedListBox, TListBox )
	EV_MESSAGE ( LB_ADDSTRING, EvAddString ),
	EV_MESSAGE ( LB_INSERTSTRING, EvInsertString ),
	EV_MESSAGE ( LB_DELETESTRING, EvDeleteString ),
	EV_MESSAGE ( LB_RESETCONTENT, EvResetContent ),
	EV_MESSAGE ( LB_GETCURSEL, EvGetCurSel ),
	EV_WM_MEASUREITEM,
END_RESPONSE_TABLE ;



class	TEnhancedBox	:  	public  TWindow
	{
	public :
		TWindow *	realparent ;

		TEnhancedBox  (  HWND source, TWindow * parent ) : TWindow  ( source )
			{
				realparent = parent ;
				:: SetWindowLong ( source, GWL_STYLE,
					:: GetWindowLong ( source, GWL_STYLE ) |
					ES_WANTRETURN ) ;
			 }

# pragma	warn	-inl
		~TEnhancedBox  ( )
			{
			 }
# pragma	warn	.inl
			LRESULT DefWindowProc  ( uint msg, WPARAM wparam, LPARAM lparam ) ;
	 } ;

LRESULT TEnhancedBox :: DefWindowProc  ( uint msg, WPARAM wparam, LPARAM lparam )
   {
	LRESULT 	Status ;


	switch  ( msg )
	   {
		case  WM_KEYDOWN :
			switch  ( wparam )
			  {
				case  VK_UP :
				case  VK_DOWN :
				case  VK_PRIOR :
				case  VK_NEXT :
					realparent -> SendMessage ( msg, wparam, lparam ) ;
					break ;
			      }
			break ;


		case	WM_LBUTTONUP :
			Status = TWindow :: DefWindowProc ( msg, wparam, lparam ) ;

			if  ( realparent -> SendMessage ( CB_GETDROPPEDSTATE, 0, 0L ) )
			   {
				TWindow :: DefWindowProc ( CB_SHOWDROPDOWN, FALSE, 0L ) ;
				realparent -> SendMessage ( WM_KEYDOWN, VK_F8, 0L ) ;
			    }
			return  ( Status ) ;



		case	WM_LBUTTONDOWN :
			Status = TWindow :: DefWindowProc ( msg, wparam, lparam ) ;
			realparent -> SendMessage ( WM_KEYDOWN, VK_RETURN, 0L ) ;
			return ( Status ) ;

	     }
	return ( TWindow :: DefWindowProc ( msg, wparam, lparam ) ) ;
      }




/**************************************************************************/
/*                                                                        */
/*  TEnhancedComboBox :: SetupWindow -                                    */
/*      Redimensionne le contrôle EDIT de la combo box pour cacher le     */
/*      bouton si le bit de style CBS_NOBUTTON est mis.                   */
/*                                                                        */
/**************************************************************************/

void	TEnhancedComboBox :: SetupWindow  ( void )
   {
	TComboBox :: SetupWindow  ( ) ;


	if  ( GetWindowLong ( GWL_STYLE ) & CBS_NOBUTTON )
	   {
		TRect		Rect = GetWindowRect ( ) ;
		HWND		hwnd = GetDlgItem ( 1001 ) ;	// contrôle EDIT
		TRect		EditRect ;
		TPoint		Left ;


		:: GetWindowRect ( hwnd, & EditRect ) ;
		Left. x = EditRect. left ;
		Left. y = EditRect. top ;
		ScreenToClient ( Left ) ;

		if  ( hwnd )
		   {
			:: SetWindowPos ( hwnd, HWND_TOPMOST, Left. x, Left. y,
				Rect. Width ( ) - ( 2 * Left. x ), 
				EditRect. Height ( ),
					SWP_NOZORDER ) ;
		     }
	     }
      }


void	TEnhancedComboBox :: Paint  ( TDC &  dc, BOOL  erase, TRect &  rect )
   {
	:: LockWindowUpdate ( HWindow ) ;
	TComboBox :: Paint ( dc, erase, rect ) ;
	:: LockWindowUpdate ( NULL ) ;
    }


/**************************************************************************/
/*                                                                        */
/*  TEnhancedComboBox :: PreProcessMsg -                                  */
/*      Envoie le message VK_F4 à la boite de dialogue (l'appui sur la    */
/*      touche F4 permet normalement d'ouvrir une combo box.              */
/*                                                                        */
/**************************************************************************/

BOOL	TEnhancedComboBox :: PreProcessMsg ( MSG &  msg )
   {
	return ( TComboBox :: PreProcessMsg ( msg ) ) ;
     }


/**************************************************************************/
/*                                                                        */
/*   TEnhancedComboBox :: DefWindowProc :                                 */
/*	Filtre sur le message de pression sur le bouton pour devalider    */
/*	l'aspet et éviter que le système ne récupère la géstion de la	  */
/*	popup window.
/*                                                                        */
/**************************************************************************/


LRESULT TEnhancedComboBox :: DefWindowProc  ( uint msg, WPARAM wparam, LPARAM lparam )
	{
	int		i ;
	char *	pchar,	Tampon [ 100 ] ;

	switch  ( msg )
		{
			case	WM_LBUTTONDBLCLK :
				return ( 0L ) ;

			case  WM_LBUTTONDOWN :
				boolpopup = FALSE ;	/* la position n'est plus valide  */

				if  ( TComboBox :: SendMessage ( CB_GETDROPPEDSTATE, 0, 0L ) )
				   {
					msg    = WM_KEYDOWN ;
					wparam = VK_ESCAPE ;
					lparam = 0L ;
				     }
				else
				   {
					SetFocus ( ) ;
					msg  	=  WM_KEYDOWN ;
					wparam	=  VK_RETURN ;
					lparam  =  0xFFFFFFFFL ;
				    }
				goto  Enter ;



			case  WM_KEYDOWN :
				switch  ( wparam )
					{
					case  VK_UP :
						if  ( TComboBox :: DefWindowProc ( CB_GETCURSEL,wparam, lparam ) )
							return  ( NULL ) ;
						else  if  ( base_display )
							{
							base_display -- ;
							TComboBox :: DefWindowProc ( CB_DELETESTRING, Nb_display_max -1, 0 ) ;
							strncpy ( Tampon, memo [ base_display ], sizeof( Tampon ) ) ;
							pchar = Tampon ;
							while ( * pchar  &&  *pchar  !=  ' '  &&  * pchar  !=  '\t' )
								pchar ++ ;
							* pchar = 0 ;
							TComboBox :: DefWindowProc ( CB_INSERTSTRING, 0, ( LPARAM ) Tampon ) ;
							TComboBox :: DefWindowProc ( CB_SETCURSEL,0L,0L);
							return  ( NULL ) ;
							}
						break ;
					case  VK_DOWN :
						if  ( TComboBox :: DefWindowProc ( CB_GETCURSEL,wparam, lparam ) +1  <  GetCount ( ) )
							return  ( NULL ) ;
						else  if  ( base_display + Nb_display <= indexmax )
							{
							base_display ++ ;
							TComboBox :: DefWindowProc ( CB_DELETESTRING, 0, 0 ) ;
							strncpy ( Tampon, memo [ base_display + Nb_display -1 ], sizeof( Tampon ) ) ;
							pchar = Tampon ;
							while ( * pchar  &&  *pchar  !=  ' '  &&  * pchar  !=  '\t' )
								pchar ++ ;
							* pchar = 0 ;
							TComboBox :: DefWindowProc ( CB_INSERTSTRING, -1, ( LPARAM )Tampon ) ;
							TComboBox :: DefWindowProc ( CB_SETCURSEL,Nb_display-1,0L);
							return  ( NULL ) ;
							 }
						break ;
					case  VK_PRIOR :
						if  ( TComboBox :: DefWindowProc ( CB_GETCURSEL,wparam, lparam ) )
							return  ( NULL ) ;
						else  if  ( base_display > Nb_display )
							base_display -= Nb_display ;
						else  if  ( base_display )
							base_display = 0 ;
						else  break ;

						TComboBox :: DefWindowProc ( CB_RESETCONTENT, 0L, 0L ) ;
						for  ( i = 0; i < Nb_display; i ++ )
							{
							strncpy ( Tampon, memo [ base_display + i ], sizeof( Tampon ) ) ;
							pchar = Tampon ;
							while ( * pchar  &&  *pchar  !=  ' '  &&  * pchar  !=  '\t' )
								pchar ++ ;
							* pchar = 0 ;
							TComboBox :: DefWindowProc ( CB_INSERTSTRING, -1, ( LPARAM )Tampon ) ;
							 }
						TComboBox :: DefWindowProc ( CB_SETCURSEL,0L,0L);
						return ( NULL ) ;
					case  VK_NEXT :
						if  ( TComboBox :: DefWindowProc ( CB_GETCURSEL,wparam, lparam ) +1  < Nb_display )
							return  ( NULL ) ;
						else  if  ( base_display + 2*Nb_display < indexmax )
							base_display += Nb_display ;
						else  if  ( base_display < indexmax - Nb_display +1 )
							base_display = indexmax - Nb_display +1 ;
						else  break ;

						TComboBox :: DefWindowProc ( CB_RESETCONTENT, 0L, 0L ) ;
						for  ( i = 0; i < Nb_display; i ++ )
							{
							strncpy ( Tampon, memo [ base_display + i ], sizeof( Tampon ) ) ;
							pchar = Tampon ;
							while ( * pchar  &&  *pchar  !=  ' '  &&  * pchar  !=  '\t' )
								pchar ++ ;
							* pchar = 0 ;
							TComboBox :: DefWindowProc ( CB_INSERTSTRING, -1, ( LPARAM )Tampon ) ;
							 }
						TComboBox :: DefWindowProc ( CB_SETCURSEL,Nb_display-1,0L);
						return ( NULL ) ;
					case  VK_ESCAPE :
					case  VK_RETURN :
						break ;

				// F4 provoque l'ouverture de la combo ->
				// relayer le message à la boiboite
					case	VK_F4 :
						break ; 

					default :
						return ( NULL ) ;
					 }
Enter :
				Parent -> SendMessage ( msg, wparam, lparam ) ;
				return  ( NULL ) ;
		 }
	return ( TComboBox :: DefWindowProc ( msg, wparam, lparam ) ) ;
	 }

/**************************************************************************/
/*                                                                        */
/*   TEnhancedComboBox :: EvMeasureItem :                                 */
/*	Corrige le message d'information des dimensions de l'objet	  */
/*	d'origine qui ne retourné pas les bonnes tailles.		  */
/*                                                                        */
/**************************************************************************/
void TEnhancedComboBox :: EvMeasureItem ( uint ctrlid, MEASUREITEMSTRUCT far & meas )
{
	meas. itemHeight = 14 ;		/* hauteur officielle à restituer */
	TComboBox :: DefWindowProc ( (uint)WM_MEASUREITEM, (WPARAM)ctrlid, (LPARAM)&meas ) ;
}


/**************************************************************************/
/*                                                                        */
/*   TEnhancedComboBox :: EvAddString :                                   */
/*	Ajoute une nouvelle ligne à la table. On décortique le message    */
/*	pour ne passer que les informations utiles à l'objet d'origine,	  */
/*	puis on stock les informations de présentation.			  */
/*                                                                        */
/**************************************************************************/
LRESULT TEnhancedComboBox :: EvAddString  ( WPARAM  key, LPARAM  param )
	{
	TClientDC	tmpDC ( HWindow ) ;	/* information d'affichage*/
	char *	pchar = ( char * ) param ;	/* pointeur de parcourt   */
	TSize	testx1,				/* structures de mesure   */
		testx2 ;

	indexmax ++ ;				/* ajout à la liste       */
	memo = ( char ** ) realloc ( memo, sizeof ( char * ) * ( indexmax + 1 ) ) ;
	memo [ indexmax ] = ( char * ) malloc ( strlen ( pchar ) +1 ) ;

	strncpy ( memo[ indexmax ], pchar, strlen ( pchar ) +1 ) ;

	while  ( * pchar  &&  * pchar  !=  ' '  &&  * pchar  !=  '\t' )
		pchar ++ ;			/* fin de la clée        */
	if  ( * pchar )				/* isolation du mot	  */
		{
		* pchar = 0 ;
		pchar ++ ;
		 }
	if  ( Nb_display < Nb_display_max )
		{
		TComboBox :: DefWindowProc ( CB_ADDSTRING,key, param ) ; /* transfert à Combo  */
		Nb_display ++ ;
		 }

	while  ( * pchar  ==  ' '  ||  * pchar  ==  '\t' )
		pchar ++ ;			/* début de chaine        */

						/* prise des mesures      */
	testx1 = tmpDC. GetTextExtent ( ( char * ) param, strlen ( ( char * ) param ) ) + 8 ;
	testx2 = tmpDC. GetTextExtent ( pchar, strlen ( pchar ) ) ;
	if  ( x1 < testx1. cx ) x1 = testx1.cx ;
	if  ( x2 < testx2. cx ) x2 = testx2.cx ;

	return  ( 0 ) ;
	 }

/**************************************************************************/
/*                                                                        */
/*   TEnhancedComboBox :: EvInsertString :                                   */
/*	Ajoute une nouvelle ligne à la table. On décortique le message    */
/*	pour ne passer que les informations utiles à l'objet d'origine,	  */
/*	puis on stock les informations de présentation.			  */
/*                                                                        */
/**************************************************************************/
LRESULT TEnhancedComboBox :: EvInsertString  ( WPARAM  key, LPARAM  param )
	{
	TClientDC	tmpDC ( HWindow ) ;	/* information d'affichage*/
	char *	pchar = ( char * ) param ;	/* pointeur de parcourt   */
	TSize	testx1,				/* structures de mesure   */
		testx2 ;
	char **	memo2 ;			/* variable de "swap" pour la table de chaine */

	indexmax ++ ;				/* ajout à la liste       */
	memo2 = ( char ** ) malloc (  sizeof ( char * ) * ( indexmax + 1 ) ) ;
	memcpy ( memo2 +1, memo, sizeof ( char * ) * indexmax ) ;
	free ( memo ) ;
	memo = memo2 ;
	memo [ 0 ] = ( char * ) malloc ( strlen ( pchar ) +1 ) ;

	strncpy ( memo[ 0 ], pchar, strlen ( pchar ) +1 ) ;

	while  ( * pchar  &&  * pchar  !=  ' '  &&  * pchar  !=  '\t' )
		pchar ++ ;			/* début de chaine        */
	if  ( * pchar )				/* isolation du mot	  */
		{
		* pchar = 0 ;
		pchar ++ ;
		 }
	if  ( Nb_display < Nb_display_max )
		{
		TComboBox :: DefWindowProc ( CB_INSERTSTRING,key, param ) ; /* transfert à Combo  */
		Nb_display ++ ;
		if  ( wpopup )
		   {
			TComboBox :: DefWindowProc ( CB_SHOWDROPDOWN, FALSE, 0L ) ;
			TComboBox :: DefWindowProc ( CB_SHOWDROPDOWN, TRUE, 0L ) ;
		    }
		 }
	else	if  ( key  <=  base_display  &&  key  != ( WPARAM )  -1 )
		base_display ++ ;

	while  ( * pchar  ==  ' '  ||  * pchar  ==  '\t' )
		pchar ++ ;			/* début de chaine        */

						/* prise des mesures      */
	testx1 = tmpDC. GetTextExtent ( ( char * ) param, strlen ( ( char * ) param ) ) + 8 ;
	testx2 = tmpDC. GetTextExtent ( pchar, strlen ( pchar ) ) ;
	if  ( x1 < testx1. cx ) x1 = testx1.cx ;
	if  ( x2 < testx2. cx ) x2 = testx2.cx ;

	return  ( 0 ) ;
	 }


/**************************************************************************/
/*                                                                        */
/*   TEnhancedComboBox :: EvDeleteString :                                   */
/*	Supprime une ligne à la table. On décortique le message pour connaitre */
/*		l'indice de l'element à eliminer.											  */
/*                                                                        */
/**************************************************************************/
LRESULT TEnhancedComboBox :: EvDeleteString  ( WPARAM  key, LPARAM  /*param*/ )
	{
	free ( memo [ key ] ) ;		/* est il utile de redimensionner la table ??*/
	if  ( key < base_display )
		base_display -- ;

	for  ( ; key < indexmax; key ++ )
		memo [ key ] = memo [key +1 ] ;
	indexmax -- ;

	return  ( 0 ) ;
	 }

/**************************************************************************/
/*                                                                        */
/*   TEnhancedComboBox :: EvResetContent :                                */
/*	Gère le message de libération du contenu de la table. On passe la */
/*	demande à l'objet d'origine puis on initialise nos propes mémoires*/
/*                                                                        */
/**************************************************************************/
LRESULT TEnhancedComboBox :: EvResetContent  ( WPARAM  key, LPARAM  param )
	{
						/* transfert à la ComboBox*/
	LRESULT retour = TComboBox :: DefWindowProc ( CB_RESETCONTENT,key, param ) ;
	for  ( ; indexmax  >=  0; indexmax -- ) /* libérations personnels */
		free ( memo [ indexmax ] ) ;
	memo = ( char ** ) realloc ( memo, 0 ) ;
	base_display = Nb_display = 0 ;

	if  ( wpopup )				/* redimensions de la popup*/
		{
		boolpopup = FALSE ;
		MovePopup ( wpopup, x2 = 0 ) ;
		 }

	return  ( retour ) ;
	 }



/**************************************************************************/
/*                                                                        */
/*   TEnhancedComboBox :: EvGetCurSel :                                */
/*		Demande l'index de la selection.	Il faut la corriger en fonction    */
/*			de notre table personnelle.												*/
/*                                                                        */
/**************************************************************************/
LRESULT TEnhancedComboBox :: EvGetCurSel  ( WPARAM  key, LPARAM  param )
	{
	LRESULT retour = TComboBox :: DefWindowProc ( CB_GETCURSEL,key, param ) ;
	return  ( retour + base_display ) ;
	 }


/**************************************************************************/
/*                                                                        */
/*   TEnhancedComboBox :: Evshowdropdown :                                */
/*		Demande l'index de la selection.	Il faut la corriger en fonction    */
/*			de notre table personnelle.												*/
/*                                                                        */
/**************************************************************************/
LRESULT TEnhancedComboBox :: EvShowdropdown  ( WPARAM  key, LPARAM  param )
	{
	LRESULT 	retour ;
	int		i ;
	char *	pchar,	Tampon [ 100 ] ;

	if  ( key )
	   {
		retour = TComboBox :: DefWindowProc ( CB_GETCURSEL,key, param ) ;
		TComboBox :: DefWindowProc ( CB_RESETCONTENT, 0L, 0L ) ;
		if  ( base_display + Nb_display > indexmax )
			if  ( Nb_display < indexmax )
				base_display = indexmax - Nb_display ;
			else	base_display = 0 ;
		for  ( i = 0; i < Nb_display; i ++ )
		   {
			strncpy ( Tampon, memo [ base_display + i ], sizeof( Tampon ) ) ;
			pchar = Tampon ;
			while ( * pchar  &&  *pchar  !=  ' '  &&  * pchar  !=  '\t' )
				pchar ++ ;
			* pchar = 0 ;
			TComboBox :: DefWindowProc ( CB_INSERTSTRING, -1, ( LPARAM )Tampon ) ;
		 }
		TComboBox :: DefWindowProc ( CB_SETCURSEL, ( WPARAM ) retour,0L);
	    }
	retour = TComboBox :: DefWindowProc ( CB_SHOWDROPDOWN, ( WPARAM ) key, param ) ;
	return  ( retour ) ;
	 }


/**************************************************************************/
/*                                                                        */
/*   TEnhancedComboBox :: MovePopup :                                     */
/*      Gère la taille et la position de la popup window en fonction de   */
/*	son contenu et de son environnement.				  */
/*                                                                        */
/**************************************************************************/
void TEnhancedComboBox :: MovePopup  ( HWND hwpopup, int large )
	{
	TRect	mainrect, popuprect ;		/* mesures de base        */
	int	maxX ;				/* repères sur le SCREEN  */
	TWindow winpopup ( ( wpopup = hwpopup ) ) ; /* struct de manip	  */

	GetWindowRect ( mainrect ) ;		/* prises mesures de base */
	winpopup. GetWindowRect ( popuprect ) ;
	maxX = GetSystemMetrics ( SM_CXSCREEN ) ;

	popuprect. left = mainrect. left ;	/* alignements		  */
	popuprect. right = max ( mainrect.right, popuprect.left + large ) ;

	if  ( popuprect. right > maxX )		/* control bord droit     */
		{
		maxX = popuprect. right - maxX ;
		popuprect. left -= maxX ;
		popuprect. right -= maxX ;
		 }
	else  if  ( popuprect. left < 0 )	/* control bord gauche    */
		{
		popuprect. right -= popuprect. left ;
		popuprect. left = 0 ;
		 }

	winpopup. MoveWindow ( popuprect, TRUE ) ; /* demande mise a jour */
	 }
/**************************************************************************/
/*                                                                        */
/*   TEnhancedComboBox :: DrawItem :                                      */
/*      Gère l'affichage d'une combobox à la mode unix ( controle de      */
/*		largeur et de positionnement.				  */
/*                                                                        */
/**************************************************************************/
void TEnhancedComboBox :: DrawItem  ( DRAWITEMSTRUCT far & drawInfo )
	{
	char 	table [100] ;			/* buffer de travail      */
	TDC	drawDC(drawInfo.hDC);		/* zones de dessin        */
	TRect	itemRect(drawInfo.rcItem.left,drawInfo.rcItem.top,drawInfo.rcItem.right,drawInfo.rcItem.bottom);
	TRect   BaseRect = itemRect ;
	char *	pchar ;
	bool	useinvert = ( drawInfo. itemState & ODS_SELECTED ) ;

	if  ( drawInfo. itemID  >=  Nb_display )
		return ;
	if  ( ! wpopup )
		new  TEnhancedBox ( drawInfo. hwndItem, this ) ;

	if  ( ! boolpopup )			/* redimensionnement ?    */
		{
		boolpopup = TRUE ;
		MovePopup ( drawInfo. hwndItem, x1+x2 ) ;
		 }
						/* récupération des infos */
	TComboBox::GetString ( table, drawInfo. itemID  ) ;

	drawDC. FillRect ( itemRect, TBrush ( TColor ( ::GetSysColor ( useinvert ? COLOR_HIGHLIGHT : COLOR_WINDOW ) ) ) ) ;

	if  ( useinvert )
		{
		SetTextColor ( drawDC, GetSysColor ( COLOR_HIGHLIGHTTEXT ) ) ;
		SetBkColor   ( drawDC, GetSysColor ( COLOR_HIGHLIGHT ) ) ;
		 }
	else
		{
		SetTextColor ( drawDC, GetSysColor ( COLOR_WINDOWTEXT ) ) ;
		SetBkColor   ( drawDC, GetSysColor ( COLOR_WINDOW ) ) ;
		 }

	itemRect. left += 3 ;		       	/* écriture formatée      */
	drawDC. DrawText ( table , -1, itemRect, DT_VCENTER ) ;
	itemRect. left += x1 ;
	pchar = memo [ base_display + drawInfo. itemID ] ;
	while  ( * pchar  &&  * pchar  !=  ' '  &&  * pchar  !=  '\t' )
		pchar ++ ;
	while  ( * pchar  ==  ' '  ||  * pchar  ==  '\t' )
		pchar ++ ;
	if  ( * pchar )
		drawDC. DrawText( pchar , -1, itemRect, DT_VCENTER ) ;
 }










/**************************************************************************/
/*                                                                        */
/*   TEnhancedListBox :: DefWindowProc :                                 */
/*	Filtre sur le message de pression sur le bouton pour devalider    */
/*	l'aspet et éviter que le système ne récupère la géstion de la	  */
/*	popup window.
/*                                                                        */
/**************************************************************************/
LRESULT TEnhancedListBox :: DefWindowProc  ( uint msg, WPARAM wparam, LPARAM lparam )
	{
	int		i ;
	char *	pchar,	Tampon [ 100 ] ;

	switch  ( msg )
		{
			case  WM_KEYDOWN :
				switch  ( wparam )
					{
					case  VK_UP :
						if  ( TListBox :: DefWindowProc ( LB_GETCURSEL,wparam, lparam ) )
							return ( TListBox :: DefWindowProc ( msg, wparam, lparam ) ) ;
						else  if  ( base_display )
							{
							base_display -- ;
							TListBox :: DefWindowProc ( LB_DELETESTRING, Nb_display_max -1, 0 ) ;
							strncpy ( Tampon, memo [ base_display ], sizeof( Tampon ) ) ;
							pchar = Tampon ;
							while ( * pchar  &&  *pchar  !=  ' '  &&  * pchar  !=  '\t' )
								pchar ++ ;
							* pchar = 0 ;
							TListBox :: DefWindowProc ( LB_INSERTSTRING, 0, ( LPARAM ) Tampon ) ;
							TListBox :: DefWindowProc ( LB_SETCURSEL,0L,0L);
							return  ( NULL ) ;
							}
						break ;
					case  VK_DOWN :
						if  ( TListBox :: DefWindowProc ( LB_GETCURSEL,wparam, lparam ) +1  <  GetCount ( ) )
							return ( TListBox :: DefWindowProc ( msg, wparam, lparam ) ) ;
						else  if  ( base_display + Nb_display <= indexmax )
							{
							base_display ++ ;
							TListBox :: DefWindowProc ( LB_DELETESTRING, 0, 0 ) ;
							strncpy ( Tampon, memo [ base_display + Nb_display -1 ], sizeof( Tampon ) ) ;
							pchar = Tampon ;
							while ( * pchar  &&  *pchar  !=  ' '  &&  * pchar  !=  '\t' )
								pchar ++ ;
							* pchar = 0 ;
							TListBox :: DefWindowProc ( LB_INSERTSTRING, -1, ( LPARAM )Tampon ) ;
							TListBox :: DefWindowProc ( LB_SETCURSEL,Nb_display-1,0L);
							return  ( NULL ) ;
							 }
						break ;
					case  VK_PRIOR :
						if  ( TListBox :: DefWindowProc ( LB_GETCURSEL,wparam, lparam ) )
							return ( TListBox :: DefWindowProc ( msg, wparam, lparam ) ) ;
						else  if  ( base_display > Nb_display )
							base_display -= Nb_display ;
						else  if  ( base_display )
							base_display = 0 ;
						else  break ;

						TListBox :: DefWindowProc ( LB_RESETCONTENT, 0L, 0L ) ;
						for  ( i = 0; i < Nb_display; i ++ )
							{
							strncpy ( Tampon, memo [ base_display + i ], sizeof( Tampon ) ) ;
							pchar = Tampon ;
							while ( * pchar  &&  *pchar  !=  ' '  &&  * pchar  !=  '\t' )
								pchar ++ ;
							* pchar = 0 ;
							TListBox :: DefWindowProc ( LB_INSERTSTRING, -1, ( LPARAM )Tampon ) ;
							 }
						TListBox :: DefWindowProc ( LB_SETCURSEL,0L,0L);
						return ( NULL ) ;
					case  VK_NEXT :
						if  ( TListBox :: DefWindowProc ( LB_GETCURSEL,wparam, lparam ) +1  < Nb_display )
							return ( TListBox :: DefWindowProc ( msg, wparam, lparam ) ) ;
						else  if  ( base_display + 2*Nb_display < indexmax )
							base_display += Nb_display ;
						else  if  ( base_display < indexmax - Nb_display +1 )
							base_display = indexmax - Nb_display +1 ;
						else  break ;

						TListBox :: DefWindowProc ( LB_RESETCONTENT, 0L, 0L ) ;
						for  ( i = 0; i < Nb_display; i ++ )
							{
							strncpy ( Tampon, memo [ base_display + i ], sizeof( Tampon ) ) ;
							pchar = Tampon ;
							while ( * pchar  &&  *pchar  !=  ' '  &&  * pchar  !=  '\t' )
								pchar ++ ;
							* pchar = 0 ;
							TListBox :: DefWindowProc ( LB_INSERTSTRING, -1, ( LPARAM )Tampon ) ;
							 }
							TListBox :: DefWindowProc ( LB_SETCURSEL,Nb_display-1,0L);
						return ( NULL ) ;
					case  VK_ESCAPE :
					case  VK_RETURN :
						break ;
					default :
						return ( NULL ) ;
					 }
//aze				Parent -> SendMessage ( msg, wparam, lparam ) ;
				return ( TListBox :: DefWindowProc ( msg, wparam, lparam ) ) ;
		 }

	return ( TListBox :: DefWindowProc ( msg, wparam, lparam ) ) ;
	 }

/**************************************************************************/
/*                                                                        */
/*   TEnhancedListBox :: Showdropdown :                                */
/*                                                                        */
/**************************************************************************/
void TEnhancedListBox :: Showdropdown  ( )
	{
	LRESULT 	retour ;
	int		i ;
	char *	pchar,	Tampon [ 100 ] ;

	retour = TListBox :: DefWindowProc ( LB_GETCURSEL, 0L, 0L ) ;
	TListBox :: DefWindowProc ( LB_RESETCONTENT, 0L, 0L ) ;
	if  ( base_display > indexmax )
		if  ( Nb_display < indexmax )
			base_display = indexmax - Nb_display ;
		else	base_display = 0 ;
	for  ( i = 0; i < Nb_display; i ++ )
	   {
		strncpy ( Tampon, memo [ base_display + i ], sizeof( Tampon ) ) ;
		pchar = Tampon ;
		while ( * pchar  &&  *pchar  !=  ' '  &&  * pchar  !=  '\t' )
			pchar ++ ;
		* pchar = 0 ;
		TListBox :: DefWindowProc ( LB_INSERTSTRING, -1, ( LPARAM )Tampon ) ;
	 }
	TListBox :: DefWindowProc ( LB_SETCURSEL,(WPARAM)retour,0L);
 }




/**************************************************************************/
/*                                                                        */
/*   TEnhancedListBox :: EvAddString :                                   */
/*	Ajoute une nouvelle ligne à la table. On décortique le message    */
/*	pour ne passer que les informations utiles à l'objet d'origine,	  */
/*	puis on stock les informations de présentation.			  */
/*                                                                        */
/**************************************************************************/
LRESULT TEnhancedListBox :: EvAddString  ( WPARAM  key, LPARAM  param )
	{
	TClientDC	tmpDC ( HWindow ) ;	/* information d'affichage*/
	char *	pchar = ( char * ) param ;	/* pointeur de parcourt   */
	TSize	testx1,				/* structures de mesure   */
		testx2 ;

	indexmax ++ ;				/* ajout à la liste       */
	memo = ( char ** ) realloc ( memo, sizeof ( char * ) * ( indexmax + 1 ) ) ;
	memo [ indexmax ] = ( char * ) malloc ( strlen ( pchar ) +1 ) ;

	strncpy ( memo[ indexmax ], pchar, strlen ( pchar ) +1 ) ;

	while  ( * pchar  &&  * pchar  !=  ' '  &&  * pchar  !=  '\t' )
		pchar ++ ;			/* fin de la clée        */
	if  ( * pchar )				/* isolation du mot	  */
		{
		* pchar = 0 ;
		pchar ++ ;
		 }
	if  ( Nb_display < Nb_display_max )
		{
		TListBox :: DefWindowProc ( LB_ADDSTRING,key, param ) ; /* transfert à Combo  */
		Nb_display ++ ;
		 }

	while  ( * pchar  ==  ' '  ||  * pchar  ==  '\t' )
		pchar ++ ;			/* début de chaine        */

						/* prise des mesures      */
	testx1 = tmpDC. GetTextExtent ( ( char * ) param, strlen ( ( char * ) param ) ) + 8 ;
	testx2 = tmpDC. GetTextExtent ( pchar, strlen ( pchar ) ) ;
	if  ( x1 < testx1. cx ) x1 = testx1.cx ;
	if  ( x2 < testx2. cx ) x2 = testx2.cx ;

	return  ( 0 ) ;
	 }

/**************************************************************************/
/*                                                                        */
/*   TEnhancedListBox :: EvInsertString :                                   */
/*	Ajoute une nouvelle ligne à la table. On décortique le message    */
/*	pour ne passer que les informations utiles à l'objet d'origine,	  */
/*	puis on stock les informations de présentation.			  */
/*                                                                        */
/**************************************************************************/
LRESULT TEnhancedListBox :: EvInsertString  ( WPARAM  key, LPARAM  param )
	{
	TClientDC	tmpDC ( HWindow ) ;	/* information d'affichage*/
	char *	pchar = ( char * ) param ;	/* pointeur de parcourt   */
	TSize	testx1,				/* structures de mesure   */
		testx2 ;
	char **	memo2 ;			/* variable de "swap" pour la table de chaine */

	indexmax ++ ;				/* ajout à la liste       */
	memo2 = ( char ** ) malloc (  sizeof ( char * ) * ( indexmax + 1 ) ) ;
	memcpy ( memo2 +1, memo, sizeof ( char * ) * indexmax ) ;
	free ( memo ) ;
	memo = memo2 ;
	memo [ 0 ] = ( char * ) malloc ( strlen ( pchar ) +1 ) ;

	strncpy ( memo[ 0 ], pchar, strlen ( pchar ) +1 ) ;

	while  ( * pchar  &&  * pchar  !=  ' '  &&  * pchar  !=  '\t' )
		pchar ++ ;			/* début de chaine        */
	if  ( * pchar )				/* isolation du mot	  */
		{
		* pchar = 0 ;
		pchar ++ ;
		 }

	if  ( Nb_display < Nb_display_max )
		{
		TListBox :: DefWindowProc ( LB_INSERTSTRING,key, param ) ; /* transfert à Combo  */
		Nb_display ++ ;
		 }
	else  if  ( key  <=  base_display  &&  key  != ( WPARAM )  -1 )
		base_display ++ ;

	while  ( * pchar  ==  ' '  ||  * pchar  ==  '\t' )
		pchar ++ ;			/* début de chaine        */

						/* prise des mesures      */
	testx1 = tmpDC. GetTextExtent ( ( char * ) param, strlen ( ( char * ) param ) ) + 8 ;
	testx2 = tmpDC. GetTextExtent ( pchar, strlen ( pchar ) ) ;
	if  ( x1 < testx1. cx ) x1 = testx1.cx ;
	if  ( x2 < testx2. cx ) x2 = testx2.cx ;

	return  ( 0 ) ;
	 }

/**************************************************************************/
/*                                                                        */
/*   TEnhancedListBox :: EvDeleteString :                                   */
/*	Supprime une ligne à la table. On décortique le message pour connaitre */
/*		l'indice de l'element à eliminer.											  */
/*                                                                        */
/**************************************************************************/
LRESULT TEnhancedListBox :: EvDeleteString  ( WPARAM  key, LPARAM )
	{
	free ( memo [ key ] ) ;		/* est il utile de redimensionner la table ??*/
	if  ( key < base_display )
		base_display -- ;

	for  ( ; key < indexmax; key ++ )
		memo [ key ] = memo [key +1 ] ;
	indexmax -- ;
	return  ( 0 ) ;
	 }


/**************************************************************************/
/*                                                                        */
/*   TEnhancedListBox :: EvResetContent :                                */
/*	Gère le message de libération du contenu de la table. On passe la */
/*	demande à l'objet d'origine puis on initialise nos propes mémoires*/
/*                                                                        */
/**************************************************************************/
LRESULT TEnhancedListBox :: EvResetContent  ( WPARAM  key, LPARAM  param )
	{
						/* transfert à la ListBox*/
	LRESULT retour = TListBox :: DefWindowProc ( LB_RESETCONTENT,key, param ) ;
	for  ( ; indexmax  >=  0; indexmax -- ) /* libérations personnels */
		free ( memo [ indexmax ] ) ;
	memo = ( char ** ) realloc ( memo, 0 ) ;
	base_display = Nb_display = 0 ;

	if  ( wpopup )				/* redimensions de la popup*/
		{
		boolpopup = FALSE ;
		MovePopup ( wpopup, x2 = 0 ) ;
		 }
	return  ( retour ) ;
	 }

/**************************************************************************/
/*                                                                        */
/*   TEnhancedListBox :: EvGetCurSel :                                */
/*		Demande l'index de la selection.	Il faut la corriger en fonction    */
/*			de notre table personnelle.												*/
/*                                                                        */
/**************************************************************************/
LRESULT TEnhancedListBox :: EvGetCurSel  ( WPARAM  key, LPARAM  param )
	{
	LRESULT retour = TListBox :: DefWindowProc ( LB_GETCURSEL,key, param ) ;
	return  ( retour + base_display ) ;
	 }



/**************************************************************************/
/*                                                                        */
/*   TEnhancedListBox :: MovePopup :                                     */
/*      Gère la taille et la position de la popup window en fonction de   */
/*	son contenu et de son environnement.				  */
/*                                                                        */
/**************************************************************************/

void TEnhancedListBox :: MovePopup  ( HWND hwpopup, int large )
   {
	TRect		mainrect, 
			popuprect ;		/* Mesures de base        */
	int		maxX, maxY ;		/* Dimensions de l'écran  */
	TWindow 	winpopup ( ( wpopup = hwpopup ) ) ;
	int		delta ;


	GetWindowRect ( mainrect ) ;		/* prises mesures de base */
	winpopup. GetWindowRect ( popuprect ) ;
	maxX = GetSystemMetrics ( SM_CXSCREEN ) ;
	maxY = GetSystemMetrics ( SM_CYSCREEN ) ;

// Aligner la liste sur le controle EDIT 
	popuprect. left  = mainrect. left ;
	popuprect. right = popuprect. left + large ;

// Recadrage du bord droit
	if  ( popuprect. right > maxX )	
	   {
		delta 	          = popuprect. right - maxX ;
		popuprect. left  -= delta ;
		popuprect. right -= delta ;
	     }
// Recadrage du bord gauche
	else  if  ( popuprect. left < 0 )
	   {
		popuprect. right -= popuprect. left ;
		popuprect. left = 0 ;
	    }

// Recadrage du bord inférieur
	if  ( popuprect. bottom  >  maxY )
	   {
		delta 	           = popuprect. Height ( ) + EditRect. Height ( ) ;
		popuprect. bottom -= delta ;
		popuprect. top    -= delta ;
	     }
// Recadrage du bord supérieur
	else if  ( popuprect. top  <  0 )
	   {
		popuprect. bottom  -=  popuprect. top ;
		popuprect. top = 0 ;
	     }

// Déplacer la fenetre
	winpopup. MoveWindow ( popuprect, TRUE ) ;
   }


/**************************************************************************/
/*                                                                        */
/*   TEnhancedListBox :: DrawItem :                                      */
/*      Gère l'affichage d'une combobox à la mode unix ( controle de      */
/*		largeur et de positionnement.				  */
/*                                                                        */
/**************************************************************************/
void TEnhancedListBox :: DrawItem  ( DRAWITEMSTRUCT far & drawInfo )
	{
	char 	table [100] ;			/* buffer de travail      */
	TDC	drawDC(drawInfo.hDC);		/* zones de dessin        */
	TRect	itemRect(drawInfo.rcItem.left,drawInfo.rcItem.top,drawInfo.rcItem.right,drawInfo.rcItem.bottom);
	TRect   BaseRect = itemRect ;
	char *	pchar ;
	bool	useinvert = ( drawInfo. itemState & ODS_SELECTED ) ;

	if  ( drawInfo. itemID >= Nb_display )
		return ;

	if  ( ! boolpopup )			/* redimensionnement ?    */
		{
		TRect	deplace ;

		boolpopup = TRUE ;

		Parent -> GetWindowRect ( deplace ) ;	/* basee sur le parent */
		deplace. right -= deplace. left ;
		deplace. left = -1 ;
		deplace. bottom -= deplace. top ;
		deplace. top = -1 ;
		deplace. right += GetSystemMetrics ( SM_CXVSCROLL ) ; /* cache scroller */
		MoveWindow ( deplace, TRUE ) ;
		 }

	TListBox :: GetString ( table, drawInfo. itemID  ) ;

	drawDC. FillRect ( itemRect, TBrush ( TColor ( ::GetSysColor ( useinvert ? COLOR_HIGHLIGHT : COLOR_WINDOW ) ) ) ) ;

	if  ( useinvert )
		{
		SetTextColor ( drawDC, GetSysColor ( COLOR_HIGHLIGHTTEXT ) ) ;
		SetBkColor   ( drawDC, GetSysColor ( COLOR_HIGHLIGHT ) ) ;
		 }
	else
		{
		SetTextColor ( drawDC, GetSysColor ( COLOR_WINDOWTEXT ) ) ;
		SetBkColor   ( drawDC, GetSysColor ( COLOR_WINDOW ) ) ;
		 }

	itemRect. left += 3 ;		       	/* écriture formatée      */
	drawDC. DrawText ( table , -1, itemRect, DT_VCENTER ) ;
	itemRect. left += x1 ;
	pchar = memo [ base_display + drawInfo. itemID ] ;
	while  ( * pchar  &&  * pchar  !=  ' '  &&  * pchar  !=  '\t' )
		pchar ++ ;
	while  ( * pchar  ==  ' '  ||  * pchar  ==  '\t' )
		pchar ++ ;
	if  ( * pchar )
		drawDC. DrawText( pchar , -1, itemRect, DT_VCENTER ) ;
	 }
