/**************************************************************************/
/*                                                                        */
/*  HideWindow.cpp  -  GELUGNE Jérôme, 26/05/95.                          */
/*                                                                        */
/*  Implémentation de la classe permettant l'envoie et la réception       */
/*	 de caractères grâce à la création d'une fenêtre invisible  	   	  */
/*  recevant et envoyant des caractères.                                  */
/*																								  */
/**************************************************************************/


# include 	"AllTerm.h"
# pragma	hdrstop

# include 	"HideWind.h"



DEFINE_RESPONSE_TABLE1 ( THideWindow, TWindow )
	EV_MESSAGE ( WM_RECEIVE_NETWORK, CmReceiveNetwork ) ,
	EV_MESSAGE ( WM_SEND_NETWORK, CmSend ) ,
END_RESPONSE_TABLE ;


/**************************************************************************/
/*                                                                        */
/*  Classe THideWindow :: THideWindow :                                   */
/*      Constructeur de la classe.													  */
/*                                                                        */
/**************************************************************************/

THideWindow :: THideWindow ( TWindow * parent, TNetworkConnection * PNet )
		: TWindow ( parent, NULL )
	{
	NetworkConnection = PNet ;
	Premiere = TRUE ;
	} ;



/**************************************************************************/
/*                                                                        */
/*  Classe THideWindow :: CmReceiveNetwork :                              */
/*      Reçoit le message WM_RECEIVED_NETWORK et le traite en conséquence */
/* 	  i.d. si on a reçu un message de fermeture de la liaison réseau    */
/*      ou si on a reçu un message de réception de caractères.            */
/*                                                              			  */
/**************************************************************************/

LRESULT THideWindow :: CmReceiveNetwork ( WPARAM, LPARAM lEvent )
   {
	register int		id ;


	switch	( WSAGETSELECTEVENT ( lEvent ) )
	   {
		case	FD_READ :
			Receive ( ) ;
			break ;

		case	FD_CLOSE :
			id = NetworkConnection -> SessionId ;
			Configuration -> Close ( id ) ;
			Configuration -> Connection ( id ).
				Device -> SetContinuousPrint ( FALSE ) ;

			if  ( id  ==  Configuration -> GetCurrentSession ( ) )
				TerminalWindow -> SetStatusBarText ( 1, 0 ) ;
	     }

	return ( 0 ) ;
     }


/**************************************************************************/
/*                                                                        */
/*  Classe THideWindow :: Receive : 		                                */
/*      Permet le traitement de la réception de caractères.					  */
/*                                                              			  */
/**************************************************************************/

void	THideWindow :: Receive ( )
	{
	static char	buffer  	[ SEND_BUFFER_SIZE_NETWORK / 4 ] ;
	char *		inbuf_temp	= new char [ RECEIVE_BUFFER_SIZE_NETWORK ] ;
	char *		inbuf       = new char [ RECEIVE_BUFFER_SIZE_NETWORK ] ;
	int		taille ;
	int		taille_inbuf = 0 ;
	register int	i ;


// Réception des caractères :
	if ( ( taille = recv ( NetworkConnection -> chaussette, inbuf_temp,
			RECEIVE_BUFFER_SIZE_NETWORK, 0 ) ) != SOCKET_ERROR )
		{
		for ( i = 0; i < taille; i ++ )
			{
// Bidouille pour éviter les caractères reçu au moment du protocole de liaison
// avec TELNET.
			if ( ( unsigned char ) inbuf_temp [ i ] ==  ( unsigned char ) IAC
					&& Premiere )
			   {
				switch ( ( unsigned char ) inbuf_temp [ i + 1 ] )
					{
					// If sub negotiation
					case SB :
						i += 5 ;
						break ;
					default :
						if ( inbuf_temp [ i + 2 ] == TELOPT_LFLOW )
							Premiere = FALSE ;
						i += 2 ;
					}
			    }
			else     
				{
				inbuf [ taille_inbuf ] = inbuf_temp [ i ] ;
				taille_inbuf ++ ;
				}
			}


		for ( i = 0; i < taille_inbuf; i ++ )
			{
			if ( NetworkConnection -> CharactersReceived + 1  >=
				  NetworkConnection -> ReceiveBufferSize )
				{
				NetworkConnection -> DestinationWindow -> HandleMessage ( WM_DATARECEIVED,
						NetworkConnection -> CharactersReceived, 0L ) ;

				if ( NetworkConnection -> CharactersReceived + 1  >=
					  NetworkConnection -> ReceiveBufferSize )
					:: MessageBox ( NULL, "Arrghhh !!!", MB_OK |
						MB_ICONEXCLAMATION, "Trop de caractères reçus!" ) ;
				}

			NetworkConnection -> ReceiveBuffer [ ( NetworkConnection -> Origin +
				NetworkConnection -> CharactersReceived ) %
					NetworkConnection -> ReceiveBufferSize ]
				= inbuf [i] ;
			NetworkConnection -> CharactersReceived =
				( NetworkConnection -> CharactersReceived + 1 ) %
					NetworkConnection -> ReceiveBufferSize ;
			}
	  }
	else
	   {
	// Si un appel est en cours, Winsock retourne ce code d'erreur. Il
	// semblerait qu'ensuite, les données soient considérées comme lues,
	// ce qui fait qu'on ne reçoit plus de message RECEIVE et donc qu'on
	// loupe des choses...
	// Dans ce cas, on se réémet soi-meme le message. Attention : cela
	// n'empeche pas d'utiliser des temporisations dans
        // TNetworkConnection :: InitTelnet ( ).
		if  ( WSAGetLastError ( )  ==  WSAEINPROGRESS )
		   {
			DelayMin ( 50 ) ;
			:: PostMessage ( HWindow, WM_RECEIVE_NETWORK,
				NetworkConnection -> chaussette, FD_READ ) ;
		     }
	     }


// Prévenir la fenêtre que des caractères ont été reçus
	if  ( NetworkConnection -> CharactersReceived )
		NetworkConnection -> DestinationWindow -> HandleMessage ( WM_DATARECEIVED,
				NetworkConnection -> CharactersReceived, 0L ) ;


// Si des caractères sont à envoyer, on en profite
	taille   = NetworkConnection -> HugeTable -> Copy ( buffer,
		SEND_BUFFER_SIZE_NETWORK / 4 ) ;

	if  ( taille )
		NetworkConnection -> DestinationWindow -> SendMessage ( WM_SENDDATA,
			taille, ( LPARAM ) ( ( char far * ) buffer ) ) ;

	delete  [] inbuf_temp  ;
	delete  [] inbuf  ;
	}


LRESULT	THideWindow :: CmSend ( WPARAM buffer_size, LPARAM buffer )
	{
// Envoie des données d'un seul tenant avec temporisation si les données
// envoyées font partie du protocole de communication.
	if ( Premiere )
		DelayMin ( ConnectionParameters -> Network. Delay ) ;

	send ( NetworkConnection -> chaussette, ( char far * ) buffer,
		( int16 ) buffer_size, 0 ) ;

	return ( 0 ) ;
	}

