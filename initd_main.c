/*===========================[[ start-of-code ]]==============================*/
#include      "init.h"



int
main (int argc, char *argv[])
{
   DEBUG_FLOW  printf("initd by the_heatherlys (%s)\n", VER_NUM);
   initialize(0);
   daemonize();
   /*---(process initd.conf)--------------------*/
   FILE     *f    = NULL;                         /* initd.conf descriptor    */
   f = fopen(CONF, "r");
   if (f == NULL) {
      printf("initd : can not get initd.conf file, FATAL\n");
      yLOG_info  ("FAILED",  "can not open the initd.conf file");
      return -3;
   } else {
      DEBUG_FLOW  printf("configuration file is open\n");
   }
   inventory(f);
   yLOG_value ("nprocs#"   , nprocs);
   DEBUG_FLOW  printf("found %02d procs\n", nprocs);
   int       i    = 0;                       /* loop iterator                 */
   for (i = 0; i < nprocs; ++i) {
      DEBUG_FLOW  printf("#%02d :: %-10s :: %s\n", i, procs[i].name, procs[i].desc);
      run(i);
   }
   /*---*/
   while (1) {
      pause();
   }
   yLOG_end   ();
   /*> execl(SHELL, SHELL, "-c", "/sbin/agetty 38400 tty1 linux", NULL, NULL);        <*/
   return 0;
}
