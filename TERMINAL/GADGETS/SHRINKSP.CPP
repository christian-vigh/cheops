/**************************************************************************/
/*                                                                        */
/*  ShrinkSP.cpp  -  Christian Vigh, 95/10.                               */
/*                                                                        */
/*  Impl�mentation de la classe TShrinkSeparatorGadget.                   */
/*                                                                        */
/**************************************************************************/


# pragma	option -k- -N- -r- -v- -vi- -y-
# include	<owl/owlall.h>
# pragma	hdrstop
# pragma	option -k. -N. -r. -v. -vi. -y.


# include	"ChopJet.h"



/**************************************************************************/
/*                                                                        */
/*  Constructeur.                                                         */
/*                                                                        */
/**************************************************************************/


TShrinkSeparatorGadget :: TShrinkSeparatorGadget  ( int 	 minw, int  maxw )
	: TSeparatorGadget ( )
   {
	MinWidth = minw ;
	MaxWidth = maxw ;
	WideAsPossible = TRUE ;
    }





/**************************************************************************/
/*                                                                        */
/*  TShrinkSeparatorGadget :: SetLimits -                                 */
/*      Red�finit les limites de taille inf�rieure et sup�rieure du       */
/*      gadget.                                                           */
/*                                                                        */
/**************************************************************************/

void	TShrinkSeparatorGadget :: SetLimits  ( int  minw, int  maxw )
   {
	if  ( minw  !=  MinWidth  &&  maxw  !=  MaxWidth )
	   {
		MinWidth = minw ;
		MaxWidth = maxw ;

		if  ( Window )
			Window -> GadgetChangedSize ( * this ) ;
	    }
    }





/**************************************************************************/
/*                                                                        */
/*  TShrinkSeparatorGadget :: SetBounds -                                 */
/*      V�rifie que la nouvelle dimension du gadget correspond bien �     */
/*      l'intervalle d�fini. Dans le cas contraire, modifie ces           */
/*      dimensions en cons�quence.                                        */
/*                                                                        */
/**************************************************************************/

void	TShrinkSeparatorGadget :: SetBounds ( TRect &  Rect )
   {
	if  ( MinWidth  !=  -1  &&  Rect. Width ( )  <  MinWidth )
		Rect. right = Rect. left + MinWidth ;

	if  ( MaxWidth  !=  -1  &&  Rect. Width ( )  >  MaxWidth )
		Rect. right = Rect. left + MaxWidth ;

	TSeparatorGadget :: SetBounds ( Rect ) ;
    }
