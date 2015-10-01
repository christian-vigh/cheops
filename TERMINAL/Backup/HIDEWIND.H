/**************************************************************************/
/*                                                                        */
/*  HideWind.h  -  GELUGNE Jérôme, 26/06/95.                              */
/*                                                                        */
/*  Classe pour la gestion de l'envoie et de la réception de caractères   */
/*	 en réseau.                   													  */
/*                                                                        */
/**************************************************************************/

# ifndef __HIDEWIND_H__
# define __HIDEWIND_H__


/***************************************************************************/
/*																									*/
/* 	Définition des codes du protocole TELNET( très succinct )            */
/*																									*/
/***************************************************************************/

# define		IAC				0xFF
# define		SB					0xFA
# define		TELOPT_LFLOW	0x21

/**************************************************************************/
/*                                                                        */
/*  WM_RECEIVED_NETWORK :                                                 */
/*      Message envoyé à la fenêtre hidewindow lorsque des caractères ont */
/*      été reçus.                                                        */
/*                                                                        */
/*  WM_SEND_NETWORK :                                                     */
/* 	  Message envoyé par TConnection à la fenêtre hidewindow lorsque    */
/*  	  des caractères sont à envoyer.                                    */
/*                                                                        */
/**************************************************************************/

# define 	WM_RECEIVE_NETWORK	WM_USER + 200
# define		WM_SEND_NETWORK		WM_USER + 201

class	TNetworkConnection ;

class THideWindow	: public TWindow
	{
	private :
		TNetworkConnection *	NetworkConnection ;

	public :

		THideWindow ( TWindow * parent, TNetworkConnection * ) ;

		LRESULT	CmReceiveNetwork ( WPARAM , LPARAM lEvent ) ;

		LRESULT	CmSend ( WPARAM buffer_size, LPARAM buffer ) ;

		void	Receive ( void ) ;

		DECLARE_RESPONSE_TABLE ( THideWindow ) ;

		BOOL						Premiere ;
	} ;

# endif
