/**************************************************************************/
/*                                                                        */
/*  Config.cpp  -  Christian Vigh, 95/07/19.                              */
/*                                                                        */
/*  Gestion des paramètres de configuration (implémentation de la classe  */
/*  TConfiguration).                                                      */
/*                                                                        */
/**************************************************************************/


# include	"AllTerm.h"
# pragma	hdrstop


# include	"UnixApp.h"



TConfiguration 	*		Configuration ;
TSessionParameters *		SessionParameters ;
TConnectionParameters *         ConnectionParameters ;



# define	IfDiffStr(f1,s)		( ( IsDefault  ||  strcmp ( s. f1, Default##s##Parameters. f1 ) ) ? \
						Session. f1 : NULL ) 
# define	IfDiffVal(f1,str,s) 	( ( IsDefault  ||  s. f1  !=  Default##s##Parameters. f1 ) ?  \
						str : NULL )  
# define	IfDiffBool(f1,s)    	( ( IsDefault  || s. f1  !=  Default##s##Parameters. f1 ) ? \
						( ( s. f1 ) ?  "True" : "False" ) : NULL )

# define	IfDiffStr2(f1,s,s2)	( ( IsDefault  ||  strcmp ( s. f1, Default##s2##Parameters. f1 ) ) ? \
						s. f1 : NULL ) 
# define	IfDiffVal2(f1,str,s,s2) ( ( IsDefault  ||  s. f1  !=  Default##s2##Parameters. f1 ) ?  \
						str : NULL )  


/**************************************************************************/
/*                                                                        */
/*  TConfiguration :: Initialize                                          */
/*      Initialise la configuration (remet les structures à zero).        */
/*                                                                        */
/**************************************************************************/

void	TConfiguration :: Initialize  ( void )
   {
	CurrentSession	=  -1 ;
	Loaded          = FALSE ;
	PreventAutoOpen = FALSE ;

	SetDirty ( FALSE ) ;

	memset ( & TerminalParameters      , 0, sizeof ( TerminalParameters ) ) ;
	memset ( & DefaultSessionParameters, 0, sizeof ( DefaultSessionParameters ) ) ;
	memset ( & DefaultNetworkParameters, 0, sizeof ( DefaultNetworkParameters ) ) ;
	memset ( & DefaultSerialParameters , 0, sizeof ( DefaultSerialParameters ) ) ;

	memset ( Sessions   , 0, sizeof ( Sessions ) ) ;
	memset ( Connections, 0, sizeof ( Connections ) ) ;

	GetApplicationDirectory  ( TerminalParameters. ApplicationDirectory ) ;

	for  ( int  i = 0 ; i < MAX_SESSIONS ; i ++ )
		SessionStates [i] = Free ;

	LogFileFP	=  NULL ;
	* LogFileName   =  0 ;
     }





/**************************************************************************/
/*                                                                        */
/*  TConfiguration :: FreeResources                                       */
/*      Libère les ressources allouées.                                   */
/*      Les TEditWindowClient étant filles de TTerminalWindow, elles ont  */
/*      déjà été supprimées par cette dernière.                           */
/*                                                                        */
/*                                                                        */
/**************************************************************************/

void	TConfiguration :: FreeResources  ( void )
   {
	if  ( ! PreventAutoOpen )	// Si c'est la vraie config et non la
	   {				// temporaire utilisée dans les boîtes de config
		WriteString ( SETTINGS_SECTION, "Exit", "TRUE" ) ;

		for  ( int  i = 0 ; i < MAX_SESSIONS ; i ++ )
		    {
			if  ( SessionStates [i]  !=  Free )
			   {
				if  ( Connections [i]. TerminalWindowClient   &&
				      Connections [i]. TerminalWindowClient -> HWindow )
					delete  Connections [i]. TerminalWindowClient ;
				if  ( Connections [i]. Connection )
					delete  Connections [i]. Connection ;
				if  ( Connections [i]. Device )
					delete  Connections [i]. Device ;
			     }
		      }
	     }
      }

				



/**************************************************************************/
/*                                                                        */
/*  TConfiguration :: GetSectionList                                      */
/*      Permet de récupérer la liste de toutes les sections d'un fichier  */
/*      .INI.                                                             */
/*      La liste est mise dans le paramètre "Buffer". Elle est terminée   */
/*      par un double zero.                                               */
/*      En attendant Win32, où le GetProfileString permet de récupérer    */
/*      cette liste, il faut se palucher le .INI à la main.               */
/*                                                                        */
/**************************************************************************/

void	TConfiguration :: GetSectionList  ( char *	List,
					    int		ListSize )
   {
	char			buffer [ MAX_PARAMETER_LENGTH ] ;
	char			key    [ MAX_PARAMETER_LENGTH ] ;
	register char *	        p,
		      *		q ;
	register FILE *		fp		=  fopen ( TerminalParameters. IniFileName, "r" ) ;
	register int		i ;



// Lecture des lignes du .INI
	while  ( fgets ( buffer, sizeof ( buffer ), fp )  !=  NULL )
	   {
		p = buffer, q = key ;

	// On saute les blancs (raciste!)
		while  ( isspace ( * p ) )
			p ++ ;

	// Une section commence toujours par le caractère '['
		if  ( * p  !=  '[' )
			continue ;
		p ++ ;			// Sauter le crochet

	// Récupérer le nom de section
		while  ( * p  &&  * p  !=  ']' )
			* q ++ = * p ++ ;
		q -- ;

	// Supprimer les espaces de fin
		while  ( q  >=  key  &&  isspace ( * q ) )
			q -- ;
		q ++ ;			// Se mettre après le dernier caractère
		* q = 0 ;		// ... pour terminer 'key' par un zero

	// Ne pas prendre en compte une section sans nom
		i = strlen ( key ) + 1 ;

		if  ( i  ==  1 )
			continue ;

	// Tester le débordement
		if  ( i + 1  >  ListSize )		// Le '+1' est pour le 2e zero de fin
			break ;

	// S'il y a de la place, on recopie
		strcpy ( List, key ) ;
		List += i ;
		ListSize -= i ;
	    }

// C'est fini, on ferme
	fclose ( fp ) ;
     }






/**************************************************************************/
/*                                                                        */
/*  TConfiguration :: GetSectionItems                                     */
/*      Récupère les éléments d'une section.                              */
/*                                                                        */
/**************************************************************************/

void	TConfiguration :: GetSectionItems  ( char *	section,
					     char * 	buffer,
					     int	size )
   {
	GetPrivateProfileString  ( section, NULL, "", buffer, size,
					TerminalParameters. IniFileName ) ;
    }






/**************************************************************************/
/*                                                                        */
/*  TConfiguration :: ReadString, WriteString                             */
/*      Lecture/écriture directes d'éléments dans le fichier .INI         */
/*                                                                        */
/**************************************************************************/

int	TConfiguration :: ReadString  ( char * 		section,
					char *		key,
					char *		defval,
					char *		buffer,
					int		buffer_size,
					BOOL		IsDefault ) 
   {
	register int	i ;


	if  ( IsDefault )
		return ( GetPrivateProfileString ( section, key, defval, buffer, 
				buffer_size, TerminalParameters. IniFileName ) ) ;

	i = GetPrivateProfileString ( section, key, "", buffer, buffer_size,
					TerminalParameters. IniFileName ) ;

	if  ( ! i )
		i = GetPrivateProfileString ( DEFAULTS_SECTION, key, defval, buffer, 
				buffer_size, TerminalParameters. IniFileName ) ;

	return ( i ) ;
    }              



int	TConfiguration :: WriteString  ( char * 	section,
					 char *		key,
					 char *		buffer )
   {
	return ( WritePrivateProfileString ( section, key, buffer, 
					TerminalParameters. IniFileName ) ) ;
    }





/**************************************************************************/
/*                                                                        */
/*  TConfiguration :: GetBoolean                                          */
/*      Retourne la valeur booléenne d'une option dans le .INI            */
/*                                                                        */
/**************************************************************************/

BOOL	TConfiguration :: GetBoolean  ( char *		section,
					char *		key,
					BOOL		defval )
   {
	char		buffer [ MAX_PARAMETER_LENGTH ] ;


	if  ( ! ReadString ( section, key, "", buffer, sizeof ( buffer ) ) )
		return ( defval ) ;

	if  ( ! stricmp ( buffer, "True" )  ||
	      ! stricmp ( buffer, "Yes" )   ||
	      ! stricmp ( buffer, "On" )    ||
	      ! stricmp ( buffer, "1" ) )
		return ( TRUE ) ;
	else
		return ( FALSE ) ;
    }





/**************************************************************************/
/*                                                                        */
/*  TConfiguration :: IsNetworkLink                                       */
/*      Retourne la valeur du paramètre "NetworkLink" pour la section     */
/*      spécifiée.                                                        */
/*                                                                        */
/**************************************************************************/

BOOL	TConfiguration :: IsNetworkLink  ( char *   section )
   {
	char		buffer [ MAX_PARAMETER_LENGTH ] ;

	
	ReadString ( section, "LinkType", "False", buffer, sizeof ( buffer ) ) ;

	if  ( ! stricmp ( buffer, "Network" )   ||
	      ! stricmp ( buffer, "Net" )	||
	      ! stricmp ( buffer, "N" ) )
		return ( TRUE ) ;
	else if  ( ! stricmp ( buffer, "Serial" )  ||
		   ! stricmp ( buffer, "S" )  	   ||
		   ! stricmp ( buffer, "RS232" ) )
		return ( FALSE ) ;
	else
		return ( FALSE ) ;
     }




/**************************************************************************/
/*                                                                        */
/*  TConfiguration :: GetApplicationDirectory                             */
/*      Récupère le répertoire de l'application et le retourne, terminé   */
/*      par un '\\'.                                                      */
/*                                                                        */
/**************************************************************************/

void  TConfiguration :: GetApplicationDirectory  ( char *   dir )
   {
	register char *		p ;


	TerminalApplication -> GetModuleFileName ( dir, MAX_PARAMETER_LENGTH ) ;
	p = dir + strlen ( dir ) - 1 ;

	while  ( p >= dir )
	   {
		if  ( strchr ( "\\/:", * p )  !=  NULL )
			break ;
		p -- ;
	    }

	p ++ ;
	* p = 0 ;
    }




/**************************************************************************/
/*                                                                        */
/*   TConfiguration :: GetConfigurationPath                               */
/*      Retourne le chemin du fichier .INI de l'application en cherchant: */
/*                                                                        */
/*      . Dans le répertoire de l'application                             */
/*      . Dans le répertoire de Windows.                                  */
/*                                                                        */
/*      S'il n'existe pas, la fonction retourne son nom dans le           */
/*      répertoire de l'application.                                      */
/*                                                                        */
/**************************************************************************/

BOOL	TConfiguration :: GetConfigurationPath  ( char *	path )
   {
	char			WinDir	[ MAX_PARAMETER_LENGTH ] ;


// Rechercher le nom du fichier dans le répertoire courant
//aze	strcpy ( path, TerminalIniFileName ) ;

	if  ( access ( path, 0 )  >=  0 )
		return ( TRUE ) ;


// Sinon rechercher dans le répertoire de l'application
	strcpy ( path, TerminalParameters. ApplicationDirectory ) ;
	strcat ( path, TerminalIniFileName ) ;


// Si le .INI n'est pas trouvé dans le répertoire de l'application, on
// cherche dans celui de Windows
	if  ( access ( path, 0 )  <  0 )
	   {
		GetWindowsDirectory ( WinDir, sizeof ( WinDir ) ) ;

		if  ( strchr ( "\\/", WinDir [ strlen ( WinDir ) - 1 ] )  ==  NULL )
			strcat ( WinDir, "\\" ) ;
		strcat ( WinDir , TerminalIniFileName ) ;

		if  ( ! access ( WinDir, 0 ) )
			strcpy ( path, WinDir ) ;
		else
			return ( FALSE ) ;
	     }

	return ( TRUE ) ;
     }







/**************************************************************************/
/*                                                                        */
/*   TConfiguration :: ConvertEmulationPath                               */
/*      Lorsque les fichiers de définition d'une émulation sont dans le   */
/*      répertoire de l'application, il est souhaitable de ne pas         */
/*      conserver la partie 'répertoires', pour ne garder que le nom de   */
/*      fichier. De cette manière, on peut déplacer l'ensemble des        */
/*      fichiers de l'application d'un répertoire à l'autre, sans que     */
/*      les noms d'émulation associés à chaque session s'en trouvent      */
/*      erronés.                                                          */
/*      ConvertEmulationPath permet alors :                               */
/*                                                                        */
/*      . De transformer le nom relatif de l'émulation en chemin absolu,  */
/*        si 'absolute' est a 'true'                                      */
/*      . De supprimer ce chemin absolu s'il est identique au répertoire  */
/*        de l'application, lorsque 'absolute' est à FALSE.               */
/*                                                                        */
/**************************************************************************/

void	TConfiguration :: ConvertEmulationPath  ( char *	path,
						  BOOL		absolute )
   {
	char			Buffer [ MAX_PARAMETER_LENGTH ] ;
	register int		i ;

	
	if  ( absolute )	// Conversion relatif -> absolu
	   {
		if  ( * path  !=  '\\'  &&  * path   !=  '/'   &&  path [1]  !=  ':' )
		   {		
			strcpy ( Buffer, TerminalParameters. ApplicationDirectory ) ;
			strcat ( Buffer, path ) ;
			strcpy ( path, Buffer ) ;
		     }
	    }
	else			// Conversion absolu -> relatif
	   {
		i = strlen ( TerminalParameters. ApplicationDirectory ) ;

		if  ( ! strncmp ( path, TerminalParameters. ApplicationDirectory, i ) )
			strcpy ( path, path + i ) ;
	     }
     }





/**************************************************************************/
/*                                                                        */
/*   TConfiguration :: ReadSessionParameters                              */
/*      Lit les paramètres relatifs à une session à partir de la          */
/*      section specifiée (Defaults ou nom de session).                   */
/*                                                                        */
/**************************************************************************/

void	TConfiguration :: ReadSessionParameters  ( char *		 section,
						   TSessionParameters &  Session, 
						   BOOL 		 IsDefault )
   {
	char			buffer      [ MAX_PARAMETER_LENGTH ] ;
	register int		i ;


// Nom réel de la section
	if  ( ! IsDefault )
		section = SessionSectionName  ( section ) ;


//  Police de caractères : nom, hauteur, largeur
	ReadString ( section, "Font", "System", Session. FontName, 
			sizeof ( Session. FontName ), IsDefault ) ;

	ReadString ( section, "FontHeight", "8", buffer, sizeof ( buffer ), IsDefault ) ;
	i = atoi ( buffer ) ;

	if  ( abs ( i ) < 5  ||  abs ( i ) >  128 )
		i = 8 ;
	Session. FontHeight = i ;


	ReadString ( section, "FontWidth", "default", buffer, sizeof ( buffer ), IsDefault ) ;

	if  ( ! stricmp ( buffer, "default" ) )
		Session. FontWidth = 0 ;
	else
	   {
		i = atoi ( buffer ) ;

		if  ( i  &  ( abs ( i ) < 5  ||  abs ( i ) >  128 ) )
			i = 0 ;
		Session. FontWidth = i ;
	     }


// Couleurs de texte et de fond
	ReadString ( section, "BackgroundColor", "default", buffer, sizeof ( buffer ), IsDefault ) ;

	if  ( ! stricmp ( buffer, "default" ) )
		Session. BackgroundColor = GetSysColor ( COLOR_WINDOW ) ;
	else
		Session. BackgroundColor = ( COLORREF ) StringToLong ( buffer ) ;

	
	ReadString ( section, "ForegroundColor", "default", buffer, sizeof ( buffer ), IsDefault ) ;

	if  ( ! stricmp ( buffer, "default" ) )
		Session. ForegroundColor = GetSysColor ( COLOR_WINDOWTEXT ) ;
	else
		Session. ForegroundColor = ( COLORREF ) StringToLong ( buffer ) ;


// Mode transparent (affichage de tous les caractères)
	Session. TransparentMode = GetBoolean ( section, "TransparentMode", 
			( IsDefault ) ?  FALSE : DefaultSessionParameters. TransparentMode ) ;

// Echo local
	Session. LocalEcho = GetBoolean ( section, "Echo", 
			 ( IsDefault ) ?  FALSE : DefaultSessionParameters. LocalEcho ) ;

// Titre de la session
	ReadString ( section, "WindowTitle", "", Session. WindowTitle,
			sizeof ( Session. WindowTitle ), TRUE ) ;

// Jeu de caractères distant
	ReadString ( section, "HostCharSet", "OEM", buffer, sizeof ( buffer ), IsDefault ) ;

	if  ( ! stricmp ( buffer, "ansi" ) )
		Session. HostCharSet = AnsiCharSet ;
	else if  ( ! stricmp ( buffer, "OEM" ) )
		Session. HostCharSet = OemCharSet ;
	else
		Session. HostCharSet = OemCharSet ;

	    
// Nom de l'émulation
	ReadString ( section, "Emulation", "Ansi.trm", Session. Emulation, 
			sizeof ( Session. Emulation ), IsDefault ) ;
	ConvertEmulationPath ( Session. Emulation, TRUE ) ;
    }




/**************************************************************************/
/*                                                                        */
/*   TConfiguration :: ReadConnectionParameters                           */
/*      Récupère les paramètres relatifs à une connexion réseau (si       */
/*      network_link = true) ou série (network_link = false).             */
/*                                                                        */
/**************************************************************************/

void	TConfiguration :: ReadConnectionParameters ( char *		     section,
						     TConnectionParameters & Conn,
						     BOOL		     network_link,
						     BOOL		     IsDefault )
    {
	char		buffer      [ MAX_PARAMETER_LENGTH ] ;
	register int	i ;


// Nom réel de la section
	if  ( ! IsDefault )
		section = SessionSectionName ( section ) ;


	Conn. NetworkLink = network_link ;

// Username et password pour les connexions automatiques
	ReadString ( section, "User", "", Conn. UserName, sizeof ( Conn. UserName ),
		IsDefault ) ;
	ReadString ( section, "Password", "", Conn. UserPassword,
		sizeof ( Conn. UserPassword ), IsDefault ) ;


// Cas du réseau
	if  ( network_link )
	   {

	// Nom du serveur
		ReadString ( section, "Server", "", Conn. Network. ServerName,
				sizeof ( Conn. Network. ServerName ), IsDefault ) ;

	// Délai entre l'émission de deux paquets
		ReadString ( section, "Delay", "0", buffer, sizeof ( buffer ), IsDefault ) ;
		Conn. Network. Delay = atoi ( buffer ) ;
	     }
// Cas de la liaison série
	else
	   {
	
	// Nom du port série
		ReadString ( section, "Port", "COM1", Conn. Serial. PortName,
				sizeof ( Conn. Serial. PortName ), IsDefault ) ;

	// Vitesse de transmission
		ReadString ( section, "Speed", "9600", buffer, sizeof ( buffer ), IsDefault ) ;

		Conn. Serial. Speed = StringToSpeed ( buffer ) ;
		Conn. Serial. RealSpeed = atol ( buffer ) ;

		if  ( Conn. Serial. RealSpeed  <  300 )
			Conn. Serial. RealSpeed = 9600 ;


	// Nombre de bits de stop
		ReadString ( section, "StopBits", "1", buffer, sizeof ( buffer ), IsDefault ) ;

		if  ( ! strcmp ( buffer, "1.5" ) )
			Conn. Serial. StopBits	=  ONE5STOPBITS ;
		else if  ( ! strcmp ( buffer, "2" ) ) 
			Conn. Serial. StopBits  =  TWOSTOPBITS ;
		else					// '1' ou n'importe quoi...
			Conn. Serial. StopBits  =  ONESTOPBIT ;

	// Nombre de bits de données
		ReadString ( section, "DataBits", "8", buffer, sizeof ( buffer ), IsDefault ) ;
		i = atoi ( buffer ) ;

		if  ( i < 4  ||  i < 8 )
			i = 8 ;
		Conn. Serial. DataBits = i ;

	// Type de parité
		ReadString ( section, "Parity", "None", buffer, sizeof ( buffer ), IsDefault ) ;
		Conn. Serial. Parity = StringToParity ( buffer ) ;
	   }
    }

		
	     

/**************************************************************************/
/*                                                                        */
/*  TConfiguration :: Read                                                */
/*      Lit les paramètres de configuration stockés dans le fichier       */
/*      .INI (Terminal.ini par défaut).                                   */
/*                                                                        */
/**************************************************************************/

TConfiguration :: TReadStatus   TConfiguration :: Read  ( char *     configname )
   {
	char			buffer        [ MAX_PARAMETER_LENGTH ] ;
	char			ConfSection   [ MAX_PARAMETER_LENGTH ] ;
	char			sessions      [ MAX_PARAMETER_LENGTH ] ;
	register int		i ;
	register BOOL		network_link ;
	register char *		p ;


// On détermine l'emplcaement du fichier INI
	if  ( ! GetConfigurationPath  ( TerminalParameters. IniFileName ) )
		return ( NoIniFile ) ;

// Récupération du nom du fichier d'aide
	GetApplicationDirectory ( HelpFileName ) ;

# if	defined ( CHEOPS )
	strcpy ( CheopsHelpFileName, HelpFileName ) ;
	strcat ( CheopsHelpFileName, CHEOPS_HLP ) ;

	TerminalParameters. OldFunctionKeys = GetBoolean ( SETTINGS_SECTION, "OldFunctionKeys", TRUE ) ;
# endif

	strcat ( HelpFileName, TERMINAL_HLP ) ;

// Paramètres généraux : état de la dernière fermeture de session
	TerminalParameters. LastExitWasOk = GetBoolean ( SETTINGS_SECTION, "Exit", FALSE ) ;
	WriteString ( SETTINGS_SECTION, "Exit", "False" ) ;

// Paramètres généraux : Lambda user (modification de config interdite si true)
	TerminalParameters. LambdaUser = GetBoolean ( SETTINGS_SECTION, "LambdaUser", FALSE ) ;

// Paramètres généraux : Ouverture auto des sessions
	TerminalParameters. AutoOpen = GetBoolean ( SETTINGS_SECTION, "AutoOpen", FALSE ) ;


// Paramètres généraux : Type de beep
	TerminalParameters. AlarmBeep = GetBoolean ( SETTINGS_SECTION, "Beep", FALSE ) ;


// Paramètres généraux : form feed après un hardcopy
	TerminalParameters. FormFeedOnHardcopy = GetBoolean ( SETTINGS_SECTION, "FFOnHardcopy", FALSE ) ;

// Paramètres généraux : tracer un rectangle autour d'une recopie-écran
	TerminalParameters. FrameHardcopy = GetBoolean ( SETTINGS_SECTION, "FrameHardcopy", FALSE ) ;

// Paramètres généraux : Taille mini d'une saisie pour qu'elle soit ajoutée à
// la redo list
	ReadString ( SETTINGS_SECTION, "MinRedoSize", "4", buffer, sizeof ( buffer ) ) ;
	i = atoi ( buffer ) ;

	if  ( i < 1  ||  i >  16 )
		i = 4 ;			// Valeur raisonnable

	TerminalParameters. MinRedoSize = i ;


// Paramètres généraux : Délai d'ouverture des menus Cheops (en millisecondes)
// 0 = pas d'ouverture automatique
	ReadString ( SETTINGS_SECTION, "AutoOpenDelay", "1500", buffer, sizeof ( buffer ) ) ;
	i = atoi ( buffer ) ;

	if  ( i < 0 )
		i = 0 ;
	TerminalParameters. MenuAutoOpenDelay = i ;


// Paramètres généraux : dernière position
	ReadString ( SETTINGS_SECTION, "LastPositionX", "20", buffer, sizeof ( buffer ) ) ;
	TerminalParameters. LastX = atoi ( buffer ) ;
	ReadString ( SETTINGS_SECTION, "LastPositionY", "20", buffer, sizeof ( buffer ) ) ;
	TerminalParameters. LastY = atoi ( buffer ) ;

// Paramètres généraux : confirmation sur sortie de l'application
	TerminalParameters. ConfirmOnExit = GetBoolean ( SETTINGS_SECTION, "ConfirmOnExit",
						TRUE ) ;
	

ShootAgain :
// Paramètres par défaut : configuration actuelle
	if  ( configname  ==  NULL )
	   {
		ReadString ( SETTINGS_SECTION,  "Configuration", "", 
				TerminalParameters. Configuration,
					sizeof ( TerminalParameters. Configuration ) ) ;
	     }
	else
		strcpy ( TerminalParameters. Configuration, configname ) ;
	strcpy ( ConfSection, ConfigurationSectionName ( TerminalParameters. Configuration ) ) ;


					
// Récupération des valeurs par défaut
	DefaultNetworkLink = IsNetworkLink ( DEFAULTS_SECTION ) ;
	ReadSessionParameters ( DEFAULTS_SECTION, DefaultSessionParameters, TRUE ) ;
	ReadConnectionParameters ( DEFAULTS_SECTION, DefaultNetworkParameters, TRUE, TRUE ) ;
	ReadConnectionParameters ( DEFAULTS_SECTION, DefaultSerialParameters, FALSE, TRUE ) ;



// Ce test succède au ReadString (Configuration). Il est placé ici afin de
// permettre la lecture des paramètres par défaut
	if  ( ! * TerminalParameters. Configuration )
		return ( NoConfiguration ) ;



// Récupération des noms de session
	GetSectionItems ( ConfSection, sessions, sizeof ( sessions ) ) ;

	if  ( ! * sessions )
	   {
		if  ( PreventAutoOpen )
			return ( NoSession ) ;

		if  ( MessageBox ( TerminalWindow -> HWindow, "Erreur de paramétrage",
			MB_YESNOCANCEL | MB_ICONEXCLAMATION,
			"Aucune session n'est définie pour la configuration %s; "
			"voulez-vous en ajouter maintenant ?",
				TerminalParameters. Configuration )  ==  IDYES )
			TSessionsDialog ( TerminalWindow, this ). Execute ( ) ;			
		return ( NoSession ) ;
	     }


// Lecture des paramètres relatifs à chaque session	
	for  ( p = sessions, i = 0 ; * p   &&  i  <  MAX_SESSIONS ; 
						i ++, p += strlen ( p ) + 1 )
	   {
		ReadString ( ConfSection, p, "", TerminalParameters. SessionSections [i],
			sizeof ( TerminalParameters. SessionSections [i] ) ) ;
		network_link = IsNetworkLink ( SessionSectionName ( 
					TerminalParameters. SessionSections [i] ) ) ;

		ReadConnectionParameters ( TerminalParameters. SessionSections [i], 
				Connections [i], network_link, FALSE ) ;
		ReadSessionParameters ( TerminalParameters. SessionSections [i], 
				Sessions [i], FALSE ) ;
		Sessions [i]. LocalSession = FALSE ;

		SessionStates [i] = Created ;

		if  ( ! PreventAutoOpen )
		   {
			CreateConnection ( i ) ;		
			SetCurrentSession ( i ) ;
		
			if  ( TerminalParameters. AutoOpen )
				TerminalWindow -> IDMFileConnect ( ) ;
		    }
	    }

// A fini
	Loaded = TRUE ;
	SetCurrentSession ( 0 ) ;
	return ( Ok ) ;
    }





/**************************************************************************/
/*                                                                        */
/*   TConfiguration :: WriteSessionParameters                             */
/*      Réécrit les paramètres relatifs à une session à partir de la      */
/*      section specifiée (Defaults ou nom de session).                   */
/*                                                                        */
/**************************************************************************/

void	TConfiguration :: WriteSessionParameters  ( char *		  section,
						    TSessionParameters &  Session,
						    BOOL		  IsDefault )
   {
	char			buffer      [ MAX_PARAMETER_LENGTH ] ;
	register char *		p ;


// Nom réel de la section
	if  ( ! IsDefault )
		section = SessionSectionName ( section ) ;

//  Police de caractères : nom, hauteur, largeur
	WriteString ( section, "Font", IfDiffStr ( FontName, Session ) ) ;

	wsprintf ( buffer, "%d", Session. FontHeight ) ;
	WriteString ( section, "FontHeight", IfDiffVal ( FontHeight, buffer, Session ) ) ;

	if  ( Session. FontWidth )
	   {
		wsprintf ( buffer, "%d", Session. FontWidth ) ;
		WriteString ( section, "FontWidth", IfDiffVal ( FontWidth, buffer, Session ) ) ;
	    }
	else
		WriteString ( section, "FontWidth", IfDiffVal ( FontWidth, "default", Session ) ) ;


// Couleurs de texte et de fond
	if  ( Session. BackgroundColor  ==  GetSysColor ( COLOR_WINDOW ) )
		WriteString ( section, "BackgroundColor", IfDiffVal ( BackgroundColor, "default", Session ) ) ;
	else
	   {
		wsprintf ( buffer, "%ld", Session. BackgroundColor ) ;
		WriteString ( section, "BackgroundColor", IfDiffVal ( BackgroundColor, buffer, Session ) ) ;
	    }

	if  ( Session. ForegroundColor  ==  GetSysColor ( COLOR_WINDOWTEXT ) )
		WriteString ( section, "ForegroundColor", IfDiffVal ( ForegroundColor, "default", Session ) ) ;
	else
	   {
		wsprintf ( buffer, "%ld", Session. ForegroundColor ) ;
		WriteString ( section, "ForegroundColor", IfDiffVal ( ForegroundColor, buffer, Session ) ) ;
	    }


// Mode transparent (affichage de tous les caractères)
	WriteString ( section, "TransparentMode", IfDiffBool ( TransparentMode, Session ) ) ;

// Echo local
	WriteString ( section, "Echo", IfDiffBool ( LocalEcho, Session ) ) ;

// Titre de la session
	WriteString ( section, "WindowTitle", Session. WindowTitle ) ;

// Jeu de caractères distant
	switch  ( Session. HostCharSet )
	   {
		case	AnsiCharSet :
			p = "Ansi" ;
			break ;

		case    OemCharSet :
		default :
			p = "Oem" ;
	    }
	WriteString ( section, "HostCharSet", IfDiffVal ( HostCharSet, p, Session ) ) ;

	    
// Nom de l'émulation
	strcpy ( buffer, Session. Emulation ) ;
	ConvertEmulationPath ( buffer, FALSE ) ;
	WriteString ( section, "Emulation", IfDiffVal ( Emulation, buffer, Session ) ) ;
    }




/**************************************************************************/
/*                                                                        */
/*   TConfiguration :: WriteConnectionParameters                          */
/*      Réécrit les paramètres relatifs à une connexion réseau (si        */
/*      network_link = true) ou série (network_link = false).             */
/*                                                                        */
/**************************************************************************/

void	TConfiguration :: WriteConnectionParameters ( char *		      section,
						      TConnectionParameters & Connection,
						      BOOL		      network_link,
						      BOOL		      IsDefault )
    {
	char			buffer     [ MAX_PARAMETER_LENGTH ] ;
	register char *         p ;


// Nom réel de la section
	if  ( ! IsDefault )
		section = SessionSectionName ( section ) ;


	WriteString ( section, "LinkType", ( network_link ) ?  "Network" : "Serial" ) ;
	WriteString ( section, "User", Connection. UserName ) ;
	WriteString ( section, "Password", Connection. UserPassword ) ;


// Cas du réseau
	if  ( network_link )
	   {

	// Nom du serveur
		WriteString ( section, "Server", Connection. Network. ServerName ) ;

	// Délai entre l'émission de deux paquets
		wsprintf ( buffer, "%d", Connection. Network. Delay ) ;
		WriteString ( section, "Delay", buffer ) ;
	     }
// Cas de la liaison série
	else
	   {
	
	// Nom du port série
		WriteString ( section, "Port", Connection. Serial. PortName ) ;

	// Vitesse de transmission
		WriteString ( section, "Speed", 
			SpeedToString ( Connection. Serial. Speed ) ) ;


	// Nombre de bits de stop
		switch  ( Connection. Serial. StopBits )
		   {
			case	ONE5STOPBITS :
				p = "1.5" ;
				break ;

			case	TWOSTOPBITS :
				p = "2" ;
				break ;

			case	ONESTOPBIT :
			default :
				p = "1" ;
		     }

		WriteString ( section, "StopBits", p ) ;


	// Nombre de bits de données
		wsprintf ( buffer, "%d", Connection. Serial. DataBits ) ;
		WriteString ( section, "DataBits", buffer ) ;

	// Type de parité
		WriteString ( section, "Parity", 
				ParityToString ( Connection. Serial. Parity ) ) ;
	   }
    }

		
	     

/**************************************************************************/
/*                                                                        */
/*  TConfiguration :: Write                                               */
/*      Réécrit les paramètres de configuration stockés dans le fichier   */
/*      .INI (Terminal.ini par défaut).                                   */
/*                                                                        */
/**************************************************************************/

void	TConfiguration :: Write  ( BOOL  forced ) 
   {
	char			buffer   [ MAX_PARAMETER_LENGTH ] ;
	register int		i, j ;


	if  ( ! IsDirty ( )  &&  ! forced )
		return ;


# if	defined ( CHEOPS )

// Style des touches de fonctions (à l'ancienne ou barre d'icônes)
	WriteString ( SETTINGS_SECTION, "OldFunctionKeys", 
			( TerminalParameters. OldFunctionKeys ) ?  "True" : "False" ) ;

# endif


// Paramètres généraux : ouverture automatique des sessions
	WriteString ( SETTINGS_SECTION, "AutoOpen", ( TerminalParameters. AutoOpen ) ?
							"True" : "False" ) ;


// Paramètres généraux : confirmer sur sortie si config modifiée
	WriteString ( SETTINGS_SECTION, "ConfirmOnExit", ( TerminalParameters. ConfirmOnExit ) ?
							"True" : "False" ) ;

// Paramètres généraux : Type de beep
	WriteString ( SETTINGS_SECTION, "Beep", ( TerminalParameters. AlarmBeep ) ?  
							"True" : "False" ) ;


// Paramètres généraux : Form feed après un hardcopy
	WriteString ( SETTINGS_SECTION, "FFOnHardcopy",  
			( TerminalParameters. FormFeedOnHardcopy ) ?  "True" : "False" ) ;

// Paramètres généraux : Form feed après un hardcopy
	WriteString ( SETTINGS_SECTION, "FrameHardcopy",  
			( TerminalParameters. FrameHardcopy ) ?  "True" : "False" ) ;


// Paramètres généraux : Taille mini d'une saisie pour qu'elle soit ajoutée à
// la redo list
	wsprintf ( buffer, "%d", TerminalParameters. MinRedoSize ) ;
	WriteString ( SETTINGS_SECTION, "MinRedoSize", buffer ) ;


// Paramètres généraux : délai d'ouverture des menus Cheops
	wsprintf ( buffer, "%d", TerminalParameters. MenuAutoOpenDelay ) ;
	WriteString ( SETTINGS_SECTION, "AutoOpenDelay", buffer ) ;


// Paramètres généraux : dernière position
	wsprintf ( buffer, "%d", TerminalParameters. LastX ) ;
	WriteString ( SETTINGS_SECTION, "LastPositionX", buffer ) ;
	wsprintf ( buffer, "%d", TerminalParameters. LastY ) ;
	WriteString ( SETTINGS_SECTION, "LastPositionY", buffer ) ;

// Récupération des valeurs par défaut
	WriteString ( DEFAULTS_SECTION, "LinkType", ( DefaultNetworkLink ) ?
							"Network" : "Serial" ) ;

	WriteSessionParameters ( DEFAULTS_SECTION, DefaultSessionParameters, TRUE ) ;
	WriteConnectionParameters ( DEFAULTS_SECTION, DefaultNetworkParameters, TRUE, TRUE ) ;
	WriteConnectionParameters ( DEFAULTS_SECTION, DefaultSerialParameters, FALSE, TRUE ) ;


// Lecture des paramètres relatifs à chaque session	
	for  ( i = j = 0 ; i  <  MAX_SESSIONS ; i ++ )
	   {
		if  ( SessionStates [i]  !=  Free  &&
		      ! Sessions [i]. LocalSession )
		   {
			j ++ ;

			WriteConnectionParameters ( TerminalParameters. SessionSections [i],
					Connections [i], Connections [i]. NetworkLink, FALSE ) ;
			WriteSessionParameters ( TerminalParameters. SessionSections [i], 
					Sessions [i], FALSE ) ;

			wsprintf ( buffer, "session%d", j ) ;
			WriteString ( ConfigurationSectionName ( TerminalParameters. Configuration ),
				buffer, TerminalParameters. SessionSections [i] ) ;
		    }
	    }

	SetDirty ( FALSE ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TConfiguration :: GetSessionCount, GetOpenedSessionCount              */
/*      Retournent le nombre de sessions créées/ouvertes.                 */
/*                                                                        */
/**************************************************************************/

int	TConfiguration :: GetSessionCount  ( void )
   {
	register int 		i, j = 0 ;


	for  ( i = 0 ; i < MAX_SESSIONS ; i ++ )
	   {
		if  ( SessionStates [i]  !=  Free )
			j ++ ;
	    }

	return ( j ) ;
     }


int	TConfiguration :: GetOpenedSessionCount  ( void )
   {
	register int 		i, j = 0 ;


	for  ( i = 0 ; i < MAX_SESSIONS ; i ++ )
	   {
		if  ( SessionStates [i]  ==  Opened )
			j ++ ;
	    }

	return ( j ) ;
     }




/**************************************************************************/
/*                                                                        */
/*  TConfiguration :: FindFreeSlot                                        */
/*      Retourne l'index d'un slot libre pour accueillir une nouvelle     */
/*      configuration.                                                    */
/*                                                                        */
/**************************************************************************/

int	TConfiguration :: FindFreeSlot ( void )
   {
	for  ( int  i = 0 ; i < MAX_SESSIONS ; i ++ )
	   {
		if  ( SessionStates [i]  ==  Free )
			return ( i ) ;
	    }

	return ( -1 ) ;
     }




/**************************************************************************/
/*                                                                        */
/*  TConfiguration :: AddSession, RemoveSession                           */
/*      Gestion de l'ajout/suppression d'une session à une config.        */
/*                                                                        */
/**************************************************************************/

int	TConfiguration :: AddSession  ( char *				name,
					TConnectionParameters &		cp,
					TSessionParameters *		sp )
   {
	register int		slot  =  FindFreeSlot ( ) ;


// Chercher un emplacement libre
	if  ( slot  ==  -1 )
		return ( -1 ) ;


// Prendre les paramètres de session par défaut s'ils n'ont pas été spécifiés
	if  ( sp  ==  NULL )
		sp = & DefaultSessionParameters ;


// Ajout
	Connections [slot] 	=   cp ;
	Sessions    [slot]      =  * sp ;
	SessionStates [slot]	=  Created ;
	strcpy ( TerminalParameters. SessionSections [slot], name ) ;
	

// Ouverture auto ?
	if  ( ! PreventAutoOpen ) 
	   {
		CreateConnection ( slot ) ;
		SetCurrentSession ( slot ) ;

		if  ( TerminalParameters. AutoOpen )
			Open ( slot ) ;
	     }

	return  ( slot ) ;
    }





int	TConfiguration :: RemoveSession  ( int		index )
   {
	if  ( index >= 0  &&  index  <=  MAX_SESSIONS  &&  SessionStates [index]  !=  Free )
	   {
		if  ( IsOpened ( index ) )
			Close ( index ) ;

		SessionStates [index] = Free ;
		TerminalParameters. SessionSections [index] [0] = 0 ;
		return ( 0 ) ;
	     }
	else
		return ( -1 ) ;
     }




int	TConfiguration :: RemoveSession  ( char *   name )
   {
	register int		i, j = -1 ;


	for  ( i = 0 ; i < MAX_SESSIONS ; i ++ )
	   {
		if  ( SessionStates [i]  !=  Free   &&
			! stricmp ( TerminalParameters. SessionSections [i], name ) )
		   {
			j = i ;
			break ;
		    }
	     }

	if  ( j  ==  -1 )
		return ( -1 ) ;
	else
		return ( RemoveSession ( i ) ) ;
     }





/**************************************************************************/
/*                                                                        */
/*  TConfiguration :: SwapSessions                                        */
/*      Echange l'emplacement de deux sessions.                           */
/*                                                                        */
/**************************************************************************/

void	TConfiguration :: SwapSessions  ( int  index1, int index2 )
   {
	char			DummyName [ MAX_PARAMETER_LENGTH ] ;
	TSessionParameters	DummySession ;
	TConnectionParameters	DummyConnection ;
	TSessionState 		DummyState ;



	strcpy ( DummyName, TerminalParameters. SessionSections [index1] ) ;
	strcpy ( TerminalParameters. SessionSections [index1],
			TerminalParameters. SessionSections [index2] ) ;
	strcpy ( TerminalParameters. SessionSections [index2], DummyName ) ;
		
	DummySession = Sessions [index1] ;
	Sessions [index1] = Sessions [index2] ;
	Sessions [index2] = DummySession ;

	DummyConnection = Connections [index1] ;
	Connections [index1] = Connections [index2] ;
	Connections [index2] = DummyConnection ;

	DummyState = SessionStates [index1] ;
	SessionStates [index1] = SessionStates [index2] ;
	SessionStates [index2] = DummyState ;
    }





/**************************************************************************/
/*                                                                        */
/*  TConfiguration :: Open, Close                                         */
/*      Ouvre/ferme une session.                                          */
/*                                                                        */
/**************************************************************************/

int	TConfiguration :: Open  ( int	session )
   {

	if  ( session  ==  -1 )	
		session = CurrentSession ;

	if  ( session < 0  ||  session  >=  MAX_SESSIONS )
		return ( -1 ) ;

	if  ( SessionStates [session]  !=  Created )
		return ( -1 ) ;

	if  ( ! Connections [session]. Connection -> Open ( ) )
		return ( -1 ) ;

	SessionStates [session] = Opened ;
	CurrentSession = session ;
	TerminalWindow -> UpdateConnectionText ( Opened ) ;
	return ( 0 ) ;
     }


int	TConfiguration :: Close  ( int  session )
   {

	if  ( session  ==  -1 )	
		session = CurrentSession ;

	if  ( session < 0  ||  session  >=  MAX_SESSIONS )
		return ( -1 ) ;

	if  ( SessionStates [session]  !=  Opened )
		return ( -1 ) ;

	Connections [session]. Connection -> Close ( ) ;
	SessionStates [session] = Created ;

	if  ( session  ==  GetCurrentSession ( ) )
		TerminalWindow -> UpdateConnectionText ( Created ) ;
	return ( 0 ) ;
     }




void	TConfiguration :: CloseAll  ( void )
   {
	for  ( int  i = 0 ; i < MAX_SESSIONS ; i ++ )
	   {
		if  ( SessionStates [i]  ==  Opened )
			Close ( i ) ;
	     }
     }




void	TConfiguration :: CreateConnection  ( int	  session )
   {
	if  ( SessionStates [session]  ==  Created )
	   {
		TRect			  Rect  = TerminalWindow -> GetClientRect ( ) ;
		TTerminalWindowClient *   wc    = new  TTerminalWindowClient ( 
								TerminalWindow, session ) ;


		:: SessionParameters    		= Sessions + session ;
		:: ConnectionParameters 		= Connections + session ;

		Connections [session]. TerminalWindowClient = wc ;
		wc -> Create ( ) ;

		if  ( Connections [session]. NetworkLink )
			Connections [session]. Connection = new  TNetworkConnection ( 
					wc, RECEIVE_BUFFER_SIZE_NETWORK, session ) ;
		else
			Connections [session]. Connection = new  TSerialConnection ( 
					wc, RECEIVE_BUFFER_SIZE_SERIAL, session ) ;

		Connections [session]. Device           = new  TDisplayDevice ( wc,
			Connections [session]. Connection, session ) ;
		wc -> Assign ( Connections [session]. Device,
				Connections [session]. Connection ) ;
	     }
     }





/**************************************************************************/
/*                                                                        */
/*  TConfiguration :: ConfigurationSectionName, SessionSectionName :      */
/*      Retourne le nom d'une configuration/section.                      */
/*                                                                        */
/**************************************************************************/

char *		TConfiguration :: ConfigurationSectionName  ( char *  name )
   {
	static char 		Buffer [ MAX_PARAMETER_LENGTH ] ;


	wsprintf ( Buffer, "%s%s", CONFIGURATION_SECTION, name ) ;
	return ( Buffer ) ;
    }



char *		TConfiguration :: SessionSectionName  ( char *  name )
   {
	static char 		Buffer [ MAX_PARAMETER_LENGTH ] ;


	wsprintf ( Buffer, "%s%s", SESSION_SECTION, name ) ;
	return ( Buffer ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TConfiguration :: SetCurrentSession                                   */
/*      Définit la session spécifiée comme étant la session active.       */
/*                                                                        */
/**************************************************************************/

void	TConfiguration :: SetCurrentSession  ( int  session, BOOL  update )
   {
	if  ( session  >=  0  &&   session  <  MAX_SESSIONS  &&
			SessionStates [session]  !=  Free   &&
			session  !=  CurrentSession ) 
	   {
		if  ( Device  &&  Device -> InMessageReceive )
			return ;

		if  ( PreventAutoOpen )
			return ;

		:: SessionParameters    = Sessions + session ;
		:: ConnectionParameters = Connections + session ;
		:: Connection		= Connections [session]. Connection ;
		:: Device		= Connections [session]. Device ;

		if  ( update )
		   {
			TTerminalWindowClient *	 Old = Connections [CurrentSession]. TerminalWindowClient,
					      *	 New = Connections [session]. TerminalWindowClient ;


			if  ( Old )
				Old -> Show ( SW_HIDE ) ;

			TerminalWindow -> SetClientWindow ( New ) ;
			
		// Le UpdateWindow est nécessaire, sinon un message 
		// WM_ERASEBACKGROUND survient juste après le UpdateScreen,
		// ce qui provoque un effacement total.
			New -> UpdateWindow ( ) ;
			Device -> ResetFont ( FALSE ) ;
			Device -> UpdateCursorPosition ( ) ;
			CurrentSession = session ;
			TerminalWindow -> UpdateConnectionText ( SessionStates [session] ) ;			
			TerminalWindow -> SetText ( ) ;
		    }
		else
			CurrentSession 		= session ;
	    }
     }





/**************************************************************************/
/*                                                                        */
/*   TConfiguration :: SetNextSession, SetPreviousSession                 */
/*      Rendent active la session suivante/précédente.                    */
/*                                                                        */
/**************************************************************************/

void	TConfiguration :: SetNextSession  ( void )
   {
	register int		i  =  CurrentSession ;


	do
	   {
		i = ( i + 1 ) % MAX_SESSIONS ;

		if  ( SessionStates [i]  !=  Free )
		   {
			SetCurrentSession ( i ) ;
			return ;
		    }
	    }  while  ( i  !=  CurrentSession ) ;
     }




void	TConfiguration :: SetPreviousSession  ( void )
   {
	register int		i  =  CurrentSession ;


	do
	   {
		i -- ;

		if  ( i < 0 )
			i = MAX_SESSIONS - 1 ;

		
		if  ( SessionStates [i]  !=  Free )
		   {
			SetCurrentSession ( i ) ;
			return ;
		    }
	    }  while  ( i  !=  CurrentSession ) ;
     }


                







/**************************************************************************/
/*                                                                        */
/*   TConfiguration :: CreateDefaultINIFile                               */
/*      Crée un fichier de configuration contenant une configuration par  */
/*      défaut, avec une session série.                                   */
/*                                                                        */
/**************************************************************************/

BOOL	TConfiguration :: CreateDefaultINIFile ( void )
   {
	register FILE *		fp  =  fopen ( TerminalParameters. IniFileName, "wt" ) ;
	HRSRC			ResourceHandle ;
	HGLOBAL			MemoryHandle ;
	register char far *	p ;


// Ouverture du fichier de configuration
	if  ( fp  ==  NULL )
	   {
		:: MessageBox ( NULL, "Erreur", MB_OK | MB_TASKMODAL,
			"Le fichier de configuration '%s' ne peut être créé",
				TerminalParameters. IniFileName ) ;
		return ( FALSE ) ;		// T'es mal, mon gars
	     }


// Récupération de la ressource
	ResourceHandle  = TerminalApplication ->  FindResource ( 
				IDR_DEFAULT_CONFIGURATION, IDT_PARAMETERS ) ;
	MemoryHandle	= TerminalApplication -> LoadResource ( ResourceHandle ) ;
	p = ( char far * ) GlobalLock ( MemoryHandle ) ;


// Ecritation des paramètrures
	for  ( ; p  !=  NULL  &&  * p ; p += strlen ( p ) + 1 )
		fprintf ( fp, "%s\n", p ) ;			
		
	
// Libérassification de la mémoire
	GlobalUnlock ( MemoryHandle ) ;
	FreeResource ( MemoryHandle ) ;
	

	fclose ( fp ) ;
	return ( TRUE ) ;
     }





/**************************************************************************/
/*                                                                        */
/*   TConfiguration :: CreateDefaultEmulation                             */
/*      Crée un fichier d'émulation par défaut contenant la définition    */
/*      du terminal ANSI.                                                 */
/*                                                                        */
/**************************************************************************/

BOOL	TConfiguration :: CreateDefaultEmulation ( void )
   {
	register FILE *		fp ;
	HRSRC			ResourceHandle ;
	HGLOBAL			MemoryHandle ;
	register char far *	p ;


// Ouverture du fichier de configuration
	strcpy ( DefaultSessionParameters. Emulation, "Ansi.Trm" ) ;
	ConvertEmulationPath ( DefaultSessionParameters. Emulation, TRUE ) ;

	if  ( ( fp = fopen ( DefaultSessionParameters. Emulation, "wt" ) )  ==  NULL )
	   {
		:: MessageBox ( NULL, "Erreur", MB_OK | MB_TASKMODAL,
			"Le fichier de définition pour l'émulation '%s' ne peut être créé",
				DefaultSessionParameters. Emulation ) ;
		return ( FALSE ) ;		// T'es mal, mon gars
	     }


// Récupération de la ressource
	ResourceHandle  = TerminalApplication ->  FindResource ( 
				IDR_DEFAULT_EMULATION, IDT_PARAMETERS ) ;
	MemoryHandle	= TerminalApplication -> LoadResource ( ResourceHandle ) ;
	p = ( char far * ) GlobalLock ( MemoryHandle ) ;


// Ecritation des paramètrures
	for  ( ; p  !=  NULL  &&  * p ; p += strlen ( p ) + 1 )
		fprintf ( fp, "%s\n", p ) ;			
		
	
// Libérassification de la mémoire
	GlobalUnlock ( MemoryHandle ) ;
	FreeResource ( MemoryHandle ) ;
	

	fclose ( fp ) ;
	return ( TRUE ) ;
     }




