// $Id: Elog.cpp,v 1.4 2010-10-01 12:52:26 frankb Exp $
// Include files
/********************************************************************\
This file is a stripped version of the original Elog, made c++;
  Name:         Elog.c
  Created by:   Stefan Ritt

  Contents:     Electronic logbook utility

  $Id: Elog.cpp,v 1.4 2010-10-01 12:52:26 frankb Exp $

\********************************************************************/

#ifndef __USE_XOPEN
#define __USE_XOPEN             /* needed for crypt() */
#endif

#ifdef WIN32
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
char *sha256_crypt(const char *key, const char *salt);

void Elog::do_crypt( const char *s, char *d, int size) {
  //#ifdef HAVE_CRYPT
  //strlcpy(d, crypt(s, "el"), size);
  //#else
  //base64_encode((unsigned char *) s, (unsigned char *) d, size);
  //#endif
  strlcpy(d, sha256_crypt(s, "$5$") + 4, size);
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
/* SHA256-based Unix crypt implementation.
   Released into the Public Domain by Ulrich Drepper <drepper@redhat.com>.
   Adapted for MS Windows by Stefan Ritt <stefan.ritt@psi.ch> 
   $Id: Elog.cpp,v 1.4 2010-10-01 12:52:26 frankb Exp $ */

#ifdef _MSC_VER
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX(x,y) ((x)>(y)?(x):(y))
#define MIN(x,y) ((x)<(y)?(x):(y))
#define __alignof__(x) sizeof(x)
#ifndef WIN32
#define alloca(x) malloc(x)
#endif
#define ERANGE 34

typedef unsigned int uint32_t;
#else
#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/types.h>
#endif

/* Structure to save state of computation between the single steps.  */
struct sha256_ctx {
   uint32_t H[8];

   uint32_t total[2];
   uint32_t buflen;
   char buffer[128];            /* NB: always correctly aligned for uint32_t.  */
};


#if __BYTE_ORDER == __LITTLE_ENDIAN
# define SWAP(n) \
    (((n) << 24) | (((n) & 0xff00) << 8) | (((n) >> 8) & 0xff00) | ((n) >> 24))
#else
# define SWAP(n) (n)
#endif


/* This array contains the bytes used to pad the buffer to the next
   64-byte boundary.  (FIPS 180-2:5.1.1)  */
static const unsigned char fillbuf[64] = { 0x80, 0 /* , 0, 0, ...  */  };


/* Constants for SHA256 from FIPS 180-2:4.2.2.  */
static const uint32_t K[64] = {
   0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
   0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
   0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
   0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
   0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
   0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
   0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
   0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
   0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
   0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
   0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
   0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
   0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
   0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
   0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
   0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};


/* Process LEN bytes of BUFFER, accumulating context into CTX.
   It is assumed that LEN % 64 == 0.  */
static void sha256_process_block(const void *buffer, size_t len, struct sha256_ctx *ctx)
{
  const uint32_t *words = (uint32_t*)buffer;
   size_t nwords = len / sizeof(uint32_t);
   uint32_t a = ctx->H[0];
   uint32_t b = ctx->H[1];
   uint32_t c = ctx->H[2];
   uint32_t d = ctx->H[3];
   uint32_t e = ctx->H[4];
   uint32_t f = ctx->H[5];
   uint32_t g = ctx->H[6];
   uint32_t h = ctx->H[7];

   /* First increment the byte count.  FIPS 180-2 specifies the possible
      length of the file up to 2^64 bits.  Here we only compute the
      number of bytes.  Do a double word increment.  */
   ctx->total[0] += len;
   if (ctx->total[0] < (unsigned int) len)
      ++ctx->total[1];

   /* Process all bytes in the buffer with 64 bytes in each round of
      the loop.  */
   while (nwords > 0) {
      uint32_t W[64];
      unsigned int t;
      uint32_t a_save = a;
      uint32_t b_save = b;
      uint32_t c_save = c;
      uint32_t d_save = d;
      uint32_t e_save = e;
      uint32_t f_save = f;
      uint32_t g_save = g;
      uint32_t h_save = h;

      /* Operators defined in FIPS 180-2:4.1.2.  */
#define Ch(x, y, z) ((x & y) ^ (~x & z))
#define Maj(x, y, z) ((x & y) ^ (x & z) ^ (y & z))
#define S0(x) (CYCLIC (x, 2) ^ CYCLIC (x, 13) ^ CYCLIC (x, 22))
#define S1(x) (CYCLIC (x, 6) ^ CYCLIC (x, 11) ^ CYCLIC (x, 25))
#define R0(x) (CYCLIC (x, 7) ^ CYCLIC (x, 18) ^ (x >> 3))
#define R1(x) (CYCLIC (x, 17) ^ CYCLIC (x, 19) ^ (x >> 10))

      /* It is unfortunate that C does not provide an operator for
         cyclic rotation.  Hope the C compiler is smart enough.  */
#define CYCLIC(w, s) ((w >> s) | (w << (32 - s)))

      /* Compute the message schedule according to FIPS 180-2:6.2.2 step 2.  */
      for (t = 0; t < 16; ++t) {
         W[t] = SWAP(*words);
         ++words;
      }
      for (t = 16; t < 64; ++t)
         W[t] = R1(W[t - 2]) + W[t - 7] + R0(W[t - 15]) + W[t - 16];

      /* The actual computation according to FIPS 180-2:6.2.2 step 3.  */
      for (t = 0; t < 64; ++t) {
         uint32_t T1 = h + S1(e) + Ch(e, f, g) + K[t] + W[t];
         uint32_t T2 = S0(a) + Maj(a, b, c);
         h = g;
         g = f;
         f = e;
         e = d + T1;
         d = c;
         c = b;
         b = a;
         a = T1 + T2;
      }

      /* Add the starting values of the context according to FIPS 180-2:6.2.2
         step 4.  */
      a += a_save;
      b += b_save;
      c += c_save;
      d += d_save;
      e += e_save;
      f += f_save;
      g += g_save;
      h += h_save;

      /* Prepare for the next round.  */
      nwords -= 16;
   }

   /* Put checksum in context given as argument.  */
   ctx->H[0] = a;
   ctx->H[1] = b;
   ctx->H[2] = c;
   ctx->H[3] = d;
   ctx->H[4] = e;
   ctx->H[5] = f;
   ctx->H[6] = g;
   ctx->H[7] = h;
}


/* Initialize structure containing state of computation.
   (FIPS 180-2:5.3.2)  */
static void sha256_init_ctx(struct sha256_ctx *ctx)
{
   ctx->H[0] = 0x6a09e667;
   ctx->H[1] = 0xbb67ae85;
   ctx->H[2] = 0x3c6ef372;
   ctx->H[3] = 0xa54ff53a;
   ctx->H[4] = 0x510e527f;
   ctx->H[5] = 0x9b05688c;
   ctx->H[6] = 0x1f83d9ab;
   ctx->H[7] = 0x5be0cd19;

   ctx->total[0] = ctx->total[1] = 0;
   ctx->buflen = 0;
}


/* Process the remaining bytes in the internal buffer and the usual
   prolog according to the standard and write the result to RESBUF.

   IMPORTANT: On some systems it is required that RESBUF is correctly
   aligned for a 32 bits value.  */
static void *sha256_finish_ctx(struct sha256_ctx *ctx, void *resbuf)
{
   /* Take yet unprocessed bytes into account.  */
   uint32_t bytes = ctx->buflen;
   size_t pad;
   unsigned int i;

   /* Now count remaining bytes.  */
   ctx->total[0] += bytes;
   if (ctx->total[0] < bytes)
      ++ctx->total[1];

   pad = bytes >= 56 ? 64 + 56 - bytes : 56 - bytes;
   memcpy(&ctx->buffer[bytes], fillbuf, pad);

   /* Put the 64-bit file length in *bits* at the end of the buffer.  */
   uint32_t tmp[2];
   tmp[1] = SWAP(ctx->total[0] << 3);
   tmp[0] = SWAP((ctx->total[1] << 3) | (ctx->total[0] >> 29));
   memcpy( &ctx->buffer[bytes+pad], tmp, 8 );

   /*
   *(uint32_t *) & ctx->buffer[bytes + pad + 4] = SWAP(ctx->total[0] << 3);
   *(uint32_t *) & ctx->buffer[bytes + pad] = SWAP((ctx->total[1] << 3) | (ctx->total[0] >> 29));
   */

   /* Process last bytes.  */
   sha256_process_block(ctx->buffer, bytes + pad + 8, ctx);

   /* Put result from CTX in first 32 bytes following RESBUF.  */
   for (i = 0; i < 8; ++i)
      ((uint32_t *) resbuf)[i] = SWAP(ctx->H[i]);

   return resbuf;
}


static void sha256_process_bytes(const void *buffer, size_t len, struct sha256_ctx *ctx)
{
   /* When we already have some bits in our internal buffer concatenate
      both inputs first.  */
   if (ctx->buflen != 0) {
      size_t left_over = ctx->buflen;
      size_t add = 128 - left_over > len ? len : 128 - left_over;

      memcpy(&ctx->buffer[left_over], buffer, add);
      ctx->buflen += add;

      if (ctx->buflen > 64) {
         sha256_process_block(ctx->buffer, ctx->buflen & ~63, ctx);

         ctx->buflen &= 63;
         /* The regions in the following copy operation cannot overlap.  */
         memcpy(ctx->buffer, &ctx->buffer[(left_over + add) & ~63], ctx->buflen);
      }

      buffer = (const char *) buffer + add;
      len -= add;
   }

   /* Process available complete blocks.  */
   if (len >= 64) {
/* To check alignment gcc has an appropriate operator.  Other
   compilers don't.  */
#if __GNUC__ >= 2
# define UNALIGNED_P(p) (((uintptr_t) p) % __alignof__ (uint32_t) != 0)
#else
# define UNALIGNED_P(p) (((uintptr_t) p) % sizeof (uint32_t) != 0)
#endif
      if (UNALIGNED_P(buffer))
         while (len > 64) {
            sha256_process_block(memcpy(ctx->buffer, buffer, 64), 64, ctx);
            buffer = (const char *) buffer + 64;
            len -= 64;
      } else {
         sha256_process_block(buffer, len & ~63, ctx);
         buffer = (const char *) buffer + (len & ~63);
         len &= 63;
      }
   }

   /* Move remaining bytes into internal buffer.  */
   if (len > 0) {
      size_t left_over = ctx->buflen;

      memcpy(&ctx->buffer[left_over], buffer, len);
      left_over += len;
      if (left_over >= 64) {
         sha256_process_block(ctx->buffer, 64, ctx);
         left_over -= 64;
         memcpy(ctx->buffer, &ctx->buffer[64], left_over);
      }
      ctx->buflen = left_over;
   }
}


/* Define our magic string to mark salt for SHA256 "encryption"
   replacement.  */
static const char sha256_salt_prefix[] = "$5$";

/* Prefix for optional rounds specification.  */
static const char sha256_rounds_prefix[] = "rounds=";

/* Maximum salt string length.  */
#define SALT_LEN_MAX 16
/* Default number of rounds if not explicitly specified.  */
#define ROUNDS_DEFAULT 5000
/* Minimum number of rounds.  */
#define ROUNDS_MIN 1000
/* Maximum number of rounds.  */
#define ROUNDS_MAX 999999999

/* Table with characters for base64 transformation.  */
static const char b64t[65] = "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";


static char *sha256_crypt_r(const char *key, const char *salt, char *buffer, int buflen)
{
#ifdef _MSC_VER
   unsigned char alt_result[32];
   unsigned char temp_result[32];
#else
   unsigned char alt_result[32]
       __attribute__ ((__aligned__(__alignof__(uint32_t))));
   unsigned char temp_result[32]
       __attribute__ ((__aligned__(__alignof__(uint32_t))));
#endif
   struct sha256_ctx ctx;
   struct sha256_ctx alt_ctx;
   size_t salt_len;
   size_t key_len;
   size_t cnt;
   char *cp;
   char *copied_key = NULL;
   char *copied_salt = NULL;
   char *p_bytes;
   char *s_bytes;
   /* Default number of rounds.  */
   size_t rounds = ROUNDS_DEFAULT;
   int rounds_custom = 0;

   /* Find beginning of salt string.  The prefix should normally always
      be present.  Just in case it is not.  */
   if (strncmp(sha256_salt_prefix, salt, sizeof(sha256_salt_prefix) - 1) == 0)
      /* Skip salt prefix.  */
      salt += sizeof(sha256_salt_prefix) - 1;

   if (strncmp(salt, sha256_rounds_prefix, sizeof(sha256_rounds_prefix) - 1)
       == 0) {
      const char *num = salt + sizeof(sha256_rounds_prefix) - 1;
      char *endp;
      unsigned long int srounds = strtoul(num, &endp, 10);
      if (*endp == '$') {
         salt = endp + 1;
         rounds = MAX(ROUNDS_MIN, MIN(srounds, ROUNDS_MAX));
         rounds_custom = 1;
      }
   }

   salt_len = MIN(strcspn(salt, "$"), SALT_LEN_MAX);
   key_len = strlen(key);

   if ((key - (char *) 0) % __alignof__(uint32_t) != 0) {
      char *tmp = (char *) alloca(key_len + __alignof__(uint32_t));
      key = copied_key = (char*)memcpy(tmp + __alignof__(uint32_t)
                                - (tmp - (char *) 0) % __alignof__(uint32_t), key, key_len);
   }

   if ((salt - (char *) 0) % __alignof__(uint32_t) != 0) {
      char *tmp = (char *) alloca(salt_len + __alignof__(uint32_t));
      salt = copied_salt = (char*)memcpy(tmp + __alignof__(uint32_t)
                                  - (tmp - (char *) 0) % __alignof__(uint32_t), salt, salt_len);
   }

   /* Prepare for the real work.  */
   sha256_init_ctx(&ctx);

   /* Add the key string.  */
   sha256_process_bytes(key, key_len, &ctx);

   /* The last part is the salt string.  This must be at most 16
      characters and it ends at the first `$' character (for
      compatibility with existing implementations).  */
   sha256_process_bytes(salt, salt_len, &ctx);


   /* Compute alternate SHA256 sum with input KEY, SALT, and KEY.  The
      final result will be added to the first context.  */
   sha256_init_ctx(&alt_ctx);

   /* Add key.  */
   sha256_process_bytes(key, key_len, &alt_ctx);

   /* Add salt.  */
   sha256_process_bytes(salt, salt_len, &alt_ctx);

   /* Add key again.  */
   sha256_process_bytes(key, key_len, &alt_ctx);

   /* Now get result of this (32 bytes) and add it to the other
      context.  */
   sha256_finish_ctx(&alt_ctx, alt_result);

   /* Add for any character in the key one byte of the alternate sum.  */
   for (cnt = key_len; cnt > 32; cnt -= 32)
      sha256_process_bytes(alt_result, 32, &ctx);
   sha256_process_bytes(alt_result, cnt, &ctx);

   /* Take the binary representation of the length of the key and for every
      1 add the alternate sum, for every 0 the key.  */
   for (cnt = key_len; cnt > 0; cnt >>= 1)
      if ((cnt & 1) != 0)
         sha256_process_bytes(alt_result, 32, &ctx);
      else
         sha256_process_bytes(key, key_len, &ctx);

   /* Create intermediate result.  */
   sha256_finish_ctx(&ctx, alt_result);

   /* Start computation of P byte sequence.  */
   sha256_init_ctx(&alt_ctx);

   /* For every character in the password add the entire password.  */
   for (cnt = 0; cnt < key_len; ++cnt)
      sha256_process_bytes(key, key_len, &alt_ctx);

   /* Finish the digest.  */
   sha256_finish_ctx(&alt_ctx, temp_result);

   /* Create byte sequence P.  */
   cp = p_bytes = (char*)alloca(key_len);
   for (cnt = key_len; cnt >= 32; cnt -= 32) {
      memcpy(cp, temp_result, 32);
      cp += 32;
   } 
   memcpy(cp, temp_result, cnt);

   /* Start computation of S byte sequence.  */
   sha256_init_ctx(&alt_ctx);

   /* For every character in the password add the entire password.  */
   for (cnt = 0; (int) cnt < 16 + alt_result[0]; ++cnt)
      sha256_process_bytes(salt, salt_len, &alt_ctx);

   /* Finish the digest.  */
   sha256_finish_ctx(&alt_ctx, temp_result);

   /* Create byte sequence S.  */
   cp = s_bytes = (char*)alloca(salt_len);
   for (cnt = salt_len; cnt >= 32; cnt -= 32) {
      memcpy(cp, temp_result, 32);
      cp += 32;
   }
   memcpy(cp, temp_result, cnt);

   /* Repeatedly run the collected hash value through SHA256 to burn
      CPU cycles.  */
   for (cnt = 0; cnt < rounds; ++cnt) {
      /* New context.  */
      sha256_init_ctx(&ctx);

      /* Add key or last result.  */
      if ((cnt & 1) != 0)
         sha256_process_bytes(p_bytes, key_len, &ctx);
      else
         sha256_process_bytes(alt_result, 32, &ctx);

      /* Add salt for numbers not divisible by 3.  */
      if (cnt % 3 != 0)
         sha256_process_bytes(s_bytes, salt_len, &ctx);

      /* Add key for numbers not divisible by 7.  */
      if (cnt % 7 != 0)
         sha256_process_bytes(p_bytes, key_len, &ctx);

      /* Add key or last result.  */
      if ((cnt & 1) != 0)
         sha256_process_bytes(alt_result, 32, &ctx);
      else
         sha256_process_bytes(p_bytes, key_len, &ctx);

      /* Create intermediate result.  */
      sha256_finish_ctx(&ctx, alt_result);
   }

   /* Now we can construct the result string.  It consists of three
      parts.  */
   strncpy(buffer, sha256_salt_prefix, MAX(0, buflen));
   cp = buffer + strlen(buffer);
   buflen -= sizeof(sha256_salt_prefix) - 1;

   if (rounds_custom) {
#ifdef _MSC_VER
      int n = _snprintf(cp, MAX(0, buflen), "%s%zu$",
                        sha256_rounds_prefix, rounds);
#else
      int n = ::snprintf(cp,MAX(0,buflen),"%s%lu$",sha256_rounds_prefix,(unsigned long)rounds);
#endif
      cp += n;
      buflen -= n;
   }

   strncpy(cp, salt, MIN((size_t) MAX(0, buflen), salt_len));
   cp = cp + strlen(cp);
   buflen -= MIN((size_t) MAX(0, buflen), salt_len);

   if (buflen > 0) {
      *cp++ = '$';
      --buflen;
   }
#define b64_from_24bit(B2, B1, B0, N)					      \
  do {									      \
    unsigned int w = ((B2) << 16) | ((B1) << 8) | (B0);			      \
    int n = (N);							      \
    while (n-- > 0 && buflen > 0)					      \
      {									      \
	*cp++ = b64t[w & 0x3f];						      \
	--buflen;							      \
	w >>= 6;							      \
      }									      \
  } while (0)

   b64_from_24bit(alt_result[0], alt_result[10], alt_result[20], 4);
   b64_from_24bit(alt_result[21], alt_result[1], alt_result[11], 4);
   b64_from_24bit(alt_result[12], alt_result[22], alt_result[2], 4);
   b64_from_24bit(alt_result[3], alt_result[13], alt_result[23], 4);
   b64_from_24bit(alt_result[24], alt_result[4], alt_result[14], 4);
   b64_from_24bit(alt_result[15], alt_result[25], alt_result[5], 4);
   b64_from_24bit(alt_result[6], alt_result[16], alt_result[26], 4);
   b64_from_24bit(alt_result[27], alt_result[7], alt_result[17], 4);
   b64_from_24bit(alt_result[18], alt_result[28], alt_result[8], 4);
   b64_from_24bit(alt_result[9], alt_result[19], alt_result[29], 4);
   b64_from_24bit(0, alt_result[31], alt_result[30], 3);
   if (buflen <= 0) {
      errno = ERANGE;
      buffer = NULL;
   } else
      *cp = '\0';               /* Terminate the string.  */

   /* Clear the buffer for the intermediate result so that people
      attaching to processes or reading core dumps cannot get any
      information.  We do it in this way to clear correct_words[]
      inside the SHA256 implementation as well.  */
   sha256_init_ctx(&ctx);
   sha256_finish_ctx(&ctx, alt_result);
   memset(temp_result, '\0', sizeof(temp_result));
   memset(p_bytes, '\0', key_len);
   memset(s_bytes, '\0', salt_len);
   memset(&ctx, '\0', sizeof(ctx));
   memset(&alt_ctx, '\0', sizeof(alt_ctx));
   if (copied_key != NULL)
      memset(copied_key, '\0', key_len);
   if (copied_salt != NULL)
      memset(copied_salt, '\0', salt_len);

   return buffer;
}


/* This entry point is equivalent to the `crypt' function in Unix
   libcs.  */
char *sha256_crypt(const char *key, const char *salt)
{
   /* We don't want to have an arbitrary limit in the size of the
      password.  We can compute an upper bound for the size of the
      result in advance and so we can prepare the buffer we pass to
      `sha256_crypt_r'.  */
   static char *buffer;
   static int buflen;
   int needed = (sizeof(sha256_salt_prefix) - 1
                 + sizeof(sha256_rounds_prefix) + 9 + 1 + strlen(salt) + 1 + 43 + 1);

   if (buflen < needed) {
      char *new_buffer = (char *) realloc(buffer, needed);
      if (new_buffer == NULL)
         return NULL;

      buffer = new_buffer;
      buflen = needed;
   }

   return sha256_crypt_r(key, salt, buffer, buflen);
}
//=============================================================================
