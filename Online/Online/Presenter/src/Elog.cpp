// $Id: Elog.cpp,v 1.2 2010-02-09 07:07:39 ocallot Exp $
// Include files
/********************************************************************\
This file is a stripped version of the original Elog, made c++;
  Name:         Elog.c
  Created by:   Stefan Ritt

  Contents:     Electronic logbook utility

  $Id: Elog.cpp,v 1.2 2010-02-09 07:07:39 ocallot Exp $

\********************************************************************/

#ifndef __USE_XOPEN
#define __USE_XOPEN             /* needed for crypt() */
#endif

#ifdef _MSC_VER
#include <windows.h>
#include <io.h>
#else
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#define closesocket(s) close(s)
#ifndef O_BINARY
#define O_BINARY 0
#endif
#endif
// local
#include "Elog.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Elog
//
// 2010-01-27 : Olivier Callot
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Elog::Elog( std::string name, int port ) {
  m_nAttach  = 0;
  m_hostname = name; 
  m_port     = port;
  
#if defined( _MSC_VER )
  {
    WSADATA WSAData;
    /* Start windows sockets */
    WSAStartup(MAKEWORD(1, 1), &WSAData);
  }
#endif
}
//=============================================================================
// Destructor
//=============================================================================
Elog::~Elog() {}
//=========================================================================
//  Add an attachement
//=========================================================================
int Elog::addAttachment ( std::string filename ) {
  if ( MAX_ATTACHMENTS == m_nAttach ) {
    std::cout << "Too many attachments : " << m_nAttach << std::endl;
    return 0;
  }

  int i = m_nAttach;
  m_nAttach++;
  strcpy( m_attachFile[i], filename.c_str() );
  
  int fh = open( filename.c_str(), O_RDONLY | O_BINARY);
  if (fh < 0) {
    std::cout << "ELOG: Attachment file \"" << filename << "\" does not exist." << std::endl;
    return 0;
  }

  m_attachSize[i] = lseek(fh, 0, SEEK_END);
  lseek(fh, 0, SEEK_SET);

  m_attachBuff[i] = (char*)malloc( m_attachSize[i] + 1);

  int n = read(fh, m_attachBuff[i], m_attachSize[i]);
  if (n < m_attachSize[i]) {
    std::cout << "ELOG: Cannot fully read attachment file \"" << filename << "\"." << std::endl;
  }
  m_attachBuff[i][n] = 0;

  close(fh);
  return 1;
}

//=========================================================================
//  Code with base64 convention.
//=========================================================================
void Elog::base64_encode(unsigned char *s, unsigned char *d, int size) {
  unsigned int t, pad;
  unsigned char *p;
  const char *map = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

  pad = 3 - strlen((char *) s) % 3;
  if (pad == 3)
    pad = 0;
  p = d;
  while (*s) {
    t = (*s++) << 16;
    if (*s)
      t |= (*s++) << 8;
    if (*s)
      t |= (*s++) << 0;

    *(d + 3) = map[t & 63];
    t >>= 6;
    *(d + 2) = map[t & 63];
    t >>= 6;
    *(d + 1) = map[t & 63];
    t >>= 6;
    *(d + 0) = map[t & 63];

    d += 4;
    if (d - p >= size - 3)
      return;
  }
  *d = 0;
  while (pad--)
    *(--d) = '=';
}
//=========================================================================
//  Copy src to string dst of size siz.  At most siz-1 characters
// will be copied.  Always NUL terminates (unless size == 0).
// Returns strlen(src); if retval >= siz, truncation occurred.
//=========================================================================
size_t Elog::strlcpy(char *dst, const char *src, size_t size) {
  char *d = dst;
  const char *s = src;
  size_t n = size;

  /* Copy as many bytes as will fit */
  if (n != 0 && --n != 0) {
    do {
      if ((*d++ = *s++) == 0)
        break;
    } while (--n != 0);
  }

  /* Not enough room in dst, add NUL and traverse rest of src */
  if (n == 0) {
    if (size != 0)
      *d = '\0';             /* NUL-terminate dst */
    while (*s++);
  }

  return (s - src - 1);        /* count does not include NUL */
}
//=========================================================================
//  Crypt if available, for the password
//=========================================================================
void Elog::do_crypt( const char *s, char *d, int size) {
#ifdef HAVE_CRYPT
  strlcpy(d, crypt(s, "el"), size);
#else
  base64_encode((unsigned char *) s, (unsigned char *) d, size);
#endif
}

//=========================================================================
//  convert all special characters to underscores in a string
//=========================================================================
void Elog::stou(char *str) {
  int i;

  for (i = 0; i < (int) strlen(str); i++)
    if (str[i] == ' ' || str[i] == '.' || str[i] == '/' ||
        str[i] == '\\' || str[i] == '-' || str[i] == '(' || str[i] == ')')
      str[i] = '_';
}

//=========================================================================
//  Encode the given string in-place by adding %XX escapes
//=========================================================================
void Elog::url_encode(char *ps, int size) {
  unsigned char *pd, *p;
  unsigned char str[500];

  pd = (unsigned char *) str;
  p = (unsigned char *) ps;
  while (*p && pd < str + 250) {
    if (strchr("%&=#?+", *p) || *p > 127) {
      sprintf((char *) pd, "%%%02X", *p);
      pd += 3;
      p++;
    } else if (*p == ' ') {
      *pd++ = '+';
      p++;
    } else {
      *pd++ = *p++;
    }
  }
  *pd = '\0';
  strlcpy(ps, (char *) str, size);
}
//=========================================================================
//  Connect to host-port
//=========================================================================
int Elog::elog_connect( ) {
  int status, sock;
  struct hostent *phe;
  struct sockaddr_in bind_addr;

  /* create socket */
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("cannot create socket");
    return -1;
  }

  /* compose remote address */
  memset(&bind_addr, 0, sizeof(bind_addr));
  bind_addr.sin_family = AF_INET;
  bind_addr.sin_addr.s_addr = 0;
  bind_addr.sin_port = htons((unsigned short) m_port);

  phe = gethostbyname( m_hostname.c_str() );
  if (phe == NULL) {
    perror("cannot get host name");
    return -1;
  }
  memcpy((char *) &(bind_addr.sin_addr), phe->h_addr, phe->h_length);

  /* connect to server */
  status = connect(sock, (const sockaddr*)&bind_addr, sizeof(bind_addr));
  if (status != 0) {
    printf("Cannot connect to host %s, port %d\n", m_hostname.c_str(), m_port );
    return -1;
  }
  return sock;
}

//=========================================================================
//  Submit the entry, passing the text as argument.
//=========================================================================
int Elog::submit( std::string text ) {
  int sock, i, n, header_length, content_length;
  char boundary[80], str[80], encrypted_passwd[256], *p;
  //struct hostent *phe;

  char request[100000], response[100000], *content;


  sock = elog_connect( );
  if (sock < 0) return -1;

  content_length = 100000;
  for (i = 0; i < m_nAttach; i++) content_length += m_attachSize[i];
  content = (char*)malloc(content_length);
  if (content == NULL) {
    printf("Not enough memory\n");
    return -1;
  }

  /* compose content */
  srand((unsigned) time(NULL));
  sprintf(boundary, "---------------------------%04X%04X%04X", rand(), rand(), rand());

  strcpy(content, boundary);
  strcat(content, "\r\nContent-Disposition: form-data; name=\"cmd\"\r\n\r\nSubmit\r\n");

  sprintf(content + strlen(content),
          "%s\r\nContent-Disposition: form-data; name=\"unm\"\r\n\r\n%s\r\n", boundary, m_username.c_str());

  do_crypt( m_password.c_str(), encrypted_passwd, sizeof(encrypted_passwd));
  sprintf(content + strlen(content),
          "%s\r\nContent-Disposition: form-data; name=\"upwd\"\r\n\r\n%s\r\n", boundary, encrypted_passwd);

  sprintf(content + strlen(content),
          "%s\r\nContent-Disposition: form-data; name=\"exp\"\r\n\r\n%s\r\n", boundary, m_logbook.c_str());

  sprintf(content + strlen(content),
          "%s\r\nContent-Disposition: form-data; name=\"encoding\"\r\n\r\nplain\r\n", boundary);
  for ( unsigned int kk = 0; kk < m_attributeKey.size(); kk++) {
    strcpy(str, m_attributeKey[kk].c_str() );
    stou(str);
    sprintf( content + strlen(content),
             "%s\r\nContent-Disposition: form-data; name=\"%s\"\r\n\r\n%s\r\n", boundary, str, m_attributeValue[kk].c_str() );
  }

  sprintf(content + strlen(content),
          "%s\r\nContent-Disposition: form-data; name=\"Text\"\r\n\r\n%s\r\n%s\r\n",
          boundary, text.c_str(), boundary);

  content_length = strlen(content);
  p = content + content_length;

  for (i = 0; i < m_nAttach; i++) {
    sprintf( p,
             "Content-Disposition: form-data; name=\"attfile%d\"; filename=\"%s\"\r\n\r\n",
             i + 1, m_attachFile[i] );

    content_length += strlen(p);
    p += strlen(p);
    memcpy(p, m_attachBuff[i], m_attachSize[i]);
    p += m_attachSize[i];
    strcpy(p, boundary);
    strcat(p, "\r\n");

    content_length += m_attachSize[i] + strlen(p);
    p += strlen(p);
  }

  /* compose request */
  strcpy(request, "POST /");
  strcpy(str, m_logbook.c_str());
  url_encode(str, sizeof(str));
  sprintf(request + strlen(request), "%s/", str);

  strcat(request, " HTTP/1.0\r\n");

  sprintf(request + strlen(request), "Content-Type: multipart/form-data; boundary=%s\r\n", boundary);
  sprintf(request + strlen(request), "Host: %s\r\n", m_hostname.c_str() );
  sprintf(request + strlen(request), "User-Agent: ELOG\r\n");
  sprintf(request + strlen(request), "Content-Length: %d\r\n", content_length);

  strcat(request, "\r\n");

  header_length = strlen(request);

  /* send request */
  send(sock, request, header_length, 0);

  /* send content */
  send(sock, content, content_length, 0);

  /* receive response */
  memset(response, 0, sizeof(response));
  i = recv(sock, response, sizeof(response) - 1, 0);
  if (i < 0) {
    perror("Cannot receive response");
    return -1;
  }

  /* discard remainder of response */
  n = i;
  while (i > 0) {
    i = recv(sock, response + n, sizeof(response) - 1 - n, 0);
    if (i > 0)
      n += i;
  }
  response[n] = 0;

  closesocket(sock);

  int entryNumber = 0;

  /* check response status */
  if (strstr(response, "302 Found")) {
    if (strstr(response, "Location:")) {
      if (strstr(response, "has moved"))
        printf("Error: elogd server has moved to another location\n");
      else if (strstr(response, "fail"))
        printf("Error: Invalid user name or password\n");
      else {
        strncpy(str, strstr(response, "Location:") + 10, sizeof(str));
        if (strchr(str, '?' )) *strchr(str, '?' ) = 0;
        if (strchr(str, '\n')) *strchr(str, '\n') = 0;
        if (strchr(str, '\r')) *strchr(str, '\r') = 0;
        char* p = str;
        if (strrchr(str, '/')) p = strrchr(str, '/')+1;

        printf("Message successfully transmitted, ID=%s\n", p);
        sscanf( p, "%d", &entryNumber );
      }
    } else
      printf("Message successfully transmitted\n");
  } else if (strstr(response, "Logbook Selection"))
    printf("Error: No logbook specified\n");
  else if (strstr(response, "enter password"))
    printf("Error: Missing or invalid password\n");
  else if (strstr(response, "form name=form1"))
    printf("Error: Missing or invalid user name/password\n");
  else if (strstr(response, "Error: Attribute")) {
    if (strstr(response, "not existing")) {
      strncpy(str, strstr(response, "Error: Attribute") + 27, sizeof(str));
      if (strchr(str, '<'))
        *strchr(str, '<') = 0;
      printf("Error: Non existing attribute option \"%s\"\n", str);
    } else {
      strncpy(str, strstr(response, "Error: Attribute") + 20, sizeof(str));
      if (strchr(str, '<'))
        *strchr(str, '<') = 0;
      printf("Error: Missing required attribute \"%s\"\n", str);
    }
  } else
    printf("Error transmitting message\n");

  return entryNumber;
}
//=============================================================================
