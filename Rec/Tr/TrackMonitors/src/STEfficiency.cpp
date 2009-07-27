// $Id: STEfficiency.cpp,v 1.2 2009-07-27 13:44:46 jluisier Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "STEfficiency.h"


#include "TrackInterfaces/IHitExpectation.h"
#include "TrackInterfaces/ITrackSelector.h"
#include "Kernel/ISTChannelIDSelector.h"
#include "STDet/DeSTDetector.h"
#include "Event/STCluster.h"
#include "STDet/DeSTSector.h"

#include "Kernel/STLexicalCaster.h"

#include "GaudiKernel/PhysicalConstants.h"

#include "AIDA/IProfile1D.h"
#include "AIDA/IHistogram2D.h"

#include "LoKi/select.h"

#include "Kernel/ITDetectorPlot.h"

#include <boost/bind.hpp>
#include <boost/foreach.hpp>

using namespace std;
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
STEfficiency::STEfficiency( const string& name,
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

  string name("");
  
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

  vector<LHCbID> expectedHits;
  vector<unsigned int> expectedSectors, expectedLayers;
  ISTClusterCollector::Hits output;

  Category type;
  vector<LHCb::LHCbID> itHits;
  
  // Loop on tracks
  for (It = Begin; It != End; It++)
    {
      // Clearing all the vectors !
      expectedHits.clear();
      expectedSectors.clear();
      expectedLayers.clear();
      itHits.clear();
      output.clear();

      // collect the expected hits
      m_expectedHits -> collect( **It, expectedHits );
      plot(expectedHits.size(), "expected", "# expected", -0.5, 20.5, 21);
      if (expectedHits.size() < m_minHits) continue; // no hits to find

      if ( !m_trackSelector -> accept( **It ) )
	{
	  continue;
	}
      
      // for processing convert the expected hits to unique sectors
      BOOST_FOREACH(LHCbID id, expectedHits)
	{
	  expectedSectors.push_back(id.stID().uniqueSector());
	  expectedLayers.push_back(id.stID().uniqueDetRegion());
	}
      
      filterNameList( expectedSectors );
      filterNameList( expectedLayers );

      const vector<LHCb::LHCbID>& ids = (*It) -> lhcbIDs();
      itHits.reserve(ids.size());
      LoKi::select(ids.begin(), ids.end(), back_inserter(itHits),
		   bind(&LHCbID::isIT,_1));

      type = ITCategory(itHits);

      plot(type, "type", "Type of the track", .5, 5.5, 5);

      // collect hits
      m_collectors[type - 1] -> execute( **It, output );
      //m_collector -> execute( **It, output );

      plot(output.size(), "collected", "collected", -0.5, 20.5, 21);
      
      //      cout << "-----" << endl;
      vector<unsigned int>::iterator iterExp = expectedSectors.begin();
      for ( ; iterExp != expectedSectors.end(); ++iterExp)
	{
	  //	cout << *iterExp << endl; 
	  ++m_expectedSector[*iterExp];
	  for (unsigned int i = 0; i < m_NbrOfCuts; ++i)
	    {
	      if (foundHitInSector(output, *iterExp, m_spacialCut[i]))
		{
		  ++m_foundSector[i][*iterExp]; 
		}
	    } // loop cuts
	} // loop expected

      for (iterExp = expectedLayers.begin(); iterExp != expectedLayers.end();
	   ++iterExp)
        {
          //    cout << *iterExp << endl;
          ++m_expectedLayer[*iterExp];
          for (unsigned int i = 0; i < m_NbrOfCuts; ++i)
            {
              if (foundHitInLayer(output, *iterExp, m_spacialCut[i]))
                {
                  ++m_foundLayer[i][*iterExp];
                }
            } // loop cuts
        } // loop expected

      
      //      cout << "found " << endl;
      
      //  BOOST_FOREACH(ISTClusterCollector::Hit aHit, output){
      //  cout << aHit.cluster->channelID().uniqueSector() << endl;
      // }
      //cout << "----" << endl;
      
    } // loop tracks
  
  return StatusCode::SUCCESS;
}

bool STEfficiency::foundHitInSector( const ISTClusterCollector::Hits& hits, 
				      const unsigned int testsector,
				      const double resCut  ) const
{
  bool found = false;

  unsigned int beetle;
  DeSTSector *sector;
  double noise, sigToNoise;

  BOOST_FOREACH(ISTClusterCollector::Hit aHit, hits)
    {
      if (aHit.cluster->channelID().uniqueSector() == testsector &&
	  abs(aHit.residual) < resCut)
	{
	  sector = m_tracker -> findSector( aHit.cluster -> channelID() );
	  beetle = sector -> beetle( aHit.cluster -> channelID());
	  noise = sector -> beetleNoise( beetle );
	  
	  sigToNoise = aHit.cluster -> totalCharge() / noise;

	  if ( sigToNoise < m_chargeCut ) continue;

	  found = true;
	  break;
	}
    } // forach
  
  return found;
}

bool STEfficiency::foundHitInLayer( const ISTClusterCollector::Hits& hits, 
				     const unsigned int testlayer,
				     const double resCut  ) const
{
  bool found = false;

  unsigned int beetle;
  DeSTSector *sector;
  double noise, sigToNoise;

  BOOST_FOREACH(ISTClusterCollector::Hit aHit, hits)
    {
      if (aHit.cluster->channelID().uniqueDetRegion() == testlayer &&
	  abs(aHit.residual) < resCut)
	{
	  sector = m_tracker -> findSector( aHit.cluster -> channelID() );
	  beetle = sector -> beetle( aHit.cluster -> channelID());
	  noise = sector -> beetleNoise( beetle );
	  
	  sigToNoise = aHit.cluster -> totalCharge() / noise;

	  if ( sigToNoise < m_chargeCut ) continue;

	  found = true;
	  break;
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

  map<unsigned int, DeSTSector*>::const_iterator iterS = m_nameMapSector.begin();

  unsigned int whichCut[2] = { 0, 0 }, presentCut(0); 

  vector<double>::const_iterator cutBegin( m_spacialCut.begin() ),
    cutEnd( m_spacialCut.end() ), cutIt;

  STChannelID channelID;
  string nick, root;

  unsigned int /*box, station, layer, sector,*/ i;

  double nExpected, nFound, err, eff;

  ITDetectorHistoProperties prop( m_detType );

  IHistogram2D* histo = book2D( prop.histoTitle(), prop.minBinX(), prop.maxBinX(),
				prop.nBinX(), prop.minBinY(), prop.maxBinY(), prop.nBinY());

//   if ( m_detType == "IT" )
//     histo = book2D("IT Efficiencies", prop.minBinX(), prop.maxBinX(),
// 		   prop.nBinX(), prop.minBinY(), prop.maxBinY(), prop.nBinY());
//   else if ( m_detType == "TT" )
//     histo = book2D("TT Efficiencies", prop.minBinX(), prop.maxBinX(),
// 		   prop.nBinX(), prop.minBinY(), prop.maxBinY(), prop.nBinY());
//   else
//     histo = 0;

  for (cutIt = cutBegin; cutIt != cutEnd; cutIt++)
    {
      if ( abs( m_xCut - *cutIt ) < .005 )
	whichCut[0] = static_cast<unsigned int>(cutIt - cutBegin);
      else if ( abs( m_stereoCut - *cutIt ) < .005 )
	whichCut[1] = static_cast<unsigned int>(cutIt - cutBegin);
    }

  for (; iterS != m_nameMapSector.end(); ++iterS )
    {
      channelID = iterS -> second -> elementID();
      nick   = iterS -> second -> nickname();

      // Find which cut has to be applied
      if ( nick.find("X1Sector") != string::npos ||
	   nick.find("X2Sector") != string::npos )
	{
	  presentCut = whichCut[ 0 ];
	  root = nick.substr( 0, nick.find("X") );
	}
      else if ( nick.find("USector") != string::npos )
	{
	  presentCut = whichCut[ 1 ];
	  root = nick.substr( 0, nick.find("USector") );
	}
      else if ( nick.find("VSector") != string::npos )
	{
	  presentCut = whichCut[ 1 ];
	  root = nick.substr( 0, nick.find("VSector") );
	}
      // how many were expected ?
      nExpected = m_expectedSector[ iterS -> first ];
      
      if (nExpected > 1)
	{
	  for (i = 0; i < m_NbrOfCuts; ++i)
	    {       
	      nFound = m_foundSector[i][iterS->first]; 
	      eff = 100 * nFound / nExpected;
	      err = sqrt( eff * (100. - eff) / nExpected );
	      profile1D(m_spacialCut[i], eff, root + "/Eff_" + nick,
			"Efficiency vs cut", 0., m_spacialCut.back(),
			static_cast<unsigned int>(1000 * m_spacialCut.back()));
	      if ( i == presentCut )
		{
		  verbose() << nick << ' ' << eff
			    << " +/- " << err << " (found " << nFound
			    << " for cut " << m_spacialCut[i] << ")" << endmsg;
		  /*
		    station = channelID.station();
		    box     = channelID.detRegion();
		    layer   = channelID.layer();
		    sector  = channelID.sector();
		    
		    debug() << station << ' ' << box << ' '<< layer << ' '
		    << sector << endmsg;
		    
		    yBin = 16*(station - 1);
		    xBin = 0.;
		    
		    if (box == 3 || box == 4)
		    {
		    xBin = 4. - static_cast<double>(sector);
		    if (box == 3) yBin += 4. + static_cast<double>(layer);
		    if (box == 4) yBin += 12. + static_cast<double>(layer);
		    } 
		    else if (box == 1)
		    {
		    yBin += 8. + static_cast<double>(layer);
		    xBin = 11. - static_cast<double>(sector);
		    }
		    else
		    {
		    yBin += 8. + static_cast<double>(layer);
		    xBin = -3. - static_cast<double>(sector);
		    }
		    
		    plot2D(xBin, yBin, "Summary", "IT Efficiencies",
		    -14.5, 14.5, -0.5, 50.5, 29, 51, eff);
		  */

		  if ( prop.setChannelID( channelID ) && histo != 0 )
		    histo -> fill( prop.xBin(), prop.yBin(), eff );
		}
	    }
	} // i
    } // iterS

  for (iterS = m_nameMapLayer.begin(); iterS != m_nameMapLayer.end(); ++iterS )
    {
      channelID = iterS -> second -> elementID();
      nick   = iterS -> second -> nickname();

      // Find which cut has to be applied
      if ( nick.find("X1Sector") != string::npos ||
	   nick.find("X2Sector") != string::npos )
	{
	  presentCut = whichCut[ 0 ];
	  //root = nick.substr( 0, nick.find("X") );
	  nick = nick.substr( 0, nick.find("X") + 2 );
	}
      else if ( nick.find("USector") != string::npos )
	{
	  presentCut = whichCut[ 1 ];
	  //root = nick.substr( 0, nick.find("USector") );
	  nick = nick.substr( 0, nick.find("USector") + 1 );
	}
      else if ( nick.find("VSector") != string::npos )
	{
	  presentCut = whichCut[ 1 ];
	  //root = nick.substr( 0, nick.find("VSector") );
	  nick = nick.substr( 0, nick.find("VSector") + 1 );
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
		info() << setw(11) << left << nick << ' ' << nFound
		       << " found tracks, cut = " << m_spacialCut[i] << ' '
		       << /*setprecision(2) <<*/ eff << " +/- " << err << endmsg;

	      }
	  }
      } // i
    } // iterS
      
  return TrackMonitorBase::finalize();  // must be called after all other actions
}

STEfficiency::Category STEfficiency::ITCategory(const vector<LHCb::LHCbID>& ids) const
{
  typedef map<unsigned int, unsigned int> BoxMap;
  BoxMap nBox;
  BOOST_FOREACH(LHCb::LHCbID id, ids)
    {
      nBox[ id.stID().detRegion() ] += 1;
    } // for each
  
  return nBox.size() == 1 ? Category(nBox.begin() -> first) : Mixed ;
}

void STEfficiency::filterNameList(vector<unsigned int>& vec)
{
  std::sort( vec.begin(), vec.end() );
  std::unique( vec.begin(), vec.end() );
}

//=============================================================================
