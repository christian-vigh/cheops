/**************************************************************************/
/*                                                                        */
/*   Connect.h  -  Christian Vigh, 94/09/13.                              */
/*                 (d'après Sébastien George et Christophe Després).      */
/*                                                                        */
/*   Définition des classes servant à gérer une connexion.                */
/*                                                                        */
/**************************************************************************/


# define		TIMER_INTERVAL			10	 	/* ms */
# define		BUFFER_SIZE_SOCKET 	8192 	/* Taille du buffer de la socket */


/**************************************************************************/
/*                                                                        */
/*   Classe TConnection :                                                 */
/*      Classe de base virtuelle pour les connexions série ou réseau.     */
/*      Elle définit un comportement de base pour les connexions :        */
/*      Les caractères reçus sont stockés dans ReceiveBuffer (de taille   */
/*      ReceiveBufferSize). Le nombre de caractères est CharactersReceived*/
/*      Configuration pointe sur les paramètres de Terminal, et           */
/*      DestinationWindow sur la fenêtre à qui un message WM_DATARECEIVED */
/*      sera envoyé sur réception d'un paquet de caractères.              */
/*                                                                        */
/**************************************************************************/

class	TConnection
	{
	public :
		static BOOL		OpenCount ;
		char *			ReceiveBuffer ;
		int			ReceiveBufferSize,
					CharactersReceived,
					Origin ;
		DWORD			LastCharTick ;
		TWindow * 		DestinationWindow ;
		DWORD			TimeOutValue,
					TimeOutTick ;
		BOOL			TimeOutFlag ;
		BOOL			OpenedFlag ;
		BOOL			InMessage ;
		int			SessionId ;



	public :
		TConnection	( TWindow *		dest,
				  int 		 	BufferSize,
				  int			session )
			 {
			ReceiveBuffer	   =  new char [ BufferSize + 1 ] ;
			ReceiveBufferSize  =  BufferSize ;
			ReceiveBuffer [ ReceiveBufferSize ] = 0 ;
			CharactersReceived =  0 ;
			Origin		   =  0 ;
			DestinationWindow  =  dest ;
			LastCharTick	   =  0L ;
			TimeOutValue	   =  0L ;
			TimeOutTick	   =  0L ;
			TimeOutFlag        =  0L ;
			InMessage	   =  FALSE ;
			SessionId	   =  session ;
				}

		virtual ~TConnection ( )
			{
			delete [] ReceiveBuffer ;
			 }


		virtual BOOL	Open  		( void )
		   {
			OpenedFlag = TRUE ;
			OpenCount ++ ;
			return ( TRUE ) ;
		    }

		virtual void	Close           ( void ) 
		   {
			OpenedFlag = FALSE ;
			OpenCount -- ;
		    }


		virtual int	DataReceived	( void ) 	=  0 ;
		virtual int	GetData		( char *	buffer,
						  int		buffer_size )
								=  0 ;
		virtual int	ReadData	( char *	buffer,
						  int		buffer_size )
								=  0 ;
		virtual int	SendData	( char * 	buffer,
						  int		buffer_size )
								=  0 ;
		virtual BOOL	Receive		( void )	=  0 ;
		virtual BOOL	SendHugeData	( char far * 	buffer,
						  long int	buffer_size )
								=  0 ;

		virtual void	Synchronize	( void )	=  0 ;
		virtual void	FlushInput	( void )	=  0 ;
		virtual void	FlushOutput	( void )	=  0 ;
		virtual void	SendBreak	( void ) 	=  0 ;


		TWindow *  SetDestinationWindow  ( TWindow *  window )
		   {
			TWindow *	OldWindow = DestinationWindow ;

			DestinationWindow = window ;
			return ( OldWindow ) ;
			 }


		TWindow *  GetDestinationWindow  ( void )
		   {
			return ( DestinationWindow ) ;
		    }


		void		SetLastTick  ( )
		   {
			LastCharTick = GetTickCount ( ) ;
			 }


		void		SetTimeOutValue  ( DWORD  Val )
		   {
			TimeOutValue  =  Val ;
			 }


		BOOL 		GotTimeOut  ( void )
		   {
			return ( TimeOutFlag ) ;
		    }

		void		ResetTimeOutFlag  ( void )
		   {
			TimeOutFlag = FALSE ;
		    }

		BOOL		IsOpened  ( void )
		   {
			return ( OpenedFlag ) ;
		    }
	 } ;




/**************************************************************************/
/*                                                                        */
/*   La classe THugeEntry conserve un paquet de données à envoyer.        */
/*   La classe THugeTable gère une file de THugeEntry.                    */
/*                                                                        */
/**************************************************************************/

class	THugeEntry
	{
	private :
		HANDLE		Handle ;
		char far *	LockedPointer ;
		long int	DataSize,
				NextByte ;

	public :
		class THugeEntry *	Next ;


	public :
		THugeEntry   ( char far *	str,
					 long int		data_size ) ;

	       ~THugeEntry   ( ) ;

		char far *	Locked		( void ) ;

		void		Unlock		( void ) ;
		int		Copy		( char *	buffer,
						  int		count ) ;

		long int	UnsentData	( void )
			{
			return ( ( Handle  ==  NULL ) ?
					0 : DataSize - NextByte ) ;
		    }

     } ;





class	THugeTable
	{
	private :
		THugeEntry *	Head,
				*	Tail ;


	public :
		THugeTable  	( )
		   {
			Head = Tail = NULL ;
		    }

	       ~THugeTable	( )
		  {
			Reset ( ) ;
		   }

		void		Reset		( void ) ;
		void		Add		( char far *	str,
						  long int	size ) ;
		int		Copy		( char *	buffer,
						  int		size ) ;
    } ;





/**************************************************************************/
/*                                                                        */
/*   Classe TSerialConnection :                                           */
/*      Dérivée de TConnection, elle permet de gérer une connexion via    */
/*      une liaison série.                                                */
/*                                                                        */
/*      En mode Emulateur, la fonction membre TimerProc est appelée par   */
/*      Windows toutes les 100ms pour vérifier si des caractères ont été  */
/*      reçus.                                                            */
/*                                                                        */
/**************************************************************************/

class	TSerialConnection 	:  public	TConnection
   {
	private :

	// ComDCB :
	//	Paramètres utilisés pour l'ouverture du port série.
	//                      
	// TimerId :
	//	Identification du timer alloué pour TimerProc.
	//
	// DeviceId :
	//	Identificateur du port série utilisé.
	//
		DCB			ComDCB ;
		int			DeviceId ;
		THugeTable *		HugeTable ;


	public :
		TSerialConnection ( TWindow *    	dest,
				    int			BufferSize,
				    int			session ) ;

		virtual ~TSerialConnection ( )
			{
			Close ( ) ;
			delete  HugeTable ;
			}


		int	GetDeviceCount  ( void )
			{
			return ( ( int )
			EscapeCommFunction ( DeviceId, GETMAXCOM ) + 1 ) ;
			}


		virtual BOOL	Open  		( void ) ;
		virtual void	Close           ( void ) ;
		virtual int	DataReceived	( void ) ;
		virtual int	GetData		( char *	buffer,
						  int		buffer_size ) ;
		virtual int	ReadData	( char * 	buffer,
						  int		buffer_size ) ;
		virtual int	SendData	( char * 	buffer,
						  int		buffer_size ) ;
		virtual BOOL	SendHugeData	( char far * 	buffer,
						  long int	buffer_size ) ;
		virtual BOOL	Receive		( void ) ;
		   

		virtual void	Synchronize     ( void ) ;


		virtual void	SendBreak  ( void )
		   {
			SetCommBreak ( DeviceId ) ;
			ClearCommBreak ( DeviceId ) ;
		    }

		virtual void	FlushInput  ( void )
		   {
			//FlushComm ( DeviceId, 1 ) ;
			CharactersReceived = 0 ;
			}


		virtual void 	FlushOutput  ( void )
			{
			//FlushComm ( DeviceId, 0 ) ;
			 }
	 } ;



/**************************************************************************/
/*                                                                        */
/*   Classe TNetworkConnection :                                          */
/*      Dérivée de TConnection, elle permet de gérer une connexion via    */
/*      une liaison réseau TCP/IP.                                        */
/*                                                                        */
/**************************************************************************/

//  Codes de contrôle pour le protocole TELNET
# define	TELNET_IAC		255	// Caractère d'échappement pour es codes suivants
# define	TELNET_SE		240	// Fin des options de sous-négotiation
# define	TELNET_NOP		241	// No-operation
# define	TELNET_DM		242	// Partie "données" d'une synchro
# define	TELNET_BRK		243	// Break
# define	TELNET_IP		244	// Interruption de process
# define	TELNET_AO		245	// Abort output
# define	TELNET_AYT		246	// "Are you there ?"
# define	TELNET_EC		247	// Erase character
# define	TELNET_EL		248	// Erase line
# define	TELNET_GA		249	// Signal Go-ahead
# define	TELNET_SB		250	// Indique le départ d'une sous-négotiation
# define	TELNET_WILL		251	// Indique le souhait ou confirme
						// la mise en route d'une option
# define	TELNET_WONT		252	// Indique le refus d'exécuter 
						// ou de continuer à exécuter une
						// option
# define	TELNET_DO		253	// Demande ou confirme l'exécution
						// d'une option
# define	TELNET_DONT		254	// Demande ou confirme l'arrêt
						// d'une option




class	THideWindow ;

class	TNetworkConnection 	:  public	TConnection
	{
	public :

		struct sockaddr_in	sin ;
		struct hostent FAR * hp ;
		struct servent FAR * sp ;
		SOCKET					chaussette ;
		THugeTable *			HugeTable ;
		THideWindow *			HideWindow ;

		TNetworkConnection ( TWindow *  dest,
				     int 	BufferSize,
				     int	session ) ;

		virtual ~TNetworkConnection ( )
			{
			Close ( ) ;
			delete  HugeTable ;
			delete 	HideWindow ;
			}

		void	InitTelnet ( void ) ;

		void 	Deconnection ( void ) ;

		int	GetDeviceCount  ( void )
			{
			return ( 1 ) ;
			}


		virtual BOOL	Open  		( void ) ;
		virtual void	Close           ( void ) ;
		virtual int	DataReceived	( void ) ;
		virtual int	GetData		( char *	buffer,
						  int		buffer_size ) ;
		virtual int	ReadData	( char * 	buffer,
						  int		buffer_size ) ;
		virtual int	SendData	( char * 	buffer,
						  int		buffer_size ) ;
		virtual BOOL	SendHugeData	( char far * 	buffer,
						  long int	buffer_size ) ;
					 virtual BOOL	Receive		( void ) ;


		virtual void	Synchronize     ( void ) ;

		virtual void	SendBreak  ( void )
		  { }

		virtual void	FlushInput  ( void )
			{
			//FlushComm ( DeviceId, 1 ) ;
			CharactersReceived = 0 ;
			 }


		virtual void 	FlushOutput  ( void )
			{
			//FlushComm ( DeviceId, 0 ) ;
			 }
	 } ;

