/**************************************************************************/
/*                                                                        */
/*  HideWind.h  -  GELUGNE J�r�me, 26/06/95.                              */
/*                                                                        */
/*  Classe pour la gestion de l'envoie et de la r�ception de caract�res   */
/*	 en r�seau.                   													  */
/*                                                                        */
/**************************************************************************/

# ifndef __HIDEWIND_H__
# define __HIDEWIND_H__


/***************************************************************************/
/*																									*/
/* 	D�finition des codes du protocole TELNET( tr�s succinct )            */
/*																									*/
/***************************************************************************/

# define		IAC				0xFF
# define		SB					0xFA
# define		TELOPT_LFLOW	0x21

/**************************************************************************/
/*                                                                        */
/*  WM_RECEIVED_NETWORK :                                                 */
/*      Message envoy� � la fen�tre hidewindow lorsque des caract�res ont */
/*      �t� re�us.                                                        */
/*                                                                        */
/*  WM_SEND_NETWORK :                                                     */
/* 	  Message envoy� par TConnection � la fen�tre hidewindow lorsque    */
/*  	  des caract�res sont � envoyer.                                    */
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
