/*============================[[ beg-of-code ]]===============================*/

/*===[[ START HDOC ]]=========================================================*/
/*===[[ HEADER ]]=============================================================#

 *   focus         : (SA) system_admin
 *   niche         : (in) init
 *   application   : kharon (hades ferryman)
 *   purpose       : consistent, reliable handling of terminated processes
 *
 *   base_system   : gnu/linux
 *   lang_name     : gnu/ansi-c (because it's right, just, best, and universal)
 *   dependencies  : yLOG only
 *   size          : tiny (less than 1,000 slocL)
 * 
 *   author        : the_heatherlys
 *   created       : 2010-10
 *   priorities    : direct, simple, brief, vigorous, and lucid (h.w. fowler)
 *   end goal      : loosely coupled, strict interface, maintainable, portable
 * 
 *   fun_image     : thanatos (greek titan of death)
 *
 */
/*===[[ PURPOSE ]]=============================================================*

 *   kharon will...
 *      - act as the ultimate parent of all processes, except the kernel
 *      - handle dead processes
 *      - periodically collect zombies
 *
 *
 *
 *
 *   kharon will be the process root with the very focused mission of handling
 *   terminated processes properly, whether that be death, respawning, or
 *   resurrection.
 *
 *
 *
 */



#include      "init.h"


#include <dirent.h>


/*====================------------------------------------====================*/
/*===----                          main driver                         ----===*/
/*====================------------------------------------====================*/

char sweep (void);

int
main (int argc, char *argv[])
{
   int rc;
   int status;
   struct rusage r_use;
   /*---(lock up this program)-----------*/
	signal(SIGTERM, SIG_IGN);
	signal(SIGSTOP, SIG_IGN);
	signal(SIGKILL, SIG_IGN);
   yLOG_begin("kharon", yLOG_SYSTEM, yLOG_NOISE);
   while (1) {
      sweep();
      /*> while (rc = wait4(-1, &status, WNOHANG, &r_use) > 0) {                                                  <* 
       *>    yLOG_senter("collected");                                                                            <* 
       *>    yLOG_svalue("pid"     , (int) rc);                                                                   <* 
       *>    yLOG_svalue("utime"   , (int) ((r_use.ru_utime.tv_sec * 1000) + (r_use.ru_utime.tv_usec / 1000)));   <* 
       *>    yLOG_svalue("stime"   , (int) ((r_use.ru_stime.tv_sec * 1000) + (r_use.ru_stime.tv_usec / 1000)));   <* 
       *>    yLOG_svalue("switches", (int) r_use.ru_nvcsw + r_use.ru_nivcsw);                                     <* 
       *>    yLOG_snote ("done");                                                                                 <* 
       *>    yLOG_sexit ();                                                                                       <* 
       *> }                                                                                                       <*/
      sleep(5);
   }
   yLOG_end   ();
   /*> execl(SHELL, SHELL, "-c", "/sbin/agetty 38400 tty1 linux", NULL, NULL);        <*/
   return 0;
}

char       /* PURPOSE : run through all processes ----------------------------*/
sweep              (void)
{
   tDIRENT  *den;
   DIR      *dir;
   FILE     *f;
   char      name[100];
   char      line[1000];
   int       rc;
   char     *p;
   int       status;
   struct rusage r_use;
   dir = opendir("/proc");
   if (dir == NULL) return -1;
   while ((den = readdir(dir)) != NULL) {
      if (atoi(den->d_name) == 0) continue;
      sprintf(name, "/proc/%s/stat", den->d_name);
      f = fopen(name, "r");
      if (f == NULL) continue;
      fgets(line, 1000, f);
      p = strtok(line, ")");
      p = strtok(NULL, " ");
      p = strtok(NULL, " ");
      fclose(f);
      if (atoi(p) != 1) continue;
      rc = wait4(atoi(den->d_name), &status, WNOHANG, &r_use);
      if (rc <= 0) continue;
      yLOG_senter("collected");
      yLOG_svalue("pid"     , (int) rc);
      yLOG_sinfo ("char"    , p);
      yLOG_svalue("utime"   , (int) ((r_use.ru_utime.tv_sec * 1000) + (r_use.ru_utime.tv_usec / 1000)));
      yLOG_svalue("stime"   , (int) ((r_use.ru_stime.tv_sec * 1000) + (r_use.ru_stime.tv_usec / 1000)));
      yLOG_svalue("switches", (int) r_use.ru_nvcsw + r_use.ru_nivcsw);
      yLOG_snote ("done");
      yLOG_sexit ();
   }
   closedir(dir);
   return 0;
}

/*============================[[ end-of-code ]]===============================*/
