// $Id: MeasurementProviderT.cpp,v 1.6 2007-11-30 15:10:39 wouter Exp $
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
    info() << "sorry, MeasurementProviderBase::load not implemented" << endreq ;
    return StatusCode::FAILURE ;
  }

private:
  const typename T::ClusterContainerType* clusters() const ;

private:
  bool m_useReference ;
  ToolHandle<typename T::PositionToolType> m_positiontool ;
  const typename T::DetectorType* m_det;
  mutable typename T::ClusterContainerType* m_clusters;
} ;


// local
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ToolFactory.h"
#include "Event/StateVector.h"
#include "Event/Measurement.h"
#include "Event/ZTrajectory.h"

//using namespace LHCb;

template<typename T>
MeasurementProviderT<T>::MeasurementProviderT( const std::string& type,
                                               const std::string& name,
                                               const IInterface* parent )
  :  GaudiTool( type, name , parent ),
     m_positiontool(T::positionToolName()),
     m_det(0)
{
  declareInterface<IMeasurementProvider>(this);
  declareProperty( "UseReference", m_useReference = false );
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
LHCb::Measurement* MeasurementProviderT<T>::measurement( const LHCb::LHCbID& id, bool ) const
{
  LHCb::Measurement* meas(0) ;
  if ( !T::checkType(id) ) {
    error() << "Not a velo measurement" << endreq ;
  } else {
    const typename T::ClusterType* clus = clusters()->object( T::channelId(id) );
    if (clus) {
      meas = new typename T::MeasurementType( *clus, *m_det, *m_positiontool );
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
                                                         const LHCb::ZTrajectory& reftraj, 
                                                         bool localY ) const 
{
  LHCb::Measurement* meas(0) ;
  if( !m_useReference ) {
    meas = measurement( id, localY ) ;
  } else {
    if ( !T::checkType(id) ) {
      error() << "Not a velo measurement" << endreq ;
    } else {
      const typename T::ClusterType* clus = clusters()->object( T::channelId(id) );
      if (clus) {
        double z = nominalZ(id) ;
        LHCb::StateVector refvector = reftraj.stateVector(z) ;
        meas = new typename T::MeasurementType( *clus, *m_det, *m_positiontool, refvector);
      } else {
        error() << "Cannot find cluster for id " << id << endreq ;
      }
    }
  }
  return meas ;
}

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

double MeasurementProviderT<MeasurementProviderTypes::VeloR>::nominalZ( const LHCb::LHCbID& id ) const
{
  return m_det->rSensor( id.veloID() )->z() ;
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

double MeasurementProviderT<MeasurementProviderTypes::VeloPhi>::nominalZ( const LHCb::LHCbID& id ) const
{
  return m_det->phiSensor( id.veloID() )->z() ;
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

double MeasurementProviderT<MeasurementProviderTypes::TT>::nominalZ( const LHCb::LHCbID& id ) const
{
  // extremely ugly. need more functionality in det elements to do this quicker.
  LHCb::STChannelID stid(id.stID()) ;
  const DeSTSector* sector = const_cast<DeSTDetector*>(m_det)->findSector(stid) ;
  float dxdy, dzdy, xAtYEq0, zAtYEq0, ybegin, yend ;
  sector->trajectory(stid.strip(),0,dxdy,dzdy,xAtYEq0,zAtYEq0,ybegin,yend) ;
  return zAtYEq0 + (ybegin+yend)/2 * dzdy ;
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

double MeasurementProviderT<MeasurementProviderTypes::IT>::nominalZ( const LHCb::LHCbID& id ) const
{
  // extremely ugly. need more functionality in det elements to do this quicker.
  LHCb::STChannelID stid(id.stID()) ;
  const DeSTSector* sector = const_cast<DeSTDetector*>(m_det)->findSector(stid) ;
  float dxdy, dzdy, xAtYEq0, zAtYEq0, ybegin, yend ;
  sector->trajectory(stid.strip(),0,dxdy,dzdy,xAtYEq0,zAtYEq0,ybegin,yend) ;
  return zAtYEq0 + (ybegin+yend)/2 * dzdy ;
}

typedef MeasurementProviderT<MeasurementProviderTypes::IT> ITMeasurementProvider ;
DECLARE_TOOL_FACTORY( ITMeasurementProvider );
