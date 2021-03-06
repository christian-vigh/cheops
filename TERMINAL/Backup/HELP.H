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

	// Origine d'une requ�te d'aide
		typedef enum
		   {
				UnclassifiedHelp,
				DialogBoxHelp,
				MenuHelp,
				WindowHelp
		     }  TOrigin ;


	// Touches sp�ciales que l'on peut associer � une s�quence d'aide
		typedef enum
		   {
				None		=  0x00,
				Control		=  0x01,
				Alt		=  0x02,
				Shift		=  0x04
		     }   TKeyModifiers ;


	// D�finition du type d'appel de l'aide pour une entr�e contextuelle
		typedef enum
		   {
				EndOfTable,	// Signale la fin de la table
				WindowStart,	// Indique que l'entr�e suivante 
						// concerne l'aide de la fan�tre
				WindowEnd,
				MenuStart,	// l'aide de ses menus
				MenuEnd,
				ControlStart,	// ou de ses contr�les
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

	

	// D�finition d'une touche d'appel � l'aide en ligne
		typedef struct
		   {
				uint		KeyCode ;  
				TKeyModifiers	Modifiers ;
				THotkeyType	Type ;
		    }  THotkey ;



	// D�finition d'une entr�e dans la table de l'aide
		typedef struct
		   {
				int		Id ;
				TEntryType	Type ;
				DWORD		ContextParam ;
		     } THelpEntry ;



	// Structure dont l'adresse est pass�e � une windowproc lorsque
	// le message WM_QUERYHELPCONTEXT est envoy�.
		typedef struct
		   {
				THelpEntry	Entry ;		// A remplir
				TOrigin		Origin ;
				THotkeyType	Type ;
				HWND		hwnd ;		// Message envoy�
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


	// D�termine si le message est une hotkey
		virtual THotkeyType
				IsHotkey		( HWND, uint, WPARAM, LPARAM ) ;



	protected :

	// Indique si l'entr�e dans la table concerne l'aide pour une fen�tre
	// ou pour une classe OWL
		typedef enum
		   {
				WindowObject,
				ClassObject
		    }  THelpApply ;


	// Une entr�e dans la table d'aide
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

	// Appel�e lorsqu'aucun contexte n'a �t� trouv�
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
	//  Variables permettant d'emp�cher la d�claration de plusieurs objets
	// THelp, de mani�re � n'avoir qu'un objet sur lequel pointer, accessible
	// par les hook procs.
		static THelp *	TheHelpObject ;
	
	// Fen�tre origine pour WinHelp
		TWindow *		MainWindow ;

	// El�ment de menu sur lequel on est positionn�
		uint			LastSelectedMenu ;
		uint			LastSelectedMenuType ;
		HMENU			LastSelectedMenuHandle ;

	// Hook pour capturer les demandes d'aide lorsqu'on est dans un menu
	// ou une bo�te de dialogue
		HHOOK			MsgFilterHookId ;


	// Ajout d'un nom de fichier � la table des noms
		int			AddHelpFileName  ( char *	filename ) ;

	// Donne le nombre de fois o� un fichier est r�f�renc�
		int			GetHelpFileUsage ( int		index ) ;
		int			GetHelpFileUsage ( char *	file ) ;

	// Ajout/suppression d'une window proc
		void			AddWindowProc	 ( int		index ) ;
		void			RemoveWindowProc ( int		index ) ;
							  
	
	// WindowProc qui va squatter celle des fen�tres	
		friend LRESULT CALLBACK  HelpWindowProc ( HWND		hwnd,
							  uint		message,
							  WPARAM	wparam,
							  LPARAM	lparam ) ;

	// Hook proc pour r�cup�rer les messages depuis un menu ou une boiboite
		friend LRESULT CALLBACK  MsgFilterHookProc   ( int, WPARAM, LPARAM ) ;
      } ;




/**************************************************************************/
/*                                                                        */
/*  Pour g�rer l'aide en ligne au moyen de l'objet THelp, il faut bien    */
/*  entendu l'allouer, puis appeler la fonction membre AddHelp pour       */
/*  ajouter une table de contextes pour une ou plusieurs fen�tres ou      */
/*  classes OWL.                                                          */
/*  Les macros suivantes permettent de d�finir de mani�re plus ais�e      */
/*  ces tables.                                                           */
/*                                                                        */
/*  Une d�finition de table doit poss�der le format suivant :             */
/*                                                                        */
/*  DEFINE_HELP  ( name )                                                 */
/*      window help                                                       */
/*      menu help                                                         */
/*      controls help                                                     */
/*  END_HELP ;                                                            */
/*                                                                        */
/*  "name" d�signe ici un nom qui doit �tre correct en C++, puisqu'il     */
/*  s'agira du nom utilis� pour cr�er un tableau de structures.           */
/*                                                                        */
/*  La table peut poss�der une, deux ou trois types d'entr�es (sous-      */
/*  tables) qui sont d�crits ici :                                        */
/*                                                                        */
/*  . windowhelp est la sous-table qui doit contenir les contextes        */
/*    relatifs � la fen�tre en g�n�ral; elle se d�finit ainsi :           */
/*                                                                        */
/*    DEFINE_WINDOW                                                       */
/*      contextdefs                                                       */
/*    END_WINDOW                                                          */
/*                                                                        */
/*    "contextdefs", qu'on retrouvera pour les autres types de sous-      */
/*    table, indique une suite de macros qui permettent d'associer un     */
/*    �l�ment fen�tre, contr�le ou autre � une cha�ne de caract�res ou    */
/*    un num�ro de contexte qui sont d�finis dans le fichier d'aide de    */
/*    l'application.                                                      */
/*                                                                        */
/*    Il existe deux macros pour d�finir ces contextes :                  */
/*                                                                        */
/*    . HELPID ( id, num )  associe � une fen�tre, dont le num�ro de      */
/*      contr�le est 'id', le num�ro de contexte 'num' (utilis� ensuite   */
/*      avec la fonction WinHelp et le param�tre HELP_CONTEXT).           */
/*                                                                        */
/*    . HELPKEY ( id, str ) associe � une fen�tre la cha�ne de caract�res */
/*      'str' (utilis�e ensuite avec la fonction WinHelp et le param�tre  */
/*      HELPKEY).                                                         */
/*                                                                        */
/*    Typiquement, l'aide pour une fen�tre normale sera d�finie par :     */
/*                                                                        */
/*      DEFINE_WINDOW                                                     */
/*              HELPID ( 0, HLC_some_context_number )                     */
/*      END_WINDOW                                                        */
/*                                                                        */
/*     (noter  l'absence de virgule � la fin de chaque appel de macro)    */
/*                                                                        */
/*     Lorsqu'il s'agit d'une aide relative � une bo�te de dialogue ou,   */
/*     plus g�n�ralement, � une classe OWL pour les bo�tes de dialogue,   */
/*     la partie "contextdefs" permet �ventuellement d'indiquer �         */
/*     quelles bo�tes de dialogue s'�pplique cette aide : il faut pour    */
/*     cela specifier autant de HELPID ou HELPKEY que n�cessaire :        */
/*                                                                        */
/*      DEFINE_WINDOW                                                     */
/*              HELPID ( IDD_DIALOG_1, 1 )                                */
/*              HELPID ( IDD_DIALOG_2, 2 )                                */
/*              ...                                                       */
/*      END_WINDOW                                                        */
/*                                                                        */
/*      Dans ce cas, l'aide ne s'appliquera qu'aux bo�tes de dialogue     */
/*      dont le num�ro a �t� d�fini ici.                                  */
/*                                                                        */
/*  . menuhelp est la sous-table qui permet d'associer un contexte � un   */
/*    �l�ment de menu; elle a le format suivant :                         */
/*                                                                        */
/*     DEFINE_MENU ( "Principal" )                                        */
/*              DEFINE_MENU ( "Fichier" )                                 */
/*                      HELPKEY ( IDC_OPEN, "Fichier/Ouvrir" )            */
/*                      ...                                               */
/*              END_MENU                                                  */
/*      END_MENU                                                          */
/*                                                                        */
/*    En r�gle g�n�rale, cette d�finition se conforme � celle contenue    */
/*    dans le fichier de ressources.                                      */
/*    L'aide sur un menu popup (un �l�ment de la barre de menu, ou un     */
/*    �l�ment de menu qui permet d'acc�der � un sous-menu) ne peut se     */
/*    sp�cifier qu'au moyen d'une cha�ne de caract�res (et non d'un       */
/*    num�ro de contexte). C'est un choix, et c'est comme �a.             */
/*                                                                        */
/*  . controlhelp est la sous-table o� l'on va associer � chaque contr�le */
/*    d'une bo�te de dialogue un num�ro de contexte ou une cha�ne de      */
/*    caract�res � utiliser avec WinHelp :                                */
/*                                                                        */
/*      DEFINE_CONTROL                                                    */
/*              HELPID ( IDC_EDIT1, 100 )                                 */
/*              HELPKEY  ( IDC_LISTBOX1, 101 )                            */
/*              ...                                                       */
/*      END_CONTROL                                                       */
/*                                                                        */
/*    A noter que cette sous-table n'est n�cessaire que s'i l'on d�sire   */
/*    une aide contextuelle (dans une bo�te de dialogue, l'appui sur une  */
/*    certaine s�quence de touches permettrait d'aboutir � l'aide         */
/*    relative au contr�le qui a le focus, et non � l'aide de la bo�te    */
/*    elle-m�me, par exemple).                                            */
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
/*  Les m�mes, pour un fichier de ressources.                             */
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
