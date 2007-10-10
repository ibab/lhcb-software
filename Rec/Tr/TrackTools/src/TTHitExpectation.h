// $Id: TTHitExpectation.h,v 1.2 2007-10-10 18:32:17 smenzeme Exp $
#ifndef _TTHitExpectation_H
#define _TTHitExpectation_H

/** @class TTHitExpectation TTHitExpectation.h
 *
 * Implementation of TTHitExpectation tool
 * see interface header for description
 *
 *  @author M.Needham
 *  @date   22/5/2007
 */

#include "GaudiAlg/GaudiTool.h"
#include "TsaKernel/Line3D.h"
#include "TrackInterfaces/IHitExpectation.h"

#include <string>

namespace LHCb{
  class Track;
  class StateVector;
  
  class STChannelID;
}



class DeSTDetector;
class DeSTSector;
class ITrackExtrapolator;

class TTHitExpectation: public GaudiTool,
                       virtual public IHitExpectation  {

public:

  /** constructer */
  TTHitExpectation(const std::string& type,
                  const std::string& name,
                  const IInterface* parent);  

  /** destructer */
  virtual ~TTHitExpectation();

  /** intialize */
  StatusCode initialize();

  /** Returns number of hits expected, from zFirst to inf
  *
  *  @param aTrack Reference to the Track to test
  *
  *  @return number of hits expected
  */
  virtual unsigned int nExpected ( const LHCb::Track& aTrack ) const;


  /** Returns number of hits expected
  *
  *  @param aTrack Reference to the Track to test
  *
  *  @return Info info including likelihood
  */
  virtual IHitExpectation::Info expectation( const LHCb::Track& aTrack ) const;
                                                                           
private:


  void collectHits(std::vector<LHCb::STChannelID>& chan, 
		  LHCb::StateVector stateVec, const unsigned int station ) const;

  bool insideSector(const DeSTSector* sector,const LHCb::StateVector& stateVec) const;
 
  ITrackExtrapolator* m_extrapolator;
  DeSTDetector* m_ttDet;
  double m_zTTa;
  double m_zTTb;

  IHitExpectation* m_expectedHits;

};


inline bool TTHitExpectation::insideSector(const DeSTSector* sector,
                                           const LHCb::StateVector& stateVec) const{

  bool isIn = false;
  Gaudi::XYZPoint point;
  Tf::Tsa::Line3D line(stateVec.position(), stateVec.slopes());
  double mu;
  if (Gaudi::Math::intersection(line, sector->plane() ,point, mu) == true){
    isIn = sector->globalInActive(point);
  }
  return isIn;
}


#endif
