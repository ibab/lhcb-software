#ifndef _FTHitExpectation_H
#define _FTHitExpectation_H

/** @class FTHitExpectation FTHitExpectation.h
 *
 * Implementation of FTHitExpectation tool
 * see interface header for description
 *
 *  @author D.Milanes
 *  @date   20/13/2013
 */


#include "THitExpectation.h"


namespace LHCb{
  class Track;
  class FTChannelID;
  class LHCbID;
}

class DeFTDetector;

/*
namespace Tf {
  namespace Tsa {
    class IITExpectedHits;
  }
}
*/
class FTHitExpectation: public THitExpectation {

public:

  /** constructer */
  FTHitExpectation(const std::string& type,
		   const std::string& name,
		   const IInterface* parent);  
  
  /** destructer */
  virtual ~FTHitExpectation();

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


  //bool select(const LHCb::FTChannelID& chan) const;

  void EntryExitPlanes( DeFTFibreMat& mat,  Gaudi::Plane3D& entry,  Gaudi::Plane3D& exit ) const;
  void collectHits( Tf::Tsa::Line3D aLine3D,
		    LHCb::FTChannelID chan,
		    std::vector<std::pair<LHCb::FTChannelID, double> >& vectFTPairs
		    ) const;
  Gaudi::XYZPoint intersection(const Tf::Tsa::Line3D& line, const Gaudi::Plane3D& aPlane) const;


  DeFTDetector* m_ftDet;
  std::vector<DeFTLayer*>    m_layers;
  std::vector<DeFTFibreMat*> m_fibremats;

  // Tf::Tsa::IITExpectedHits* m_expectedITHits;
  // std::string m_selectorType;
  //  std::string m_selectorName;
  //  ISTChannelIDSelector* m_selector;
  //  bool m_allStrips;  
 
};

/*
inline bool ITHitExpectation::select(const LHCb::STChannelID& chan) const{
  return m_selector == 0 ? true : m_selector->select(chan); 
}
*/

#endif
