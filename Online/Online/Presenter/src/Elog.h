// $Id: Elog.h,v 1.3 2010-03-23 12:51:41 ocallot Exp $
#ifndef ELOG_H 
#define ELOG_H 1

// Include files
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <string>
#include <iostream>
#include <vector>
/** @class Elog Elog.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2010-01-27
 */
class Elog {
public: 
#define MAX_ATTACHMENTS  50
  /// Standard constructor
  Elog( std::string name, int port ); 

  virtual ~Elog( ); ///< Destructor

  void setCredential( std::string name, std::string pass ) { m_username = name; m_password = pass; }

  void setLogbook( std::string name ) { m_logbook = name; }

  int  addAttachment( std::string file );

  void addAttribute ( std::string key, std::string value) {
    m_attributeKey.push_back( key );
    m_attributeValue.push_back( value );
  }

  int    submit( std::string text );

  void   url_encode(char *ps, int size);
 
protected:
  void   base64_encode(unsigned char *s, unsigned char *d, int size);
  size_t strlcpy(char *dst, const char *src, size_t size);
  void   do_crypt( const char *s, char *d, int size);
  void   stou(char *str);
  int    elog_connect( );
private:
  std::string m_hostname;
  int         m_port;
  std::string m_username;
  std::string m_password;
  std::string m_logbook;
  std::string m_text;
  int    m_nAttach;
  char * m_attachBuff[MAX_ATTACHMENTS];
  char   m_attachFile[MAX_ATTACHMENTS][256];
  int    m_attachSize[MAX_ATTACHMENTS];
  std::vector<std::string> m_attributeKey;
  std::vector<std::string> m_attributeValue;
};
#endif // ELOG_H
