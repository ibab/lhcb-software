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
class ITrackPtKick;

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

  virtual StatusCode initialize();
  
  virtual StatusCode execute();
 
 
private:

  LHCb::Track* convert(const SeedTrack* aTrack) const;
  void addState(const SeedTrack* aTrack, 
                LHCb::Track* lTrack, const double z) const;

  ITrackPtKick* m_ptKickTool;

  double m_EX2;
  double m_EY2;
  double m_ETx2;
  double m_ETy2;
  double m_EQdivP2;

  double m_likCut;
  double m_curvFactor;
  bool m_pFromCurvature;
  
};

#endif // _TsaToTrTrack_H_








