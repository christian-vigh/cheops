/**************************************************************************/
/*                                                                        */
/*  MFText.cpp  -  Christian Vigh, 95/11.                                 */
/*                                                                        */
/*  Impl�mentation de la classe TMultiFontTextGadget.                     */
/*                                                                        */
/**************************************************************************/


# pragma	option -k- -N- -r- -v- -vi- -y-
# include	<owl/owlall.h>
# pragma	hdrstop
# pragma	option -k. -N. -r. -v. -vi. -y.


# include	"ChopJet.h"



/**************************************************************************/
/*                                                                        */
/*  Constructeur.                                                         */
/*                                                                        */
/**************************************************************************/

TMultiFontTextGadget :: TMultiFontTextGadget  ( int		id,
						TBorderStyle	style,
						uint		numchars,
						char *		text,
						int		margins,
						BOOL		clickable )
	: TAutoTextGadget ( id, style, AutoAdjust, numchars, 0, margins, clickable )
   {
	TextEntries = LastTextEntry = 0 ;
	Text = 0 ;

	if  ( text )
		SetText ( text ) ;
    }





/**************************************************************************/
/*                                                                        */
/*  Destructeur.                                                          */
/*                                                                        */
/**************************************************************************/

TMultiFontTextGadget :: ~TMultiFontTextGadget  ( )
   {
	DeleteEntries ( ) ;
     }





/**************************************************************************/
/*                                                                        */
/*  TMultiFontTextGadget :: DeleteEntries -                               */
/*      Efface les entr�es de texte, s'il y en a.                         */
/*                                                                        */
/**************************************************************************/

void	TMultiFontTextGadget :: DeleteEntries  ( void )
   {
	if  ( TextEntries )
	   {
		register TTextEntry *	te	=  TextEntries,
				    *   temp ;


		while  ( te )
		   {
			temp = te -> Next ;

			if  ( te -> Text )
				delete  []  te -> Text ;
			if  ( te -> Font )
				delete  te -> Font ;

			delete  te ;
			te = temp ;
		     }

		TextEntries = LastTextEntry = 0 ;
	     }

	if  ( Text )
	   {
		delete  Text ;
		Text = 0 ;
	    }
      }
			
		



/**************************************************************************/
/*                                                                        */
/*  TMultiFontTextGadget :: GetDesiredSize -                              */
/*      Calcule la taille compl�te du gadget.                             */
/*                                                                        */
/**************************************************************************/

void	TMultiFontTextGadget :: GetDesiredSize  ( TSize &  size )
   {
	int		width		=  0,
			maxheight       =  0 ;


	TAutoTextGadget :: GetDesiredSize ( size ) ;

// Parcourir la liste des entr�es
	for  ( TTextEntry *  te = TextEntries ; te ; te = te -> Next )
	   {
		TScreenDC	DC ;
		TSize		TextSize ;

		DC. SelectObject ( * te -> Font ) ;
		TextSize = DC. GetTextExtent ( te -> Text, te -> TextLength ) ;
		width += TextSize. cx ;

		if  ( TextSize. cy  >  maxheight )
			maxheight = TextSize. cy ;
	     }


// Ajouter tout �a � la taille d�j� trouv�e
	size. cx += width ;
	size. cy += maxheight ;
    }






/**************************************************************************/
/*                                                                        */
/*  TMultiFontTextGadget :: Paint -                                       */
/*      Dessine le texte.                                                 */
/*                                                                        */
/**************************************************************************/

void	TMultiFontTextGadget :: Paint  ( TDC &  dc )
   {
	TRect		InnerRect ;
	register int	left, x, y ;


	dc. SetMapperFlags ( 0 ) ;
	TAutoTextGadget :: Paint ( dc ) ;

// R�cup�rer les coordonn�es initiales
	GetInnerRect ( InnerRect ) ;
	left	=  InnerRect. left ;

	x 	=  left ;


// Parcourir la liste
	for  ( TTextEntry *  te = TextEntries ; te ; te = te -> Next ) 
	   {
		TSize		Extent ;
		COLORREF	OldText, OldBk, 
				NewText, NewBk ;
		int		OldBkMode ;


		dc. SelectObject ( * te -> Font ) ;
		Extent = dc. GetTextExtent ( te -> Text, te -> TextLength ) ;
		y = InnerRect. top + ( ( InnerRect. Height ( ) - Extent. cy ) / 2 ) ;

			  
		NewBk = te -> BackgroundColor ;

		if  ( NewBk  ==  0xFFFFFFFFL )
			NewBk = GetSysColor ( COLOR_BTNFACE ) ;

		NewText = te -> ForegroundColor ;

		if  ( NewText  ==  0xFFFFFFFFL )
			NewText = GetSysColor ( COLOR_BTNTEXT ) ;

		OldText   = dc. SetTextColor  ( NewText ) ;
		OldBk     = dc. SetBkColor    ( NewBk ) ;
		OldBkMode = dc. SetBkMode     ( OPAQUE ) ;

		dc. ExtTextOut ( x, y, ETO_OPAQUE, 
			& TRect ( x, InnerRect. top, x + Extent. cx, InnerRect. bottom ),
				te -> Text, te -> TextLength ) ;

		dc. SetBkMode    ( OldBkMode ) ;
		dc. SetBkColor   ( OldBk ) ;
		dc. SetTextColor ( OldText ) ;

		dc. RestoreFont ( ) ;

		x += Extent. cx ;
	     }
     }

		





/**************************************************************************/
/*                                                                        */
/*  TMultiFontTextGadget :: SetText -                                     */
/*      Change le texte du gadget.                                        */
/*                                                                        */
/**************************************************************************/

void	TMultiFontTextGadget :: SetText ( char *	text )
   {
	register char *		p 		=  text ;
	LOGFONT			CurrentFont,
				NewFont ;
	COLORREF		OldFore, OldBack,
				Fore, Back ;
	int			size,
				total_size 	=  0 ;


// Effacer les entr�es de texte pr�c�dentes
	DeleteEntries ( ) ;

	if  ( ! text )
		goto  Return ;


// R�cup�rer les param�tres de la police du gadget, ainsi que les couleurs du texte
	{
		GetFont ( ). GetObject ( CurrentFont ) ;
		NewFont	=  CurrentFont ;

		OldBack = 0xFFFFFFFFL ;
		OldFore = 0xFFFFFFFFL ;
		Fore    = OldFore ;
		Back    = OldBack ;
	  }
	


// Traiter tous les formats
	while  ( * p )
	   {
	// R�cup�rer le prochain �l�ment de texte
		p = GetTextElement ( p, size, CurrentFont, NewFont, OldFore,
			OldBack, Fore, Back ) ;

	// Allouer un nouvel �l�ment dans la liste cha�n�e
		TTextEntry *	te	=  new  TTextEntry ;


	// Initialiser la structure
		te -> Font 		= new  TFont ( & NewFont ) ;
		te -> ForegroundColor   = Fore ;
		te -> BackgroundColor   = Back ;
		te -> TextLength	= size ;
		te -> Text		= new  char [size+1] ;
		te -> Next		= 0 ;
		strncpy ( te -> Text, p, size ) ;
		te -> Text [size] = 0 ;
		p += size ;


	// L'ajouter dans la liste cha�n�e
		if  ( TextEntries ) 
		   {
			LastTextEntry -> Next = te ;
			LastTextEntry = te ;
		    }
		else
			TextEntries = LastTextEntry = te ;
	      }
		   

Return :
// Supprimer l'ancien texte
	if  ( Text )	
		delete  []  Text ;


// Et le reconstituer avec les nouveaux �l�ments
	if  ( total_size )
	   {
		Text = p = new  char [ total_size + 1 ] ;

		for  ( TTextEntry *  te = TextEntries ; te ; te = te -> Next )
		   {
			strcpy ( p, te -> Text ) ;
			p += te -> TextLength + 1 ;
		    }
	     }
	else
		Text = 0 ;

// Indiquer qu'il y a eu un changement de taille
	Window -> GadgetChangedSize ( * this ) ;
    }		







/**************************************************************************/
/*                                                                        */
/*  TMultiFontTextGadget :: GetTextElement -                              */
/*      Met dans 'format' les �ventuels param�tres pr�cedent le texte     */
/*      (contenu dans 'input'), dans 'size' la taille du texte (sans      */
/*      les param�tres) et retourne un pointeur sur le d�but du texte.    */
/*                                                                        */
/**************************************************************************/

char *	TMultiFontTextGadget :: GetTextElement  ( char *  	input,
						  char *        format,
						  int &		size )
   {
	register char *		p = format ;



	if  ( * input  ==  '[' )
	   {
		input ++ ;

		while  ( * input )
		   {
			if  ( * input  ==  ']' )
			   {
				if  ( * ( input - 1 )  ==  '\\' ) 
					* p ++ = * input ;
				else if  ( * ( p + 1 )  ==  '[' )
					input ;
				else
				   {
					input ++ ;
					break ;
				    }
			     }
			else
				* p ++ = * input ;

			input ++ ;
		     }
	      }

	* p ++ = 0 ;


// input pointe maintenant sur le premier caract�re apr�s la fin des param�tres;
// trouver la taille de ce texte
	size = 0 ;
	p = input ;

	while  ( * input )
	   {
		if  ( * input  ==  '['  &&  * ( input - 1 )  !=  '\\' )
			break ;
		size ++ ;
		input ++ ;
	    }
	

	return ( p ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TMultiFontTextGadget :: GetTextElement -                              */
/*      Interpr�te le texte sp�cifi�. Retourne les param�tres de          */
/*      formatage relatif � ce texte.                                     */
/*                                                                        */
/*  Param�tres -                                                          */
/*      text -                                                            */
/*              Cha�ne de caract�res � analyser.                          */
/*                                                                        */
/*      size -                                                            */
/*              Variable qui recevra la longueur du texte (hors param�tre */
/*              de formatage).                                            */
/*                                                                        */
/*      orifont -                                                         */
/*              Param�tres de la police par d�faut pour ce gadget.        */
/*                                                                        */
/*      newfont -                                                         */
/*              Combinaison des param�tres par d�faut de la police du     */
/*              gadget, et de ceux sp�cifi�s dans le texte.               */
/*                                                                        */
/*      oldfore -                                                         */
/*              Couleur de texte d'origine.                               */
/*                                                                        */
/*      oldback -                                                         */
/*              Couleur de fond d'origine.                                */
/*                                                                        */
/*      newfore -                                                         */
/*              Nouvelle couleur de texte.                                */
/*                                                                        */
/*      newback -                                                         */
/*              Nouvelle couleur de fond.                                 */
/*                                                                        */
/*                                                                        */
/*  Le param�tre "Text" a le format suivant :                             */
/*                                                                        */
/*      "[opt1]text1[opt2]text2..."                                       */
/*                                                                        */
/*  o�  [optx] repr�sente les param�tres (de police, de couleur) �        */
/*  appliquer � "textx".                                                  */
/*  Ces param�tres se pr�sentent ainsi :                                  */
/*                                                                        */
/*  [pdef[,pdef...]]                                                      */
/*                                                                        */
/*  Chaque "pdef" est une d�finition de param�tres sous forme d'un        */
/*  mot-cl�, suivi �ventuellement du signe ':' et d'une valeur            */
/*  optionnelle. Lorsque la valeur n'est pas sp�cifi�e, celle d'origine   */
/*  sera r�tablie pour l'�criture du texte.                               */
/*                                                                        */
/*  Les combinaisons permises sont les suivantes :                        */
/*                                                                        */
/*  Weight:intval                                                         */
/*      Le texte qui suit sera �crit dans une police ayant l'�paisseur    */
/*      indiqu�e par 'intval'.                                            */
/*                                                                        */
/*  Underline:0|1                                                         */
/*      Annule le soulign� (0) ou l'active (1).                           */
/*                                                                        */
/*  Italic:0|1                                                            */
/*      Annule l'italique (0) ou l'active (1).                            */
/*                                                                        */
/*  Height:intval                                                         */
/*      Le texte qui suit sera �crit avec la taille de police sp�cifi�e   */
/*      par 'intval'.                                                     */
/*                                                                        */
/*  Font:name                                                             */
/*      Le texte qui suit sera �crit avec la police dont le nom est       */
/*      sp�cifi� par 'name'.                                              */
/*                                                                        */
/*  Background:x                                                          */
/*      Le texte qui suit sera �crit avec la couleur de fond d�finie par  */
/*      la valeur RGB 'x'.                                                */
/*                                                                        */
/*  Foreground:x                                                          */
/*      Le texte qui suit sera �crit avec la couleur d�finie par la       */
/*      valeur RGB 'x'.                                                   */
/*                                                                        */
/*                                                                        */
/*  Exemples :                                                            */
/*      SetText ( "Hello World" ) ;                                       */
/*              Le texte du gadget sera "Hello World", �crit avec la      */
/*              police et les couleurs par d�faut.                        */
/*                                                                        */
/*      SetText ( "[Weight:900,Italic:1,Font:Times New Roman]Hello World" */
/*              Le texte du gadget sera "Hello World", �crit avec la      */
/*              police "Times New Roman" et les attributes Italique et    */
/*              gras, dans les couleurs par d�faut.                       */
/*                                                                        */
/*      SetText ( "[Italic:1]Hello [Italic,Weight:900]World" ) ;          */
/*              Le texte du gadget sera :                                 */
/*              . "Hello" �crit en italique (avec les autres param�tres   */
/*                par d�faut)                                             */
/*              . "World" �crit en gras (le mot-cl� "Italic", juste       */
/*                avant le mot-cl� "Weight", a provoqu� le retour au      */
/*                mode par d�faut en ce qui concerne l'italique .         */
/*                                                                        */
/*  La valeur retourn�e par cette fonction est un pointeur sur le premier */
/*  caract�re du texte (juste apr�s tout d�finition de param�tre).        */
/*  En ajoutant 'size' � cette valeur, on tombe sur la fin de la cha�ne,  */
/*  ou bien sur le premier caract�re introduisant une d�finition de       */
/*  param�tres.                                                           */
/*                                                                        */
/*  Note :                                                                */
/*      Les espace avant et apr�s le deux-points ou la virgule sont       */
/*      interdits (je n'avais pas envie de m'emm...)                      */
/*                                                                        */
/**************************************************************************/

# define	match(str)		( ! stricmp ( p, str ) )


char *	TMultiFontTextGadget :: GetTextElement  ( char *    	text,
						  int &		size,
						  LOGFONT & 	oldfont,
						  LOGFONT & 	newfont,
						  COLORREF  	oldfore,
						  COLORREF  	oldback,
						  COLORREF &	newfore,
						  COLORREF &	newback )
   {
	char			FormatBuffer [ 256 ] ;	
	register char *		TextStart ;
	register char *		p ;
	


// R�cup�rer les param�tres
	TextStart = GetTextElement ( text, FormatBuffer, size ) ;


// S'il n'y a pas de changement de param�tres, on peut retourner tout de suite
	if  ( ! * FormatBuffer )
		return  ( TextStart ) ;

// Remplacer les virgules du format par des zeros	
	for  ( p = FormatBuffer ; * p ; p ++ )
	   {
		if  ( * p  ==  ',' )
			* p = 0 ;
	    }

	* p = 0 ;	// Deux z�ros � la fin


// Interpr�ter chaque format
	for  ( p = FormatBuffer ; * p ; p += strlen ( p ) + 1 )
	   {
		register char *	  semicolon = strchr ( p, ':' ) ;
		register char *	  param     = 0 ;


	// Remplacer l'�ventuel ':' par un z�ro, de mani�re � avoir deux 
	// pointeurs sur des cha�nes termin�es par 0 (un pour le mot-cl�, 
	// l'autre pour le param�tre �ventuel)
		if  ( semicolon )
		   {
			* semicolon = 0 ;
			param       = semicolon + 1 ;
		    }
	

	// Int�rpr�tation des mots-cl� :
	// Graisse 
		if  ( match ( "Weight" )  ||  match ( "W" ) )
		   {
			if  ( param )
				newfont. lfWeight = atoi ( param ) ;
			else
				newfont. lfWeight = oldfont. lfWeight ;
		    }

	// Soulign�
		else if  ( match ( "Underline" )  ||  match ( "U" ) )
		   {
			if  ( param )
				newfont. lfUnderline = atoi ( param ) ? 1 : 0 ;
			else
				newfont. lfUnderline = oldfont. lfUnderline ;
		    }

	// Italique 
		else if  ( match ( "Italic" )  ||  match ( "I" ) )
		   {
			if  ( param )
				newfont. lfItalic = atoi ( param ) ?  1 : 0 ;
			else
				newfont. lfItalic = oldfont. lfItalic ;
		    }

	// Hauteur de police
		else if  ( match ( "Height" )  ||  match ( "H" ) )
		   {
			if  ( param )
			   {
				if  ( * param  ==  '+' )
					newfont. lfHeight += atoi ( param + 1 ) ;
				else if  ( * param  ==  '-' )
					newfont. lfHeight -= atoi ( param + 1 ) ;
				else
					newfont. lfHeight = atoi ( param ) ;
			     }
			else
				newfont. lfHeight = oldfont. lfHeight ;
		     }

	// Nom de la police
		else if  ( match ( "Font" )  ||  match ( "F" ) )
		   {
			if  ( param )
				strcpy ( newfont. lfFaceName, param ) ;
			else
				strcpy ( newfont. lfFaceName, oldfont. lfFaceName ) ;
		    }

	// Couleur de fond
		else if  ( match ( "Background" )  ||  match ( "Back" )  ||  match ( "Bk" ) )
		   {
			if  ( param )
				newback = atol ( param ) ;
			else
				newback = oldback ;
		    }

	// Couleur de texte
		else if  ( match ( "Foreground" )  ||  match ( "Fore" ) )
		   {
			if  ( param )
				newfore = atol ( param ) ;
			else
				newfore = oldfore ;
		     }


	// R�tablir le ':' pour que le strlen ( p ) + 1 se calcule bien
		if  ( semicolon )
			* semicolon = ':' ;
	     }



	return ( TextStart ) ;
    }
