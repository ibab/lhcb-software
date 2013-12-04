//  $Id: VPExpectation.h,v 1.7 2009-08-06 18:19:10 smenzeme Exp $
#ifndef TRACKTOOLS_VPExpectation_H
#define TRACKTOOLS_VPExpectation_H

//-----------------------------------------------------------------------------
/** @class VPExpectation VPExpectation.h
 *
 *  Tool to estimate how many hits are expected in the velo
 *
 *  @author M.Needham Matt.Needham@cern.ch
 *
 *  @date   30/12/2005
 */
//-----------------------------------------------------------------------------

#include "GaudiAlg/GaudiTool.h"
#include "TrackInterfaces/IVPExpectation.h"
#include <string>

#include "Event/Track.h"
#include "TsaKernel/Line.h"
#include "DetDesc/IGeometryInfo.h"

#include "Linker/AllLinks.h"
#include "Linker/LinkedTo.h"
#include "Linker/LinkedFrom.h"
#include "Linker/LinkerTool.h"

class DeVP;
class DeVPSensor;

class VPExpectation : public GaudiTool,
                        virtual public IVPExpectation
{

public:

  /// constructer
  VPExpectation( const std::string& type,
                   const std::string& name,
                   const IInterface* parent );

  virtual ~VPExpectation();

  /// Tool initialization
  virtual StatusCode initialize();


  /** Returns number of hits expected, from zFirst to endVelo
   *
   *  @param aTrack Reference to the Track to test
   *
   *  @return number of hits expected
   */
  virtual int nExpected ( const LHCb::Track& aTrack ) const;


  /** Returns info on expected hits and fill bitmap for hit pattern
   *
   *  @param aTrack Reference to the Track to test
   *  @return Info
   */
  virtual IVPExpectation::Info expectedInfo ( const LHCb::Track& aTrack, std::bitset<30> velo[4] ) const;


  /** Returns info on expected hits
   *
   *  @param aTrack Reference to the Track to test
   *  @return Info
   */
  virtual IVPExpectation::Info expectedInfo ( const LHCb::Track& aTrack ) const;


  /** Returns number of hits expected, from zStart to zStop
   *
   *  @param aTrack Reference to the Track to test
   *  @param zStart --> start of scan range
   *  @param zStop --> end of scan range
   *  @return Info
   */
  virtual int nExpected ( const LHCb::Track& aTrack , const double zStart, const double zStop) const;


  /** Returns expected hits info, from zStart to zStop
   *
   *  @param aTrack Reference to the Track to test
   *  @param zStart --> start of scan range
   *  @param zStop --> end of scan range
   *  @return Info
   */
  virtual IVPExpectation::Info expectedInfo ( const LHCb::Track& aTrack , const double zStart, const double zStop) const;


   /** Returns expected hits info, from zStart to zStop and hit pattern
   *
   *  @param aTrack Reference to the Track to test
   *  @param zStart --> start of scan range
   *  @param zStop --> end of scan range
   *  @return Info
   */
  virtual IVPExpectation::Info expectedInfo ( const LHCb::Track& aTrack , const double zStart, const double zStop,
						std::bitset<30> velo[4]) const;


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
  virtual int nMissed( const LHCb::Track& aTrack, const double z) const;


  /** Returns true if track passses through a working strip in the sensor
   *
   *
   *  @param aTrack Reference to the Track to test
   *  @param sensorNum ---> sensor number
   *
   *  @return true if crosses a working strip
   */
  virtual bool isInside(const LHCb::Track& aTrack,
                const unsigned int sensorNum) const;



private:

  IVPExpectation::Info scan(const LHCb::Track& aTrack,
                              const double zStart, const double zStop,std::bitset<30> velo[4] ) const;

  bool isInside(const DeVPSensor* sensor, const Tf::Tsa::Line& xLine,
                const Tf::Tsa::Line& yLine, const double z) const;

  double zMin(const LHCb::Track& aTrack) const;

  double zMax(const LHCb::Track& aTrack) const;

  int nFound(const LHCb::Track& aTrack,
             const double zStart, const double zStop) const;

  void param(const LHCb::Track& aTrack, const double z,
             Tf::Tsa::Line& xLine, Tf::Tsa::Line& yLine) const;

  double zBeamLine(const LHCb::Track& aTrack) const;

  bool isInsideChildren(const IGeometryInfo* igi, const Gaudi::XYZPoint globalPoint) const;

  DeVP* m_veloDet;

};

#endif // TRACKTOOLS_VPExpectation_H
