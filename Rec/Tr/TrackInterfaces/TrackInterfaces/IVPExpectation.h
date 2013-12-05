// $Id: IVPExpectation.h,v 1.5 2009-08-13 14:58:04 smenzeme Exp $
#ifndef _IVPExpectation_H
#define _IVPExpectation_H

#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IVPExpectation( "IVPExpectation", 0, 0 );

/** @class IVPExpectation IVPExpectation.h TrackInterfaces/IVPExpectation.h
 *
 *  interface for selecting tracks....
 *
 *  @author M.Needham
 *  @date   11/03/2007
 */

namespace LHCb{
 class Track;
}

class IVPExpectation: virtual public IAlgTool{

public:


  /** Helper struct
  * n - Number of hits
  */
  struct Info {
    unsigned int n;
    double firstR;
    double firstX;
    double firstY;
    double firstZ;
    std::vector<double> expectedZ;
  };


  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IVPExpectation ; }

  /** Returns number of hits expected, from zFirst to endVelo
   *
   *  @param aTrack Reference to the Track to test
   *
   *  @return number of hits expected
   */
  virtual int nExpected ( const LHCb::Track& aTrack ) const = 0;


  /** Returns info on expected hits
   *
   *  @param aTrack Reference to the Track to test
   *  @return Info
   */
  virtual IVPExpectation::Info expectedInfo ( const LHCb::Track& aTrack ) const = 0;


  /** Returns number of hits expected, from zStart to zStop
   *
   *  @param aTrack Reference to the Track to test
   *  @param zStart --> start of scan range
   *  @param zStop --> end of scan range
   *  @return Info
   */
  virtual int nExpected ( const LHCb::Track& aTrack , const double zStart, const double zStop) const = 0;


  /** Returns expected hits info, from zStart to zStop
   *
   *  @param aTrack Reference to the Track to test
   *  @param zStart --> start of scan range
   *  @param zStop --> end of scan range
   *  @return Info
   */
  virtual IVPExpectation::Info expectedInfo ( const LHCb::Track& aTrack , const double zStart, const double zStop) const = 0;


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
   *  @param z --> z to start from
   *
   *  @return number of hits missed before first hit
   */
  virtual int nMissed( const LHCb::Track& aTrack, const double z) const = 0;


  /** Returns true if track passses through a working strip in the sensor
   *
   *
   *  @param aTrack Reference to the Track to test
   *  @param sensorNum ---> sensor number
   *
   *  @return true if crosses a working strip
   */
  virtual bool isInside(const LHCb::Track& aTrack,
                const unsigned int sensorNum) const = 0;


};

#endif
