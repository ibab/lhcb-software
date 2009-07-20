// $Id: IHitExpectation.h,v 1.2 2009-07-20 11:16:09 mneedham Exp $ 
#ifndef _IHitExpectation_H
#define _IHitExpectation_H
 
#include "GaudiKernel/IAlgTool.h"
 
static const InterfaceID IID_IHitExpectation( "IHitExpectation", 0, 0 );
 
/** @class IHitExpectation IHitExpectation.h TrackInterfaces/IHitExpectation
 *
 *  How many hits (of a given type) do we expect on a track ?
 *
 *  @author M.Needham
 *  @date   07/09/2007
 */

namespace LHCb{ 
 class Track;
 class LHCbID;
};
  
#include <vector>

class IHitExpectation: virtual public IAlgTool{
 
public:

  /** small struct returning hit info....
  * nExpected --> number of expected hits
  * number of the expected hits that are found 
  * likelihood -> what these hits would contribute to likelihood 
  * (In fact this only matters for OT where eff = function of r)  
  */ 
  typedef struct {
    unsigned int nExpected;
    unsigned int nFound;
    double likelihood;
  } Info;
      
                                                                        
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IHitExpectation ; }
                                                                               
  /** Returns number of hits expected, from zFirst to inf
  *
  *  @param aTrack Reference to the Track to test
  *
  *  @return unsigned int number of hits expected
  */
  virtual unsigned int nExpected ( const LHCb::Track& aTrack ) const = 0;
                                                                           

  /** Returns number of hits expected
  *
  *  @param aTrack Reference to the Track to test
  *
  *  @return Info info including likelihood
  */
  virtual Info expectation( const LHCb::Track& aTrack ) const = 0;
                                                                                   
  /** Collect all the expected hits
   *
   * @param aTrack Reference to the Track to test
   * @param hits collected lhcbIDs
   * 
   **/ 
  virtual void collect(const LHCb::Track& aTrack, 
                       std::vector<LHCb::LHCbID>& ids ) const = 0;
                                                                                 

                                                                      
};
                                                                               
#endif
