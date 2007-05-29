//  $Id: VeloExpectation.h,v 1.3 2007-05-29 13:43:06 mneedham Exp $
#ifndef TRACKTOOLS_VeloExpectation_H
#define TRACKTOOLS_VeloExpectation_H

//-----------------------------------------------------------------------------
/** @class VeloExpectation VeloExpectation.h
 *
 *  Tool to estimate how many hits are expected in the velo
 *
 *  @author M.Needham Matt.Needham@cern.ch
 *
 *  @date   30/12/2005
 */
//-----------------------------------------------------------------------------

#include "GaudiAlg/GaudiTool.h"
#include "TrackInterfaces/IVeloExpectation.h"
#include <string>

#include "Event/Track.h"

class DeVelo;
class DeVeloSensor;

namespace Tsa{
 class Line;
}

class VeloExpectation : public GaudiTool,
                      virtual public IVeloExpectation
{

public:

  /// constructer
  VeloExpectation( const std::string& type,
                 const std::string& name,
                 const IInterface* parent );

  virtual ~VeloExpectation();

  /// Tool initialization
  virtual StatusCode initialize();

  /** Returns number of hits expected, from zFirst to endVelo
   *
   *  @param aTrack Reference to the Track to test
   *
   *  @return number of hits expected
   */
  virtual int nExpected ( const LHCb::Track& aTrack ) const;


  /** Returns number of hits expected, from zFirst to endVelo
   *
   *  @param aTrack Reference to the Track to test
   *
   *  @return number of hits expected
  */
  virtual IVeloExpectation::Info expectedInfo ( const LHCb::Track& aTrack ) const;

  /** Returns number of hits expected, from zStart to zStop
   *
   *  @param aTrack Reference to the Track to test
   *  @param zStart --> start of scan range
   *  @param zStop --> end of scan range
   *  @return number of hits expected
   */
  virtual int nExpected ( const LHCb::Track& aTrack , const double zStart, const double zStop) const;

  /** Returns number of hits expected, from zStart to zStop
   *
   *  @param aTrack Reference to the Track to test
   *  @param zStart --> start of scan range
   *  @param zStop --> end of scan range
   *  @return Info
   */
  virtual IVeloExpectation::Info expectedInfo ( const LHCb::Track& aTrack , const double zStart, const double zStop) const;

  /** Returns number of hits missed, from zBeamLine to firstHit
   *
   *  @param aTrack Reference to the Track to test
   *
   *  @return number of hits missed before first hit
  */
  virtual int nMissed( const LHCb::Track& aTrack ) const;


  /** Returns number of hits missed, from z to firstHit
   *
   *  @param aTrack Reference to the Track to test
   *  @param z --> z to start from
   *
   *  @return number of hits missed before first hit
  */
  virtual int nMissed( const LHCb::Track& aTrack, const double z ) const;


  /** Returns true if track passses thro a given sensor
   *
   *  @param aTrack Reference to the Track to test
      @param sensorNum ---> sensor number 
   *
   *  @return true if inside sensor
  */
  bool isInside(const LHCb::Track& aTrack, 
	        const unsigned int sensorNum) const;

private:

  IVeloExpectation::Info scan(const LHCb::Track& aTrack, 
                              const double zStart, const double zStop) const;

  bool isInside(const DeVeloSensor* sensor, const Tsa::Line& xLine, 
                const Tsa::Line& yLine, const double z) const;

  double zMin(const LHCb::Track& aTrack) const;

  double zMax(const LHCb::Track& aTrack) const;

  int nFound(const LHCb::Track& aTrack,
             const double zStart, const double zStop) const;

  void param(const LHCb::Track& aTrack, const double z, 
             Tsa::Line& xLine, Tsa::Line& yLine) const;

  double zBeamLine(const LHCb::Track& aTrack) const;
 
  DeVelo* m_veloDet;

};

#endif // TRACKTOOLS_VeloExpectation_H
