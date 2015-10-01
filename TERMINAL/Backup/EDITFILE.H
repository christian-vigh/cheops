/**************************************************************************/
/*                                                                        */
/*  EditFile.h  -  Patrick Sauvé, Mai 95.                                 */
/*                                                                        */
/*  Gestion d'une fenêtre d'édition, avec décompression des données.      */
/*                                                                        */
/**************************************************************************/


# if	! defined ( __EDITFILE_H__ )
#	  define    __EDITFILE_H__





# define	ICON_LISTE		( Resource_Base + 200 )



# if	COMPRESS_STATISTICS   &&  ! STANDALONE

/**************************************************************************/
/*                                                                        */
/*  Classe  TDictionaryStatistics -                                       */
/*      Collecte les statistiques sur la décompression.                   */
/*                                                                        */
/**************************************************************************/

# define	IDD_DICTIONARY_STATISTICS	1000
# define	IDC_DICTIONARY_STATISTICS	1000


# define	BBN_NORMAL			1000
# define	BBN_PRESSED			3000
# define	BBN_FOCUSED			5000


# define	IDC_HT_HITS			100
# define	IDC_LL_HITS			101
# define	IDC_LL_HITS_SEARCHES		102
# define	IDC_LL_ADDS			103
# define	IDC_LL_ADDS_SEARCHES		104
# define	IDC_MAX_CODE_SIZE		105
# define	IDC_DICTIONARY_SIZE		106
# define	IDC_NEXT_BUTTON			107
# define	IDC_PREVIOUS_BUTTON		108
# define	IDC_DICTIONARY_COUNT		109
# define	IDC_DICTIONARY_COUNT_TEXT	110
# define	IDC_FIRST_BUTTON		111
# define	IDC_LAST_BUTTON			112
# define	IDC_BLOCK_COUNT			113
# define	IDC_BLOCK_COUNT_TEXT		114
# define	IDC_GLOBAL_GROUPBOX		115



class	TDictionaryStatistics	:  public  TDialog
   {
	public :

	// Statistiques du dictionnaire :
		typedef struct  TStatistics
		   {
			// Nombre d'éléments recherchés dans le dictionnaire et
			// trouvés dans la hash table
				DWORD	HashTableHits ;
			
			// Nombre d'éléments recherchés dans le dictionnaire et
			// trouvés dans une liste chaînée
				DWORD	LinkedListHits ;

			// Nombre d'éléments parcourus lors dès recherches dans
			// les listes chaînées
				DWORD	LinkedListHitsSearches ;

			// Nombre d'éléments ajoutés dans une liste chaînée
				DWORD	LinkedListAdds ;

			// Nombre d'éléments parcourus lors de ces ajouts 
				DWORD	LinkedListAddsSearches ;

			// Taille maximum du code utilisé
				BYTE	MaxCodeSize ;

			// Nombre d'éléments présents dans le dictionnaire
				DWORD	DictionarySize ;


			// Constructeur
				TStatistics ( ) :
					HashTableHits 		( 0 ),
					LinkedListHits 		( 0 ),
					LinkedListHitsSearches 	( 0 ),
					LinkedListAdds 		( 0 ),
					LinkedListAddsSearches 	( 0 ),
					MaxCodeSize 		( 0 ),
					DictionarySize 		( 0 )
				   { }

				
		      }  TStatistics ;


	private :
	// Il y a autant de statistiques sur le dictionnaires qu'il y a eu de
	// dictionnaires recréés suite au BUMP_CODE
		TStatistics *		Statistics ;
		TStatistics 		Global ;
		int  			StatisticCount ;

		int  			CurrentStatistics ;


	public :
		int  			BlockCount ;


	public :
	// Constructeur & destructeur
		TDictionaryStatistics ( ) ;
	       ~TDictionaryStatistics ( ) ;


		void  		NewDictionary   ( void ) ;
		TStatistics &   operator []  	( int		index ) ;
		int		Execute		( void ) ;


	// Affiche les statistiques globales
		BOOL		EvInitDialog  ( HWND  focus )
		   {
			register BOOL	Status   =  TDialog :: EvInitDialog ( focus ) ;


			CmShow ( -1 ) ;
			return  ( Status ) ;
		    }


	// Bouton "Suivant"
		void		CmNext		( void )
		   {
			if  ( CurrentStatistics + 1  >=  StatisticCount )
				CmShow ( -1 ) ;
			else
				CmShow ( CurrentStatistics + 1 ) ;
		    }


	// Bouton "Précédent"
		void		CmPrevious	( void )
		   {
			if  ( CurrentStatistics  ==  -1 )
				CmShow ( StatisticCount - 1 ) ;
			else
				CmShow ( CurrentStatistics - 1 ) ;
		    }

	// Boutons "Premier" et "dernier"
		void	CmFirst  ( void )
		   {
			CmShow ( -1 ) ;
		    }


		void	CmLast  ( void )
		   {
			CmShow  ( StatisticCount - 1 ) ;
		    }


		void		CmShow		( int		index ) ;

	DECLARE_RESPONSE_TABLE ( TDictionaryStatistics ) ;
     } ;

# endif		/*  COMPRESS_STATISTICS  */			




/**************************************************************************/
/*                                                                        */
/*  Classe  TDecompressor -                                               */
/*      Gestion de la décompression du fichier texte.                     */
/*                                                                        */
/**************************************************************************/


// Code des erreurs renvoyées

# define 	OK			0	// Devinez...
# define 	ERR_OPEN		2       // Erreur d'ouverture de fichier
# define 	ERR_ALLOC		3	// Erreur d'allocation mémoire
# define 	ERR_WRITE		4	// Erreur d'écriture
# define 	ERR_COMPRESS		5	// Erreur de compression
# define 	ERR_UNCOMPRESS		6	// Erreur de décompression
# define 	ERR_SAVE		7	// Erreur de sauvegarde du dictionnaire
# define 	ERR_LOAD		8	// ...et de rechargement
# define 	ERR_CLOSED		9	// La fenêtre est fermée
# define 	ERR_BLOCK		10	// Erreur de bloc
# define 	ERR_INC			11
# define 	ERR_EOF			12	// Fin de fichier atteinte
# define 	ERR_RECEIVED		13	// Bloc déjà reçu



// Nombre maximum de bits sur lequel sera codé une reference au dico
# define 	BITS                    15

// Nombre de code d'éléments maxi du dico
# define 	MAX_CODE                32767     // ( ( 1 << BITS ) - 1 )


// Taille réelle du dico 
// le dico est un peu grand que prévu afin d'ameliorer la fonction
// de hachage
# define 	TABLE_SIZE              32767


// definition des codes spéciaux
# define 	END_OF_STREAM           259	// code de fin de fichier
# define 	BUMP_CODE               257	// changement de codage 9->10 bits etc
# define 	FLUSH_CODE              258   	// vider le dico
# define 	FIRST_CODE              260   	// Premier code dispo pour le codage
# define 	FIN_BLOC		256   	// code de fin de bloc


// compartiment libre dans le dico
# define 	UNUSED                  -1


// pendant la compression si les données restantes à compresser sont
// inferieures au seuil fixé elles sont ajoutées au dernier bloc.
// Cela évite d'avoir des petits blocs suplémentaires à renvoyer
# define 	SEUIL			4096


// Taille (approximative) d'un bloc
# define 	TAILLE_BLOC		4096




// dictionnaire utilisé pour la compression et la decompression
typedef struct TDictionary
   {
		int 			parent_code;
		char 			character;
		unsigned int 		index ;
		struct TDictionary *	next ;

		TDictionary  ( )
		   	: index ( -1 ), next ( 0 )  { } 
    } TDictionary;




// structure servant à lire le bloc de données compréssées bit par bit
typedef struct 
   {
		char *		data;
		int 		size;
		unsigned char 	mask;
		int 		rack;
     } BIT_INPUT;


class	TEditWindow ;


class	TUncompressor
   {
# if	COMPRESS_STATISTICS  &&  ! STANDALONE
	public :
		TDictionaryStatistics	DictionaryStatistics ;
# endif


	private :


	// Variables pour la décompression
		unsigned int   		NextCode,
					OldCode ;
		int	    		CurrentCodeBits ;

	// Le dictionnaire
		TDictionary *		Dictionary ;
		BOOL			DictionaryResident ;

	// Pile utilisée pour reconstituer le fichier d'origine à partir des
	// codes
		char *			DecodeStack ;
		unsigned int 		DecodeStackIndex ;

		unsigned char  		BlockCount ;

	// Fichiers 
		char 	    		DictionaryFileName [ MAX_PARAMETER_LENGTH ],
					DataFileName       [ MAX_PARAMETER_LENGTH ] ;
		FILE *			DictionaryFileFP,
		     *			DataFileFP ;
		char *			DataBuffer,
		     *			DictionaryBuffer ;

		BOOL			KeepDataFile ;


	// Titre de l'édition
		char			Title   	   [ MAX_PARAMETER_LENGTH ] ;

	// TRUE si la décompression s'est correctement terminée
		BOOL			Complete ;

	// Gestion de la décompression avec blocs de taille fixe
		BOOL			SpanBlocks ;
		BIT_INPUT		BitInput ;
		unsigned int		NewCode ;
		unsigned long int	Mask,
					ReturnValue ;
		




	public :
		TUncompressor  ( char *  output = 0, BOOL  keep_data = FALSE ) ;
	       ~TUncompressor  ( ) ;




	// Retourne TRUE si la décompression est terminée
		BOOL		IsComplete  ( void )
		   {
			return ( Complete ) ;
		    }

	// Retourne TRUE si la décompression est en cours
		BOOL		IsProcessing ( void )
		   {
			return  ( DictionaryFileFP  !=  NULL ) ;
		    }



	// Retourne le nom du fichier de données
		char *		GetFileName ( void )
		   {
			return  ( DataFileName ) ;
		    }


	// Retourne un pointeur sur le fichier de données
		operator FILE *  ( )
		   {
			return  ( DataFileFP ) ;
		    }


	// Retourne le titre de l'édition
		char *	GetTitle ( void )
		   {
			return  ( Title ) ;
		    }

	
	
	// Fonctions de décompression
		int  		Initialize		( TUnixMessage *	Msg,
							  char **		pData,
							  int *			size ) ;

		int  		Uncompress		( char *		pData,
							  int 			size ) ;

		TDictionary &	DICT	 		( unsigned int  	index ) ;
		void		CheckError		( int 			erreur ) ;
		int 		LoadDictionary		( FILE *		f ) ;
		int 		SaveDictionary		( FILE *		f ) ;
		void 		InitializeDictionary	( void ) ;
		int 		AllocateDictionary	( void ) ;
		void 		FreeDictionary		( BOOL			reset_only = FALSE  ) ;

		void		DecodeString		( unsigned int 		code ) ;
		int 		InputBits		( BIT_INPUT *		bit_file,
							  unsigned int *	val_ret ) ;

	// SetSpanning : indique si la décompression doit se faire sur des
	// frontières de blocs
		void	SetSpanning  ( BOOL  spanning )
		   {
			SpanBlocks = spanning ;
		    }

		BOOL	GetSpanning  ( void )
		   {
			return  ( SpanBlocks ) ;
		     }


	// Write : permet d'effectuer des opérations avant toute écriture de données
		virtual int	Write  ( char *  buffer, int  size )
		   {
			return ( fwrite ( buffer, size, 1, DataFileFP ) ) ;
		    }

	// Seek : déplace le pointeur de fichier
		virtual long int  Seek  ( long int  offset, int  whence )
		   {
			return  ( fseek ( DataFileFP, offset, whence ) ) ;
		    }


	// Getchar : retourne le prochain caractère du fichier
		virtual int	GetChar ( void )
		   {
			return  ( fgetc ( DataFileFP ) ) ;
		     }



	// FileNumber : retourne l descripteur du fichier
		virtual int	FileNumber  ( void )
		   {
			return  ( fileno ( DataFileFP ) ) ;
		    }
		

	// Flush : Ecrit les données sur le disque
		void	Flush ( void ) 
		   {
			fflush ( DataFileFP ) ;
		    }
			

	// Appelée après la réception complète du fichier, CheckFF en supprime
	// le dernier caractère s'il s'agit d'un saut de page
		void	CheckFF  ( void )
		   {
			register int		c ;


			Seek ( -1, SEEK_END ) ;
			c = GetChar ( ) ;

			if  ( c  ==  '\f' )
			   {
				register int	fd = FileNumber ( ) ;
				chsize ( fd, filelength ( fd ) - 1 ) ;
			    }
		     }
    } ;                                                       






/**************************************************************************/
/*                                                                        */
/*  Classe  TFileUncompressor -                                           */
/*      Permet la décompression à la volée d'un fichier (fonctions        */
/*      UWFOpen/Read/Close/WriteFile).                                    */
/*                                                                        */
/**************************************************************************/

class	TFileUncompressor :  public  TUncompressor
   {
	private :
		int		FileFd ;
		bool		BinaryMode ;
		int		BytesWritten ;


	public :
		TFileUncompressor  ( int  fd )
			: TUncompressor ( "", TRUE ) 
		   {
			FileFd		= fd ;
			BytesWritten	= 0 ;
			BinaryMode	= true ;
			SetSpanning ( TRUE ) ;
		    }


	// Begin : à appeler avant décompression d'un bloc
		void	Begin  ( bool  bin )
		   {
			BinaryMode   = bin ;
			BytesWritten = 0 ;
		    }

	// End : à appeler après décompression d'un bloc
		void	End  ( int &  bytes_written )
		   {
			bytes_written = BytesWritten ;
		    }


	// operator int : retourne le descripteur
		operator int  ( )
		   {
			return  ( FileFd ) ;
		    }


	// Write : effectue l'écriture des données
		int	Write  ( char *  data, int  size ) ;

	// Seek : déplace le pointeur de fichier
		long int  Seek  ( long int  offset, int  whence )
		   {
			return  ( lseek ( FileFd, offset, whence ) ) ;
		    }

	// GetChar : retourne le prochain caractère
		int	GetChar ( void )
		    {
			char		c ;


			if  ( read ( FileFd, & c, 1 )  ==  1 )
				return ( c ) ;
			else
				return  ( -1 ) ;
		     }


	// FileNumber : retourne le descripteur du fichier
		int	FileNumber ( void )
		   {
			return  ( FileFd ) ;
		    }


	// Flush : Ecrit les données sur le disque
		void	Flush ( void ) 
		   {
		    }
			
      } ;



/**************************************************************************/
/*                                                                        */
/*   Classe TCheopsTextFile -                                             */
/*      Héritée de TTextFile, permet de traiter les sauts de page comme   */
/*      une ligne à part entière.                                         */
/*      Collecte des statistiques sur le texte.                           */
/*      Gère les séquences d'échappement (CPI,LPI)                        */
/*                                                                        */
/**************************************************************************/

class 	TCheopsTextFile		:  public  TTextFile
   {
	public :
		int			MaxLineLength10CPI,
					MaxLineLength12CPI,
					MaxLineLength15CPI,
					MaxLineLength ;
		int			CPI,
					LPI ;
		int			MaxPageLength,
					CurrentPageLength ;
		long int		LastReadOffset ;
		long int   		PageCount ;
		BOOL			AnsiCharSet ;
		BOOL			OrientationSet ;



	public :

	// Constructeur et destructeur
		TCheopsTextFile ( ) 
			: TTextFile ( )
		   {
			MaxLineLength10CPI	=  0 ;
			MaxLineLength12CPI	=  0 ;
			MaxLineLength15CPI	=  0 ;
			MaxLineLength 		=  0 ;
			CPI			=  10 ;
			LPI			=  6 ;
			MaxPageLength   	=  0 ;
			CurrentPageLength 	=  0 ;
			LastReadOffset		=  -1 ;
			PageCount		=  1 ;
			AnsiCharSet		=  TRUE ;
			OrientationSet 		=  FALSE ;
		    }



		BOOL		Read  ( long int  	offset, 
					char *		buffer,
					long int &	newpos ) ;


	// Gestion des sauts de page
		TCharacterType	CheckCharacter  ( int c )
		   {
			if  ( c  ==  '\f' )
				return  ( EndOfPage ) ;
			else if  ( c  ==  LIST_OEM )
			   {
				AnsiCharSet = FALSE ;
				return  ( Regular | Discard ) ;
			    }
			else if  ( c  ==  LIST_PORTRAIT  ||  c  ==  LIST_LANDSCAPE )
			   {
				OrientationSet = TRUE ;
				return  ( Regular ) ;
			    }							
			else
				return ( TTextFile :: CheckCharacter ( c ) ) ;
		     }




	// Gestion du jeu de caractères
		void	HostToLocalCharSet  ( char *  buffer )
		   {
			if  ( AnsiCharSet )
				Device -> HostToLocalCharSet ( buffer ) ;
		    }


		BOOL	IsAnsi  ( void )
		   {
			return  ( AnsiCharSet ) ;
		    }


	// Retourne TRUE si le document définit l'orientation du papier
		BOOL	HasOrientationSet  ( void )
		   {
			return  ( OrientationSet ) ;
		    }

     } ;





/**************************************************************************/
/*                                                                        */
/*  Classe TEditWindowClient -                                            */
/*      Sert de support pour l'affichage du texte.                        */
/*                                                                        */
/**************************************************************************/
	

# define	MAX_EDITWINDOW_LINES		256 


class	TEditWindow ;
class	TWindowPrintout ;


class	TEditWindowClient 	:  public  TWindow
   {
	private :
		TEditWindow *			EditWindow ;
		char *				Display  [ MAX_EDITWINDOW_LINES ] ;
		long int			TopLine ;
		int				LineCount ;

		TFont *				Font ;
		int				CharacterHeight,
						CharacterWidth,
						AverageCharacterWidth,
						ExternalLeading ;


	public :
		TEditWindowClient  ( TEditWindow *  win ) ;
	       ~TEditWindowClient  ( ) ;

		void		Free		( void ) ;

		void		GetFontMetrics  ( void ) ;

		BOOL		Create  	( void ) ;
		void		Paint		( TDC &  	dc,
						  BOOL		erase,
						  TRect	&	rect ) ;
		void		DrawMatch	( TDC * 	dc  	=  0,
						  BOOL		cancel  =  FALSE ) ;


		void		EvSize		( UINT		type,
						  TSize &	size ) ;

		void		EvKeyDown	( UINT, UINT, UINT ) ;
		void		EvChar		( UINT, UINT, UINT ) ;

		void		Adjust 		( long int	newtop,
						  long int	newbottom,
						  int 		old_count ) ;

		void		ProcessCR	( char *	buffer ) ;

		void		CreateScroller	( void ) ;
		void		UpdateScroller	( TSize &	s = TSize ( 0, 0 ) ) ;

		void		DisplaySpecial  ( char *	line,
						  TDC &		dc,
						  int		x,
						  int		y,
						  int		cx,
						  int		cy ) ;


# pragma	warn	-inl
		BOOL		IsSpecial  ( char  c )
		   {
			switch  ( c )
			   {
				case	'\f' :
					return ( TRUE ) ;
	
				default :
					return ( FALSE ) ;
			     }
		    }
# pragma 	warn	.inl

		
		
	// Gestion du caret
		void		ManageCaret  	( BOOL  on ) ;
		void		EvSetFocus	( HWND ) ;
		void		EvKillFocus 	( HWND ) ;	

		void		SetCaretPos  ( int  x, int  y )
		   {
			:: SetCaretPos ( x * CharacterWidth, y * CharacterHeight ) ;
	 	    }



	// Gestion de la sélection 
	protected :
		long int	SelectionX1, 
				SelectionX2,
				SelectionY1,
				SelectionY2,
				LastSelectionX1,
				LastSelectionX2,
				LastSelectionY1,
				LastSelectionY2 ;
		BOOL		LButtonDown ;



		void		EvLButtonDown 	( uint  modkeys, TPoint &  pos ) ;
		void		EvLButtonUp 	( uint  modkeys, TPoint &  pos ) ;
		void		EvMouseMove 	( uint  modkeys, TPoint &  pos ) ;

		void		DrawSelection	( TDC * 	dc	=  0 ) ;
		void		PointToPos	( long int	OldX,
						  long int	OldY,
						  long int &	NewX,
						  long int &	NewY ) ;


		void		ProcessSelectionKey  ( void ) ;


	// Normalise les coordonnées de la sélection afin que les coordonnées
	// du coin supérieur gauche soient inférieures à celles du coin
	// inférieur droit
		void		GetNormalizedSelection ( long int & 	X1,
							 long int &     Y1,
							 long int &	X2,
							 long int &     Y2 )
		   {
			long int	a, b ;


			a = min ( SelectionX1, SelectionX2 ) ;
			b = max ( SelectionX1, SelectionX2 ) ;
			X1 = a ;
			X2 = b ;

			a = min ( SelectionY1, SelectionY2 ) ;
			b = max ( SelectionY1, SelectionY2 ) ;
			Y1 = a ;
			Y2 = b ;
		     }


	// annule toute sélection
		void 		CancelSelection ( )
		   {
			SelectionX1     = -1 ;
			SelectionX2     =  0 ;
			SelectionY1     = -1 ;
			SelectionY2     =  0 ;


			if  ( LButtonDown )
				ReleaseCapture ( ) ;

			DrawSelection ( ) ;
			LastSelectionX1 = -1 ;
			LastSelectionX2 =  0 ;
			LastSelectionY1 = -1 ;
			LastSelectionY2 =  0 ;

			LButtonDown = FALSE ;
		     }


	// Retourne TRUE s'il y a une sélection en cours
		BOOL	HasSelection ( void )
		   {
			return  ( SelectionX1  >=  0 ) ;
		    }
		



	friend class	TEditWindow ;
	friend class	TEditWindowPrintout ;

	DECLARE_RESPONSE_TABLE ( TEditWindowClient ) ;
     } ;




/**************************************************************************/
/*                                                                        */
/*  Classe  TEditWindow -                                                 */
/*      Gère l'affichage de l'édition Cheops dans une fenêtre.            */
/*                                                                        */
/**************************************************************************/

# define	EDITWINDOW_TIMER_ID		1001


// Commandes de menu
# define	IDC_FILE_FONT			900
# define	IDC_FILE_PRINT			901
# define	IDC_FILE_PRINT_CONFIG		902
# define	IDC_FILE_EXIT			903
# define	IDC_EDIT_COPY			904
# define	IDC_EDIT_SELECT_ALL		905
# define	IDC_FILE_SAVE			906
# define	IDC_EDIT_GOTO_PAGE		907
# define	IDC_EDIT_GOTO_LINE		908
# define	IDC_EDIT_SEARCH			909
# define	IDC_EDIT_SEARCH_AGAIN		910
# define	IDC_EDIT_CANCEL_SELECTION	911



class	TColoredStatusBar ;


class	TEditWindow 	:  public  TDecoratedFrame
   {
	private :
		char *				FileName ;
		TColoredStatusBar *  		StatusBar ;	// Décorations
		TControlBar *			ControlBar ;
		TFilletBar *			FilletBar ;
		TEditWindowClient *		EditWindowClient ;

		TChooseFontDialog :: TData *	FontData ;	// Infos sur la police

		TCheopsTextFile *		TextFile ;	// Gestion du texte

		int				TimerId ;
		long int			UnixFileSize ;

		TPoint				CaretPosition ; // Position du caret (en caractères)

		long int			CurrentPage ;	// Page courante

		TFindDialog *			FindDialog ;
		TFindReplaceDialog :: TData	FindData ;
		long int			FindMatchLine ;
		int				FindMatchColumn,
						FindMatchLength ;



	public :
		TUnixApplication *		Application ;
		integer16			ObjectId ;
		TUncompressor *			Uncompressor ;	// Fonctions de décompression


	public :
		TEditWindow  ( TUnixApplication *	app,
			       TWindow *  		parent, 
			       long int  		size ) ;
		TEditWindow  ( char *    filename ) ;
	       ~TEditWindow  ( ) ;
	
        	void		Zero		( void ) ;
		void		Free		( void ) ;



	// Gestion du texte
		BOOL		GetLine 	( long int     	num,
						  char *	buffer ) ;
		void		UpdateText	( void ) ;
		void		UpdatePosition  ( void ) ;



	// Fonctions liées à la fenêtre
		BOOL		Create 		( void ) ;

		void		EvTimer		( UINT ) ;

		void		EvClose  	( void )  ;


	//  Fonction appelée sur réception du message FINDMSGSTRING
		LRESULT		EvFindMsg  ( WPARAM,  LPARAM  lp )
		   {
			FindDialog -> UpdateData  (lp ) ;

			if  ( FindData. Flags  &  FR_DIALOGTERM )
			   {
				delete  FindDialog ;
				FindDialog = 0 ;
			    }
			else
				CmEditSearchAgain ( ) ;

			return  ( 0L ) ;
		     }


	// Demande au menu de créer une fenêtre icônique MDI
		void	EvSysCommand  ( uint  type, TPoint &  pos ) ;

	// Donner le focus à la fenetre cliente
		void	EvSetFocus  ( HWND  lost )
		   {
			TDecoratedFrame :: EvSetFocus ( lost ) ;
		      }
	


	// Commandes de menu
		void	CmFileSave 		( void ) ;
		void	CmFileFont		( void ) ;
		void	CmFilePrint		( void ) ;
		void	CmFilePrintConfig	( void ) ;
		void	CmFileExit 		( void ) ;

		void	CmEditCopy 		( void ) ;
		void	CmEditSelectAll		( void ) ;
		void	CmEditGotoPage		( void ) ;
		void	CmEditGotoLine		( void ) ;
		void	CmEditSearch		( void ) ;
		void	CmEditSearchAgain	( void ) ;
		void	CmEditCancelSelection	( void ) ;


		void	Find			( char *	line,
						  char *	regexp,
						  uint32	flags,
						  int &		start,
						  size_t &	length,
						  int		start_col = 0 ) ;


# if	COMPRESS_STATISTICS  &&  ! STANDALONE
		void	CmDictionaryStatistics  ( void )
		   {
			Uncompressor -> DictionaryStatistics. Execute ( ) ;
		    }
# endif

		void	CommandEnable		( TCommandEnabler &  ce ) ;



	// Remise à jour des couleurs
		void	EvSysColorChange ( void )
		   {
			Invalidate ( ) ;
		    }


	// Initialisation
		int 	InitializeDecompression  ( TUnixMessage *	Msg,
						   char **		pData,
						   int *		size )
		   {
			register int		Status = Uncompressor -> Initialize ( Msg, pData, size ) ;


			UpdateText ( ) ;
			return  ( Status ) ;
		     }



	// Fonctions pour l'enregistrement de la classe
		char *	GetClassName  ( void )
		   {
                        return  ( "TEditWindow" ) ;
		    }


		void	GetWindowClass ( WNDCLASS &  cls )
		   {
			TDecoratedFrame :: GetWindowClass ( cls ) ;
			cls. hIcon = :: LoadIcon ( * GetModule ( ), ( LPSTR ) ICON_LISTE ) ;
		    }



	// PreProcessMsg : Appelle IdleAction car cette dernière n'est appelée 
	// que pour la fenêtre principale...
		virtual BOOL	PreProcessMsg ( MSG &  msg )
		   {
			IdleAction ( 0 ) ;
			return  ( TDecoratedFrame :: PreProcessMsg ( msg ) ) ;
		    }



	friend class  TEditWindowClient ;
	friend class  TEditWindowPrintout ;
					     
	DECLARE_RESPONSE_TABLE  ( TEditWindow ) ;
     } ;

			


# endif		/*  __EDITFILE_H__  */
