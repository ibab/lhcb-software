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
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "Event/TrackParameters.h"

template <typename T>
class MeasurementProviderT : public GaudiTool,
                             virtual public IMeasurementProvider,
                             virtual public IIncidentListener
{
public:
  
  /// constructer
  MeasurementProviderT(const std::string& type,
			  const std::string& name,
			  const IInterface* parent);
  
  virtual ~MeasurementProviderT() {}
             
  virtual StatusCode initialize() ;
  virtual void handle(const Incident&) ;
  virtual LHCb::Measurement* measurement( const LHCb::LHCbID& id, bool /*localY*/ ) const  ;
  virtual LHCb::Measurement* measurement( const LHCb::LHCbID& id, const LHCb::ZTrajectory& refvector,
                                          bool /*localY*/  ) const ; 
  virtual void addToMeasurements( const std::vector<LHCb::LHCbID>& lhcbids,
                                  std::vector<LHCb::Measurement*>& measurements,
                                  const LHCb::ZTrajectory& reftraj) const ;
  virtual double nominalZ( const LHCb::LHCbID& id ) const  ;

  StatusCode load( LHCb::Track&  ) const {
    info() << "sorry, MeasurementProviderBase::load not implemented" << endmsg ;
    return StatusCode::FAILURE ;
  }

private:
  const typename T::ClusterContainerType* clusters() const ;

private:
  bool m_useReference ;
  std::string m_clusterLocation ;
  ToolHandle<typename T::PositionToolType> m_positiontool ;
  const typename T::DetectorType* m_det;
  mutable typename T::ClusterContainerType* m_clusters;
} ;


// local
#include "GaudiKernel/IIncidentSvc.h"
#include "Event/StateVector.h"
#include "Event/Measurement.h"
#include "TrackKernel/ZTrajectory.h"

//using namespace LHCb;

template<typename T>
MeasurementProviderT<T>::MeasurementProviderT( const std::string& type,
                                               const std::string& name,
                                               const IInterface* parent )
  :  GaudiTool( type, name , parent ),
     m_positiontool(T::positionToolName()),
     m_det(0), m_clusters(0)
{
  declareInterface<IMeasurementProvider>(this);
  declareProperty( "UseReference", m_useReference = true );
  declareProperty( "ClusterLocation", m_clusterLocation = T::defaultClusterLocation() ) ;
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
  
  // make damn sure m_clusters is initialised properly
  m_clusters = 0;
  // Retrieve the detector element
  m_det = getDet<typename T::DetectorType>( T::defaultDetectorLocation() );

  // reset pointer to list of clusters at end of event (that is the point in
  // time at which the pointee is supposed to become invalid)
  incSvc()->addListener(this, IncidentType::EndEvent);
 
  return sc;
}

//-----------------------------------------------------------------------------
/// Handle a end-event incidence: Make sure clusters are reloaded next time.
//-----------------------------------------------------------------------------

template <typename T>
void MeasurementProviderT<T>::handle ( const Incident& incident )
{
  if ( IncidentType::EndEvent == incident.type() ) m_clusters = 0 ;
}

//-----------------------------------------------------------------------------
/// Load clusters from the TES
//-----------------------------------------------------------------------------

template <typename T>
const typename T::ClusterContainerType* MeasurementProviderT<T>::clusters() const
{
  if( !m_clusters )
    m_clusters = get<typename T::ClusterContainerType>( m_clusterLocation );
  return m_clusters ;
}

//-----------------------------------------------------------------------------
/// Create a measurement
//-----------------------------------------------------------------------------

template <typename T>
LHCb::Measurement* MeasurementProviderT<T>::measurement( const LHCb::LHCbID& id, bool ) const
{
  LHCb::Measurement* meas(0) ;
  if ( !T::checkType(id) ) {
    error() << "Not a velo measurement" << endmsg ;
  } else {
    const typename T::ClusterType* clus = clusters()->object( T::channelId(id) );
    if (clus) {
      meas = new typename T::MeasurementType( *clus, *m_det, *m_positiontool );
    } else {
      error() << "Cannot find cluster for id " << id << endmsg ;
    }
  }
  return meas ;
}

//-----------------------------------------------------------------------------
/// Create a measurement with statevector. For now very inefficient.
//-----------------------------------------------------------------------------

template <typename T>
LHCb::Measurement* MeasurementProviderT<T>::measurement( const LHCb::LHCbID& id, 
                                                         const LHCb::ZTrajectory& reftraj, 
                                                         bool localY ) const 
{
  LHCb::Measurement* meas(0) ;
  if( !m_useReference ) {
    meas = measurement( id, localY ) ;
  } else {
    if ( !T::checkType(id) ) {
      error() << "Not a velo measurement" << endmsg ;
    } else {
      const typename T::ClusterType* clus = clusters()->object( T::channelId(id) );
      if (clus) {
        double z = nominalZ(id) ;
        LHCb::StateVector refvector = reftraj.stateVector(z) ;
        meas = new typename T::MeasurementType( *clus, *m_det, *m_positiontool, refvector);
      } else {
        error() << "Cannot find cluster for id " << id << endmsg ;
      }
    }
  }
  return meas ;
}


//-----------------------------------------------------------------------------
/// Create a measurement with statevector. For now very inefficient.
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
/// Create measurements for list of LHCbIDs
//-----------------------------------------------------------------------------

template <typename T>
void MeasurementProviderT<T>::addToMeasurements( const std::vector<LHCb::LHCbID>& lhcbids,
                                                 std::vector<LHCb::Measurement*>& measurements,
                                                 const LHCb::ZTrajectory& reftraj) const
{
  for( std::vector<LHCb::LHCbID>::const_iterator id = lhcbids.begin() ;
       id != lhcbids.end(); ++id)
    measurements.push_back( measurement(*id,reftraj,false) ) ;
}


////////////////////////////////////////////////////////////////////////////////////////
// Template instantiations using Traight classes
////////////////////////////////////////////////////////////////////////////////////////

#include "VeloDet/DeVelo.h"
#include "Event/VeloCluster.h"
#include "Event/VeloLiteCluster.h"
#include "Event/VeloRMeasurement.h"
#include "Event/VeloLiteRMeasurement.h"
#include "TrackInterfaces/IVeloClusterPosition.h"

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

  struct VeloLiteR {
    typedef LHCb::VeloLiteRMeasurement   MeasurementType ;
    typedef IVeloClusterPosition         PositionToolType ;
    typedef LHCb::VeloLiteCluster        ClusterType ;
    typedef LHCb::VeloLiteCluster::VeloLiteClusters   ClusterContainerType ;
    typedef DeVelo                       DetectorType ;
    static std::string positionToolName() { return "VeloClusterPosition" ; }
    static std::string defaultDetectorLocation() { return DeVeloLocation::Default ; }
    static std::string defaultClusterLocation() { return LHCb::VeloLiteClusterLocation::Default ; }
    static LHCb::VeloChannelID channelId( const LHCb::LHCbID& id ) { return id.veloID(); }
    static bool checkType(const LHCb::LHCbID& id) { return id.isVelo() && channelId(id).isRType() ; }
  };

}

template<>
double MeasurementProviderT<MeasurementProviderTypes::VeloR>::nominalZ( const LHCb::LHCbID& id ) const
{
  return m_det->rSensor( id.veloID() )->z() ;
}

typedef MeasurementProviderT<MeasurementProviderTypes::VeloR> VeloRMeasurementProvider ;
DECLARE_TOOL_FACTORY( VeloRMeasurementProvider )

template<>
double MeasurementProviderT<MeasurementProviderTypes::VeloLiteR>::nominalZ( const LHCb::LHCbID& id ) const
{
  return m_det->rSensor( id.veloID() )->z() ;
}

typedef MeasurementProviderT<MeasurementProviderTypes::VeloLiteR> VeloLiteRMeasurementProvider ;
DECLARE_TOOL_FACTORY( VeloLiteRMeasurementProvider )


////////////////////////////////////////////////////////////////////////////////////////

#include "Event/VeloPhiMeasurement.h"
#include "Event/VeloLitePhiMeasurement.h"

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

  struct VeloLitePhi {
    typedef LHCb::VeloLitePhiMeasurement MeasurementType ;
    typedef IVeloClusterPosition         PositionToolType ;
    typedef LHCb::VeloLiteCluster        ClusterType ;
    typedef LHCb::VeloLiteCluster::VeloLiteClusters ClusterContainerType ;
    typedef DeVelo                       DetectorType ;
    static std::string positionToolName() { return "VeloClusterPosition" ; }
    static std::string defaultDetectorLocation() { return DeVeloLocation::Default ; }
    static std::string defaultClusterLocation() { return LHCb::VeloLiteClusterLocation::Default ; }
    static LHCb::VeloChannelID channelId( const LHCb::LHCbID& id ) { return id.veloID(); }
    static bool checkType(const LHCb::LHCbID& id) { return id.isVelo() && channelId(id).isPhiType() ; }
  };

}

template<>
double MeasurementProviderT<MeasurementProviderTypes::VeloPhi>::nominalZ( const LHCb::LHCbID& id ) const
{
  return m_det->phiSensor( id.veloID() )->z() ;
}

typedef MeasurementProviderT<MeasurementProviderTypes::VeloPhi> VeloPhiMeasurementProvider ;
DECLARE_TOOL_FACTORY( VeloPhiMeasurementProvider )

template<>
double MeasurementProviderT<MeasurementProviderTypes::VeloLitePhi>::nominalZ( const LHCb::LHCbID& id ) const
{
  return m_det->phiSensor( id.veloID() )->z() ;
}

typedef MeasurementProviderT<MeasurementProviderTypes::VeloLitePhi> VeloLitePhiMeasurementProvider ;
DECLARE_TOOL_FACTORY( VeloLitePhiMeasurementProvider )

////////////////////////////////////////////////////////////////////////////////////////

#include "Event/STCluster.h"
#include "Event/STMeasurement.h"
#include "Event/STLiteMeasurement.h"
#include "TrackInterfaces/ISTClusterPosition.h"
#include "STDet/DeSTDetector.h"
#include "Event/STLiteCluster.h"

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

template<>
double MeasurementProviderT<MeasurementProviderTypes::TT>::nominalZ( const LHCb::LHCbID& id ) const
{
  LHCb::STChannelID stid(id.stID()) ;
  const DeSTSector* sector = const_cast<DeSTDetector*>(m_det)->findSector(stid) ;
  return sector->globalCentre().z();
}

typedef MeasurementProviderT<MeasurementProviderTypes::TT> TTMeasurementProvider ;
DECLARE_TOOL_FACTORY( TTMeasurementProvider )

////////////////////////////////////////////////////////////////////////////////////////

namespace MeasurementProviderTypes {
  struct TTLite {
    typedef LHCb::STLiteMeasurement      MeasurementType ;
    typedef ISTClusterPosition       PositionToolType ;
    typedef LHCb::STLiteCluster          ClusterType ;
    typedef LHCb::STLiteCluster::STLiteClusters  ClusterContainerType ;
    typedef DeSTDetector             DetectorType ;
    static std::string positionToolName() { return "STOnlinePosition" ; }
    static std::string defaultDetectorLocation() { return DeSTDetLocation::location("TT") ; }
    static std::string defaultClusterLocation() { return LHCb::STLiteClusterLocation::TTClusters ; }
    static LHCb::STChannelID channelId( const LHCb::LHCbID& id ) { return id.stID() ; }
    static bool checkType(const LHCb::LHCbID& id) { return id.isTT() ; }
  };
}

template<>
double MeasurementProviderT<MeasurementProviderTypes::TTLite>::nominalZ( const LHCb::LHCbID& id ) const
{
  LHCb::STChannelID stid(id.stID()) ;
  const DeSTSector* sector = const_cast<DeSTDetector*>(m_det)->findSector(stid) ;
  return sector->globalCentre().z();
}

template <>
LHCb::Measurement* MeasurementProviderT<MeasurementProviderTypes::TTLite>::measurement( const LHCb::LHCbID& id, bool ) const
{

  using namespace MeasurementProviderTypes;

  LHCb::Measurement* meas(0) ;
  if ( !TTLite::checkType(id) ) {
    error() << "Not correct measurement" << endmsg ;
  } else {
    TTLite::ClusterContainerType::const_iterator clus = clusters()->find<LHCb::STLiteCluster::findPolicy>( TTLite::channelId(id) );
    if (clus != clusters()->end()){
      meas = new TTLite::MeasurementType( *clus, *m_det, *m_positiontool );
    }
    else {
      error() << "Cannot find cluster for id " << id << endmsg ;
    }
  }
  return meas ;
}

template <>
LHCb::Measurement* MeasurementProviderT<MeasurementProviderTypes::TTLite>::measurement( const LHCb::LHCbID& id, 
                                                         const LHCb::ZTrajectory& reftraj, 
                                                         bool localY ) const 
{
 
  using namespace MeasurementProviderTypes;
  
  LHCb::Measurement* meas(0) ;
  if( !m_useReference ) {
    meas = measurement( id, localY ) ;
  } else {
    if ( !TTLite::checkType(id) ) {
      error() << "Not correct measurement" << endmsg ;
    } else {
      TTLite::ClusterContainerType::const_iterator clus = clusters()->find<LHCb::STLiteCluster::findPolicy>( TTLite::channelId(id) );     
      if (clus != clusters()->end()){
        const double z = nominalZ(id) ;      
        LHCb::StateVector refvector = reftraj.stateVector(z) ;
        meas = new TTLite::MeasurementType( *clus, *m_det, *m_positiontool, refvector );
      }
      else {
        error() << "Cannot find cluster for id " << id << endmsg ;
      }
    }
  }
  return meas ;
}

typedef MeasurementProviderT<MeasurementProviderTypes::TTLite> TTLiteMeasurementProvider ;
DECLARE_TOOL_FACTORY( TTLiteMeasurementProvider )

////////////////////////////////////////////////////////////////////////////////////////
namespace MeasurementProviderTypes {
  struct UT {
    typedef LHCb::STMeasurement      MeasurementType ;
    typedef ISTClusterPosition       PositionToolType ;
    typedef LHCb::STCluster          ClusterType ;
    typedef LHCb::STClusters         ClusterContainerType ;
    typedef DeSTDetector             DetectorType ;
    static std::string positionToolName() { return "STOfflinePosition/UTClusterPosition" ; }
    static std::string defaultClusterLocation() { return LHCb::STClusterLocation::UTClusters ; }
    static std::string defaultDetectorLocation() { return DeSTDetLocation::location("UT") ; }
    static LHCb::STChannelID channelId( const LHCb::LHCbID& id ) { return id.stID() ; }
    static bool checkType(const LHCb::LHCbID& id) { return id.isUT() ; }
  };
}

template<>
double MeasurementProviderT<MeasurementProviderTypes::UT>::nominalZ( const LHCb::LHCbID& id ) const
{
  LHCb::STChannelID stid(id.stID()) ;
  const DeSTSector* sector = const_cast<DeSTDetector*>(m_det)->findSector(stid) ;
  return sector->globalCentre().z();
}

typedef MeasurementProviderT<MeasurementProviderTypes::UT> UTMeasurementProvider ;
DECLARE_TOOL_FACTORY( UTMeasurementProvider )

namespace MeasurementProviderTypes {
  struct UTLite {
    typedef LHCb::STLiteMeasurement      MeasurementType ;
    typedef ISTClusterPosition           PositionToolType ;
    typedef LHCb::STLiteCluster          ClusterType ;
    typedef LHCb::STLiteCluster::STLiteClusters  ClusterContainerType ;
    typedef DeSTDetector                 DetectorType ;
    static std::string positionToolName() { return "STOnlinePosition/UTLiteClusterPosition" ; }
    static std::string defaultClusterLocation() { return LHCb::STLiteClusterLocation::UTClusters ; }
    static std::string defaultDetectorLocation() { return DeSTDetLocation::location("UT") ; }
    static LHCb::STChannelID channelId( const LHCb::LHCbID& id ) { return id.stID() ; }
    static bool checkType(const LHCb::LHCbID& id) { return id.isUT() ; }
  };
}

template<>
double MeasurementProviderT<MeasurementProviderTypes::UTLite>::nominalZ( const LHCb::LHCbID& id ) const
{
  LHCb::STChannelID stid(id.stID()) ;
  const DeSTSector* sector = const_cast<DeSTDetector*>(m_det)->findSector(stid) ;
  return sector->globalCentre().z();
}

template <>
LHCb::Measurement* MeasurementProviderT<MeasurementProviderTypes::UTLite>::measurement( const LHCb::LHCbID& id, bool ) const
{

  using namespace MeasurementProviderTypes;

  LHCb::Measurement* meas(0) ;
  if ( !UTLite::checkType(id) ) {
    error() << "Not correct measurement" << endmsg ;
  } else {
    UTLite::ClusterContainerType::const_iterator clus = clusters()->find<LHCb::STLiteCluster::findPolicy>( UTLite::channelId(id) );
    if (clus != clusters()->end()){
      meas = new UTLite::MeasurementType( *clus, *m_det, *m_positiontool );
    }
    else {
      error() << "Cannot find cluster for id " << id << endmsg ;
    }
  }
  return meas ;
}

template <>
LHCb::Measurement* MeasurementProviderT<MeasurementProviderTypes::UTLite>::measurement( const LHCb::LHCbID& id, 
                                                         const LHCb::ZTrajectory& reftraj, 
                                                         bool localY ) const 
{
 
  using namespace MeasurementProviderTypes;
  
  LHCb::Measurement* meas(0) ;
  if( !m_useReference ) {
    meas = measurement( id, localY ) ;
  } else {
    if ( !UTLite::checkType(id) ) {
      error() << "Not correct measurement" << endmsg ;
    } else {
      UTLite::ClusterContainerType::const_iterator clus = clusters()->find<LHCb::STLiteCluster::findPolicy>( UTLite::channelId(id) );     
      if (clus != clusters()->end()){
        const double z = nominalZ(id) ;      
        LHCb::StateVector refvector = reftraj.stateVector(z) ;
        meas = new UTLite::MeasurementType( *clus, *m_det, *m_positiontool, refvector );
      }
      else {
        error() << "Cannot find cluster for id " << id << endmsg ;
      }
    }
  }
  return meas ;
}

typedef MeasurementProviderT<MeasurementProviderTypes::UTLite> UTLiteMeasurementProvider ;
DECLARE_TOOL_FACTORY( UTLiteMeasurementProvider )

////////////////////////////////////////////////////////////////////////////////////////

namespace MeasurementProviderTypes {
  struct IT {
    typedef LHCb::STMeasurement      MeasurementType ;
    typedef ISTClusterPosition       PositionToolType ;
    typedef LHCb::STCluster          ClusterType ;
    typedef LHCb::STClusters         ClusterContainerType ;
    typedef DeSTDetector             DetectorType ;
    static std::string positionToolName() { return "STOfflinePosition/ITClusterPosition" ; }
    static std::string defaultClusterLocation() { return LHCb::STClusterLocation::ITClusters ; }
    static std::string defaultDetectorLocation() { return DeSTDetLocation::location("IT") ; }
    static LHCb::STChannelID channelId( const LHCb::LHCbID& id ) { return id.stID() ; }
    static bool checkType(const LHCb::LHCbID& id) { return id.isIT() ; }
  };
}

template<>
double MeasurementProviderT<MeasurementProviderTypes::IT>::nominalZ( const LHCb::LHCbID& id ) const
{
  // extremely ugly. need more functionality in det elements to do this quicker.
  LHCb::STChannelID stid(id.stID()) ;
  const DeSTSector* sector = const_cast<DeSTDetector*>(m_det)->findSector(stid) ;
  return sector->globalCentre().z() ;
}

typedef MeasurementProviderT<MeasurementProviderTypes::IT> ITMeasurementProvider ;
DECLARE_TOOL_FACTORY( ITMeasurementProvider )


////////////////////////////////////////////////////////////////////////////////////////

namespace MeasurementProviderTypes {
  struct ITLite {
    typedef LHCb::STLiteMeasurement      MeasurementType ;
    typedef ISTClusterPosition       PositionToolType ;
    typedef LHCb::STLiteCluster          ClusterType ;
    typedef LHCb::STLiteCluster::STLiteClusters         ClusterContainerType ;
    typedef DeSTDetector             DetectorType ;
    static std::string positionToolName() { return "STOnlinePosition/ITLiteClusterPosition" ; }
    static std::string defaultClusterLocation() { return LHCb::STLiteClusterLocation::ITClusters; }
    static std::string defaultDetectorLocation() { return DeSTDetLocation::location("IT") ; }
    static LHCb::STChannelID channelId( const LHCb::LHCbID& id ) { return id.stID() ; }
    static bool checkType(const LHCb::LHCbID& id) { return id.isIT() ; }
  };
}

template<>
double MeasurementProviderT<MeasurementProviderTypes::ITLite>::nominalZ( const LHCb::LHCbID& id ) const
{
  // extremely ugly. need more functionality in det elements to do this quicker.
  LHCb::STChannelID stid(id.stID()) ;
  const DeSTSector* sector = const_cast<DeSTDetector*>(m_det)->findSector(stid) ;
  return sector->globalCentre().z() ;
}


template <>
LHCb::Measurement* MeasurementProviderT<MeasurementProviderTypes::ITLite>::measurement( const LHCb::LHCbID& id, bool ) const
{

  using namespace MeasurementProviderTypes;

  LHCb::Measurement* meas(0) ;
  if ( !ITLite::checkType(id) ) {
    error() << "Not correct measurement" << endmsg ;
  } else {
    ITLite::ClusterContainerType::const_iterator clus = clusters()->find<LHCb::STLiteCluster::findPolicy>( TTLite::channelId(id) );
    if (clus != clusters()->end()){
      meas = new ITLite::MeasurementType( *clus, *m_det, *m_positiontool );
    }
    else {
      error() << "Cannot find cluster for id " << id << endmsg ;
    }
  }
  return meas ;
}

template <>
LHCb::Measurement* MeasurementProviderT<MeasurementProviderTypes::ITLite>::measurement( const LHCb::LHCbID& id, 
                                                         const LHCb::ZTrajectory& reftraj, 
                                                         bool localY ) const 
{
 
  using namespace MeasurementProviderTypes;

  LHCb::Measurement* meas(0) ;
  if( !m_useReference ) {
    meas = measurement( id, localY ) ;
  } else {
    if ( !ITLite::checkType(id) ) {
      error() << "Not correct measurement" << endmsg ;
    } else {
      ITLite::ClusterContainerType::const_iterator clus = clusters()->find<LHCb::STLiteCluster::findPolicy>( ITLite::channelId(id) );
      if (clus != clusters()->end()){
        const double z = nominalZ(id) ;
        LHCb::StateVector refvector = reftraj.stateVector(z) ;
        meas = new ITLite::MeasurementType( *clus, *m_det, *m_positiontool, refvector);
      }
      else {
        error() << "Cannot find cluster for id " << id << endmsg;
      }
    }
  }
  return meas ;
}


typedef MeasurementProviderT<MeasurementProviderTypes::ITLite> ITLiteMeasurementProvider ;
DECLARE_TOOL_FACTORY( ITLiteMeasurementProvider )


