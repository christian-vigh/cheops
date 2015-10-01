/**************************************************************************/
/*                                                                        */
/*  Window.ch  -  Christian Vigh, 94/10/18.                               */
/*                                                                        */
/*  Gestion d'une fenêtre en provenance d'Unix.                           */
/*                                                                        */
/**************************************************************************/



/**************************************************************************/
/*                                                                        */
/*  Classe  TUnixWindow :                                                 */
/*      Définition de la classe Window en provenance d'Unix.              */
/*                                                                        */
/**************************************************************************/




DEFINE_RESPONSE_TABLE1  ( TUnixWindow, TWindow )
	EV_WM_PAINT,
	EV_MESSAGE ( WM_SETICON	, EvSetIcon ),
END_RESPONSE_TABLE ;
