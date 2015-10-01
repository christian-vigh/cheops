/**************************************************************************/
/*                                                                        */
/*  Util.cpp  -  Christian Vigh, 94/09/19.                                */
/*                                                                        */
/*  Fonctions d'intérêt général.                                          */
/*                                                                        */
/**************************************************************************/

/**************************************************************************

   MODIFICATIONS :

   29/04/97 (CV)
	. Prise en charge du paramètre HardCopyMode de la section [Settings]
	  du fichier .INI dans la fonction CreateDialogBoxTemplate.
	  Lorsque ce paramètre booléen est à true, il se passe les choses
	  suivantes :

	  . La classe de boite de dialogue standard est utilisée
	  . Les controles "BorStatic" sont changés en "STATIC"
	  . Les controles "BorShade" ayant le style BSS_GROUP ou BSS_RGROUP
	    (les cadres, donc) se voient supprimer le bit de style WS_VISIBLE.

	  Tout ceci afin de permettre d'effectuer des recopies d'écran qui ne
	  contiennent pas de zones grises (ces zones passent mal à la 
	  photocopieuse).

   02/07/97 (CV)
	Ajout de la fonction EnableTextItem.

 **************************************************************************/


# include	"AllTerm.h"
# pragma	hdrstop

# include	"Gadgets/DlgObjs.h"



/**************************************************************************/
/*                                                                        */
/*  StringToSpeed, SpeedToString :                                        */
/*      Convertit une chaîne de caractères en une constante spécifiant    */
/*      une vitesse de transmission, et vice-versa.                       */
/*                                                                        */
/**************************************************************************/

long int	StringToSpeed   ( char *  speed )
   {
	register long int	l 	=  atol ( speed ) ;

	switch  ( l )
	   {
		case	300  :	return ( CBR_300    ) ;
		case    600  :	return ( CBR_600    ) ;
		case   1200  :	return ( CBR_1200   ) ;
		case   2400  :	return ( CBR_2400   ) ;
		case   4800  :	return ( CBR_4800   ) ;
		case   9600  :	return ( CBR_9600   ) ;
		case  19200  :	return ( CBR_19200  ) ;
		case  38400L :	return ( CBR_38400  ) ;
		case  56000L :	return ( CBR_56000  ) ;
		case 128000L :	return ( CBR_128000 ) ;
		case 256000L :	return ( CBR_256000 ) ;
		default      :  return ( CBR_9600   ) ;
	    }
	  }



char *	SpeedToString   ( long int   speed )
	{
	switch  ( speed )
		{
		case  CBR_300    : return ( "300"    ) ;
		case  CBR_600    : return ( "600"    ) ;
		case  CBR_1200   : return ( "1200"   ) ;
		case  CBR_2400   : return ( "2400"   ) ;
		case  CBR_4800   : return ( "4800"   ) ;
		case  CBR_9600   : return ( "9600"   ) ;
		case  CBR_19200  : return ( "19200"  ) ;
		case  CBR_38400  : return ( "38400"  ) ;
		case  CBR_56000  : return ( "56000"  ) ;
		case  CBR_128000 : return ( "128000" ) ;
		case  CBR_256000 : return ( "256000" ) ;
		default     	 : return ( "9600"   ) ;
		 }
	  }

/**************************************************************************/
/*                                                                        */
/*  StringToSpeed, DelayoString :                                         */
/*      Convertit une chaîne de caractères en une constante spécifiant    */
/*      un delai de temporisation de transmission, et vice-versa.         */
/*                                                                        */
/**************************************************************************/

char *	DelayToString   ( int d )
	{
	static char delai [ 7 ] ;

	sprintf ( delai, "%7.7d", d ) ;

	return ( delai ) ;
	}

int 	StringToDelay   ( char * delai )
	{
	return ( atoi ( delai ) );
	}




/**************************************************************************/
/*                                                                        */
/*  StringToParity, ParityToString :                                      */
/*      La même chose, mais pour la parité.                               */
/*                                                                        */
/**************************************************************************/

UINT	StringToParity  ( char *  buffer )
   {
	if ( ! stricmp ( buffer, "None" )  ||  ! stricmp ( buffer, "N" ) )
		return ( NOPARITY ) ;
	else if ( ! stricmp ( buffer, "Even" )  ||  ! stricmp ( buffer, "E" ) )
		return ( EVENPARITY ) ;
	else if ( ! stricmp ( buffer, "Mark" )  ||  ! stricmp ( buffer, "M" ) )
		return ( MARKPARITY ) ;
	else if ( ! stricmp ( buffer, "Odd" )  ||  ! stricmp ( buffer, "O" ) )
		return ( ODDPARITY ) ;
	else if ( ! stricmp ( buffer, "Space" )  ||  ! stricmp ( buffer, "S" ) )
		return ( SPACEPARITY ) ;
	else
		return ( NOPARITY ) ;
     }



char *	ParityToString  ( UINT  parity )
   {
	switch ( parity )
	   {
		case	EVENPARITY :
			return ( "Even" ) ;
		case	MARKPARITY :
			return ( "Mark" ) ;
		case	ODDPARITY :
			return ( "Odd" ) ;
		case	SPACEPARITY :
			return ( "Space" ) ;
		case	NOPARITY :
		default :
			return ( "None" ) ;
	     }
     }




/**************************************************************************/
/*                                                                        */
/*   StringToLong :                                                       */
/*      Convertir une chaîne de caractères en entier long.                */
/*      Les préfixes indiquant une base (0x, 0o, etc...) sont reconnus.   */
/*                                                                        */
/**************************************************************************/

long int	StringToLong  ( char *  buffer )
   {
	static char		digits []	=  "0123456789abcdef" ;
	register long int	value		=  0L ;
	register int		base		=  10,
				c ;


	if  ( * buffer  ==  '0' )	// Zero : peut-être une base derrière
	   {
		buffer ++ ;
		c = tolower ( * buffer ) ;

		switch ( c )
		   {
			case	'b' :		// C'est du binaire
				base = 2 ;  break ;

			case	'o' :		// de l'octal
				base = 8 ;  break ;

			case	'd' :		// du décimal
				base = 10 ; break ;

			case	'x' :		// de l'hexa
				base = 16 ; break ;

			default :
				if  ( ! isxdigit ( c ) )
					return ( 0L ) ;
				base = 8 ;
		     }

		buffer ++ ;	// Pour pointer après la lettre de la base
	      }


	while  ( * buffer  &&  isxdigit ( * buffer ) )
	   {
		c = ( int ) ( strchr ( digits, tolower ( * buffer ) ) -
				digits ) ;

		if  ( c > base )	// Ignorer les caractères après le nombre
			return ( value ) ;

		value = ( value * base ) + c ;
		buffer ++ ;
	    }

	return ( value ) ;
    }






/**************************************************************************/
/*                                                                        */
/*  CreateDialogBoxTemplate :                                             */
/*      A partir d'une définition de boîte de dialogue en provenance      */
/*      d'Unix, crée une DLGTEMPLATE et retourne le handle de mémoire     */
/*      globale contenant le template.                                    */
/*                                                                        */
/**************************************************************************/

# define	GetI16(field,pdata)     CHARtoI16 ( field, pdata ) ; pdata += 2
# define	GetI32(field,pdata)	CHARtoI32 ( field, pdata ) ; pdata += 4

# if	STANDALONE
# define	GetStr(field,pdata)					\
	p = ( unsigned char * ) buffer ;				\
	while ( * pdata ) 						\
		* p ++ = * pdata ++ ;					\
	* p = 0 ;							\
	pdata ++ ;							\
	field = new char [ strlen ( buffer ) + 1 ] ; 			\
	strcpy ( field, buffer )
# else
# define	GetStr(field,pdata)					\
	p = ( unsigned char * ) buffer ;				\
	while ( * pdata ) 						\
		* p ++ = * pdata ++ ;					\
	* p = 0 ;							\
	pdata ++ ;							\
	field = new char [ strlen ( buffer ) + 1 ] ; 			\
	Device -> HostToLocalCharSet ( buffer ) ;			\
	strcpy ( field, buffer )
# endif


# pragma  	argsused

void	CreateDialogBoxTemplate ( unsigned char *  	Data,
				  int  			DataSize,
				  DIALOGBOX &		DialogBoxTemplate )
   {
	char				buffer [ MAX_PARAMETER_LENGTH ] ;
	register unsigned char *        p,
			       *	pdata ;
	DLGTEMPLATE			DlgTemplate ;
	DLGITEMTEMPLATE *		ItemTemplates,
			*		pItem ;
	TDialogEntryData *		DlgDatas ;
	FONTINFO			FontInfo ;
	register int			i ;
	int				x, y ;
	BOOL				HardcopyMode ;
	integer16			remote_data_size, 
					unix_data_size ;


// Si le paramètre HardCopyMode de la section [Settings] est à TRUE, on
// enlève toute référence à des BorShade et on utilise les controles standard
// de Windows. Les BorStatic deviennent alors des STATIC.
	HardcopyMode = Configuration -> GetBoolean ( "Settings", "HardCopyMode", FALSE ) ;


// Dans la définition du dialogue (structure TDialogDefinition), on saute
// l'id de la boîte ainsi que les coordonnées en mode texte.
// (ne pas oublier qu'une structure UTDialogBox précède cette définition)
	pdata = Data + sizeof ( integer16 ) + sizeof ( TDialogDimension ) ;

// pdata pointe sur les coordonnées graphiques : on les récupère
	GetI16 ( DlgTemplate. dtX , pdata ) ;
	GetI16 ( DlgTemplate. dtY , pdata ) ;
	GetI16 ( DlgTemplate. dtCX, pdata ) ;
	GetI16 ( DlgTemplate. dtCY, pdata ) ;

// Récupération du nom de menu
	GetStr ( DlgTemplate. dtMenuName, pdata ) ;

// de la classe
	GetStr ( DlgTemplate. dtClassName, pdata ) ;

	if  ( HardcopyMode )
		* DlgTemplate. dtClassName = 0 ;


// du nom de la police et de sa taille
	GetStr ( FontInfo. szTypeFace, pdata ) ;
	GetI16 ( FontInfo. PointSize , pdata ) ;

// du titre
	GetStr ( DlgTemplate. dtCaptionText, pdata ) ;

// du style
	GetI32 ( DlgTemplate. dtStyle, pdata ) ;

// Ce flag indique à CreateDialogIndirect qu'une structure de type FONTINFO
// suit la DLGTEMPLATE...
	DlgTemplate. dtStyle  |=  DS_SETFONT ;

// et du nombre de contrôles
	GetI16 ( DlgTemplate. dtItemCount, pdata ) ;

// On vérifie l'origine de la fenêtre
	GetDefaultWindowPosition ( x, y, DlgTemplate. dtCX, DlgTemplate. dtCY ) ;

	if  ( DlgTemplate. dtX  ==  CW_USEDEFAULT )
		DlgTemplate. dtX = x ;

	if  ( DlgTemplate. dtY  ==  CW_USEDEFAULT )
		DlgTemplate. dtY = y ;


// Allocation des DLGITEMTEMPLATEs
	if  ( DlgTemplate. dtItemCount > 0 )
	   {
		ItemTemplates = new DLGITEMTEMPLATE  [ DlgTemplate. dtItemCount ] ;
		DlgDatas      = new TDialogEntryData [ DlgTemplate. dtItemCount ] ;
		memset ( DlgDatas, 0, sizeof ( TDialogEntryData ) * DlgTemplate. dtItemCount ) ;
		

	// Pour chaque contrôle...
		for  ( i = 0, pItem = ItemTemplates ;
				i < DlgTemplate. dtItemCount ;
					i ++, pItem ++ )
		   {
		// Récupération de l'identificateur du contrôle
			GetI16 ( pItem -> dtilID, pdata ) ;

		// On saute les coordonnées en mode texte
			pdata += sizeof ( TDialogDimension ) ;

		// Récupération des coordonnées
			GetI16 ( pItem -> dtilX , pdata ) ;
			GetI16 ( pItem -> dtilY , pdata ) ;
			GetI16 ( pItem -> dtilCX, pdata ) ;
			GetI16 ( pItem -> dtilCY, pdata ) ;

		// Récupération du titre
			GetStr ( pItem -> dtilText, pdata ) ;

		// de la classe
			GetStr ( pItem -> dtilClass, pdata ) ;

		// du style
			GetI32 ( pItem -> dtilStyle, pdata ) ;

			if  ( HardcopyMode )
			   {
			// Masquer les controles BorShade
				if  ( ! stricmp( pItem -> dtilClass, "BorShade" )  &&
					( pItem -> dtilStyle & ( BSS_GROUP | BSS_RGROUP ) ) )
				   {
					pItem -> dtilStyle |=  WS_DISABLED ;
					pItem -> dtilStyle &= ~WS_VISIBLE ;
				     }

			// Transformer les BorStatic en Static
				else if  ( ! stricmp ( pItem -> dtilClass, "BorStatic" ) )
				   {
					strcpy ( pItem -> dtilClass, "STATIC" ) ;

					if  ( pItem -> dtilStyle & BSS_LEFT )
					   {
						pItem -> dtilStyle  &=  ~BSS_LEFT ;
						pItem -> dtilStyle  |=  SS_LEFT ;
					     }

					if  ( pItem -> dtilStyle & BSS_CENTER )
					   {
						pItem -> dtilStyle  &=  ~BSS_CENTER ;
						pItem -> dtilStyle  |=  SS_CENTER ;
					     }

					if  ( pItem -> dtilStyle & BSS_RIGHT )
					   {
						pItem -> dtilStyle  &=  ~BSS_RIGHT ;
						pItem -> dtilStyle  |=  SS_RIGHT ;
					     }
				      }
			    }



		// On ne se préoccupe pas pour le moment des infos
			GetI16 ( unix_data_size  , pdata ) ;
			GetI16 ( remote_data_size, pdata ) ;

			if  ( unix_data_size )
				memcpy ( DlgDatas + i, pdata, sizeof ( DlgDatas [i] ) ) ;

			pdata += unix_data_size ;
			pdata += remote_data_size ;	// On ne gère pas les "RemoteData"

			pItem -> dtilInfo = 0 ;
			pItem -> dtilData = 0 ;

		// La modif suivante est annulée (pb de ressources disponibles
		// suite à la création de controles multiline :
		//**************************************************************
		//  Aligner les numériques à droite
		//  Cela se fait avant la création car je ne sais toujours pas
		//  comment faire prendre en compte à un controle EDIT des
		//  modifications de bits de style après la création du controle.
		/***	if  ( * DlgDatas [i]. Format  ==  'd' )
			   {
				pItem -> dtilStyle  &=  ~ES_LEFT ;
				pItem -> dtilStyle  &=  ~ES_CENTER ;
				pItem -> dtilStyle  |=   ES_RIGHT | ES_MULTILINE ;
			     }
		 ***/
		    }
	    }
	DialogBoxTemplate. DialogTemplate 	=  DlgTemplate ;
	DialogBoxTemplate. FontInfo	   	=  FontInfo ;
	DialogBoxTemplate. DialogItemTemplates  =  ItemTemplates ;
	DialogBoxTemplate. DialogEntryData	=  DlgDatas ;
    }

HANDLE	CreateDialogBoxHandle (	  DIALOGBOX &		DialogBoxTemplate )
   {
	register char *			memp,
		      *			memory_dialog ;
	register char far *		globalp ;
	DLGTEMPLATE			DlgTemplate = DialogBoxTemplate. DialogTemplate ;
	DLGITEMTEMPLATE *		ItemTemplates = DialogBoxTemplate. DialogItemTemplates ,
			*		pItem ;
	FONTINFO			FontInfo = DialogBoxTemplate. FontInfo ;
	HANDLE				handle ;
	register int			i, size, memsize ;

// On ne se préoccupe pas pour le moment des infos à la fin de la
// définition
// et on calcule la taille du bazar.
// Remarque : on utilise sizeof ( DLGTEMPLATE ), etc... la taille calculée
// sera légèrement supérieure à la taille réelle du dialogue car, par ce
// procédé, on inclut aussi les pointeurs sur les chaînes de caractères.
	size  = sizeof ( DLGTEMPLATE ) +
			( sizeof ( DLGITEMTEMPLATE ) * DlgTemplate. dtItemCount ) ;
	size += strlen ( DlgTemplate. dtMenuName ) 	+ 1 +
		strlen ( DlgTemplate. dtClassName )     + 1 +
		strlen ( DlgTemplate. dtCaptionText )	+ 1 ;

	for  ( i = 0, pItem = ItemTemplates ;
			i < DlgTemplate. dtItemCount ;
				i ++, pItem ++ )
		size += strlen ( pItem -> dtilClass ) 	+ 1 +
			strlen ( pItem -> dtilText )	+ 1 ;

	memp    = memory_dialog = new  char [ size ] ;
	memset ( memp, 0, size ) ;
	memsize = size ;


// La place mémoire est allouée pour la définition du dialogue : on copie
// maintenant les données de la boîte.

// Recopie des champs entiers
	size  = sizeof ( DlgTemplate. dtStyle ) 	+
		sizeof ( DlgTemplate. dtItemCount )	+
		sizeof ( DlgTemplate. dtX )		+
		sizeof ( DlgTemplate. dtY )		+
		sizeof ( DlgTemplate. dtCX )		+
		sizeof ( DlgTemplate. dtCY ) ;
	memmove ( memp, & DlgTemplate. dtStyle, size ) ;
	memp += size ;

 // Recopie du nom de menu, de la classe et du titre
	strcpy ( memp, DlgTemplate. dtMenuName ) ;
	memp += strlen ( DlgTemplate. dtMenuName ) + 1 ;

	strcpy ( memp, DlgTemplate. dtClassName ) ;
	memp += strlen ( DlgTemplate. dtClassName ) + 1 ;

	strcpy ( memp, DlgTemplate. dtCaptionText ) ;
	memp += strlen ( DlgTemplate. dtCaptionText ) + 1 ;

// Taille et nom de la police
	memmove ( memp, & FontInfo. PointSize, sizeof ( FontInfo. PointSize ) ) ;
	memp += sizeof ( FontInfo. PointSize ) ;

	strcpy ( memp, FontInfo. szTypeFace ) ;
	memp += strlen ( FontInfo. szTypeFace ) + 1 ;


// Recopie des contrôles
	for  ( i = 0, pItem = ItemTemplates ; i < DlgTemplate. dtItemCount ; i ++, pItem ++ )
	   {

	// Recopie des premiers champs numériques
		size  = sizeof ( pItem -> dtilX )  +
			sizeof ( pItem -> dtilY )  +
			sizeof ( pItem -> dtilCX ) +
			sizeof ( pItem -> dtilCY ) +
			sizeof ( pItem -> dtilID ) +
			sizeof ( pItem -> dtilStyle ) ;

		memmove ( memp, & pItem -> dtilX, size ) ;
		memp += size ;

	// Recopie de la classe et du texte
		strcpy ( memp, pItem -> dtilClass ) ;
		memp += strlen ( pItem -> dtilClass ) + 1 ;

		strcpy ( memp, pItem -> dtilText ) ;
		memp += strlen ( pItem -> dtilText ) + 1 ;

	// Recopie des éventuelles données propres à ce contrôle
		memmove ( memp, & pItem -> dtilInfo, sizeof ( pItem -> dtilInfo ) ) ;
		memp += sizeof ( pItem -> dtilInfo ) ;

		if  ( pItem -> dtilInfo )
		   {
			memmove ( memp, pItem -> dtilData, pItem -> dtilInfo ) ;
			memp += pItem -> dtilInfo ;
		    }
	}



// Allocation d'un bloc de mémoire globale pour y placer la définition de
// la boîte
	handle = GlobalAlloc ( GMEM_FIXED, memsize ) ;
	globalp      = ( char far * ) GlobalLock  ( handle ) ;
	movedata ( FP_SEG ( memory_dialog ), FP_OFF ( memory_dialog ),
		   FP_SEG ( globalp       ), FP_OFF ( globalp ),
			memsize ) ;
	GlobalUnlock ( handle ) ;


// Effacement de tous les objects alloués
	delete []  memory_dialog ;


	return ( handle ) ;
    }



void	FreeDialogBoxTemplate ( DIALOGBOX &   dlgbox )
   {
	register int			i ;
	register DLGITEMTEMPLATE *	ItemTemplates	=  dlgbox. DialogItemTemplates,
				 * 	pItem ;


	if  ( dlgbox. DialogTemplate. dtMenuName )
		delete []  dlgbox. DialogTemplate. dtMenuName ;

	if  ( dlgbox. DialogTemplate. dtClassName )
		delete []  dlgbox. DialogTemplate. dtClassName ;

	if  ( dlgbox. DialogTemplate. dtCaptionText )
		delete []  dlgbox. DialogTemplate. dtCaptionText ;

	if  ( dlgbox. FontInfo. szTypeFace )
		delete []  dlgbox. FontInfo. szTypeFace ;

	if  ( dlgbox. DialogEntryData )
		delete []  dlgbox. DialogEntryData ;
	
	for  ( i = 0, pItem = ItemTemplates ; 
			i < dlgbox. DialogTemplate. dtItemCount ;
				i ++, pItem ++ )
	   {
		if  ( pItem -> dtilText )
			delete []  pItem -> dtilText ;

		if  ( pItem -> dtilClass )
			delete []  pItem -> dtilClass ;
	    }

	if  ( ItemTemplates )
		delete []  ItemTemplates ;
    }



/**************************************************************************/
/*                                                                        */
/*   CreateIconFromTemplate :                                             */
/*      Crée un HICON à partir du contenu d'un fichier .ICO.              */
/*   Remarque :                                                           */
/*      Le contenu du fichier .ICO fourni par Unix est réécrit sur un     */
/*      fichier disque, puis la fonction Windows ExtractIcon est appelée  */
/*      pour extraire cette icône du fichier.                             */
/*      Initialement, la création de l'icône aurait du se faire par la    */
/*      fonction CreateIcon, qui aurait évité cette étape d'écriture sur  */
/*      le disque. Malheureusement, documentation absconse et manque      */
/*      d'exemples font que l'icône résultante n'est pas tout à fait      */
/*      conforme à l'original.                                            */
/*      Le passage en commentaire pourra être décommenté lorsque je       */
/*      saurai pourquoi ça ne marche pas.                                 */
/*                                                                        */
/**************************************************************************/

typedef struct
   {
		WORD		Reserved ;
		WORD		ResourceType ;
		WORD		ResourceCount ;
    }   ICONHEADER ;


typedef struct
   {
		BYTE		Width,
				Height ;
		BYTE		ColorCount ;
		BYTE		Reserved1 ;
		WORD		Reserved2,
				Reserved3 ;
		DWORD		Size ;
		DWORD		Offset ;
     }  ICONENTRY ;


# pragma 	argsused

HICON	CreateIconFromTemplate  ( char far *  def, int  size )
   {

/***  PASSAGE EN COMMENTAIRE : CREATION DE L'ICONE A PARTIR DE LA CHAINE EN
      MEMOIRE (DEF).

	char far *	entry		=  def + sizeof ( ICONHEADER ) ;
	int		Width,
			Height,
			ColorCount,
			Planes ;
	long int	Offset,
			Size ;
	char far *	XORBits,
		 *	ANDBits ;


// Récupération des paramètres
	Width 		=  ( ( ICONENTRY * ) entry ) -> Width ;
	Height 		=  ( ( ICONENTRY * ) entry ) -> Height ;
	ColorCount	=  ( ( ICONENTRY * ) entry ) -> ColorCount ;
	Size		=  ( ( ICONENTRY * ) entry ) -> Size ;
	Offset		=  ( ( ICONENTRY * ) entry ) -> Offset ;

	switch ( ColorCount )
	   {
		case	2 : Planes = 1 ; break ;
		case    4 : Planes = 2 ; break ;
		case    8 : Planes = 3 ; break ;
		case   16 : Planes = 4 ; break ;
	    }


// Adresse des bits XOR
	Size		=  ( Width * Height ) / ( 8 / Planes ) ;
	XORBits		=  def + ( int ) Offset + 0x7E ;
	ANDBits		=  def + ( int ) Offset + 0x7E + ( int ) Size ;


// On crée l'icône
	return ( CreateIcon ( * GetApplicationObject ( ),
			Width, Height, 1, Planes,
				ANDBits, XORBits ) ) ;

  FIN DU PASSAGE COMMENTE ***/

	char			tempname [ MAX_PARAMETER_LENGTH ] ;
	register FILE *		fp ;
	register HICON		HIcon ;

	tmpnam ( tempname ) ;
	fp = fopen ( tempname, "w" ) ;
	fwrite ( def, 1, size, fp ) ;
	fclose ( fp ) ;

	HIcon = ExtractIcon ( * GetApplicationObject ( ), tempname, 0 ) ;
	unlink ( tempname ) ;

	return ( HIcon ) ;
    }




/**************************************************************************/
/*                                                                        */
/*   GetDefaultWindowPosition :                                           */
/*      Retourne dans (x,y) une position par défaut pour une fenêtre      */
/*      (permet de simuler l'action qu'effectue Windows lorsqu'on crée    */
/*      une fenêtre avec des coordonnées CW_USEDEFAULT).                  */
/*                                                                        */
/**************************************************************************/

void	GetDefaultWindowPosition  ( int &   x,
				    int &   y,
				    int     w,
				    int     h )
   {
	static int		ScreenWidth,
				ScreenHeight,
				XOffset,
				YOffset,
				NewX,
				NewY ;
	static BOOL		FirstTime	=  TRUE ;



	if  ( FirstTime )
	   {
		FirstTime 	=  FALSE ;
		ScreenWidth	=  GetSystemMetrics ( SM_CXSCREEN ) ;
		ScreenHeight 	=  GetSystemMetrics ( SM_CYSCREEN ) ;
		XOffset		=
		YOffset		=  ( ( GetSystemMetrics ( SM_CYCAPTION ) +
				       GetSystemMetrics ( SM_CYBORDER  ) )
					  * 3 ) / 2 ;
		NewX		=
		NewY		=  0 ;
	     }


	NewX  +=  XOffset ;
	NewY  +=  YOffset ;

	if  ( NewX + w  >  ScreenWidth )
		NewX = XOffset ;

	if  ( NewY + h  >  ScreenHeight )
		NewY = YOffset ;

	x = NewX ;
	y = NewY ;
   }




/**************************************************************************/
/*                                                                        */
/*  GetObjectOrigin :                                                     */
/*      Calcule l'origine d'un bouton minimize ou maximize.               */
/*                                                                        */
/**************************************************************************/

void	GetObjectOrigin ( TDialog *	Dialog,
			  long int  	style,
			  POINT &   	Origin,
			  POINT &   	Size )
   {
	TRect		WindowRect,
			ClientRect ;
	TPoint		ClientOrigin ;
	int		DSModalFrameW,
			DSModalFrameH ;

	Dialog -> GetWindowRect  ( WindowRect ) ;
	Dialog -> GetClientRect  ( ClientRect ) ;
	ClientOrigin. x = ClientRect. left ;
	ClientOrigin. y = ClientRect. top ;

	Dialog -> ClientToScreen ( ClientOrigin ) ;

	DSModalFrameW = ClientOrigin. x - WindowRect. left ;
	DSModalFrameH = ClientOrigin. y - GetSystemMetrics ( SM_CYCAPTION )
				      - WindowRect. top ;


	switch ( style )
	   {
		case	WS_MINIMIZEBOX :
			Origin. y = DSModalFrameH + 1 ;
			Origin. x = WindowRect. right - WindowRect. left
						      - DSModalFrameW
						      - Size. x - 1 ;
			return ;

		default :
			Origin. x = Origin. y = 0 ;
			return ;
	     }
     }




/**************************************************************************/
/*                                                                        */
/*  DrawMinimizeBox :                                                     */
/*      Redessine une boîte Minimize pour une fenêtre ayant le style      */
/*      DS_MODALFRAME.                                                    */
/*                                                                        */
/**************************************************************************/


void	DrawMinimizeBox ( TDialog *  Dialog, BOOL  pressed )
   {
	int		BitmapId ;


// Ne dessiner le bouton que si son état a changé
	if  ( pressed )
		BitmapId   = OBM_REDUCED ;
	else
		BitmapId   = OBM_REDUCE ;


// Charger la bitmap prédéfinie
	HDC		WindowDC,
			MemDC ;
	HBITMAP		HBitmap ;
	POINT		PObjectPlacement,
			PSize,
			POrigin ;
	BITMAP		BitmapInfo ;


	WindowDC = GetWindowDC ( Dialog -> HWindow ) ;
	MemDC    = CreateCompatibleDC ( WindowDC ) ;
	HBitmap  = LoadBitmap ( NULL, ( LPSTR ) BitmapId ) ;

	if  ( HBitmap  ==  NULL )
	   {
		DeleteDC  ( MemDC ) ;
		ReleaseDC ( Dialog -> HWindow, WindowDC ) ;
		return ;
	    }

	SelectObject ( MemDC, HBitmap ) ;
	SetMapMode   ( MemDC, GetMapMode ( WindowDC ) ) ;

	GetObject ( HBitmap, sizeof ( BITMAP ), ( LPSTR ) & BitmapInfo ) ;

	PSize. x  =  BitmapInfo. bmWidth ;
	PSize. y  =  BitmapInfo. bmHeight ;
	DPtoLP ( WindowDC, & PSize, 1 ) ;

	POrigin. x = POrigin. y = 0 ;
	DPtoLP ( MemDC, & POrigin, 1 ) ;

	GetObjectOrigin ( Dialog, WS_MINIMIZEBOX, PObjectPlacement, PSize ) ;

	BitBlt ( WindowDC, PObjectPlacement. x, PObjectPlacement. y,
			PSize. x, PSize. y,
		 MemDC, POrigin. x, POrigin. y, SRCCOPY ) ;

// Ménage
	DeleteDC      ( MemDC ) ;
	DeleteObject  ( HBitmap ) ;
	ReleaseDC     ( Dialog -> HWindow, WindowDC ) ;
    }






/**************************************************************************/
/*									  */
/*    DelayMin : 							  */
/*  	Fonction permettant un delay d'au moins n millisecondes.	  */
/*                                                                        */
/**************************************************************************/

void		DelayMin	( DWORD Delay )
	{
	DWORD		Timer_Begin = GetTickCount ( ) ;
	DWORD		Timer_End ;

	do
		Timer_End = GetTickCount ( ) ;
	while ( Timer_End - Timer_Begin < Delay ) ;
	}




/**************************************************************************/
/*                                                                        */
/*  MessageBox :                                                          */
/*      Affiche une boîte de message, en permettant d'utiliser des        */
/*      paramètres variables comme dans printf().                         */
/*                                                                        */
/**************************************************************************/

int	MessageBox  ( HWND	parent,
		      char *	title,
		      UINT	flags,
		      char *	format,
		      ... )
    {
	char			buffer [ 512 ] ;
	va_list			args ;
	int			status ;


	va_start ( args, format ) ;
	wvsprintf ( buffer, format, args ) ;
	va_end ( args ) ;

	status = :: MessageBox ( parent, buffer, title, flags ) ;

	return ( status ) ;
     }






/**************************************************************************/
/*                                                                        */
/*  Beautify -                                                            */
/*      Embellit du texte en mettant la première lettre en majuscule, le  */
/*      reste en minuscules.                                              */
/*                                                                        */
/**************************************************************************/

void	Beautify  ( char *  text )
   {
	register char *		p	=  text,
		      *		q	=  text ;
	register int		c ;


	while  ( isspace ( * p ) ) 
		p ++ ;
	* q ++ = toupper ( * p ) ;

	if  ( * p ) 
		p ++ ;
	c = 0 ;

	while  ( * p )
	   {
		if  ( isspace ( * p ) )
		   {
			if  ( ! isspace ( c ) )
				* q ++ = * p ;
		    }
		else
			* q ++ = tolower ( * p ) ;

		c = * p ;
		p ++ ;
	     }

	* q = 0 ;
    }






/**************************************************************************/
/*                                                                        */
/*  SetWindowVisible -                                                    */
/*      Enlève ou rajoute le style WS_VISIBLE à la fenêtre selon la       */
/*      valeur de 'visible'.                                              */
/*                                                                        */
/**************************************************************************/

void	SetWindowVisible  ( TWindow *  win, BOOL  visible ) 
   {
	register DWORD		Style  =  win -> GetWindowLong ( GWL_STYLE ) ;


	if  ( visible )
		Style  |=  WS_VISIBLE ;
	else
		Style  &=  ~( WS_VISIBLE ) ;

	win -> SetWindowLong ( GWL_STYLE, Style ) ;
	win -> Invalidate ( ) ;
	win -> UpdateWindow ( ) ;
     }




/**************************************************************************/
/*                                                                        */
/*  IsFocusMessage -                                                      */
/*      Détermine si le message spécifié est un message SET/KILLFOCUS,    */
/*      que ce soit WM_xxx ou un message de notification d'un controle.   */
/*                                                                        */
/**************************************************************************/

BOOL	IsFocusMessage  ( UINT   Message, WPARAM  /* WParam */, LPARAM  LParam )
   {
	if  ( Message  ==  WM_SETFOCUS  ||
	      Message  ==  WM_KILLFOCUS )
	      return ( TRUE ) ;

	if  ( Message  !=  WM_COMMAND )
		return  ( FALSE ) ;

// Vérifier s'il ne s'agit pas d'un message de notification
	char		ClassName [ 256 ] ;
	HWND		hwnd		=  ( HWND ) LOWORD ( LParam ) ;
	UINT		NotifyCode      =  HIWORD ( LParam ) ;


	GetClassName ( hwnd, ClassName, sizeof ( ClassName ) ) ;

	if  ( ! stricmp ( ClassName, "EDIT" )  ||
	      ! stricmp ( ClassName, "LZCColor" )  ||
	      ! stricmp ( ClassName, "GROUPBOX" )  ||
	      ( ! stricmp ( ClassName, "BorShade" )  &&
	      	( :: GetWindowLong ( hwnd, GWL_STYLE ) & WS_GROUP ) ) )
	   {
		if  ( NotifyCode  ==  EN_SETFOCUS  ||  NotifyCode  ==  EN_KILLFOCUS )
		      return ( TRUE ) ;
	     }
	else if  ( ! stricmp ( ClassName, "COMBOBOX" ) )
	   {
		if  ( NotifyCode  ==  CBN_SETFOCUS  ||  NotifyCode  ==  CBN_KILLFOCUS )
		      return ( TRUE ) ;
	     }
	else if  ( ! stricmp ( ClassName, "LISTBOX" ) )
	   {
		if  ( NotifyCode  ==  LBN_SETFOCUS  ||  NotifyCode  ==  LBN_KILLFOCUS )
		      return ( TRUE ) ;
	     }
	else if  ( ! stricmp ( ClassName, "BUTTON" )  ||
		   ! stricmp ( ClassName, "BorBtn" )  ||
		   ! stricmp ( ClassName, "CHECKBOX" )  ||
		   ! stricmp ( ClassName, "BorCheck" )  ||
		   ! stricmp ( ClassName, "RADIOBUTTON" ) ||
		   ! stricmp ( ClassName, "BorRadio" ) )
	   {
		if  ( NotifyCode  ==  BN_KILLFOCUS  ||  NotifyCode  ==  BN_SETFOCUS )
			return ( TRUE ) ;
	     }

	return ( FALSE ) ;
    }





/**************************************************************************/
/*                                                                        */
/*  RestoreFocusToChild -                                                 */
/*      Redonne le focus à une fenerte fille de la classe TCheopsDialog   */
/*      ou TEditWindow. Le paramètre 'avoid_this_one' est le handle       */
/*      d'une fenetre à laquelle il ne faut pas redonner le focus.        */
/*      Cette fonction est appelée par les destructeurs des objets        */
/*      TCheopsDialog et TEditWindow afin que le menu graphique redonne   */
/*      le focus à une éventuelle fenetre du meme type après fermeture.   */
/*      Retourne TRUE si une fenetre a été trouvée.                       */
/*                                                                        */
/**************************************************************************/

static HWND	FoundChild ;


static BOOL  FAR PASCAL  EnumProc  ( HWND  hwnd, LPARAM  avoid_this_one )
   {
	char		Buffer [ 256 ] ;


	if  ( ( HWND ) avoid_this_one  !=  hwnd  &&
	      IsWindowVisible ( hwnd )           &&
	      ! IsIconic ( hwnd ) )
	   {
		GetClassName ( hwnd, Buffer, sizeof ( Buffer ) ) ;

		if  ( ! stricmp ( Buffer, "TCheopsDialog" )  ||
		      ! stricmp ( Buffer, "TEditWindow" ) )
		   {
			FoundChild = hwnd ;
			return ( FALSE ) ;
		    }
	      }

	return ( TRUE ) ;
     }



BOOL	RestoreFocusToChild  ( HWND  parent, HWND  avoid_this_one )
   {
	FoundChild = 0 ;
	EnumTaskWindows ( GetWindowTask ( parent ), EnumProc, ( LPARAM ) avoid_this_one ) ;

	if  ( FoundChild )
	   {
		SetActiveWindow ( FoundChild ) ;
		return ( TRUE ) ;
	     }
	else
		return ( FALSE ) ;
     }
	



/**************************************************************************/
/*                                                                        */
/*  EnableTextItem -                                                      */
/*      Fonction appelée pour chaque controle à rendre disabled.          */
/*      Dans le cas des controles statiques, cette fonction ne fait rien. */
/*      Dans le cas des controles EDIT, au lieu de les mettre disabled    */
/*      (ce qui provoque l'affichage du texte en gris), elle positionne   */
/*      le style READONLY du controle.                                    */
/*                                                                        */
/**************************************************************************/

void	EnableTextItem  ( HWND  control_hwnd, BOOL  state )
   {
	char		ClassName [ MAX_PARAMETER_LENGTH ] ;
	BOOL		IsCombo ;


	GetClassName ( control_hwnd, ClassName, sizeof ( ClassName ) ) ;


	if  ( IsWindowEnabled ( GetParent ( GetParent ( GetParent ( control_hwnd ) ) ) ) )
	   {
		if  ( ! stricmp ( ClassName, "STATIC" )  ||  ! stricmp ( ClassName, "BorStatic" ) ) 
			return ;
		else if  ( ! stricmp ( ClassName, "EDIT" )  ||
			   ! stricmp ( ClassName, "CHECKBOX" )  ||  
			   ! stricmp ( ClassName, "RADIOBUTTON" )  ||  
			   ! stricmp ( ClassName, "BUTTON" )  ||  
			   ! stricmp ( ClassName, "BorBtn" )  ||  
			   ! stricmp ( ClassName, "BorCheck" )  ||  
			   ! stricmp ( ClassName, "BorRadio" )  ||  
			   ( IsCombo =  stricmp ( ClassName, "COMBOBOX" )  ==  0 )  !=  0 )
		   {
			DWORD	Style = GetWindowLong ( control_hwnd, GWL_STYLE ) ;


			if  ( IsCombo )
			   {
			// C'est pas portable : c'est l'id du controle EDIT de la combo
				HWND  	edit_hwnd = GetDlgItem ( control_hwnd, 1001 ) ;
				DWORD   Style ;

				if  ( edit_hwnd )
				   {
					Style = GetWindowLong ( edit_hwnd, GWL_STYLE ) ;

					if  ( state )
						Style  &=  ~WS_DISABLED ;
					else
						Style  |=  WS_DISABLED ;

					SendMessage ( edit_hwnd, EM_SETREADONLY, ! state, 0L ) ;
				     }	
			     }

			if  ( state )
				Style &= ~WS_DISABLED ;
			else
				Style |= WS_DISABLED ;

			SetWindowLong ( control_hwnd, GWL_STYLE, Style ) ;
			SendMessage ( control_hwnd, EM_SETREADONLY, ! state, 0L ) ;
			return ;
		     }
	     }

// Autres cas
	EnableWindow ( control_hwnd, state ) ;
     }
			

