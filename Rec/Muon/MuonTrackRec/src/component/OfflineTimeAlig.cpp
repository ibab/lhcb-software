// $Id: OfflineTimeAlig.cpp,v 1.1 2010-02-11 16:15:46 ggiacomo Exp $
// Include files

/** 
 *  
 *  machinery for muon offline time alignment
 *  @author Giacomo GRAZIANI
 *  @date   2009-10-15
 */
#include "MuonInterfaces/MuonLogPad.h"
#include <fstream>
#include <map>
#include <string>

#ifdef _WIN32
    #define DLLEXPORT __declspec(dllexport) 
#else
    #define DLLEXPORT
#endif


namespace OfflineTimeAlig {

  DLLEXPORT bool m_offlineTimeAlignment;
  DLLEXPORT std::string m_timeResidualFile;

  DLLEXPORT std::map<long int, float>* ResMap;
  DLLEXPORT long int logicalPadKey(int q, int s, int r, int nx, int ny, int view) {
    return (view + nx*3 + ny*96*3 + r*16*96*3 + s*4*16*96*3 + q*5*4*16*96*3);
  }

  DLLEXPORT bool loadTimeRes() {
    bool out = true;
    ResMap = NULL;
    if(m_offlineTimeAlignment && m_timeResidualFile != "none") { 
      char inbuf[100];
      int s=0,r=0,q=0,nx=0,ny=0,v=0;
      float res=0.;
      std::ifstream resList;
      resList.open(m_timeResidualFile.c_str());
      if (resList.good() ) {
        ResMap = new std::map<long int, float>;
        while (! resList.eof()) {
          resList.getline(inbuf,sizeof(inbuf));
          sscanf(inbuf,"M%dR%dQ%d %d %d v%d  %f",&s,&r,&q,&nx,&ny,&v,&res);
          long int key = logicalPadKey(q-1, s-1, r-1, nx, ny, v);
          (*ResMap)[key] = res /25. * 16. ; // convert from ns to TDC units
        }
      }
      else {
        m_offlineTimeAlignment = false;
        out = false;
      }
    }
    else {
      m_offlineTimeAlignment = false;
    }
    return out;
  }
  
  DLLEXPORT void correctMisAlignment(MuonLogPad* pad) {
    // this has to be updated with the unified format!!
    long int key=0;
    if (pad->type() == MuonLogPad::XTWOFE) {
      double d1=0.,d2=0.;
      key = logicalPadKey(pad->tile()->quarter(),
                          pad->tile()->station(),
                          pad->tile()->region(),
                          pad->tile()->nX(),
                          pad->tile()->nY() ,
                          1);
      if (ResMap->count(key)) {
        d1 = (*ResMap)[key];
      }
      key = logicalPadKey(pad->tile()->quarter(),
                                        pad->tile()->station(),
                                        pad->tile()->region(),
                                        pad->tile()->nX(),
                                        pad->tile()->nY() ,
                                        2);
      if (ResMap->count(key)) {
        d2 = (*ResMap)[key];
      }
      pad->shiftTimes( -d1, -d2 );
    }
    else {
      key = logicalPadKey(pad->tile()->quarter(),
                                        pad->tile()->station(),
                                        pad->tile()->region(),
                                        pad->tile()->nX(),
                                        pad->tile()->nY() ,
                                        0);
      if (ResMap->count(key)) {
        pad->shiftTime(-1. *  (*ResMap)[key]);
      }
    }
  }
  DLLEXPORT void clearResMap() {
    if(ResMap) delete ResMap;
  }

};


