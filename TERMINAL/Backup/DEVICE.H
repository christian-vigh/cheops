/**************************************************************************/
/*                                                                        */
/*  Device.h  -  Christian Vigh, 94/09/13.                                */
/*               (d'après Sébastien George et Christophe Després)         */
/*                                                                        */
/*  La classe TDisplayDevice est responsable de l'affichage de tout ce    */
/*  qui concerne la partie émulateur normal. Sa seconde tâche est aussi   */
/*  d'interpréter les séquences d'échappement, après avoir chargé une     */
/*  définition de terminal à partir d'un fichier .TRM.                    */
/*                                                                        */
/**************************************************************************/



# define	MAX_FUNCTION_KEYS	12


/**************************************************************************/
/*                                                                        */
/*  Lorsqu'une touche est frappée, la fonction ProcessKey est appelée     */
/*  pour déterminer si cette touche fait partie des touches émulées.      */
/*  Le code de la touche est passé en paramètre à cette fonction, ainsi   */
/*  qu'un paramètre Flags, constitué par un ou logique entre un ou        */
/*  plusieurs de ces drapeaux :                                           */
/*                                                                        */
/*  SHIFT       - La touche SHIFT en même temps que l'autre.              */
/*  CONTROL     - Idem, pour CONTROL                                      */
/*  ALT         - Idem, pour ALT                                          */
/*  REGULARCHAR - La fonction ProcessKey n'a pas été appelée suite à un   */
/*                message WM_KEYDOWN ou WM_SYSKEYDOWN, mais suite à un    */
/*                message WM_CHAR.                                        */
/*                                                                        */
/**************************************************************************/

# define	SHIFT			0x01	
# define	CONTROL			0x02
# define	ALT			0x04
# define	REGULARCHAR		0x08


/**************************************************************************/
/*                                                                        */
/*  Attributs de caractère.                                               */
/*                                                                        */
/**************************************************************************/

# define	UNDERLINE		0x0001
# define	BOLD			0x0002
# define	ITALIC			0x0004
# define	SPECIAL			0x0010



/**************************************************************************/
/*                                                                        */
/*  Dans le fichier .TRM, les séquences d'échappement peuvent contenir    */
/*  des parties variables; ces parties sont exprimées avec un caractère   */
/*  '%' suivi d'une lettre désignant leur type. De manière interne, pour  */
/*  faciliter l'analyse des séquences envoyées par l'hôte, ces deux       */
/*  caractères ('%quelquechose' sont remplacés par :                      */
/*                                                                        */
/*  Paramètre   Remplacé par            Signification                     */
/*     %n       ANY_ASCII_NUMBER        Nombre exprimé sous forme ascii   */
/*     %s       ANY_STRING              Chaîne de caractères quelconque   */
/*     %l       ANY_STRING_LENGTH       Longueur de la chaîne suivante    */
/*                                      (ANY_STRING) dans la séquence.    */
/*                                                                        */
/**************************************************************************/


# define	ANY_ASCII_NUMBER	1	// Séquence '%n'
# define	ANY_STRING		2	// Séquence '%s' 
# define	ANY_STRING_LENGTH	3	// Séquence '%l'

# define	UNUSED_COLOR		0xFFFFFFFFL
# define	MAX_LABEL_LENGTH	12


/**************************************************************************/
/*                                                                        */
/*  Unix a besoin de savoir quelle suite de caractères est renvoyée par   */
/*  les touches F1, F2, ..., Ins, Suppr, etc...                           */
/*  La structure TEmulatedKey définit :                                   */
/*                                                                        */
/*  1-Quelle combinaison de touches (sous Windows) fait partie de celles  */
/*    reconnues par Unix                                                  */
/*  2-Ce qu'il faut envoyer à Unix lorsque l'une de ces combinaisons      */
/*    arrive.                                                             */
/*                                                                        */
/*  Tout celà est défini dans un fichier d'émulation (ANSI.TRM, par       */
/*  exemple). Ce fichier, identique à un .INI, si ce n'est son extension, */
/*  contient plusieurs sections. Celle qui nous intéresse ici s'appelle   */
/*  [Keys]. Chaque ligne de cette section est de la forme :               */
/*                                                                        */
/*              keyname = value                                           */
/*                                                                        */
/*  où 'keyname' est un nom de touche de fonction bien défini, et 'value' */
/*  une suite de caractères qui sera envoyée à Unix par appui sur la      */
/*  touche.                                                               */
/*                                                                        */
/*  Les champs de la structure sont les suivants :                        */
/*                                                                        */
/*                                                                        */
/*  KeyName :                                                             */
/*      Nom de la touche de fonction (hardcodé...)                        */
/*                                                                        */
/*  KeyCode :                                                             */
/*      Code à renvoyer (provient du .TRM)                                */
/*                                                                        */
/*  VirtualKeyCode :                                                      */
/*      Code virtuel de cette touche pour Windows (hardcode). Par exemple,*/
/*      VK_F1, VK_INS, etc.                                               */
/*                                                                        */
/*  VirtualKeyFlags :                                                     */
/*      Ce champ peut être une combinaison des 3 constantes suivantes :   */
/*      ALT, CONTROL et SHIFT. La touche ne sera reconnue par l'émulateur */
/*      que si la touche spéciale correspondante (ALT, CONTROL et SHIFT)  */
/*      a été enfoncée.                                                   */
/*                                                                        */
/**************************************************************************/

typedef struct
   {
		char *		KeyName ;
		char *		KeyCode ;
		UINT		VirtualKeyCode ;
		UINT		VirtualKeyFlags ;
     }  TEmulatedKey ;





/**************************************************************************/
/*                                                                        */
/*  La structure TEmulatorSequence définit quelles séquences d'échappement*/
/*  (envoyées par la machine distante) doivent être interprétées par      */
/*  l'émulateur. Les champs sont les suivants :                           */
/*                                                                        */
/*  SequenceName :                                                        */
/*      Nom de la séquence d'échappement (fait partie d'une liste bien    */
/*      définie).                                                         */
/*                                                                        */
/*  Sequence :                                                            */
/*      Caractères composant la séquence d'échappement. Elle peut contenir*/
/*      des caractères spéciaux tels que :                                */
/*                                                                        */
/*      \E                      -> traduit par Escape                     */
/*      \t, \n, \r, etc...      -> séquences identiques à celles du       */
/*                                 langage C.                             */
/*      %n                     -> Indique qu'à cet emplacement de la     */
/*                                 séquence, il peut y avoir n'importe    */
/*                                 quelle suite de caractères représentant*/
/*                                 un nombre compris entre 0 et 255.      */
/*      %%                      -> Le caractère '%'                       */
/*                                                                        */
/*  Action :                                                              */
/*      Action associée à cette séquence d'échappement (du type           */
/*      TEmulatorAction, défini ci-dessous).                              */
/*                                                                        */
/*  Les définitions sont lues dans la section '[Escapes]' du fichier .TRM */
/*                                                                        */
/*  Remarque : Unix désire connaître la séquence envoyée par les touches  */
/*  Tabulation et Newline. Carriage return, par contre, ne l'intéresse    */
/*  pas. Cependant, par souci d'homogénéité, carriage return a été inclus */
/*  dans cette liste.                                                     */
/*                                                                        */
/**************************************************************************/

typedef enum
   {
		CarriageReturnAction,	// Retour en début de ligne
		BackspaceAction,	// Effacer le caractère à gauche du curseur
		BellAction,		// Faire Bip
		EnterBlinkAction,	// Faire du texte clignotant
		EnterBoldAction,	// ... ou gras
		BackTabAction,		// Shift-Tab
		ClearAction,		// Effacer l'écran
		NewlineAction,		// Descendre d'une ligne
		CursorLeftAction,	// Mouvements du curseur
		CursorRightAction,
		CursorUpAction,
		CursorDownAction,
		GotoXYAction,		// Positionner le curseur en (x,y)
		DeleteCharacterAction,	// Effacer le caractère sous le curseur
		DeleteLineAction,	// Effacer la ligne courante
		ClearToEODAction,	// Effacer de la position courante à la fin de l'écran
		ClearToEOLAction,	// Effacer de la position courante à la fin de la ligne
		CursorHomeAction,	// Mettre le curseur en (0,0)
		TabAction,		// Tabulation
		InsertCharacterAction,	// Insérer un blanc à la position courante
		InsertLineAction,	// ... ou une ligne
		ScrollUpAction,		// Défiler d'une ligne vers le haut
		EnterReverseAction,	// Passer en surbrillance
		ScrollDownAction,	// Défiler d'une ligne vers le bas
		StartUnderlineAction,	// Passer en mode souligné
		StopUnderlineAction,	// Arrêter le souligné (et la surbrillance...)
		AttributesOffAction,	// Repasser en texte normal
		InvisibleModeAction,	// Les caractères à afficher sont invisibles
		StartStandoutAction,	// Surbrillance
		StopStandoutAction,
		StartACSAction,		// Alternate character set
		StopACSAction,
		SetBackgroundAction,	// Définir la couleur de fond
		SetForegroundAction,	// et de texte
		SetColorAction,
		SetFKStringAction,	// Programmer une touche de fonction
		SetFKLabelAction,	// Définir le label d'une touche de fonction
		EnableFKAction,		// Afficher les touches de fonction
		DisableFKAction,	// Masquer les touches de fonction

		WriteCharacterAction,	// Non utilisé (éventuellement, peut
					// correspondre à un caratcère quelconque)
		MessageHeadAction,	// Début de message
		MessageTailAction,	// Fin de message

		BulkPrintBeginAction,	// Début et fin d'impression d'un fichier entier
		BulkPrintEndAction,	

		FramePrintBeginAction,	// Début et fin d'impression d'un bloc de fichier
		FramePrintEndAction,	

		CancelPrintAction,	// Annulation d'une impression "bulk"

		OpenFileAction,  	// Transfert de fichier
		CloseFileAction,	// Fin de transfert
		WriteFileAction,	// Ecrire un bloc
		MessageAction,		// Afficher un message
		ExecuteAction,		// Exécuter une application
		TypeAction,		// Retourner le type de connexion
		UserNameAction,		// Pour l'autologin
		UserPasswordAction,

		ZeroedAction,		// Actions à ne faire qu'une fois
		UndefinedAction		// Action non définie
     }  TEmulatorAction ;
		
		

typedef struct
   {
		char *			SequenceName ;
		char *			Sequence ;
		TEmulatorAction         Action ;
    }  TEmulatorSequence ;





/**************************************************************************/
/*                                                                        */
/*   La structure TProgrammedKey sert à gérer les touches de fonction     */
/*   devant émettre une chaîne de caractères lorsqu'elles sont pressées.  */
/*                                                                        */
/**************************************************************************/

typedef struct
   {
		UINT		VirtualKeyCode ;
		char		StringToSend [ MAX_PARAMETER_LENGTH ] ;
    }  TProgrammedKey ;



/**************************************************************************/
/*                                                                        */
/*  Unix (comme l'émulateur ANSI) peut gérer des caractères spéciaux      */
/*  (en envoyant une séquence d'échappement indiquant que les caractères  */
/*  suivants sont spéciaux, puis les caractères eux-même).                */
/*  Afin de pouvoir les gérer par une table externe (définie dans le      */
/*  fichier xxx.TRM), on a besoin de la structure suivante :              */
/*                                                                        */
/**************************************************************************/

typedef struct
   {
	char *		CharacterName ;         // Mot-clé dans le .TRM
	char		HostValue,		// Caractère envoyé par Unix
			LocalValue ;		// Sa traduction chez nous
    }  TSpecialCharacter ;





/**************************************************************************/
/*                                                                        */
/*   La classe TFunctionKeypad permet de gérer les pavés de touche de     */
/*   fonction.                                                            */
/*   Note : EV_COMMAND est utilisé ici pour la table de réponses, car     */
/*   EV_COMMAND_AND_ID semble transmettre un paramètre toujours égal à 0. */
/*                                                                        */
/**************************************************************************/

# define	FunctionKeyHandler(x)	void  F##x ( void ) { Command ( x - 1 ) ; }



typedef struct
   {
		char		Label  		[ MAX_LABEL_LENGTH + 1 ] ;
		UINT		VirtualKeyCode ;
		TButton *	Button ;
     }  TFunctionKey ;


class	TFunctionKeypad		:  public  TWindow
   {
	private :
		int			Width ;
		BOOL			Disabled ;
		TDisplayDevice *	Device ;
		TFunctionKey *		FunctionKeys ;


	public :
		TFunctionKeypad  ( TDisplayDevice *	device ) ;
	       ~TFunctionKeypad  ( ) ;

		void		PaintRectangle	( void ) ;				  
		virtual void	SetupWindow	( void ) ;
		void		Enable	  	( void ) ;
		void		Disable  	( void ) ;
		void		SetText  	( int		key,
						  char *	label ) ;
		void		Command  	( WPARAM	id ) ;
		void		ResizeWindow	( void ) ;

		int		GetWidth  ( void )
		   {
			return ( ( Disabled ) ? 0 : Width ) ;
		    }


		BOOL		IsActive  ( void )
		   {
			return ( ! Disabled ) ;
		    }


		void		Paint		( TDC &		dc, 
						  BOOL		erase, 
						  TRect & 	rect )
		   {
			TWindow :: Paint ( dc, erase, rect ) ;
			PaintRectangle ( ) ;
		    }

		    
		FunctionKeyHandler (  1 )		    
		FunctionKeyHandler (  2 )		    
		FunctionKeyHandler (  3 )		    
		FunctionKeyHandler (  4 )		    
		FunctionKeyHandler (  5 )		    
		FunctionKeyHandler (  6 )		    
		FunctionKeyHandler (  7 )		    
		FunctionKeyHandler (  8 )		    
		FunctionKeyHandler (  9 )
		FunctionKeyHandler ( 10 )		    
		FunctionKeyHandler ( 11 )		    
		FunctionKeyHandler ( 12 )


	DECLARE_RESPONSE_TABLE  ( TFunctionKeypad ) ;
      } ;




/**************************************************************************/
/*                                                                        */
/*  Classe  TRedoList :                                                   */
/*      Gère les commandes saisies par l'utilisateur.                     */
/*                                                                        */
/**************************************************************************/

# define	MaxLines		64
# define	LINE_SIZE		256



class	TRedoList
   {
	private :
		char 		Lines [ MaxLines ] [ LINE_SIZE ] ;
		int		Origin, LineCount ;
		int		MinRedoSize ;


	public :
		TRedoList	( ) ;
	       ~TRedoList       ( ) ;


		TRedoList &  operator  +=  ( char	c ) ;
		TRedoList &  operator  +=  ( char *	s ) ;
		char *       operator  []  ( int	index ) ;

		int	     Size  ( void )
		   {
			return ( LineCount ) ;
		    }

		void	     SetMinSize  ( int  size )
		   {
			MinRedoSize = size ;
		    }
     } ;



/**************************************************************************/
/*                                                                        */
/*  Classe  TDisplayDevice :                                              */
/*      Elle est responsable de l'interprétation des séquences de         */
/*      caractères envoyées par Unix, ainsi que de l'envoi des séquences  */
/*      correspondant aux touches reconnues par Unix.                     */
/*      Son rôle principal est cependant la gestion de l'affichage à      */
/*      l'écran. Pour ce faire, un tableau de TScreenCharacter, à deux    */
/*      dimensions, est alloué dynamiquement. La 1e dimension correspond  */
/*      aux nombre de lignes de l'écran, et la seconde au nombre de       */
/*      colonnes (ces deux paramètres sont définis à la section [Settings]*/
/*      du fichier .TRM, mots-clé 'Lines' et 'Columns').                  */
/*                                                                        */
/*      De manière à pouvoir gérer les attributs tels que : souligné,     */
/*      surbrillance, etc..., les éléments de ce tableau (les caractères  */
/*      actuellement affichés à l'écran) contiennent aussi les attributs  */
/*      des caractères.                                                   */
/*                                                                        */
/**************************************************************************/

extern  TEmulatedKey 		__EmulatedKeys      [ ] ;
extern  TEmulatorSequence	__EmulatorSequences [ ] ;
extern  COLORREF		__EmulatorColors    [ ] ;
extern  TSpecialCharacter	__SpecialCharacters [ ] ;
extern  TProgrammedKey		__ProgrammedKeys    [ ] ;


class	THardcopy ;
class	TPrintingDialog ;


class	TDisplayDevice
   {
	public :

	// Attributs d'un caractère
		typedef unsigned int	 TCharacterAttributes ;


	// Un caractère de l'écran
		typedef struct
		   {
			char			Character ; // le caractère lui-même
			TCharacterAttributes    Attributes ;
			COLORREF		BackgroundColor,
						ForegroundColor ;
		    }  TScreenCharacter ;


	// Valeur renvoyée par la fonction membre FindMatch
		typedef enum
		    {
				NoMatch,
				PartialMatch,
				FullMatch
		     }  TSequenceMatch ;


	private :
	//  Tableaux permettant à l'émulateur d'émuler...
		TEmulatedKey *		EmulatedKeys ;
		TEmulatorSequence *	EmulatorSequences ;
		COLORREF *		EmulatorColors ;
		HBRUSH			BackgroundBrush ;
		TSpecialCharacter *	SpecialCharacters ;
		TProgrammedKey *	ProgrammedKeys ;
		THardcopy *		Hardcopy ;


	// Afin d'éviter d'interminables GetApplication () -> GetMainWindow () ->
	// GetMachin ()..., des pointeurs sur certaines données de TApplication
	// et TTerminalWindow sont recopiés ici :
		TScreenCharacter **	ScreenMemory ; 	// Ca, c'est la mémoire écran


	// Données servant à la gestion de l'écran :
	//
	// DCHandle :
	//	Handle sur le device context de la fenêtre CLIENTE de
	//	TTerminalWindow (eh oui, les TDecoratedFrame ont une fenêtre
	//	cliente qui occupe toute la partie cliente de la fenêtre, moins
	//	les éventuels gadgets (barre d'état, de m'essage, etc...)
	//	On peut tout faire dans cette fenêtre cliente sans crainte de
	//	déborder sur les gadgets.
	//
	// FontHandle :
	//	Handle sur la police actuellement utilisée.
	//	Malgré la gestion de plusieurs attributs pour les caractères,
	//	un seul handle est utilisé. Lorsqu'il faut afficher un caractère
	//	avec un attribut différent, on se contente de libérer le handle
	//	et de créer une nouvelle police logique (il aurait fallu 24
	//	handles, sinon...)
	//
	// PreviousWeight, Previous... :
	//	Attributs du dernier caractère écrit
	//
	// PreviousBackground, PreviousForeground :
	//	Indique la couleur courante pour le prochain caractère.
	//
		HDC			DCHandle ;
		TPrintDC *		PrinterDC ;
		int			DCHandleReferenceCount ;
		HFONT			FontHandles [ 8 ] ;
		HFONT			SpecialFontHandle ;
		TCharacterAttributes	PreviousAttributes ;
		COLORREF		PreviousBackground,
					PreviousForeground ;
		char 			SpecialFontName [ MAX_PARAMETER_LENGTH ] ;
		int			SpecialFontHeight ;
		BOOL			InSpecialSequence ;
		BOOL			Reverse ;
		BOOL			ContinuousPrint ;
		int			FileTransferFd ;



	private :
		COLORREF		StaticPreviousBackground ;
		char			StaticBuffer [ MAX_PARAMETER_LENGTH ] ;
		BOOL			StaticGotSync ;
		HCURSOR			StaticSavedCursor ;



	// Les données publiques : elles sont susceptibles d'être accédées
	// depuis d'autres classes.
	//
	// CharacterWidth, CharacterHeight :
	//	Taille d'un caractère de Terminal (en fonction de la police
	// 	choisie).
	//
	// Origin :
	// 	Désigne la ligne numéro zéro de Terminal. Il s'agit d'une
	//	optimisation : si l'on doit défiler d'une ligne vers le haut,
	//	par exemple, on fera Origin++ et on mettra à zero la ligne
	//	no Origin-1.
	//
	// CurrentLine, CurrentColumn :
	//	Position actuelle du curseur.
	//
	// ScreenWidth, ScreenHeight :
	//	Hauteur et largeur de l'écran du terminal, en caractères.
	//
	// FunctionKeypad :
	//	Pavé de touches de fonction.
	//
	public :
		int			CharacterWidth,
					CharacterHeight ;
		BOOL			VariablePitch ;
		int			Origin ;
		int			CurrentLine,
					CurrentColumn ;
		int			ScreenWidth,
					ScreenHeight ;
		TPoint			SelectionStart,
					SelectionEnd ;
		BOOL			InSelection,
					ButtonReleased ;
		TFunctionKeypad	*	FunctionKeypad ;
		TRedoList		RedoList ;
		BOOL			InMessageReceive ;
		BOOL			InPrintMode ;
		TInputMessage *		InputMessage ;
		FILE *			PrinterFP ;
		TWindow *		TerminalWindowClient ;
		TPrintingDialog *	PrintingWindow ;

		TConnection *		Connection ;
		int			SessionId ;
		

	public :
		TDisplayDevice  ( TWindow *  	client,
				  TConnection * connection,
				  int		session ) ;
	       ~TDisplayDevice  ( ) ;


		void		ResetFont	( BOOL  Create  =  FALSE ) ;
		void		ResetColors	( void ) ;
		
		void		Scroll  	( int			count,
						  BOOL			clean = TRUE ) ;
		void		ManageCaret     ( BOOL			Activate ) ;

		void		UpdateScreen	( TRect	&		rect ) ;
		void		UpdateScreen	( int			x1,
						  int			y1,
						  int			x2,
						  int			y2 ) ;

		BOOL		ProcessKey	( UINT			virtkey,
						  UINT			flags ) ;
		void		InsertEmptyLine ( int			where ) ;
		void		DeleteLine	( int			where ) ;

		void		LoadKeys	( void ) ;
		void		LoadSettings	( void ) ;
		void		LoadSequences	( void ) ;
		void		LoadSpecialCharacters
						( void ) ;
		void		AdjustWindowSize( void ) ;

		TSequenceMatch  FindMatch	( char *		sequence,
						  int &			index ) ;

		void		GetSequenceParameter (
						  int			seqindex,
						  char *		received,
						  int			number,
						  int &			result ) ;
		void		GetSequenceParameter (
						  int			seqindex,
						  char *		received,
						  int			number,
						  int 			string_length,
						  char *		result ) ;
				      

		int		Write		( char			c ) ;
		void		Write		( char *		s,
						  int			sz ) ;
		void		WriteCharacter  ( char 			c ) ;
		void		TextOut		( HDC			hDC,
						  int			x,
						  int			y,
						  char *		line,
						  int			length,
						  BOOL			issemigraphic ) ;

		void		SysColorChange  ( COLORREF		prevfore,
						  COLORREF		prevback,
						  COLORREF		newfore,
						  COLORREF		newback ) ;


		void		StartSelection  ( TPoint &		where ) ;
		void		StopSelection	( void ) ;
		BOOL		ExpandSelection ( TPoint & 		where,
						  BOOL			InMouseMove ) ;
		void		DrawSelection	( TPoint &		newcoord ) ;
		int		GetSelectionSize( void ) ;
		int		GetSelection    ( char *		buffer ) ;

		void		ClearArea	( int			x1,
						  int			y1,
						  int			x2,
						  int			y2 ) ;



	// Convertit les coordonnées dans TPoint (exprimées en pixels) en
	// coordonnées caractères.

		void		PixelToCharacter  ( TPoint &   where )
		   {
			where. x  /=  CharacterWidth ;
			where. y  /=  CharacterHeight ;

			if  ( where. x + 1  >=  ScreenWidth )  
				where. x = ScreenWidth - 1 ;

			if  ( where. y + 1  >=  ScreenHeight )
                        	where. y = ScreenHeight - 1 ;
		    }


	// Retourne TRUE si le point est en dehors de la fenêtre Terminal
	// (cad sa partie cliente réservée à l'émulation).
		BOOL		OutsideWindow  ( TPoint &  point )
		   {
			TRect		Rect ;


			TerminalWindow -> GetClientRect ( Rect ) ;

		// Pourquoi recalculer le bord droit et inférieur ? parceque
		// la fenêtre cliente occupe toute la surface de la fenêtre,
		// y compris à la place de la fenêtre touches de fonction
			Rect. right  = Rect. left + ( ScreenWidth  * CharacterWidth ) ;
			Rect. bottom = Rect. top  + ( ScreenHeight * CharacterHeight ) ;

			return ( ! PtInRect ( & Rect, point ) ) ;
		     }



	// Retourne dans Width et Height la taille que la fenêtre doit avoir
	// (barre de titre et bords compris) pour pouvoir contenir ScreenWidth
	// colonnes sur ScreenHeight lignes.
		void	GetWindowSize	( int &			Width,
					  int &			Height )
		   {
			Width	=  ( CharacterWidth * ScreenWidth ) +
				( 2 * GetSystemMetrics ( SM_CXFRAME ) ),
			Height  =  ( CharacterHeight * ScreenHeight ) +
				( 2 * GetSystemMetrics ( SM_CYFRAME ) ) +
				GetSystemMetrics ( SM_CYCAPTION ) +
				GetSystemMetrics ( SM_CYMENU ) ;
		     }


	// Retourne l'indicateur spécifiant si tous les caractères sont
	// affichables
		BOOL	IsTransparent  ( void )
		   {
			return ( SessionParameters -> TransparentMode ) ;
		    }


	// Donne la valeur spécifiée à l'indicateur de mode transparent
		void	SetTransparent  ( BOOL  state )
		   {
			SessionParameters -> TransparentMode = state ;
		    }

	// Remplace un caractère par un blanc (utilisé pour les séquences
	// d'échappement provoquant un effacement)
		void	ClearCharacter  ( int  column, int  line )
		   {
			register int	where = ( Origin + line ) % ScreenHeight ;

			ScreenMemory [ where ] [ column ]. Character 		=  ' ' ;
			ScreenMemory [ where ] [ column ]. Attributes		=  0 ;
			ScreenMemory [ where ] [ column ]. ForegroundColor	=  SessionParameters -> ForegroundColor ;
			ScreenMemory [ where ] [ column ]. BackgroundColor	=  SessionParameters -> BackgroundColor ;

			if  ( InSpecialSequence )
				ScreenMemory [ where ] [ column ]. Attributes  |=  SPECIAL ;
		     }


	// SetFont choisit la fonte appropriée au caractère à écrire
		void	SetFont  ( TScreenCharacter *  Ch  =  0 )
		   {
			register int	Attributes = ( Ch ) ?  Ch -> Attributes : PreviousAttributes ;


			if  ( Attributes  &  SPECIAL )
				SelectObject ( DCHandle, SpecialFontHandle ) ;
			else
				SelectObject ( DCHandle, FontHandles [ Attributes & 7 ] ) ;
		    }


	// SetReverse change l'état du mode inversion vidéo
		void	SetReverse  ( BOOL  mode )
		   {
			COLORREF	TempColor ;


			if  ( ( mode  &&  ! Reverse )  ||
			      ( ! mode  &&  Reverse ) )
			   {
				Reverse = mode ;
				TempColor          = PreviousBackground ;
				PreviousBackground = PreviousForeground ;
				PreviousForeground = TempColor ;
				SetTextColor ( DCHandle, PreviousForeground ) ;
				SetBkColor   ( DCHandle, PreviousBackground ) ;
			    }
		     }



	//  Récupère le DC de la fenêtre cliente
		void	GetDC  ( void )
		   {
			if  ( ! DCHandleReferenceCount )
				DCHandle = :: GetDC ( TerminalWindowClient -> HWindow ) ;
			DCHandleReferenceCount ++ ;
		    }


	// Libère le DC de la fenêtre cliente
		void	ReleaseDC ( void ) 
		   {
			if  ( DCHandleReferenceCount  ==  1 )			
				:: ReleaseDC ( TerminalWindowClient -> HWindow, DCHandle ) ;

			if  ( DCHandleReferenceCount ) 
				DCHandleReferenceCount -- ;
		     }


	// Met à jour la position courante dans la barre d'état de la fenêtre
		void	UpdateCursorPosition  ( void ) 
		   {
			if  ( this  ==  :: Device )
			   {
				char		pos [ 32 ] ;


				wsprintf ( pos, "%d:%d", CurrentLine + 1,
					min ( CurrentColumn + 1, ScreenWidth ) ) ;
				TerminalWindow -> SetStatusBarText ( 1, pos ) ;
			     }
		     }



	// Fonctions de conversion de caractères
		void		Translate  ( THostCharSet  from,
					     THostCharSet  to,
					     char *  in, char *  out, int  size )
		   {
			if  ( from  ==  AnsiCharSet  &&  to  ==  OemCharSet )
				AnsiToOemBuff ( in, out, size ) ;
			else if  ( from  ==  OemCharSet  &&  to  ==  AnsiCharSet )
				OemToAnsiBuff ( in, out, size ) ;
		     }


		void		LocalToHost  ( char *  input, char *  output, int  size )
		   {
			Translate  ( AnsiCharSet, 
				     Configuration -> Session ( ). HostCharSet,
				     input, output, size ) ;
		     }


		void		HostToLocal  ( char *  input, char *  output, int  size )
		   {
			Translate  ( Configuration -> Session ( ). HostCharSet,
				     AnsiCharSet,
				     input, output, size ) ;
		     }


		void		LocalToHostCharSet	( char &	c )
		   {
			char 	buf [1] ;


			buf [0] = c ;
			LocalToHost ( buf, buf, 1 ) ;
			c = buf [0] ;
		     }


		void		LocalToHostCharSet	( char *	s )
		   {
			LocalToHost ( s, s, strlen ( s ) ) ;
		    }

		void		LocalToHostCharSet	( char *	s,
							  int		buflen )
		   {
			LocalToHost  ( s, s, buflen ) ;
		    }


		void		HostToLocalCharSet	( char &	c )
		   {
			char 	buf [1] ;


			buf [0] = c ;
			HostToLocal ( buf, buf, 1 ) ;
			c = buf [0] ;
		     }


		void		HostToLocalCharSet	( char *	s )
		   {
			HostToLocal  ( s, s, strlen ( s ) ) ;
		    }


		void		HostToLocalCharSet	( char *	s,
							  int		buflen )
		   {
			HostToLocal ( s, s, buflen ) ;
		    }




	// Fonctions pour l'impression
		void	StartPrint  ( void ) ;
		void    EndPrint    ( void ) ;

		BOOL	GetContinuousPrint  ( void )
		   {
			return ( ContinuousPrint ) ;
		    }


		void	SetContinuousPrint  ( int  flag )
		   {
			if  ( flag < 0 )
				ContinuousPrint = ! ContinuousPrint ;
			else
				ContinuousPrint = flag ;

			if  ( ContinuousPrint )
				StartPrint ( ) ;
			else
				EndPrint ( ) ;
		    }


		void	PrintPage  	( void ) ;
		void	PrintCharacter  ( char    c ) ;
		
     } ;
