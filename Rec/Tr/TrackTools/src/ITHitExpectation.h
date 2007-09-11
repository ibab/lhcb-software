// $Id: ITHitExpectation.h,v 1.1 2007-09-11 14:40:48 mneedham Exp $
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

#include <string>

namespace LHCb{
  class Track;
}

class DeSTDetector;
class IITExpectedHits;

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


  DeSTDetector* m_itDet;
  IITExpectedHits* m_expectedITHits;

};


#endif
