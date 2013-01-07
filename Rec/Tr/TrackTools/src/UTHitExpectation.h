// $Id: UTHitExpectation.h,v 1.7 2009-07-20 11:16:57 mneedham Exp $
#ifndef _UTHitExpectation_H
#define _UTHitExpectation_H

/** @class UTHitExpectation UTHitExpectation.h
 *
 * Implementation of UTHitExpectation tool
 * see interface header for description
 *
 *  @author M.Needham
 *  @date   22/5/2007
 */

#include "GaudiAlg/GaudiTool.h"
#include "TsaKernel/Line3D.h"
#include "TrackInterfaces/IHitExpectation.h"
#include "LHCbMath/GeomFun.h"
#include "GaudiKernel/Plane3DTypes.h"
#include <string>
#include "Kernel/ISTChannelIDSelector.h"

namespace LHCb{
  class Track;
  class StateVector;  
  class STChannelID; 
  class LHCbID;
}


class DeSTDetector;
class DeSTSensor;
class ITrackExtrapolator;


class UTHitExpectation: public GaudiTool,
                       virtual public IHitExpectation  {

public:

  /** constructer */
  UTHitExpectation(const std::string& type,
                  const std::string& name,
                  const IInterface* parent);  

  /** destructer */
  virtual ~UTHitExpectation();

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

  /** Collect all the expected hits
   *
   * @param aTrack Reference to the Track to test
   * @param hits collected lhcbIDs
   * 
   **/ 
  virtual void collect(const LHCb::Track& aTrack, 
                       std::vector<LHCb::LHCbID>& ids ) const;
                                                                                                                                                            
private:


  void collectHits(std::vector<LHCb::STChannelID>& chans, 
		  LHCb::StateVector stateVec, const unsigned int station ) const;

  bool insideSensor(const DeSTSensor* sensor,const Tf::Tsa::Line3D& line) const;

  Gaudi::XYZPoint intersection(const Tf::Tsa::Line3D& line,
                               const Gaudi::Plane3D& aPlane) const;

  bool select(const LHCb::STChannelID& chan) const;

  std::string m_extrapolatorName; 
  ITrackExtrapolator* m_extrapolator;
  DeSTDetector* m_utDet;
  double m_zUTa;
  double m_zUTb;

  std::string m_selectorType;
  std::string m_selectorName;
  ISTChannelIDSelector* m_selector;

  bool m_allStrips;

};


#include "STDet/DeSTSensor.h"

inline bool UTHitExpectation::insideSensor(const DeSTSensor* sensor,
                                           const Tf::Tsa::Line3D& line) const{

  bool isIn = false;
  Gaudi::XYZPoint point;
  double mu;
  if (Gaudi::Math::intersection(line, sensor->plane() ,point, mu) == true){
    isIn = sensor->globalInActive(point);
  }
  return isIn;
}



inline bool UTHitExpectation::select(const LHCb::STChannelID& chan) const{
  return m_selector == 0 ? true : m_selector->select(chan); 
}


#endif
