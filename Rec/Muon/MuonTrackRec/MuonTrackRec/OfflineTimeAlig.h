// $Id: OfflineTimeAlig.h,v 1.1 2009-10-16 17:05:58 ggiacomo Exp $
#ifndef MUONTRACKREC_OFFLINETIMEALIG_H 
#define MUONTRACKREC_OFFLINETIMEALIG_H 1

// Include files

/** @class OfflineTimeAlig OfflineTimeAlig.h MuonTrackRec/OfflineTimeAlig.h
 *  
 *  machinery for muon offline time alignment
 *  @author Giacomo GRAZIANI
 *  @date   2009-10-15
 */
#include <map>
#include <string>
class MuonLogPad;
#ifdef _WIN32
    #define EXTERN extern __declspec(dllimport) 
#else
    #define EXTERN extern
#endif

namespace OfflineTimeAlig {
      EXTERN bool m_offlineTimeAlignment;
      EXTERN std::string m_timeResidualFile;
      
      EXTERN std::map<long int, float>* ResMap;
      EXTERN long int logicalPadKey(int q, int s, int r, int nx, int ny, int view); 
      EXTERN bool loadTimeRes();  
      EXTERN void correctMisAlignment(MuonLogPad* pad);
      EXTERN void clearResMap();
    };
#endif // MUONTRACKREC_OFFLINETIMEALIG_H
