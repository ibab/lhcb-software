// $Id: MuonMeasurementProvider.cpp,v 1.3 2007-07-05 15:08:24 cattanem Exp $

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

class MuonMeasurementProvider : public GaudiTool, 
				virtual public IMeasurementProvider
{
public:
  
  /// constructer
  MuonMeasurementProvider(const std::string& type,
			  const std::string& name,
			  const IInterface* parent);
  
  virtual ~MuonMeasurementProvider() {}
  
  virtual StatusCode initialize() ;
  virtual LHCb::Measurement* measurement( const LHCb::LHCbID& id ) const  ;
  virtual LHCb::Measurement* measurement( const LHCb::LHCbID& id, const LHCb::StateVector& refvector ) const ;
  virtual StatusCode update(  LHCb::Measurement&, const LHCb::StateVector& refvector ) const ;
  virtual double nominalZ( const LHCb::LHCbID& id ) const ;
  virtual StatusCode load( LHCb::Track& track ) const ; 

private:
  // tools
  ToolHandle<ITrackExtrapolator>           m_extrapolator ; ///< Extrapolator tool
  
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
  : GaudiTool ( type, name , parent ),
    m_extrapolator("TrackMasterExtrapolator"),
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

  sc = m_extrapolator.retrieve() ;
  if( sc.isFailure() ) { return Error( "Failed to initialize extrapolator!", sc ); }

  // Retrieve the detector element
  m_det = getDet<DeMuonDetector>( "/dd/Structure/LHCb/DownstreamRegion/Muon" ) ;
  
  return sc;
}

//-----------------------------------------------------------------------------
/// Create a measurement
//-----------------------------------------------------------------------------

LHCb::Measurement* MuonMeasurementProvider::measurement( const LHCb::LHCbID& id ) const
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
      meas = new LHCb::MuonMeasurement(id,Gaudi::XYZPoint(x,y,z),dx,dy);
      debug() << " Created muon measurement! " << muid << endreq; 
    }
  }
  return meas ;
}

//-----------------------------------------------------------------------------
/// Create a measurement with statevector. For now very inefficient.
//-----------------------------------------------------------------------------
LHCb::Measurement* MuonMeasurementProvider::measurement( const LHCb::LHCbID& id, const LHCb::StateVector& refvector ) const 
{
  // default implementation
  LHCb::Measurement* rc = measurement(id) ;
  if(rc) {
    StatusCode sc = update(*rc,refvector) ;
    if(!sc.isSuccess()) {
      // if the update failed, we delete the measurement
      delete rc ;
      rc =0 ;
    }
  }
  return rc ;
}

//-----------------------------------------------------------------------------
/// Update a measurement with a statevector
//-----------------------------------------------------------------------------

StatusCode MuonMeasurementProvider::update( LHCb::Measurement& ameas, 
					   const LHCb::StateVector& refvector ) const
{
  StatusCode sc = StatusCode::SUCCESS ;
  LHCb::MuonMeasurement* meas = dynamic_cast<LHCb::MuonMeasurement*>(&ameas) ;
  if( !meas ) {
    error() << "Wrong type " << ameas.type() << endreq ;
    sc = StatusCode::FAILURE ;
  } else {
    // move the reference vector to the z position of the measurement
    LHCb::StateVector refvectoratz = refvector ;
    if(refvectoratz.z() != meas->z() ) {
      sc = const_cast<ITrackExtrapolator*>(&(*m_extrapolator))->propagate( refvectoratz, meas->z() ) ;
      if( !sc.isSuccess() ) 
	error() << "Problem progagating state from " << refvector.z() << " to " << ameas.z() << endreq ;
    }
    meas->setRefVector( refvector.parameters() ) ;
  }
  return sc ;
}

//-----------------------------------------------------------------------------
// Return the z-coordinate of this lhcb-id (w/o creating the hit)
//-----------------------------------------------------------------------------

double MuonMeasurementProvider::nominalZ( const LHCb::LHCbID& id ) const
{
  LHCb::MuonTileID muid = id.muonID();
  double x,y,z,dx,dy,dz;
  StatusCode sc = m_det->Tile2XYZ(id.muonID(),x,dx,y,dy,z,dz);
  sc.ignore() ;
  // this would be a lot cheaper:
  std::cout << "muon z: " << m_det->getStationZ(id.muonID().station()) << " " << z << std::endl ;
  return z ;
}

//-----------------------------------------------------------------------------
// Load all hits of this type on the track
//-----------------------------------------------------------------------------

StatusCode MuonMeasurementProvider::load( LHCb::Track& ) const 
{
  info() << "Sorry, ::load(track), not implemented for " << name() << endreq ;
  return StatusCode::FAILURE ;
}
