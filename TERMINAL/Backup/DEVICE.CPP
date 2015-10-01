/**************************************************************************/
/*                                                                        */
/*  Device.cpp  -  Christian Vigh, 94/09/13.                              */
/*                 (d'après Sébastien George et Christophe Després)       */
/*                                                                        */
/*  Implémentation de la classe TDisplayDevice.                           */
/*                                                                        */
/**************************************************************************/


# include	"AllTerm.h"
# pragma	hdrstop

# include	"ChopJet.h"
# include	"TextFile.h"
# include	"UnixApp.h"
# include	"Icon.h"
# include	"UnixDlg.h"
# include	"Event.h"
# include	"Cheops.h"
# include	"EditFile.h"
# include	"MNCommon.h"
# include	"ChopCfg.h"
# include	"ChopTree.h"
# include	"ChopMenu.h"
# include	"ChopUp.h"
# include	"ChopMail.h"
# include	"Print.h"



/**************************************************************************/
/*                                                                        */
/*  Touches de fonction faisant partie de l'émulation.                    */
/*                                                                        */
/**************************************************************************/

TEmulatedKey 		__EmulatedKeys [ ] =
   {
	{ "F1"		, NULL, VK_F1		, 0 },
	{ "F2"		, NULL, VK_F2		, 0 },
	{ "F3"		, NULL, VK_F3		, 0 },
	{ "F4"		, NULL, VK_F4		, 0 },
	{ "F5"		, NULL, VK_F5		, 0 },
	{ "F6"		, NULL, VK_F6		, 0 },
	{ "F7"		, NULL, VK_F7		, 0 },
	{ "F8"		, NULL, VK_F8		, 0 },
	{ "F9"		, NULL, VK_F9		, 0 },
	{ "F10"		, NULL, VK_F10		, 0 },
	{ "F11"		, NULL, VK_F11		, 0 },
	{ "F12"		, NULL, VK_F12		, 0 },
	{ "Up"		, NULL, VK_UP		, 0 },
	{ "Down"	, NULL, VK_DOWN		, 0 },
	{ "Right"	, NULL, VK_RIGHT	, 0 },
	{ "Left"	, NULL, VK_LEFT		, 0 },
	{ "Home"	, NULL, VK_HOME		, 0 },
	{ "NextPage"	, NULL, VK_NEXT		, 0 },
	{ "PreviousPage", NULL, VK_PRIOR	, 0 },
	{ "Tab"		, NULL, '\t'		, REGULARCHAR },
	{ "BackTab"	, NULL, VK_TAB		, SHIFT },
	{ "BackSpace"	, NULL, VK_BACK		, REGULARCHAR },
	{ "Delete"	, NULL, VK_DELETE	, 0 },
	{ "DelLine"	, NULL, VK_DELETE	, CONTROL },
	{ "End"		, NULL, VK_END		, 0 },
	{ "Insert"	, NULL, VK_INSERT	, 0 },
	{ "Break"	, NULL, VK_DELETE	, SHIFT | REGULARCHAR   },
	{ "XON"		, NULL, XON		, CONTROL | REGULARCHAR },
	{ "XOFF"	, NULL, XOFF		, CONTROL | REGULARCHAR },
	{  NULL		, NULL, 0		, 0 }
    } ;
	


/**************************************************************************/
/*                                                                        */
/*  Séquences d'échappement reconnues par l'émulateur.                    */
/*                                                                        */
/**************************************************************************/

TEmulatorSequence	__EmulatorSequences [ ] =
   {
	{ "Bell"		, NULL, BellAction		},
	{ "Blink"               , NULL, EnterBlinkAction	},
	{ "Bold"		, NULL, EnterBoldAction		},
	{ "BackTab"		, NULL, BackTabAction		},
	{ "Clear"		, NULL, ClearAction		},
	{ "CarriageReturn"	, NULL, CarriageReturnAction	},
	{ "Newline"		, NULL, NewlineAction		},
	{ "Left"		, NULL, CursorLeftAction	},
	{ "Right"		, NULL, CursorRightAction	},
	{ "Up"			, NULL, CursorUpAction		},
	{ "Down"		, NULL, CursorDownAction	},
	{ "GotoXY"		, NULL, GotoXYAction		},
	{ "Delete"		, NULL, DeleteCharacterAction	},
	{ "DelLine"		, NULL, DeleteLineAction	},
	{ "ClearEOD"		, NULL, ClearToEODAction	},
	{ "ClearEOL"		, NULL, ClearToEOLAction	},
	{ "Home"		, NULL, CursorHomeAction	},
	{ "Tab"			, NULL, TabAction		},
	{ "Insert"		, NULL, InsertCharacterAction	},
	{ "InsertLine"		, NULL, InsertLineAction	},
	{ "ScrollUp"		, NULL, ScrollUpAction		},
	{ "Reverse"		, NULL, EnterReverseAction	},
	{ "ScrollDown"		, NULL, ScrollDownAction	},
	{ "StartUnderline"	, NULL, StartUnderlineAction	},
	{ "StopUnderline"	, NULL, StopUnderlineAction	},
	{ "AllAttrOFF"		, NULL, AttributesOffAction	},
	{ "StartInvisible"	, NULL, InvisibleModeAction	},
	{ "StartStandout"	, NULL, StartStandoutAction	},
	{ "StopStandout"	, NULL, StopStandoutAction	},
	{ "StartACS"		, NULL, StartACSAction		},
	{ "StopACS"		, NULL, StopACSAction		},
	{ "SetBackground"	, NULL, SetBackgroundAction	},
	{ "SetForeground"	, NULL, SetForegroundAction	},
	{ "SetColor"		, NULL, SetColorAction		},
	{ "SetFKString"		, NULL, SetFKStringAction	},
	{ "SetFKLabel"		, NULL, SetFKLabelAction	},
	{ "EnableFK"		, NULL, EnableFKAction		},
	{ "DisableFK"		, NULL, DisableFKAction		},
	{ "BulkPrintBegin"	, NULL, BulkPrintBeginAction	},
	{ "BulkPrintEnd"	, NULL, BulkPrintEndAction	},
	{ "FramePrintBegin"	, NULL, FramePrintBeginAction   },
	{ "FramePrintEnd" 	, NULL, FramePrintEndAction	},
	{ "OpenFile"		, NULL, OpenFileAction		},
	{ "CloseFile"		, NULL, CloseFileAction		},
	{ "WriteFile"		, NULL, WriteFileAction		},
	{ "Message"		, NULL, MessageAction		},
	{ "Execute"		, NULL, ExecuteAction		},
	{ "Type"		, NULL, TypeAction		},

// Pseudo-actions (internes à l'émulateur)
	{  NULL			, MESSAGE_HEADER , MessageHeadAction 	},
	{  NULL			, MESSAGE_TRAILER, MessageTailAction 	},
	{  NULL			, "ogin:"	 , UserNameAction	},
	{  NULL			, "assword:"	 , UserPasswordAction 	},


	{  NULL			, NULL, UndefinedAction		}
    } ;




/**************************************************************************/
/*                                                                        */
/*   Table des couleurs gérées (elles sont au maximum de 8).              */
/*                                                                        */
/*                                                                        */
/**************************************************************************/

# define	MAX_COLORS			8


COLORREF		__EmulatorColors [ MAX_COLORS ]  =
   {
	RGB ( 0x00, 0x00, 0x00 ),	// Noir
	RGB ( 0xFF, 0x00, 0x00 ),	// Rouge
	RGB ( 0x00, 0x80, 0x00 ),	// Vert
	RGB ( 0xFF, 0xFF, 0x00 ),	// Jaune
	RGB ( 0x00, 0x00, 0xFF ),	// Bleu
	RGB ( 0xFF, 0x00, 0x80 ),	// Magenta
	RGB ( 0x00, 0xFF, 0xFF ),	// Cyan
	RGB ( 0xFF, 0xFF, 0xFF )	// Blanc
     } ;




/**************************************************************************/
/*                                                                        */
/*  Table des caractères spéciaux.                                        */
/*                                                                        */
/**************************************************************************/

TSpecialCharacter	__SpecialCharacters [ ]  =
   {
	{ "UpperLeftCorner" 	, 0, 0 },
	{ "UpperRightCorner" 	, 0, 0 },
	{ "LowerLeftCorner" 	, 0, 0 },
	{ "LowerRightCorner" 	, 0, 0 },
	{ "VerticalLine"	, 0, 0 },
	{ "HorizontalLine"	, 0, 0 },
	{ "RightArrow"		, 0, 0 },
	{ "LeftArrow"		, 0, 0 },
	{ "DownArrow"		, 0, 0 },
	{ "SolidSquare"		, 0, 0 },
	{ "Lantern"		, 0, 0 },
	{ "UpArrow"		, 0, 0 },
	{ "Diamond"		, 0, 0 },
	{ "CheckerBoard"	, 0, 0 },
	{ "Degree"		, 0, 0 },
	{ "PlusMinus"		, 0, 0 },
	{ "Squares"		, 0, 0 },
	{ "Plus"		, 0, 0 },
	{ "ScanLine1"		, 0, 0 },
	{ "ScanLine9"		, 0, 0 },
	{ "LeftTee"		, 0, 0 },
	{ "RightTee"		, 0, 0 },
	{ "BottomTee"		, 0, 0 },
	{ "TopTee"		, 0, 0 },
	{ "Bullet"		, 0, 0 },
	{  NULL 		, 0, 0 }
     } ;
     


/**************************************************************************/
/*                                                                        */
/*  Table des touches de fonction devant émettre une chaîne de caractères */
/*  au lieu d'une séquence d'échappement.                                 */
/*                                                                        */
/**************************************************************************/

TProgrammedKey		__ProgrammedKeys [ ]  =
   {
	{ VK_F1 , "" },
	{ VK_F2 , "" },
	{ VK_F3 , "" },
	{ VK_F4 , "" },
	{ VK_F5 , "" },
	{ VK_F6 , "" },
	{ VK_F7 , "" },
	{ VK_F8 , "" },
	{ VK_F9 , "" },
	{ VK_F10, "" },
	{ VK_F11, "" },
	{ VK_F12, "" },
	{ 0xFFFF }
     } ;
     





/**************************************************************************/
/*                                                                        */
/*   GetOutputName -                                                      */
/*      Retourne le nom du périphérique d'impression, sans le ':' à la    */
/*      fin ( car fopen() n'aime pas le ':' ).                            */
/*                                                                        */
/**************************************************************************/

char *		GetOutputName ( void )
   {
	static char 		Buffer [ MAX_PARAMETER_LENGTH ] ;
	register char *	        p ;



	strcpy ( Buffer, Printer -> GetSetup ( ). GetOutputName ( ) ) ;


	if  ( ( p = strchr ( Buffer, ':' ) )  !=  NULL )
		* p = 0 ;

	return ( Buffer ) ;
     }





/**************************************************************************/
/*                                                                        */
/*  NextCharacter :                                                       */
/*      Retourne le prochain caractère d'une chaîne, en interprétant les  */
/*      séquences d'échappement.                                          */
/*      En cas de succès, fait pointer '*str' sur le caractère suivant la */
/*      séquence.                                                         */
/*      Retourne 0 si la fin de la chaîne est atteinte.                   */
/*                                                                        */
/**************************************************************************/

int	NextCharacter  ( char **  str )
    {
	register char *		p	=  * str ;
	register char		c ;
	int			value,
				base,
				i ;


	if  ( ! * p )
		return ( 0 ) ;


// Caractère normal
	if  ( * p  !=  '\\'  &&  * p  !=  '^' )
	   {
		* str = p + 1 ;
		return ( * p ) ;
	    }


// Caractère préfixé par '^' (control)
	if  ( * p  ==  '^' )
	   {
		register char 	c = * ( p + 1 ) ;


	// Si le caractère est valide (le NUL est totalement exclus...), on
	// le convertit
		if  ( c  > '@'  &&  c <= '@' + 31 )
		   {
			* str = p + 2 ;
			return ( c - '@' ) ;
		    }
		else
		   {
			* str = p + 1 ;
			return ( '^' ) ;
		    }
	      }

	p ++ ;

// Caractère préfixé par '\' : il faut l'interpréter
	switch ( * p )
	    {
		case	0 :
			return ( 0 ) ;		// N'a pus

		case	'E' :
		case	'e' :
			c = 27 ;		// Escape
			break ;

		case	'b' :
			c = 8 ;   		// Backspace
			break ;

		case	't' :
			c = '\t' ;		// Tab
			break ;

		case	'n' :
			c = '\n' ;		// Newline
			break ;

		case	'r' :
			c = '\r' ;		// Carriage return
			break ;

		default :
			if  ( ! isxdigit ( * p ) )
			   {
				c = * p ;
				break ;
			    }

			value = 0 ;

			if  ( * p  ==  '0' )
			   {
				switch ( * ( p + 1 ) )
				   {
					case	'b' :
					case	'B' :
						base = 2 ;
						p   += 2 ;
						break ;

					case	'o' :
					case	'O' :
						base = 8 ;
						p   += 2 ;
						break ;

					case	'd' :
					case	'D' :
						base = 10 ;
						p   += 2 ;
						break ;

					case	'x' :
					case	'X' :
						base = 16 ;
						p   += 2 ;
						break ;

					default :
						if  ( isdigit ( * p ) )
						   {
							base = 8 ;
							p   += 1 ;
						    }
				    }
			    }
			else
				base  =  10 ;

			while  ( * p  &&  isxdigit ( * p ) )
			   {
				i = * p - '0' ;

				if  ( base  ==  16  &&  i > 9 )
					i = * p - '@' ;
				if  ( i >= base )
					break ;

				value = ( value * base ) + i ;
				p ++ ;
			     }

			p -- ;
			c = value ;
	    }

	* str = p + 1 ;
	return ( c ) ;
    }




/**************************************************************************/
/*                                                                        */
/*   TDisplayDevice :: TDisplayDevice                                     */
/*      Initialiseur de la classe.                                        */
/*      Initialise le display context et la police de caractères.         */
/*                                                                        */
/**************************************************************************/

# define	ALLOCATE(var,type)      var = new  type [ sizeof ( __##var ) / sizeof ( type ) ]



TDisplayDevice :: TDisplayDevice   ( TWindow *  	client,
				     TConnection * 	c,
				     int		session )


   {
	register int			i ;
	register char *			p ;


	Connection	=  c ;
	SessionId	=  session ;
	InputMessage	=  new  TInputMessage ( ) ;
	Hardcopy	=  new  THardcopy ;
	FileTransferFd	=  -1 ;

	RedoList. SetMinSize ( Configuration -> Terminal ( ). MinRedoSize ) ;


// Chargement des touches de fonction émulées, des options et des séquences
// d'échappement reconnues
	ALLOCATE ( EmulatedKeys     , TEmulatedKey ) ;
	ALLOCATE ( EmulatorSequences, TEmulatorSequence ) ;
	ALLOCATE ( EmulatorColors   , COLORREF ) ;
	ALLOCATE ( SpecialCharacters, TSpecialCharacter ) ;
	ALLOCATE ( ProgrammedKeys   , TProgrammedKey ) ;

	memcpy ( EmulatedKeys     , __EmulatedKeys     , sizeof ( __EmulatedKeys      ) ) ;
	memcpy ( EmulatorSequences, __EmulatorSequences, sizeof ( __EmulatorSequences ) ) ;
	memcpy ( EmulatorColors   , __EmulatorColors   , sizeof ( __EmulatorColors    ) ) ;
	memcpy ( SpecialCharacters, __SpecialCharacters, sizeof ( __SpecialCharacters ) ) ;
	memcpy ( ProgrammedKeys   , __ProgrammedKeys   , sizeof ( __ProgrammedKeys    ) ) ;

	LoadKeys ( ) ;
	LoadSettings ( ) ;
	LoadSequences ( ) ;
	LoadSpecialCharacters ( ) ;


// Initialisation de la mémoire écran.
	ScreenMemory		=  new  TScreenCharacter * [ ScreenHeight ] ;

	for  ( i = 0 ; i < ScreenHeight ; i ++ )
		ScreenMemory [i]  =  new  TScreenCharacter [ ScreenWidth ] ;
		


// Initialisation de la position courante et des attributs de caractère.
	CurrentColumn		=
	CurrentLine		=
	Origin			=  0 ;

	PreviousAttributes	=  0 ;

	PreviousBackground	=  SessionParameters -> BackgroundColor ;
	PreviousForeground	=  SessionParameters -> ForegroundColor ;
	BackgroundBrush		=  0 ;

	InSelection		=  FALSE ;
	InSpecialSequence	=  FALSE ;
	InMessageReceive	=  FALSE ;
	InPrintMode		=  FALSE ;
	PrintingWindow		=  NULL ;
	ButtonReleased		=  FALSE ;
	Reverse			=  FALSE ;
	ContinuousPrint		=  FALSE ;
	FunctionKeypad		=  NULL ;
	PrinterFP		=  NULL ;
	
	DCHandle		=  NULL ;
	DCHandleReferenceCount	=  0 ;
	TerminalWindowClient	=  client ;

	for  ( i = 0 ; i <= ( UNDERLINE | BOLD | ITALIC ) ; i ++ )
		FontHandles [i] = 0 ;
	SpecialFontHandle = 0 ;


// Variables initialement locales et statiques
	StaticPreviousBackground = 0 ;
	* StaticBuffer = 0 ;
	StaticGotSync = 0 ;
	StaticSavedCursor = :: LoadCursor ( NULL, IDC_ARROW ) ;	


// Effacement du tableau de caractères
	ClearArea ( 0, 0, ScreenWidth - 1, ScreenHeight - 1 ) ;

// Chargement de la police de caractères.
	ResetFont ( TRUE ) ;
	ResetColors ( ) ;


// Annuler les actions UserName et UserPassword si aucun username n'est défini
// pour cette session
	p = Configuration -> Connection ( SessionId ). UserName ;

	if  ( ! * p )
	   {
		for  ( i = 0 ; EmulatorSequences [i]. Action  !=  UndefinedAction ; i ++ )
		   {
			if  ( EmulatorSequences [i]. Action  ==  UserNameAction )
			   {
				EmulatorSequences [i]. Action = ZeroedAction ;
				break ;
			    }
		     }
	     }


	p = Configuration -> Connection ( SessionId ). UserPassword ;

	if  ( ! * p )
	   {
		for  ( i = 0 ; EmulatorSequences [i]. Action  !=  UndefinedAction ; i ++ )
		   {
			if  ( EmulatorSequences [i]. Action  ==  UserPasswordAction )
			   {
				EmulatorSequences [i]. Action = ZeroedAction ;
				break ;
			    }
		     }
	     }
    }         



/**************************************************************************/
/*                                                                        */
/*   TDisplayDevice :: ~TDisplayDevice                                    */
/*      Destructeur de la classe.                                         */
/*      Libère la mémoire allouée pour l'écran ainsi que toutes les       */
/*      ressources utilisées.                                             */
/*                                                                        */
/**************************************************************************/

TDisplayDevice :: ~TDisplayDevice ( )
   {

	register int		i ;

// Libérer le message
	delete  InputMessage ;
	delete  Hardcopy ;
	
// Libérer la mémoire écran
	for  ( i = 0 ; i < ScreenHeight ; i ++ )
		delete []  ScreenMemory [i] ;
	delete []  ScreenMemory ;
	ScreenMemory = 0 ;


// Libérer l'espace occupé par les séquences d'échappement et les touches de
// fonction :
	for  ( i = 0 ; EmulatedKeys [i]. KeyName  !=  NULL ; i ++ )
		free ( EmulatedKeys [i]. KeyCode ) ;

	for  ( i = 0 ; EmulatorSequences [i]. SequenceName  !=  NULL ; i ++ )
		free ( EmulatorSequences [i]. Sequence ) ;

// Libérer le handle de la police
	for  ( i = 0 ; i <= ( BOLD | ITALIC | UNDERLINE ) ; i ++ )
	   {
		if  ( FontHandles [i] )
			DeleteObject ( FontHandles [i] ) ;
	    }

	if  ( SpecialFontHandle )
		DeleteObject ( SpecialFontHandle ) ;

	DeleteObject ( BackgroundBrush ) ;


	if  ( FunctionKeypad  !=  NULL )
		delete  FunctionKeypad ;

// Libération des paramètres spécifiques à l'émulateur
	delete []  EmulatedKeys ;
	delete []  EmulatorSequences ;
	delete []  EmulatorColors ;
	delete []  SpecialCharacters ;
	delete []  ProgrammedKeys ;

	if  ( FileTransferFd  !=  -1 )
		close ( FileTransferFd ) ;

	if  ( PrintingWindow )
		delete  PrintingWindow ;
    }




/**************************************************************************/
/*                                                                        */
/*  TDisplayDevice :: ClearArea                                           */
/*      Remet à blanc une portion de l'écran.                             */
/*                                                                        */
/**************************************************************************/

void	TDisplayDevice :: ClearArea  ( int  x1, int  y1, int  x2, int  y2 )
   {
	for  ( int  i = y1 ; i <= y2 ; i ++ )
	   {
		for   ( int  j = x1 ; j <= x2 ; j ++ )
			ClearCharacter ( j, i ) ;
	    }
     }




/**************************************************************************/
/*                                                                        */
/*  TDisplayDevice :: ResetFont                                           */
/*      Permet de réinitialiser le device après avoir changé de police de */
/*      caractères.                                                       */
/*                                                                        */
/**************************************************************************/

void	TDisplayDevice :: ResetFont  ( BOOL  Recreate )
   {
	HFONT		Old ;


	GetDC ( ) ;

	if  ( Recreate )
	   {
		ManageCaret ( FALSE ) ;

		Old  = FontHandles [0] ;

		FontHandles [0] = CreateFont  ( SessionParameters -> FontHeight,
					   SessionParameters -> FontWidth,
					   0, 0,
					   FW_NORMAL,
					   0,
					   0,
					   0,
					   DEFAULT_CHARSET,
					   OUT_DEFAULT_PRECIS,
					   CLIP_DEFAULT_PRECIS,
					   DEFAULT_QUALITY,
					   FIXED_PITCH | FF_DONTCARE,
					   SessionParameters -> FontName ) ;


		SelectObject ( DCHandle, FontHandles [0] ) ;

		if  ( Old )
			DeleteObject ( Old ) ;

		Old = SpecialFontHandle ;

		SpecialFontHandle = CreateFont  ( SessionParameters -> FontHeight,
					   SessionParameters -> FontWidth,
					   0, 0,
					   FW_NORMAL,
					   0,
					   0,
					   0,
					   DEFAULT_CHARSET,
					   OUT_DEFAULT_PRECIS,
					   CLIP_DEFAULT_PRECIS,
					   DEFAULT_QUALITY,
					   FIXED_PITCH | FF_DONTCARE,
					   SpecialFontName ) ;

		if  ( Old )
			DeleteObject ( Old ) ;

	
		for  ( int  i = 1 ; i <= ( UNDERLINE | BOLD | ITALIC ) ; i ++ )
		   {
			Old = FontHandles [i] ;

			FontHandles [i] = CreateFont  ( SessionParameters -> FontHeight,
						   SessionParameters -> FontWidth,
						   0, 0,
						   ( i & BOLD ) ? FW_BOLD : FW_NORMAL,
						   ( i & ITALIC ),
						   ( i & UNDERLINE ),
						   0,
						   DEFAULT_CHARSET,
						   OUT_DEFAULT_PRECIS,
						   CLIP_DEFAULT_PRECIS,
						   DEFAULT_QUALITY,
						   FIXED_PITCH | FF_DONTCARE,
						   SessionParameters -> FontName ) ;

			if  ( Old )
				DeleteObject ( Old ) ;
		     }

	    

	// il faut calculer la taille réelle d'un caractère ;
		TEXTMETRIC		Metrics ;

		:: GetTextMetrics ( DCHandle, & Metrics ) ;

		CharacterHeight		= Metrics. tmHeight ;
		CharacterWidth		= Metrics. tmAveCharWidth ;
		VariablePitch		= ( Metrics. tmMaxCharWidth  !=
						Metrics. tmAveCharWidth ) ||
					  Metrics. tmPitchAndFamily & VARIABLE_PITCH ;

		ManageCaret ( TRUE ) ;
	    }
	else
		SelectObject ( DCHandle, FontHandles [0] ) ;

	:: SetTextCharacterExtra ( DCHandle, 0 ) ;


// Et la couleur des caractères
	:: SetTextColor ( DCHandle, SessionParameters -> ForegroundColor ) ;
	:: SetBkColor   ( DCHandle, SessionParameters -> BackgroundColor ) ;

	if  ( VariablePitch )
		:: SetBkMode  ( DCHandle, TRANSPARENT ) ;
	else
		:: SetBkMode ( DCHandle, OPAQUE ) ;


// Création de la fenêtre touches de fonctions
	if  ( FunctionKeypad  ==  NULL )
	   {
		FunctionKeypad	=  new  TFunctionKeypad ( this ) ;
		FunctionKeypad -> Create ( ) ;
	    }


// Réajuster l'écran
	FunctionKeypad -> ResizeWindow ( ) ;
	
	AdjustWindowSize ( ) ;
	UpdateScreen ( 0, 0, ScreenWidth - 1, ScreenHeight - 1 ) ;
	ManageCaret ( TRUE ) ;
	ReleaseDC ( ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TDisplayDevice :: ResetFont                                           */
/*      Permet de réinitialiser le device après avoir changé les couleurs */
/*      de texte et de fond.                                              */
/*                                                                        */
/**************************************************************************/

void	TDisplayDevice :: ResetColors ( void )
   {
	PreviousBackground = SessionParameters -> BackgroundColor ;
	PreviousForeground = SessionParameters -> ForegroundColor ;

	GetDC ( ) ;
	:: SetTextColor ( DCHandle, SessionParameters -> ForegroundColor ) ;
	:: SetBkColor   ( DCHandle, SessionParameters -> BackgroundColor ) ;

	for  ( int  i = 0 ; i < ScreenHeight ; i ++ )
	   {
		for  ( int  j = 0 ; j < ScreenWidth ; j ++ )
		    {
			ScreenMemory [i] [j]. BackgroundColor = PreviousBackground ;
			ScreenMemory [i] [j]. ForegroundColor = PreviousForeground ;
		     }
	    }

	if  ( BackgroundBrush )
		DeleteObject ( BackgroundBrush ) ;

	BackgroundBrush = CreateSolidBrush ( PreviousBackground ) ;	    
	ReleaseDC ( ) ;
    }
    


/**************************************************************************/
/*                                                                        */
/*   TDisplayDevice :: Scroll  :                                          */
/*      Effectue un scroll de la fenêtre.                                 */
/*      Le scroll s'effectue vers le haut si count est négatif.           */
/*      Le contenu de la fenêtre est remis à jour (UpdateWindow).         */
/*                                                                        */
/**************************************************************************/

void	TDisplayDevice :: Scroll  ( int  count, BOOL  clean )
   {
	TRect			Rect ;


	Rect. left   = Rect. top = 0 ;
	Rect. right  = ( ScreenWidth  * CharacterWidth  ) + 1 ;
	Rect. bottom = ( ScreenHeight * CharacterHeight ) + 1 ;

// Scrolling vers le haut
	if  ( count  <  0 )
	   {
		count   = -count ;

	// Initialiser à blanc la partie du bas libérée par le scrolling
		Origin = ( Origin + count ) % ScreenHeight ;

		if  ( clean )
			ClearArea ( 0, ScreenHeight - count, ScreenWidth - 1, ScreenHeight - 1 ) ;

	// Le scroll est plus couteux que la remise à jour de l'écran entier...
	// ScrollDC ( DCHandle, 0, - count * CharacterHeight, & Rect, & Rect,
	//	NULL, NULL ) ;

	// On remet à jour la partie du bas laissée vide par le défilement
		if  ( clean )
			UpdateScreen ( 0, 0, 	// ScreenHeight - count,  (pour le scroll)
				ScreenWidth - 1, ScreenHeight - 1 ) ;
	     }
// Scrolling vertical
	else
	   {
		Origin = ( Origin - count ) ;

		if  ( Origin < 0 )
			Origin = ScreenHeight - count ;

		ClearArea ( 0, 0, ScreenWidth - 1, count ) ;
		UpdateScreen ( 0, 0, ScreenWidth - 1, ScreenHeight - 1 ) ;
	     }

     }





/**************************************************************************/
/*                                                                        */
/*   TDisplayDevice :: InsertEmptyLine                                    */
/*      Insère une ligne vide à la position courante.                     */
/*      Il faut appeler UpdateWindow ensuite pour que le contenu soit     */
/*      remis à jour.                                                     */
/*                                                                        */
/**************************************************************************/

void	TDisplayDevice :: InsertEmptyLine  ( int  where )
   {
	register int		index1, index2 ;

	
	for  ( int  i = ScreenHeight - 1 ; i > where ; i -- )
	   {
		index1 = ( Origin + i ) % ScreenHeight ;
		index2 = ( Origin + i - 1 ) % ScreenHeight ;

		memcpy ( ScreenMemory [ index1 ], ScreenMemory [ index2 ],
			sizeof ( TScreenCharacter ) * ScreenWidth ) ;
	    }

	ClearArea ( 0, where, ScreenWidth - 1, where ) ;
     }
     



/**************************************************************************/
/*                                                                        */
/*  TDisplayDevice :: DeleteLine                                          */
/*      Supprime la ligne spécifiée, et décale les lignes du dessous      */
/*      vers le haut.                                                     */
/*                                                                        */
/**************************************************************************/

void	TDisplayDevice :: DeleteLine  ( int  where )
   {
	TScreenCharacter *	Deleted  =  ScreenMemory [ ( Origin + where ) % ScreenHeight ] ;
	register int		i, from, to ;

	
	ClearArea ( 0, where, ScreenWidth - 1, where ) ;


	for  ( i = where + 1 ; i < ScreenHeight ; i ++ )
	   {
		from  =  ( Origin + i )     % ScreenHeight ;
		to    =  ( Origin + i - 1 ) % ScreenHeight ;

		ScreenMemory [to] = ScreenMemory [from] ;
	    }

	ScreenMemory [ ( Origin + ScreenHeight - 1 ) % ScreenHeight ] = Deleted ;
	
     }
     


/**************************************************************************/
/*                                                                        */
/*  TDisplayDevice :: ManageCaret                                         */
/*      Crée / détruit le caret en cas de gain/perte de focus ou de       */
/*      destruction de la classe.                                         */
/*                                                                        */
/**************************************************************************/

void 	TDisplayDevice :: ManageCaret  ( BOOL  Active )
   {
	if  ( Active )
	   {
		TerminalWindowClient -> CreateCaret ( NULL, CharacterWidth, CharacterHeight ) ;
		TerminalWindowClient -> SetCaretPos ( CharacterWidth * CurrentColumn,
				CharacterHeight * CurrentLine ) ;
		TerminalWindowClient -> ShowCaret ( ) ;
	    }
	else
	   {
		TerminalWindowClient -> HideCaret ( ) ;
		TerminalWindowClient -> DestroyCaret ( ) ;
	    }
     }




/**************************************************************************/
/*                                                                        */
/*  TDisplayDevice :: TextOut                                             */
/*      Affiche le texte spécifié en tenant compte de la sélection        */
/*      éventuelle.                                                       */
/*                                                                        */
/**************************************************************************/

void  TDisplayDevice ::	TextOut	( HDC		hDC,
				  int		x,
				  int		y,
				  char *	line,
				  int		length,
				  BOOL		issemigraphic )
    {
	if  ( VariablePitch  ||  issemigraphic )
	   {
		register int		i, j, k ;
		register COLORREF       BkColor = GetBkColor ( hDC ) ;
		TRect			Rect ( x * CharacterWidth, y * CharacterHeight, 
							( x + length ) * CharacterWidth,
							( y + 1 ) * CharacterHeight ) ;



		if  ( BkColor  !=  StaticPreviousBackground )
		   {
			DeleteObject ( BackgroundBrush ) ;
			BackgroundBrush = CreateSolidBrush ( BkColor ) ;
			StaticPreviousBackground = BkColor ;
		    }

		FillRect ( hDC, & Rect, BackgroundBrush ) ;


		for  ( i = 0 ; i < length ; i ++ )
		   {
		// Centrer le caractère sur la largeur moyenne
			j = ( int ) GetTextExtent ( hDC, line + i, 1 ) ;
			k = ( CharacterWidth - j ) / 2 ;
			

			:: TextOut ( hDC, ( ( x + i ) * CharacterWidth ) + k,
				y * CharacterHeight, line + i, 1 ) ;
		     }
	    }
	else
	:: TextOut  ( hDC, x * CharacterWidth, y * CharacterHeight,
		line, length ) ;
     }



/**************************************************************************/
/*                                                                        */
/*   TDisplayDevice :: UpdateScreen                                       */
/*      Réaffiche les caractères dans le rectangle donné.                 */
/*      Remet la fenêtre à jour (UpdateWindow).                           */
/*                                                                        */
/**************************************************************************/

// Première forme, lorsque la fenêtre principale reçoit un WM_PAINT

void	TDisplayDevice :: UpdateScreen  (  TRect &  Rect )
   {
	register int	x1	=  Rect. left,
			x2	=  Rect. right,
			y1	=  Rect. top,
			y2 	=  Rect. bottom ;


// Convertir les coordonnées de pixels en caractères
	x1 /= CharacterWidth ;
	x2 /= CharacterWidth ;
	y1 /= CharacterHeight ;
	y2 /= CharacterHeight ;

	if  ( ! x2  ||  x2  >=  ScreenWidth )
		x2 = ScreenWidth - 1 ;
	if  ( ! y2  ||  y2  >=  ScreenHeight )
		y2 = ScreenHeight - 1 ;

	UpdateScreen ( x1, y1, x2, y2 ) ;
     }


// 2e forme, utilisée par la classe TDisplayDevice

void	TDisplayDevice :: UpdateScreen ( int  x1, int  y1, int  x2, int  y2 ) 
   {
	char			line [ 256 ] ;
	register char *		p ;
	register int		i, j, X, Y, length ;
	int			previous_attribute ;
	COLORREF		previous_background,
				previous_foreground ;
	TScreenCharacter *	Ch ;
	TRect			Rect ;


	
	GetDC ( ) ;
	TerminalWindowClient -> HideCaret ( ) ;
	p = line, length = 0 ;

// Le premier attribut
	Y = ( Origin + y1 ) % ScreenHeight ;
	previous_attribute   = ScreenMemory [Y] [x1]. Attributes ;
	previous_background  = ScreenMemory [Y] [x1]. BackgroundColor ;
	previous_foreground  = ScreenMemory [Y] [x1]. ForegroundColor ;

	SetFont ( ScreenMemory [Y] + x1 ) ;

	SetBkColor ( DCHandle, previous_background ) ;
	SetTextColor ( DCHandle, previous_foreground ) ;


	for  ( i = y1 ; i <= y2 ; i ++ )
	   {
		Y = ( Origin + i ) % ScreenHeight ;

		for  ( j = x1 ; j <= x2 ; j ++ )
		   {
			X = j ;
			Ch = & ScreenMemory [Y] [X] ;

			if  ( previous_attribute   !=  Ch -> Attributes  ||
			      previous_background  !=  Ch -> BackgroundColor  ||
			      previous_foreground  !=  Ch -> ForegroundColor )
			   {
				if  ( length )
				  {
					* p = 0 ;
					TextOut ( DCHandle, X - length, i, line, length,
						previous_attribute & SPECIAL ) ;
					p = line, length = 0 ;
				   }

				SetFont ( Ch ) ;

				if  ( previous_background  !=  Ch -> BackgroundColor )
					SetBkColor ( DCHandle, Ch -> BackgroundColor ) ;
				if  ( previous_foreground  !=  Ch -> ForegroundColor ) 
					SetTextColor ( DCHandle, Ch -> ForegroundColor ) ;

				previous_attribute  =  Ch -> Attributes ;
				previous_background =  Ch -> BackgroundColor ;
				previous_foreground =  Ch -> ForegroundColor ;
			     }

			* p ++ = Ch -> Character ;
			length ++ ;
		     }

	// Ecrire les éventuels caractères restant
		* p = 0 ;
		TextOut ( DCHandle, X - length + 1, i, line, length,
				previous_attribute & SPECIAL ) ;
		p = line, length = 0 ;
	    }

	

	if  ( InSelection )
		StopSelection ( ) ;

	ReleaseDC ( ) ;
	TerminalWindowClient -> UpdateWindow ( ) ;		
	TerminalWindowClient -> ShowCaret ( ) ;
    }





/**************************************************************************/
/*                                                                        */
/*   TDisplayDevice :: LoadKeys                                           */
/*      Charge les touches de fonctions émulées.                          */
/*                                                                        */
/**************************************************************************/

void    TDisplayDevice :: LoadKeys ( void )
   {
	register int		i, c ;
	char			buffer    [ MAX_PARAMETER_LENGTH ],
				processed [ MAX_PARAMETER_LENGTH ] ;
	char *			p,
	     *			q ;
	

	for  ( i = 0 ; EmulatedKeys [i]. KeyName  !=  NULL ; i ++ )
	    {
		GetPrivateProfileString ( "Keys",
			EmulatedKeys [i]. KeyName, "",
				buffer, MAX_PARAMETER_LENGTH,
					SessionParameters -> Emulation ) ;
			
		p = buffer, q = processed ;
		
		while  ( ( c = NextCharacter ( & p ) )  !=  0 )
			* q ++ = c ;
		* q = 0 ;
					
		EmulatedKeys [i]. KeyCode = strdup ( processed ) ;
	     }
     }



/**************************************************************************/
/*                                                                        */
/*   TDisplayDevice :: LoadSequences :                                    */
/*      Charge les séquences d'échappement reconnues.                     */
/*                                                                        */
/**************************************************************************/

void    TDisplayDevice :: LoadSequences ( void )
   {
	register int		i ;
	char			buffer [ MAX_PARAMETER_LENGTH ],
				temp   [ MAX_PARAMETER_LENGTH ] ;
	char *			p,
	     *			q ;
	register char 		c ;


	for  ( i = 0 ; EmulatorSequences [i]. SequenceName  !=  NULL ; i ++ )
	    {
		GetPrivateProfileString ( "Escapes", 
			EmulatorSequences [i]. SequenceName, "",
				temp, MAX_PARAMETER_LENGTH,
					SessionParameters -> Emulation ) ;

		p = temp, q = buffer ;

		while  ( ( c = NextCharacter ( & p ) )  !=  0 )
		   {
			if  ( c  !=  '%' )
				* q ++ = c ;
			else
			    {
			    // Lorsqu'une séquence d'échappement contient une
			    // partie variable (par exemple, %n pour désigner 
			    // un nombre codé sous forme ascii), cette partie 
			    // est remplacée de manière interne par un unique
			    // caractère, de manière à faciliter les interprétations.
			    // Ici, %n est remplacée par le caractère ascii 1.
				switch ( * p )
				   {
					case	'n' :	// Tt nombre en ascii
					case	'N' :
						* q ++ = ANY_ASCII_NUMBER ;
						break ;

					case	'l' :   // Longueur d'une chaîne
					case	'L' :
						* q ++ = ANY_STRING_LENGTH ;
						break ;
					
					case	's' :	// Chaîne
					case	'S' :
						* q ++ = ANY_STRING ;
						break ;

					case	0 :
					case	'%' :
						* q ++ = '%' ;
						break ;

					default :
						* q ++ = * p ;
				      }

				if  ( * p )
					p ++ ;
			    }
		    }

		* q = 0 ;
		EmulatorSequences [i]. Sequence = strdup ( buffer ) ;
	     }
     }







/**************************************************************************/
/*                                                                        */
/*   LoadSettings :                                                       */
/*      Charge les options concernant le terminal (lignes, colonnes, etc.)*/
/*                                                                        */
/**************************************************************************/

void  TDisplayDevice :: LoadSettings  ( void )
   {
	char		buffer [ MAX_PARAMETER_LENGTH ] ;


	GetPrivateProfileString ( "Settings", "Columns", "80",
				buffer, MAX_PARAMETER_LENGTH,
					SessionParameters -> Emulation ) ;

	ScreenWidth = atoi ( buffer ) ;

	if  ( ScreenWidth < 10  ||  ScreenWidth > 255 )
		ScreenWidth = 80 ;

	GetPrivateProfileString ( "Settings", "Lines", "25",
				buffer, MAX_PARAMETER_LENGTH,
					SessionParameters -> Emulation ) ;

	ScreenHeight = atoi ( buffer ) ;

	if  ( ScreenHeight < 2  ||  ScreenHeight > 80 )
		ScreenHeight = 25 ;

	GetPrivateProfileString ( "Settings", "GraphicFont", "MS LineDraw",
				SpecialFontName, MAX_PARAMETER_LENGTH,
					SessionParameters -> Emulation ) ;

	SpecialFontHeight = SessionParameters -> FontHeight ;
    }



/**************************************************************************/
/*                                                                        */
/*  TDisplayDevice :: LoadSpecialCharacters :                             */
/*      Charge la table des caractères graphiques.                        */
/*                                                                        */
/**************************************************************************/

void    TDisplayDevice :: LoadSpecialCharacters ( void )
   {
	register int		i ;
	char			buffer [ MAX_PARAMETER_LENGTH ] ;
	char *			p ;


	for  ( i = 0 ; SpecialCharacters [i]. CharacterName  !=  NULL ; i ++ )
	    {
		GetPrivateProfileString ( "Special Characters",
			SpecialCharacters [i]. CharacterName, "",
				buffer, MAX_PARAMETER_LENGTH,
					SessionParameters -> Emulation ) ;


		p = buffer ;
		while  ( * p  &&  isspace ( * p ) )
			p ++ ;

		SpecialCharacters [i]. HostValue  = NextCharacter ( & p ) ;

		while  ( * p  && ( isspace ( * p ) ||  * p  ==  ',' ) )
			p ++ ;

		SpecialCharacters [i]. LocalValue = NextCharacter ( & p ) ;
	     }
     }




/**************************************************************************/
/*                                                                        */
/*  TDisplayDevice :: ProcessKey                                          */
/*      Recherche si la touche de fonction spécifiée fait partie de celles*/
/*      gérées par l'émulateur.                                           */
/*      Le paramètre Flags sert pour les touches de fonction qui ne sont  */
/*      interprétées qu'en combinaison avec une touche SHIFT, CONTROL ou  */
/*      ALT.                                                              */
/*      La recherche s'effectue dans la table EmulatedKeys, dont les      */
/*      séquences d'échappement ont été lues depuis le fichier désigné    */
/*      par le mot-clé "Emulation" du fichier TERMINAL.INI.               */
/*                                                                        */
/**************************************************************************/

BOOL    TDisplayDevice :: ProcessKey  ( UINT  KeyCode, UINT  Flags )
   {
	register int		i ;
	int			KeyFlags ;
	register char *		p ;
	register int		size ;


	for  ( i = 0 ; EmulatedKeys [i]. KeyName  !=  NULL ; i ++ )
	    {
		if  ( KeyCode  !=   EmulatedKeys [i]. VirtualKeyCode )
			continue ;

		KeyFlags = EmulatedKeys [i]. VirtualKeyFlags ;

		if  ( ( KeyFlags & ~REGULARCHAR )  ==  ( Flags & ~REGULARCHAR ) )
		   {
			if  ( Flags  ==  KeyFlags )
			   {
				p = EmulatedKeys [i]. KeyCode ;
				
				for  ( int  j = 0 ; ProgrammedKeys [j]. VirtualKeyCode  !=  0xFFFF ; j ++ )
				   {
					if  ( EmulatedKeys [i]. VirtualKeyCode  ==  
							ProgrammedKeys [j]. VirtualKeyCode  &&
								ProgrammedKeys [j]. StringToSend [0] )
					   {
						p = ProgrammedKeys [j]. StringToSend ;
						break ;
					    }
				     }
				
				size = strlen ( p ) ;
				Connection -> SendData ( p, size ) ;

				if  ( SessionParameters -> LocalEcho )
					Write ( p , size ) ;
			     }					
			return ( TRUE ) ;
		    }
	      }


	return ( FALSE ) ;
    }





/**************************************************************************/
/*                                                                        */
/*  TDisplayDevice :: AdjustWindowSize                                    */
/*      Ajuste la taille de la fenêtre de manière à avoir ScreenHeight    */
/*      lignes et ScreenWidth colonnes, dans la police courante.          */
/*                                                                        */
/**************************************************************************/

void	TDisplayDevice :: AdjustWindowSize  ( void )
   {
	int	Width, Height ;
	TRect	Rect ;

	GetWindowSize ( Width, Height ) ;
	TerminalWindow -> StatusBar -> GetWindowRect ( Rect ) ;
	Height += Rect. bottom - Rect. top - 1 ;
	Width  += FunctionKeypad -> GetWidth ( ) ;

	TerminalWindow -> SetWindowPos ( NULL, 0, 0, Width, Height, 
		SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE ) ;
	TerminalWindow -> UpdateWindow ( ) ;
    }





/**************************************************************************/
/*                                                                        */
/*  TDisplayDevice :: FindMatch                                           */
/*      Tente de faire correspondre la séquence donnée par 'seq' avec     */
/*      l'une des séquences interprétées.                                 */
/*      Retourne :                                                        */
/*                                                                        */
/*      NoMatch         si aucune séquence ne correspond.                 */
/*      PartialMatch    si la séquence spécifiée n'est pas complète.      */
/*      FullMatch       si la séquence correspond.                        */
/*                                                                        */
/*      Dans les deux derniers cas, le paramètre 'index' contiendra       */
/*      l'indice, dans la table EmulatorSequences, de la séquence         */
/*      correspondante.                                                   */
/*      Cette fonction est pour l'instant assez intelligente pour         */
/*      reconnaître une séquence d'échappement variable (du style :       */
/*      "positionner le curseur en x,y) mais elle ne sait pas encore      */
/*      gérer le cas où deux séquences commencent par la même suite de    */
/*      caractères.                                                       */
/*                                                                        */
/**************************************************************************/

TDisplayDevice :: TSequenceMatch  TDisplayDevice :: FindMatch ( char *	seq,
								int &   index )
   {
	register char *		p,
		      *		q ;
	register int		string_length 	=  -1,
				i, j ;


// Pour chaque entrée de la table des séquences...
	for  ( i = 0 ; EmulatorSequences [i]. Action  !=  UndefinedAction ; i ++ )
	   {
		if  ( EmulatorSequences [i]. Action  ==  ZeroedAction )
			continue ;

		p = seq ;
		q = EmulatorSequences [i]. Sequence ;

	// L'algorithme qui suit fait n'importe quoi si la longueur de la
	// séquence prédéfinie est nulle
		if  ( ! strlen ( q ) )
			continue ;

	// Faire la comparaison
		while  ( * p  &&  * q )
		   {
			switch ( * q )
			   {
			// On s'attend à un nombre codé sous forme ascii :
			// il faut sauter tous les chiffres de la séquence
			// passée en paramètre
				case	ANY_ASCII_NUMBER :
					while  ( isdigit ( * p ) )
						p ++ ;
					p -- ;
					break ;

			// On s'attend à trouver une chaîne de caractères
			// quelconque : on récupère le maximum jusqu'à
			// trouver un caractère de la séquence égal au
			// suivant
			// Note : il faut absolument que la chaîne soit
			// suffixée par quelque chose pour que ça marche.
			// En outre, il ne faut pas qu'elle soit suivie
			// d'une combinaison variable
				case	ANY_STRING :
					if  ( string_length >= 0 )
					   {
						if  ( strlen ( p )  <  string_length )
							return ( PartialMatch ) ;
						p += string_length - 1 ;
					    }
					else
						return ( NoMatch ) ;
					break ;


			// Longueur d'une chaîne devant suivre dans la
			// séquence
				case	ANY_STRING_LENGTH :
					j = 0 ;
					while  ( isdigit ( * p ) )
					   {
						j = ( j * 10 ) + ( * p - '0' ) ;
						p ++ ;
					    }

					p -- ;
					string_length = j ;
					break ;
					
			   
				default :
					if  ( * p  !=  * q )
						goto  ShootAgain ;
			     }

			p ++, q ++ ;
		     }


	// Si *p et *q sont nuls, la comparaison est complète
	// Dans le cas contraire, * q ne pourra jamais être nul.
	// Si * p est nul, la correspondance n'est que partielle, il faudra
	// donc rappeler FindMatch avec le caractère suivant...
		index = i ;

		if  ( ! * q )
			return ( FullMatch ) ;
		else
			return ( PartialMatch ) ;

ShootAgain :
	   }

	return ( NoMatch ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TDisplayDevice :: GetSequenceParameter                                */
/*      Pour la séquence d'index donné, retourne dans 'result' le         */
/*      paramètre variable numéro 'number' en le prenant de la séquence   */
/*      d'échappement indiquée par 'received'.                            */
/*      Ne touche pas à 'result' en cas de probleme.                      */
/*      La numérotation des paramètres commence à 1.                      */
/*                                                                        */
/**************************************************************************/


void   TDisplayDevice :: GetSequenceParameter (	int   	seqindex,
						char *	received,
						int   	number,
						int   	string_length,
						char *	result )
    {
	char			buffer [ 256 ] ;
	register char *		p 	= received,
		      *		q 	= EmulatorSequences [ seqindex ]. Sequence ;
	register char *		z ;
	register int		count   = 0 ;
	register int		prevstr = 0 ;


// L'algorithme qui suit fait n'importe quoi si la longueur de la
// séquence prédéfinie est nulle
	if  ( ! strlen ( q ) )
		return ;

// Faire la comparaison
	while  ( * p  &&  * q )
	   {
		switch ( * q )
		   {

		// On tombe sur le %n
			case	ANY_ASCII_NUMBER :
			case	ANY_STRING_LENGTH :
				count ++ ;		// Un paramètre de plus
				z = buffer ;

				while  ( isdigit ( * p ) )
					* z ++ = * p ++ ;
				* z = 0 ;
				p -- ;

				if  ( * q  ==  ANY_STRING_LENGTH )
					prevstr = atoi ( buffer ) ;
				else
					prevstr = 0 ;
				break ;
				

			case	ANY_STRING :
				count ++ ;
				
				if  ( count  ==  number )
				   {
					while  ( * p  &&  string_length -- )
						* result ++ = * p ++ ;
					* result = 0 ;
					return ;
				     }
				else			// Sauter la chaîne
				   {
					while  ( * p  &&  prevstr -- )
						p ++ ;
					p -- ;
				     }
				break ;
				
				
			default :
				if  ( * p  !=  * q )	// Séquence erronée
				   {
					* result = 0 ;
					return ;
				    }
		     }

		p ++, q ++ ;
	    }
    }




     
void	TDisplayDevice :: GetSequenceParameter  ( int	   SequenceIndex,
						  char *   SequenceReceived,
						  int	   Number,
						  int &    Result )
    {
	char			buffer  [ MAX_PARAMETER_LENGTH ] ;
	register char *		p 	= SequenceReceived,
		      *		q 	= EmulatorSequences [ SequenceIndex ]. Sequence,
		      *		z ;
	register int		count   = 0 ;
	register int		string_length 	=  0 ;


// L'algorithme qui suit fait n'importe quoi si la longueur de la
// séquence prédéfinie est nulle
	if  ( ! strlen ( q ) )
		return ;

// Faire la comparaison
	while  ( * p  &&  * q )
	   {
		switch ( * q )
		   {

		// On tombe sur le %n
			case	ANY_ASCII_NUMBER :
			case	ANY_STRING_LENGTH :
				count ++ ;		// Un paramètre de plus
				z = buffer ;		// On va récupérer sa valeur

				while  ( isdigit ( * p ) )
					* z ++  =  * p ++ ;
				p -- ;

			// Si c'est le bon paramètre, on retourne sa valeur
				if  ( count  ==  Number )
				   {
					* z = 0 ;
					Result = atoi ( buffer ) ;
					return ;
				    }
				else if  ( * q  ==  ANY_STRING_LENGTH )
					string_length = atoi ( buffer ) ;
				else
					string_length = 0 ;
				break ;


		// Une chaîne de caractères à sauter
			case	ANY_STRING :
				if  ( string_length )
				   {
					count ++ ;
				
					if  ( count  ==  Number )	// erreur sur type de paramètre
						break ;

					while  ( * p  &&  string_length -- )
						p ++ ;
					p -- ;
				     }
				break ;

			default :
				if  ( * p  !=  * q )	// Séquence erronée
					return ;
		     }

		p ++, q ++ ;
	    }
    }





/**************************************************************************/
/*                                                                        */
/*   TDisplayDevice :: WriteCharacter                                     */
/*      Ecrit un caractère à la position courante, sans interprétation.   */
/*      Il faut faire un UpdateWindow ensuite.                            */
/*                                                                        */
/**************************************************************************/

void	TDisplayDevice :: WriteCharacter  ( char  c )
   {
	TScreenCharacter	Ch ;
	register int		j ;
	BOOL			Found ;

   
// Caractère de contrôle : il faut le remplacer par '^quelquechose'
	if  ( ( unsigned ) c < 32 )
	   {
		WriteCharacter ( '^' ) ;
		WriteCharacter ( c + '@' ) ;
	    }
	else
	   {
		register int	where ;


		where 		=  ( Origin + CurrentLine ) % ScreenHeight ;
		Ch. Character	=  c ;
		Ch. Attributes  =  PreviousAttributes ;

		Ch. BackgroundColor = PreviousBackground ;
		Ch. ForegroundColor = PreviousForeground ;


	// Caractère semi-graphique : remplacer par sa valeur locale
		if  ( InSpecialSequence )
		   {
			Ch. Attributes  |=  SPECIAL ;

			for  ( j = 0, Found = FALSE ;
				SpecialCharacters [j]. CharacterName != NULL ;
					j ++ )
			   {
				if  ( Ch. Character   ==  SpecialCharacters [j]. HostValue )
				   {
					Ch. Character = SpecialCharacters [j]. LocalValue ;
					Found = TRUE ;
					break ;
				    }
			    }

			if  ( ! Found ) 
				Ch. Character = '?' ;
		    }
		else
			Ch. Attributes  &= ~SPECIAL ;

		ScreenMemory [ where ] [ CurrentColumn ] = Ch ;

		UpdateScreen ( CurrentColumn, CurrentLine, CurrentColumn, CurrentLine ) ;

		if  ( :: GetFocus ( )  ==  TerminalWindowClient -> HWindow )
			TerminalWindowClient -> SetCaretPos (
				( min ( CurrentColumn + 1, ScreenWidth - 1 ) ) * CharacterWidth,
					CurrentLine * CharacterHeight ) ;

		if  ( CurrentColumn + 1  >=  ScreenWidth )
		    {
			if  ( CurrentLine + 1  >=  ScreenHeight )
				Scroll ( -1 ) ;
			else
				CurrentLine ++ ;

			CurrentColumn = 0 ;
		      }
		else
			CurrentColumn ++ ;
	    }
    }




/**************************************************************************/
/*                                                                        */
/*   TDisplayDevice :: WriteString                                        */
/*      Ecrit une chaîne de caractères, en interprétant les séquences     */
/*      d'échappement.                                                    */
/*                                                                        */
/**************************************************************************/

void	TDisplayDevice :: Write  ( char *   buffer, int  size )
   {
	char *				p  		=  buffer ;
	register int			c ;
	register int			PreviousLine,
					PreviousColumn ;
	register FILE *			fp      	=  Configuration -> LogFileFP ;


	if  ( fp  !=  NULL )
	   {
		for  ( PreviousLine = size; PreviousLine ; PreviousLine-- )
		   {
			if  ( * p  <  ' '  && ! isspace ( * p ) )
			   {
				fputc ( '^', fp ) ;
				fputc ( ( * p ) + '@', fp ) ;
			    }
			else
				fputc ( * p, fp ) ;

			p ++ ;
		     }

		p = buffer ;
	      }




// Pas de caret durant les affichages
	TerminalWindowClient -> HideCaret ( ) ;


// Ecriture des caractères reçus
// Note : il eut été possible, pour optimiser, de compter le nombre de
// caractères newline présents dans la séquence, et d'effectuer le scrolling
// éventuel avant d'écrire les caractères. Malheureusement, faire cela
// provoque une impression de 'tressautement' lorsqu'on fait défiler une
// liste à l'écran
	while  (  size -- )
	   {
		c = * p ++ ;
		PreviousLine   = CurrentLine ;
		PreviousColumn = CurrentColumn ;

		if  ( Write ( c ) )
		   {
			Connection -> FlushInput ( ) ;
			Connection -> FlushOutput ( ) ;
			break ;
		     }

	// Remettre à jour la position du curseur si celle-ci a changé
		if  ( PreviousLine    !=  CurrentLine  ||
		      PreviousColumn  !=  CurrentColumn )
		   {
			if  ( :: GetFocus ( )  ==  TerminalWindowClient -> HWindow )
				TerminalWindowClient -> SetCaretPos (
					CurrentColumn * CharacterWidth,
						CurrentLine * CharacterHeight ) ;
		    }
	    }
	    

// Mettre à jour la fenêtre
	TerminalWindowClient -> UpdateWindow ( ) ;


// Réafficher le caret
	TerminalWindowClient -> ShowCaret ( ) ;
     }






/**************************************************************************/
/*                                                                        */
/*   TDisplayDevice :: Write                                              */
/*      Ecrit un caractère à la position courante.                        */
/*                                                                        */
/**************************************************************************/

int	TDisplayDevice :: Write  ( char   c )
   {
	char				strparam  [ MAX_PARAMETER_LENGTH ],
					strparam2 [ MAX_PARAMETER_LENGTH ],
					b      [2]      =  " " ;
	register int			i, j, where ;
	int				label, length ;
	register char *			p ;
	int				index, color ;
	TInputMessage :: TMessageState  message_state ;


// Escape : c'est soit le début/la fin d'un emssage, soit un caractère normal
	if  ( c  == 27 )
		InMessageReceive = FALSE ;

// Caractère de synchronisation : on remet les pendules à l'heure
	else if  ( InMessageReceive  &&  c  ==  SYNC_CHAR )
	   {
		if  ( ! StaticGotSync )
		   {
			StaticGotSync = TRUE ;
			InputMessage -> Zero ( ) ;
			InMessageReceive = FALSE ;
		    }

		return ( FALSE ) ;
	    }

// En cours de réception d'un message : ajouter le caractère au message
	else if  ( InMessageReceive )
	   {
		message_state = InputMessage -> GetState ( ) ;

	// N'ajouter le caractère que si le message n'est pas complet
		switch ( message_state )
		   {
			case	TInputMessage :: Zeroed :
			case	TInputMessage :: InFields :
			case	TInputMessage :: InData :
				InputMessage -> Add ( c ) ;
				return ( FALSE ) ;

		// Message complet ou erreur
			default :
				InputMessage -> Zero ( ) ;
				InMessageReceive = FALSE ;
				Connection -> Synchronize ( ) ;
				return ( TRUE ) ;
		     }
	      }


// Convertir le caractère dans le jeu de l'hôte, sauf pour les impressions :
// dans ce cas, les données viennent de la page de code 437 sur une imprimante
// en page de code 437
	if  ( ! InPrintMode )
		HostToLocalCharSet ( c ) ;
	StaticGotSync = FALSE ;


// Effacer la sélection s'il y en a une
	if  ( InSelection )
	   {
		StopSelection ( ) ;
		InSelection = FALSE ;
	    }


// Si on est en mode transparent, il n'y a juste qu'à afficher le caractère
	if  ( SessionParameters -> TransparentMode )
	   {
		WriteCharacter ( c ) ;

		if  ( ! InMessageReceive )
			return ( FALSE ) ;
	    }


// On ajoute le caractère au buffer et on vérifie s'il s'agit d'une séquence
// d'échappement
	* b = c ;
	strcat ( StaticBuffer, b ) ;
	TSequenceMatch		status  =  FindMatch ( StaticBuffer, index ) ;

	switch ( status )
	   {
		case	NoMatch :	// Ce n'est pas une séquence d'échappement
			// Impression "multiplexée" d'un fichier depuis Unix.
MultiplexPrint :
			if  ( InPrintMode )
			   {
				if  ( fputc ( * StaticBuffer, PrinterFP )  !=  * StaticBuffer )
					goto  CancelPrint ;

				if  ( PrintingWindow )
					PrintingWindow -> AnimateSlider ( ) ;
			     }
			else
				WriteCharacter ( * StaticBuffer ) ;
			strcpy ( StaticBuffer, StaticBuffer + 1 ) ;
				

		case	PartialMatch :	// C'est peut-être une séquence d'échappement
			return ( FALSE ) ;

		case	FullMatch :	// C'en est une...
			if  ( InPrintMode )
			   {
				if  ( EmulatorSequences [ index ]. Action != BulkPrintEndAction  &&
				      EmulatorSequences [ index ]. Action != FramePrintEndAction )
					goto  MultiplexPrint ;
			    }
			break ;
	     }


// On a trouvé une séquence : il faut l'interpréter
	switch  ( EmulatorSequences [ index ]. Action )
	   {

	// Caractère BELL : On sonne les cloches
		case	BellAction :
			if  ( Configuration -> Terminal ( ). AlarmBeep  ==  TRUE )
				MessageBeep  ( MB_ICONEXCLAMATION ) ;
			else
			   {
				register int  i ;

				for ( i = 0 ; i < 5 ; i ++ )
				   {
					TerminalWindow -> FlashWindow ( TRUE ) ;
					TerminalWindow -> FlashWindow ( FALSE ) ;
				    }
			    }
			break ;


	// Tabulation
		case	TabAction :
		// Tabulations non remplacées par des espaces
			i = ( CurrentColumn + 8 ) / 8 ;
			CurrentColumn = i * 8 ;

			if  ( CurrentColumn >= ScreenWidth )
				CurrentColumn = ScreenWidth - 1 ;
			break ;


	// Tabulation arrière
		case	BackTabAction :
			if  ( CurrentColumn % 8  ||  ! i )
				i = CurrentColumn / 8 ;
			else
				i = ( CurrentColumn - 1 ) / 8 ;
			CurrentColumn = i * 8 ;

			if  ( CurrentColumn >= ScreenWidth )
				CurrentColumn = ScreenWidth - 1 ;
			break ;


	// Backspace : On efface le caractère à gauche du curseur
	// (c'est Unix qui s'en charge normalement, nous, on n'a juste qu'à
	// changer la position du curseur)
		case	BackspaceAction :
			if  ( CurrentColumn )
				CurrentColumn -- ;
			break ;


	// Carriage return : retour en début de ligne
		case	CarriageReturnAction :
			CurrentColumn = 0 ;
			break ;


	// Newline : Passage à la ligne suivante
		case	NewlineAction :
			if  ( CurrentLine + 1  >=  ScreenHeight )
				Scroll ( -1 ) ;
			else
				CurrentLine ++ ;
			break ;


	// Clear : effacement de tout l'écran
		case	ClearAction :
			ClearArea ( 0, 0, ScreenWidth - 1, ScreenHeight - 1 ) ;
			UpdateScreen ( 0, 0, ScreenWidth - 1, ScreenHeight - 1 ) ;
			CurrentLine = CurrentColumn = 0 ;
			break ;


	// Effacement jusqu'en fin de ligne
		case	ClearToEOLAction :
			for  ( i = CurrentColumn ; i < ScreenWidth ; i ++ )
				ClearCharacter  ( i, CurrentLine ) ;
			UpdateScreen ( CurrentColumn, CurrentLine,
				ScreenWidth - 1, CurrentLine ) ;
			break ;


	// Effacement de la position courante à la fin de l'écran
		case	ClearToEODAction :
			for  ( i = CurrentColumn ; i < ScreenWidth ; i ++ )
				ClearCharacter ( i, CurrentLine ) ;

			for  ( i = CurrentLine + 1 ; i < ScreenHeight ; i ++ )
			   {
				for  ( j = 0 ; j < ScreenWidth ; j ++ )
					ClearCharacter ( j, i ) ;
			    }

			UpdateScreen ( 0, CurrentLine, ScreenWidth - 1,
				ScreenHeight - 1 ) ;
			break ;
				

	// Curseur à gauche
		case	CursorLeftAction :
			if  ( CurrentColumn )
				CurrentColumn -- ;
			break ;

	// Curseur à droite
		case	CursorRightAction :
			if  ( CurrentColumn + 1  <  ScreenWidth )
				CurrentColumn ++ ;
			break ;

	// Cursor en haut
		case	CursorUpAction :
			if ( CurrentLine )
				CurrentLine -- ;
			break ;

	// Curseur en bas
		case	CursorDownAction :
			if  ( CurrentLine + 1  <  ScreenHeight )
				CurrentLine ++ ;
			break ;

	// Curseur en haut à gauche de l'écran
		case	CursorHomeAction :
			CurrentLine = CurrentColumn = 0 ;
			break ;

	// Positionnement du curseur en x,y
		case	GotoXYAction :
			GetSequenceParameter ( index, StaticBuffer, 1, i ) ;
			GetSequenceParameter ( index, StaticBuffer, 2, j ) ;

			i --, j -- ;

		// Paranoia
			if  ( i >= 0  &&  i < ScreenHeight )
				CurrentLine = i ;
			if  ( j >= 0  &&  j < ScreenWidth )
				CurrentColumn = j ;
			break ;


	// Insérer un caractère à la position courante
		case	InsertCharacterAction :
			where = ( Origin + CurrentLine ) % ScreenHeight ;

			for  ( i = ScreenWidth - 1 ; i > CurrentColumn ; i -- )
				ScreenMemory [ where ] [ i ] =
					ScreenMemory [ where ] [ i - 1 ] ;

			ScreenMemory [ where ] [ CurrentColumn ]. Character = ' ' ;

			UpdateScreen ( CurrentColumn, CurrentLine, ScreenWidth - 1,
				CurrentLine ) ;
			break ;


	// Supprimer le caractère à la position courante
		case	DeleteCharacterAction :
			where = ( Origin + CurrentLine ) % ScreenHeight ;

			for  ( i = CurrentColumn ; i < ScreenWidth - 1 ; i ++ )
				ScreenMemory [ where ] [ i ] =
					ScreenMemory [ where ] [ i + 1 ] ;
			ScreenMemory [ where ] [ ScreenWidth - 1 ]. Character = ' ' ;

			UpdateScreen ( CurrentColumn, CurrentLine, ScreenWidth - 1,
				CurrentLine ) ;
			break ;


	// Blink : on passe ne italique
		case	EnterBlinkAction :
			PreviousAttributes  |=  ITALIC ;
			SetFont ( ) ;
			break ;


	// Bold : on passe en gras
		case	EnterBoldAction :
			PreviousAttributes |= BOLD ;
			SetFont ( ) ;
			break ;


	// Reverse : Passage en inversion video
		case	StartStandoutAction :
		case	EnterReverseAction :
			SetReverse ( TRUE ) ;
			SetFont ( ) ;
			break ;

	// Passage en souligné
		case	StartUnderlineAction :
			PreviousAttributes |= UNDERLINE ;
			SetFont ( ) ;
			break ;

	// Arrêt du mode souligné
		case	StopUnderlineAction :
			SetReverse ( FALSE ) ;
			SetFont ( ) ;
			PreviousAttributes = 0 ;
			break ;

	// Arrêt de tous les attributs
		case	AttributesOffAction :
			SetReverse ( FALSE ) ;
			PreviousAttributes = 0 ;              
			SetFont ( ) ;
			break ;

	// Standout : Arrêt de la surbrillance
		case	StopStandoutAction :
			SetReverse ( FALSE ) ;
			PreviousAttributes = 0 ;   
			SetFont ( ) ;
			break ;

	// Définir la couleur de texte
		case	SetForegroundAction :
			color = -1 ;
			GetSequenceParameter ( index, StaticBuffer, 1, color ) ;

			if  ( color  !=  -1 )
			   {
				GetDC ( ) ; 
	
				if  ( Reverse )
				   {
					PreviousBackground = EmulatorColors [ color % MAX_COLORS ] ;
					SetBkColor ( DCHandle, PreviousBackground ) ;
				    }
				else
				   {
					PreviousForeground = EmulatorColors [ color % MAX_COLORS ] ;
					SetTextColor ( DCHandle, PreviousForeground ) ;
				    }

				ReleaseDC ( ) ;
			    }
			break ;

	// Définir la couleur de fond
		case	SetBackgroundAction :
			color = -1 ;
			GetSequenceParameter ( index, StaticBuffer, 1, color ) ;

			if  ( color  !=  -1 )
			   {
				GetDC ( ) ;

				if  ( Reverse )
				   {
					PreviousForeground = EmulatorColors [ color % MAX_COLORS ] ;
					SetTextColor ( DCHandle, PreviousForeground ) ;
				    }
				else
				   {
					PreviousBackground = EmulatorColors [ color % MAX_COLORS ] ;
					SetBkColor ( DCHandle, PreviousBackground ) ;
				    }

				ReleaseDC ( ) ;
			    }
			break ;


	// Définir la couleur de texte et de fond
		case	SetColorAction :
			color = -1 ;
			GetSequenceParameter ( index, StaticBuffer, 1, color ) ;
			GetDC ( ) ;

			if  ( color  !=  -1 )
			   {
				PreviousForeground = EmulatorColors [ color % MAX_COLORS ] ;
				SetTextColor ( DCHandle, PreviousForeground ) ;
			    }

			color = -1 ;
			GetSequenceParameter ( index, StaticBuffer, 2, color ) ;

			if  ( color  !=  -1 )
			   {
				PreviousBackground = EmulatorColors [ color % MAX_COLORS ] ;
				SetBkColor ( DCHandle, PreviousBackground ) ;
			     }

			ReleaseDC ( ) ;			
			break ;                                                           


	// Insérer une ligne blanche
		case	InsertLineAction :
			InsertEmptyLine ( CurrentLine ) ;
			UpdateScreen ( 0, CurrentLine, ScreenWidth - 1,
				ScreenHeight - 1 ) ;
			break ;


	// Effacer une ligne
		case	DeleteLineAction :
			DeleteLine ( CurrentLine ) ;
			UpdateScreen ( 0, CurrentLine, ScreenWidth - 1,
				ScreenHeight - 1 ) ;
			break ;


	// Scroll up
		case	ScrollUpAction :
			Scroll ( -1 ) ;
			break ;


	// Scroll down
		case	ScrollDownAction :
			Scroll ( 1 ) ;
			break ;

	// Gestion des caractères spéciaux
		case	StartACSAction :
			InSpecialSequence    = TRUE ;
			PreviousAttributes  |=  SPECIAL ;
			break ;

		case	StopACSAction :
			InSpecialSequence    = FALSE ;
			PreviousAttributes  &= ~SPECIAL ;
			break ;


	// Gestion des touches de fonction
		case	SetFKStringAction :
			GetSequenceParameter ( index, StaticBuffer, 1, label ) ;
			GetSequenceParameter ( index, StaticBuffer, 2, length ) ;
			GetSequenceParameter ( index, StaticBuffer, 3, length, strparam ) ;

			if  ( label  >  0  &&  label  <=  MAX_FUNCTION_KEYS )
				strcpy ( ProgrammedKeys [ label - 1 ]. StringToSend,
					strparam ) ;
			break ;

		case	SetFKLabelAction :
			GetSequenceParameter ( index, StaticBuffer, 1, label ) ;
			GetSequenceParameter ( index, StaticBuffer, 2, length ) ;
			GetSequenceParameter ( index, StaticBuffer, 3, length, strparam ) ;

			FunctionKeypad -> SetText ( label - 1, strparam ) ;
			break ;

		case	EnableFKAction :
			FunctionKeypad -> Enable ( ) ;
			break ;


		case	DisableFKAction :
			FunctionKeypad -> Disable ( ) ;
			break ;


	// Début/fin d'un message
		case	MessageHeadAction :
			if  ( InputMessage -> GetState ( )  !=  TMessageBase :: Zeroed )
			   {
				InputMessage -> Zero ( ) ;
				InMessageReceive = FALSE ;
				Connection -> Synchronize ( ) ;
			    }
			else
			   {
				InputMessage -> Zero ( ) ;
				* StaticBuffer = 0 ;
				InMessageReceive = TRUE ;
			    }
			break ;

		case	MessageTailAction :
			InMessageReceive = FALSE ;
			* StaticBuffer = 0 ;

		// Ne pas transmettre le message à TerminalWindow s'il y a
		// eu un pépin
			if  ( InputMessage -> GetState ( )  ==
						TMessageBase :: Complete )
			   {
				TerminalWindowClient -> SendMessage ( WM_UNIXMESSAGE, 1,
					( LPARAM ) ( char far * ) & InputMessage -> Message ) ;
				InputMessage -> Zero ( ) ;
			     }
			else
			    {
				InputMessage -> Zero ( ) ;
				Connection -> Synchronize ( ) ;
				return ( TRUE ) ;
			     }
			break ;


	// Début d'impression d'un fichier (en une seule fois)
		case	BulkPrintBeginAction :
		   {
			if  ( ( PrinterFP = fopen ( GetOutputName ( ), "wb" ) )  ==  NULL )
				break ;
			InPrintMode = TRUE  ;

			TWindow * TheParent = ( TheCheopsMenuWindowObject ) ?
						( TWindow * ) TheCheopsMenuWindowObject :
						TerminalWindow ;

			if  ( TheParent )
			   {
				HWND  hwnd  = TheParent -> GetLastActivePopup ( ) ;

				if  ( hwnd ) 
				   {
					TWindow *  w  =  GetWindowPtr ( hwnd ) ;

					if  ( w )
						TheParent = w ;
				     }
			      }

			PrintingWindow = new  TPrintingDialog ( TheParent,
						& InPrintMode, 256 ) ;
			PrintingWindow -> Create ( ) ;
			PrintingWindow -> BringWindowToTop ( ) ;
			StaticSavedCursor = :: SetCursor ( LoadCursor ( NULL, IDC_WAIT ) ) ;
			break ;
		    }


	// Fin d'impression d'un fichier (en une seule fois)
		case	BulkPrintEndAction :
			InPrintMode = FALSE ;

			if  ( PrintingWindow )
			   {
				delete  PrintingWindow ;
				PrintingWindow = 0 ;
			     }
			fclose ( PrinterFP ) ;
			PrinterFP = NULL ;
			:: SetCursor ( StaticSavedCursor ) ;
			break ;            

	// Début d'impression d'une trame d'un fichier
		case	FramePrintBeginAction :
			if  ( ( PrinterFP = fopen ( GetOutputName ( ), "w" ) )  ==  NULL )
				break ;
			InPrintMode = TRUE ;
			break ;                           

	// Fin d'impression d'une trame
		case 	FramePrintEndAction :
			InPrintMode = FALSE ;
			fclose ( PrinterFP ) ;
			PrinterFP = NULL ;
			break ;             


	// Annulation d'une impression
CancelPrint :
		case	CancelPrintAction :
			if  ( InPrintMode )
			   {
				InPrintMode = FALSE ;
				fclose ( PrinterFP ) ;
				:: SetCursor ( StaticSavedCursor ) ;
			    }
			break ;

	
	// Afficher une message box
		case	MessageAction :
			GetSequenceParameter ( index, StaticBuffer, 1, i ) ;
			GetSequenceParameter ( index, StaticBuffer, 2, length ) ;
			GetSequenceParameter ( index, StaticBuffer, 3, length, strparam ) ;
			GetSequenceParameter ( index, StaticBuffer, 4, length ) ;
			GetSequenceParameter ( index, StaticBuffer, 5, length, strparam2 ) ;
			i = MessageBox ( TerminalWindow -> HWindow, strparam, i, strparam2 ) ;
			sprintf ( strparam, "%d\n", i ) ;
			Connection -> SendData ( strparam, strlen ( strparam ) ) ;
			break ;


	// Exécuter une application
		case	ExecuteAction :
			GetSequenceParameter ( index, StaticBuffer, 1, length ) ;
			GetSequenceParameter ( index, StaticBuffer, 2, length, strparam ) ;
			WinExec ( strparam, SW_SHOW ) ;
			break ;


	// Ouvrir un fichier
		case	OpenFileAction :
			GetSequenceParameter ( index, StaticBuffer, 1, length ) ;
			GetSequenceParameter ( index, StaticBuffer, 2, length, strparam ) ;
			GetSequenceParameter ( index, StaticBuffer, 3, i ) ;

			FileTransferFd = open ( strparam, 
				O_CREAT | O_RDWR | 
					( ( i  ==  1 ) ? O_BINARY : O_TEXT ),
						S_IREAD | S_IWRITE ) ;

			if  ( FileTransferFd  ==  -1 )
				:: MessageBox ( TerminalWindow -> HWindow,
					"Erreur d'ouverture", 
					MB_OK | MB_ICONEXCLAMATION,
					   "Création impossible du fichier %s",
							strparam ) ;
			break ;


	// Fermer le fichier 
		case	CloseFileAction :
			if  ( FileTransferFd  !=  -1 )
			   {
				close ( FileTransferFd ) ;
				FileTransferFd = -1 ;
			    }
			break ;


	// Ajouter un bloc au fichier
		case	WriteFileAction :
			if  ( FileTransferFd  !=  -1 )
			   {
				GetSequenceParameter ( index, StaticBuffer, 1, length ) ;
				GetSequenceParameter ( index, StaticBuffer, 2, length, strparam ) ;
				write ( FileTransferFd, strparam, length ) ;
			    }
			break ;


	// Envoyer le type de la connexion
		case	TypeAction :
			if  ( Configuration -> Connection ( SessionId ). NetworkLink )
				p = "network\n" ;
			else
				p = "rs232\n" ;
			Connection -> SendData ( p, strlen ( p ) ) ;
			break ;


	// Autologin
		case	UserNameAction :
			p = Configuration -> Connection ( SessionId ). UserName ;

			if  ( * p )
			   {
				strcpy ( strparam, p ) ;
				strcat ( strparam, "\n" ) ;
				Connection -> SendData ( strparam, strlen ( strparam ) ) ;

			// Faire en sorte que ça ne marche qu'une fois
				EmulatorSequences [ index ]. Action = ZeroedAction ;
			     }
			break ;


		case	UserPasswordAction :
			p = Configuration -> Connection ( SessionId ). UserPassword ;

			if  ( * p )
			   {
				strcpy ( strparam, p ) ;
				strcat ( strparam, "\n" ) ;
				Connection -> SendData ( strparam, strlen ( strparam ) ) ;

			// Faire en sorte que ça ne marche qu'une fois
				EmulatorSequences [ index ]. Action = ZeroedAction ;
			     }
			break ;

			
	
	
	// Afficher un message si le développeur a oublié de gérer une action
		default :
			MessageBox ( TerminalWindow -> HWindow, "Erreur interne dans DEVICE.CPP",
				MB_OK | MB_ICONEXCLAMATION, "Action inconnue !" ) ;
			* StaticBuffer = 0 ;
	    }



// Et vider le buffer, puisque la totalité de la séquence a été interprétée
	* StaticBuffer = 0 ;

	return ( FALSE ) ;
     }




/**************************************************************************/
/*                                                                        */
/*  TDisplayDevice :: StartSelection, StopSelection, ExpandSelection :    */
/*      Gestion de la sélection.                                          */
/*                                                                        */
/**************************************************************************/

void 	TDisplayDevice :: StartSelection  ( TPoint &  coord )
   {
	if  ( OutsideWindow ( coord ) )
		return  ;

	if  ( InSelection )
		DrawSelection ( SelectionEnd ) ;
	InSelection = TRUE ;

	PixelToCharacter ( coord ) ;

	SelectionStart = SelectionEnd = coord ;
	ButtonReleased = FALSE ;
	TerminalWindowClient -> HideCaret ( ) ;
	DrawSelection ( coord ) ;
    }


void	TDisplayDevice :: StopSelection  ( void )
   {
	InSelection = ButtonReleased = FALSE ;
	DrawSelection ( SelectionEnd ) ;
    }



BOOL	TDisplayDevice :: ExpandSelection  ( TPoint &  coord, BOOL  in_mouse_move )
   {
	TPoint		PreviousStart 	=  SelectionStart,
			PreviousEnd	=  SelectionEnd ;


	if  ( OutsideWindow ( coord ) )
		return ( InSelection ) ;


	if  ( ! InSelection  ||  ButtonReleased )
		return ( FALSE ) ;

	PixelToCharacter ( coord ) ;

	if  ( coord  !=  SelectionEnd )
		DrawSelection ( coord ) ;

	if  ( ! in_mouse_move )		// LBUTTONUP
	   {
		ButtonReleased = TRUE ;
		TerminalWindowClient -> ShowCaret ( ) ;
	    }

	return ( TRUE ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TDisplayDevice :: DrawSelection                                       */
/*      Masque la précédente sélection et affiche la nouvelle.            */
/*                                                                        */
/**************************************************************************/


void	TDisplayDevice :: DrawSelection  ( TPoint &  newcoord )
    {
	register int		x, y, width, height ;


	GetDC ( ) ;
				  
// Effacer la précédente sélection (newcoord n'est égal à SelectionEnd que
// lorsque l'utilisateur vient d'appuyer sur le bouton gauche de la souris)
	if  ( newcoord  !=  SelectionEnd )
	   {
	   // Convertir les coordonnées 'caractère' en 'point'
		x	=  SelectionStart. x,
		y	=  SelectionStart. y,
		width	=  SelectionEnd. x - SelectionStart. x + 1,
		height  =  SelectionEnd. y - SelectionStart. y + 1 ;

		x	*=  CharacterWidth ;
		y 	*=  CharacterHeight ;
		width	*=  CharacterWidth ;
		height  *=  CharacterHeight ;

	// Effacer l'ancien rectangler de sélection
		PatBlt  ( DCHandle, x, y, width, height, PATINVERT ) ;

	// Calculer le nouveau rectangle de sélection
		if  ( newcoord. x  <  SelectionEnd. x )
			SelectionStart. x = newcoord. x ;
		else
			SelectionEnd.   x = newcoord. x ;

		if  ( newcoord. y  <  SelectionEnd. y )
			SelectionStart. y = newcoord. y ;
		else
			SelectionEnd.   y = newcoord. y ;
	     }

// Afficher le nouveau rectangle de sélection
	x	=  SelectionStart. x,
	y	=  SelectionStart. y,
	width	=  SelectionEnd. x - SelectionStart. x + 1,
	height  =  SelectionEnd. y - SelectionStart. y + 1 ;

	x	*=  CharacterWidth ;
	y 	*=  CharacterHeight ;
	width	*=  CharacterWidth ;
	height  *=  CharacterHeight ;

	PatBlt  ( DCHandle, x, y, width, height, PATINVERT ) ;
	ReleaseDC ( ) ;
     }



/**************************************************************************/
/*                                                                        */
/*  TDisplayDevice :: GetSélectionSize                                    */
/*      Retourne la taille (maximale, blancs compris) de la sélection.    */
/*                                                                        */
/**************************************************************************/

int	TDisplayDevice :: GetSelectionSize  ( void )
   {
	if  ( ! InSelection )
		return ( 0 ) ;

// Le 2e '1' représente le caractère newline sera
// ajouté à la fin de chaque ligne par GetSelection.
	return ( ( SelectionEnd. x - SelectionStart. x + 1 ) *
			( SelectionEnd. y - SelectionStart. y + 1 + 1 ) ) ;
     }



/**************************************************************************/
/*                                                                        */
/*  TDisplayDevice :: GetSelection                                        */
/*      Retourne le texte de la sélection, ou une chaîne vide s'il n'y en */
/*      a pas.                                                            */
/*      Les espaces en fin de ligne ne sont pas retournés.                */
/*      Les caractères '\r' et '\n' sont ajoutés à chaque ligne.          */
/*      GetSelection retourne le nombre de caractères effectivement copiés*/
/*      dans le buffer.                                                   */
/*                                                                        */
/**************************************************************************/

int	TDisplayDevice :: GetSelection  ( char *	Result )
   {
	char			buffer [ MAX_PARAMETER_LENGTH ] ;
	register char		c ;
	register char *         p ;
	register int		index ;


// Au kazou...
	if  ( ! InSelection )
	   {
		* buffer = 0 ;
		return ( 0 ) ;
	    }


// Pour chaque ligne de sélection...
	* Result = 0 ;

	for  ( int  i = SelectionStart. y ; i <= SelectionEnd. y ; i ++ )
	   {
		index = ( Origin + i ) % ScreenHeight ;

	// Collecter les caractères de la ligne...
		p = buffer ;

		for  ( int  j = SelectionStart. x ; j < SelectionEnd. x ; j ++ )
		    {
			c = ScreenMemory [ index ] [j]. Character ;

			if  ( ! c )
				c = ' ' ;
			* p ++ = c ;
		      }
		* p = 0 ;
		p -- ;

	// Supprimer les espaces de fin
		while  ( p >= buffer  &&  isspace ( * p ) )
			p -- ;
		p ++ ;
		* p ++ = '\n' ;
		* p    = 0 ;

		strcat ( Result, buffer ) ;
	    }


	return ( strlen ( Result ) ) ;
    }





/**************************************************************************/
/*                                                                        */
/*   TDisplayDevice :: StartPrint, StopPrint                              */
/*      Démarrage et arrêt de l'impression.                               */
/*                                                                        */
/**************************************************************************/

void	TDisplayDevice :: StartPrint  ( void )
   {
	if  ( InPrintMode )
		return ;

	PrinterFP = fopen ( GetOutputName ( ), "w" ) ;
     }


void	TDisplayDevice :: EndPrint  ( void ) 
   {
	if  ( InPrintMode  ||  PrinterFP  ==   NULL )
		return ;

	fputc ( '\f', PrinterFP ) ;
	fclose ( PrinterFP ) ;
	PrinterFP = NULL ;
     }



/**************************************************************************/
/*                                                                        */
/*  TDisplayDevice :: PrintPage                                           */
/*      Imprime une page-écran complète.                                  */
/*                                                                        */
/**************************************************************************/

void	TDisplayDevice :: PrintPage  ( void )
   {
	register int			i, j, Y, DeltaX, DeltaY ;
	HCURSOR				SavedCursor ; 
	BOOL				IsMonochrome ;
	register int			PixelsX		=  CharacterWidth * ScreenWidth,
					PixelsY		=  CharacterHeight * ScreenHeight ;
	COLORREF			PreviousBack	=  0xFFFFFFFFL,
					PreviousFore	=  0xFFFFFFFFL ;
	int				PreviousAttr	=  0xFFFF ;



	SavedCursor = :: SetCursor ( LoadCursor ( NULL, IDC_WAIT ) ) ;
	GetDC ( ) ;



// Ajuster le cadre s'il y en a un
	if  ( Configuration -> Terminal ( ). FrameHardcopy )
	   {
		DeltaX = CharacterWidth / 2 ;
		DeltaY = CharacterHeight / 2 ;
	    }
	else
		DeltaX = DeltaY = 0 ;


// Démarrer (éventuellement) un nouveau job; en tout cas, mettre à jour la
// position sur la page
	Hardcopy -> MustRemain ( "Terminal - Recopie d'écran",
					TPoint ( PixelsX + ( 2 * DeltaX ),
						PixelsY + ( 2 * DeltaY ) +
						CharacterHeight ) ) ;
	TPrintDC &	PrintDC 	=  Hardcopy -> GetDC ( ) ;


// Couleurs 
	if  ( PrintDC. GetDeviceCaps ( COLORRES )  >  2 )
		IsMonochrome = FALSE ;
	else
		IsMonochrome = TRUE ;


// Impression des caractères
	for  ( i = 0 ; i < ScreenHeight ; i ++ )
	   {
		Y = ( Origin + i ) % ScreenHeight ;

		for  ( j = 0 ; j < ScreenWidth ; j ++ )
		   {
			TScreenCharacter & 	Ch  =  ScreenMemory [Y] [j] ;


		// Configuration de la couleur.
		// Je n'ai pas trouvé comment redéfinir la couleur de fond
		// (par contre, la couleur de texte, si). Ni SetBkColor,
		// ni Escape ( SET_BACKGROUND_COLOR ) ne fonctionnent.
		// il semblerait qu'on ne puisse définir le mode TRANSPARENT
		// au moyen de SetBkMode.
			if  ( IsMonochrome )
			   {
				if  ( Ch. BackgroundColor  !=  SessionParameters -> BackgroundColor )
				   {
					PrintDC. SetTextColor ( RGB ( 100, 100, 100 ) ) ;
					PrintDC. SetBkColor ( RGB ( 0, 0, 0 ) ) ;
				    }
				else
				   {
					PrintDC. SetBkColor ( RGB ( 255, 255, 255 ) ) ;
					PrintDC. SetTextColor ( RGB ( 0, 0, 0 ) ) ;
				    }
			    }
			else        
			   {
				if  ( PreviousFore  !=  Ch. ForegroundColor )
					PrintDC. SetTextColor ( Ch. ForegroundColor ) ;
	
				if  ( PreviousBack  !=  Ch. BackgroundColor )
					PrintDC. SetBkColor ( Ch. BackgroundColor ) ;
			    }


		// Choix de la police (montée)
			if  ( PreviousAttr  !=  Ch. Attributes )
			   {
				if  ( Ch. Attributes  &  SPECIAL )
					PrintDC. SelectObject ( SpecialFontHandle ) ;
				else	// pouf pouf
					PrintDC. SelectObject ( FontHandles [ Ch. Attributes & 7 ] ) ;
			    }



		// Ecriture du caractère (un par un, au cas où la police est
		// proportionnelle, car je n'ai pas envie de refaire UpdateScreen)
			PrintDC. TextOut  ( DeltaX + ( j * CharacterWidth ),
					    DeltaY + ( i * CharacterHeight ),
					    & Ch. Character, 1 ) ;
			
		// Mémorisation des derniers attributs rencontrés
			PreviousBack = Ch. BackgroundColor ;
			PreviousFore = Ch. ForegroundColor ;
			PreviousAttr = Ch. Attributes ;
		     }
	     }


// Faut-il tracer un rectangle autour ?
	if  ( Configuration -> Terminal ( ). FrameHardcopy )
		PrintDC. Rectangle ( 0, 0, 
				PixelsX + ( DeltaX * 2 ),
				PixelsY + ( DeltaY * 2 ) ) ;


// Saut de page éventuel
	Hardcopy -> EndJob ( Configuration -> Terminal ( ). FormFeedOnHardcopy ) ;

	ReleaseDC ( ) ;
	:: SetCursor ( SavedCursor ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TDisplayDevice :: PrintCharacter                                      */
/*      Imprime un caractère.                                             */
/*                                                                        */
/**************************************************************************/

void	TDisplayDevice :: PrintCharacter  ( char   c  ) 
   {
	if  ( ContinuousPrint  &&  PrinterFP  !=  NULL )
	   {
		switch  ( c )
		  {
			case	'\t' :
			case	'\f' :
			case	'\n' :
			case	'\r' :
			case	8    : 	// Backspace
				fputc ( c, PrinterFP ) ;
				break ;

				
			default :
				if  ( c >= ' ' )
					fputc ( c, PrinterFP ) ;
				else
				   {
					fputc ( '^', PrinterFP ) ;
					fputc ( c + '@', PrinterFP ) ;
				    }
		     }
	     }
    }






/**************************************************************************/
/*                                                                        */
/*  TDisplayDevice :: SysColorChange                                      */
/*      Remet à jour les couleurs de texte et de fond.                    */
/*                                                                        */
/**************************************************************************/

void	TDisplayDevice :: SysColorChange ( COLORREF	prevfore,
					   COLORREF	prevback,
					   COLORREF	newfore,
					   COLORREF	newback ) 
   {
	register int		i, j ;
					

	for  ( i = 0 ; i < ScreenHeight ; i ++ )
	   {
		for  ( j = 0 ; j < ScreenWidth ; j ++ )
		   {
			TScreenCharacter &  Ch = ScreenMemory [i] [j] ;


			if  ( Ch. ForegroundColor  ==  prevfore )
				Ch. ForegroundColor = newfore ;

			if  ( Ch. BackgroundColor  ==  prevback )
				Ch. BackgroundColor = newback ;
		     }
	      }

	PreviousBackground =  newback ;
	PreviousForeground =  newfore ;
       }



