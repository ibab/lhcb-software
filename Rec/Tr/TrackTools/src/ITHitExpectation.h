// $Id: ITHitExpectation.h,v 1.4 2009-07-03 13:31:03 mneedham Exp $
#ifndef _ITHitExpectation_H
#define _ITHitExpectation_H

/** @class ITHitExpectation ITHitExpectation.h
 *
 * Implementation of ITHitExpectation tool
 * see interface header for description
 *
 *  @author M.Needham
 *  @date   22/5/2007
 */


#include "THitExpectation.h"
#include "Kernel/ISTChannelIDSelector.h"
#include <string>



namespace LHCb{
  class Track;
  class STChannelID;
}

class DeSTDetector;


namespace Tf {
  namespace Tsa {
    class IITExpectedHits;
  }
}

class ITHitExpectation: public THitExpectation {

public:

  /** constructer */
  ITHitExpectation(const std::string& type,
                  const std::string& name,
                  const IInterface* parent);  

  /** destructer */
  virtual ~ITHitExpectation();

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

  bool select(const LHCb::STChannelID& chan) const;

  DeSTDetector* m_itDet;
  Tf::Tsa::IITExpectedHits* m_expectedITHits;
  std::string m_selectorType;
  std::string m_selectorName;
  ISTChannelIDSelector* m_selector;
  
 
};


inline bool ITHitExpectation::select(const LHCb::STChannelID& chan) const{
  return m_selector == 0 ? true : m_selector->select(chan); 
}


#endif
