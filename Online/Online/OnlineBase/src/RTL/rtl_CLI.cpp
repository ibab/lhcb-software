#include "RTL/rtl.h"
#include "RTL/strdef.h"
#include <cstdarg>
#include <cstring>
#include <cstdio>
#include <string>

static int string_assign(const char* p, const char* fmt, ...)  {
  va_list marker;
  va_start( marker, fmt );     /* Initialize variable arguments. */
  std::string *s = va_arg(marker, std::string*);
  va_end(marker);
  if ( s ) {
    *s = p; 
    return 1;
  }
  return 0;
}

const char* RTL::CLI::getopt(const char* name, int nchars)  {
  if ( name )  {
    for (int i=1; i<m_argc; ++i)  {
      const char* cptr = m_argv[i];
      if ( *cptr == '-' || *cptr == '/' ) {
        if ( cptr[1] == name[0] && ::strncmp(name,cptr+1,nchars)==0 )  {
          const char* loc = ::strchr_safe(cptr+1,'=');
          if ( loc )  {
            return loc+1;
          }
          return cptr + strlen(cptr);
        }
        else if ( cptr[1] == '?' || cptr[1] == 'h' || cptr[1] == 'H' ) {
          if ( m_help ) {
            (*m_help)();
          }
          ::exit(0);
        }
      }
    }
    return 0;
  }
  if ( m_help ) {
    (*m_help)();
    ::exit(0);
  }
  return 0;
}

int RTL::CLI::getopt(const char* name, int nchars, void* value, const char* fmt, int (*deformat)(const char*, const char*,...))  {
  const char* loc = getopt(name, nchars);
  if ( loc )  {
    if ( value && fmt )  {
      (*deformat)(loc,fmt,value);
    }
    return 1;
  }
  return 0;
}

int RTL::CLI::getopt(const char* name, int nchars, char& value)   
{  return getopt(name, nchars, &value, "%c", ::sscanf);                   }

int RTL::CLI::getopt(const char* name, int nchars, unsigned char& value)
{  return getopt(name, nchars, &value, "%c", ::sscanf);                   }

int RTL::CLI::getopt(const char* name, int nchars, short& value)
{  return getopt(name, nchars, &value, "%d", ::sscanf);                   }

int RTL::CLI::getopt(const char* name, int nchars, unsigned short& value) 
{  return getopt(name, nchars, &value, "%d", ::sscanf);                   }

int RTL::CLI::getopt(const char* name, int nchars, int& value)
{  return getopt(name, nchars, &value, "%d", ::sscanf);                   }

int RTL::CLI::getopt(const char* name, int nchars, unsigned int& value) 
{  return getopt(name, nchars, &value, "%d", ::sscanf);                   }

int RTL::CLI::getopt(const char* name, int nchars, long& value)   
{  return getopt(name, nchars, &value, "%ld", ::sscanf);                  }

int RTL::CLI::getopt(const char* name, int nchars, unsigned long& value)   
{  return getopt(name, nchars, &value, "%ld", ::sscanf);                  }

int RTL::CLI::getopt(const char* name, int nchars, float& value)    
{  return getopt(name, nchars, &value, "%f", ::sscanf);                   }

int RTL::CLI::getopt(const char* name, int nchars, double& value)   
{  return getopt(name, nchars, &value, "%f", ::sscanf);                   }

int RTL::CLI::getopt(const char* name, int nchars, std::string& value)    
{  return getopt(name, nchars, &value, "%s", ::string_assign);            }
