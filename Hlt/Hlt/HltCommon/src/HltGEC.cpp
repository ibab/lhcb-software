// $Id: HltGEC.cpp,v 1.4 2010-08-25 07:41:02 graven Exp $
// ============================================================================
// Include files
// ============================================================================
// local
// ============================================================================
#include "HltGEC.h"
#include "Event/STLiteCluster.h"
#include "Event/VeloLiteCluster.h"
// ============================================================================
/** @file
 *  Implementation file for class Hlt::GEC
 *  @see Hlt::GEC
 *  @author Jose Angel Hernando Morata
 *  @author Vanya Belyaev@nikhef.nl

 *  @date 2008-04-08
 */
// ============================================================================
/*  Standard constructor
 *  @param type the tol type (???)
 *  @param name the tool instance name
 *  @param parent the tool parent
 */
// ============================================================================
Hlt::GEC::GEC( const std::string& type,   // the tool type
               const std::string& name,   // the tool instance name
               const IInterface* parent ) // the tool parent
    : base_class( type, name, parent ),
      m_maxVeloBalance{ -1 },
      m_maxOTHits{ 10000 },
      m_maxITHits{ 3000 },
      m_maxVeloHits{ 3000 },
      m_minOTHits{ 50 },
      m_minITHits{ 50 },
      m_minVeloHits{ 50 },
      m_rawBankDecoder{ nullptr },
      m_veloDet{ nullptr },
      m_isActivity{ false }
{
    declareProperty( "MaxOTHits", m_maxOTHits, "Maximum number of OT-hits" );
    declareProperty( "MaxITHits", m_maxITHits, "Maximum number of IT-hits" );
    declareProperty( "MaxVeloHits", m_maxVeloHits, "Maximum number of Velo-hits" );
    declareProperty( "MinOTHits", m_minOTHits, "Minimum number of OT-hits" );
    declareProperty( "MinITHits", m_minITHits, "Minimum number of IT-hits" );
    declareProperty( "MinVeloHits", m_minVeloHits, "Minimum number of Velo-hits" );
    declareProperty( "IsActivity", m_isActivity,
                     "is activity trigger or upper GEC " );
}
// ============================================================================
// virtual & protected destructor
// ============================================================================
Hlt::GEC::~GEC()
{
}
// ============================================================================
// standard initialization
// ============================================================================
StatusCode Hlt::GEC::initialize()
{
    StatusCode sc = GaudiTool::initialize();
    if ( sc.isFailure() ) return sc;
    m_rawBankDecoder = tool<IOTRawBankDecoder>( "OTRawBankDecoder" );
    m_veloDet = getDet<DeVelo>( DeVeloLocation::Default );
    return sc;
}
// ============================================================================
// standard finalization
// ============================================================================
StatusCode Hlt::GEC::finalize()
{
    m_rawBankDecoder = nullptr;
    return GaudiTool::finalize();
}
// ============================================================================
// accept
// ============================================================================
bool Hlt::GEC::accept() const
{
    auto nOTHits = [&]() { return (int)m_rawBankDecoder->totalNumberOfHits(); };
    auto nITHits = [&]() {
        return (int)get<LHCb::STLiteCluster::STLiteClusters>(
            LHCb::STLiteClusterLocation::ITClusters )->size();
    };
    auto nVeloHits = [&]() {
        return (int)get<LHCb::VeloLiteCluster::VeloLiteClusters>(
            LHCb::VeloLiteClusterLocation::Default )->size();
    };
    if ( m_isActivity ) {
        // always()<<"run activity trigger"<<endmsg;
        return ( m_minOTHits < 0 || nOTHits() > m_minOTHits ) ||
               ( m_minITHits < 0 || nITHits() > m_minITHits ) ||
               ( m_minVeloHits < 0 || nVeloHits() > m_minVeloHits );
    } else {
        return ( m_maxOTHits < 0 || nOTHits() < m_maxOTHits ) &&
               ( m_maxITHits < 0 || nITHits() < m_maxITHits ) &&
               ( m_maxVeloHits < 0 || nVeloHits() < m_maxVeloHits ) &&
               ( m_maxVeloBalance < 0 || veloBalance() < m_maxVeloBalance );
    }
}
// ============================================================================
// check
// ============================================================================
StatusCode Hlt::GEC::check()
{
    return accept() ? StatusCode( StatusCode::SUCCESS, true )
                    : StatusCode( StatusCode::RECOVERABLE, true );
}

// ============================================================================
// improved VELO observable
// ============================================================================

#include "Event/VeloLiteCluster.h"
#include "VeloDet/DeVelo.h"
#include <cmath>

double Hlt::GEC::veloBalance() const
{
    // Loop over velo clusters and compute the vector sum of all phi vectors
    const auto* clusters = get<LHCb::VeloLiteCluster::VeloLiteClusters>(
        LHCb::VeloLiteClusterLocation::Default );
    assert( clusters != 0 );
    double sumx( 0 ), sumy( 0 );
    size_t nphi( 0 );
    for ( const LHCb::VeloLiteCluster& c : *clusters ) {
        if ( !c.isPhiType() ) continue;
        // navigate to the phi sensor
        LHCb::VeloChannelID channelID = c.channelID();
        int sensorID = channelID.sensor();
        const DeVeloPhiType* sensor = m_veloDet->phiSensor( sensorID );

        // get phi
        unsigned int stripID = channelID.strip();
        double phi = sensor->globalPhiOfStrip( stripID );

        // add to the vector sum
        sumx += std::cos( phi );
        sumy += std::sin( phi );
        ++nphi;
    }
    // compute the size of the vector. this should be good enough for selection.
    double totalphivectmag = std::sqrt( sumx * sumx + sumy * sumy );

    // in the displaced vertex analysis, we actually resize by the total
    // number of clusters, but I doubt that that makes any difference.
    // GR: advantage of NOT resizing would be that the number is between 0 and 1...
    //     and maybe one can/should make a 2D cut in the #clusters vs. avphi...
    double avphitimesntot = ( totalphivectmag / nphi ) * clusters->size();
    return avphitimesntot;
    // double avphi = (totalphivectmag / nphi) ;
    // return avphi;
}

// ============================================================================
// The Factory
// ============================================================================
DECLARE_NAMESPACE_TOOL_FACTORY( Hlt, GEC )
// ============================================================================
// The END
// ============================================================================
