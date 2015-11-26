/*
        upic_DLPACK.C
         Created           :  3-JAN-1990 by Christian Arnault
*/

//---------------------------------------------------------------------------
#include "UPI/upidef.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#define YES "Yes"
#define NO  "No "
static size_t Lines = 0;
static size_t Cursor = 0;
#ifdef SCREEN
static size_t Cols = 0;
static char Blank[1024] = "\
                                                  \
                                                  \
                                                  \
                                                  ";
static char Text[3];
static const char* Yes_no[] = { NO, YES };
#endif  
static char* Buffer;
static int Was_param = 0;

//---------------------------------------------------------------------------
int upic_dlinput ()  {
  int menu, command, param;
  return upic_get_input (&menu, &command, &param);
}

//---------------------------------------------------------------------------
int upic_dlhead (const char* title, int lines, int cols)  {
#ifdef SCREEN
  int len, i;  
  if (Lines) upic_delete_menu (UPI_DLPACK);
  upic_open_detached_menu (UPI_DLPACK, 0, 0, " ", title, " ");
  if (!lines) lines = 5;
  if (!cols) cols = 60;
  if (cols < 10) cols = 10;
  len = strlen(title);
  if (len < cols) len = cols;
  Blank[len-1] = '.';
  Blank[len] = 0;
  upic_add_comment (1, Blank, " ");
  Blank[len] = ' ';
  if (lines < 2) lines = 2;
  for (i=1; i<lines; i++) upic_add_comment (i, " ", " ");
  upic_close_menu();
  Cursor = 1;
  Lines = lines;
  Cols = cols;
  Was_param = 0;
#else
  upir_dlhead (title, lines, cols);
#endif
  return UPI_SS_NORMAL;
}

//---------------------------------------------------------------------------
void upic_dlcheck()  {
  if (!Lines) upic_dlhead ("DLPACK", 5, 60);
}

//---------------------------------------------------------------------------
void upic_install_dl_line (const char* text) {
  upic_replace_command (UPI_DLPACK, Cursor, text, " ");
  upic_enable_command (UPI_DLPACK, Cursor);
  if (Was_param)  {
    int i = Cursor - 1;
    if (i <= 0) i = Lines;
    upic_disable_command (UPI_DLPACK, i);
  }
  upic_set_cursor (UPI_DLPACK, Cursor, 0);
  Cursor++;
  if (Cursor > Lines) Cursor = 1;
}

//---------------------------------------------------------------------------
void upic_do_param_line (const char* prompt, int length) {
  int p_len = strlen(prompt);
  Buffer = list_malloc (p_len + length + 1);
  strcpy (Buffer, prompt);
  char* field = Buffer + p_len;
  memset (field, '^', length);
  field[length] = 0;
  upic_install_dl_line (Buffer);
  Was_param = 1;
  free (Buffer);
}

//---------------------------------------------------------------------------
void upic_dlout_any (const char* text, int var1, int var2, int nvar, const char* format) {
  upic_dlcheck();  
  Buffer = list_malloc (strlen(text) + 1 + nvar * 10);
  strcpy (Buffer, text);
  int i = strlen(Buffer);
  if (nvar >= 1) ::sprintf (&Buffer[i], format, var1);
  i = strlen(Buffer);
  if (nvar >= 2) ::sprintf (&Buffer[i], format, var2);
  upic_install_dl_line (Buffer);
  Was_param = 0;
  free (Buffer);
}

//---------------------------------------------------------------------------
int upic_dlout (const char* text, int var1, int var2, int nvar)   {
#ifdef SCREEN
  upic_dlout_any (text, var1, var2, nvar, " %d");
#else
  upir_dlout (text, var1, var2, nvar);
#endif
  return UPI_SS_NORMAL;
}

//---------------------------------------------------------------------------
int upic_dlouto (const char* text, int var1,  int var2,  int nvar)    {
#ifdef SCREEN
  upic_dlout_any (text, var1, var2, nvar, " %o");
#else
  upir_dlouto (text, var1, var2, nvar);
#endif
  return UPI_SS_NORMAL;
}

//---------------------------------------------------------------------------
int upic_dloutx (const char* text, int var1, int var2, int nvar)    {
#ifdef SCREEN
  upic_dlout_any (text, var1, var2, nvar, " %x");
#else
  upir_dloutx (text, var1, var2, nvar);
#endif
  return UPI_SS_NORMAL;
}

//---------------------------------------------------------------------------
int upic_dlkey ()   {
#ifdef SCREEN
  upic_dlcheck();
  upic_dlout_any ("Hit [Return] to continue ", 0, 0, 0, 0);
  return upic_dlinput ();
#else
  return upir_dlkey ();
#endif
}

//---------------------------------------------------------------------------
int upic_dltxt (const char* prompt, char* def, char* value, int length, int* 
#ifdef SCREEN
) {
  char format[12];  
  upic_dlcheck();
  sprintf (format, "A%d", length);
  if (strlen(prompt) + length > Cols) length = Cols - strlen(prompt);
  if (length <= 0) return UPI_SS_INVFORM;
  upic_set_param (value, 1, format, def, 0, 0, 0, 0, 0);
  upic_do_param_line (prompt, length);
  int status = upic_dlinput ();
#else
ret_len) {
  int status = upir_dltxt (prompt, def, value, length, ret_len);
#endif
  return (status);
}

//---------------------------------------------------------------------------
int upic_dldec (const char* prompt, int def, int* value, int min, int max)  {
  int status;
#ifdef SCREEN
  char format[12];

  upic_dlcheck();
  int length = 10;
  sprintf (format, "I%d", length);
  if (strlen(prompt) + length > Cols) length = Cols - strlen(prompt);
  if (length <= 0) return UPI_SS_INVFORM;
  upic_set_param (value, 1, format, def, min, max, 0, 0, 0);
  upic_do_param_line (prompt, length);
  status = upic_dlinput ();
#else
  status = upir_dldec (prompt, def, value, min, max);
#endif
  return (status);
}

//---------------------------------------------------------------------------
int upic_dloct (const char* prompt, int def, int* value, int min, int max)  {
#ifdef SCREEN
  char format[12];
  int length = 10;  
  upic_dlcheck();
  sprintf (format, "O%d", length);
  if (strlen(prompt) + length > Cols) length = Cols - strlen(prompt);
  if (length <= 0) return UPI_SS_INVFORM;
  upic_set_param (value, 1, format, def, min, max, 0, 0, 0);
  upic_do_param_line (prompt, length);
  int status = upic_dlinput ();
#else
  int status = upir_dloct (prompt, def, value, min, max);
#endif
  return (status);
}

//---------------------------------------------------------------------------
int upic_dlhex (const char* prompt, int def, int* value, int min, int max)  {
#ifdef SCREEN
  char format[12];
  int length = 10;
  
  upic_dlcheck();
  sprintf (format, "X%d", length);
  if (strlen(prompt) + length > Cols) length = Cols - strlen(prompt);
  if (length <= 0) return UPI_SS_INVFORM;
  upic_set_param (value, 1, format, def, min, max, 0, 0, 0);
  upic_do_param_line (prompt, length);
  int status = upic_dlinput ();
#else
  int status = upir_dlhex (prompt, def, value, min, max);
#endif
  return (status);
}

//---------------------------------------------------------------------------
int upic_dlmask (const char* prompt, int def, int* value) {
#ifdef SCREEN
  char format[12];
  int length = 16;
  upic_dlcheck();
  sprintf (format, "M%d", length);
  if (strlen(prompt) + length > Cols) length = Cols - strlen(prompt);
  if (length <= 0) return UPI_SS_INVFORM;
  upic_set_param (value, 1, format, def, 0, 0xffff, 0, 0, 0);
  upic_do_param_line (prompt, length);
  int status = upic_dlinput ();
#else
  int status = upir_dlmask (prompt, def, value);
#endif
  return (status);
}

//---------------------------------------------------------------------------
int upic_dlm32 (const char* prompt, int def, int* value)    {
#ifdef SCREEN
  char format[12];
  int length=32;
  upic_dlcheck();
  sprintf (format, "M%d", length);
  if (strlen(prompt) + length > Cols) length = Cols - strlen(prompt);
  if (length <= 0) return UPI_SS_INVFORM;
  upic_set_param (value, 1, format, def, 0, 0, 0, 0, 0);
  upic_do_param_line (prompt, length);
  int status = upic_dlinput ();
#else
  int status = upir_dlm32 (prompt, def, value);
#endif
  return (status);
}

//---------------------------------------------------------------------------
int upic_dlyeno ( const char* prompt, int def, int* value)  {
#ifdef SCREEN
  char format[12];
  int length = 3;
  upic_dlcheck();
  if (def) strcpy (Text, YES);
  else strcpy (Text, NO);
  sprintf (format, "A%d", length);
  if (strlen(prompt) + length > Cols) length = Cols - strlen(prompt);
  if (length <= 0) return UPI_SS_INVFORM;
  upic_set_param (Text, 1, format, Text, 0, 0, Yes_no, 2, 1);
  upic_do_param_line (prompt, length);
  int status = upic_dlinput ();
  if (status == UPI_SS_NORMAL && Text[0] == 'Y') *value = 1;
  else *value = 0;
#else
  int status = upir_dlyeno (prompt, def, value);
#endif
  return (status);
}

//---------------------------------------------------------------------------
int upic_dlend ()   {
#ifdef SCREEN
  upic_delete_menu (UPI_DLPACK);
  Lines = 0;
#else
  upir_dlend ();
#endif
  return UPI_SS_NORMAL;
}
