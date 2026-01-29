#include "argus.h"


char
SHOW__parser_prepare    (char *a_parser, char r_feedback [LEN_RECD])
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        x_ch        =  '-';
   int         l           =    0;
   char        x_curr      [LEN_TERSE] = "";
   char        x_error     [LEN_RECD]  = "";
   /*---(defense)------------------------*/
   DEBUG_PROG   yLOG_point   ("r_feedback", r_feedback);
   --rce;  if (r_feedback == NULL) {
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_PROG   yLOG_point   ("a_parser"  , a_parser);
   --rce;  if (a_parser == NULL) {
      if (r_feedback != NULL)  strlcat (r_feedback, BOLD_ERR "(null)" BOLD_OFF, LEN_RECD);
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_PROG   yLOG_info    ("a_parser"  , a_parser);
   l = strlen (a_parser);
   DEBUG_PROG   yLOG_value   ("l"         , l);
   --rce;  if (l <= 0) {
      if (r_feedback != NULL)  strlcat (r_feedback, BOLD_ERR "(empty)" BOLD_OFF, LEN_RECD);
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (strncmp (a_parser, "#ù "            ,  3) != NULL) {
      sprintf (x_error, BOLD_ERR "%s-3.3s" BOLD_OFF "%s", a_parser, a_parser + 3);
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (strncmp (a_parser, "#ù parsing··´··", 15) != NULL) {
      sprintf (x_error, BOLD_GRN "#ù" BOLD_OFF BOLD_ERR "12.12s" BOLD_OFF "%s", a_parser + 3, a_parser + 15);
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (strncmp (a_parser, "#ù parsing··´··å", 16) != 0) {
      sprintf (x_error, BOLD_GRN "#ù parsing··´··" BOLD_OFF BOLD_ERR "%c" BOLD_OFF "%s", a_parser [15], a_parser + 16);
      if (r_feedback != NULL)  strlcat (r_feedback, x_error, LEN_RECD);
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (l == 17 && strcmp (a_parser, "#ù parsing··´··åæ") == 0) {
      if (r_feedback != NULL)  strlcpy (r_feedback, BOLD_ERR "#ù parsing··´··åæ (no fields)" BOLD_OFF, LEN_RECD);
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare feedback)---------------*/
   if (r_feedback != NULL) {
      strcpy  (r_feedback, BOLD_GRN);
      strlcat (r_feedback, "#ù parsing··´··å", LEN_RECD);
   }
   /*---(complete)-----------------------*/
   return 1;
}

char
SHOW__parser_clear      (char *r_width, char *r_gap, char *r_dots, char *r_suffix, char r_field [LEN_RECD], char *r_delm, char *r_algn, char *r_form, char *r_decs, char *r_unit, char r_check [LEN_HUND])
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(defense)------------------------*/
   --rce;  if (r_width  == NULL)  return rce;
   --rce;  if (r_gap    == NULL)  return rce;
   --rce;  if (r_dots   == NULL)  return rce;
   --rce;  if (r_suffix == NULL)  return rce;
   --rce;  if (r_field  == NULL)  return rce;
   --rce;  if (r_delm   == NULL)  return rce;
   --rce;  if (r_algn   == NULL)  return rce;
   --rce;  if (r_form   == NULL)  return rce;
   --rce;  if (r_decs   == NULL)  return rce;
   --rce;  if (r_unit   == NULL)  return rce;
   /*---(initialize)---------------------*/
   *r_width   = 0;
   *r_gap     = 0;
   *r_dots    = '-';
   *r_suffix  = 0;
   strcpy (r_field, "");
   *r_delm    = '·';
   *r_algn    = '·';
   *r_form    = '·';
   *r_decs    = '·';
   *r_unit    = '·';
   /*---(unit-testing)-------------------*/
   if (r_check   != NULL)  sprintf (r_check, "····· w=%2d, g=%2d, d=%c, s=%d, %c %c %c %c %c , å%sæ", *r_width, *r_gap, *r_dots, *r_suffix, *r_delm, *r_algn, *r_form, *r_decs, *r_unit, r_field);
   /*---(complete)-----------------------*/
   return 1;
}

char
SHOW__parser_title      (char a_ch, char a_curr [LEN_TERSE], char *b_width, char *b_gap, char *b_dots, char *b_suffix, char b_field [LEN_RECD], char r_check [LEN_HUND])
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char       *x_names     = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-_0123456789/èéêëìíîïðñòóôõö÷øùúûüýþÿ·´±Ú§<|> ";
   char       *x_dotss     = "·´±Ú§<|> ";
   /*---(defense)------------------------*/
   --rce;  if (a_curr   == NULL)  return rce;
   --rce;  if (b_width  == NULL)  return rce;
   --rce;  if (b_gap    == NULL)  return rce;
   --rce;  if (b_dots   == NULL)  return rce;
   --rce;  if (b_suffix == NULL)  return rce;
   --rce;  if (b_field  == NULL)  return rce;
   /*---(quick-out)----------------------*/
   if (*b_suffix >  0)    return 0;
   --rce;  if (a_ch == 0)         return rce;
   /*---(check character)----------------*/
   --rce; if (strchr (x_names , a_ch) == NULL) {
      DEBUG_PROG   yLOG_note    ("title can only use use åa-z0-9è-ÿ/_-æ, move on with å·´±Ú§<|>æ, or next field with space ( )");
      return rce;
   }
   --rce;  if (a_ch == '·' && *b_width <= 0) {
      DEBUG_PROG   yLOG_note    ("can not start using dots (·) with no title");
      return rce;
   }
   --rce;  if (*b_dots == 'y' && strchr (x_dotss, a_ch) == NULL) {
      DEBUG_PROG   yLOG_note    ("once using dots, can only use use å·æ, move on with å´±Ú§<|>æ, or next field with space ( )");
      return rce;
   }
   /*---(handle)-------------------------*/
   switch (a_ch) {
   case ' '  :
      DEBUG_PROG   yLOG_note    ("title hit space ( ), field complete");
      ++(*b_gap);
      break;
   case '·'  :
      DEBUG_PROG   yLOG_note    ("title hit dot (·), done with title");
      *b_dots = 'y';
      ++(*b_width);
      break;
   case '´'  :  case '±'  :  case 'Ú'  :  case '§'  :
      DEBUG_PROG   yLOG_note    ("title hit delimiter marker å´±Ú§æ, done with title");
      *b_suffix = 1;
      break;
   case '<'  :  case '|'  :  case '>'  :
      DEBUG_PROG   yLOG_note    ("title hit alignment marker å<|>æ, done with title");
      *b_suffix = 2;
      break;
   default   :
      DEBUG_PROG   yLOG_note    ("title adds a new character");
      ++(*b_width);
      strlcat (b_field, a_curr, LEN_RECD);
      break;
   }
   /*---(unit-testing)-------------------*/
   if (r_check != NULL)  sprintf (r_check, "%c/%-3d w=%2d, g=%2d, d=%c, s=%d, · · · · · , å%sæ", ychrvisible (a_ch), (uchar) a_ch, *b_width, *b_gap, *b_dots, *b_suffix, b_field);
   /*---(complete)-----------------------*/
   return 1;
}

char
SHOW__parser_suffix     (char a_ch, char a_width, char *b_suffix, char *r_delm, char *r_algn, char *r_form, char *r_decs, char *r_unit, char r_check [LEN_HUND])
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char       *x_delms     = "´±Ú§";
   char       *x_algns     = "<|>";
   char       *x_forms     = "iIfF,cCaAsS$#pPeErRbBoOxXzZ-tTdDyY";
   char       *x_decss     = "01253456789";
   char       *x_units     = "YZEPTGMKHD-.dcmunpfazy";
   /*---(defense)------------------------*/
   --rce;  if (b_suffix == NULL)  return rce;
   --rce;  if (r_delm   == NULL)  return rce;
   --rce;  if (r_algn   == NULL)  return rce;
   --rce;  if (r_form   == NULL)  return rce;
   --rce;  if (r_decs   == NULL)  return rce;
   --rce;  if (r_unit   == NULL)  return rce;
   /*---(quick-out)----------------------*/
   if (*b_suffix <= 0)            return 0;
   --rce;  if (*b_suffix > 5)     return rce;
   --rce;  if (a_ch == 0)         return rce;
   /*---(defense)------------------------*/
   --rce;  if (a_width <= 0) {
      DEBUG_PROG   yLOG_note    ("can not start suffix with no title");
      return rce;
   }
   /*---(handle)-------------------------*/
   switch (*b_suffix) {
   case  1  :
      DEBUG_PROG   yLOG_note    ("checking either delimiter or alignment");
      if      (strchr (x_delms, a_ch) != NULL)  {  *r_delm = a_ch;  *b_suffix = 2; }
      else if (strchr (x_algns, a_ch) != NULL)  {  *r_algn = a_ch;  *b_suffix = 3; }
      else                                         return rce - 1;
      break;
   case  2  :
      DEBUG_PROG   yLOG_note    ("checking alignment");
      if (strchr (x_algns, a_ch) != NULL)       {  *r_algn = a_ch;  *b_suffix = 3; }
      else                                         return rce - 2;
      break;
   case  3  :
      DEBUG_PROG   yLOG_note    ("checking formatting");
      if (strchr (x_forms, a_ch) != NULL)       {  *r_form = a_ch;  *b_suffix = 4; }
      else                                         return rce - 3;
      break;
   case  4  :
      DEBUG_PROG   yLOG_note    ("checking decimal places");
      if (strchr (x_decss, a_ch) != NULL)       {  *r_decs = a_ch;  *b_suffix = 5; }
      else                                         return rce - 4;
      break;
   case  5  :
      DEBUG_PROG   yLOG_note    ("checking unit-of-measure");
      if (strchr (x_units, a_ch) != NULL)       {  *r_unit = a_ch;  *b_suffix = 6; }
      else                                         return rce - 5;
      break;
   }
   /*---(unit-testing)-------------------*/
   if (r_check != NULL)  sprintf (r_check, "%c/%-3d w=%2d, ····, ···, s=%d, %c %c %c %c %c , Ï", ychrvisible (a_ch), (uchar) a_ch, a_width, *b_suffix, *r_delm, *r_algn, *r_form, *r_decs, *r_unit);
   /*---(complete)-----------------------*/
   return 1;
}

char
SHOW__parser_field      (char *b_count, char a_ch, char a_width, char a_gap, char a_field [LEN_FULL], char a_delm, char a_algn, char a_form, char a_decs, char a_unit, char b_header [LEN_RECD], char b_break [LEN_RECD], char b_control [LEN_RECD])
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        x_algn      =  '·';
   char        t           [LEN_FULL]  = "";
   char        x_val       [LEN_TERSE] = "";
   /*---(defense)------------------------*/
   --rce;  if (b_count   == NULL)    return rce;
   --rce;  if (a_field   == NULL)    return rce;
   --rce;  if (a_delm    == NULL)    return rce;
   --rce;  if (a_algn    == NULL)    return rce;
   --rce;  if (a_form    == NULL)    return rce;
   --rce;  if (a_decs    == NULL)    return rce;
   --rce;  if (a_unit    == NULL)    return rce;
   --rce;  if (b_header  == NULL)    return rce;
   --rce;  if (b_break   == NULL)    return rce;
   --rce;  if (b_control == NULL)    return rce;
   /*---(quick-out)----------------------*/
   --rce;  if (a_width   <= 0)       return rce;
   --rce;  if (a_gap     <  0)       return rce;
   if (a_ch != 0 && strchr (" æ", a_ch) == NULL)  return 0;
   /*---(prepare)------------------------*/
   if (*b_count == 0) {
      strcpy (b_header , "");
      strcpy (b_break  , "");
      strcpy (b_control, "");
   }
   ++(*b_count);
   if (a_delm == '·')  a_delm = '´';
   if (a_algn == '·')  x_algn = '<';
   else                x_algn = a_algn;
   /*---(add gap)------------------*/
   if (a_gap > 0) {
      ystrlpad (""      , t, '²', '<'    , a_gap);
      strlcat  (b_header, t, LEN_RECD);
      strlcat  (b_break , t, LEN_RECD);
   }
   /*---(add header)---------------*/
   ystrlpad (a_field, t, '·', x_algn, a_width);
   DEBUG_PROG   yLOG_info    ("t"         , t);
   strlcat  (b_header, t, LEN_RECD);
   DEBUG_PROG   yLOG_info    ("b_header"  , b_header);
   /*---(add break)----------------*/
   ystrlpad ("-"    , t, '·', x_algn, a_width);
   DEBUG_PROG   yLOG_info    ("t"         , t);
   strlcat  (b_break , t, LEN_RECD);
   DEBUG_PROG   yLOG_info    ("b_break"   , b_break);
   /*---(add control)--------------*/
   ystrlpadn (a_width, x_val, '·', '>'    , 3);
   sprintf (t, "%c%s%c%c%c%c  ", a_delm, x_val, a_algn, a_form, a_decs, a_unit);
   DEBUG_PROG   yLOG_info    ("t"         , t);
   strlcat  (b_control, t, LEN_RECD);
   DEBUG_PROG   yLOG_info    ("b_control" , b_control);
   /*---(complete)-----------------------*/
   return 1;
}

char
SHOW_parser_interpret   (char *a_parser, char *r_header, char *r_break, int a_max, char r_control [LEN_RECD], char r_feedback [LEN_RECD])
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        c           =    0;
   char        x_ch        =  '-';
   char        x_curr      [LEN_TERSE] = "";
   char        x_error     [LEN_RECD]  = "";
   int         l           =    0;
   int         i           =    0;
   /*---(parsring)-----------------------*/
   char        x_field     [LEN_FULL]  = "";
   char        x_width     =    0;
   char        x_gap       =    0;
   char        x_dots      =  '-';
   char        x_suffix    =    0;
   char        x_algnt     =  '·';
   /*---(gyges present)------------------*/
   char        x_delm      =  '·';
   char        x_algn      =  '·';
   char        x_form      =  '·';
   char        x_decs      =  '·';
   char        x_unit      =  '·';
   /*---(valid strings)------------------*/
   char       *x_valid     = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-_0123456789/èéêëìíîïðñòóôõö÷øùúûüýþÿ·´±Ú§<|>,.$# ";
   /*---(outputs)------------------------*/
   char        x_header    [LEN_RECD]  = "";
   char        x_break     [LEN_RECD]  = "";
   char        x_control   [LEN_RECD]  = "";
   char        x_feedback  [LEN_RECD]  = "";
   /*---(trouble marker)-----------------*/
   char        x_boom      =    0;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(default)------------------------*/
   if (r_header   != NULL)  strcpy (r_header  , "");
   if (r_break    != NULL)  strcpy (r_break   , "");
   if (r_control  != NULL)  strcpy (r_control , "");
   if (r_feedback != NULL)  strcpy (r_feedback, "");
   /*---(defense)------------------------*/
   rc = SHOW__parser_prepare (a_parser, x_feedback);
   DEBUG_PROG   yLOG_value   ("prepare"   , rc);
   --rce;  if (rc < 0) {
      if (r_feedback != NULL)  strlcpy (r_feedback, x_feedback, LEN_RECD);
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   SHOW__parser_clear (&x_width, &x_gap, &x_dots, &x_suffix, x_field, &x_delm, &x_algn, &x_form, &x_decs, &x_unit, NULL);
   l = strlen (a_parser);
   /*---(walk characters)----------------*/
   --rce;  for (i = 16; i <= l; ++i) {
      /*---(prepare loop)----------------*/
      DEBUG_PROG   yLOG_complex ("LOOP"      , "%3d, g=%2d w=%2d,%s, d=%c, s=%d, %c %c %c %c %c", i, x_gap, x_width, x_field, x_dots, x_suffix, x_delm, x_algn, x_form, x_decs, x_unit);
      x_ch = a_parser [i];
      DEBUG_PROG   yLOG_char    ("x_ch"      , x_ch);
      sprintf (x_curr , "%c", x_ch);
      if (x_ch != ' ')  sprintf (x_error, BOLD_OFF BOLD_ERR "%c" BOLD_OFF "%s", x_ch, a_parser + i + 1);
      else              sprintf (x_error, BOLD_OFF BOLD_ERR "%c" BOLD_OFF "%s", '²' , a_parser + i + 1);
      DEBUG_PROG   yLOG_info    ("x_curr"    , x_curr);
      /*---(handle complete field)-------*/
      rc = 0;
      rc = SHOW__parser_field (&c, x_ch, x_width, x_gap, x_field, x_delm, x_algn, x_form, x_decs, x_unit, x_header, x_break, x_control);
      DEBUG_PROG   yLOG_value   ("field"     , rc);
      if (rc == 1) {
         SHOW__parser_clear (&x_width, &x_gap, &x_dots, &x_suffix, x_field, &x_delm, &x_algn, &x_form, &x_decs, &x_unit, NULL);
         DEBUG_PROG   yLOG_value   ("clear"     , rc);
      }
      /*---(filters for end)-------------*/
      if (x_ch == '\0')  break;
      if (x_ch == 'æ')   break;
      /*---(defenses)--------------------*/
      if (strchr (x_valid   , x_ch) == NULL) {
         DEBUG_PROG   yLOG_note    ("used illegal character");
         x_boom = rce - 1;
         break;
      }
      /*---(titles)----------------------*/
      x_boom = SHOW__parser_title  (x_ch, x_curr, &x_width, &x_gap, &x_dots, &x_suffix, x_field, NULL);
      if (x_boom < 0) break;
      /*---(suffix)----------------------*/
      if (x_boom >= 0)  x_boom = SHOW__parser_suffix (x_ch, x_width, &x_suffix, &x_delm, &x_algn, &x_form, &x_decs, &x_unit, NULL);
      if (x_boom < 0) break;
      /*---(concatinate)-----------------*/
      if (x_feedback != NULL)  strlcat (x_feedback, x_curr, LEN_RECD);
      /*---(done)------------------------*/
   }
   /*---(check end)----------------------*/
   DEBUG_PROG   yLOG_value   ("x_boom"    , x_boom);
   rce -= 20;
   if (x_boom >= 0 && x_ch != 'æ') {
      DEBUG_PROG   yLOG_note    ("did not have æ at end");
      x_boom = rce;
      sprintf (x_error, BOLD_OFF BOLD_ERR "¢" BOLD_OFF);
   }
   /*---(check trouble)------------------*/
   DEBUG_PROG   yLOG_value   ("x_boom"    , x_boom);
   if (x_boom < 0) {
      if (x_feedback != NULL)  strlcat (x_feedback, x_error   , LEN_RECD);
      if (r_feedback != NULL)  strlcpy (r_feedback, x_feedback, LEN_RECD);
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, x_boom);
      return x_boom;
   }
   /*---(finish feedback)----------------*/
   strlcat (x_feedback, x_curr, LEN_RECD);
   strlcat (x_feedback, BOLD_OFF, LEN_RECD);
   if (x_header [0] != '\0')  x_header [0] = '#';
   if (x_header [1] != '\0')  x_header [1] = '#';
   if (x_break  [0] != '\0')  x_break  [0] = '#';
   if (x_break  [1] != '\0')  x_break  [1] = '#';
   /*---(save-back)----------------------*/
   if (r_header   != NULL)  strlcpy (r_header  , x_header  , a_max);
   if (r_break    != NULL)  strlcpy (r_break   , x_break   , a_max);
   if (r_control  != NULL)  strlcpy (r_control , x_control , LEN_RECD);
   if (r_feedback != NULL)  strlcpy (r_feedback, x_feedback, LEN_RECD);
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return c;
}

char
SHOW__parsing           (char *a_parser, char *r_header, char *r_break, int a_max, char r_control [LEN_RECD], char r_feedback [LEN_RECD])
{
   char        rce         =  -10;
   int         l           =    0;
   int         i           =    0;
   char        c           =    0;
   char        x_ch        =  '-';
   char        x_curr      [LEN_TERSE] = "";
   char        x_error     [LEN_RECD]  = "";
   char        x_gap       =    0;
   char        t           [LEN_FULL]  = "";
   char        x_val       [LEN_TERSE] = "";
   /*---(parsring)-----------------------*/
   char        x_field     [LEN_FULL]  = "";
   char        x_width     =    0;
   char        x_dots      =  '-';
   char        x_suffix    =    0;
   char        x_algnt     =  '·';
   /*---(gyges present)------------------*/
   char        x_delm      =  '·';
   char        x_algn      =  '·';
   char        x_form      =  '·';
   char        x_decs      =  '·';
   char        x_unit      =  '·';
   /*---(valid strings)------------------*/
   char       *x_valid     = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-_0123456789/èéêëìíîïðñòóôõö÷øùúûüýþÿ·´±Ú§<|>,.$# ";
   char       *x_names     = "abcdefghijklmnopqrstuvwxyz-_0123456789/èéêëìíîïðñòóôõö÷øùúûüýþÿ·´±Ú§<|> ";
   char       *x_dotss     = "·´±Ú§<|> ";
   char       *x_delms     = "´±Ú§";
   char       *x_algns     = "<|>";
   char       *x_forms     = "iIfF,cCaAsS$#pPeErRbBoOxXzZ-tTdDyY";
   char       *x_decss     = "01253456789";
   char       *x_units     = "YZEPTGMKHD-.dcmunpfazy";
   char       *x_symbols   = "´±Ú§<|> ";
   /*---(trouble marker)-----------------*/
   char        x_boom      =    0;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(default)------------------------*/
   if (r_header   != NULL)  strcpy (r_header  , "");
   if (r_break    != NULL)  strcpy (r_break   , "");
   if (r_control  != NULL)  strcpy (r_control , "");
   if (r_feedback != NULL)  strcpy (r_feedback, "");
   /*---(defense)------------------------*/
   DEBUG_PROG   yLOG_point   ("a_parser"  , a_parser);
   --rce;  if (a_parser == NULL) {
      if (r_feedback != NULL)  strlcat (r_feedback, BOLD_ERR "(null)" BOLD_OFF, LEN_RECD);
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_PROG   yLOG_info    ("a_parser"  , a_parser);
   l = strlen (a_parser);
   DEBUG_PROG   yLOG_value   ("l"         , l);
   --rce;  if (l <= 0) {
      if (r_feedback != NULL)  strlcat (r_feedback, BOLD_ERR "(empty)" BOLD_OFF, LEN_RECD);
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (l == 2 && strcmp (a_parser, "åæ") == 0) {
      if (r_feedback != NULL)  strlcat (r_feedback, BOLD_ERR "(n/a)" BOLD_OFF, LEN_RECD);
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check start)--------------------*/
   x_ch = a_parser [0];
   sprintf (x_curr , "%c", x_ch);
   sprintf (x_error, BOLD_ERR "%c" BOLD_OFF "%s", x_ch, a_parser + 1);
   --rce;  if (x_ch != 'å') {
      DEBUG_PROG   yLOG_note    ("did not have å at beg");
      if (r_feedback != NULL)  strlcat (r_feedback, x_error, LEN_RECD);
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare)------------------------*/
   if (r_feedback != NULL)  strcpy (r_feedback, BOLD_GRN);
   if (r_feedback != NULL)  strlcat (r_feedback, x_curr, LEN_RECD);
   /*---(walk characters)----------------*/
   --rce;  for (i = 1; i <= l; ++i) {
      /*---(prepare loop)----------------*/
      DEBUG_PROG   yLOG_complex ("LOOP"      , "%3d, g=%2d w=%2d,%s, d=%c, s=%d, %c %c %c %c %c", i, x_gap, x_width, x_field, x_dots, x_suffix, x_delm, x_algn, x_form, x_decs, x_unit);
      x_ch = a_parser [i];
      DEBUG_PROG   yLOG_char    ("x_ch"      , x_ch);
      sprintf (x_curr , "%c", x_ch);
      if (x_ch != ' ')  sprintf (x_error, BOLD_OFF BOLD_ERR "%c" BOLD_OFF "%s", x_ch, a_parser + i + 1);
      else              sprintf (x_error, BOLD_OFF BOLD_ERR "%c" BOLD_OFF "%s", '²' , a_parser + i + 1);
      DEBUG_PROG   yLOG_info    ("x_curr"    , x_curr);
      /*---(handle complete field)-------*/
      if (x_width > 0 && (x_ch == '\0' || strchr ("å æ", x_ch) != NULL)) {
         DEBUG_PROG   yLOG_note    ("triggered writting field");
         ++c;
         if (x_delm == '·')  x_delm  = '´';
         if (x_algn == '·')  x_algnt = '<';
         else                x_algnt = x_algn;
         /*---(add gap)------------------*/
         if (x_gap > 0) {
            ystrlpad (""      , t, '²', '<'    , x_gap);
            if (r_header != NULL)  strlcat  (r_header, t, a_max);
            if (r_break  != NULL)  strlcat  (r_break , t, a_max);
         }
         /*---(add header)---------------*/
         ystrlpad (x_field, t, '·', x_algnt, x_width);
         DEBUG_PROG   yLOG_info    ("t"         , t);
         if (r_header  != NULL)  strlcat  (r_header, t, a_max);
         DEBUG_PROG   yLOG_info    ("r_header"  , r_header);
         /*---(add break)----------------*/
         ystrlpad ("-"    , t, '·', x_algnt, x_width);
         DEBUG_PROG   yLOG_info    ("t"         , t);
         if (r_break   != NULL)  strlcat  (r_break , t, a_max);
         DEBUG_PROG   yLOG_info    ("r_break"   , r_break);
         /*---(add control)--------------*/
         ystrlpadn (x_width, x_val, '·', '>'    , 3);
         sprintf (t, "%c%s%c%c%c%c  ", x_delm, x_val, x_algn, x_form, x_decs, x_unit);
         DEBUG_PROG   yLOG_info    ("t"         , t);
         if (r_control != NULL)  strlcat  (r_control, t, a_max);
         DEBUG_PROG   yLOG_info    ("r_control" , r_control);
         /*---(reset values)-------------*/
         x_gap     = 0;
         strcpy (x_field, "");
         x_width = 0;
         x_dots    = '-';
         x_suffix  = 0;
         x_delm = x_algn = x_form = x_decs = x_unit = '·';
         /*---(done)---------------------*/
      }
      /*---(filters for end)-------------*/
      if (x_ch == '\0')  break;
      if (x_ch == 'æ')   break;
      /*---(defenses)--------------------*/
      if (strchr (x_valid   , x_ch) == NULL) {
         DEBUG_PROG   yLOG_note    ("used illegal character");
         x_boom = rce - 1;
         break;
      }
      /*---(titles)----------------------*/
      --rce;  if (x_dots != 'y' && x_suffix == 0) {
         if (strchr (x_names , x_ch) == NULL) {
            DEBUG_PROG   yLOG_note    ("title can only use use åa-z0-9è-ÿ/_-æ, move on with å·´±Ú§<|>æ, or next field with space ( )");
            x_boom = rce - 2;
            break;
         }
         switch (x_ch) {
         case ' '  :  ++x_gap;                                              break;
         case '·'  :  x_dots = 'y';                                         break;
         case '´'  :  case '±'  :  case 'Ú'  :  case '§'  :  x_suffix = 1;  break;
         case '<'  :  case '|'  :  case '>'  :               x_suffix = 2;  break;
         default   :  ++x_width;   strlcat (x_field, x_curr, LEN_RECD);     break;
         }
      }
      /*---(dots)------------------------*/
      if (x_dots == 'y' && x_suffix == 0) {
         if (strchr (x_dotss, x_ch) == NULL) {
            DEBUG_PROG   yLOG_note    ("dots can only use use å·æ, move on with å´±Ú§<|>æ, or next field with space ( )");
            x_boom = rce - 3;
            break;
         }
         if (x_width <= 0) {
            DEBUG_PROG   yLOG_note    ("can not start using dots (·) with no title");
            x_boom = rce - 4;
            break;
         }
         switch (x_ch) {
         case ' '  :  ++x_gap;    break;
         case '·'  :  ++x_width;                                            break;
         case '´'  :  case '±'  :  case 'Ú'  :  case '§'  :  x_suffix = 1;  break;
         case '<'  :  case '|'  :  case '>'  :               x_suffix = 2;  break;
         }
      }
      /*---(suffix)----------------------*/
      if (x_suffix > 0) {
         if (x_width <= 0) {
            DEBUG_PROG   yLOG_note    ("can not start suffix with no title");
            x_boom = rce - 5;
            break;
         }
         switch (x_suffix) {
         case  1  :
            DEBUG_PROG   yLOG_note    ("checking either delimiter or alignment");
            if      (strchr (x_delms, x_ch) != NULL)  {  x_delm = x_ch;  x_suffix = 2; }
            else if (strchr (x_algns, x_ch) != NULL)  {  x_algn = x_ch;  x_suffix = 3; }
            else                                         x_boom = rce - 10;
            break;
         case  2  :
            DEBUG_PROG   yLOG_note    ("checking alignment");
            if (strchr (x_algns, x_ch) != NULL)  {  x_algn = x_ch;  x_suffix = 3; }
            else                                         x_boom = rce - 11;
            break;
         case  3  :
            DEBUG_PROG   yLOG_note    ("checking formatting");
            if (strchr (x_forms, x_ch) != NULL)  {  x_form = x_ch;  x_suffix = 4; }
            else                                         x_boom = rce - 12;
            break;
         case  4  :
            DEBUG_PROG   yLOG_note    ("checking decimal places");
            if (strchr (x_decss, x_ch) != NULL)  {  x_decs = x_ch;  x_suffix = 5; }
            else                                         x_boom = rce - 13;
            break;
         case  5  :
            DEBUG_PROG   yLOG_note    ("checking unit-of-measure");
            if (strchr (x_units, x_ch) != NULL)  {  x_unit = x_ch;  x_suffix = 6; }
            else                                         x_boom = rce - 14;
            break;
         default  :
            DEBUG_PROG   yLOG_note    ("suffix identifier unknown");
            x_boom = rce - 15;
            break;
         }
      }
      /*---(trouble)---------------------*/
      if (x_boom < 0) {
         DEBUG_PROG   yLOG_note    ("found trouble, ended looping");
         break;
      }
      /*---(concatinate)-----------------*/
      if (r_feedback != NULL)  strlcat (r_feedback, x_curr, LEN_RECD);
      /*---(done)------------------------*/
   }
   /*---(check end)----------------------*/
   DEBUG_PROG   yLOG_value   ("x_boom"    , x_boom);
   rce -= 20;
   if (x_boom == 0 && x_ch != 'æ') {
      DEBUG_PROG   yLOG_note    ("did not have æ at end");
      x_boom = rce;
      sprintf (x_error, BOLD_OFF BOLD_ERR "¢" BOLD_OFF);
   }
   /*---(check trouble)------------------*/
   DEBUG_PROG   yLOG_value   ("x_boom"    , x_boom);
   if (x_boom < 0) {
      if (r_header   != NULL)  strcpy  (r_header  , "");
      if (r_break    != NULL)  strcpy  (r_break   , "");
      if (r_control  != NULL)  strcpy  (r_control , "");
      if (r_feedback != NULL)  strlcat (r_feedback, x_error, LEN_RECD);
      DEBUG_PROG   yLOG_exitr   (__FUNCTION__, x_boom);
      return x_boom;
   }
   /*---(finish feedback)----------------*/
   if (r_feedback != NULL)  strlcat (r_feedback, x_curr, LEN_RECD);
   if (r_feedback != NULL)  strlcat (r_feedback, BOLD_OFF, LEN_RECD);
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return c;
}

char*
SHOW__timestamp         (void)
{
   /*---(locals)-----------+-----+-----+-*/
   long        x_now       =    0;
   tTIME      *x_broke     = NULL;
   static char t           [LEN_TITLE] = "";
   /*---(handle)-------------------------*/
   x_now  = time (NULL);
   x_broke   = localtime (&x_now);
   strftime (t, LEN_TITLE, "%y.%m.%d.%H.%M.%S.%u.%W.%j", x_broke);
   /*---(complete------------------------*/
   return t;
}

char
SHOW_exim               (char c_content, int c, char c_exim, char c_titles)
{
   char        rce         =  -10;
   char        rc          =    0;
   char       *x_group     = "#ù parsing··´··åID  GID··>  LN> name········<  q  a  CNT>  LN> unames············································<  LN> uids····································<æ";
   char       *x_gspace    = "´·  ´····  ´· ´···········  ´  ´  ´··  ´· ´·················································  ´· ´·······································  Ï";
   char       *x_gheader   = "##  --gid  ln name--------  q  a  cnt  ln unames--------------------------------------------  ln uids------------------------------------  Ï";
   char       *x_gbreak    = "·-  -····  ·- -···········  -  -  ··-  ·- -·················································  ·- -·······································  Ï";
   char       *r, *t, *s, *h, *b;
   static char x_header    [LEN_RECD]  = "";
   static char x_break     [LEN_RECD]  = "";
   static char x_control   [LEN_RECD]  = "";
   static char x_feedback  [LEN_RECD]  = "";
   static char x_host      [LEN_HUND]  = "";
   /*---(handle openning)----------------*/
   if (c == 0) {
      rc = SHOW_parser_interpret (x_group, x_header, x_break, LEN_FULL, x_control, x_feedback);
      /*> fprintf (my.m_output, "parsing %d\n", rc);                                  <* 
       *> fprintf (my.m_output, "## header···´··å%sæ\n", x_header);                   <* 
       *> fprintf (my.m_output, "## break····´··å%sæ\n", x_break);                    <* 
       *> fprintf (my.m_output, "## control··´··å%sæ\n", x_control);                  <* 
       *> fprintf (my.m_output, "## feedback·´··å%sæ\n", x_feedback);                 <*/
      if ( c_exim == 'y') {
         gethostname (x_host, LEN_HUND);
         fprintf (my.m_output, "#!/usr/local/bin/argus%s\n", my.m_cmdline);
         fprintf (my.m_output, "\n");
         fprintf (my.m_output, "#ö orgin····´··å%sæ\n", P_ONELINE);
         fprintf (my.m_output, "#ý version··´··å%s: %sæ\n", P_VERNUM, P_VERTXT);
         fprintf (my.m_output, "#î time·····´··å%sæ\n", SHOW__timestamp());
         fprintf (my.m_output, "#ó machine··´··å%sæ\n", x_host);
         fprintf (my.m_output, "#÷ process··´··å%dæ\n", getpid ());
         fprintf (my.m_output, "\n");
         fprintf (my.m_output, "#ú title····´··å%sæ\n", "linux group inventory (local system)");
         fprintf (my.m_output, "%s\n", x_group);
      } else if (c_titles == 'y') {
         fprintf (my.m_output, "## %s\n", "group inventory");
      }
   }
   /*---(handle titles)------------------*/
   if (c_titles == 'y') {
      if      (c % 25 == 0) fprintf (my.m_output, "\n%s\n\n", x_header);
      else if (c %  5 == 0) fprintf (my.m_output, "\n%s\n\n", x_break);
   }
   /*---(handle closing)-----------------*/
   if (c == -1) {
      if (c_titles == 'y' && c %  5 >= 3) fprintf (my.m_output, "\n%s\n", x_break);
      if (c_exim   == 'y') {
         if (c_titles == 'y') fprintf (my.m_output, "\n");
         fprintf (my.m_output, "#ÿ end-of-data.  fatto, completare, finito, morto, sepolta, y la signora grassa sta cantando.");
      }
   }
   /*---(complete)-----------------------*/
   return 0;
}

