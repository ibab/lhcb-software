// Include files 

// from Gaudi
#include "GaudiKernel/PhysicalConstants.h"

// local
#include "STEfficiency.h"

// track interfaces
#include "TrackInterfaces/IHitExpectation.h"

// kernel
#include "Kernel/ISTChannelIDSelector.h"
#include "Kernel/STLexicalCaster.h"
#include "Kernel/ITDetectorPlot.h"
#include "Kernel/TTDetectorPlot.h"

// detector
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"
#include "STDet/DeTTSector.h"
#include "STDet/DeSTSensor.h"

// Event
#include "Event/STCluster.h"

// AIDA
#include "AIDA/IProfile1D.h"
#include "AIDA/IProfile2D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IHistogram1D.h"

#include "LoKi/select.h"

#include <sstream>

#include <cmath>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/assign/list_of.hpp>

#include <iostream>
using namespace std;


using namespace LHCb;
using namespace ST;
using namespace AIDA;
using namespace Gaudi;
using namespace boost::assign;

//-----------------------------------------------------------------------------
// Implementation file for class : STEfficiency
//
// 2009-06-16 : Johan Luisier
// 2010-07-27 : Frederic Dupertuis
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
    m_binSize( 10000. )
{
  declareProperty( "DetType"            , m_detType = "IT" );
  declareProperty( "ClusterCollector"   , m_clustercollectorName = m_detType+"ClusterCollector");  
  declareProperty( "HitExpectation"     , m_expectedHitsToolName = m_detType+"HitExpectation");  
  std::vector<double> tmp = list_of( 300. * Gaudi::Units::um );
  declareProperty( "Cuts"               , m_spacialCuts = tmp );
  declareProperty( "XLayerCut"          , m_xCut = 300. * Gaudi::Units::um );
  declareProperty( "StereoLayerCut"     , m_stereoCut = 300. * Gaudi::Units::um );
  declareProperty( "MinExpectedSectors" , m_minExpSectors = 1 );
  declareProperty( "MinNbResSectors"    , m_minNbResSectors = -10 );
  declareProperty( "MaxNbResSectors"    , m_maxNbResSectors = 10 );
  declareProperty( "MinDistToEdgeX"     , m_minDistToEdgeX = -1. );
  declareProperty( "MinDistToEdgeY"     , m_minDistToEdgeY = -1. );
  declareProperty( "MinStationPassed"   , m_minStationPassed = 1 );
  declareProperty( "MinCharge"          , m_minCharge = 0. );
  declareProperty( "EfficiencyPlot"     , m_effPlot = true );
  declareProperty( "ResidualsPlot"      , m_resPlot = false );
  declareProperty( "TakeEveryHit"       , m_everyHit = true );
  declareProperty( "SingleHitPerSector" , m_singlehitpersector = false );
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
  
  m_clustercollectorName = m_detType+"ClusterCollector";
  m_expectedHitsToolName = m_detType+"HitExpectation";
  
  m_clustercollector = tool< ISTClusterCollector >( "STClusterCollector", m_clustercollectorName );
  m_expectedHits  = tool< IHitExpectation >( m_detType+"HitExpectation", m_expectedHitsToolName );
  
  m_tracker = getDet< DeSTDetector >( DeSTDetLocation::location( m_detType ) );
  m_NbrOfCuts = m_spacialCuts.size();
  m_foundSector.resize(m_NbrOfCuts);
  
  const DeSTDetector::Sectors& sectors = m_tracker->sectors();
  DeSTDetector::Sectors::const_iterator iterS = sectors.begin();
  for ( ; iterS != sectors.end(); ++iterS)
  {
    STChannelID elemID = (*iterS)->elementID();
    m_expectedSector[elemID.uniqueSector()] = 0u;
    m_nameMapSector[elemID.uniqueSector()] = *iterS;
    
    for (unsigned int i=0 ; i < m_NbrOfCuts; ++i )
    {
      m_foundSector[i][elemID.uniqueSector()] = 0u; 
    }
      
  } // iterS

  unsigned int iCut;

  for (iCut = 0; iCut < m_NbrOfCuts; ++iCut)
  {
    if ( std::abs( m_xCut - m_spacialCuts[iCut] ) < .005 )
      m_whichCut[0] = iCut;
    if ( std::abs( m_stereoCut -  m_spacialCuts[iCut]) < .005 )
      m_whichCut[1] = iCut;
  } // loop cuts

  for(iCut = 1; iCut < m_NbrOfCuts; iCut++)
  {
    m_binSize = std::min( m_binSize,
                          m_spacialCuts[ iCut ] - m_spacialCuts[ iCut - 1 ] );
  }
  
  //m_binSize = std::min( m_binSize, .01 );
  //m_binSize = std::max( m_binSize, .01 );

  m_binNumber = static_cast<unsigned int>( (m_spacialCuts.back() -
                                            m_spacialCuts.front()) / m_binSize );
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
  std::vector<unsigned int> expectedSectors;
  std::vector<unsigned int> foundSectors;
  std::vector<unsigned int>::iterator itFoundSectors;
  ISTClusterCollector::Hits prefoundHits;
  ISTClusterCollector::Hits foundHits;
  StatusCode inSensor;
  bool inActiveSensor;
  unsigned int nbFoundHits,nbExpHits,nbExpSectors,nbFoundSectors;
  int nbResHits,nbResSectors;
  State trackState;
  Gaudi::XYZPoint toleranceOnEdge(- std::max(0.,m_minDistToEdgeX), - std::max(0.,m_minDistToEdgeY), 0.);

  DeSTSector* sector = 0;
  std::vector<DeSTSensor*> sensors;
  
  // Loop on tracks
  for (It = Begin; It != End; It++)
  {
    // Clearing all the std::vectors !
    expectedHits.clear();
    prefoundHits.clear();
    foundHits.clear();
    expectedSectors.clear();
    foundSectors.clear();

    // ask for tracks crossing enough stations
    if( ! hasMinStationPassed( (*It) ) )
      continue;
        
    // collect the list of expected hits
    m_expectedHits -> collect( **It, expectedHits );

    nbExpHits = expectedHits.size();
    
    // remove tracks with too few expected hits
    if ( nbExpHits < m_minExpSectors ) 
      continue; 

    // collect the list of found hits
    m_clustercollector -> execute( **It, prefoundHits );
    
    nbFoundHits = prefoundHits.size();
    
    nbResHits = (int)(nbFoundHits) - (int)(nbExpHits);
    
    if(fullDetail()){
      plot(nbFoundHits, "Control/NbFoundHits","Number of found hits",-0.5,24.5,25);
      plot(nbExpHits, "Control/NbExpHits","Number of expected hits",-0.5,24.5,25);
      plot(nbResHits, "Control/NbResHits","Number of found - expected hits",-5.5,7.5,13);
    }
        
    // convert the expected hits into unique sectors cutting sensor edges
    BOOST_FOREACH(LHCbID id, expectedHits)
    {
      if( m_minDistToEdgeX > 0. || m_minDistToEdgeY > 0.){
        
        sector = m_tracker -> findSector( id.stID() );
        if ( sector == 0 ){
          info() << "Should not happen !" << endmsg;
          continue;
        }
        
        sensors = sector -> sensors();
        inActiveSensor = false;
        
        BOOST_FOREACH(DeSTSensor* sensor, sensors)
        {
          inSensor = extrapolator() -> propagate( (**It), sensor -> plane(), trackState, sensor -> thickness() );
          if( inSensor.isSuccess() ){
            inActiveSensor = sensor -> globalInActive(trackState.position(), toleranceOnEdge) ;
            if( inActiveSensor ) break;
          }
        }
        
        if( inActiveSensor )
        {
          expectedSectors.push_back(id.stID().uniqueSector());
        }
      }
    }

    nbExpSectors = expectedSectors.size();

    if ( nbExpSectors < m_minExpSectors ) 
      continue; 
        
    BOOST_FOREACH(ISTClusterCollector::Hit hit, prefoundHits)
    {
      if(find(expectedSectors.begin(), expectedSectors.end(), hit.cluster->channelID().uniqueSector()) != expectedSectors.end()){
        if(find(foundSectors.begin(), foundSectors.end(), hit.cluster->channelID().uniqueSector()) == foundSectors.end()){
          foundHits.push_back(hit);
          foundSectors.push_back(hit.cluster->channelID().uniqueSector());
        }else if( !m_singlehitpersector )
          foundHits.push_back(hit);
        else
          continue;
      }
    }

    nbExpHits = nbExpSectors;
    nbFoundHits = foundHits.size();
    nbResHits = (int)(nbFoundHits) - (int)(nbExpHits);
    
    nbFoundSectors = foundSectors.size();
    nbResSectors = (int)(nbFoundSectors) - (int)(nbExpSectors);
    
    // remove track with too much found - expected hits
    if ( nbResHits > m_maxNbResSectors ) 
      continue;
    
    // remove track with too few found - expected sectors
    if ( nbResSectors < m_minNbResSectors ) 
      continue;
    
    if(fullDetail()){
      plot(nbFoundHits, "Control/NbFoundHitsFinal","Number of found hits after full selection",-0.5,24.5,25);
      plot(nbExpHits, "Control/NbExpHitsFinal","Number of expected hits after full selection",-0.5,24.5,25);
      plot(nbResHits, "Control/NbResHitsFinal","Number of found - expected hits after full selection",-5.5,7.5,13);
      plot(nbFoundSectors, "Control/NbFoundSectorsFinal","Number of found sectors after full selection",-0.5,24.5,25);
      plot(nbExpSectors, "Control/NbExpSectorsFinal","Number of expected sectors after full selection",-0.5,24.5,25);
      plot(nbResSectors, "Control/NbResSectorsFinal","Number of found - expected sectors after full selection",-5.5,7.5,13);
    }
        
    bool foundHit = false;
    std::vector<unsigned int>::iterator iterExp = expectedSectors.begin();
    for ( ; iterExp != expectedSectors.end(); ++iterExp)
    {
      ++m_expectedSector[*iterExp];
      ++m_totalExpected;
      foundHit = false;
      
      for (unsigned int i = 0; i < m_NbrOfCuts; ++i)
	    {
	      foundHit = foundHitInSector(foundHits, *It, *iterExp, m_spacialCuts[i], (sector ->isStereo() && i == m_whichCut[1])||(! sector -> isStereo() &&  i == m_whichCut[0]));
        if (foundHit)
        {
          if ( sector ->isStereo() && i == m_whichCut[1] )
            ++m_totalFound;
          else if ( ! sector -> isStereo() &&  i == m_whichCut[0] )
            ++m_totalFound;
          ++m_foundSector[i][*iterExp]; 
	      }
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
                                     const double resCut,
                                     const bool toplot ) const
{
  BOOST_FOREACH(ISTClusterCollector::Hit aHit, hits)
    {
      if ( aHit.cluster->channelID().uniqueSector() == testsector ){
        if(fullDetail()&&toplot){
          plot(aHit.residual, "Control/HitResidual","Hit residual",-resCut,resCut,static_cast<unsigned int>(resCut*100+0.01)*2);
          plot(aHit.cluster->totalCharge(), "Control/HitCharge","Hit charge",0.,200.,200);
        }
        if(m_resPlot&&toplot){
          std::string name("perSector/Residuals_");
          name += aHit.cluster->sectorName();
          plot(aHit.residual, name,"Hit residual",-resCut,resCut,static_cast<unsigned int>(resCut*100+0.01)*2);
        }
        if( fabs(aHit.residual) < resCut ){
          if ( !( m_everyHit || track -> isOnTrack( LHCbID( aHit.cluster -> channelID() ) ) ) )
            continue;
          if( aHit.cluster->totalCharge() < m_minCharge )
            continue;
          return true;
        }
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
 
  std::vector<double>::const_iterator cutBegin( m_spacialCuts.begin() ),
    cutEnd( m_spacialCuts.end() ), cutIt;

  STChannelID channelID;
  std::string nick, root;

  unsigned int i, presentCut( 0u );

  //  double totExpected = 0; double totFound = 0;
  double nExpected, nFound, err;
  double eff  = 999.0;
  
  Histo2DProperties* prop;
  Histo2DProperties* propSectorStatus;
  Histo2DProperties* propNbHits;
  Histo2DProperties* propNbFoundHits;
  
  if(m_detType=="IT"){
    prop = new ST::ITDetectorPlot( "EfficiencyPlot", "Efficiency Plot" );
    propSectorStatus = new ST::ITDetectorPlot( "SectorStatus", "Sector Status Plot" );
    propNbHits = new ST::ITDetectorPlot( "NbExpHits", "Number of Expected Hits Plot" );
    propNbFoundHits = new ST::ITDetectorPlot( "NbFoundHits", "Number of Found Hits Plot" );
  }else{
    prop = new ST::TTDetectorPlot( "EfficiencyPlot", "Efficiency Plot" );
    propSectorStatus = new ST::TTDetectorPlot( "SectorStatus", "Sector Status Plot" );
    propNbHits = new ST::TTDetectorPlot( "NbExpHits", "Number of Expected Hits Plot" );
    propNbFoundHits = new ST::TTDetectorPlot( "NbFoundHits", "Number of Found Hits Plot" );
  }
  
  IHistogram2D* histoEff = 0;
  if(m_effPlot){
    histoEff = book2D( prop->name() , prop->title(),
                       prop->minBinX(), prop->maxBinX(), prop->nBinX(),
                       prop->minBinY(), prop->maxBinY(), prop->nBinY());
  }
  
  IProfile2D* histoSectorStatus = bookProfile2D( propSectorStatus->name() , propSectorStatus->title(),
                                                 propSectorStatus->minBinX(), propSectorStatus->maxBinX(), propSectorStatus->nBinX(),
                                                 propSectorStatus->minBinY(), propSectorStatus->maxBinY(), propSectorStatus->nBinY());
  
  IHistogram2D* histoNbHits = book2D( propNbHits->name() , propNbHits->title(),
                                      propNbHits->minBinX(), propNbHits->maxBinX(), propNbHits->nBinX(),
                                      propNbHits->minBinY(), propNbHits->maxBinY(), propNbHits->nBinY());
  
  IHistogram2D* histoNbFoundHits = book2D( propNbFoundHits->name() , propNbFoundHits->title(),
                                           propNbFoundHits->minBinX(), propNbFoundHits->maxBinX(), propNbFoundHits->nBinX(),
                                           propNbFoundHits->minBinY(), propNbFoundHits->maxBinY(), propNbFoundHits->nBinY());
  
  for (; iterS != m_nameMapSector.end(); ++iterS )
  {
    channelID = iterS -> second -> elementID();
    nick   = iterS -> second -> nickname();
    iterS->second->isStereo() == true ? presentCut = m_whichCut[1]: presentCut = m_whichCut[0] ;
      
    root = "perSector";
    
    // how many were expected ?
    nExpected = static_cast<double>( m_expectedSector[ iterS -> first ] );
    
    if (nExpected >= 1)
    {
      if(fullDetail())
        plot(-10., root + "/NbFoundHits_" + nick,
             "Number of Found Hits vs Cut",
             m_spacialCuts.front() - m_binSize / 2.,
             m_spacialCuts.back() + m_binSize / 2.,
             m_binNumber, nExpected);
      if(fullDetail())
        plot(-10., root + "/NbExpHits_" + nick,
             "Number of Found Hits vs Cut",
             m_spacialCuts.front() - m_binSize / 2.,
             m_spacialCuts.back() + m_binSize / 2.,
             m_binNumber, nExpected);
      if(m_effPlot){
        if(fullDetail())
          profile1D(-10., nExpected, root + "/Eff_" + nick,
                    "Efficiency vs Cut",
                    m_spacialCuts.front() - m_binSize / 2.,
                    m_spacialCuts.back() + m_binSize / 2.,
                    m_binNumber);
      }
      if(m_detType=="IT"){
        ST::ITDetectorPlot::Bins theBins = static_cast<ST::ITDetectorPlot*>(propSectorStatus)->toBins(channelID); 
        histoSectorStatus -> fill( theBins.xBin, theBins.yBin, 1 );
      }else{
        ST::TTDetectorPlot::Bins theBins = 
          static_cast<ST::TTDetectorPlot*>(propSectorStatus)->toBins(static_cast<DeTTSector*>(iterS->second)); 
        for(int ybin(theBins.beginBinY);ybin<theBins.endBinY;ybin++){
          histoSectorStatus -> fill( theBins.xBin, ybin, 1 );
        }
      }
      
      for (i = 0; i < m_NbrOfCuts; ++i)
	    {
	      nFound = static_cast<double>( m_foundSector[i][iterS->first] );
	      eff = 100. * nFound / nExpected;
	      err = sqrt( eff * (100. - eff) / nExpected );
       
        if(fullDetail())
          plot(m_spacialCuts[i], root + "/NbFoundHits_" + nick,
               "Number of Found Hits vs Cut",
               m_spacialCuts.front() - m_binSize / 2.,
               m_spacialCuts.back() + m_binSize / 2.,
               m_binNumber, nFound);
        if(fullDetail())
          plot(m_spacialCuts[i], root + "/NbExpHits_" + nick,
               "Number of Exp Hits vs Cut",
               m_spacialCuts.front() - m_binSize / 2.,
               m_spacialCuts.back() + m_binSize / 2.,
               m_binNumber, nExpected);
        if(m_effPlot){
          if(fullDetail())
            profile1D(m_spacialCuts[i], eff, root + "/Eff_" + nick,
                      "Efficiency vs cut",
                      m_spacialCuts.front() - m_binSize / 2.,
                      m_spacialCuts.back() + m_binSize / 2.,
                      m_binNumber);
        }
        if ( i == presentCut )
        {
          verbose() << nick << ' ' << eff
                    << " +/- " << err << " (found " << nFound
                    << " for cut " << m_spacialCuts[i] << ")" << endmsg;
          
          if(m_detType=="IT"){
            ST::ITDetectorPlot::Bins theBins = static_cast<ST::ITDetectorPlot*>(propSectorStatus)->toBins(channelID); 
            if(m_effPlot)
              histoEff -> fill( theBins.xBin, theBins.yBin, eff );
            histoNbHits -> fill( theBins.xBin, theBins.yBin, nExpected );
            histoNbFoundHits -> fill( theBins.xBin, theBins.yBin, nFound );
          }else{
            ST::TTDetectorPlot::Bins theBins = 
              static_cast<ST::TTDetectorPlot*>(propSectorStatus)->toBins(static_cast<DeTTSector*>(iterS->second)); 
            for(int ybin(theBins.beginBinY);ybin<theBins.endBinY;ybin++){
              if(m_effPlot)
                histoEff -> fill( theBins.xBin, ybin, eff );
              histoNbHits -> fill( theBins.xBin, ybin, nExpected );
              histoNbFoundHits -> fill( theBins.xBin, ybin, nFound );
            }
          }
        }
	    } // i
    } //nExpected
    else
    {
      if(m_detType=="IT"){
        ST::ITDetectorPlot::Bins theBins = static_cast<ST::ITDetectorPlot*>(propSectorStatus)->toBins(channelID); 
        if(m_effPlot)
          histoEff -> fill( theBins.xBin, theBins.yBin, -1 );
        histoSectorStatus -> fill( theBins.xBin, theBins.yBin, -1 );
        histoNbHits -> fill( theBins.xBin, theBins.yBin, 0 );
        histoNbFoundHits -> fill( theBins.xBin, theBins.yBin, 0 );
      }else{
        ST::TTDetectorPlot::Bins theBins = 
          static_cast<ST::TTDetectorPlot*>(propSectorStatus)->toBins(static_cast<DeTTSector*>(iterS->second));
        for(int ybin(theBins.beginBinY);ybin<theBins.endBinY;ybin++){
          if(m_effPlot)
            histoEff -> fill( theBins.xBin, ybin, -1 );
          histoSectorStatus -> fill( theBins.xBin, ybin, -1 );
          histoNbHits -> fill( theBins.xBin, ybin, 0 );
          histoNbFoundHits -> fill( theBins.xBin, ybin, 0 );
        }
      }
    }
  } // iterS
  /*
  if(m_detType=="IT"){
    delete static_cast<ST::ITDetectorPlot*>(prop);
    delete static_cast<ST::ITDetectorPlot*>(propSectorStatus);
    delete static_cast<ST::ITDetectorPlot*>(propNbHits);
    delete static_cast<ST::ITDetectorPlot*>(propNbFoundHits);
  }else{
    delete static_cast<ST::TTDetectorPlot*>(prop);
    delete static_cast<ST::TTDetectorPlot*>(propSectorStatus);
    delete static_cast<ST::TTDetectorPlot*>(propNbHits);
    delete static_cast<ST::TTDetectorPlot*>(propNbFoundHits);
  }
  */

  delete prop;
  delete propSectorStatus;
  delete propNbHits;
  delete propNbFoundHits;
  
  return TrackMonitorBase::finalize();  // must be called after all other actions
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

/**
 *
 */
bool STEfficiency::hasMinStationPassed(LHCb::Track* const& aTrack) const
{
  std::set<unsigned int> stations;
  const std::vector<LHCb::LHCbID>& ids = aTrack -> lhcbIDs();

  for (std::vector<LHCb::LHCbID>::const_iterator iter = ids.begin(); iter != ids.end(); ++iter){
    if (m_detType == "IT"){
      if (iter->isIT() == true ) 
        stations.insert( iter->stID().station() );
    }else{
      if (iter->isTT() == true ) 
        stations.insert( iter->stID().station() );
    }
  }

  if(fullDetail())
    plot(stations.size(), "Control/NbCrossedStation","Number of stations crossed by the track",-0.5,3.5,4);

  if(stations.size() >= m_minStationPassed)
    return true;
  
  return false;
}

//=============================================================================
