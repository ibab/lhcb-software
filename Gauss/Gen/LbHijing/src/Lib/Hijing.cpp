// $Id: Hijing.cpp,v 1.1.1.1 2006-07-03 17:23:35 gcorti Exp $
// Include files

// local
#include "LbHijing/Hijing.h"

#include <string.h>

//-----------------------------------------------------------------------------
// Implementation file for class : Hijing
//
// 2006-02-25 : Daniele Gregori
//-----------------------------------------------------------------------------

// HIJSET Fortran function
extern "C" {
#ifdef WIN32
  void __stdcall HIJSET(const float*, const char*, int, const char*, int,
                        const char*, int,  const int*, const int*, const int*,
                        const int*) ;
#else
  void hijset_ (float&, const char*, const char*, const char*, 
                int&, int&, int&, int&, int, int, int);
#endif
}

void Hijing::HijingInit(double l_efrm, const std::string l_frame,
                        const std::string l_proj, const std::string l_targ,
                        int l_iap, int l_izp, int l_iat, int l_izt) {
  float efrm=l_efrm;
  
  char frame[8]; memset(frame,' ',8); strncpy(frame, l_frame.c_str(), strlen(l_frame.c_str()));
  char proj[8]; memset(proj,' ',8); strncpy(proj, l_proj.c_str(), strlen(l_proj.c_str()));
  char targ[8]; memset(targ,' ',8); strncpy(targ, l_targ.c_str(), strlen(l_targ.c_str()));
  
  int iap=l_iap;
  int izp=l_izp;
  int iat=l_iat;
  int izt=l_izt;
  
#ifdef WIN32
  HIJSET( &efrm, frame, strlen(frame), proj, strlen(proj), targ,
          strlen(targ), &iap, &izp, &iat, &izt);
#else
  hijset_ ( efrm, frame, proj, targ, iap, izp, iat, izt,
            strlen(frame), strlen(proj), strlen(targ));
#endif
}

// HIJING Fortran function
extern "C" {
#ifdef WIN32
  void __stdcall HIJING( const char*, int, const float*, const float* ) ;
#else
  void hijing_ (const char*, float&, float&, int ) ;
#endif
}

// LUHEPC Fortran function
extern "C" {
#ifdef WIN32
  void __stdcall HILUNHEP( int * ) ;
#else
  void hilunhep_( int & ) ;
#endif 
}

void Hijing::LunHep( int mcconv ) {
#ifdef WIN32
  HILUNHEP( &mcconv) ;
#else
  hilunhep_( mcconv ) ;
#endif
}

void Hijing::HijingEvnt( const std::string l_frame, double l_bmin, double l_bmax ) {
  char frame[8]; memset(frame,' ',8); strncpy(frame, l_frame.c_str(), strlen(l_frame.c_str()));
  float bmin=l_bmin;
  float bmax=l_bmax;
  
#ifdef WIN32
  HIJING( frame, strlen(frame), &bmin, &bmax) ;
#else
  hijing_( frame , bmin , bmax , strlen(frame) ) ;
#endif
}

