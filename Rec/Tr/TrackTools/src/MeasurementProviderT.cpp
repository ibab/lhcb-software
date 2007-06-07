// $Id: MeasurementProviderT.cpp,v 1.1 2007-06-07 08:57:17 wouter Exp $
// Include files

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================

/** @class MeasurementProviderT MeasurementProviderT.cpp
 *
 * Implementation of templated MeasurementProvider tool
 * see interface header for description
 *
 *  @author W. Hulsbergen
 *  @date   07/06/2007
 */
  
#include "TrackInterfaces/IMeasurementProvider.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IIncidentListener.h"

template <typename T>
class MeasurementProviderT : public GaudiTool, 
			     virtual public IIncidentListener,
			     virtual public IMeasurementProvider
{
public:
  
  /// constructer
  MeasurementProviderT(const std::string& type,
			  const std::string& name,
			  const IInterface* parent);
  
  virtual ~MeasurementProviderT() {}
             
  virtual StatusCode initialize() ;
  virtual void handle(const Incident&) ;
  virtual StatusCode load( LHCb::Track& track ) const ;
  virtual LHCb::Measurement* measurement( const LHCb::LHCbID& id ) const  ;
  virtual LHCb::Measurement* measurement( const LHCb::LHCbID& id, const LHCb::StateVector& refvector ) const ;
  virtual StatusCode update(  LHCb::Measurement&, const LHCb::StateVector& refvector ) const ;
  virtual double nominalZ( const LHCb::LHCbID& id ) const  ;

private:
  const typename T::ClusterContainerType* clusters() const ;
  LHCb::Measurement* createUnchecked( const typename T::ClusterType& cluster ) const  ;
  void updateUnchecked( typename T::MeasurementType& meas, const LHCb::StateVector& refvector ) const ;

private:
  
  // tools
  ToolHandle<typename T::PositionToolType> m_positiontool ;
  ToolHandle<ITrackExtrapolator>           m_extrapolator ; ///< Extrapolator tool

  // pointer to detector
  const typename T::DetectorType* m_det;
  
  // this one will always be mutable
  mutable typename T::ClusterContainerType* m_clusters;
} ;


// local
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ToolFactory.h"
#include "Event/StateVector.h"
#include "Event/Measurement.h"
#include "TrackInterfaces/ITrackExtrapolator.h"

//using namespace LHCb;

template<typename T>
MeasurementProviderT<T>::MeasurementProviderT( const std::string& type,
					       const std::string& name,
					       const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_positiontool(T::positionToolName()),
    m_extrapolator("TrackMasterExtrapolator"),
    m_det(0)
{
  declareInterface<IMeasurementProvider>(this);
}

//-----------------------------------------------------------------------------
/// Initialize
//-----------------------------------------------------------------------------

template <typename T>
StatusCode MeasurementProviderT<T>::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if( sc.isFailure() ) { return Error( "Failed to initialize!", sc ); }

  sc = m_positiontool.retrieve() ;
  if( sc.isFailure() ) { return Error( "Failed to initialize position tool!", sc ); }
  
  sc = m_extrapolator.retrieve() ;
  if( sc.isFailure() ) { return Error( "Failed to initialize extrapolator!", sc ); }
  
  // Retrieve the detector element
  m_det = getDet<typename T::DetectorType>( T::defaultDetectorLocation() );

  // reset pointer to list of clusters at beginevent
  incSvc()->addListener(this, IncidentType::BeginEvent);
 
  return sc;
}

//-----------------------------------------------------------------------------
/// Handle a begin-event incidence: Make sure clusters are reloaded.
//-----------------------------------------------------------------------------

template <typename T>
void MeasurementProviderT<T>::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) m_clusters = 0 ;
}

//-----------------------------------------------------------------------------
/// Load clusters from the TES
//-----------------------------------------------------------------------------

template <typename T>
const typename T::ClusterContainerType* MeasurementProviderT<T>::clusters() const
{
  if( !m_clusters ) {
    if ( exist<typename T::ClusterContainerType>( T::defaultClusterLocation() ) ) {
      m_clusters = get<typename T::ClusterContainerType>( T::defaultClusterLocation() );
    }  
    else
      error() << T::defaultClusterLocation() << " asked to be loaded but not present!" << endreq;
  }
  return m_clusters ;
}

//-----------------------------------------------------------------------------
/// Create a measurement
//-----------------------------------------------------------------------------

template <typename T>
LHCb::Measurement* MeasurementProviderT<T>::createUnchecked( const typename T::ClusterType& cluster ) const
{
  return new typename T::MeasurementType( cluster, *m_det, *m_positiontool );
}

template <typename T>
LHCb::Measurement* MeasurementProviderT<T>::measurement( const LHCb::LHCbID& id ) const
{
  LHCb::Measurement* meas(0) ;
  if ( !T::checkType(id) ) {
    error() << "Not a velo measurement" << endreq ;
  } else {
    const typename T::ClusterType* clus = clusters()->object( T::channelId(id) );
    if (clus) {
      meas = createUnchecked(*clus) ;
    } else {
      error() << "Cannot find cluster for id " << id << endreq ;
    }
  }
  return meas ;
}

//-----------------------------------------------------------------------------
/// Create a measurement with statevector. For now very inefficient.
//-----------------------------------------------------------------------------

template <typename T>
LHCb::Measurement* MeasurementProviderT<T>::measurement( const LHCb::LHCbID& id, 
							 const LHCb::StateVector& refvector ) const 
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

template <typename T>
void MeasurementProviderT<T>::updateUnchecked( typename T::MeasurementType& meas,
					       const LHCb::StateVector& refvector) const
{
  meas.setRefVector( refvector.parameters() ) ;
  meas.init(  *(meas.cluster()), *m_det, *m_positiontool, true ) ;
}

template <typename T>
StatusCode MeasurementProviderT<T>::update( LHCb::Measurement& ameas, 
					    const LHCb::StateVector& refvector) const
{
  StatusCode sc = StatusCode::SUCCESS ;
  typename T::MeasurementType* meas = dynamic_cast<typename T::MeasurementType*>(&ameas) ;
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
    updateUnchecked(*meas,refvectoratz) ;
  }
  return sc ;
}

//-----------------------------------------------------------------------------
// Return the z-coordinate of this lhcb-id (w/o creating the hit)
//-----------------------------------------------------------------------------

template <typename T>
double MeasurementProviderT<T>::nominalZ( const LHCb::LHCbID& id ) const
{
  std::auto_ptr<LHCb::Trajectory> traj = const_cast<typename T::DetectorType*>(m_det)->trajectory(id,0) ;
  return traj->position(0.5*(traj->beginRange()+traj->endRange())).z() ;
}

//-----------------------------------------------------------------------------
// Load all hits of this type on the track
//-----------------------------------------------------------------------------

template <typename T>
StatusCode MeasurementProviderT<T>::load( LHCb::Track& ) const 
{
  info() << "Sorry, ::load(track), not implemented for " << name() << endreq ;
  return StatusCode::FAILURE ;
}

////////////////////////////////////////////////////////////////////////////////////////
// Template instantiations using Traight classes
////////////////////////////////////////////////////////////////////////////////////////

#include "VeloDet/DeVelo.h"
#include "Event/VeloCluster.h"
#include "Event/VeloRMeasurement.h"
#include "Kernel/IVeloClusterPosition.h"

namespace MeasurementProviderTypes {
  struct VeloR {
    typedef LHCb::VeloRMeasurement   MeasurementType ;
    typedef IVeloClusterPosition     PositionToolType ;
    typedef LHCb::VeloCluster        ClusterType ;
    typedef LHCb::VeloClusters       ClusterContainerType ;
    typedef DeVelo                   DetectorType ;
    static std::string positionToolName() { return "VeloClusterPosition" ; }
    static std::string defaultDetectorLocation() { return DeVeloLocation::Default ; }
    static std::string defaultClusterLocation() { return LHCb::VeloClusterLocation::Default ; }
    static LHCb::VeloChannelID channelId( const LHCb::LHCbID& id ) { return id.veloID(); }
    static bool checkType(const LHCb::LHCbID& id) { return id.isVelo() && channelId(id).isRType() ; }
  };
}

typedef MeasurementProviderT<MeasurementProviderTypes::VeloR> VeloRMeasurementProvider ;
DECLARE_TOOL_FACTORY( VeloRMeasurementProvider );

////////////////////////////////////////////////////////////////////////////////////////

#include "Event/VeloPhiMeasurement.h"

namespace MeasurementProviderTypes {
  struct VeloPhi {
    typedef LHCb::VeloPhiMeasurement MeasurementType ;
    typedef IVeloClusterPosition     PositionToolType ;
    typedef LHCb::VeloCluster        ClusterType ;
    typedef LHCb::VeloClusters       ClusterContainerType ;
    typedef DeVelo                   DetectorType ;
    static std::string positionToolName() { return "VeloClusterPosition" ; }
    static std::string defaultDetectorLocation() { return DeVeloLocation::Default ; }
    static std::string defaultClusterLocation() { return LHCb::VeloClusterLocation::Default ; }
    static LHCb::VeloChannelID channelId( const LHCb::LHCbID& id ) { return id.veloID(); }
    static bool checkType(const LHCb::LHCbID& id) { return id.isVelo() && channelId(id).isPhiType() ; }
  };
}

typedef MeasurementProviderT<MeasurementProviderTypes::VeloPhi> VeloPhiMeasurementProvider ;
DECLARE_TOOL_FACTORY( VeloPhiMeasurementProvider );

////////////////////////////////////////////////////////////////////////////////////////

#include "Event/STCluster.h"
#include "Event/STMeasurement.h"
#include "Kernel/ISTClusterPosition.h"
#include "STDet/DeSTDetector.h"

namespace MeasurementProviderTypes {
  struct TT {
    typedef LHCb::STMeasurement      MeasurementType ;
    typedef ISTClusterPosition       PositionToolType ;
    typedef LHCb::STCluster          ClusterType ;
    typedef LHCb::STClusters         ClusterContainerType ;
    typedef DeSTDetector             DetectorType ;
    static std::string positionToolName() { return "STOfflinePosition" ; }
    static std::string defaultDetectorLocation() { return DeSTDetLocation::location("TT") ; }
    static std::string defaultClusterLocation() { return LHCb::STClusterLocation::TTClusters ; }
    static LHCb::STChannelID channelId( const LHCb::LHCbID& id ) { return id.stID() ; }
    static bool checkType(const LHCb::LHCbID& id) { return id.isTT() ; }
  };
}

typedef MeasurementProviderT<MeasurementProviderTypes::TT> TTMeasurementProvider ;
DECLARE_TOOL_FACTORY( TTMeasurementProvider );

////////////////////////////////////////////////////////////////////////////////////////

namespace MeasurementProviderTypes {
  struct IT {
    typedef LHCb::STMeasurement      MeasurementType ;
    typedef ISTClusterPosition       PositionToolType ;
    typedef LHCb::STCluster          ClusterType ;
    typedef LHCb::STClusters         ClusterContainerType ;
    typedef DeSTDetector             DetectorType ;
    static std::string positionToolName() { return "STOfflinePosition/ITClusterPosition" ; }
    static std::string defaultDetectorLocation() { return DeSTDetLocation::location("IT") ; }
    static std::string defaultClusterLocation() { return LHCb::STClusterLocation::ITClusters ; }
    static LHCb::STChannelID channelId( const LHCb::LHCbID& id ) { return id.stID() ; }
    static bool checkType(const LHCb::LHCbID& id) { return id.isIT() ; }
  };
}

typedef MeasurementProviderT<MeasurementProviderTypes::IT> ITMeasurementProvider ;
DECLARE_TOOL_FACTORY( ITMeasurementProvider );

////////////////////////////////////////////////////////////////////////////////////////
// OTMeasurementProviderT
//
// Requires two template instantiations, because interface is slightly
// different. Also a bit ugly that this has a member
// 'velopositiontool'
////////////////////////////////////////////////////////////////////////////////////////

#include "Event/OTTime.h"
#include "Event/OTMeasurement.h"
#include "OTDet/DeOTDetector.h"

namespace MeasurementProviderTypes {
  struct OT  {
    typedef LHCb::OTMeasurement      MeasurementType ;
    // there is no OT position tool. we'll just use the velo one.
    typedef IVeloClusterPosition     PositionToolType ;
    typedef LHCb::OTTime             ClusterType ;
    typedef LHCb::OTTimes            ClusterContainerType ;
    typedef DeOTDetector             DetectorType ;
    static std::string positionToolName() { return "VeloClusterPosition" ; }
    static std::string defaultDetectorLocation() { return DeOTDetectorLocation::Default ; }
    static std::string defaultClusterLocation() { return LHCb::OTTimeLocation::Default ; }
    static LHCb::OTChannelID  channelId( const LHCb::LHCbID& id ) { return id.otID(); }
    static bool checkType(const LHCb::LHCbID& id) { return id.isOT() ; }
  };
}

LHCb::Measurement* MeasurementProviderT<MeasurementProviderTypes::OT>::
createUnchecked( const MeasurementProviderTypes::OT::ClusterType& cluster ) const
{
  return new MeasurementProviderTypes::OT::MeasurementType( cluster, *m_det, 0 );
}

void MeasurementProviderT<MeasurementProviderTypes::OT>::
updateUnchecked( MeasurementProviderTypes::OT::MeasurementType& meas,
		 const LHCb::StateVector& refvector) const
{
  meas.setRefVector( refvector.parameters() ) ;
  meas.init(  *(meas.time()), *m_det, 0, 0 ) ;
}

typedef MeasurementProviderT<MeasurementProviderTypes::OT> OTMeasurementProvider ;
DECLARE_TOOL_FACTORY( OTMeasurementProvider );

