#pragma once

/** @class VPExpectation VPExpectation.h
 *
 *  Tool to estimate how many hits are expected in the VELO 
 *
 *  @author Thomas Bird
 *  @date   05/12/2012
 */

#include "GaudiAlg/GaudiTool.h"

// LHCb
// Event/TrackEvent
#include "Event/Track.h"

#include "TrackInterfaces/IVPExpectation.h"

class DeVP;
class DeVPSensor;

class VPExpectation : public GaudiTool, virtual public IVPExpectation {

 public:
  /// Constructor
  VPExpectation(const std::string &type, const std::string &name,
                const IInterface *parent);
  /// Destructor
  virtual ~VPExpectation();

  /// Tool initialization
  virtual StatusCode initialize();

  /** Returns number of hits expected, from zFirst to endVelo
   *
   *  @param track Reference to the Track to test
   *  @return number of hits expected
   */
  virtual int nExpected(const LHCb::Track &track) const;

  /** Returns info on expected hits
   *
   *  @param track Reference to the Track to test
   *  @return Info
   */
  virtual IVPExpectation::Info expectedInfo(const LHCb::Track &track) const;

  /** Returns number of hits expected, from zStart to zStop
   *
   *  @param track Reference to the Track to test
   *  @param zStart Start of scan range
   *  @param zStop End of scan range
   *  @return Info
   */
  virtual int nExpected(const LHCb::Track &track, const double zStart,
                        const double zStop) const;

  /** Returns expected hits info, from zStart to zStop
   *
   *  @param track Reference to the Track to test
   *  @param zStart Start of scan range
   *  @param zStop End of scan range
   *  @return Info
   */
  virtual IVPExpectation::Info expectedInfo(const LHCb::Track &track,
                                            const double zStart,
                                            const double zStop) const;

  /** Returns number of hits missed, from zBeamLine to firstHit
   *
   *  @param track Reference to the Track to test
   *  @return number of hits missed before first hit
   */
  virtual int nMissed(const LHCb::Track &track) const;

  /** Returns number of hits missed, from z to firstHit
   *
   *  @param track Reference to the Track to test
   *  @param z z to start from
   *  @return number of hits missed before first hit
   */
  virtual int nMissed(const LHCb::Track &track, const double z) const;

  /** Returns true if track passes through the active area of a sensor
   *
   *  @param track Reference to the Track to test
   *  @param sensorNum sensor number
   *  @return true if track crosses a working pixel
   */
  virtual bool isInside(const LHCb::Track &track,
                        const unsigned int sensorNum) const;

 private:
  IVPExpectation::Info scan(const LHCb::Track &track, const double zStart,
                            const double zStop) const;

  bool isInside(const DeVPSensor *sensor, const double x,
                const double y) const;

  double zMin(const LHCb::Track &track) const;
  double zMax(const LHCb::Track &track) const;

  int nFound(const LHCb::Track &track, const double zStart,
             const double zStop) const;

  double zBeamLine(const LHCb::Track &track) const;

  DeVP *m_det;
};

