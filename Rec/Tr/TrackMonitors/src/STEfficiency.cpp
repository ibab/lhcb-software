// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/PhysicalConstants.h"


// local
#include "STEfficiency.h"

// track interfaces
#include "TrackInterfaces/IHitExpectation.h"
#include "TrackInterfaces/ITrackSelector.h"

// kernel
#include "Kernel/ISTChannelIDSelector.h"
#include "Kernel/STLexicalCaster.h"
#include "Kernel/ITDetectorPlot.h"

// detector
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"
#include "STDet/DeSTSensor.h"

// Event
#include "Event/STCluster.h"


// AIDA
#include "AIDA/IProfile1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IHistogram1D.h"

#include "LoKi/select.h"

#include <sstream>

#include <cmath>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/assign/list_of.hpp>

using namespace LHCb;
using namespace ST;
using namespace AIDA;
using namespace Gaudi;
using namespace boost::assign;

//-----------------------------------------------------------------------------
// Implementation file for class : STEfficiency
//
// 2009-06-16 : Johan Luisier
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( STEfficiency )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
STEfficiency::STEfficiency( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : TrackMonitorBase ( name , pSvcLocator ),
    m_totalExpected( 0u ),
    m_totalFound( 0u ),
    m_NbrOfCuts( 0u ),
    m_binSize( 10000. ),
    m_binNumber( 0u )
{
  declareProperty( "DetType"         , m_detType =  "IT" );
  declareProperty( "ExpectedHitsTool", m_expectedHitsTool =
                   "ITHitExpectation" );
  std::vector<double> tmp = list_of( 500. * Gaudi::Units::um )( 700. * Gaudi::Units::um );
  declareProperty( "Cut"             , m_spacialCut = tmp );
  declareProperty( "TrackSelName"    , m_trackSelectorName =
                   "ITIsolatedTrackSelector" );
  declareProperty( "MinExpectedHits" , m_minHits = 10 );
  declareProperty( "XLayerCut"       , m_xCut = 500. * Gaudi::Units::um );
  declareProperty( "StereoLayerCut"  , m_stereoCut = 700. * Gaudi::Units::um );
  declareProperty( "ChargeCut"       , m_chargeCut = 0. );
  declareProperty( "CollectorPrefix" , m_collectorPrefix = "" );
  declareProperty( "MinExpectedNbr"  , m_minExpected = 100 );
  declareProperty( "Types"           , m_wantedTypes = std::vector<unsigned int>(1, 3) );
  declareProperty( "TakeEveryHit"    , m_everyHit = true );
  declareProperty( "MinDistToEdge"   , m_minDistToEdge = -1. );

  m_whichCut[0] = 0u;
  m_whichCut[1] = 0u;
}

//=============================================================================
// Destructor
//=============================================================================
STEfficiency::~STEfficiency() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode STEfficiency::initialize()
{
  StatusCode sc( TrackMonitorBase::initialize() ); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by TrackMonitorBase
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  std::string name("");
  
  for (unsigned int i( 0 ) ; i < 5; i++) 
  {
    name = "Mixed";
    if ( i < 4 ) name = ITNames().BoxToString( i + 1 );
    name += "Collector";
    name = m_collectorPrefix + name;
    m_collectors.push_back( tool< ISTClusterCollector >( "STClusterCollector", name ) );
  }

  m_expectedHits  = tool< IHitExpectation >( "ITHitExpectation",
                                             m_expectedHitsTool );
  m_trackSelector = tool< ITrackSelector >( "ITIsolatedTrackSelector",
                                            m_trackSelectorName );

  m_tracker = getDet< DeSTDetector >( DeSTDetLocation::location( m_detType ) );
  m_NbrOfCuts = m_spacialCut.size();
  m_foundSector.resize(m_NbrOfCuts);
  m_foundLayer.resize(m_NbrOfCuts);

  const DeSTDetector::Sectors& sectors = m_tracker->sectors();
  DeSTDetector::Sectors::const_iterator iterS = sectors.begin();
  for ( ; iterS != sectors.end(); ++iterS)
  {
    STChannelID elemID = (*iterS)->elementID();
    m_expectedSector[elemID.uniqueSector()] = 0u;
    m_expectedLayer[elemID.uniqueDetRegion()] = 0u;
    m_nameMapSector[elemID.uniqueSector()] = *iterS;
    m_nameMapLayer[elemID.uniqueDetRegion()] = *iterS;
      
    for (unsigned int i=0 ; i < m_NbrOfCuts; ++i )
    {
      m_foundSector[i][elemID.uniqueSector()] = 0u; 
      m_foundLayer[i][elemID.uniqueSector()] = 0u;       
    }
      
  } // iterS

  unsigned int iCut;

  for (iCut = 0; iCut < m_NbrOfCuts; ++iCut)
  {
    if ( std::abs( m_xCut - m_spacialCut[iCut] ) < .005 )
      m_whichCut[0] = iCut;
    else if ( std::abs( m_stereoCut -  m_spacialCut[iCut]) < .005 )
      m_whichCut[1] = iCut;
  } // loop cuts

  for(iCut = 1; iCut < m_NbrOfCuts; iCut++)
  {
    m_binSize = std::min( m_binSize,
                          m_spacialCut[ iCut ] - m_spacialCut[ iCut - 1 ] );
  }

  m_binSize = std::min( m_binSize, .01 );
  m_binSize = std::max( m_binSize, .01 );

  m_binNumber = static_cast<unsigned int>( (m_spacialCut.back() -
                                            m_spacialCut.front()) / m_binSize );
  m_binNumber++;
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode STEfficiency::execute()
{
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  Tracks* tracks = get< LHCb::Tracks >( inputContainer() );
  Tracks::const_iterator It, Begin( tracks -> begin() ),
    End( tracks -> end() );

  std::vector<LHCbID> expectedHits;
  std::vector<unsigned int> expectedSectors, expectedLayers;
  ISTClusterCollector::Hits output;

  Category type;
  std::vector<LHCb::LHCbID> itHits;

  DeSTSector* sector;
  DeSTSensor* sensor;
  XYZPoint trackintersect, lPoint;
  double distToEdgeX, distToEdgeY, distToEdge;

  std::vector< unsigned int >::const_iterator
    typeBegin( m_wantedTypes.begin() ),
    typeEnd( m_wantedTypes.end() );
  
  // Loop on tracks
  for (It = Begin; It != End; It++)
  {
    // Clearing all the std::vectors !
    expectedHits.clear();
    expectedSectors.clear();
    expectedLayers.clear();
    itHits.clear();
    output.clear();

    plot( static_cast<unsigned int>((*It) -> type()),
          "Control/RawTrackType", "Track type", -.5, 9.5, 10 );

    // Track type selection
    if ( find( typeBegin, typeEnd,
               static_cast<unsigned int>((*It) -> type()) ) == typeEnd )
      continue;

    plot( static_cast<unsigned int>((*It) -> type()),
          "Control/CutTrackType", "Track type", -.5, 9.5, 10 );

    // collect the expected hits
    m_expectedHits -> collect( **It, expectedHits );
    plot(expectedHits.size(), "expected", "# expected", -0.5, 20.5, 21);
    if (expectedHits.size() < m_minHits) continue; // no hits to find

    plot( static_cast<unsigned int>((*It) -> type()),
          "Control/MinHitTrackType", "Track type", -.5, 9.5, 10 );      

    if ( ! m_trackSelector -> accept( **It )) continue;

    plot( static_cast<unsigned int>((*It) -> type()),
          "Control/IsolTrackType", "Track type", -.5, 9.5, 10 );
      
    // for processing convert the expected hits to unique sectors
    BOOST_FOREACH(LHCbID id, expectedHits)
      {
        sector = m_tracker -> findSector( id.stID() );
        if ( sector == 0 )
          continue;
        // Get the intersection point between track and sector
        trackintersect = XYZPoint( 0., 0., 0. );
        extrapolator() -> position( (**It), sector -> globalCentre().z(),
                                    trackintersect );
        // Get the coresponding sensor
        sensor = sector -> findSensor( trackintersect );
        if ( sensor == 0 )
          continue;
        // Get the position in local frame
        lPoint = sensor -> toLocal( trackintersect );
        distToEdgeX = sensor -> activeWidth() / 2. - std::abs( lPoint.x() );
        distToEdgeY = sensor -> activeWidth() / 2. - std::abs( lPoint.x() );
        distToEdge = std::min( distToEdgeX, distToEdgeY );
        // Cut on distance to edge
        if ( distToEdge < m_minDistToEdge )
        {
          plot( id.stID().uniqueSector(), "Control/sectorCheck", "Sectors with hit too close from edge",
                -.5, 8191.5, 8192 );
          plot( id.stID().uniqueDetRegion(), "Control/layerCheck", "Layers with hit too close from edge",
                -.5, 255.5, 256 );
          continue;
        }
        expectedSectors.push_back(id.stID().uniqueSector());
        expectedLayers.push_back(id.stID().uniqueDetRegion());
      }
      
    filterNameList( expectedSectors );
    filterNameList( expectedLayers );

    if ( expectedSectors.empty() && expectedLayers.empty() )
      continue;

    const std::vector<LHCb::LHCbID>& ids = (*It) -> lhcbIDs();
    itHits.reserve(ids.size());
    LoKi::select(ids.begin(), ids.end(), back_inserter(itHits),
                 bind(&LHCbID::isIT,_1));

    type = ITCategory( itHits );

    plot(type, "type", "Type of the track", .5, 5.5, 5);

    // collect hits
    m_collectors[type - 1] -> execute( **It, output );

    plot(output.size(), "collected", "collected", -0.5, 20.5, 21);
      
    std::vector<unsigned int>::iterator iterExp = expectedSectors.begin();
    for ( ; iterExp != expectedSectors.end(); ++iterExp)
    {
      ++m_expectedSector[*iterExp];
      ++m_totalExpected;
      for (unsigned int i = 0; i < m_NbrOfCuts; ++i)
	    {
	      if (foundHitInSector(output, *It, *iterExp, m_spacialCut[i]))
        {
          DeSTSector* sector = m_nameMapSector[*iterExp];
          if ( sector ->isStereo() && i == m_whichCut[1] )
            ++m_totalFound;
          else if ( ! sector -> isStereo() &&  i == m_whichCut[0] )
            ++m_totalFound;
          ++m_foundSector[i][*iterExp]; 
	      }
	    } // loop cuts
    } // loop expected

    for (iterExp = expectedLayers.begin();
         iterExp != expectedLayers.end(); ++iterExp)
    {
      ++m_expectedLayer[*iterExp];
      for (unsigned int i = 0; i < m_NbrOfCuts; ++i)
	    {
        if (foundHitInLayer(output, *It, *iterExp, m_spacialCut[i]))
          ++m_foundLayer[i][*iterExp];
      } // loop cuts
    } // loop expected

  } // loop tracks
  
  return StatusCode::SUCCESS;
}

/**
 * Checks whether there is a hit in the given sector.
 *
 * @param hits list of clusters.
 * @param testsector uniqueID of the wanted sector
 * @param resCut window size
 *
 * @return \e true if yes.
 */
bool STEfficiency::foundHitInSector( const ISTClusterCollector::Hits& hits,
                                     Track* const& track,
                                     const unsigned int testsector,
                                     const double resCut ) const
{
  const DeSTSector*  sector;
  unsigned int beetle;
  double sigToNoise;
  BOOST_FOREACH(ISTClusterCollector::Hit aHit, hits)
    {
      if ( aHit.cluster->channelID().uniqueSector() == testsector &&
           fabs(aHit.residual) < resCut )
      {
        if ( !( m_everyHit || track -> isOnTrack( LHCbID( aHit.cluster -> channelID() ) ) ) )
          continue;
        sector     = m_tracker -> findSector( aHit.cluster -> channelID() );
        beetle     = sector -> beetle( aHit.cluster -> channelID());
        sigToNoise = aHit.cluster -> totalCharge() / sector->beetleNoise(beetle);
        if ( sigToNoise < m_chargeCut )
          continue;
        else
          return true;
      }
    } // foreach
  
  return false;
}
/**
 * Checks whether there is a hit in the given layer.
 *
 * @param hits list of clusters.
 * @param testlayer uniqueID of the wanted layer
 * @param resCut window size
 *
 * @return \e true if yes.
 */
bool STEfficiency::foundHitInLayer( const ISTClusterCollector::Hits& hits,
                                    Track* const& track,
                                    const unsigned int testlayer,
                                    const double resCut  ) const
{
  const DeSTSector*  sector;
  unsigned int beetle;
  double sigToNoise;
  BOOST_FOREACH(ISTClusterCollector::Hit aHit, hits)
    {
      if ( aHit.cluster->channelID().uniqueDetRegion() == testlayer &&
           fabs(aHit.residual) < resCut )
      {
        if ( !( m_everyHit || track -> isOnTrack( LHCbID( aHit.cluster -> channelID() ) ) ) )
          continue;
        sector     = m_tracker -> findSector( aHit.cluster -> channelID() );
        beetle     = sector -> beetle( aHit.cluster -> channelID());
        sigToNoise = aHit.cluster -> totalCharge()/sector->beetleNoise(beetle);
        if ( sigToNoise < m_chargeCut )
          continue;
        else
          return true;
      }
    } // foreach
  
  return false;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode STEfficiency::finalize()
{
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  if ( m_totalExpected == 0 )
  {
    warning() << "I'm sorry, but no track was found, please check that"
              << " you selected the right track type." << std::endl
              << "Your selection was : ";
    for (unsigned int i( 0 ); i < m_wantedTypes.size(); i++)
      warning() << m_wantedTypes[ i ] << ' ';
    warning() << endmsg;
    return TrackMonitorBase::finalize();
  }

  std::map<unsigned int, DeSTSector*>::const_iterator iterS = m_nameMapSector.begin();
 
  std::vector<double>::const_iterator cutBegin( m_spacialCut.begin() ),
    cutEnd( m_spacialCut.end() ), cutIt;

  STChannelID channelID;
  std::string nick, root;

  unsigned int i, presentCut( 0u );

  //  double totExpected = 0; double totFound = 0;
  double nExpected, nFound, err;
  double eff  = 999.0;

  ITDetectorPlot prop( "EfficiencyPlot", "Efficiency Plot" );

  IHistogram2D* histo = book2D( prop.name() , prop.title(),
                                prop.minBinX(), prop.maxBinX(), prop.nBinX(),
                                prop.minBinY(), prop.maxBinY(), prop.nBinY());

  for (; iterS != m_nameMapSector.end(); ++iterS )
  {
    channelID = iterS -> second -> elementID();
    nick   = iterS -> second -> nickname();
    iterS->second->isStereo() == true ? presentCut = m_whichCut[1]: presentCut = m_whichCut[0] ;
      
    if (m_detType == "IT")
      root = ITNames().UniqueBoxToString(channelID);

    // how many were expected ?
    nExpected = static_cast<double>( m_expectedSector[ iterS -> first ] );
      
    if (nExpected > 1)
    {
      profile1D(-10., nExpected, root + "/Eff_" + nick,
                "Efficiency vs cut",
                m_spacialCut.front() - m_binSize / 2.,
                m_spacialCut.back() + m_binSize / 2.,
                m_binNumber);
      for (i = 0; i < m_NbrOfCuts; ++i)
	    {
	      nFound = static_cast<double>( m_foundSector[i][iterS->first] );
	      eff = 100. * nFound / nExpected;
	      err = sqrt( eff * (100. - eff) / nExpected );
	      profile1D(m_spacialCut[i], eff, root + "/Eff_" + nick,
                  "Efficiency vs cut",
                  m_spacialCut.front() - m_binSize / 2.,
                  m_spacialCut.back() + m_binSize / 2.,
                  m_binNumber);
	      if ( i == presentCut )
        {
          if(msgLevel(MSG::VERBOSE)) verbose() << nick << ' ' << eff
                                             << " +/- " << err << " (found " << nFound
                                             << " for cut " << m_spacialCut[i] << ")" << endmsg;
		  
          ST::ITDetectorPlot::Bins theBins = prop.toBins(channelID); 
          if ( nExpected > m_minExpected )
          {
            histo -> fill( theBins.xBin, theBins.yBin, eff );
            plot(eff,"sectorEff", "sector eff", 0., 100., 200);
          }
          else
            histo -> fill( theBins.xBin, theBins.yBin, 102. );
        }
	    } // i
    } //nExpected
  } // iterS
  
  for (iterS = m_nameMapLayer.begin(); iterS != m_nameMapLayer.end(); ++iterS )
  {
    channelID = iterS -> second -> elementID();
    iterS->second->isStereo() == true ? presentCut = m_whichCut[1]: presentCut = m_whichCut[0] ;
    if (m_detType == "IT")
    {
      nick  = ITNames().UniqueLayerToString(channelID);
    }
      
    root = "Layers";
      
    // how many were expected ?
    nExpected = m_expectedLayer[ iterS -> first ];
      
    if (nExpected > 1)
    {
      profile1D(-10., nExpected, root + "/Eff_" + nick,
                "Efficiency vs cut",
                m_spacialCut.front() - m_binSize / 2.,
                m_spacialCut.back() + m_binSize / 2.,
                m_binNumber);
      for (i = 0; i < m_NbrOfCuts; ++i)
	    {
	      nFound = static_cast<double>( m_foundLayer[i][iterS->first] );
	      eff = 100. * nFound /  nExpected;
	      err = sqrt( eff * (100. - eff) / nExpected );
	      profile1D(m_spacialCut[i], eff, root + "/Eff_" + nick,
                  "Efficiency vs cut",
                  m_spacialCut.front() - m_binSize / 2.,
                  m_spacialCut.back() + m_binSize / 2.,
                  m_binNumber);
	      if ( i == presentCut )
        {
          plot(eff, "layerEff", "layer eff", 0., 100, 200);
          info() << std::setw(11) << std::left << nick << ' ' 
                 << std::setw(4) << std::right << nFound
                 << " found hits" << " (" << std::setw(4) << std::right
                 << nExpected << " expected ones), cut = "
                 << formatNumber( m_spacialCut[i], 1u )
                 << ' ' << std::setw(6) << formatNumber( eff )
                 << " +/- "
                 << std::setw(5) << formatNumber( err )
                 << endmsg;
        }
	    } // i
    } // nExpected
  } // iterS
  
  // total efficency
  double teff = 999.;
  if ( m_totalExpected != 0u )
  {
    teff = 100. * static_cast<double>(m_totalFound)/static_cast<double>(m_totalExpected);
    double terror = sqrt( teff * (100. - teff) / static_cast<double>(m_totalExpected ));
    info() << "Total Eff " << formatNumber( teff ) <<  " +/- "
           << formatNumber( terror ) << endmsg;
  }
  
  std::string layerEff( "layer eff" );
  AIDA::IHistogram1D* layerHisto = histo1D( layerEff );
  if ( layerHisto != 0 )
    info() << "Layer Eff: " << formatNumber( layerHisto->mean() ) << " rms "
           << formatNumber( layerHisto->rms() ) << endmsg;  
  else
    warning() << "Got an empty histo for \"layer eff\"" << endmsg;
    

  std::string sectorEff( "sector eff" );
  IHistogram1D* sectorHisto = histo1D( sectorEff);
  if ( sectorHisto == 0 )
    warning() << "Got an empty histo for \"sector eff\"" << endmsg;
  else
    info() << "Sector Eff: " << formatNumber( sectorHisto->mean() ) << " rms "
           << formatNumber( sectorHisto->rms() ) << endmsg;  

  return TrackMonitorBase::finalize();  // must be called after all other actions
}

STEfficiency::Category STEfficiency::ITCategory(const std::vector<LHCb::LHCbID>& ids) const
{
  typedef std::map<unsigned int, unsigned int> BoxMap;
  BoxMap nBox;
  BOOST_FOREACH(LHCb::LHCbID id, ids)
    {
      nBox[ id.stID().detRegion() ] += 1;
    } // for each
  
  return nBox.size() == 1 ? Category(nBox.begin() -> first) : Mixed ;
}

void STEfficiency::filterNameList(std::vector<unsigned int>& vec)
{
  std::sort( vec.begin(), vec.end() );
  std::unique( vec.begin(), vec.end() );
}

/**
 * Stupid function that allows to print the number with 2 decimals,
 * since using std::setprecision and std::fixed directly in the info()
 * screws everythings up.
 *
 * @param nbr number (double) to print
 * @param digits number of wanted decimal (2 is the default)
 *
 * @return a string with the correct number of decimals.
 */
std::string STEfficiency::formatNumber(const double& nbr,
                                       const unsigned int& digits ) const
{
  std::stringstream ss;
  ss.setf(std::ios::fixed, std::ios::floatfield);
  ss << std::setprecision( digits ) << nbr;
  return ss.str();
}

//=============================================================================
