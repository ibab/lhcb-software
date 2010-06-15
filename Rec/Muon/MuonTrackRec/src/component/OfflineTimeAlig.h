// $Id: OfflineTimeAlig.h,v 1.1 2010-02-11 16:15:47 ggiacomo Exp $
#ifndef MUONTRACKREC_OFFLINETIMEALIG_H 
#define MUONTRACKREC_OFFLINETIMEALIG_H 1

// Include files

/** @class OfflineTimeAlig OfflineTimeAlig.h MuonInterfaces/OfflineTimeAlig.h
 *  
 *  machinery for muon offline time alignment
 *  @author Giacomo GRAZIANI
 *  @date   2009-10-15
 */
#include <map>
#include <string>
class MuonLogPad;

namespace OfflineTimeAlig {
      extern bool m_offlineTimeAlignment;
      extern std::string m_timeResidualFile;
      
      extern std::map<long int, float>* ResMap;
      extern long int logicalPadKey(int q, int s, int r, int nx, int ny, int view); 
      extern bool loadTimeRes();  
      extern void correctMisAlignment(MuonLogPad* pad);
      extern void clearResMap();
    };
#endif // MUONTRACKREC_OFFLINETIMEALIG_H
