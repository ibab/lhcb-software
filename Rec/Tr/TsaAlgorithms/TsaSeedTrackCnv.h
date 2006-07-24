// $Id: TsaSeedTrackCnv.h,v 1.1.1.1 2006-07-24 14:56:45 mneedham Exp $
#ifndef _TsaSeedTrackCnv_H_
#define _TsaSeedTrackCnv_H_

/** @class TsaSeedTrackCnv TsaSeedTrackCnv Tsa/TsatoTrackCnv.h
 *
 * Convertor to fit tracks
 *
 *  @author M.Needham
 *  @date   30/04/2004
 */

#include "TsaBaseAlg.h"

#include <string>
#include <vector>

class SeedTrack;

namespace LHCb{
  class State;
  class Track;
};

class TsaSeedTrackCnv: public TsaBaseAlg {

public:

  // Constructors and destructor
  TsaSeedTrackCnv(const std::string& name, 
                   ISvcLocator* pSvcLocator); 
  virtual ~TsaSeedTrackCnv();  


  
  virtual StatusCode execute();
 
 
private:

  LHCb::Track* convert(const SeedTrack* aTrack) const;
  void addState(const SeedTrack* aTrack, 
                LHCb::Track* lTrack, const double z) const;


  double m_EX2;
  double m_EY2;
  double m_ETx2;
  double m_ETy2;
  double m_EQdivP2;
  double m_likCut;
  double m_curvFactor;
  
};

#endif // _TsaToTrTrack_H_








