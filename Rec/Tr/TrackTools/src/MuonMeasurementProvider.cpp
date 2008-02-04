// $Id: MuonMeasurementProvider.cpp,v 1.8 2008-02-04 09:52:44 wouter Exp $

/** @class MuonMeasurementProvider MuonMeasurementProvider.cpp
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
#include "Event/MuonMeasurement.h"
#include "Event/StateVector.h"
#include "MuonDet/DeMuonDetector.h"
#include "GaudiKernel/ToolFactory.h"
#include "Event/TrackParameters.h"

class MuonMeasurementProvider : public GaudiTool, virtual public IMeasurementProvider
{
public:
  
  /// constructer
  MuonMeasurementProvider(const std::string& type,
                          const std::string& name,
                          const IInterface* parent);
  
  ~MuonMeasurementProvider() {}
  
  StatusCode initialize() ;
  LHCb::Measurement* measurement( const LHCb::LHCbID& id, bool localY ) const  ;
  LHCb::Measurement* measurement( const LHCb::LHCbID& id, 
                                  const LHCb::ZTrajectory& /*reftraj*/,  bool localY ) const {
    return measurement(id,localY) ;
  }
  
  double nominalZ( const LHCb::LHCbID& id ) const ;
  
  void addToMeasurements( const std::vector<LHCb::LHCbID>& lhcbids,
                          std::vector<LHCb::Measurement*>& measurements,
                          const LHCb::ZTrajectory& reftraj) const ;
  StatusCode load( LHCb::Track&  ) const {
    info() << "sorry, MeasurementProviderBase::load not implemented" << endreq ;
    return StatusCode::FAILURE ;
  }

private:
  // pointer to detector
  mutable DeMuonDetector* m_det;
} ;

//=============================================================================
// Declare to tool factory
//=============================================================================


DECLARE_TOOL_FACTORY( MuonMeasurementProvider );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonMeasurementProvider::MuonMeasurementProvider( const std::string& type,
						const std::string& name,
						const IInterface* parent )
  :  GaudiTool( type, name , parent ),
     m_det(0)
{
  declareInterface<IMeasurementProvider>(this);
}

//-----------------------------------------------------------------------------
/// Initialize
//-----------------------------------------------------------------------------

StatusCode MuonMeasurementProvider::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if( sc.isFailure() ) { return Error( "Failed to initialize!", sc ); }

  // Retrieve the detector element
  m_det = getDet<DeMuonDetector>( "/dd/Structure/LHCb/DownstreamRegion/Muon" ) ;
  
  return sc;
}

//-----------------------------------------------------------------------------
/// Create a measurement
//-----------------------------------------------------------------------------

LHCb::Measurement* MuonMeasurementProvider::measurement( const LHCb::LHCbID& id, bool localY ) const
{
  LHCb::Measurement* meas(0) ;
  if( !id.isMuon() ) {
    error() << "Not a Muon measurement" << endreq ;
  } else {
    LHCb::MuonTileID muid = id.muonID();
    double x,y,z,dx,dy,dz;
    StatusCode sc = m_det->Tile2XYZ(muid,x,dx,y,dy,z,dz);
    if (sc.isFailure()){
      warning() << "Failed to get x,y,z of tile " << muid << endreq;
    } else {
      LHCb::MuonMeasurement::MuonMeasurementType dir = localY ? 
        LHCb::MuonMeasurement::Y : LHCb::MuonMeasurement::X ;
      meas = new LHCb::MuonMeasurement(id,*m_det,Gaudi::XYZPoint(x,y,z),dx,dy, dir);
      debug() << " Created muon measurement! " << muid << " x "<< x<<" y "<<y<<" z "<<z
              <<" dx "<<dx<<" dy "<<dy<<" dz "<<dz << endreq; 
    }
  }
  return meas ;
}


//-----------------------------------------------------------------------------
// Return the z-coordinate of this lhcb-id (w/o creating the hit)
//-----------------------------------------------------------------------------

double MuonMeasurementProvider::nominalZ( const LHCb::LHCbID& id ) const
{
  return m_det->getStationZ(id.muonID().station()) ;
}

//-----------------------------------------------------------------------------
/// Create measurements for list of LHCbIDs
//-----------------------------------------------------------------------------

void MuonMeasurementProvider::addToMeasurements( const std::vector<LHCb::LHCbID>& lhcbids,
                                                 std::vector<LHCb::Measurement*>& measurements,
                                                 const LHCb::ZTrajectory& reftraj) const
{
  for( std::vector<LHCb::LHCbID>::const_iterator id = lhcbids.begin() ;
       id != lhcbids.end(); ++id) {
    measurements.push_back( measurement(*id,reftraj,false) )  ;
    measurements.push_back( measurement(*id,reftraj,true) )  ;
  }
}
