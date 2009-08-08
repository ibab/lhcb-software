// $Id: STEfficiency.cpp,v 1.3 2009-08-08 10:59:42 mneedham Exp $
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

// Event
#include "Event/STCluster.h"


// AIDA
#include "AIDA/IProfile1D.h"
#include "AIDA/IHistogram2D.h"

#include "LoKi/select.h"



#include <boost/bind.hpp>
#include <boost/foreach.hpp>

using namespace LHCb;
using namespace ST;
using namespace AIDA;

//-----------------------------------------------------------------------------
// Implementation file for class : STEfficiency
//
// 2009-06-16 : Johan Luisier
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( STEfficiency );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
STEfficiency::STEfficiency( const std::string& name,
			      ISvcLocator* pSvcLocator)
  : TrackMonitorBase ( name , pSvcLocator )
{
  declareProperty("DetType"         , m_detType =  "IT" );
  declareProperty("ExpectedHitsTool", m_expectedHitsTool = "ITHitExpectation");
  declareProperty("Cut"             , m_spacialCut);
  declareProperty("CollectorName"   , m_collectorName = "STClusterCollector");
  declareProperty("TrackSelName"    , m_trackSelectorName
		  = "ITIsolatedTrackSelector");
  declareProperty("MinExpectedHits" , m_minHits = 10);
  declareProperty("XLayerCut"       , m_xCut = 0.500 );
  declareProperty("StereoLayerCut"  , m_stereoCut = 0.700 );
  declareProperty("ChargeCut"       , m_chargeCut = 0.);

  m_spacialCut.push_back( 500 * Gaudi::Units::um );
  m_spacialCut.push_back( 700 * Gaudi::Units::um );
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
  
  for (unsigned int i = 0 ; i < 5; i++) 
    {
      name = "Mixed";
      if ( i < 4 ) name = ITNames().BoxToString( i + 1 );
      name += "Collector";
      m_collectors.push_back( tool< ISTClusterCollector >( "STClusterCollector", name ) );
    }

  m_expectedHits  = tool< IHitExpectation >( "ITHitExpectation",
					     m_expectedHitsTool );
  m_trackSelector = tool< ITrackSelector >( "ITIsolatedTrackSelector",
					    m_trackSelectorName );


  m_tracker = getDet<DeSTDetector>(DeSTDetLocation::location(m_detType));
  m_NbrOfCuts = m_spacialCut.size();
  m_foundSector.resize(m_NbrOfCuts);
  m_foundLayer.resize(m_NbrOfCuts);

  const DeSTDetector::Sectors& sectors = m_tracker->sectors();
  DeSTDetector::Sectors::const_iterator iterS = sectors.begin();
  for ( ; iterS != sectors.end(); ++iterS){
    STChannelID elemID = (*iterS)->elementID();
    m_expectedSector[elemID.uniqueSector()] = 0u;
    m_expectedLayer[elemID.uniqueDetRegion()] = 0u;
    m_nameMapSector[elemID.uniqueSector()] = *iterS;
    m_nameMapLayer[elemID.uniqueDetRegion()] = *iterS;
       
    for (unsigned int i=0 ; i < m_NbrOfCuts; ++i ){
      m_foundSector[i][elemID.uniqueSector()] = 0u; 
      m_foundLayer[i][elemID.uniqueSector()] = 0u;       
    }

  } // iterS


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
  
  // Loop on tracks
  for (It = Begin; It != End; It++) {
      // Clearing all the std::vectors !
      expectedHits.clear();
      expectedSectors.clear();
      expectedLayers.clear();
      itHits.clear();
      output.clear();

      // collect the expected hits
      m_expectedHits -> collect( **It, expectedHits );
      plot(expectedHits.size(), "expected", "# expected", -0.5, 20.5, 21);
      if (expectedHits.size() < m_minHits) continue; // no hits to find

      if ( !m_trackSelector -> accept( **It )) continue;
      
      // for processing convert the expected hits to unique sectors
      BOOST_FOREACH(LHCbID id, expectedHits)
	{
	  expectedSectors.push_back(id.stID().uniqueSector());
	  expectedLayers.push_back(id.stID().uniqueDetRegion());
	}
      
      filterNameList( expectedSectors );
      filterNameList( expectedLayers );

      const std::vector<LHCb::LHCbID>& ids = (*It) -> lhcbIDs();
      itHits.reserve(ids.size());
      LoKi::select(ids.begin(), ids.end(), back_inserter(itHits),
		   bind(&LHCbID::isIT,_1));

      type = ITCategory(itHits);

      plot(type, "type", "Type of the track", .5, 5.5, 5);

      // collect hits
      m_collectors[type - 1] -> execute( **It, output );
      //m_collector -> execute( **It, output );

      plot(output.size(), "collected", "collected", -0.5, 20.5, 21);
      
      std::vector<unsigned int>::iterator iterExp = expectedSectors.begin();
      for ( ; iterExp != expectedSectors.end(); ++iterExp){
	  ++m_expectedSector[*iterExp];
	  for (unsigned int i = 0; i < m_NbrOfCuts; ++i){
	      if (foundHitInSector(output, *iterExp, m_spacialCut[i])){
		  ++m_foundSector[i][*iterExp]; 
	      }
	    } // loop cuts
	} // loop expected

      for (iterExp = expectedLayers.begin(); iterExp != expectedLayers.end();++iterExp){
          ++m_expectedLayer[*iterExp];
          for (unsigned int i = 0; i < m_NbrOfCuts; ++i){
              if (foundHitInLayer(output, *iterExp, m_spacialCut[i])){
                  ++m_foundLayer[i][*iterExp];
                }
            } // loop cuts
        } // loop expected

    } // loop tracks
  
  return StatusCode::SUCCESS;
}

bool STEfficiency::foundHitInSector( const ISTClusterCollector::Hits& hits, 
				      const unsigned int testsector,
				      const double resCut  ) const
{
  bool found = false;

  BOOST_FOREACH(ISTClusterCollector::Hit aHit, hits){
    if (aHit.cluster->channelID().uniqueSector() == testsector && fabs(aHit.residual) < resCut) {
       const DeSTSector* sector = m_tracker -> findSector( aHit.cluster -> channelID() );
       const unsigned int beetle = sector -> beetle( aHit.cluster -> channelID());
       const double sigToNoise = aHit.cluster -> totalCharge() / sector->beetleNoise(beetle);
       if ( sigToNoise < m_chargeCut ) {
         continue;
       }
       else {
         found = true;
         break;
       }
    }
  } // forach
  
  return found;
}

bool STEfficiency::foundHitInLayer( const ISTClusterCollector::Hits& hits, 
				     const unsigned int testlayer,
				     const double resCut  ) const
{
  bool found = false;

  BOOST_FOREACH(ISTClusterCollector::Hit aHit, hits){
    if (aHit.cluster->channelID().uniqueDetRegion() == testlayer &&
	  fabs(aHit.residual) < resCut) {
      const DeSTSector*  sector = m_tracker -> findSector( aHit.cluster -> channelID() );
      const unsigned int beetle = sector -> beetle( aHit.cluster -> channelID());
      const double sigToNoise = aHit.cluster -> totalCharge()/sector->beetleNoise(beetle);
      if ( sigToNoise < m_chargeCut ) {
        continue;
      }
      else {
       found = true;
       break;
      }
    }
  } // forach
  
  return found;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode STEfficiency::finalize()
{
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  std::map<unsigned int, DeSTSector*>::const_iterator iterS = m_nameMapSector.begin();

 
  std::vector<double>::const_iterator cutBegin( m_spacialCut.begin() ),
    cutEnd( m_spacialCut.end() ), cutIt;

  STChannelID channelID;
  std::string nick, root;

  unsigned int i;

  double nExpected, nFound, err, eff;

  ITDetectorPlot prop( "EffciencyPlot", "EfficiencyPlot" );

  IHistogram2D* histo = book2D( prop.name() , prop.title(), prop.minBinX(), prop.maxBinX(),
				prop.nBinX(), prop.minBinY(), prop.maxBinY(), prop.nBinY());


  // get the defaults used to quote cuts for the x and stereo ladders
  unsigned int whichCut[2] = { 0, 0 }, presentCut(0); 
  for (cutIt = cutBegin; cutIt != cutEnd; cutIt++){
      if ( fabs( m_xCut - *cutIt ) < .005 )
	whichCut[0] = static_cast<unsigned int>(cutIt - cutBegin);
      else if ( fabs( m_stereoCut - *cutIt ) < .005 )
	whichCut[1] = static_cast<unsigned int>(cutIt - cutBegin);
  } // loop cuts

  for (; iterS != m_nameMapSector.end(); ++iterS ) {

      channelID = iterS -> second -> elementID();
      nick   = iterS -> second -> nickname();
      iterS->second->isStereo() == true ? presentCut = whichCut[1]: presentCut = whichCut[0] ;
      
      if (m_detType == "IT"){
        root = ITNames().UniqueBoxToString(channelID);
      } 

      // how many were expected ?
      nExpected = m_expectedSector[ iterS -> first ];
      
      if (nExpected > 1) {
	  for (i = 0; i < m_NbrOfCuts; ++i) {       
	      nFound = m_foundSector[i][iterS->first]; 
	      eff = 100 * nFound / nExpected;
	      err = sqrt( eff * (100. - eff) / nExpected );
	      profile1D(m_spacialCut[i], eff, root + "/Eff_" + nick,
			"Efficiency vs cut", 0., m_spacialCut.back(),
			static_cast<unsigned int>(1000 * m_spacialCut.back()));
	      if ( i == presentCut ) {
		  verbose() << nick << ' ' << eff
			    << " +/- " << err << " (found " << nFound
			    << " for cut " << m_spacialCut[i] << ")" << endmsg;
	       
  		  ST::ITDetectorPlot::Bins theBins = prop.toBins(channelID); 
		  histo -> fill( theBins.xBin, theBins.yBin, eff );
	      }
	  } // i
      } //nExpected
    } // iterS

  for (iterS = m_nameMapLayer.begin(); iterS != m_nameMapLayer.end(); ++iterS )
    {
      channelID = iterS -> second -> elementID();
      iterS->second->isStereo() == true ? presentCut = whichCut[1]: presentCut = whichCut[0] ;
      if (m_detType == "IT") {
        nick  = ITNames().UniqueLayerToString(channelID);
      }

      root = "Layers";

      // how many were expected ?
      nExpected = m_expectedLayer[ iterS -> first ];
      
      if (nExpected > 1) {
	for (i = 0; i < m_NbrOfCuts; ++i)
	  {
	    nFound = m_foundLayer[i][iterS->first]; 
	    eff = 100 * nFound/nExpected;
	    err = sqrt( eff * (100. - eff) / nExpected );
	    profile1D(m_spacialCut[i], eff, root + "/Eff_" + nick,
		      "Efficiency vs cut", 0., m_spacialCut.back(),
		      static_cast<unsigned int>(1000 * m_spacialCut.back()));
	    if ( i == presentCut )
	      {
		info() << std::setw(11) << std::left << nick << ' ' << nFound
		       << " found tracks, cut = " << m_spacialCut[i] << ' '
		       << /*setprecision(2) <<*/ eff << " +/- " << err << endmsg;
                
	      }
	  } // i
      } // nExpected
    } // iterS
      
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

//=============================================================================
