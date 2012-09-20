/** @class VPLiteMeasurementProvider VPLiteMeasurementProvider.cpp
 *
 * Implementation of VPLiteMeasurementProvider tool
 * see interface header for description
 *
 *  @author Victor Coco based on Wouter Hulsbergen + Stefania Vecchi MuonMeasurmentProvider
 *  @date   12/02/2010
 */
  
#include "TrackInterfaces/IMeasurementProvider.h"
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrackInterfaces/ITrackExtrapolator.h"
#include "Event/VPLiteMeasurement.h"
#include "Event/VPLiteCluster.h"
#include "Event/StateVector.h"
#include "VPDet/DeVP.h"
#include "GaudiKernel/ToolFactory.h"
#include "Event/TrackParameters.h"
#include "TrackInterfaces/IVPClusterPosition.h"
#include "GaudiKernel/IIncidentListener.h"

#include "Event/Measurement.h"
#include "TrackKernel/ZTrajectory.h"

#include "GaudiKernel/IIncidentSvc.h"

class VPLiteMeasurementProvider : public GaudiTool, virtual public IMeasurementProvider,
                             virtual public IIncidentListener
{
public:
  
  /// constructer
  VPLiteMeasurementProvider(const std::string& type,
                          const std::string& name,
                          const IInterface* parent);
  
  ~VPLiteMeasurementProvider() {}
  
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
    info() << "sorry, MeasurementProviderBase::load not implemented" << endmsg ;
    return StatusCode::FAILURE ;
  }

private:

  const LHCb::VPLiteCluster::VPLiteClusters* clusters() const ;
  

private:
  // pointer to detector
  mutable DeVP* m_det;
  ToolHandle<IVPClusterPosition> m_positiontool ;
  std::string m_clusterLocation ;
  mutable LHCb::VPLiteCluster::VPLiteClusters* m_clusters;
} ;

//=============================================================================
// Declare to tool factory
//=============================================================================


DECLARE_TOOL_FACTORY( VPLiteMeasurementProvider )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VPLiteMeasurementProvider::VPLiteMeasurementProvider( const std::string& type,
						const std::string& name,
						const IInterface* parent )
  :  GaudiTool( type, name , parent ),
     m_det(0),
     m_positiontool("VPClusterPosition")
{
  declareInterface<IMeasurementProvider>(this);
  declareProperty("ClusterLocation",m_clusterLocation = LHCb::VPLiteClusterLocation::Default ) ;
}

//-----------------------------------------------------------------------------
/// Initialize
//-----------------------------------------------------------------------------

StatusCode VPLiteMeasurementProvider::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if( sc.isFailure() ) { return Error( "Failed to initialize!", sc ); }
  // retrieve the postion tool
  sc = m_positiontool.retrieve() ;
  if( sc.isFailure() ) { return Error( "Failed to initialize position tool!", sc ); }
  // Retrieve the detector element
  m_det = getDet<DeVP>( "/dd/Structure/LHCb/BeforeMagnetRegion/VP" ) ;

  // reset pointer to list of clusters at beginevent
  incSvc()->addListener(this, IncidentType::BeginEvent);
 
  return sc;
}

//-----------------------------------------------------------------------------
/// Handle a begin-event incidence: Make sure clusters are reloaded.
//-----------------------------------------------------------------------------

void VPLiteMeasurementProvider::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) m_clusters = 0 ;
}

//-----------------------------------------------------------------------------
/// Load clusters from the TES
//-----------------------------------------------------------------------------

const LHCb::VPLiteCluster::VPLiteClusters* VPLiteMeasurementProvider::clusters() const
{
  if( !m_clusters )
    m_clusters = get<LHCb::VPLiteCluster::VPLiteClusters>( m_clusterLocation );
  return m_clusters ;
}

//-----------------------------------------------------------------------------
/// Create a measurement
//-----------------------------------------------------------------------------

LHCb::Measurement* VPLiteMeasurementProvider::measurement( const LHCb::LHCbID& id, bool localY ) const
{
  LHCb::Measurement* meas(0) ;
  if( !id.isVP() ) {
    error() << "Not a VP measurement" << endmsg ;
  } else {
    const LHCb::VPLiteCluster* clus = 
      clusters()->object( id.vpID () );
    if (clus){
      LHCb::VPLiteMeasurement::VPLiteMeasurementType dir = localY ? 
        LHCb::VPLiteMeasurement::Y : LHCb::VPLiteMeasurement::X ;
      meas = new LHCb::VPLiteMeasurement(  *m_det,*clus, *m_positiontool , dir);
    } else {
      error() << "Cannot find cluster for id " << id << endmsg ;
    }
  }
  return meas ;
}
//-----------------------------------------------------------------------------
/// Create a measurement with ref vector
//-----------------------------------------------------------------------------


LHCb::Measurement* VPLiteMeasurementProvider::measurement( const LHCb::LHCbID& id,  const LHCb::ZTrajectory& reftraj,
                                                                bool localY ) const
{
  LHCb::Measurement* meas(0) ;
  if( !id.isVP() ) {
    error() << "Not a VP measurement" << endmsg ;
  } else {
    const LHCb::VPLiteCluster* clus = 
      clusters()->object( id.vpID () );
    
    if (clus){
      LHCb::VPLiteMeasurement::VPLiteMeasurementType dir = localY ? 
        LHCb::VPLiteMeasurement::Y : LHCb::VPLiteMeasurement::X ;
      double z = nominalZ(id) ;
      LHCb::StateVector refvector = reftraj.stateVector(z) ;
      meas = new LHCb::VPLiteMeasurement(  *m_det, *clus,*m_positiontool, dir, refvector );
    } else {
      error() << "Cannot find cluster for id " << id << endmsg ;
    }
  }
  return meas ;
}

//-----------------------------------------------------------------------------
// Return the z-coordinate of this lhcb-id (w/o creating the hit)
//-----------------------------------------------------------------------------

double VPLiteMeasurementProvider::nominalZ( const LHCb::LHCbID& id ) const
{
  return m_det->squareSensor(id.vpID().sensor())->z ()  ;
}

//-----------------------------------------------------------------------------
/// Create measurements for list of LHCbIDs
//-----------------------------------------------------------------------------

void VPLiteMeasurementProvider::addToMeasurements( const std::vector<LHCb::LHCbID>& lhcbids,
                                                 std::vector<LHCb::Measurement*>& measurements,
                                                 const LHCb::ZTrajectory& reftraj) const
{
    
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug()<<" VPLiteMeasurementProvider makes 2 measurements for each LHCbID, one X one Y"<<endmsg;
  for( std::vector<LHCb::LHCbID>::const_iterator id = lhcbids.begin() ;
       id != lhcbids.end(); ++id) {
    measurements.push_back( measurement(*id,reftraj,false) )  ;
    measurements.push_back( measurement(*id,reftraj,true) )  ;
  }
}
