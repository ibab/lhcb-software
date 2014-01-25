// $Id: HepMCJets2HepMCJets.cpp,v 1.1 2009-12-14 12:34:33 cocov Exp $
// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// DaVinci Kernel
// ============================================================================
#include "Kernel/IMatterVeto.h"
// ============================================================================
// Event
// ============================================================================
#include "GaudiKernel/IUpdateManagerSvc.h"

#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/IDetectorElement.h"
#include "DetDesc/ILVolume.h"
#include "DetDesc/IPVolume.h"
#include "DetDesc/Material.h"
#include "DetDesc/ITransportSvc.h"
#include <VeloDet/DeVeloSensor.h>
#include <VeloDet/DeVeloRType.h>
#include "VeloDet/DeVelo.h"
#include "DetDesc/Condition.h"

using namespace Gaudi::Units;
using namespace LHCb;
using namespace std;

namespace
{
template <typename iterator_>
struct iter_pair_range : pair<iterator_, iterator_>
{
    using super = pair<iterator_, iterator_>;
    using super::super; // delegate c'tor
    using iterator = iterator_;
    iterator begin() const
    {
        return this->first;
    }
    iterator end() const
    {
        return this->second;
    }
};

template <typename Iter, typename I2> // I2 must be convertable to Iter
iter_pair_range<typename std::decay<Iter>::type> make_range( Iter&& begin, I2&& end )
{
    return {std::forward<Iter>( begin ), std::forward<I2>( end )};
}

template <typename Iter, typename I2> // I2 must be convertable to Iter
iter_pair_range<typename std::decay<Iter>::type> make_range( pair<Iter, I2>&& p )
{
    return {std::forward<pair<Iter, I2>>( p )};
}

// (could use std::find_adjacent, but then would have to
//  handle the case where it returns 'end'...)
template <typename Iter>
Iter find_z_bin( Iter first, Iter end, double z )
{
    assert( first != end );
    auto next = first;
    ++next;
    assert( next != end );
    for ( ; next != end; ++first, ++next ) {
        if ( z < 0.5 * ( next->z() + first->z() ) ) break;
    }
    return first;
}
}

class MatterVetoTool : public virtual IMatterVeto, public GaudiTool
{
    // ========================================================================
    /// friend factory for instantiation
    friend class ToolFactory<MatterVetoTool>;

  public:
    // ========================================================================
    /// standard initialization of the tool
    StatusCode initialize();
    StatusCode finalize()
    {
        return GaudiTool::finalize();
    };
    bool isInMatter( const Gaudi::XYZPoint& point ) const;

  protected:
    // ========================================================================
    /// standard constructor
    MatterVetoTool( const std::string& type, ///< tool type ???
                    const std::string& name, ///< tool name
                    const IInterface* parent )
        : GaudiTool( type, name, parent )
    {
        //
        declareInterface<IMatterVeto>( this );
        //
        // this->registering();
        // this->InitialiseGeoInfo();
    }
    /// virtual protected destructor
    virtual ~MatterVetoTool()
    {
    }
    // ========================================================================
  private:
    // ========================================================================
    // default constructor is disabled
    MatterVetoTool() = delete;
    // copy constructor is disabled
    MatterVetoTool( const MatterVetoTool& );
    // assignement operator is disabled
    MatterVetoTool& operator=( const MatterVetoTool& );
    // ========================================================================
  private:
    // ========================================================================
    StatusCode i_cacheGeo();
    bool IsInMaterialBoxLeft( const Gaudi::XYZPoint& ) const;  ///<Point in material
                                                               ///region in Left
                                                               ///halfbox
    bool IsInMaterialBoxRight( const Gaudi::XYZPoint& ) const; ///<Point in material
                                                               ///region in Right
                                                               ///halfbox

    Gaudi::Transform3D m_toVeloLFrame; ///< to transform to local velo L frame
    Gaudi::Transform3D m_toVeloRFrame; ///< to transform to local velo R frame
    std::vector<Gaudi::XYZPoint> m_LeftSensorsCenter;
    std::vector<Gaudi::XYZPoint> m_RightSensorsCenter;
    Condition* m_motionSystem;
    // ========================================================================
};

// ============================================================================
// standard initialization of the tool
// ============================================================================
StatusCode MatterVetoTool::initialize()
{
    StatusCode sc = GaudiTool::initialize();
    if ( sc.isFailure() ) return sc;

    if ( !existDet<DataObject>( detSvc(), "Conditions/Online/Velo/MotionSystem" ) ) {
        Warning( "VELO motion system not in conditions DB", StatusCode::SUCCESS )
            .ignore();
        // m_useConditions = false;
    } else {
        registerCondition( "Conditions/Online/Velo/MotionSystem", m_motionSystem,
                           &MatterVetoTool::i_cacheGeo );
        sc = runUpdate();
        if ( !sc ) return sc;
    }

    return StatusCode::SUCCESS;

    // m_updMgrSvc = svc<IUpdateManagerSvc>("UpdateManagerSvc", true);
    // return GaudiTool::initialize () ;
}
//=============================================================================
// Check if particle vertex is in material
//=============================================================================
bool MatterVetoTool::isInMatter( const Gaudi::XYZPoint& point ) const
{
    // move to local Velo half frame
    // right half OR left half
    return ( ( point.x() <  2. ) && IsInMaterialBoxRight( m_toVeloRFrame * point ) ) 
       ||  ( ( point.x() > -2. ) && IsInMaterialBoxLeft(  m_toVeloLFrame * point ) );
}

//=============================================================================

//=============================================================================
// Initialize the geometric info
//=============================================================================
StatusCode MatterVetoTool::i_cacheGeo()
{
    // get the Velo geometry
    string velostr{"/dd/Structure/LHCb/BeforeMagnetRegion/Velo/Velo"};
    const IDetectorElement* lefthalv = getDet<IDetectorElement>( velostr + "Left" );
    const IDetectorElement* righthalv =
        getDet<IDetectorElement>( velostr + "Right" );
    const IGeometryInfo* halflgeominfo = lefthalv->geometry();
    const IGeometryInfo* halfrgeominfo = righthalv->geometry();
    const Gaudi::XYZPoint zero{0., 0., 0.};
    Gaudi::XYZPoint leftcenter = lefthalv->geometry()->toGlobal( zero );
    Gaudi::XYZPoint rightcenter = righthalv->geometry()->toGlobal( zero );
    if ( msgLevel( MSG::DEBUG ) )
        debug() << "Velo global right half center " << rightcenter
                << ", left half center " << leftcenter << endmsg;

    // matrix to transform to local velo frame
    m_toVeloRFrame = halfrgeominfo->toLocalMatrix();
    // m_toGlobalFrame = halfgeominfo->toGlobalMatrix();
    m_toVeloLFrame = halflgeominfo->toLocalMatrix();

    DeVelo* velo = getDet<DeVelo>( DeVeloLocation::Default );

    m_LeftSensorsCenter.clear();
    for ( const DeVeloRType* i :
          make_range( velo->leftRSensorsBegin(), velo->leftRSensorsEnd() ) ) {
        if ( i->isPileUp() ) continue;
        const DeVeloPhiType* sens = i->associatedPhiSensor();

        Gaudi::XYZPoint halfBoxCenter{i->localToVeloHalfBox( zero )};
        halfBoxCenter += 0.5 * ( sens->localToVeloHalfBox( zero ) - halfBoxCenter );

        m_LeftSensorsCenter.emplace_back( std::move( halfBoxCenter ) );
    }

    m_RightSensorsCenter.clear();
    for ( const DeVeloRType* i :
          make_range( velo->rightRSensorsBegin(), velo->rightRSensorsEnd() ) ) {
        if ( i->isPileUp() ) continue;
        const DeVeloPhiType* sens = i->associatedPhiSensor();

        Gaudi::XYZPoint halfBoxCenter{i->localToVeloHalfBox( zero )};
        halfBoxCenter += 0.5 * ( sens->localToVeloHalfBox( zero ) - halfBoxCenter );

        m_RightSensorsCenter.emplace_back( std::move( halfBoxCenter ) );
    } // end sensorloop
    return StatusCode::SUCCESS;
}

//=============================================================================
// Check if a point is in a region containing RFFoil and sensors in the Left
// halfbox frame
//=============================================================================

bool MatterVetoTool::IsInMaterialBoxLeft( const Gaudi::XYZPoint& point ) const
{

    if ( m_LeftSensorsCenter.size() < 3u ) return false;

    // First get the z bin
    Gaudi::XYZVector delta =
        point - *find_z_bin( m_LeftSensorsCenter.begin(), m_LeftSensorsCenter.end(),
                             point.z() );

    double r = delta.Rho();
    if ( r < ( point.z() < 370. ? 5. : 4.3 ) ) return false;
    // Is in the module area
    double halfModuleBoxThickness( 1.75 );
    if ( fabs( delta.z() ) < halfModuleBoxThickness ) return true;

    // depending on z:
    // in the region of small corrugation
    if ( point.z() < 290. && delta.x() > 4 ) {
        // first rather large region, rather small r
        float smallerCyl{8.};
        float RsmallerCyl{7.};
        float largerCyl{11.};
        float RlargerCyl{9.};

        if ( fabs( delta.z() ) > smallerCyl && r < RsmallerCyl ) return false;
        if ( fabs( delta.z() ) > largerCyl && r < RlargerCyl ) return false;
    }
    if ( r < 12.5 && point.z() < 440. ) return true;
    return fabs( delta.x() ) < ( point.z() < 440. ? 5.5 : 8.5 );
}

//=============================================================================
// Check if a point is in a region containing RFFoil and sensors in the Right
// halfbox frame
//=============================================================================

bool MatterVetoTool::IsInMaterialBoxRight( const Gaudi::XYZPoint& point ) const
{
    if ( m_RightSensorsCenter.size() < 3u ) return false;
    // First get the z bin
    Gaudi::XYZVector delta =
        point - *find_z_bin( m_RightSensorsCenter.begin(),
                             m_RightSensorsCenter.end(), point.z() );
    // Is in vaccum clean cylinder?
    double r = delta.Rho();

    // inner cylinder
    if ( r < ( point.z() < 390. ? 5. : 4.3 ) ) return false;
    // is in the module area
    double halfModuleBoxThickness( 1.75 );
    if ( fabs( delta.z() ) < +halfModuleBoxThickness ) return true;
    // depending on z:
    // in the region of small corrugation
    if ( point.z() < 300. && delta.x() < -4 ) {
        // first rather large region, rather small r
        float smallerCyl = 8.;
        float RsmallerCyl = 7.;
        float largerCyl = 11.;
        float RlargerCyl = 9.;

        if ( fabs( delta.z() ) > smallerCyl && r < RsmallerCyl ) return false;
        if ( fabs( delta.z() ) > largerCyl && r < RlargerCyl ) return false;
    }

    // Is clearly outside RFFoil part
    if ( r < 12.5 && point.z() < 450. ) return true;
    return fabs( delta.x() ) < ( point.z() < 450. ? 5.5 : 8.5 );
}

// ============================================================================
/// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MatterVetoTool )
// ============================================================================
// The END
// ============================================================================
