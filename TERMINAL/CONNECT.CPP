/**************************************************************************/
/*                                                                        */
/*   Connect.cpp  - Christian Vigh, 94/09/13.                             */
/*                  (d'apr�s S�bastien George et Christophe Despr�s).     */
/*                                                                        */
/*   Impl�mentation de la classe TSerialConnection.                       */
/*                                                                        */
/*   Modifications -                                                      */
/*                                                                        */
/*      24/06/97 (CV,MBT)                                                 */
/*              Suppression des options de n�gociation WILL/DO pour       */
/*              l'option No24 (terminal type) dans InitTelnet, ainsi      */
/*              que de l'option TELNET_SB pour indiquer le type de        */
/*              terminal. Ces trois options ont �t� remplac�es par un     */
/*              WONT.                                                     */
/*              Les lignes en double dans la structure d'initialisation   */
/*              ont �t� supprim�es.                                       */
/*              Cette correction r�sout un probl�me de connexion sur un   */
/*              syst�me AIX version 4.1, mais ne r�sout pas celui de      */
/*              SCO V5.                                                   */
/*                                                                        */
/**************************************************************************/


# include	"AllTerm.h"
# pragma	hdrstop



TConnection *		Connection ;


BOOL			TConnection :: OpenCount 	=  0 ;




/**************************************************************************/
/*                                                                        */
/*  TSerialConnection :: TSerialConnection :                              */
/*      Initialiseur de la classe.                                        */
/*                                                                        */
/**************************************************************************/

TSerialConnection :: TSerialConnection  ( TWindow *   		dest,
					  int		      buffer_size,
					  int			session )
		: TConnection ( dest, buffer_size, session )
	{
	HugeTable 	=  new  THugeTable ;
	DeviceId	=  -1 ;
	}






/**************************************************************************/
/*                                                                        */
/*   TSerialConnection :: Open                                            */
/*      Etablit la connexion.                                             */
/*                                                                        */
/**************************************************************************/

BOOL	TSerialConnection :: Open ( void )
	{
	TConnection :: Open ( ) ;

	DeviceId = OpenComm ( ConnectionParameters -> Serial. PortName,
			RECEIVE_BUFFER_SIZE_SERIAL, SEND_BUFFER_SIZE_SERIAL ) ;

	if  ( DeviceId  <  0 )
		return ( FALSE ) ;


	memset ( & ComDCB, 0, sizeof ( ComDCB ) ) ;
	
	ComDCB. Id		=  DeviceId ;
	ComDCB. BaudRate	=  ( WORD ) ConnectionParameters -> Serial. Speed ;
	ComDCB. ByteSize	=  ConnectionParameters -> Serial. DataBits ;
	ComDCB. Parity		=  ConnectionParameters -> Serial. Parity ;
	ComDCB. StopBits	=  ConnectionParameters -> Serial. StopBits ;


// Remarque TRES IMPORTANTE sur XoffLim et XonLim :
// Il est absolument n�cessaire d'assigner � ces champs une valeur bien
// inf�rieure aux tailles des files de transmission/reception.
// Pourquoi ? Lorsque la file d'attente du PC est pleine, il envoie un
// caract�re XOFF. Mais durant cet envoi, Unix continue de balancer ses
// caract�res vers le PC... caract�res qu'il ne peut que perdre, puisqu'il
// a d�j� signal� qu'il n'en voulait plus (au moyen du XOFF).
// En taillant ces valeurs au plus bas, on est assur� de ne pas avoir de
// perte.
// Autre remarque TRES importante : si fBinary n'est pas � 1, le caract�re
// EOFChar est reconnu. Par d�faut, on le meyt � z�ro.
// Dans ces conditions, la r�ception du caract�re 0 est donc reconnue comme
// End of Data. La fonction GetCommError ne retourne aucun statut d'erreur,
// et ReadComm satisfait toutes les demandes de lectures (quand bien m�me
// il n'y a pas de donn�es en attente). La seule mani�re possible de s'en
// sortir semble �tre de fermer et de rouvrir le port... ou de mettre 
// fBinary � 1.
	ComDCB. fOutX		=  1 ;
	ComDCB. fInX		=  1 ; 	// Pour le XON-XOFF
	ComDCB. fRtsDisable	=  1 ;
	ComDCB. fDtrflow	=  1 ;
	ComDCB. fBinary		=  1 ;
	ComDCB. XonLim		=  0 ;
	ComDCB. XoffLim		=  RECEIVE_BUFFER_SIZE_SERIAL / 20 ;
	ComDCB. XonChar		=  XON ;
	ComDCB. XoffChar	=  XOFF ;

	if  ( SetCommState ( & ComDCB )  <  0 )
		return ( FALSE ) ;

	CharactersReceived = 0 ;
	Origin 		   = 0 ;

	return ( TRUE ) ;
	 }




/**************************************************************************/
/*                                                                        */
/*  TSerialConnection :: Close  :                                         */
/*      Ferme la connexion et arr�te le timer.                            */
/*                                                                        */
/**************************************************************************/

void  TSerialConnection :: Close  ( void )
   {
	TConnection :: Close ( ) ;

	CloseComm  ( DeviceId ) ;
	CharactersReceived = 0 ;
	Origin		   = 0 ;
	DeviceId	   = -1 ;
    }





/**************************************************************************/
/*                                                                        */
/*   TSerialConnection :: DataReceived  :                                 */
/*      Retourne le nombre de caract�res re�us.                           */
/*                                                                        */
/**************************************************************************/

int	TSerialConnection :: DataReceived  ( void )
   {
	return ( CharactersReceived ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TSerialConnection :: GetData :                                        */
/*      Copie dans un buffer dont l'adresse est indiqu�e par l'appelant   */
/*      les caract�res qui ont �t� re�us.                                 */
/*      Retourne le nombre de caract�res copi�s.                          */
/*                                                                        */
/**************************************************************************/

int	TSerialConnection :: GetData  ( char *	buffer,
					int	buffer_size )
   {
	if  ( ! CharactersReceived )
		return ( 0 ) ;

	buffer_size = min ( buffer_size, CharactersReceived ) ;

	for  ( int  i = 0 ; i < buffer_size ; i ++ )
		buffer [i] = ReceiveBuffer [ ( Origin + i ) % ReceiveBufferSize ] ;
	CharactersReceived -= buffer_size ;
	Origin = ( Origin + buffer_size ) % ReceiveBufferSize ;

	return  ( buffer_size ) ;
	 }




/**************************************************************************/
/*                                                                        */
/*   TSerialConnection :: SendData (                                         */
/*      Envoie les donn�es sp�cifi�es sur le port s�rie.                  */
/*                                                                        */
/**************************************************************************/

int	TSerialConnection :: SendData ( char *  buffer,
					 int	 buffer_size )
   {
	COMSTAT		ComStat ;
	register int	size	=  buffer_size ;


	if  ( DeviceId  <  0 )
		return ( 0 ) ;

	while  ( size )
		{
		SetCommEventMask ( DeviceId, EV_TXEMPTY ) ;

		while  ( GetCommEventMask ( DeviceId, EV_TXEMPTY ) )
			{
			SetCommEventMask ( DeviceId, EV_TXEMPTY ) ;
		    }

		GetCommError  ( DeviceId, & ComStat ) ;

		if  ( ComStat. cbOutQue )
			continue ;

		WriteComm ( DeviceId, buffer, 1 ) ;
		buffer ++, size -- ;
		  }

	return ( buffer_size ) ;
	 }





/**************************************************************************/
/*                                                                        */
/*   TSerialConnection :: SendHugeData                                    */
/*      Permet d'envoyer, en asynchrone, des donn�es de taille importante */
/*                                                                        */
/**************************************************************************/

BOOL	TSerialConnection :: SendHugeData  ( char far *		buffer,
					     long int		size )
	{
	if  ( DeviceId < 0 )
		return ( FALSE ) ;
	HugeTable -> Add ( buffer, size ) ;
	return ( TRUE ) ;
	}



/**************************************************************************/
/*                                                                        */
/*   TSerialConnection :: Receive                                         */
/*      R�ceptionne les caract�res en provenance de la liaison s�rie.     */
/*                                                                        */
/**************************************************************************/

BOOL  TSerialConnection :: Receive  ( void )
	 {
	char		buffer  [ SEND_BUFFER_SIZE_SERIAL / 4 ] ;
	char		inbuf   [ RECEIVE_BUFFER_SIZE_SERIAL ] ;
	int		size, count ;
	int		CommError ;
	DWORD		TickCount ;
	COMSTAT		ComStat ;
	register int	i ;


	if  ( DeviceId  <  0 )
		return ( FALSE ) ;

	TickCount = GetTickCount ( ) ;

// R�ception des caract�res :
	CommError = GetCommError ( DeviceId, & ComStat ) ;

	if  ( CommError  &  ( CE_BREAK | CE_RXOVER ) )
		ClearCommBreak ( DeviceId ) ;
	size = ReadComm ( DeviceId, inbuf, RECEIVE_BUFFER_SIZE_SERIAL ) ;

	if  ( size  <  0 )
	   {
		size = - size ;
		GetCommError ( DeviceId, & ComStat ) ;
		ClearCommBreak ( DeviceId ) ;
	    }
	    

	for  ( i = 0 ; i < size ; i ++ )
	   {
		if ( CharactersReceived + 1  >=  ReceiveBufferSize )
			{
			DestinationWindow -> HandleMessage ( WM_DATARECEIVED,
					CharactersReceived, 0L ) ;

			if ( CharactersReceived + 1  >=  ReceiveBufferSize )
				MessageBox ( NULL, "Arrghhh !!!", MB_OK | MB_ICONEXCLAMATION,
					"Trop de caract�res re�us!" ) ;
		     }

		ReceiveBuffer [ ( Origin + CharactersReceived ) % ReceiveBufferSize ] =
			inbuf [i] ;
		CharactersReceived = ( CharactersReceived + 1 ) % ReceiveBufferSize ;
	    }


// On v�rifie s'il y a eu un timeout
	if  ( TimeOutValue  &&  ! size )
		{
		if  ( TickCount - TimeOutTick  >  TimeOutValue )
			{
			ReceiveBuffer [ ( Origin + CharactersReceived ) % ReceiveBufferSize ] =
				TIMEOUT_CHAR ;
			CharactersReceived = ( CharactersReceived + 1 ) % ReceiveBufferSize ;
			size = 1 ;
			TimeOutFlag = TRUE ;
			 }
		  }

	if  ( size )
		TimeOutTick = TickCount ;


// Pr�venir la fen�tre que des caract�res ont �t� re�us
	if  ( CharactersReceived )
		DestinationWindow -> HandleMessage ( WM_DATARECEIVED,
				CharactersReceived, 0L ) ;


// Si des caract�res sont � envoyer, on en profite
	count  = ( int ) ( ( ConnectionParameters -> Serial. RealSpeed / 12 )
					/ ( 1000 / TIMER_INTERVAL ) ) ;
	count  = min ( count, SEND_BUFFER_SIZE_SERIAL / 4 ) ;
	size   = HugeTable -> Copy ( buffer, count ) ;

	if  ( size )
		DestinationWindow -> SendMessage ( WM_SENDDATA, size,
			( LPARAM ) ( ( char far * ) buffer ) ) ;

	return ( size ) ;
    }







/**************************************************************************/
/*                                                                        */
/*  TSerialConnection :: ReadData                                         */
/*      Lit une suite de caract�res.                                      */
/*                                                                        */
/**************************************************************************/

int	TSerialConnection :: ReadData  ( char *  buffer, int  buffer_size )
	{
	register int		size ;

// Lire les �ventuels caract�res restants
	size = GetData ( buffer, buffer_size ) ;
	return ( size ) ;
	  }





/**************************************************************************/
/*                                                                        */
/*  TSerialConnection :: Synchronize                                      */
/*      Durant des p�riodes de trafic intense, certains caract�res des    */
/*      messages peuvent �tre perdus. Lorsque cette situation est d�tect�e*/
/*      cette fonction est appel�e afin d'�mettre sur la ligne SYNC_COUNT */
/*      caract�res �gaux � SYNC_CHAR, � des intervalles de SYNC_INTERVAL  */
/*      millisecondes.                                                    */
/*                                                                        */
/**************************************************************************/

void	TSerialConnection :: Synchronize  ( void )
	{
	char		b [1] ;
	DWORD		CurrentTicks, LastTicks ;
	register int	i ;


	* b = SYNC_CHAR ;

	ResetTimeOutFlag ( ) ;
	FlushOutput ( ) ;
	GetCommError ( DeviceId, NULL ) ;

	for  ( i = 0 ; i < SYNC_SIZE ; i ++ )
		{
		CurrentTicks = LastTicks = GetTickCount ( ) ;

		while  ( CurrentTicks - LastTicks  <  SYNC_INTERVAL )
			CurrentTicks = GetTickCount ( ) ;
		SendData ( b, 1 ) ;
		 }

	* b = '*' ;
	SendData ( b, 1 ) ;

	CurrentTicks = LastTicks = GetTickCount ( ) ;

	while  ( CurrentTicks - LastTicks  <  50 )
		CurrentTicks = GetTickCount ( ) ;
	GetCommError ( DeviceId, NULL ) ;
	}





/**************************************************************************/
/*                                                                        */
/*  TNetworkConnection :: TNetworkConnection :                             */
/*      Initialiseur de la classe.                                        */
/*                                                                        */
/**************************************************************************/

TNetworkConnection :: TNetworkConnection  ( TWindow *   dest,
					    int		buffer_size,
					    int		session )
		: TConnection ( dest, buffer_size, session )
	{
	HideWindow 	= 	new 	THideWindow ( DestinationWindow, this ) ;
	HugeTable 	=  new  	THugeTable ;
	}


/**************************************************************************/
/*                                                                        */
/*  TNetworkConnection :: Init_Telnet                                     */
/*      Initialisation de Telnet.	                                  */
/*                                                                        */
/**************************************************************************/

# define	INIT_MAX		16


void	TNetworkConnection :: InitTelnet ( )
   {
// Le premier caract�re indique le nombre d'octets � envoyer
	static char	telnet_init [ ]	[ INIT_MAX ] =
			   {
				{ 3, TELNET_IAC, TELNET_WILL	, 1 	},
				{ 3, TELNET_IAC, TELNET_DO	, 1 	},
				{ 3, TELNET_IAC, TELNET_WILL	, 3 	},
				{ 3, TELNET_IAC, TELNET_DO  	, 3 	},
				{ 3, TELNET_IAC, TELNET_WILL	, 5 	},
				{ 3, TELNET_IAC, TELNET_DO	, 5 	},
				{ 3, TELNET_IAC, TELNET_WILL	, 255 	},
				{ 3, TELNET_IAC, TELNET_DO	, 255 	},
				{ 3, TELNET_IAC, TELNET_WONT	, 24 	},
  
				{ 3, TELNET_IAC, TELNET_WONT	, 32 	},

				{ 3, TELNET_IAC, TELNET_WONT 	, 35 	},

				{ 3, TELNET_IAC, TELNET_WONT 	, 36 	},

				{ 3, TELNET_IAC, TELNET_WONT 	, 5 	},

				{ 3, TELNET_IAC, TELNET_WONT 	, 255 	},
				{ 3, TELNET_IAC, TELNET_DONT 	, 255 	},
				{ 3, TELNET_IAC, TELNET_WONT 	, 1 	},
				{ 3, TELNET_IAC, TELNET_WONT 	, 31 	},
				{ 3, TELNET_IAC, TELNET_WONT	, 33 	},
				{ 3, TELNET_IAC, TELNET_DO	, 1 	},
			    } ;


	for  ( int  i = 0 ; i < sizeof ( telnet_init ) / sizeof ( telnet_init [0] ) ; i ++ )
	   {
		SendData ( & telnet_init [i] [1], telnet_init [i] [0] ) ;
		DelayMin ( ConnectionParameters -> Network. Delay ) ;
	    }
     }




/**************************************************************************/
/*                                                                        */
/*   TNetworkConnection :: Open                                           */
/*      Etablit la connexion.                                             */
/*                                                                        */
/**************************************************************************/

BOOL	TNetworkConnection :: Open ( void )
	{
	char 		Server_Name [ MAX_PARAMETER_LENGTH ] ;
	WORD		wVersionRequested ;
	WSADATA		wsa_Data ;
	int 		erreur ;

	TConnection :: Open ( ) ;

	CharactersReceived = 0 ;
	Origin 		   = 0 ;

	HideWindow -> Create ( ) ;


	strcpy ( Server_Name, ConnectionParameters -> Network. ServerName ) ;


// On test si winsock.dll est pr�sente et si son num�ro de version est
//	sup�rieur ou �gale � la version 1.1 .
//	WSAStartup doit �tre obligatoirement la premi�re fonction des
//	Windows Sockets � �tre appel� par une application ou une DLL .
	wVersionRequested = ( ( 1 << 8 ) | 1 ) ;
	erreur = WSAStartup ( wVersionRequested, &wsa_Data ) ;

	if ( erreur )
	   {
		switch ( erreur )
		   {
			case	WSASYSNOTREADY :
				MessageBox ( 0, "Erreur de r�seau", MB_OK | MB_ICONSTOP ,
					"Le r�seau n'est pas disponible.\nVeuillez contacter le service assistance de Chartier Informatique." ) ;
				break ;
 
			case	WSAVERNOTSUPPORTED :
				MessageBox ( 0, "Erreur de r�seau", MB_OK | MB_ICONSTOP ,
					"Votre version de WINSOCK.DLL est incorrecte.\nVeuillez contacter le service assistance de Chartier Informatique." ) ;
				break ;

			case	WSAEINVAL :
				MessageBox ( 0, "Erreur de r�seau", MB_OK | MB_ICONSTOP ,
					"WINSOCK.DLL ne supporte pas la version demand�e par Cheops.\nVeuillez contacter le service assistance de Chartier Informatique." ) ;
				break ;

			default :
				MessageBox ( 0, "Erreur de r�seau", MB_OK | MB_ICONSTOP ,
					"Le code d'erreur %d a �t� retourn� lors du premier acc�s au r�seau.\nVeuillez contacter le service assistance de Chartier Informatique.",
						erreur ) ;
				break ;
		     }
		
		return ( FALSE ) ;
	    }


	if ( LOBYTE ( wsa_Data. wVersion ) < 1 ||
			 ( LOBYTE ( wsa_Data. wVersion ) == 1 &&
				HIBYTE ( wsa_Data. wVersion ) < 1 ) )
		{
		MessageBox ( 0, "Erreur", MB_OK | MB_ICONSTOP,
			"WINSOCK.DLL ne supporte pas la version 1.1 du r�seau. Cheops ne peut fonctionner.\n"
			"Veuillez contacter le service assistance de Chartier Informatique." ) ;
		WSACleanup ( ) ;
		return ( FALSE ) ;
		}

// Recherche de l'adresse Internet du serveur .
	if ( ( hp = gethostbyname ( Server_Name ) ) == NULL )
	   {
		MessageBox ( 0, "Erreur de r�seau", MB_OK | MB_ICONSTOP, 
			"Le serveur %s n'a pas �t� trouv�.", Server_Name ) ;
		return ( FALSE ) ;
	     }

	_fmemcpy ( ( LPSTR ) &sin. sin_addr, hp -> h_addr, hp -> h_length ) ;
	sin.sin_family = hp->h_addrtype;

// Recherche du num�ro du port du service TELNET avec le protocole TCP .
	if ( ( sp = getservbyname ( "telnet", "tcp" ) ) == NULL )
	   {
		MessageBox ( 0, "Erreur de r�seau", MB_OK | MB_ICONSTOP,
			"Le port telnet/tcp n'a pas �t� trouv�." ) ;
		return ( FALSE ) ;
	    }

	sin.sin_port = sp->s_port;

//	Cr�ation du descripteur de la socket .
	if ( ( chaussette = socket ( AF_INET, SOCK_STREAM, 0 ) )
			== INVALID_SOCKET )
	   {
		MessageBox ( 0, "Erreur de r�seau", MB_OK | MB_ICONSTOP,
			"Cr�ation impossible d'un socket STREAM pour le r�seau INET" ) ;
		return ( FALSE );
	     }

// Etablit une connection avec le serveur .
	if ( connect ( chaussette, ( LPSOCKADDR ) &sin, sizeof ( sin ) )
			== SOCKET_ERROR )
	   {
		MessageBox ( 0, "Erreur de r�seau", MB_OK | MB_ICONSTOP,
			"La connexion au socket a �chou�" ) ;
		return ( FALSE ) ;
	    }

// Sp�cifie une taille de BUFFER_SIZE_SOCKET pour les buffers de r�ception
//	et d'envoie propre � la socket .
	static int		size ;


	size = BUFFER_SIZE_SOCKET ;
	setsockopt ( chaussette, SOL_SOCKET, SO_RCVBUF, ( char * ) & size, sizeof ( size ) ) ;
	setsockopt ( chaussette, SOL_SOCKET, SO_SNDBUF, ( char * ) & size, sizeof ( size ) ) ;


// Initialisation de TELNET .
	InitTelnet ( ) ;


// Intercepte tous les �v�nements relatif au r�seaux et envoie un message
// WM_RECEIVE_NETWORK � la fen�tre traitant ces �v�nements .
	WSAAsyncSelect ( chaussette, HideWindow -> HWindow,
		WM_RECEIVE_NETWORK, FD_READ | FD_CLOSE ) ;

	return ( TRUE ) ;
	}




/**************************************************************************/
/*                                                                        */
/*  TNetworkConnection :: Close  :                                        */
/*      Ferme la connexion et arr�te le timer.                            */
/*                                                                        */
/**************************************************************************/

void  TNetworkConnection :: Close  ( void )
	{
	TConnection :: Close ( ) ;


// Permet de dire � TELNET de ne plus attendre d'envoie ou de r�ception.
// TELNET se ferme donc. Cela sert quand on se d�connecte sans taper la
// commande exit sous TELNET .
	shutdown ( chaussette, 2 ) ;

	closesocket ( chaussette ) ;
	chaussette = INVALID_SOCKET ;

// Termine l'utilisation des Windows Sockets .
	Deconnection ( ) ;
	WSACleanup ( ) ;

	CharactersReceived = 0 ;
	Origin		   = 0 ;
	}





/**************************************************************************/
/*                                                                        */
/*   TNetworkConnection :: DataReceived  :                                */
/*      Retourne le nombre de caract�res re�us.                           */
/*                                                                        */
/**************************************************************************/

int	TNetworkConnection :: DataReceived  ( void )
	{
	return ( CharactersReceived ) ;
	}




/**************************************************************************/
/*                                                                        */
/*  TNetworkConnection :: GetData :                                       */
/*      Copie dans un buffer dont l'adresse est indiqu�e par l'appelant   */
/*      les caract�res qui ont �t� re�us.                                 */
/*      Retourne le nombre de caract�res copi�s.                          */
/*                                                                        */
/**************************************************************************/

int	TNetworkConnection :: GetData  ( char *	buffer,
					int	buffer_size )
	{
	if  ( ! CharactersReceived )
		return ( 0 ) ;

	buffer_size = min ( buffer_size, CharactersReceived ) ;

	for  ( int  i = 0 ; i < buffer_size ; i ++ )
		buffer [i] = ReceiveBuffer [ ( Origin + i ) % ReceiveBufferSize ] ;
	CharactersReceived -= buffer_size ;
	Origin = ( Origin + buffer_size ) % ReceiveBufferSize ;

	return  ( buffer_size ) ;
	}




/**************************************************************************/
/*                                                                        */
/*   TNetworkConnection :: SendData :                                     */
/*      Envoie les donn�es sp�cifi�es sur le r�seau.                      */
/*                                                                        */
/**************************************************************************/

int	TNetworkConnection :: SendData ( char *  buffer,
					 int	 buffer_size )
	{
// On envoie le message, la taille et les donn�es � HideWindow .
	SendMessage ( HideWindow -> HWindow, WM_SEND_NETWORK,
		( WPARAM ) ( ( int16 ) buffer_size ),
		( LPARAM ) ( ( char far * ) buffer ) ) ;

	return ( buffer_size ) ;
	}





/**************************************************************************/
/*                                                                        */
/*   TNetworkConnection :: SendHugeData                                   */
/*      Permet d'envoyer, en asynchrone, des donn�es de taille importante */
/*                                                                        */
/**************************************************************************/

BOOL	TNetworkConnection :: SendHugeData  ( char far *		buffer,
						  long int		size )
	{
	HugeTable -> Add ( buffer, size ) ;
	return ( TRUE ) ;
	}



/**************************************************************************/
/*                                                                        */
/*   TNetworkConnection :: Receive                                        */
/*      R�ceptionne les caract�res en provenance du r�seau.				     */
/*                                                                        */
/**************************************************************************/

BOOL  TNetworkConnection :: Receive  ( void )
	{
	return ( 0 ) ;
	}





/**************************************************************************/
/*                                                                        */
/*  TNetworkConnection :: ReadData                                        */
/*      Lit une suite de caract�res.                                      */
/*                                                                        */
/**************************************************************************/

int	TNetworkConnection :: ReadData  ( char *  buffer, int  buffer_size )
	{
	register int		size ;

// Lire les �ventuels caract�res restants
	size = GetData ( buffer, buffer_size ) ;
	return ( size ) ;
	}





/**************************************************************************/
/*                                                                        */
/*  TNetworkConnection :: Synchronize                                     */
/*      Durant des p�riodes de trafic intense, certains caract�res des    */
/*      messages peuvent �tre perdus. Lorsque cette situation est d�tect�e*/
/*      cette fonction est appel�e afin d'�mettre sur la ligne SYNC_COUNT */
/*      caract�res �gaux � SYNC_CHAR, � des intervalles de SYNC_INTERVAL  */
/*      millisecondes.                                                    */
/*                                                                        */
/**************************************************************************/

void	TNetworkConnection :: Synchronize  ( void )
	{
	char		b [1] ;
	DWORD		CurrentTicks, LastTicks ;
	register int	i ;


	* b = SYNC_CHAR ;

	ResetTimeOutFlag ( ) ;
	FlushOutput ( ) ;

	for  ( i = 0 ; i < SYNC_SIZE ; i ++ )
		{
		CurrentTicks = LastTicks = GetTickCount ( ) ;

		while  ( CurrentTicks - LastTicks  <  SYNC_INTERVAL )
			CurrentTicks = GetTickCount ( ) ;
		SendData ( b, 1 ) ;
		}

	* b = '*' ;
	SendData ( b, 1 ) ;

	CurrentTicks = LastTicks = GetTickCount ( ) ;

	while  ( CurrentTicks - LastTicks  <  50 )
		CurrentTicks = GetTickCount ( ) ;

	}



/**************************************************************************/
/*                                                                        */
/*  TNetworkConnection :: Deconnection                                    */
/*  		Renvoie un message quand on se deconnecte.                       */
/*																								  */
/**************************************************************************/

void 	TNetworkConnection :: Deconnection ( void )
   {
	HideWindow -> Premiere = TRUE ;

	if  ( DestinationWindow -> HWindow  &&
		IsWindow ( DestinationWindow -> HWindow ) )
	   {
		static char   	deconnect [ ] = "\n\rConnection closed by foreign host.\n\r" ;
		register int	length	      = strlen ( deconnect ) ;

		Origin		   = 0 ;

		strncpy ( ReceiveBuffer, deconnect, ReceiveBufferSize ) ;
		CharactersReceived = min ( length, ReceiveBufferSize ) ;

		DestinationWindow -> HandleMessage ( WM_DATARECEIVED,
			CharactersReceived, 0L ) ;
	     }
     }

