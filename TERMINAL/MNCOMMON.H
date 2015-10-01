/**************************************************************************/
/*                                                                        */
/*   MNCommon.h  -  Christian Vigh, 95/10.                                */
/*                                                                        */
/*   Structures concernant la gestion du menu Cheops.                     */
/*                                                                        */
/**************************************************************************/



# if	! defined ( __MNCOMMON_H__ )
#	  define    __MNCOMMON_H__



# define	WM_MENU_EVENT			( WM_USER + 2678 )




/**************************************************************************/
/*                                                                        */
/*  Sous-fonctions du menu.                                               */
/*                                                                        */
/**************************************************************************/

# define	MNF_INITIALIZE			0
# define	MNF_END				1
# define	MNF_INTRO_BEGIN			2
# define	MNF_INTRO_END			3
# define	MNF_GET_APPLICATION_PATH	4
# define	MNF_USER_CONFIG			5
# define	MNF_USER_INFO			6
# define	MNF_CHEOPS_MODULES		7
# define	MNF_INI_FILE			8
# define	MNF_MENU_FILE			9
# define	MNF_EXECUTE			10
# define	MNF_MAIL_FILE			11
# define	MNF_INTRO_SETTEXT 		12
# define	MNF_READ_MAIL			13
# define	MNF_SEND_MAIL			14
# define	MNF_MAIL_EVENT			15
# define	MNF_CHECK_MAIL			16
# define	MNF_DELETE_MAIL			17
# define	MNF_OPEN_MENU			18
# define	MNF_SET_SHELL_MODE		19
# define	MNF_GET_FREE_RESOURCES		20



/**************************************************************************/
/*                                                                        */
/*  Macros d'assistance à la mise à plat des messages.                    */
/*                                                                        */
/**************************************************************************/

# define        copystr(d,s)            strcpy ( d, s ) ;       \
					d += strlen ( s ) + 1
# define        copych(d,s)             * d ++ = ( char ) s
# define        copyi16(d,s)            I16toCHAR ( s, d ) ;    \
					d += sizeof ( INTEGER16 )
# define        copyi32(d,s)            I32toCHAR ( s, d ) ;    \
					d += sizeof ( INTEGER32 )

# if	defined ( SCO_UNIX )
# define	getstr(d,s)		if ( d ) free ( d ) ;		\
					d = strclone ( s ) ;		\
					s += strlen ( s ) + 1 
# else
# define	getstr(d,s)		ifdelete ( d ) ;		\
					d = newstring ( s ) ;		\
					Device -> HostToLocalCharSet ( d ) ; \
					s += strlen ( s ) + 1 
# endif

# define	getch(d,s)		d = * s ++
# define	geti16(d,s)		CHARtoI16 ( d, s ) ;		\
					s += sizeof ( INTEGER16 ) 
# define	geti32(d,s)		CHARtoI32 ( d, s ) ;		\
					s += sizeof ( INTEGER32 ) 




/**************************************************************************/
/*                                                                        */
/*  TCheopsMenuItemType -                                                 */
/*      Type d'un élément de menu Cheops :                                */
/*                                                                        */
/*      . ProgramItem :                                                   */
/*              Programme                                                 */
/*                                                                        */
/*      . MenuItem :                                                      */
/*              Sous-menu                                                 */
/*                                                                        */
/*      . SeparatorItem :                                                 */
/*              Séparateur.                                               */
/*                                                                        */
/**************************************************************************/


typedef enum				
   {
		ProgramItem,
		MenuItem,
		SeparatorItem,
     }  TCheopsMenuItemType ;




/**************************************************************************/
/*                                                                        */
/*  TCheopsMenuItem -                                                     */
/*      Description d'un élément de menu :                                */
/*                                                                        */
/*      ItemName -                                                        */
/*              Nom de l'élément (code du menu ou du programme).          */
/*                                                                        */
/*      ItemText -                                                        */
/*              Libellée de l'élément.                                    */
/*                                                                        */
/*      ItemType -                                                        */
/*              Type de l'élément.                                        */
/*                                                                        */
/**************************************************************************/

typedef struct				
   {
		char *			ItemName ;
		char *			ItemText ;
		TCheopsMenuItemType     ItemType ;
     }   TCheopsMenuItem ;





/**************************************************************************/
/*                                                                        */
/*  TCheopsMenu -                                                         */
/*      Description d'un menu Cheops :                                    */
/*                                                                        */
/*      MenuName -                                                        */
/*              Nom du menu (code).                                       */
/*                                                                        */
/*      MenuText -                                                        */
/*              Libellé du menu.                                          */
/*                                                                        */
/*      MenuItems -                                                       */
/*              Elements du menu.                                         */
/*                                                                        */
/*      ItemCount -                                                       */
/*             Nombre d'éléments du menu.                                 */
/*                                                                        */
/**************************************************************************/


typedef struct				
   {	
		char *			MenuName ;
		char *			MenuText ;
		TCheopsMenuItem *	MenuItems ;
		int			ItemCount ;
     }  TCheopsMenu ;




/**************************************************************************/
/*                                                                        */
/*  TCheopsUserConfig -                                                   */
/*      Description de la configuration de l'utilisateur :                */
/*                                                                        */
/*      SiteName -                                                        */
/*              NOMSITE.                                                  */
/*                                                                        */
/*      PrinterName -                                                     */
/*              PRINTER.                                                  */
/*                                                                        */
/*      Year -                                                            */
/*              EXERCICE.                                                 */
/*                                                                        */
/*      Month -                                                           */
/*              HPAGE.                                                    */
/*                                                                        */
/*      Protection -                                                      */
/*              PROT.                                                     */
/*                                                                        */
/*      ProgramPath -                                                     */
/*              PROGS.                                                    */
/*                                                                        */
/*      DescripPath -                                                     */
/*              DESCRIPT.                                                 */
/*                                                                        */
/*      DataPath -                                                        */
/*              DONNEES.                                                  */
/*                                                                        */
/**************************************************************************/

typedef struct				
   {
		char *			SiteName ;
		char *			PrinterName ;
		int			Year ;
		int			Month ;
		char *			Protection ;
		char *			ProgramPath ;
		char *			DescriptPath ;
		char *			DataPath ;
     }  TCheopsUserConfig ;



/**************************************************************************/
/*                                                                        */
/*  TCheopsUserInfo -                                                     */
/*      Informations utilsateur provenant d'Unix.                         */
/*                                                                        */
/*      UserName -                                                        */
/*              Nom de l'utilisateur.                                     */
/*                                                                        */
/*      TTYName -                                                         */
/*              Nom de son TTY.                                           */
/*                                                                        */
/*      UnixPrinters -                                                    */
/*              Noms des imprimantes actuellement définies sous Unix.     */
/*                                                                        */
/*      UnixPrinterCount -                                                */
/*              Nombre d'imprimantes Unix définies.                       */
/*                                                                        */
/**************************************************************************/

typedef struct           		
   {
		char *			UserName ;
		char *			TTYName ;
		char			IsSuperUser ;
		char **			UnixPrinters ;
		int			UnixPrinterCount ;
    }  TCheopsUserInfo ;




/**************************************************************************/
/*                                                                        */
/*  TCheopsModule -                                                       */
/*      Description d'un module Cheops (apparaissant dans la barre        */
/*      d'onglets).                                                       */
/*                                                                        */
/*      ModuleName -                                                      */
/*              Nom du module.                                            */
/*                                                                        */
/*      ModuleText -                                                      */
/*              Intitulé du module.                                       */
/*                                                                        */
/*      ModuleColor -                                                     */
/*              Couleur de référence du module.                           */
/*                                                                        */
/**************************************************************************/


typedef struct				
   {
		char *			ModuleName ;
		char *			ModuleText ;
		COLORREF		ModuleColor ;
    }  TCheopsModule ;






/**************************************************************************/
/*                                                                        */
/*   TCheopsHistory -                                                     */
/*      Définition d'un enregistrement de l'historique.                   */
/*                                                                        */
/**************************************************************************/

typedef struct
   {
		char		Month ;		/* Mois de config			*/
		int		Year ;		/* Exerice                              */ 
		char *		Printer ;	/* imprimante de config.                */
		char *		MenuName ;	/* Menu d'origine du programme          */
		char *		ProgramName ;	/* Chemin complet du programme          */
		char **		Arguments ;	/* Arguments                            */
		int		ArgumentCount ;	/* Nombre d'arguments pour ce programme */
		int		ReturnValue ;	/* Valeur de retour                     */
		long int	ExecutionStart ;/* Heure de début de lancement          */
		long int	UserTime,	/* CPU User                             */
				SystemTime,	/* CPU système                          */
				ElapsedTime ;	/* Temps elapsed                        */
      }  TCheopsHistory ;


# endif		/*  __MNCOMMON_H__  */
