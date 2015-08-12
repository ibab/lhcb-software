/** @class OTMeasurementProvider OTMeasurementProvider.cpp
 *
 * Implementation of VeloRMeasurementProvider tool
 * see interface header for description
 *
 *  @author Wouter Hulsbergen
 *  @date   30/12/2005
 */
  
#include "TrackInterfaces/IMeasurementProvider.h"
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrackInterfaces/ITrackExtrapolator.h"
#include "Event/OTMeasurement.h"
#include "Event/StateVector.h"
#include "OTDet/DeOTDetector.h"
#include "OTDAQ/IOTRawBankDecoder.h"
#include "Event/TrackParameters.h"
#include "TrackKernel/TrackTraj.h"

class OTMeasurementProvider : public GaudiTool, virtual public IMeasurementProvider
{
public:
  
  /// constructer
  OTMeasurementProvider(const std::string& type,
			  const std::string& name,
			  const IInterface* parent);
  
  ~OTMeasurementProvider() override = default;
  
  StatusCode initialize() override;
  StatusCode finalize() override;
  LHCb::Measurement* measurement( const LHCb::LHCbID& id, bool localY=false ) const  override;
  LHCb::Measurement* measurement( const LHCb::LHCbID& id, const LHCb::ZTrajectory& refvector, bool localY=false) const override;
  double nominalZ( const LHCb::LHCbID& id ) const override;

  void addToMeasurements( const std::vector<LHCb::LHCbID>& lhcbids,
                          std::vector<LHCb::Measurement*>& measurements,
                          const LHCb::ZTrajectory& reftraj) const override;

  StatusCode load( LHCb::Track&  ) const override {
    info() << "sorry, MeasurementProviderBase::load not implemented" << endmsg ;
    return StatusCode::FAILURE ;
  }

private:
  inline LHCb::OTMeasurement* otmeasurement( const LHCb::LHCbID& id ) const  ;

  const DeOTDetector* m_det;
  ToolHandle<IOTRawBankDecoder> m_otdecoder ;
  bool m_applyTofCorrection ;
  double m_magnetZTofCorrection ;
  double m_tofCorrectionScale ;
} ;

//=============================================================================
// Declare to tool factory
//=============================================================================


DECLARE_TOOL_FACTORY( OTMeasurementProvider )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
OTMeasurementProvider::OTMeasurementProvider( const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent )
  :  GaudiTool( type, name , parent ),
     m_det(0),
     m_otdecoder("OTRawBankDecoder"),
     m_applyTofCorrection(true),
     m_magnetZTofCorrection(5140*Gaudi::Units::mm),
     m_tofCorrectionScale(0.46) 
{
  declareInterface<IMeasurementProvider>(this);
  declareProperty("ApplyTofCorrection",m_applyTofCorrection) ;
  declareProperty("RawBankDecoder",m_otdecoder) ;
}

//-----------------------------------------------------------------------------
/// Initialize
//-----------------------------------------------------------------------------

StatusCode OTMeasurementProvider::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if( sc.isFailure() ) { return Error( "Failed to initialize!", sc ); }

  // Retrieve the detector element
  m_det = getDet<DeOTDetector>( DeOTDetectorLocation::Default ) ;
  // Retrieve the raw bank decoder (for recreating the OTLiteTime)
  m_otdecoder.retrieve().ignore() ;
  
  return sc;
}

//-----------------------------------------------------------------------------
/// Finalize
//-----------------------------------------------------------------------------

StatusCode OTMeasurementProvider::finalize()
{
  m_otdecoder.release().ignore() ;
  return GaudiTool::finalize();
}

//-----------------------------------------------------------------------------
/// Create a measurement
//-----------------------------------------------------------------------------

inline LHCb::OTMeasurement* OTMeasurementProvider::otmeasurement( const LHCb::LHCbID& id ) const
{
  if( UNLIKELY(!id.isOT()) ) {
    error() << "Not an OT measurement" << endmsg ;
    return nullptr;
  } 
  LHCb::OTChannelID otid = id.otID() ;
  const DeOTModule* module = m_det->findModule( otid ) ;
  return new LHCb::OTMeasurement(m_otdecoder->time(otid),*module) ;
}

LHCb::Measurement* OTMeasurementProvider::measurement( const LHCb::LHCbID& id, bool /*localY*/ ) const
{
  return otmeasurement(id) ;
}

//-----------------------------------------------------------------------------
/// Create a measurement with statevector. For now very inefficient.
//-----------------------------------------------------------------------------

inline double sqr(double x) { return x*x ; }
LHCb::Measurement* OTMeasurementProvider::measurement( const LHCb::LHCbID& id, 
                                                       const LHCb::ZTrajectory& reftraj, bool /*localY*/ ) const 
{
  // default implementation
  LHCb::OTMeasurement* meas = otmeasurement(id) ;
  if(meas) {
    // set the tof correction
    if(m_applyTofCorrection) {
      // make the time-of-flight correction
      const double zmagnet  = m_magnetZTofCorrection ;
      // straight line length
      LHCb::StateVector refvector = reftraj.stateVector(meas->z()) ;
      auto point = refvector.position() ;
      auto L0 = point.R() ;
      // Correction following the kick formula (to 2nd order in dtx)
      auto f = zmagnet/point.z() ;
      auto dtx = refvector.tx() - point.x()/point.z() ;
      auto dL = 0.5*(1-f)/f*dtx*dtx*L0 ;
      // The kick correction is actually too long. Until we find something better, we simply scale with a factor derived in MC:
      auto L = L0 + m_tofCorrectionScale * dL ;
      meas->setTimeOfFlight( L / Gaudi::Units::c_light ) ;
    }
  }
  return meas ;
}

//-----------------------------------------------------------------------------
// Return the z-coordinate of this lhcb-id (w/o creating the hit)
//-----------------------------------------------------------------------------

double OTMeasurementProvider::nominalZ( const LHCb::LHCbID& id ) const
{
  LHCb::OTChannelID otid = id.otID() ;
  const DeOTModule* module = m_det->findModule(otid) ;
  if(UNLIKELY(!module)) { 
      error() << "Cannot find OT module for ID = " << otid << endmsg ;
      return 0;
  }
  // It would be nice if there were a short cut ...
  double dxdy, dzdy, xAtYEq0, zAtYEq0, ybegin, yend ;
  module->trajectory(otid.straw(),dxdy,dzdy,xAtYEq0,zAtYEq0,ybegin,yend) ;
  // This is the z-position of the 'non-readout' side.
  // z = zAtYEq0 + ybegin * dzdy ;
  // This is the z-position of the middle of the wire
  return zAtYEq0 + (ybegin+yend)/2 * dzdy ;
}

//-----------------------------------------------------------------------------
/// Create measurements for list of LHCbIDs
//-----------------------------------------------------------------------------

void OTMeasurementProvider::addToMeasurements( const std::vector<LHCb::LHCbID>& lhcbids,
                                               std::vector<LHCb::Measurement*>& measurements,
                                               const LHCb::ZTrajectory& reftraj) const
{
  measurements.reserve( measurements.size() + lhcbids.size() );
  std::transform( std::begin(lhcbids), std::end(lhcbids),
                  std::back_inserter(measurements),
                  [&](const LHCb::LHCbID& id) {
                      return measurement(id,reftraj,false);
                  } );
}
