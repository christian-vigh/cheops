/**************************************************************************/
/*                                                                        */
/*  ChopTree.h  -  Christian Vigh, 95/11.                                 */
/*                                                                        */
/*  Définition de deux classes :                                          */
/*                                                                        */
/*  TCheopsTree, pour conserver la hiérarchie de menus Cheops.            */
/*  TCheopsMenuObjects, pour la hiérarchie de menus associés à des        */
/*  fenêtres.                                                             */
/*                                                                        */
/**************************************************************************/




# if    ! defined ( __CHOPTREE_H__ )
#         define    __CHOPTREE_H__



/**************************************************************************/
/*                                                                        */
/*  Classe TCheopsTree -                                                  */
/*      Arborescence de menus Cheops.                                     */
/*                                                                        */
/**************************************************************************/

class	TCheopsMenuTree 
   {
	public :
		TCheopsMenuTree  ( FILE *	fp ) ;
	       ~TCheopsMenuTree  ( ) ;	


		TCheopsMenu *		GetMenu   ( char *  		name ) ;
		static void		FreeMenu  ( TCheopsMenu *	menu ) ;
		static TCheopsMenu *	CopyMenu  ( TCheopsMenu *	menu ) ;
		BOOL			ExistMenu ( char *		name ) ;
		

	private :
		struct  TMenuPosition 
		   {
				long int	Offset ;
				char *		Name ;
		    } ;


		TMenuPosition *		Menus ;
		int			MenuCount ;
		FILE *			MenuFileFP ;
		char *			MenuFileBuffer ;
     } ;



# endif		/*  __CHOPTREE_H__  */

