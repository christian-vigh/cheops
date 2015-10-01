/**************************************************************************/
/*                                                                        */
/*  Help.h  -  Christian Vigh, 95/08/10.                                  */
/*                                                                        */
/*  Gestion de l'aide contextuelle.                                       */
/*                                                                        */
/**************************************************************************/




# if	! defined ( __HELP_H__ )
#	  define    __HELP_H__


# if	! defined ( RC_INVOKED )


# define	MAX_HOTKEYS			8
# define	MAX_HELP_ENTRIES		256
# define	VIRTUAL_KEY_FLAG		0x8000


# define	VIRTUALKEY(vk)			( (vk) | VIRTUAL_KEY_FLAG )




/**************************************************************************/
/*                                                                        */
/*  Classe  THelp -                                                       */
/*      Gestion de l'aide en ligne.                                       */
/*                                                                        */
/**************************************************************************/

class	THelp
   {
	public :

	// Origine d'une requête d'aide
		typedef enum
		   {
				UnclassifiedHelp,
				DialogBoxHelp,
				MenuHelp,
				WindowHelp
		     }  TOrigin ;


	// Touches spéciales que l'on peut associer à une séquence d'aide
		typedef enum
		   {
				None		=  0x00,
				Control		=  0x01,
				Alt		=  0x02,
				Shift		=  0x04
		     }   TKeyModifiers ;


	// Définition du type d'appel de l'aide pour une entrée contextuelle
		typedef enum
		   {
				EndOfTable,	// Signale la fin de la table
				WindowStart,	// Indique que l'entrée suivante 
						// concerne l'aide de la fanêtre
				WindowEnd,
				MenuStart,	// l'aide de ses menus
				MenuEnd,
				ControlStart,	// ou de ses contrôles
				ControlEnd,
				ByID,		// appeler l'aide avec HELP_CONTEXT
				ByKey		// ou avec HELP_KEY
		    }  TEntryType ;


	// Type d'une touche d'aide (normale/contextuelle)
		typedef enum
		    {
				Unknown,
				Normal,
				Contextual,
		     }  THotkeyType ;

	

	// Définition d'une touche d'appel à l'aide en ligne
		typedef struct
		   {
				uint		KeyCode ;  
				TKeyModifiers	Modifiers ;
				THotkeyType	Type ;
		    }  THotkey ;



	// Définition d'une entrée dans la table de l'aide
		typedef struct
		   {
				int		Id ;
				TEntryType	Type ;
				DWORD		ContextParam ;
		     } THelpEntry ;



	// Structure dont l'adresse est passée à une windowproc lorsque
	// le message WM_QUERYHELPCONTEXT est envoyé.
		typedef struct
		   {
				THelpEntry	Entry ;		// A remplir
				TOrigin		Origin ;
				THotkeyType	Type ;
				HWND		hwnd ;		// Message envoyé
				UINT		message ;
				WPARAM		wparam ;
				LPARAM		lparam ;
		    }  THelpQueryContext ;




	public :
		THelp  ( TWindow *	mainwindow ) ;
		virtual  ~THelp  ( ) ;	


	// Fonctions d'initialisation
		int		AddHelp			( TWindow *		window,
							  char *		helpfile,
							  THelpEntry *		help ) ;

		int		AddHelp			( char *		classname,
							  char *		helpfile,
							  THelpEntry *		help ) ;

		int		AddHelp			( TWindow *		window,
							  char *		helpfile,
							  TResId		id ) ;

		int		AddHelp			( char *		classname,
							  char *		helpfile,
							  TResId		id ) ;

		BOOL		RemoveHelp 		( int 			id ) ;
		

		void		SetHotkeys		( THotkey *		keys,
							  int			count ) ;


	// Fonctions d'appels de l'aide en ligne
		virtual int	Help			( char *		helpfile,
							  DWORD			context_id ) ;
		virtual int	Help			( char *		helpfile,
							  char *		key ) ;
		virtual int	Help			( char *		helpfile ) ;
		virtual int	Help			( char *		helpfile,
							  MULTIKEYHELP *        keys ) ;
		virtual int	HelpOnHelp		( void ) ;

		virtual int	WinHelp			( char *		filename,
							  uint			command,
							  DWORD			data ) ;
		virtual int	Quit			( char *		filename ) ;


	// Détermine si le message est une hotkey
		virtual THotkeyType
				IsHotkey		( HWND, uint, WPARAM, LPARAM ) ;



	protected :

	// Indique si l'entrée dans la table concerne l'aide pour une fenêtre
	// ou pour une classe OWL
		typedef enum
		   {
				WindowObject,
				ClassObject
		    }  THelpApply ;


	// Une entrée dans la table d'aide
		typedef struct
		   {
			int		HelpFileIndex ;
			THelpApply	AppliesTo ;

			union
			   {
				HWND	WindowHwnd ;
				char *	ClassName ;
			    } ;

			WNDPROC		WindowProc ;
			BOOL		FromResource ;
			THelpEntry *	ResourceEntries ;

			THelpEntry *	WindowEntries,
				   *	MenuEntries,
				   * 	ControlEntries ;
		     }  THelpDefinition ;
				   


	protected :

	// Table des contextes d'aide
		THelpDefinition		HelpTable     [ MAX_HELP_ENTRIES ] ;
		char *			HelpFileNames [ MAX_HELP_ENTRIES ] ;
		int			HelpTableCount ;

	// Touches d'appel de l'aide
		THotkey 		Hotkeys   	 [ MAX_HOTKEYS ] ;
		int			HotkeyCount ;


	// Effectue le traitement de la hotkey
		virtual void	ProcessHelpRequest  	( TOrigin		origin,
							  THotkeyType		type,
							  HWND, uint, WPARAM, LPARAM ) ;

	// Appelée lorsqu'aucun contexte n'a été trouvé
		virtual BOOL	NotFound            	( BOOL			window,
							  char *		helpfile,
							  TOrigin		origin,
							  THotkeyType		type,
							  HWND, uint, WPARAM, LPARAM ) ;


		int		AddHelp			( THelpApply		appliesto,
							  void *		object,
							  char *		helpfile,
							  THelpEntry *		help ) ;

	
		int		AddHelp			( THelpApply		appliesto,
							  void *		object,
							  char *		helpfile,
							  TResId		id ) ;
							  
	private :
	//  Variables permettant d'empêcher la déclaration de plusieurs objets
	// THelp, de manière à n'avoir qu'un objet sur lequel pointer, accessible
	// par les hook procs.
		static THelp *	TheHelpObject ;
	
	// Fenêtre origine pour WinHelp
		TWindow *		MainWindow ;

	// Elément de menu sur lequel on est positionné
		uint			LastSelectedMenu ;
		uint			LastSelectedMenuType ;
		HMENU			LastSelectedMenuHandle ;

	// Hook pour capturer les demandes d'aide lorsqu'on est dans un menu
	// ou une boîte de dialogue
		HHOOK			MsgFilterHookId ;


	// Ajout d'un nom de fichier à la table des noms
		int			AddHelpFileName  ( char *	filename ) ;

	// Donne le nombre de fois où un fichier est référencé
		int			GetHelpFileUsage ( int		index ) ;
		int			GetHelpFileUsage ( char *	file ) ;

	// Ajout/suppression d'une window proc
		void			AddWindowProc	 ( int		index ) ;
		void			RemoveWindowProc ( int		index ) ;
							  
	
	// WindowProc qui va squatter celle des fenêtres	
		friend LRESULT CALLBACK  HelpWindowProc ( HWND		hwnd,
							  uint		message,
							  WPARAM	wparam,
							  LPARAM	lparam ) ;

	// Hook proc pour récupérer les messages depuis un menu ou une boiboite
		friend LRESULT CALLBACK  MsgFilterHookProc   ( int, WPARAM, LPARAM ) ;
      } ;




/**************************************************************************/
/*                                                                        */
/*  Pour gérer l'aide en ligne au moyen de l'objet THelp, il faut bien    */
/*  entendu l'allouer, puis appeler la fonction membre AddHelp pour       */
/*  ajouter une table de contextes pour une ou plusieurs fenêtres ou      */
/*  classes OWL.                                                          */
/*  Les macros suivantes permettent de définir de manière plus aisée      */
/*  ces tables.                                                           */
/*                                                                        */
/*  Une définition de table doit posséder le format suivant :             */
/*                                                                        */
/*  DEFINE_HELP  ( name )                                                 */
/*      window help                                                       */
/*      menu help                                                         */
/*      controls help                                                     */
/*  END_HELP ;                                                            */
/*                                                                        */
/*  "name" désigne ici un nom qui doit être correct en C++, puisqu'il     */
/*  s'agira du nom utilisé pour créer un tableau de structures.           */
/*                                                                        */
/*  La table peut posséder une, deux ou trois types d'entrées (sous-      */
/*  tables) qui sont décrits ici :                                        */
/*                                                                        */
/*  . windowhelp est la sous-table qui doit contenir les contextes        */
/*    relatifs à la fenêtre en général; elle se définit ainsi :           */
/*                                                                        */
/*    DEFINE_WINDOW                                                       */
/*      contextdefs                                                       */
/*    END_WINDOW                                                          */
/*                                                                        */
/*    "contextdefs", qu'on retrouvera pour les autres types de sous-      */
/*    table, indique une suite de macros qui permettent d'associer un     */
/*    élément fenêtre, contrôle ou autre à une chaîne de caractères ou    */
/*    un numéro de contexte qui sont définis dans le fichier d'aide de    */
/*    l'application.                                                      */
/*                                                                        */
/*    Il existe deux macros pour définir ces contextes :                  */
/*                                                                        */
/*    . HELPID ( id, num )  associe à une fenêtre, dont le numéro de      */
/*      contrôle est 'id', le numéro de contexte 'num' (utilisé ensuite   */
/*      avec la fonction WinHelp et le paramètre HELP_CONTEXT).           */
/*                                                                        */
/*    . HELPKEY ( id, str ) associe à une fenêtre la chaîne de caractères */
/*      'str' (utilisée ensuite avec la fonction WinHelp et le paramètre  */
/*      HELPKEY).                                                         */
/*                                                                        */
/*    Typiquement, l'aide pour une fenêtre normale sera définie par :     */
/*                                                                        */
/*      DEFINE_WINDOW                                                     */
/*              HELPID ( 0, HLC_some_context_number )                     */
/*      END_WINDOW                                                        */
/*                                                                        */
/*     (noter  l'absence de virgule à la fin de chaque appel de macro)    */
/*                                                                        */
/*     Lorsqu'il s'agit d'une aide relative à une boîte de dialogue ou,   */
/*     plus généralement, à une classe OWL pour les boîtes de dialogue,   */
/*     la partie "contextdefs" permet éventuellement d'indiquer à         */
/*     quelles boîtes de dialogue s'épplique cette aide : il faut pour    */
/*     cela specifier autant de HELPID ou HELPKEY que nécessaire :        */
/*                                                                        */
/*      DEFINE_WINDOW                                                     */
/*              HELPID ( IDD_DIALOG_1, 1 )                                */
/*              HELPID ( IDD_DIALOG_2, 2 )                                */
/*              ...                                                       */
/*      END_WINDOW                                                        */
/*                                                                        */
/*      Dans ce cas, l'aide ne s'appliquera qu'aux boîtes de dialogue     */
/*      dont le numéro a été défini ici.                                  */
/*                                                                        */
/*  . menuhelp est la sous-table qui permet d'associer un contexte à un   */
/*    élément de menu; elle a le format suivant :                         */
/*                                                                        */
/*     DEFINE_MENU ( "Principal" )                                        */
/*              DEFINE_MENU ( "Fichier" )                                 */
/*                      HELPKEY ( IDC_OPEN, "Fichier/Ouvrir" )            */
/*                      ...                                               */
/*              END_MENU                                                  */
/*      END_MENU                                                          */
/*                                                                        */
/*    En règle générale, cette définition se conforme à celle contenue    */
/*    dans le fichier de ressources.                                      */
/*    L'aide sur un menu popup (un élément de la barre de menu, ou un     */
/*    élément de menu qui permet d'accéder à un sous-menu) ne peut se     */
/*    spécifier qu'au moyen d'une chaîne de caractères (et non d'un       */
/*    numéro de contexte). C'est un choix, et c'est comme ça.             */
/*                                                                        */
/*  . controlhelp est la sous-table où l'on va associer à chaque contrôle */
/*    d'une boîte de dialogue un numéro de contexte ou une chaîne de      */
/*    caractères à utiliser avec WinHelp :                                */
/*                                                                        */
/*      DEFINE_CONTROL                                                    */
/*              HELPID ( IDC_EDIT1, 100 )                                 */
/*              HELPKEY  ( IDC_LISTBOX1, 101 )                            */
/*              ...                                                       */
/*      END_CONTROL                                                       */
/*                                                                        */
/*    A noter que cette sous-table n'est nécessaire que s'i l'on désire   */
/*    une aide contextuelle (dans une boîte de dialogue, l'appui sur une  */
/*    certaine séquence de touches permettrait d'aboutir à l'aide         */
/*    relative au contrôle qui a le focus, et non à l'aide de la boîte    */
/*    elle-même, par exemple).                                            */
/*                                                                        */
/**************************************************************************/

# define	DEFINE_HELP(name)		THelp :: THelpEntry  name [] = {
# define	END_HELP			{ 0, THelp :: EndOfTable, 0L } }

# define	DEFINE_WINDOW			{ 0, THelp :: WindowStart, 0L },
# define	DEFINE_MENU(key)		{ 0, THelp :: MenuStart, ( DWORD ) key },
# define	DEFINE_CONTROL			{ 0, THelp :: ControlStart, 0L },

# define	END_WINDOW			{ 0, THelp :: WindowEnd, 0L },
# define	END_MENU			{ 0, THelp :: MenuEnd, 0L },
# define	END_CONTROL			{ 0, THelp :: ControlEnd, 0L },
										
# define	HELPID(id,value)		{ id, THelp :: ByID, value },
# define	HELPKEY(id,key)			{ id, THelp :: ByKey, ( DWORD ) key },



# endif		/*  RC_INVOKED  */



/**************************************************************************/
/*                                                                        */
/*  Les mêmes, pour un fichier de ressources.                             */
/*                                                                        */
/**************************************************************************/

# define	HELP_RESOURCE			"HELPTABLE"
# define	DEFWINTYPE			'1'
# define	DEFMENUTYPE			'2'
# define	DEFCONTROLTYPE			'3'
# define	ENDWINTYPE			'4'
# define	ENDMENUTYPE			'5'
# define	ENDCONTROLTYPE			'6'
# define	BYIDTYPE			'7'
# define	BYKEYTYPE			'8'

# define	BEGINHELP			{
# define	ENDHELP				'0'  }


# if	defined ( RC_INVOKED )

# define	DEFINE_WINDOW			DEFWINTYPE,
# define	DEFINE_MENU(key)		DEFMENUTYPE, key, '0',
# define	DEFINE_CONTROL			DEFCONTROLTYPE,

# define	END_WINDOW			ENDWINTYPE,
# define	END_MENU			ENDMENUTYPE,
# define	END_CONTROL			ENDCONTROLTYPE,
										
# define	HELPID(id,value)		BYIDTYPE, id, value##L,
# define	HELPKEY(id,key)			BYKEYTYPE, id, key, '0',

# endif		/*  RC_INVOKED  */


# endif		/*  __HELP_H__  */
