// $Id: Pythia.cpp,v 1.2 2005-11-04 11:00:09 robbep Exp $
// Include files

// local
#include "LbPythia/Pythia.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Pythia
//
// 2005-08-19 : Patrick Robbe
//-----------------------------------------------------------------------------

Pysubs Pythia::s_pysubs ; 
Pydat1 Pythia::s_pydat1 ; 
Pypars Pythia::s_pypars ; 
Pydat2 Pythia::s_pydat2 ; 
Pydat3 Pythia::s_pydat3 ; 
Pydatr Pythia::s_pydatr ; 
Pyint2 Pythia::s_pyint2 ;
Pymssm Pythia::s_pymssm ;

// PDFLIB_INIT Fortran function
extern "C" {
#ifdef WIN32
  void __stdcall PDFLIB_INIT( ) ;
#else
  void pdflib_init_( ) ;
#endif
}

void Pythia::PdfLib_Init() {
#ifdef WIN32
  PDFLIB_INIT( ) ;
#else
  pdflib_init_ ( ) ;
#endif
}

// SETUSERPROCESS Fortran function
extern "C" {
#ifdef WIN32
  void __stdcall SETUSERPROCESS( int * ) ;
#else
  void setuserprocess_( int * ) ;
#endif
}

void Pythia::SetUserProcess( int val ) {
#ifdef WIN32
  SETUSERPROCESS( &val ) ;
#else
  setuserprocess_( &val ) ;
#endif
}

// SETBEAM Fortran function
extern "C" {
#ifdef WIN32
  void __stdcall SETBEAM( double *, double *, double *, double *, double *, 
                          double *) ;
#else
  void setbeam_( double *, double *, double *, double *, double *, double * ) ;
#endif
}

void Pythia::SetBeam( double p1x, double p1y, double p1z, 
              double p2x, double p2y, double p2z ) {
#ifdef WIN32
  SETBEAM( &p1x, &p1y, &p1z, &p2x, &p2y, &p2z ) ;
#else
  setbeam_( &p1x, &p1y, &p1z, &p2x, &p2y, &p2z ) ;
#endif
}

// INITPYBLOCK Fortran function
extern "C" {
#ifdef WIN32
  void __stdcall INITPYBLOCK( int *, const char *, int ) ;
#else
  void initpyblock_ ( int *, const char *, int ) ;
#endif
}

void Pythia::InitPyBlock( int unit , const std::string fileName ) {
  const char * name = fileName.c_str() ;
#ifdef WIN32
  INITPYBLOCK( &unit, name , strlen( name ) ) ;
#else
  initpyblock_( &unit, name, strlen( name ) ) ;
#endif
}

// PYINIT Fortran function
extern "C" {
#ifdef WIN32
  void __stdcall PYINIT( const char*, int, const char*, int, const char*, int,
                         double *) ;
#else
  void pyinit_ (const char*, const char*, const char*, double*, int, int, int);
#endif
}

void Pythia::PyInit( const std::string frame , const std::string beam ,
                     const std::string target , double win ) {
  const char * cframe  = frame.c_str() ;
  const char * cbeam   = beam.c_str() ;
  const char * ctarget = target.c_str() ;
#ifdef WIN32
  PYINIT( cframe , strlen( cframe ) , cbeam , strlen( cbeam ) , ctarget , 
          strlen( ctarget ) , &win ) ;
#else
  pyinit_ ( cframe, cbeam, ctarget, &win, strlen( cframe ), strlen( cbeam ) ,
            strlen( ctarget ) ) ;
#endif
}

// PYLIST Fortran function
extern "C" {
#ifdef WIN32
  void __stdcall PYLIST( int * ) ;
#else
  void pylist_ ( int * ) ;
#endif
}

void Pythia::PyList( int val ) {
#ifdef WIN32
  PYLIST( &val ) ;
#else
  pylist_ ( &val ) ;
#endif
}


// PYEVNT Fortran function
extern "C" {
#ifdef WIN32
  void __stdcall PYEVNT( ) ;
#else
  void pyevnt_ ( ) ;
#endif
}

void Pythia::PyEvnt( ) {
#ifdef WIN32
  PYEVNT( ) ;
#else
  pyevnt_ ( ) ;
#endif
}

// LUNHEP Fortran function
extern "C" {
#ifdef WIN32
  int __stdcall LUNHEP( int * ) ;
#else
  int lunhep_ ( int * ) ;
#endif
}

int Pythia::LunHep( int val ) {
#ifdef WIN32
  return LUNHEP( &val ) ;
#else
  return lunhep_ ( &val ) ;
#endif
}

// PYCOMP Fortran function
extern "C" {
#ifdef WIN32
  int __stdcall PYCOMP( int * ) ;
#else
  int pycomp_ ( int * ) ;
#endif
}

int Pythia::PyComp( int val ) {
#ifdef WIN32
  return PYCOMP( &val ) ;
#else
  return pycomp_ ( &val ) ;
#endif
}

// PDFLIB_END Fortran function
extern "C" {
#ifdef WIN32
  void __stdcall PDFLIB_END( ) ;
#else
  void pdflib_end_ ( ) ;
#endif
}

void Pythia::PdfLib_End( ) {
#ifdef WIN32
  PDFLIB_END( ) ;
#else
  pdflib_end_ ( ) ;
#endif
}

// PYSTAT Fortran function
extern "C" {
#ifdef WIN32
  void __stdcall PYSTAT( int * ) ;
#else
  void pystat_ ( int * ) ;
#endif
}

void Pythia::PyStat( int val ) {
#ifdef WIN32
  PYSTAT( &val ) ;
#else
  pystat_ ( &val ) ;
#endif
}

// PYEDIT Fortran function
extern "C" {
#ifdef WIN32
  void __stdcall PYEDIT( int * ) ;
#else
  void pyedit_ ( int * ) ;
#endif
} 

void Pythia::PyEdit( int val ) {
#ifdef WIN32
  PYEDIT( &val ) ;
#else
  pyedit_ ( &val ) ;
#endif
}

// PYEDIT Fortran function
extern "C" {
#ifdef WIN32
  void __stdcall PYEXEC( ) ;
#else
  void pyexec_ ( ) ;
#endif
} 

void Pythia::PyExec( ) {
#ifdef WIN32
  PYEXEC( ) ;
#else
  pyexec_ ( ) ;
#endif
}
