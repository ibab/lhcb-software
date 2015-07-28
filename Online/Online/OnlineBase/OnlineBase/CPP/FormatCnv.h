//============================================================================//
// Description:
//              This is set of classes and utilitities used to convert data
//              from different farmats. This is needed to exchange information 
//              between systems with different internal data representations.
//
//============================================================================//

#ifndef ONLINEKERNEL_CPP_FORMATCNV
#define ONLINEKERNEL_CPP_FORMATCNV 1

#include <cstdlib>

enum AtomicDataType {
  DATATYPE_INT,
  DATATYPE_SHORT,
  DATATYPE_CHAR,
  DATATYPE_FLOAT,
  DATATYPE_DOUBLE
};

enum DataFormat {
  DATAFORMAT_VMS,
  DATAFORMAT_OS9,
  DATAFORMAT_NETWORK
};

#ifdef OSK
#define LOCAL_DATAFORMAT DATAFORMAT_OS9
#else
#define LOCAL_DATAFORMAT DATAFORMAT_VMS
#endif

class DataStructure {
public:
  class DataElem {
  public:
    AtomicDataType type;
    int            length;
  };

private:
  DataElem* array;
  int       alloc_size;
  int       curr_size;  
public:
  explicit DataStructure( int = 8);
  explicit DataStructure( const char* s);
  DataStructure( const DataStructure& );
  virtual ~DataStructure();
  DataStructure& Add( AtomicDataType, int);
  DataStructure& Add( const char* );
  DataStructure& Add( const DataStructure& );
  DataStructure& Compress();
  DataStructure& operator =  ( const DataStructure& );  
  DataStructure& operator += ( const DataStructure& );
  DataStructure& operator += ( const char* );
  DataStructure& operator *= ( int );
  operator char* ();
  DataElem& operator[](int i) { return array[i]; }
  int     size() { return curr_size; }
  void    Dump();
  friend DataStructure operator + ( const DataStructure&, const DataStructure& );
  static void ConvertfromHost( void* buffer_out, const void* buffer_in, size_t buffer_size,
                               DataStructure& dstruct, DataFormat dformat );  
};

void host2net( void* buffer_out, void* buffer_in, size_t size, DataStructure& DataStructure);  
void net2host( void* buffer_out, void* buffer_in, size_t size, DataStructure& DataStructure);  

void ConverttoHost( void* buffer_out, const void* buffer_in, size_t buffer_size,
                    DataStructure& dstruct, DataFormat dformat );  
#endif /*  ONLINEKERNEL_CPP_FORMATCNV */
