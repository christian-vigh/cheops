/**************************************************************************/
/*                                                                        */
/*  Device.h  -  Christian Vigh, 94/09/13.                                */
/*               (d'apr�s S�bastien George et Christophe Despr�s)         */
/*                                                                        */
/*  La classe TDisplayDevice est responsable de l'affichage de tout ce    */
/*  qui concerne la partie �mulateur normal. Sa seconde t�che est aussi   */
/*  d'interpr�ter les s�quences d'�chappement, apr�s avoir charg� une     */
/*  d�finition de terminal � partir d'un fichier .TRM.                    */
/*                                                                        */
/**************************************************************************/



# define	MAX_FUNCTION_KEYS	12


/**************************************************************************/
/*                                                                        */
/*  Lorsqu'une touche est frapp�e, la fonction ProcessKey est appel�e     */
/*  pour d�terminer si cette touche fait partie des touches �mul�es.      */
/*  Le code de la touche est pass� en param�tre � cette fonction, ainsi   */
/*  qu'un param�tre Flags, constitu� par un ou logique entre un ou        */
/*  plusieurs de ces drapeaux :                                           */
/*                                                                        */
/*  SHIFT       - La touche SHIFT en m�me temps que l'autre.              */
/*  CONTROL     - Idem, pour CONTROL                                      */
/*  ALT         - Idem, pour ALT                                          */
/*  REGULARCHAR - La fonction ProcessKey n'a pas �t� appel�e suite � un   */
/*                message WM_KEYDOWN ou WM_SYSKEYDOWN, mais suite � un    */
/*                message WM_CHAR.                                        */
/*                                                                        */
/**************************************************************************/

# define	SHIFT			0x01	
# define	CONTROL			0x02
# define	ALT			0x04
# define	REGULARCHAR		0x08


/**************************************************************************/
/*                                                                        */
/*  Attributs de caract�re.                                               */
/*                                                                        */
/**************************************************************************/

# define	UNDERLINE		0x0001
# define	BOLD			0x0002
# define	ITALIC			0x0004
# define	SPECIAL			0x0010



/**************************************************************************/
/*                                                                        */
/*  Dans le fichier .TRM, les s�quences d'�chappement peuvent contenir    */
/*  des parties variables; ces parties sont exprim�es avec un caract�re   */
/*  '%' suivi d'une lettre d�signant leur type. De mani�re interne, pour  */
/*  faciliter l'analyse des s�quences envoy�es par l'h�te, ces deux       */
/*  caract�res ('%quelquechose' sont remplac�s par :                      */
/*                                                                        */
/*  Param�tre   Remplac� par            Signification                     */
/*     %n       ANY_ASCII_NUMBER        Nombre exprim� sous forme ascii   */
/*     %s       ANY_STRING              Cha�ne de caract�res quelconque   */
/*     %l       ANY_STRING_LENGTH       Longueur de la cha�ne suivante    */
/*                                      (ANY_STRING) dans la s�quence.    */
/*                                                                        */
/**************************************************************************/


# define	ANY_ASCII_NUMBER	1	// S�quence '%n'
# define	ANY_STRING		2	// S�quence '%s' 
# define	ANY_STRING_LENGTH	3	// S�quence '%l'

# define	UNUSED_COLOR		0xFFFFFFFFL
# define	MAX_LABEL_LENGTH	12


/**************************************************************************/
/*                                                                        */
/*  Unix a besoin de savoir quelle suite de caract�res est renvoy�e par   */
/*  les touches F1, F2, ..., Ins, Suppr, etc...                           */
/*  La structure TEmulatedKey d�finit :                                   */
/*                                                                        */
/*  1-Quelle combinaison de touches (sous Windows) fait partie de celles  */
/*    reconnues par Unix                                                  */
/*  2-Ce qu'il faut envoyer � Unix lorsque l'une de ces combinaisons      */
/*    arrive.                                                             */
/*                                                                        */
/*  Tout cel� est d�fini dans un fichier d'�mulation (ANSI.TRM, par       */
/*  exemple). Ce fichier, identique � un .INI, si ce n'est son extension, */
/*  contient plusieurs sections. Celle qui nous int�resse ici s'appelle   */
/*  [Keys]. Chaque ligne de cette section est de la forme :               */
/*                                                                        */
/*              keyname = value                                           */
/*                                                                        */
/*  o� 'keyname' est un nom de touche de fonction bien d�fini, et 'value' */
/*  une suite de caract�res qui sera envoy�e � Unix par appui sur la      */
/*  touche.                                                               */
/*                                                                        */
/*  Les champs de la structure sont les suivants :                        */
/*                                                                        */
/*                                                                        */
/*  KeyName :                                                             */
/*      Nom de la touche de fonction (hardcod�...)                        */
/*                                                                        */
/*  KeyCode :                                                             */
/*      Code � renvoyer (provient du .TRM)                                */
/*                                                                        */
/*  VirtualKeyCode :                                                      */
/*      Code virtuel de cette touche pour Windows (hardcode). Par exemple,*/
/*      VK_F1, VK_INS, etc.                                               */
/*                                                                        */
/*  VirtualKeyFlags :                                                     */
/*      Ce champ peut �tre une combinaison des 3 constantes suivantes :   */
/*      ALT, CONTROL et SHIFT. La touche ne sera reconnue par l'�mulateur */
/*      que si la touche sp�ciale correspondante (ALT, CONTROL et SHIFT)  */
/*      a �t� enfonc�e.                                                   */
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
/*  La structure TEmulatorSequence d�finit quelles s�quences d'�chappement*/
/*  (envoy�es par la machine distante) doivent �tre interpr�t�es par      */
/*  l'�mulateur. Les champs sont les suivants :                           */
/*                                                                        */
/*  SequenceName :                                                        */
/*      Nom de la s�quence d'�chappement (fait partie d'une liste bien    */
/*      d�finie).                                                         */
/*                                                                        */
/*  Sequence :                                                            */
/*      Caract�res composant la s�quence d'�chappement. Elle peut contenir*/
/*      des caract�res sp�ciaux tels que :                                */
/*                                                                        */
/*      \E                      -> traduit par Escape                     */
/*      \t, \n, \r, etc...      -> s�quences identiques � celles du       */
/*                                 langage C.                             */
/*      %n                     -> Indique qu'� cet emplacement de la     */
/*                                 s�quence, il peut y avoir n'importe    */
/*                                 quelle suite de caract�res repr�sentant*/
/*                                 un nombre compris entre 0 et 255.      */
/*      %%                      -> Le caract�re '%'                       */
/*                                                                        */
/*  Action :                                                              */
/*      Action associ�e � cette s�quence d'�chappement (du type           */
/*      TEmulatorAction, d�fini ci-dessous).                              */
/*                                                                        */
/*  Les d�finitions sont lues dans la section '[Escapes]' du fichier .TRM */
/*                                                                        */
/*  Remarque : Unix d�sire conna�tre la s�quence envoy�e par les touches  */
/*  Tabulation et Newline. Carriage return, par contre, ne l'int�resse    */
/*  pas. Cependant, par souci d'homog�n�it�, carriage return a �t� inclus */
/*  dans cette liste.                                                     */
/*                                                                        */
/**************************************************************************/

typedef enum
   {
		CarriageReturnAction,	// Retour en d�but de ligne
		BackspaceAction,	// Effacer le caract�re � gauche du curseur
		BellAction,		// Faire Bip
		EnterBlinkAction,	// Faire du texte clignotant
		EnterBoldAction,	// ... ou gras
		BackTabAction,		// Shift-Tab
		ClearAction,		// Effacer l'�cran
		NewlineAction,		// Descendre d'une ligne
		CursorLeftAction,	// Mouvements du curseur
		CursorRightAction,
		CursorUpAction,
		CursorDownAction,
		GotoXYAction,		// Positionner le curseur en (x,y)
		DeleteCharacterAction,	// Effacer le caract�re sous le curseur
		DeleteLineAction,	// Effacer la ligne courante
		ClearToEODAction,	// Effacer de la position courante � la fin de l'�cran
		ClearToEOLAction,	// Effacer de la position courante � la fin de la ligne
		CursorHomeAction,	// Mettre le curseur en (0,0)
		TabAction,		// Tabulation
		InsertCharacterAction,	// Ins�rer un blanc � la position courante
		InsertLineAction,	// ... ou une ligne
		ScrollUpAction,		// D�filer d'une ligne vers le haut
		EnterReverseAction,	// Passer en surbrillance
		ScrollDownAction,	// D�filer d'une ligne vers le bas
		StartUnderlineAction,	// Passer en mode soulign�
		StopUnderlineAction,	// Arr�ter le soulign� (et la surbrillance...)
		AttributesOffAction,	// Repasser en texte normal
		InvisibleModeAction,	// Les caract�res � afficher sont invisibles
		StartStandoutAction,	// Surbrillance
		StopStandoutAction,
		StartACSAction,		// Alternate character set
		StopACSAction,
		SetBackgroundAction,	// D�finir la couleur de fond
		SetForegroundAction,	// et de texte
		SetColorAction,
		SetFKStringAction,	// Programmer une touche de fonction
		SetFKLabelAction,	// D�finir le label d'une touche de fonction
		EnableFKAction,		// Afficher les touches de fonction
		DisableFKAction,	// Masquer les touches de fonction

		WriteCharacterAction,	// Non utilis� (�ventuellement, peut
					// correspondre � un caratc�re quelconque)
		MessageHeadAction,	// D�but de message
		MessageTailAction,	// Fin de message

		BulkPrintBeginAction,	// D�but et fin d'impression d'un fichier entier
		BulkPrintEndAction,	

		FramePrintBeginAction,	// D�but et fin d'impression d'un bloc de fichier
		FramePrintEndAction,	

		CancelPrintAction,	// Annulation d'une impression "bulk"

		OpenFileAction,  	// Transfert de fichier
		CloseFileAction,	// Fin de transfert
		WriteFileAction,	// Ecrire un bloc
		MessageAction,		// Afficher un message
		ExecuteAction,		// Ex�cuter une application
		TypeAction,		// Retourner le type de connexion
		UserNameAction,		// Pour l'autologin
		UserPasswordAction,

		ZeroedAction,		// Actions � ne faire qu'une fois
		UndefinedAction		// Action non d�finie
     }  TEmulatorAction ;
		
		

typedef struct
   {
		char *			SequenceName ;
		char *			Sequence ;
		TEmulatorAction         Action ;
    }  TEmulatorSequence ;





/**************************************************************************/
/*                                                                        */
/*   La structure TProgrammedKey sert � g�rer les touches de fonction     */
/*   devant �mettre une cha�ne de caract�res lorsqu'elles sont press�es.  */
/*                                                                        */
/**************************************************************************/

typedef struct
   {
		UINT		VirtualKeyCode ;
		char		StringToSend [ MAX_PARAMETER_LENGTH ] ;
    }  TProgrammedKey ;



/**************************************************************************/
/*                                                                        */
/*  Unix (comme l'�mulateur ANSI) peut g�rer des caract�res sp�ciaux      */
/*  (en envoyant une s�quence d'�chappement indiquant que les caract�res  */
/*  suivants sont sp�ciaux, puis les caract�res eux-m�me).                */
/*  Afin de pouvoir les g�rer par une table externe (d�finie dans le      */
/*  fichier xxx.TRM), on a besoin de la structure suivante :              */
/*                                                                        */
/**************************************************************************/

typedef struct
   {
	char *		CharacterName ;         // Mot-cl� dans le .TRM
	char		HostValue,		// Caract�re envoy� par Unix
			LocalValue ;		// Sa traduction chez nous
    }  TSpecialCharacter ;





/**************************************************************************/
/*                                                                        */
/*   La classe TFunctionKeypad permet de g�rer les pav�s de touche de     */
/*   fonction.                                                            */
/*   Note : EV_COMMAND est utilis� ici pour la table de r�ponses, car     */
/*   EV_COMMAND_AND_ID semble transmettre un param�tre toujours �gal � 0. */
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
/*      G�re les commandes saisies par l'utilisateur.                     */
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
/*      Elle est responsable de l'interpr�tation des s�quences de         */
/*      caract�res envoy�es par Unix, ainsi que de l'envoi des s�quences  */
/*      correspondant aux touches reconnues par Unix.                     */
/*      Son r�le principal est cependant la gestion de l'affichage �      */
/*      l'�cran. Pour ce faire, un tableau de TScreenCharacter, � deux    */
/*      dimensions, est allou� dynamiquement. La 1e dimension correspond  */
/*      aux nombre de lignes de l'�cran, et la seconde au nombre de       */
/*      colonnes (ces deux param�tres sont d�finis � la section [Settings]*/
/*      du fichier .TRM, mots-cl� 'Lines' et 'Columns').                  */
/*                                                                        */
/*      De mani�re � pouvoir g�rer les attributs tels que : soulign�,     */
/*      surbrillance, etc..., les �l�ments de ce tableau (les caract�res  */
/*      actuellement affich�s � l'�cran) contiennent aussi les attributs  */
/*      des caract�res.                                                   */
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

	// Attributs d'un caract�re
		typedef unsigned int	 TCharacterAttributes ;


	// Un caract�re de l'�cran
		typedef struct
		   {
			char			Character ; // le caract�re lui-m�me
			TCharacterAttributes    Attributes ;
			COLORREF		BackgroundColor,
						ForegroundColor ;
		    }  TScreenCharacter ;


	// Valeur renvoy�e par la fonction membre FindMatch
		typedef enum
		    {
				NoMatch,
				PartialMatch,
				FullMatch
		     }  TSequenceMatch ;


	private :
	//  Tableaux permettant � l'�mulateur d'�muler...
		TEmulatedKey *		EmulatedKeys ;
		TEmulatorSequence *	EmulatorSequences ;
		COLORREF *		EmulatorColors ;
		HBRUSH			BackgroundBrush ;
		TSpecialCharacter *	SpecialCharacters ;
		TProgrammedKey *	ProgrammedKeys ;
		THardcopy *		Hardcopy ;


	// Afin d'�viter d'interminables GetApplication () -> GetMainWindow () ->
	// GetMachin ()..., des pointeurs sur certaines donn�es de TApplication
	// et TTerminalWindow sont recopi�s ici :
		TScreenCharacter **	ScreenMemory ; 	// Ca, c'est la m�moire �cran


	// Donn�es servant � la gestion de l'�cran :
	//
	// DCHandle :
	//	Handle sur le device context de la fen�tre CLIENTE de
	//	TTerminalWindow (eh oui, les TDecoratedFrame ont une fen�tre
	//	cliente qui occupe toute la partie cliente de la fen�tre, moins
	//	les �ventuels gadgets (barre d'�tat, de m'essage, etc...)
	//	On peut tout faire dans cette fen�tre cliente sans crainte de
	//	d�border sur les gadgets.
	//
	// FontHandle :
	//	Handle sur la police actuellement utilis�e.
	//	Malgr� la gestion de plusieurs attributs pour les caract�res,
	//	un seul handle est utilis�. Lorsqu'il faut afficher un caract�re
	//	avec un attribut diff�rent, on se contente de lib�rer le handle
	//	et de cr�er une nouvelle police logique (il aurait fallu 24
	//	handles, sinon...)
	//
	// PreviousWeight, Previous... :
	//	Attributs du dernier caract�re �crit
	//
	// PreviousBackground, PreviousForeground :
	//	Indique la couleur courante pour le prochain caract�re.
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



	// Les donn�es publiques : elles sont susceptibles d'�tre acc�d�es
	// depuis d'autres classes.
	//
	// CharacterWidth, CharacterHeight :
	//	Taille d'un caract�re de Terminal (en fonction de la police
	// 	choisie).
	//
	// Origin :
	// 	D�signe la ligne num�ro z�ro de Terminal. Il s'agit d'une
	//	optimisation : si l'on doit d�filer d'une ligne vers le haut,
	//	par exemple, on fera Origin++ et on mettra � zero la ligne
	//	no Origin-1.
	//
	// CurrentLine, CurrentColumn :
	//	Position actuelle du curseur.
	//
	// ScreenWidth, ScreenHeight :
	//	Hauteur et largeur de l'�cran du terminal, en caract�res.
	//
	// FunctionKeypad :
	//	Pav� de touches de fonction.
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



	// Convertit les coordonn�es dans TPoint (exprim�es en pixels) en
	// coordonn�es caract�res.

		void		PixelToCharacter  ( TPoint &   where )
		   {
			where. x  /=  CharacterWidth ;
			where. y  /=  CharacterHeight ;

			if  ( where. x + 1  >=  ScreenWidth )  
				where. x = ScreenWidth - 1 ;

			if  ( where. y + 1  >=  ScreenHeight )
                        	where. y = ScreenHeight - 1 ;
		    }


	// Retourne TRUE si le point est en dehors de la fen�tre Terminal
	// (cad sa partie cliente r�serv�e � l'�mulation).
		BOOL		OutsideWindow  ( TPoint &  point )
		   {
			TRect		Rect ;


			TerminalWindow -> GetClientRect ( Rect ) ;

		// Pourquoi recalculer le bord droit et inf�rieur ? parceque
		// la fen�tre cliente occupe toute la surface de la fen�tre,
		// y compris � la place de la fen�tre touches de fonction
			Rect. right  = Rect. left + ( ScreenWidth  * CharacterWidth ) ;
			Rect. bottom = Rect. top  + ( ScreenHeight * CharacterHeight ) ;

			return ( ! PtInRect ( & Rect, point ) ) ;
		     }



	// Retourne dans Width et Height la taille que la fen�tre doit avoir
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


	// Retourne l'indicateur sp�cifiant si tous les caract�res sont
	// affichables
		BOOL	IsTransparent  ( void )
		   {
			return ( SessionParameters -> TransparentMode ) ;
		    }


	// Donne la valeur sp�cifi�e � l'indicateur de mode transparent
		void	SetTransparent  ( BOOL  state )
		   {
			SessionParameters -> TransparentMode = state ;
		    }

	// Remplace un caract�re par un blanc (utilis� pour les s�quences
	// d'�chappement provoquant un effacement)
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


	// SetFont choisit la fonte appropri�e au caract�re � �crire
		void	SetFont  ( TScreenCharacter *  Ch  =  0 )
		   {
			register int	Attributes = ( Ch ) ?  Ch -> Attributes : PreviousAttributes ;


			if  ( Attributes  &  SPECIAL )
				SelectObject ( DCHandle, SpecialFontHandle ) ;
			else
				SelectObject ( DCHandle, FontHandles [ Attributes & 7 ] ) ;
		    }


	// SetReverse change l'�tat du mode inversion vid�o
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



	//  R�cup�re le DC de la fen�tre cliente
		void	GetDC  ( void )
		   {
			if  ( ! DCHandleReferenceCount )
				DCHandle = :: GetDC ( TerminalWindowClient -> HWindow ) ;
			DCHandleReferenceCount ++ ;
		    }


	// Lib�re le DC de la fen�tre cliente
		void	ReleaseDC ( void ) 
		   {
			if  ( DCHandleReferenceCount  ==  1 )			
				:: ReleaseDC ( TerminalWindowClient -> HWindow, DCHandle ) ;

			if  ( DCHandleReferenceCount ) 
				DCHandleReferenceCount -- ;
		     }


	// Met � jour la position courante dans la barre d'�tat de la fen�tre
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



	// Fonctions de conversion de caract�res
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