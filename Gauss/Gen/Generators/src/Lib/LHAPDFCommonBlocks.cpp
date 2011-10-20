// $Id: LHAPDFCommonBlocks.cpp,v 1.4 2009-10-22 16:43:05 robbep Exp $
// access LHAPDF common LHASILENT
#include "Generators/LHAPDFCommonBlocks.h"
#include <cstring>

extern "C" {
#ifdef WIN32
  void __stdcall GGETLHAPARM( int * , char * , int ) ;
  void __stdcall GSETLHAPARM( int * , char * , int ) ;
  void __stdcall GGETLHAVALUE( int * , double * ) ;
  void __stdcall GSETLHAVALUE( int * , double * ) ;
#else 
  void ggetlhaparm_( int * , char * , int ) ;
  void gsetlhaparm_( int * , char * , int ) ;
  void ggetlhavalue_( int * , double * ) ;
  void gsetlhavalue_( int * , double * ) ;
#endif
}

// Constructor
Lhacontrol::Lhacontrol(): m_dummystr( "" ) ,  m_dummy( -999. ) { }

// Destructor
Lhacontrol::~Lhacontrol() { }

// access lhaparm in common
void Lhacontrol::setlhaparm( int n , const std::string & value ) {
  char * arg = new char[ 20 ] ; 
  strncpy( arg , value.c_str() , value.size() ) ;
  if ( n < 1 || n > lenlhaparm() ) { 
    delete [] arg ;
    return ; 
  }
#ifdef WIN32
  GSETLHAPARM( & n , arg , strlen( arg ) ) ;
#else
  gsetlhaparm_( & n , arg , strlen( arg ) ) ;
#endif
  delete [] arg ;
}

void Lhacontrol::getlhaparm( int n , std::string & value ) {
  if ( n < 1 || n > lenlhaparm() ) {
    value = m_dummystr ;
    return ;
  }
  char * arg = new char[ 20 ] ;
#ifdef WIN32
  GGETLHAPARM( &n , arg , strlen( arg ) ) ;
#else
  ggetlhaparm_( &n , arg , strlen( arg ) ) ;
#endif
  value = arg ;
  delete [] arg ;
}

// access lhavalue in common
void Lhacontrol::setlhavalue( int n , double value ) {
  if ( n < 1 || n > lenlhaparm() ) return ;
#ifdef WIN32
  GSETLHAVALUE( &n , &value ) ;
#else
  gsetlhavalue_( &n , &value ) ;
#endif
}

void Lhacontrol::getlhavalue( int n , double & value ) {
  if ( n < 1 || n > lenlhaparm() ) {
    value = m_dummy ;
    return ;
  }  
#ifdef WIN32
  GGETLHAVALUE( &n , &value ) ;
#else
  ggetlhavalue_( &n , &value ) ;
#endif
}

