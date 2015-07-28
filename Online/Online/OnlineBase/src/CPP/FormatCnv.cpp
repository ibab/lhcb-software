//============================================================================
// Description:
//              This is set of classes and utilitities used to convert data
//              from different farmats. This is needed to exchange information 
//              between systems with different internal data representations.
//
//============================================================================
#include <cstdio>
#include <cstring>
#include "CPP/FormatCnv.h"

#define MIN( a, b ) (a)<(b) ? (a) : (b)
static void ConvertINTfromHost( void*, const void*, int, DataFormat );
static void ConvertSHORTfromHost( void*, const void*, int, DataFormat );

static inline void ConvertINTtoHost( void* out, const void* in, int s, DataFormat df)
{ ConvertINTfromHost( out, in, s, df ); } 
static inline void ConvertSHORTtoHost( void* out, const void* in, int s, DataFormat df)
{ ConvertSHORTfromHost( out, in, s, df ); } 
static inline void ConvertCHARtoHost( void* out, const void* in, int s, DataFormat)
{ memcpy( out, in, s ); }
static inline void ConvertCHARfromHost( void* out, const void* in, int s, DataFormat)
{ memcpy( out, in, s ); }

static void ConvertFLOATtoHost( void*, const void*, int, DataFormat);
static void ConvertFLOATfromHost( void*, const void*, int, DataFormat);
static void ConvertDOUBLEtoHost( void*, const void*, int, DataFormat);
static void ConvertDOUBLEfromHost( void*, const void*, int, DataFormat);
static void SwapBytes ( char*, char*, int);
static void SwapShorts( char*, char*, int);

//------------------------------------------------------------------------------
DataStructure::DataStructure( int size )
//------------------------------------------------------------------------------
{
  array = new DataElem[size];
  alloc_size = size;
  curr_size = 0;
}
//------------------------------------------------------------------------------
DataStructure::DataStructure( const DataStructure& f )  {
  array = new DataElem[f.alloc_size];
  alloc_size = f.alloc_size;
  curr_size  = f.curr_size;
  for (int i = 0; i < curr_size; i++ ) array[i] = f.array[i];  
}

//------------------------------------------------------------------------------
DataStructure::DataStructure( const char* s )  {
  array = new DataElem[8];
  alloc_size = 8;
  curr_size  = 0;
  Add( s );
}

//------------------------------------------------------------------------------
DataStructure::~DataStructure()   {
  delete [] array;
}

//------------------------------------------------------------------------------
DataStructure& DataStructure::Add( AtomicDataType type , int size ) {
  if( curr_size >= alloc_size )  {
    DataElem* newarray = new DataElem[ alloc_size + 8 ]; 
    for( int i = 0; i < alloc_size; i++ ) newarray[i] = array[i];
    alloc_size += 8;
    delete array;
    array = newarray;
  }
  array[curr_size].type   = type;
  array[curr_size].length = size;
  curr_size++;
  return *this;
}

//------------------------------------------------------------------------------
DataStructure& DataStructure::Add( const char* form ) {
  int number = 0;
  const char* n_first;
  const char* p_first;
  const char* p_last;

  for(const char* c = form; *c; c++)    { 
    if( strchr ( "0123456789", *c) )  {
      n_first = c;
      while( *(c+1) && strchr ( "0123456789", *(c+1)) ) c++;
      number = strtol (n_first, 0, 10);
    }
    else if( strchr ( "IDFDSCAidfdsca", *c) )  {
      AtomicDataType type;
      if( !number ) number = 1;
      switch( *c )  {
      case 'i':
      case 'I':
        type = DATATYPE_INT;
      break;
      case 's':
      case 'S':
        type = DATATYPE_SHORT;
      break;
      case 'a':
      case 'A':
      case 'c':
      case 'C':
        type = DATATYPE_CHAR;
      break;
      case 'f':
      case 'F':
        type = DATATYPE_FLOAT;
      break;
      case 'd':
      case 'D':
        type = DATATYPE_DOUBLE;
      break;
      default:
        type = DATATYPE_INT;
      }
      Add( type, number);
    }
    else if( strchr ( ",", *c) )    {
      number = 0;
    }
    else if( strchr ( "([", *c) )    {
      int   level = 1;
      char*   s_tmp;
      DataStructure  f_tmp;
      p_first = c + 1;
      p_last  = p_first + strlen(c);
      while( *(++c) )   {
        if( strchr( "])", *c) )  { 
          level--;
          if( level == 0)  {
            p_last = c;
            break;
          }
        }        
        if( strchr( "[(", *c) )  { 
          level++;
        }
      }
      if( level ) printf("Syntax error: Unbalanced parenthesis\n");
      s_tmp = new char[ p_last - p_first + 1];
      strncpy( s_tmp, p_first, p_last - p_first);
      s_tmp[p_last - p_first] = '\0';
      f_tmp.Add( s_tmp );
      f_tmp *= number;
      Add( f_tmp );
      number = 0;
      delete s_tmp;
    }
  }
  Compress();
  return *this;
}

//------------------------------------------------------------------------------
DataStructure& DataStructure::Add( const DataStructure& ds )  {
  if( ds.curr_size + curr_size > alloc_size )  {
    DataElem* newarray;
    alloc_size = ds.curr_size + curr_size;
    newarray = new DataElem[ alloc_size ];
    for(int i = 0; i < curr_size; i++ ) newarray[i] = array[i];
    delete [] array;
    array = newarray;
  }
  for(int i = 0; i < ds.curr_size; i++) array[i + curr_size] = ds.array[i];
  curr_size = curr_size + ds.curr_size;
  Compress();
  return *this;
}

//------------------------------------------------------------------------------
DataStructure& DataStructure::Compress( )   {
  for ( int i = 0; i < (curr_size - 1); i++)  {
    if( array[i].type == array[i+1].type )  {
      array[i].length += array[i+1].length;
      for(int j = i + 1; j < (curr_size - 1); j++ ) array[j] = array[j+1];
      curr_size--; 
      i--;
    }
  } 
  return *this;
}

//------------------------------------------------------------------------------
void DataStructure::Dump( )    {
  char f[] = "ISAFD";
  ::printf( "DataStructure = [");
  for (int i = 0; i < curr_size; i++ )  {
    ::printf("%d%c,", array[i].length, f[array[i].type]); 
  }
  ::printf( "]\n");
}

//------------------------------------------------------------------------------
DataStructure& DataStructure::operator = ( const DataStructure& f )   {
  if ( this != &f )  {
    if( f.curr_size > alloc_size )  {
      delete array;
      array = new DataElem[ f.alloc_size ];
      alloc_size = f.alloc_size;
    }
    curr_size = f.curr_size;
    for(int i = 0; i < curr_size; i++) array[i] = f.array[i];
  }
  return *this;
}
//------------------------------------------------------------------------------
DataStructure& DataStructure::operator += ( const DataStructure& ds ) {
  Add( ds );
  return *this;
}
//------------------------------------------------------------------------------
DataStructure& DataStructure::operator *= ( int factor )  {
  if( curr_size * factor > alloc_size )  {
    DataElem* newarray;
    alloc_size = curr_size * factor;
    newarray = new DataElem[ alloc_size ];
    for(int i = 0; i < curr_size; i++ ) newarray[i] = array[i];
    delete array;
    array = newarray;
  }
  for(int f = 0; f < (factor - 1); f++)  {
    for(int i = 0; i < curr_size; i++) array[ curr_size * (f + 1) + i ] = array[i];
  }
  curr_size = curr_size * factor;
  Compress();
  return *this;
}
//------------------------------------------------------------------------------
DataStructure::operator char* ( ) {
  static char* str = 0;
  char f[] = "IAFD";
  size_t len = curr_size*6;
  if( str ) delete str;
  str = new char[len];
  char* s = str;
  s += ::snprintf( s, len-(s-str),"(");
  for (int i = 0; i < curr_size; i++ )  {
    s += ::snprintf( s, len-(s-str), "%d%c,", array[i].length, f[array[i].type]); 
  }
  ::snprintf( s - 1, len-(s-str), ")");
  return str;
}

//------------------------------------------------------------------------------
DataStructure operator + ( const DataStructure& f1, const DataStructure& f2 ) {
  int i;
  DataStructure t( f1.curr_size + f2.curr_size );
  for( i = 0; i < f1.curr_size; i++) t.array[i] = f1.array[i];
  for( i = 0; i < f2.curr_size; i++) t.array[i + f1.curr_size] = f2.array[i];
  t.curr_size = f1.curr_size + f2.curr_size;
  t.Compress();
  return t;   
}

//------------------------------------------------------------------------------
void host2net( void* out_buffer, void* in_buffer, size_t size, DataStructure& format)  {
  DataStructure::ConvertfromHost( out_buffer, in_buffer, size, format, DATAFORMAT_NETWORK );
}

//------------------------------------------------------------------------------
void net2host( void* out_buffer, void* in_buffer, size_t size, DataStructure& format)  {
  ConverttoHost( out_buffer, in_buffer, size, format, DATAFORMAT_NETWORK );
}

//------------------------------------------------------------------------------
void DataStructure::ConvertfromHost( void* out_buffer, const void* in_buffer, size_t buffer_size,
                                     DataStructure& ds, DataFormat df) {
  int    i;
  const char*  in;
  char*  out;  
  size_t s     = buffer_size;
  int    nitem = ds.size();

  in  = (char*)in_buffer;
  out = (char*)out_buffer;

  while( s > 0 )  {
    for( i = 0; i < nitem ; i++)  {
      size_t m, n = ds[i].length;
      switch ( ds[i].type )      {
      case DATATYPE_INT:
        m = MIN( n * sizeof(int) , s );
        ConvertINTfromHost( out, in, m, df );
        break;
      case DATATYPE_SHORT:
        m = MIN( n * sizeof(short), s);
        ConvertSHORTfromHost( out, in, m, df );
        break;
      case DATATYPE_CHAR:
        m = MIN( n, s);
        ConvertCHARfromHost (out, in, m, df);
        break;
      case DATATYPE_FLOAT:
        m = MIN( n * sizeof(float) , s );
        ConvertFLOATfromHost (out, in, m, df);
        break;
      case DATATYPE_DOUBLE:
        m = MIN( n * sizeof(double), s );
        ConvertDOUBLEfromHost (out, in, m, df);
        break;
      default:
        m = 0;
        break;
      }
      s   -= m;
      in  += m;
      out += m;
    }
  }
}

//------------------------------------------------------------------------------
void ConverttoHost( void* out_buffer, const void* in_buffer, size_t buffer_size,
                    DataStructure& ds, DataFormat df)
{
  int    i;
  const char*  in;
  char*  out;  
  size_t s     = buffer_size;
  int    nitem = ds.size();

  in  = (char*)in_buffer;
  out = (char*)out_buffer;

  while( s > 0 )  {
    for( i = 0; i < nitem ; i++)  {
      size_t m, n = ds[i].length;
      switch ( ds[i].type )  {
      case DATATYPE_INT:
        m = MIN( n * sizeof(int) , s );
        ConvertINTtoHost( out, in, m, df );
        break;
      case DATATYPE_SHORT:
        m = MIN( n * sizeof(short), s );
        ConvertSHORTtoHost( out, in, m, df );
        break;
      case DATATYPE_CHAR:
        m = MIN( n, s);
        ConvertCHARtoHost (out, in, m, df);
        break;
      case DATATYPE_FLOAT:
        m = MIN( n * sizeof(float), s );
        ConvertFLOATtoHost (out, in, m, df);
        break;
      case DATATYPE_DOUBLE:
        m = MIN( n * sizeof(double), s );
        ConvertDOUBLEtoHost (out, in, m, df);
        break;
      default:
        m = 0;
        break;
      }
      s   -= m;
      in  += m;
      out += m;
    }
  }
}

//------------------------------------------------------------------------------
void ConvertINTfromHost( void* out, const void* in, int s, DataFormat df )  {
  switch( df )  {
  case DATAFORMAT_OS9:
  case DATAFORMAT_VMS:
    memcpy( out, in, s);
    break;
  default:
    SwapBytes( (char*)out, (char*)in, s);
    break;
  }
}
//------------------------------------------------------------------------------
void ConvertSHORTfromHost( void* out, const void* in, int s, DataFormat df )
//------------------------------------------------------------------------------
{
  switch( df )  {
  case DATAFORMAT_OS9:
  case DATAFORMAT_VMS:
    memcpy( out, in, s);
    break;
  default:
    SwapShorts( (char*)out, (char*)in, s);
    break;
  }
}

#ifdef __VMS
#include cvtdef
extern "C" unsigned long cvt$convert_float( const void* in, int in_code,
                                            void* out, int out_code, int opt);
#endif

//------------------------------------------------------------------------------
#if defined(__VMS)
void ConvertFLOATtoHost( void* out, const void* in, int s, DataFormat df)   {
  int i, n;
  switch( df )  {
  case DATAFORMAT_OS9:
  case DATAFORMAT_NETWORK:
    n = s/sizeof(float);
    SwapBytes( (char*)out, (char*)in, s);
    for( i = 0; i < n; i++)  {
      cvt$convert_float((float*)out+i,CVT$K_IEEE_S,(float*)out+i,CVT$K_VAX_F,0);
    }
    break;
  default:
    memcpy( out, in, s );
    break;
  }
#elif defined(_WIN32) || defined(__linux)
  void ConvertFLOATtoHost(void*, const void*, int, DataFormat)   {
#elif defined(_OSK)
    void ConvertFLOATtoHost(void*, const void*, int, DataFormat)   {
      // not yet implemented
#endif
    }

    //-----------------------------------------------------------------------------
#if defined(__VMS)
    void ConvertFLOATfromHost(void* out, const void* in, int s, DataFormat df)   {
      int i, n;
      switch( df )  {
      case DATAFORMAT_OS9:
      case DATAFORMAT_NETWORK:
        n = s/sizeof(float);
        for( i = 0; i < n; i++) {
          cvt$convert_float((float*)in+i,CVT$K_VAX_F,(float*)out+i,CVT$K_IEEE_S,0);
        }
        SwapBytes( (char*)out, (char*)out, s);
        break;
      default:
        memcpy( out, in, s );
        break;
      }
#elif defined(_WIN32) || defined(__linux)
      void ConvertFLOATfromHost(void*, const void*, int, DataFormat)   {
#elif defined(_OSK)
        void ConvertFLOATfromHost(void*, const void*, int, DataFormat)   {
          // not yet implemented
#endif
        }

        //-----------------------------------------------------------------------------
#if defined(__VMS)
        void ConvertDOUBLEtoHost( void* out, const void* in, int s, DataFormat df)  {
          int i, n;
          switch( df )  {
          case DATAFORMAT_OS9:
          case DATAFORMAT_NETWORK:
            n = s/sizeof(double);
            SwapBytes( (char*)out, (char*)in, s);
            for( i = 0; i < n; i++) {
              cvt$convert_float((double*)out+i,CVT$K_IEEE_T,(double*)out+i,CVT$K_VAX_D,0);
              break;
            default:
              memcpy( out, in, s );
              break;
            }
#elif defined(_WIN32) || defined(__linux)
            void ConvertDOUBLEtoHost(void*, const void*, int, DataFormat)  {
#elif defined(_OSK)
              void ConvertDOUBLEtoHost(void*, const void*, int, DataFormat)  {
                // not yet implemented
#endif
              }

              //----------------------------------------------------------------------------
#ifdef __VMS
              void ConvertDOUBLEfromHost( void* out, const void* in, int s, DataFormat df) {
                int i, n;
                switch( df )  {
                case DATAFORMAT_OS9:
                case DATAFORMAT_NETWORK:
                  n = s/sizeof(double);
                  for( i = 0; i < n; i++) cvt$convert_float( (double*)in + i, CVT$K_VAX_D,
                                                             (double*)out + i, CVT$K_IEEE_T,0);
                  SwapBytes((char*)out, (char*)out, s);
                  break;
                default:
                  memcpy( out, in, s );
                  break;
                }
#elif defined(_WIN32) || defined(__linux)
                void ConvertDOUBLEfromHost(void*, const void*, int, DataFormat)  {
#elif defined(_OSK)
                  void ConvertDOUBLEfromHost(void*, const void*, int, DataFormat)  {
                    // not yet implemented
#endif
                  }

                  //------------------------------------------------------------------------------
                  void SwapBytes( char* out, char* in, int n)  {
                    char c;
                    while( n > 0 )  {
                      c = in[0];
                      out[0] = in[3];
                      out[3] = c;    
                      c = in[1];
                      out[1] = in[2];
                      out[2] = c;    
                      in   += sizeof(int);
                      out  += sizeof(int);
                      n    -= sizeof(int);
                    }
                  }
                  //------------------------------------------------------------------------------
                  void SwapShorts( char* out, char* in, int n)    {
                    char c;
                    while( n > 0 )  {
                      c = in[0];
                      out[0] = in[1];
                      out[1] = c;    
                      in   += sizeof(short);
                      out  += sizeof(short);
                      n    -= sizeof(short);
                    }
                  }
