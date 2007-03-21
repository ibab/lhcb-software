// $Id: TsaToTrackCnv.h,v 1.2 2007-03-21 14:35:02 cattanem Exp $
#ifndef _TsaToTrackCnv_H_
#define _TsaToTrackCnv_H_

/** @class TsaToTrackCnv TsaToTrackCnv.h
 *
 * Converter to fit tracks
 *
 *  @author M.Needham
 *  @date   30/04/2004
 */

#include "GaudiAlg/GaudiAlgorithm.h"

#include <string>
#include <vector>

namespace Tsa{
  class Track;
}

namespace LHCb{
  class State;
  class Track;
};



class TsaToTrackCnv: public GaudiAlgorithm {

public:

  // Constructors and destructor
  TsaToTrackCnv(const std::string& name, 
                   ISvcLocator* pSvcLocator); 
  virtual ~TsaToTrackCnv();  


  
  virtual StatusCode execute();
 
 
private:

  LHCb::Track* convert(const Tsa::Track* aTrack) const;
  void addState(const Tsa::Track* aTrack, 
                LHCb::Track* lTrack) const;



  double m_EX2;
  double m_EY2;
  double m_ETx2;
  double m_ETy2;
  double m_EQdivP2;

};

#endif // _TsaToTrTrack_H_








