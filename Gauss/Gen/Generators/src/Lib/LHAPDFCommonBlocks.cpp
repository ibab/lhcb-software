// $Id: LHAPDFCommonBlocks.cpp,v 1.1 2005-12-07 23:01:32 robbep Exp $
// access LHAPDF common LHASILENT
#include "Generators/LHAPDFCommonBlocks.h"

extern "C" {
#ifdef WIN32
  void __stdcall GETLHAPARM( int * , char * , int ) ;
  void __stdcall SETLHAPARM( int * , char * , int ) ;
  void __stdcall GETLHAVALUE( int * , double * ) ;
  void __stdcall SETLHAVALUE( int * , double * ) ;
#else 
  void getlhaparm_( int * , char * , int ) ;
  void setlhaparm_( int * , char * , int ) ;
  void getlhavalue_( int * , double * ) ;
  void setlhavalue_( int * , double * ) ;
#endif
}

// Constructor
Lhacontrol::Lhacontrol(): m_dummystr( "" ) ,  m_dummy( -999. ) { }

// Destructor
Lhacontrol::~Lhacontrol() { }

// access lhaparm in common
void Lhacontrol::setlhaparm( int n , const std::string & value ) {
  //  char * arg = new char[ value.length() + 1 ] ;
  char * arg = new char[ 3 ] ;
  strcpy( arg , value.c_str() ) ;
  if ( n < 1 || n > lenlhaparm() ) return ;
#ifdef WIN32
  SETLHAPARM( & n , arg , strlen( arg ) ) ;
#else
  setlhaparm_( & n , arg , strlen( arg ) ) ;
#endif
  delete arg ;
}

void Lhacontrol::getlhaparm( int n , std::string & value ) {
  if ( n < 1 || n > lenlhaparm() ) {
    value = m_dummystr ;
    return ;
  }
  char * arg = new char[ 20 ] ;
#ifdef WIN32
  GETLHAPARM( &n , arg , strlen( arg ) ) ;
#else
  getlhaparm_( &n , arg , strlen( arg ) ) ;
#endif
  value = arg ;
  delete arg ;
}

// access lhavalue in common
void Lhacontrol::setlhavalue( int n , double value ) {
  if ( n < 1 || n > lenlhaparm() ) return ;
#ifdef WIN32
  SETLHAVALUE( &n , &value ) ;
#else
  setlhavalue_( &n , &value ) ;
#endif
}

void Lhacontrol::getlhavalue( int n , double & value ) {
  if ( n < 1 || n > lenlhaparm() ) {
    value = m_dummy ;
    return ;
  }  
#ifdef WIN32
  GETLHAVALUE( &n , &value ) ;
#else
  getlhavalue_( &n , &value ) ;
#endif
}

