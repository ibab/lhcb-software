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
#include "GaudiKernel/ToolFactory.h"
#include "Event/TrackParameters.h"
#include "TrackKernel/TrackTraj.h"
#include "Event/FTRawCluster.h"

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
  const FastClusterContainer<LHCb::FTRawCluster,int>* clusters() const;

  void addToMeasurements( const std::vector<LHCb::LHCbID>& lhcbids,
                          std::vector<LHCb::Measurement*>& measurements,
                          const LHCb::ZTrajectory& reftraj) const ;

  StatusCode load( LHCb::Track&  ) const {
    return Error( "sorry, MeasurementProviderBase::load not implemented" );
  }


private:
  const DeFTDetector* m_det;
  mutable FastClusterContainer<LHCb::FTRawCluster,int>* m_clusters;
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
  if ( IncidentType::BeginEvent == incident.type() ) m_clusters = 0 ;
}

//-----------------------------------------------------------------------------
/// Load clusters from the TES
//-----------------------------------------------------------------------------

const FastClusterContainer<LHCb::FTRawCluster,int>* FTMeasurementProvider::clusters() const
{
  if( !m_clusters )
    m_clusters = get<FastClusterContainer<LHCb::FTRawCluster,int> >( LHCb::FTRawClusterLocation::Default );
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

inline LHCb::FTMeasurement* FTMeasurementProvider::ftmeasurement( const LHCb::LHCbID& id ) const {
  LHCb::FTMeasurement* meas(0) ;
  if( !id.isFT() ) {
    error() << "Not an FT measurement" << endmsg ;
  } else {
    LHCb::FTChannelID ftid = id.ftID() ;
    for ( FastClusterContainer<LHCb::FTRawCluster,int>::const_iterator itH = clusters()->begin();
          clusters()->end() != itH; ++itH ) {
      if ( (*itH).channelID() == ftid ) {
        meas = new LHCb::FTMeasurement( (*itH), *m_det ) ;
        break;
      }
    }
  }
  return meas ;
}

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

  if(m_det->version() != 20) { 
    const DeFTLayer* layer = m_det->findLayer( ftid ) ;
    if ( NULL == layer ) {
      error() << "Cannot find FT layer for ID = " << ftid << endmsg ;
    } else {
      z = layer->layerCenterZ();
    }
  } else {
   const DeFTFibreMat* fibreMat = m_det->findFibreMat(ftid);
    if ( NULL == fibreMat ) {
      error() << "Cannot find FT fibreMat for ID = " << ftid << endmsg ;
    } else {
      z = fibreMat->layerCenterZ() ;
    }
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



