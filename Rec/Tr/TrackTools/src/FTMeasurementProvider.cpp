/** @class FTMeasurementProvider FTMeasurementProvider.cpp
 *
 * Implementation of FTMeasurementProvider
 * see interface header for description
 *
 *  @author Wouter Hulsbergen
 *  @date   30/12/2005
 */
  
#include "TrackInterfaces/IMeasurementProvider.h"
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"

#include "TrackInterfaces/ITrackExtrapolator.h"
#include "Event/FTMeasurement.h"
#include "Event/StateVector.h"
#include "FTDet/DeFTDetector.h"
#include "Event/TrackParameters.h"
#include "TrackKernel/TrackTraj.h"
#include "Event/FTLiteCluster.h"

class FTMeasurementProvider : public GaudiTool, virtual public IMeasurementProvider,
                              virtual public IIncidentListener
{
public:
  
  /// constructer
  FTMeasurementProvider(const std::string& type,
			  const std::string& name,
			  const IInterface* parent);
  
  ~FTMeasurementProvider() {}
  
  StatusCode initialize() ;
  StatusCode finalize() ;
  LHCb::Measurement* measurement( const LHCb::LHCbID& id, bool localY=false ) const  ;
  LHCb::Measurement* measurement( const LHCb::LHCbID& id, const LHCb::ZTrajectory& refvector, bool localY=false) const ;
  double nominalZ( const LHCb::LHCbID& id ) const ;
  inline LHCb::FTMeasurement* ftmeasurement( const LHCb::LHCbID& id ) const  ;

  void handle ( const Incident& incident );
  const FastClusterContainer<LHCb::FTLiteCluster,int>* clusters() const;

  void addToMeasurements( const std::vector<LHCb::LHCbID>& lhcbids,
                          std::vector<LHCb::Measurement*>& measurements,
                          const LHCb::ZTrajectory& reftraj) const ;

  StatusCode load( LHCb::Track&  ) const {
    return Error( "sorry, MeasurementProviderBase::load not implemented" );
  }


private:
  const DeFTDetector* m_det;
  mutable FastClusterContainer<LHCb::FTLiteCluster,int>* m_clusters;
} ;

//=============================================================================
// Declare to tool factory
//=============================================================================

DECLARE_TOOL_FACTORY( FTMeasurementProvider )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FTMeasurementProvider::FTMeasurementProvider( const std::string& type,
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

StatusCode FTMeasurementProvider::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if( sc.isFailure() ) { return Error( "Failed to initialize!", sc ); }

  // Retrieve the detector element
  m_det = getDet<DeFTDetector>( DeFTDetectorLocation::Default ) ;

  // reset pointer to list of clusters at beginevent
  incSvc()->addListener(this, IncidentType::BeginEvent);
 
  return sc;
}

//-----------------------------------------------------------------------------
/// Handle a begin-event incidence: Make sure clusters are reloaded.
//-----------------------------------------------------------------------------

void FTMeasurementProvider::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) m_clusters = nullptr ;
}

//-----------------------------------------------------------------------------
/// Load clusters from the TES
//-----------------------------------------------------------------------------

const FastClusterContainer<LHCb::FTLiteCluster,int>* FTMeasurementProvider::clusters() const
{
  /// If there is a new event, get the clusters and sort them according to their channel ID
  if( m_clusters == nullptr ){
    m_clusters = getIfExists<FastClusterContainer<LHCb::FTLiteCluster,int> >( LHCb::FTLiteClusterLocation::Default );
    if(m_clusters != nullptr){
      std::sort(m_clusters->begin(), m_clusters->end(), 
                [](const LHCb::FTLiteCluster& lhs, const LHCb::FTLiteCluster& rhs){ return lhs.channelID() < rhs.channelID(); });
    }else{
      error() << "Could not find FTLiteClusters at: " <<  LHCb::FTLiteClusterLocation::Default << endmsg;
    }
    
    
  }
  
  return m_clusters ;
}

//-----------------------------------------------------------------------------
/// Finalize
//-----------------------------------------------------------------------------

StatusCode FTMeasurementProvider::finalize() {
  return GaudiTool::finalize();
}

//-----------------------------------------------------------------------------
/// Create a measurement
//-----------------------------------------------------------------------------
LHCb::FTMeasurement* FTMeasurementProvider::ftmeasurement( const LHCb::LHCbID& id ) const {
  LHCb::FTMeasurement* meas(nullptr) ;
  
  if( !id.isFT() ) {
    error() << "Not an FT measurement" << endmsg ;
  } else {
    
    LHCb::FTChannelID ftID = id.ftID() ;
    /// The clusters are sorted, so we can use a binary search (lower bound search) to find the element
    /// corresponding to the channel ID
    FastClusterContainer<LHCb::FTLiteCluster,int>::const_iterator itH = 
      std::lower_bound( clusters()->begin(),  clusters()->end(), ftID, 
                        [](const LHCb::FTLiteCluster clus, const LHCb::FTChannelID id){  
                          return clus.channelID() < id;
                        });
    
    if( itH != clusters()->end() ) meas = new LHCb::FTMeasurement( (*itH), *m_det ) ;
  }
  return meas ;
}


//-----------------------------------------------------------------------------
/// Return the measurement
//-----------------------------------------------------------------------------
LHCb::Measurement* FTMeasurementProvider::measurement( const LHCb::LHCbID& id, bool /*localY*/ ) const {
  return ftmeasurement(id) ;
}

//-----------------------------------------------------------------------------
/// Create a measurement with statevector. For now very inefficient.
//-----------------------------------------------------------------------------

LHCb::Measurement* FTMeasurementProvider::measurement( const LHCb::LHCbID& id, 
                                                       const LHCb::ZTrajectory& /* reftraj */, bool /*localY*/ ) const {
  // default implementation
  LHCb::FTMeasurement* meas = ftmeasurement(id) ;
  return meas ;
}

//-----------------------------------------------------------------------------
// Return the z-coordinate of this lhcb-id (w/o creating the hit)
//-----------------------------------------------------------------------------

double FTMeasurementProvider::nominalZ( const LHCb::LHCbID& id ) const {
  double z(0) ;
  LHCb::FTChannelID ftid = id.ftID() ;

  const DeFTFibreMat* fibreMat = m_det->findFibreMat(ftid);
  if ( nullptr == fibreMat ) {
    error() << "Cannot find FT fibreMat for ID = " << ftid << endmsg ;
  } else {
    z = fibreMat->layerCenterZ() ;
  }
  
  return z ;
}

//-----------------------------------------------------------------------------
/// Create measurements for list of LHCbIDs
//-----------------------------------------------------------------------------

void FTMeasurementProvider::addToMeasurements( const std::vector<LHCb::LHCbID>& lhcbids,
                                               std::vector<LHCb::Measurement*>& measurements,
                                               const LHCb::ZTrajectory& reftraj) const
{
  for( std::vector<LHCb::LHCbID>::const_iterator id = lhcbids.begin() ;
       id != lhcbids.end(); ++id)
    measurements.push_back( measurement(*id,reftraj,false) ) ;
}



