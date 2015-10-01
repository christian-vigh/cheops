/**************************************************************************/
/*                                                                        */
/*   AllTerm.h  -  Christian Vigh, 95/08/04.                              */
/*                                                                        */
/*   Fichier regroupant tous les includes.                                */
/*                                                                        */
/**************************************************************************/


# if 	! defined ( __ALTERM_H__ )
# 	  define    __ALLTERM_H__



// Includes OWL
# pragma	option  -k- -N- -vi- -y- -v-
# include	<owl/owlall.h>
# pragma	option  -k. -N. -vi. -y. -v.


// Includes de la librairie C
# include	<stdio.h>
# include	<stdlib.h>
# include	<ctype.h>
# include	<string.h>
# include	<time.h>
# include	<stddef.h>
# include	<memory.h>
# include	<sys/types.h>
# include	<fcntl.h>
# include	<sys/stat.h>
# include	<io.h>
# include	<dir.h>
# include	<errno.h>
# include	<bwcc.h>
# include	<stdarg.h>
# include	<ver.h>
# include	<cstring.h>


# include	"WinSock.h"
# include	"UWCommon.h"


// Includes de l'application
# include	"Util.h"
# include	"Terminal.h"
# include	"Help.h"
# include	"TermWin.h"
# include	"Message.h"
# include	"Device.h"
# include	"HideWind.h"
# include	"Connect.h"
# include	"Dialogs.h"



# endif		/*  __ALLTERM_H__  */
