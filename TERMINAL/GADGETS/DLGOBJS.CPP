/**************************************************************************/
/*                                                                        */
/*  dlgobjs.h  -  Christian Vigh, Mohamed Bentahar, 97/12/26.             */
/*                                                                        */
/*  Implémentation des objets pour la gestion des controles               */
/*  supplémentaires de Cheops.                                            */
/*                                                                        */
/**************************************************************************/

# pragma	option -k- -N- -r- -v- -vi- -y-
# include	<owl/owlall.h>
# pragma	hdrstop
# pragma	option -k. -N. -r. -v. -vi. -y.


# include	<string.h>
# include	<ctype.h>
# include	"uwcommon.h"
# include 	"util.h"
# include	"DlgObjs.h"





/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/***                                                                       ***/
/***    Implémentation de la classe  TDialogCheckBox.                      ***/
/***                                                                       ***/
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

DEFINE_RESPONSE_TABLE1 ( TDialogCheckBox, TCheckBox )
	EV_WM_SETTEXT,
	EV_WM_GETTEXT,
	EV_WM_SETFOCUS,
	EV_WM_KILLFOCUS,
END_RESPONSE_TABLE ;


/**************************************************************************/
/*                                                                        */
/*  TDialogCheckBox :: EvGetText, EvSetText -                             */
/*      Gestion du titre du controle sous forme de valeur booléenne.      */
/*                                                                        */
/**************************************************************************/

void	TDialogCheckBox :: EvSetText ( const char *  text )
   {
	if  ( strchr ( FieldData. Ctl, 'b' ) )
	   {
		if  ( ! stricmp ( text, "Y" )  ||  ! stricmp ( text, "O" ) )
		   {
			Check ( ) ;
		    }
		else if  ( ! stricmp ( text, "N" )  ||  ! * text )
		   {
			Uncheck ( ) ;	
		     }
	     }
	else
		TCheckBox :: EvSetText ( text ) ;
    }



void	TDialogCheckBox :: EvGetText  ( uint  buflen, char *  text )
   {
	if  ( ! UseStdGetText  &&  strchr ( FieldData. Ctl, 'b' ) )
	   {
		if  ( GetCheck ( )  ==  BF_CHECKED )
		   {
			strcpy ( text, "Y" ) ;
		     }
		else
		   {
			strcpy ( text, "N" ) ;
		    }
	    }                      
	else
		TCheckBox :: EvGetText ( buflen, text ) ;              
     }




/**************************************************************************/
/*                                                                        */
/*  TDialogCheckBox :: DefWindowProc -                                    */
/*      Sur réception de certains messages, le controle BorCheck          */
/*      réaffiche son titre. Pour ce faire, il s'envoie un message        */
/*      WM_GETTEXT puis réécrit le titre sans s'envoyer de WM_SETTEXT.    */
/*      Cette DefWindowProc encapsule l'envoi du WM_GETTEXT de manière    */
/*      à ce que le controle utilise le bon titre.                        */
/*                                                                        */
/**************************************************************************/

LRESULT	TDialogCheckBox :: DefWindowProc ( uint  msg, WPARAM  wparam, LPARAM  lparam )
   {
	register LRESULT		Status ;


	switch ( msg )
	   {
		case	WM_SETFOCUS :
		case	WM_KILLFOCUS :
		case	WM_ACTIVATE :
		case	WM_ACTIVATEAPP :  
		case	WM_MOUSEACTIVATE :
		case	WM_SHOWWINDOW :
		case	WM_CREATE :
		case	WM_PAINT :
			UseStdGetText = TRUE ;
			Status = TCheckBox :: DefWindowProc ( msg, wparam, lparam )  ;
			UseStdGetText = FALSE ;
			break ;

		default :
			Status = TCheckBox :: DefWindowProc ( msg, wparam, lparam )  ;
	     }

	return ( Status ) ;
    }			

		


/**************************************************************************/
/*                                                                        */
/*  TDialogCheckBox :: EvSetFocus, EvKillFocus -                          */
/*      Gèrent :                                                          */
/*      . le changement de titre généré par la BorCheck lorsque le        */
/*        controle reçoit ou perd le focus                                */
/*      . l'envoi d'un message de notification vers le parent.            */
/*                                                                        */
/**************************************************************************/

void	TDialogCheckBox :: EvSetFocus  ( HWND  lost )
   {
	TCheckBox :: EvSetFocus ( lost ) ;

	if  ( Parent )
		Parent -> SendMessage ( WM_COMMAND, Attr. Id,
			MAKELPARAM ( HWindow, BN_SETFOCUS ) ) ;
      }




void	TDialogCheckBox :: EvKillFocus  ( HWND  gained )
   {
	TCheckBox :: EvKillFocus ( gained ) ;
	
	if  ( Parent )
		Parent -> SendMessage ( WM_COMMAND, Attr. Id,
			MAKELPARAM ( HWindow, BN_KILLFOCUS ) ) ;
      }







/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/***                                                                       ***/
/***    Implémentation de la classe  TDialogEdit.                          ***/
/***                                                                       ***/
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

DEFINE_RESPONSE_TABLE1 ( TDialogEdit, TEdit )
	EV_WM_SETTEXT,
	EV_WM_GETTEXT,
	EV_WM_MOUSEACTIVATE,
	EV_WM_SETFOCUS,
	EV_WM_KILLFOCUS,
	EV_WM_LBUTTONDOWN,
END_RESPONSE_TABLE ;




/**************************************************************************/
/*                                                                        */
/*   Validateur pour les champs Date.                                     */
/*                                                                        */
/**************************************************************************/

class	TDateValidator 	:  public  TValidator
   {
	public :
		TDateValidator  ( TDialogEdit *	  edit )
			: TValidator ( )
		   { 
			EditParent = edit ;
		     }


		BOOL	IsValidInput  ( char *  str, bool ) ;

		void	Error ( )	// Laisser Unix gérer l'erreur
		   {
		     }

	private :
		TDialogEdit *		EditParent ;
		
      } ;



/**************************************************************************/
/*                                                                        */
/*   Fonction de mise en forme/validation d'un champ date.                */
/*                                                                        */
/**************************************************************************/

BOOL	TDateValidator :: IsValidInput  ( char *  input, bool )
   {
	char			Day    [ 32 ],
				Month  [ 32 ],
				Year   [ 32 ] ;
	BOOL			HasCentury ;
	register char *		slash1		=  0,
		      *		slash2 		=  0 ;
	register char *		p ;
	int			insertions	=  0 ;


	HasCentury = ( EditParent -> FieldData. Format [1]  ==  '8' ) ;
	slash1 	   = strchr ( input, '/' ) ;

	if  ( slash1 )
		slash2 = strchr ( slash1 + 1, '/' ) ;

	memset ( Day  , 0, sizeof ( Day   ) ) ;
	memset ( Month, 0, sizeof ( Month ) ) ;
	memset ( Year , 0, sizeof ( Year  ) ) ;
	
// Extraire la partie 'Jour'
	if  ( slash1 )
	   {
		if  ( slash1  !=  input )
			strncpy ( Day, input, ( int ) ( slash1 - input ) ) ;
	     }
	else
		strcpy ( Day, input ) ;

// La partie 'Mois'
	if  ( slash1 )
	   {
		if  ( slash2 )
		   {
			if  ( slash2  >  slash1 + 1 )
				strncpy ( Month, slash1 + 1,
					( int ) ( slash2 - slash1 - 1 ) ) ;
		     }
		else
			strcpy ( Month, slash1 + 1 ) ;
	     }

// La partie année 
	if  ( slash2 )
		strcpy ( Year, slash2 + 1 ) ;

//  Vérifier le jour
	switch ( strlen ( Day ) )
	   {
		case	0 :
			* Month = * Year = 0 ;
			break ;

		case	1 :
			if  ( slash1 )
			   {
				Day  [1] = Day [0] ;
				Day [0] = '0' ;
				insertions ++ ;
			     }

		case	2 :
			Day [2] = '/' ;

			if  ( ! slash1 )
				insertions ++ ;
			break ;

		default :
			Day [3] = Day [2] ;
			Day [2] = '/' ;
			insertions ++ ;
			* Month = * Year = 0 ;
			break ;
	     }

//  Le mois				
	switch ( strlen ( Month ) )
	   {
		case	0 :
			* Year = 0 ;
			break ;

		case	1 :
			if  ( slash2 )
			   {
				Month  [1] = Month [0] ;
				Month [0] = '0' ;
				insertions ++ ;
			     }

		case	2 :
			Month [2] = '/' ;

			if  ( ! slash2 )
				insertions ++ ;
			break ;

		default :
			Month [3] = Month [2] ;
			Month [2] = '/' ;
			insertions ++ ;
			* Year = 0 ;
			break ;
	     }

// L'année
	switch ( strlen ( Year ) )
	   {
		case	0 :
		case 	1 :
		case	2 :
			break ;

		case	3 :
		case	4 :
			if  ( ! HasCentury )
				Year [2] = 0 ;
			break ;
	     }
	   

	wsprintf ( input, "%s%s%s", Day, Month, Year ) ;
	p = input ;

	while  ( * p )
	   {
		if  ( ! isdigit ( * p )  &&  * p  !=  '/' )
		   {
			* p = 0 ;
			break ;
		    }

		p ++ ;
	     }

// Pour repositionner le caret
	while  ( insertions -- )
	   {
		EditParent -> PostMessage ( WM_KEYDOWN, VK_RIGHT, 0L ) ;
		EditParent -> PostMessage ( WM_KEYUP  , VK_RIGHT, 0L ) ;
	     }

	return ( TRUE ) ;
    }

					



/**************************************************************************/
/*                                                                        */
/*   Constructeur -                                                       */
/*      Cadre le texte à droite s'il s'agit d'un numérique.               */
/*                                                                        */
/**************************************************************************/

TDialogEdit :: TDialogEdit      ( TWindow *		parent,
				  int			resid,
				  TDialogEntryData &	data,
				  DLGITEMTEMPLATE &     /*tmpl*/ ) 
	: TEdit ( parent, resid, 0, 0 )
   {
	FieldData      = data ;
	UseStdGetText  = FALSE ;

	switch ( tolower ( * data. Format ) )
	   {
		case	'd' :
			SetValidator ( new  TDateValidator ( this ) ) ;
			break ;
	     }
     }




/**************************************************************************/
/*                                                                        */
/*  TDialogEdit :: EvSetText -                                            */
/*      Supprime les espaces en début de champ.                           */
/*                                                                        */
/**************************************************************************/

void	TDialogEdit :: EvSetText  ( const char *  text )
   {
	char			buffer [ 256 ] ;
	register char *		p  =  const_cast <char *> (text) ;


// Champs numériques : supprimer les espaces de début
	if  ( * FieldData. Format  ==  'n' )
	   {
		while  ( * p  &&  isspace ( * p ) )
			p ++ ;
	    }

// Champs date : formater selon le filtre défini
	if  ( * FieldData. Format  ==  'd' )
	   {
		if  ( FieldData. Format [1]  ==  '6' )
			wsprintf ( buffer, "%c%c/%c%c/%c%c",
				p [0], p [1], p [2], p [3], p [4], p [5] ) ;
		else
			wsprintf ( buffer, "%c%c/%c%c/%s",
				p [0], p [1], p [2], p [3], p + 4 ) ;
		p = buffer ;
	     }


// Le paramètre text est une constante : c'est une erreur !
// La fonction TEdit :: EvSetText appelle en effet DefaultProcessing qui 
// récupère les paramètres en appelant GetCurrentEvent et en se foutant 
// royalement de son paramètre 'text'. Toute modification de l'adresse de
// 'text' ou tout passage d'un pointeur sur une autre adresse ne sert à rien.
	DefWindowProc ( WM_SETTEXT, 0, ( LPARAM ) p ) ;

	if  ( :: GetFocus ( )  ==  HWindow )
		SetSelection ( 0, 0xFFFF ) ;
     }



/**************************************************************************/
/*                                                                        */
/*  TDialogEdit :: EvGetText -                                            */
/*      Post-processing des données saisies.                              */
/*                                                                        */
/**************************************************************************/

void	TDialogEdit :: EvGetText  ( uint  length, char *  title )
   {
	register char *		p, 
		      *		sl ;
	char			Buffer [ 256 ] ;
	int			title_length ;
	int			dd	=  0,
				mm	=  0,
				yy	=  0 ;


	TEdit :: EvGetText ( length, title ) ;

	if  ( UseStdGetText )
		return ;

	switch ( tolower ( * FieldData. Format ) )
	   {
	// Champs numériques : remplacer la virgule par un point décimal
	// Remettre les espaces de début, sinon la librairie fm croira que le
	// champ a été modifié
		case	'n' :
			p = strchr ( title, ',' ) ;

			if  ( p )
				* p = '.' ;

			title_length = atoi ( FieldData. Format + 1 ) - 
					strlen ( title ) ;
			p      = Buffer ;

			while  ( title_length --  >  0 )
				* p ++ = ' ' ;
			* p = 0 ;
			strcat ( Buffer, title ) ;
			strcpy ( title, Buffer ) ;
			break ;

	// Champs date : supprimer les '/'
		case	'd' :
			dd = atoi ( title ) ;
			sl = strchr ( title, '/' ) ;

			if  ( sl )
			   {
				mm = atoi ( sl + 1 ) ;	
				sl = strchr ( sl + 1, '/' ) ;

				if  ( sl )
					yy = atoi ( sl + 1 ) ;
			     }

			if  ( ! dd  &&  ! mm  &&  ! yy )
				* title = 0 ;				
			else if  ( FieldData. Format [1]  ==  '6' )
				wsprintf ( title, "%.2d%.2d%.2d", dd, mm,
					yy % 100 ) ;
			else
				wsprintf ( title, "%.2d%.2d%d", dd, mm, yy ) ;
			break ;           
	      }
      }




/**************************************************************************/
/*                                                                        */
/*  TDialogEdit :: EvMouseActivate, EvLButtonDown -                       */
/*      Affiche la sélection.                                             */
/*                                                                        */
/**************************************************************************/

uint	TDialogEdit :: EvMouseActivate ( HWND  top, uint  hittest, uint  msg )
   {
	uint		Status  =  TEdit :: EvMouseActivate ( top, hittest, msg ) ;

	if  ( :: GetFocus ( )  ==  HWindow )
		SetSelection ( 0, 0xFFFF ) ;

	return ( Status ) ;
     }


void	TDialogEdit :: EvLButtonDown  ( uint  modkeys, TPoint &  pt )
   {
	TEdit :: EvLButtonDown ( modkeys, pt ) ;

	if  ( :: GetFocus ( )  ==  HWindow )
		SetSelection ( 0, 0xFFFF ) ;
     }




/**************************************************************************/
/*                                                                        */
/*  TDialogEdit :: EvSetFocus -                                           */
/*      Sélectionne la totalité du champ si ce dernier reçoit le focus.   */
/*                                                                        */
/**************************************************************************/

void	TDialogEdit :: EvSetFocus ( HWND lost )
   {
//  Cadrer à gauche pour la saisie
	if  ( * FieldData. Format  ==  'n'  ||  * FieldData. Format  ==  'd' )
	   {
		SetAlignment ( TA_RIGHT ) ;
	     }

	TEdit :: EvSetFocus ( lost ) ;
	SetSelection ( 0, 0xFFFF ) ;
    }




void	TDialogEdit :: EvKillFocus ( HWND gained )
   {
//  Cadrer à gauche pour la saisie
	if  ( * FieldData. Format  ==  'n'  ||  * FieldData. Format  ==  'd' )
		SetAlignment ( TA_LEFT ) ;

	TEdit :: EvKillFocus ( gained ) ;
    }


/**************************************************************************/
/*                                                                        */
/*  TDialogEdit :: DefWindowProc -                                        */
/*      Voir TDialogCheckBox :: DefWindowProc.                            */
/*                                                                        */
/**************************************************************************/

LRESULT	TDialogEdit :: DefWindowProc ( uint  msg, WPARAM  wparam, LPARAM  lparam )
   {
	register LRESULT		Status ;


	switch ( msg )
	   {
	// Utiliser le vrai EvGetText lorsque le controle s'appelle lui-meme
	// pour se rafraichir
		case	WM_CREATE :
		case	WM_SHOWWINDOW :
		case	WM_SETFOCUS :
		case	WM_KILLFOCUS :
		case	WM_ACTIVATE :
		case	WM_ACTIVATEAPP :  
		case	WM_MOUSEACTIVATE :
		case	WM_PAINT :
			UseStdGetText = TRUE ;
			Status = TEdit :: DefWindowProc ( msg, wparam, lparam )  ;
			UseStdGetText = FALSE ;
			break ; 

	//  Si le controle possède le style ES_MULTILINE, remplacer RETURN par
	//  TAB, sauf pour le dernier champ, où la saisie est bloquée
		case	WM_KEYDOWN :
			if ( wparam  ==  VK_RETURN  &&
				( GetWindowLong ( GWL_STYLE )  &  ES_MULTILINE )  &&
				strchr ( "nd", * FieldData. Format )  !=  NULL  &&
				Parent )
			   {
				Parent -> HandleMessage ( msg, wparam, lparam ) ;
				return ( 0L ) ;
			     }
			else
				Status = TEdit :: DefWindowProc ( msg, wparam, lparam ) ;
			break ;

	//  Pour les dates, pour flouzer TEdit :: EvChar qui s'alloue un
	//  buffer de la taille exacte du texte saisi, et pour TDateValidator
	//  qui est susceptible de rajouter des caractères en cours de saisie
	//  (et donc de déborder du buffer), on retourne la taille d'une date
	//  complète (séparateurs inclus).
	//  Pour ne pas décontenancer TEdit :: EvChar, on appelle artificiellement
	//  Validator -> IsValidInput afin de calculer la taille réelle que
	//  possèdera le texte du controle après modification de son contenu
	//  par cette fonction.
		case	WM_GETTEXTLENGTH :
			if  ( * FieldData. Format  ==  'd' )
				return ( FieldData. Format [1] - '0' + 2 ) ;
			Status = TEdit :: DefWindowProc ( msg, wparam, lparam )  ;
			break ;
				
	//  Pour les dates, prendre en compte les caractères séparateurs pour
	//  la longueur max
		case	EM_LIMITTEXT :
			if  ( * FieldData. Format  ==  'd' )
				wparam += 2 ;

		default :
			Status = TEdit :: DefWindowProc ( msg, wparam, lparam )  ;
	     }

	return ( Status ) ;
    }			






/**************************************************************************/
/*                                                                        */
/*  TDialogEdit :: SetAlignment -                                         */
/*      Cette fonction est censée aligner le texte d'un controle EDIT à   */
/*      gauche ou à droite (eh oui ! il ne suffit pas de modifier un      */
/*      bit de style !).                                                  */
/*                                                                        */
/**************************************************************************/

void	TDialogEdit :: SetAlignment ( int  /*alignment*/ ) 
   {
    }








/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/***                                                                       ***/
/***    Implémentation de la classe  TDialogGroupBox.                      ***/
/***                                                                       ***/
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

DEFINE_RESPONSE_TABLE1 ( TDialogGroupBox, TGroupBox )
	EV_WM_SETTEXT,
	EV_WM_GETTEXT,
	EV_WM_SETFOCUS,
	EV_WM_KILLFOCUS,
END_RESPONSE_TABLE ;




/**************************************************************************/
/*                                                                        */
/*  Constructeur.                                                         */
/*                                                                        */
/**************************************************************************/

TDialogGroupBox :: TDialogGroupBox   ( TWindow *		parent,
				       int			resid,
				       TDialogEntryData &	data,
				       DLGITEMTEMPLATE *	first,
				       DLGITEMTEMPLATE *	last )
	: TGroupBox ( parent, resid )
    {
	
	FieldData =  data ;
	FirstId	  = 
	LastId	  =  0 ;

	if  ( IsRadioButton ( first ) )
	   {
		FirstId = first -> dtilID ;
		first ++ ;

		while  ( first  <=  last )
		   {
			if  ( ! IsRadioButton ( first ) )
				break ;

			first ++ ;
		     }

		first -- ;
		LastId = first -> dtilID ;
	     }
      }




/**************************************************************************/
/*                                                                        */
/*  TDialogGroupBox :: IsRadioButton -                                    */
/*      Renvoie TRUE si le controle spécifié est un bouton radio.         */
/*                                                                        */
/**************************************************************************/

BOOL	TDialogGroupBox :: IsRadioButton ( DLGITEMTEMPLATE *  item )
   {
	if  ( ( ! stricmp ( item -> dtilClass, "BUTTON" )  ||
		! stricmp ( item -> dtilClass, "BorRadio" ) )  &&
		( item -> dtilStyle & BS_AUTORADIOBUTTON ) )
		return ( TRUE ) ;
	return ( FALSE ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TDialogGroupBox :: GetActiveId -                                      */
/*      Retourne l'ID du bouton radio coché.                              */
/*                                                                        */
/**************************************************************************/

int	TDialogGroupBox :: GetActiveId ( )
   {
	register int	i ;

	
	if  ( FirstId )
	   {
		for  ( i = FirstId ; i <= LastId ; i ++ )
		   {
			if  ( IsDlgButtonChecked ( i ) )
				return ( i ) ;
		     }
	     }

	return ( 0 ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TDialogGroupBox :: SetActiveId -                                      */
/*      Coche le bouton dont l'ID est spécifié.                           */
/*                                                                        */
/**************************************************************************/

void	TDialogGroupBox :: SetActiveId  ( int  newid )
   {
	if  ( FirstId  &&  FirstId  <=  newid  &&  LastId  >=  newid ) 
		CheckRadioButton ( FirstId, LastId, newid ) ;
    }





/**************************************************************************/
/*                                                                        */
/*  TDialogGroupBox :: SetActiveChoice -                                  */
/*      Coche le bouton correspondant au choix donné par le caractère     */
/*      passé en argument.                                                */
/*                                                                        */
/**************************************************************************/

void	TDialogGroupBox :: SetActiveChoice ( char  choice )
   {
	if  ( ! FirstId )
		return ;	// En cas de mauvaise définition de la boite de dialogue


	register int		i 	=  0 ;
	register char *		p	=  FieldData. CpCtl ;


	choice = toupper ( choice ) ;

	while  ( * p )
	  {
		if  ( toupper ( * p )  ==  choice )
		   {
			SetActiveId ( FirstId + i ) ;
			return ;
		    }

		p ++, i ++ ;
	    }
     }




/**************************************************************************/
/*                                                                        */
/*  TDialogGroupBox :: GetActiveChoice -                                  */
/*      Retourne le choix correspondant au bouton actuellement coché.     */
/*                                                                        */
/**************************************************************************/

char	TDialogGroupBox :: GetActiveChoice ( )
   {
	register int		i	=  GetActiveId ( ) ;


	if  ( i )
	   {
		i -= FirstId ;

		if  ( i  <  strlen ( FieldData. CpCtl ) )
			return ( FieldData. CpCtl [i] ) ;
	     }

	return ( 0 ) ;
     }




/**************************************************************************/
/*                                                                        */
/*  TDialogGroupBox :: EvGetText, EvSetText -                             */
/*      Gestion du titre du controle sous forme de valeur booléenne.      */
/*                                                                        */
/**************************************************************************/

void	TDialogGroupBox :: EvSetText ( const char *  text )
   {
	if  ( strchr ( FieldData. Ctl, 'c' ) )
	   {
		SetActiveChoice ( * text ) ;
	     }
	else
		TGroupBox :: EvSetText ( text ) ;
    }



void	TDialogGroupBox :: EvGetText  ( uint  buflen, char *  text )
   {
	if  ( ! UseStdGetText  &&  strchr ( FieldData. Ctl, 'c' ) )
	   {
		* text = GetActiveChoice ( ) ;
		* ( text + 1 ) = 0 ;
	    }                      
	else
		TGroupBox :: EvGetText ( buflen, text ) ;              
     }




/**************************************************************************/
/*                                                                        */
/*  TDialogGroupBox :: DefWindowProc -                                    */
/*      Sur réception de certains messages, le controle BorCheck          */
/*      réaffiche son titre. Pour ce faire, il s'envoie un message        */
/*      WM_GETTEXT puis réécrit le titre sans s'envoyer de WM_SETTEXT.    */
/*      Cette DefWindowProc encapsule l'envoi du WM_GETTEXT de manière    */
/*      à ce que le controle utilise le bon titre.                        */
/*                                                                        */
/**************************************************************************/

LRESULT	TDialogGroupBox :: DefWindowProc ( uint  msg, WPARAM  wparam, LPARAM  lparam )
   {
	register LRESULT		Status ;


	switch ( msg )
	   {
		case	WM_SETFOCUS :
		case	WM_KILLFOCUS :
		case	WM_ACTIVATE :
		case	WM_ACTIVATEAPP :  
		case	WM_MOUSEACTIVATE :
		case	WM_SHOWWINDOW :
		case	WM_CREATE :
		case	WM_PAINT :
			UseStdGetText = TRUE ;
			Status = TGroupBox :: DefWindowProc ( msg, wparam, lparam )  ;
			UseStdGetText = FALSE ;
			break ;

		default :
			Status = TGroupBox :: DefWindowProc ( msg, wparam, lparam )  ;
	     }

	return ( Status ) ;
    }			

		
	


/**************************************************************************/
/*                                                                        */
/*  TDialogGroupBox :: EvSetFocus -                                       */
/*      Donne le controle au premier bouton (si aucun n'est coché) ou     */
/*      au bouton qui était coché dans le cas contraire.                  */
/*      Envoie un message de notification (EN_SETFOCUS) au parent.        */
/*                                                                        */
/**************************************************************************/

void	TDialogGroupBox :: EvSetFocus  ( HWND  lost )
   {
	if  ( FirstId )
	   {
		int	id  =  GetActiveId ( ) ;

		:: SetFocus ( GetDlgItem ( ( id ) ?  id : FirstId ) ) ;

		Parent -> SendMessage ( WM_COMMAND, Attr. Id,
			MAKELPARAM ( HWindow, EN_SETFOCUS ) ) ;
	     }

	TGroupBox :: EvSetFocus ( lost ) ;
     }
			
		

/**************************************************************************/
/*                                                                        */
/*  TDialogGroupBox :: EvKillFocus -                                      */
/*      Envoie un message de notification au parent.                      */
/*                                                                        */
/**************************************************************************/

void	TDialogGroupBox :: EvKillFocus  ( HWND  gained )
   {
	TGroupBox :: EvKillFocus ( gained ) ;
	
	if  ( Parent )
		Parent -> SendMessage ( WM_COMMAND, Attr. Id,
			MAKELPARAM ( HWindow, EN_KILLFOCUS ) ) ;
    }





/**************************************************************************/
/*                                                                        */
/*  TDialogGroupBox :: Create -                                           */
/*      Change le parent des boutons qui appartiennent à la groupbox.     */
/*      La groupbox devient le nouveau parent des buttons.                */
/*                                                                        */
/**************************************************************************/

BOOL	TDialogGroupBox :: Create ( )
   {
	register BOOL		Status 	=  TGroupBox :: Create ( ) ;


	if  ( Parent  &&  FirstId )
	   {
		for  ( int  i = FirstId ; i  <=  LastId ; i ++ )
		   {
			HWND		ButtonHwnd	=  Parent -> GetDlgItem ( i ) ;
			TRect		ButtonRect ;
			TPoint		TopLeft, BottomRight ;

		// Recadrer les boutons en convertissant leurs coordonnées par
		// rapport à la groupbox
			:: GetWindowRect ( ButtonHwnd, & ButtonRect ) ;
			TopLeft      = ButtonRect. TopLeft ( ) ;
			BottomRight  = ButtonRect. BottomRight ( ) ;

			ScreenToClient ( TopLeft ) ;
			ScreenToClient ( BottomRight ) ;

			ButtonRect. top    = TopLeft. y ;
			ButtonRect. left   = TopLeft. x ;
			ButtonRect. bottom = BottomRight. y ;
			ButtonRect. right  = BottomRight. x ;

			:: SetParent ( ButtonHwnd, HWindow )  ;
			:: MoveWindow ( ButtonHwnd, ButtonRect. left, 
				ButtonRect. top,
				ButtonRect. Width ( ),
				ButtonRect. Height ( ),
				FALSE ) ;
		    }			
	     }

	return ( Status ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TGroupBox :: PreProcessMsg -                                          */
/*      Gère les touches de direction.                                    */
/*                                                                        */
/**************************************************************************/

BOOL	TDialogGroupBox :: PreProcessMsg  ( MSG &  msg )
   {
	register int		active_id ;


	if  ( ! IsChild  ( msg. hwnd )  ||  ! FirstId )
		return ( TGroupBox :: PreProcessMsg ( msg ) ) ;


	switch ( msg. message )
	   {
	//  La groupbox doit gérer les déplacements d'un bouton à l'autre au
	//  moyen des touches fléchées.
		case	WM_KEYDOWN :
			switch  ( msg. wParam )
			   {
				case	VK_LEFT :
					active_id = GetActiveId ( ) ;
					active_id -- ;

					if  ( active_id  <  FirstId )
						active_id = LastId ;
					SetActiveId ( active_id ) ;
					:: SetFocus ( GetDlgItem ( active_id ) ) ;
					return ( TRUE ) ;

				case	VK_RIGHT :
					active_id = GetActiveId ( ) ;
					active_id ++ ;

					if  ( active_id  >  LastId )
						active_id = FirstId ;
					SetActiveId ( active_id ) ;
					:: SetFocus ( GetDlgItem ( active_id ) ) ;
					return ( TRUE ) ;
			    }
	    }

	return  ( TGroupBox :: PreProcessMsg ( msg ) ) ;
    }

