/*============================[[ beg-of-code ]]===============================*/

#include      "init.h"
#include      <sys/reboot.h>

/*> RB_POWER_OFF                                                                   <* 
 *> RB_AUTOBOOT                                                                    <* 
 *> RB_DISABLE_CAD                                                                 <* 
 *> RB_HALT_SYSTEM                                                                 <*/

char      prog_args         (int argc, char *argv[]);

char      my_reboot   = 'n';
char      my_halt     = 'n';


int
main (int argc, char *argv[])
{
   printf("nyx -- system shutdown\n");
   /*---(locals)-------------------------*/
   FILE     *f         = NULL;
   int       rc        = 0;
   int       pid       = 0;
   /*---(clean up the drives)------------*/
   printf("   - sync drives three times\n");
   sync();
   sync();
   sync();
   sleep(1);
   /*---(check args)---------------------*/
   prog_args (argc, argv);
   /*---(lock up this program)-----------*/
	signal(SIGTERM, SIG_IGN);
	signal(SIGSTOP, SIG_IGN);
	signal(SIGKILL, SIG_IGN);
   /*---(shutdown processes)-------------*/
   /*> printf("   - send TERM message\n");                                            <* 
    *> kill(-1, SIGTERM);                                                             <* 
    *> sleep(1);                                                                      <*/
   /*> printf("   - send KILL message\n");                                            <* 
    *> kill(-1, SIGKILL);                                                             <* 
    *> sleep(1);                                                                      <*/
   /*---(kill slim)----------------------*/
   f = fopen ("/var/run/slim.pid", "r");
   if (f != NULL) {
      rc = fscanf (f, "%d", &pid);
      if (rc == 1) {
         kill (pid, SIGTERM);
      }
      fclose (f);
   }
   /*---(call the shutdown)--------------*/
   if  (my_halt   == 'y') {
      printf("   - halt\n");
      rc = reboot(RB_POWER_OFF);
   }
   if  (my_reboot == 'y') {
      printf("   - reboot\n");
      rc = reboot(RB_AUTOBOOT);
   }
   /*---(these calls shouldn't come back)*/
   return rc;
}

char       /* PURPOSE : process the command line arguments                    */
prog_args          (int argc, char *argv[])
{
   char     *a         = NULL;         /* current argument                    */
   int       i         = 0;            /* loop iterator -- arguments          */
   for (i = 1; i < argc; ++i) {
      a = argv[i];
      if      (strncmp(a, "-r",     5) == 0)         my_reboot = 'y';
      if      (strncmp(a, "-h",     5) == 0)         my_halt   = 'y';
   }
   return 0;
}



/*============================[[ end-of-code ]]===============================*/
