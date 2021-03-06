/**************************************************************************/
/*                                                                        */
/*  HideWindow.cpp  -  GELUGNE J�r�me, 26/05/95.                          */
/*                                                                        */
/*  Impl�mentation de la classe permettant l'envoie et la r�ception       */
/*	 de caract�res gr�ce � la cr�ation d'une fen�tre invisible  	   	  */
/*  recevant et envoyant des caract�res.                                  */
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
/*      Re�oit le message WM_RECEIVED_NETWORK et le traite en cons�quence */
/* 	  i.d. si on a re�u un message de fermeture de la liaison r�seau    */
/*      ou si on a re�u un message de r�ception de caract�res.            */
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
/*      Permet le traitement de la r�ception de caract�res.					  */
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


// R�ception des caract�res :
	if ( ( taille = recv ( NetworkConnection -> chaussette, inbuf_temp,
			RECEIVE_BUFFER_SIZE_NETWORK, 0 ) ) != SOCKET_ERROR )
		{
		for ( i = 0; i < taille; i ++ )
			{
// Bidouille pour �viter les caract�res re�u au moment du protocole de liaison
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
						MB_ICONEXCLAMATION, "Trop de caract�res re�us!" ) ;
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
	// semblerait qu'ensuite, les donn�es soient consid�r�es comme lues,
	// ce qui fait qu'on ne re�oit plus de message RECEIVE et donc qu'on
	// loupe des choses...
	// Dans ce cas, on se r��met soi-meme le message. Attention : cela
	// n'empeche pas d'utiliser des temporisations dans
        // TNetworkConnection :: InitTelnet ( ).
		if  ( WSAGetLastError ( )  ==  WSAEINPROGRESS )
		   {
			DelayMin ( 50 ) ;
			:: PostMessage ( HWindow, WM_RECEIVE_NETWORK,
				NetworkConnection -> chaussette, FD_READ ) ;
		     }
	     }


// Pr�venir la fen�tre que des caract�res ont �t� re�us
	if  ( NetworkConnection -> CharactersReceived )
		NetworkConnection -> DestinationWindow -> HandleMessage ( WM_DATARECEIVED,
				NetworkConnection -> CharactersReceived, 0L ) ;


// Si des caract�res sont � envoyer, on en profite
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
// Envoie des donn�es d'un seul tenant avec temporisation si les donn�es
// envoy�es font partie du protocole de communication.
	if ( Premiere )
		DelayMin ( ConnectionParameters -> Network. Delay ) ;

	send ( NetworkConnection -> chaussette, ( char far * ) buffer,
		( int16 ) buffer_size, 0 ) ;

	return ( 0 ) ;
	}

