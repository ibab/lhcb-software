// $Id: OTHitExpectation.h,v 1.1 2007-09-11 14:41:23 mneedham Exp $
#ifndef _OTHitExpectation_H
#define _OTHitExpectation_H

/** @class OTHitExpectation THitExpectation.h
 *
 * Implementation of OTHitExpectation tool
 * see interface header for description
 *
 *  @author M.Needham
 *  @date   22/5/2007
 */

#include "THitExpectation.h"

#include <string>
#include <vector>

namespace LHCb{
  class Track;
  class LHCbID;
  class OTChannelID;
}

class DeOTDetector;
class IOTExpectedHits;

class OTHitExpectation: public THitExpectation  {

public:

  /** constructer */
  OTHitExpectation(const std::string& type,
                   const std::string& name,
                   const IInterface* parent);  

  /** destructer */
  virtual ~OTHitExpectation();

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


  LHCb::OTChannelID channelHint(const LHCb::OTChannelID& hintChan,
                                const std::vector<LHCb::LHCbID>& ids) const;

  DeOTDetector* m_otDet;
  IOTExpectedHits* m_expectedOTHits;
  std::vector<double> m_likPar;

};


#endif
