#include "argus.h"



char
main               (int a_argc, char *a_argv [])
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_PROG  yLOG_enter   (__FUNCTION__);
   /*---(run)----------------------------*/
   rc = PROG_pseudo (a_argc, a_argv);
   DEBUG_PROG  yLOG_value   ("pseudo"    , rc);
   /*---(complete)-----------------------*/
   DEBUG_PROG  yLOG_exit    (__FUNCTION__);
   return rc;
}
