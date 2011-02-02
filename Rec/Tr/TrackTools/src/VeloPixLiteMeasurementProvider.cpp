 // $Id: VeloPixLiteMeasurementProvider.cpp,v 1.1 2010-04-13 09:17:45 cocov Exp $

/** @class VeloPixLiteMeasurementProvider VeloPixLiteMeasurementProvider.cpp
 *
 * Implementation of VeloPixLiteMeasurementProvider tool
 * see interface header for description
 *
 *  @author Victor Coco based on Wouter Hulsbergen + Stefania Vecchi MuonMeasurmentProvider
 *  @date   12/02/2010
 */
  
#include "TrackInterfaces/IMeasurementProvider.h"
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrackInterfaces/ITrackExtrapolator.h"
#include "Event/VeloPixLiteMeasurement.h"
#include "Event/VeloPixLiteCluster.h"
#include "Event/StateVector.h"
#include "VeloPixDet/DeVeloPix.h"
#include "GaudiKernel/ToolFactory.h"
#include "Event/TrackParameters.h"
#include "TrackInterfaces/IVeloPixClusterPosition.h"
#include "GaudiKernel/IIncidentListener.h"

#include "Event/Measurement.h"
#include "TrackKernel/ZTrajectory.h"

#include "GaudiKernel/IIncidentSvc.h"

class VeloPixLiteMeasurementProvider : public GaudiTool, virtual public IMeasurementProvider,
                             virtual public IIncidentListener
{
public:
  
  /// constructer
  VeloPixLiteMeasurementProvider(const std::string& type,
                          const std::string& name,
                          const IInterface* parent);
  
  ~VeloPixLiteMeasurementProvider() {}
  
  StatusCode initialize() ;
  /// measurement for single hits

  void handle ( const Incident& incident );

  LHCb::Measurement* measurement( const LHCb::LHCbID& id, bool localY ) const  ;              
  LHCb::Measurement* measurement( const LHCb::LHCbID& id, 
                                  const LHCb::ZTrajectory& reftraj,  bool localY ) const ;
  double nominalZ( const LHCb::LHCbID& id ) const ;
  
  void addToMeasurements( const std::vector<LHCb::LHCbID>& lhcbids,
                          std::vector<LHCb::Measurement*>& measurements,
                          const LHCb::ZTrajectory& reftraj) const ;
  

  StatusCode load( LHCb::Track&  ) const {
    info() << "sorry, MeasurementProviderBase::load not implemented" << endreq ;
    return StatusCode::FAILURE ;
  }

private:

  const LHCb::VeloPixLiteCluster::VeloPixLiteClusters* clusters() const ;
  

private:
  // pointer to detector
  mutable DeVeloPix* m_det;
  ToolHandle<IVeloPixClusterPosition> m_positiontool ;
  std::string m_clusterLocation ;
  mutable LHCb::VeloPixLiteCluster::VeloPixLiteClusters* m_clusters;
} ;

//=============================================================================
// Declare to tool factory
//=============================================================================


DECLARE_TOOL_FACTORY( VeloPixLiteMeasurementProvider );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VeloPixLiteMeasurementProvider::VeloPixLiteMeasurementProvider( const std::string& type,
						const std::string& name,
						const IInterface* parent )
  :  GaudiTool( type, name , parent ),
     m_det(0),
     m_positiontool("VeloPixClusterPosition")
{
  declareInterface<IMeasurementProvider>(this);
  declareProperty("ClusterLocation",m_clusterLocation = LHCb::VeloPixLiteClusterLocation::Default ) ;
}

//-----------------------------------------------------------------------------
/// Initialize
//-----------------------------------------------------------------------------

StatusCode VeloPixLiteMeasurementProvider::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if( sc.isFailure() ) { return Error( "Failed to initialize!", sc ); }
  // retrieve the postion tool
  sc = m_positiontool.retrieve() ;
  if( sc.isFailure() ) { return Error( "Failed to initialize position tool!", sc ); }
  // Retrieve the detector element
  m_det = getDet<DeVeloPix>( "/dd/Structure/LHCb/BeforeMagnetRegion/VeloPix" ) ;

  // reset pointer to list of clusters at beginevent
  incSvc()->addListener(this, IncidentType::BeginEvent);
 
  return sc;
}

//-----------------------------------------------------------------------------
/// Handle a begin-event incidence: Make sure clusters are reloaded.
//-----------------------------------------------------------------------------

void VeloPixLiteMeasurementProvider::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) m_clusters = 0 ;
}

//-----------------------------------------------------------------------------
/// Load clusters from the TES
//-----------------------------------------------------------------------------

const LHCb::VeloPixLiteCluster::VeloPixLiteClusters* VeloPixLiteMeasurementProvider::clusters() const
{
  if( !m_clusters )
    m_clusters = get<LHCb::VeloPixLiteCluster::VeloPixLiteClusters>( m_clusterLocation );
  return m_clusters ;
}

//-----------------------------------------------------------------------------
/// Create a measurement
//-----------------------------------------------------------------------------

LHCb::Measurement* VeloPixLiteMeasurementProvider::measurement( const LHCb::LHCbID& id, bool localY ) const
{
  LHCb::Measurement* meas(0) ;
  if( !id.isVeloPix() ) {
    error() << "Not a VeloPix measurement" << endreq ;
  } else {
    const LHCb::VeloPixLiteCluster* clus = 
      clusters()->object( id.velopixID () );
    if (clus){
      LHCb::VeloPixLiteMeasurement::VeloPixLiteMeasurementType dir = localY ? 
        LHCb::VeloPixLiteMeasurement::Y : LHCb::VeloPixLiteMeasurement::X ;
      meas = new LHCb::VeloPixLiteMeasurement(  *m_det,*clus, *m_positiontool , dir);
    } else {
      error() << "Cannot find cluster for id " << id << endreq ;
    }
  }
  return meas ;
}
//-----------------------------------------------------------------------------
/// Create a measurement with ref vector
//-----------------------------------------------------------------------------


LHCb::Measurement* VeloPixLiteMeasurementProvider::measurement( const LHCb::LHCbID& id,  const LHCb::ZTrajectory& reftraj,
                                                                bool localY ) const
{
  LHCb::Measurement* meas(0) ;
  if( !id.isVeloPix() ) {
    error() << "Not a VeloPix measurement" << endreq ;
  } else {
    const LHCb::VeloPixLiteCluster* clus = 
      clusters()->object( id.velopixID () );
    
    if (clus){
      LHCb::VeloPixLiteMeasurement::VeloPixLiteMeasurementType dir = localY ? 
        LHCb::VeloPixLiteMeasurement::Y : LHCb::VeloPixLiteMeasurement::X ;
      double z = nominalZ(id) ;
      LHCb::StateVector refvector = reftraj.stateVector(z) ;
      meas = new LHCb::VeloPixLiteMeasurement(  *m_det, *clus,*m_positiontool, dir, refvector );
    } else {
      error() << "Cannot find cluster for id " << id << endreq ;
    }
  }
  return meas ;
}

//-----------------------------------------------------------------------------
// Return the z-coordinate of this lhcb-id (w/o creating the hit)
//-----------------------------------------------------------------------------

double VeloPixLiteMeasurementProvider::nominalZ( const LHCb::LHCbID& id ) const
{
  return m_det->squareSensor(id.velopixID().sensor())->z ()  ;
}

//-----------------------------------------------------------------------------
/// Create measurements for list of LHCbIDs
//-----------------------------------------------------------------------------

void VeloPixLiteMeasurementProvider::addToMeasurements( const std::vector<LHCb::LHCbID>& lhcbids,
                                                 std::vector<LHCb::Measurement*>& measurements,
                                                 const LHCb::ZTrajectory& reftraj) const
{
    
  debug()<<" VeloPixLiteMeasurementProvider makes 2 measurements for each LHCbID, one X one Y"<<endmsg;
  for( std::vector<LHCb::LHCbID>::const_iterator id = lhcbids.begin() ;
       id != lhcbids.end(); ++id) {
    measurements.push_back( measurement(*id,reftraj,false) )  ;
    measurements.push_back( measurement(*id,reftraj,true) )  ;
  }
}
