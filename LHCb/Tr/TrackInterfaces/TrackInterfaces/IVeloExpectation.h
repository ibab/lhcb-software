// $Id: IVeloExpectation.h,v 1.2 2007-03-13 13:38:46 cattanem Exp $ 
#ifndef _IVeloExpectation_H
#define _IVeloExpectation_H
 
#include "GaudiKernel/IAlgTool.h"
 
static const InterfaceID IID_IVeloExpectation( "IVeloExpectation", 0, 0 );
 
/** @class IVeloExpectation IVeloExpectation.h TrackInterfaces/IVeloExpectation.h
 *
 *  interface for selecting tracks....
 *
 *  @author M.Needham
 *  @date   11/03/2007
 */

namespace LHCb{ 
 class Track;
};
  
class IVeloExpectation: virtual public IAlgTool{
 
public:
                                                                                
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IVeloExpectation ; }
                                                                               
  /** Returns number of hits expected, from zFirst to endVelo
  *
  *  @param aTrack Reference to the Track to test
  *
  *  @return number of hits expected
  */
  virtual int nExpected ( const LHCb::Track& aTrack ) const = 0;


  /** Returns number of hits expected, from zStart to zStop
   *
   *  @param aTrack Reference to the Track to test
   *  @param zStart --> start of scan range
   *  @param zStop --> end of scan range
   *
   *  @return number of hits expected
   */
  virtual int nExpected ( const LHCb::Track& aTrack , const double zStart, const double zStop) const = 0;


  /** Returns number of hits missed, from zBeamLine to firstHit
  *
  *  @param aTrack Reference to the Track to test
  *
  *  @return number of hits missed before first hit
  */
  virtual int nMissed( const LHCb::Track& aTrack ) const = 0;

  /** Returns number of hits missed, from z to firstHit
   *
   *  @param aTrack Reference to the Track to test
   *  @param z --> start of scan range
   *
   *  @return number of hits missed before first hit
  */
  virtual int nMissed( const LHCb::Track& aTrack, const double z ) const = 0;
        
   /** Returns true if track passses thro a given sensor
   *
   *  @param aTrack Reference to the Track to test
      @param sensorNum ---> sensor number 
   *
   *  @return true if inside sensor
   */
   virtual bool isInside(const LHCb::Track& aTrack, 
	                 const unsigned int sensorNum) const = 0;

                                                                       
};
                                                                               
#endif
