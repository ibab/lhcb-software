// $Id: MCCaloMonitor.cpp,v 1.10 2009-10-23 10:45:22 jonrob Exp $

// Include files

// STL
#include <stdio.h>
#include <string>
#include <algorithm>

// from Gaudi
#include "Kernel/CaloCellID.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
// from Event
#include "Event/MCHit.h"
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
#include "Event/MCCaloDigit.h"
// OD : RENAMING >>>> WAIT FOR NEW NAME/LOCATION - COMMENTED IN THE MEANTIME
//#include "Event/MCCaloSensPlaneHit.h"

// Gaudi Histograms:
#include "GaudiAlg/GaudiHistoAlg.h"
// CaloKernel
#include "CaloKernel/CaloException.h"
#include "CaloKernel/CaloCollection.h"

// local
#include "MCCaloMonitor.h"
using namespace LHCb;

//-----------------------------------------------------------------------------
/** @file
 *  Implementation file for class MCCaloMonitor
 *
 *  @date 24-11-2003
 *  @author Carlos Gonzalez
 */
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MCCaloMonitor )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCCaloMonitor::MCCaloMonitor( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
  , m_nameOfMCHits              ( "" )
  , m_GeometryRoot        ( "/dd/Structure/LHCb/DownstreamRegion/" )
  , m_DivMonitor                ( false      )
  , m_MaxE                 (  10 * Gaudi::Units::MeV  )
  , m_MinE                 (   0 * Gaudi::Units::MeV  )
  , m_Threshold                 ( 1.5 * Gaudi::Units::MeV  )
  , m_MaxT                   (  7   )
  , m_MinT                   (  -1   )
  , m_hDir                  ( " "   )
  , m_nEvents                   (   0   )
  , m_DetectorName              ( " "   )
  , m_textNorm ("Norm")
{
  declareProperty("GeometryRoot"    , m_GeometryRoot    );
  declareProperty("Regions"         , m_DivMonitor            );
  declareProperty("MaximumEnergy"   , m_MaxE            );
  declareProperty("MinimumEnergy"   , m_MinE            );
  declareProperty("MaximumTime"     , m_MaxT              );
  declareProperty("Threshold"       , m_Threshold            );
  declareProperty("MinimumTime"     , m_MinT              );
  declareProperty("hDir"            , m_hDir             );
  declareProperty("Detector"        , m_Detector             );
  declareProperty("DetectorName"    , m_DetectorName         );
  declareProperty("MCParticles"     , m_mcParticles = MCParticleLocation::Default ) ;
}

//=============================================================================
// Destructor
//=============================================================================
MCCaloMonitor::~MCCaloMonitor() {}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode MCCaloMonitor::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  info() << "==> Initialise Monitoring " << m_Detector << endreq;

  m_hDir = m_Detector;
  m_nameOfMCHits = "MC/" + m_Detector + "/Hits";
  if (m_Detector == "Spd"){
    m_DetectorName = "SPD";
  }
  if (m_Detector == "Prs"){
    m_DetectorName = "PS";
  }
  if (m_Detector == "Ecal"){
    m_DetectorName = "ECAL";
  }
  if (m_Detector == "Hcal"){
    m_DetectorName = "HCAL";
  }

  // Histograms

  m_hName1  = "Subhits in the " + m_DetectorName;
  m_hName1a = "Subhits in the INNER " + m_DetectorName;
  m_hName1b = "Subhits in the MIDDLE " + m_DetectorName;
  m_hName1c = "Subhits in the OUTER " + m_DetectorName;
  m_hName2  = "Energy Weighted Subhits in the " + m_DetectorName;
  m_hName2a = "Energy Weighted Subhits in the INNER " + m_DetectorName;
  m_hName2b = "Energy Weighted Subhits in the MIDDLE " + m_DetectorName;
  m_hName2c = "Energy Weighted Subhits in the OUTER" + m_DetectorName;
  m_hName3  = "Accumulated Energy in the " + m_DetectorName;
  m_hName3a = "Accumulated Energy in the INNER " + m_DetectorName;
  m_hName3b = "Accumulated Energy in the MIDDLE " + m_DetectorName;
  m_hName3c = "Accumulated Energy in the OUTER " + m_DetectorName;
  m_hName4  = "Number of Subhits in the " + m_DetectorName;
  m_hName40 = "Number of Subhits in the " + m_DetectorName + " ( BC = -1 )";
  m_hName41 = "Number of Subhits in the " + m_DetectorName + " ( BC = 0  )";
  m_hName42 = "Number of Subhits in the " + m_DetectorName + " ( BC = 1  )";
  m_hName43 = "Number of Subhits in the " + m_DetectorName + " ( BC = 2  )";
  m_hName44 = "Number of Subhits in the " + m_DetectorName + " ( BC = 3  )";
  m_hName45 = "Number of Subhits in the " + m_DetectorName + " ( BC = 4  )";
  m_Bin = int(m_MaxT-m_MinT+1);
  MinT = 1.*m_MinT - 0.5;
  MaxT = 1.*m_MaxT + 0.501;

  m_detector = getDet<DeCalorimeter>(m_GeometryRoot + m_Detector);
  if( 0 == m_detector ) {
    error() <<
      "Cannot locate Detector Element ="<< m_GeometryRoot+m_Detector << endreq;
    return StatusCode::FAILURE ;
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MCCaloMonitor::execute() {

  ++m_nEvents;
  m_nEvents  = 1;
  debug() << "Execute Monitoring " << m_Detector << endreq;

  MCCaloHits::const_iterator iHit;

  // Get the MCHits
  const MCCaloHits * CaloHits = get<MCCaloHits>(m_nameOfMCHits);

  // Get the MCParticles
  //const MCParticles * mcParts = get<MCParticles>(m_mcParticles);

  int hits_Counter = 0;

  int hits_CounterBC_1 = 0;
  int hits_CounterBC0 = 0;
  int hits_CounterBC1 = 0;
  int hits_CounterBC2 = 0;
  int hits_CounterBC3 = 0;
  int hits_CounterBC4 = 0;

  for ( iHit = CaloHits->begin(); iHit != CaloHits->end() ; ++iHit ){

    // NORMAL ENERGY SUBHITS HISTOGRAMS
    int binTime;
    if ( (*iHit)->time() >= 7){
      binTime = 7 ;
    }
    else {
      binTime=(*iHit)->time();
    }
    CaloCellID ID = (*iHit)->cellID();
    int zone = ID.area();

    plot1D( (*iHit)->activeE(),11,m_hName1,m_MinE,m_MaxE,100,1./m_nEvents) ;
    plot1D( (*iHit)->activeE(),12,m_hName2,m_MinE,m_MaxE,100,((*iHit)->activeE())/m_nEvents) ;
    plot1D( binTime,13,m_hName3,MinT,MaxT,m_Bin,((*iHit)->activeE())/m_nEvents) ;
    if (m_DivMonitor) {
      if ( ( 2 == zone && m_DetectorName != "Hcal" ) ||
           ( 1 == zone && m_DetectorName == "Hcal" ) ){
        if ( m_detector->valid(ID) ){
          plot1D((*iHit)->activeE(),111,m_hName1a,m_MinE,m_MaxE,100,1./m_nEvents) ;
          plot1D((*iHit)->activeE(),121,m_hName2a,m_MinE,m_MaxE,100,((*iHit)->activeE())/m_nEvents) ;
          plot1D(binTime,131,m_hName3a,MinT,MaxT,m_Bin,((*iHit)->activeE())/m_nEvents) ;
        }
      }
      if ( ( 1 == zone   && m_DetectorName != "Hcal" ) ){
        if ( m_detector->valid(ID) ){
          plot1D((*iHit)->activeE(),112,m_hName1b,m_MinE,m_MaxE,100,1./m_nEvents) ;
          plot1D((*iHit)->activeE(),122,m_hName2b,m_MinE,m_MaxE,100,((*iHit)->activeE())/m_nEvents) ;
          plot1D(binTime,132,m_hName3b,MinT,MaxT,m_Bin,((*iHit)->activeE())/m_nEvents) ;
        }
      }
      if ( 0 == zone ) {
        if ( m_detector->valid(ID) ){
          plot1D((*iHit)->activeE(),113,m_hName1c,m_MinE,m_MaxE,100,1./m_nEvents) ;
          plot1D((*iHit)->activeE(),123,m_hName2c,m_MinE,m_MaxE,100,((*iHit)->activeE())/m_nEvents) ;
          plot1D(binTime,133,m_hName3c,MinT,MaxT,m_Bin,((*iHit)->activeE())/m_nEvents) ;
        }
      }
    }
    // BUNCH CROSSING HISTOGRAMS
    if ( (*iHit)->activeE() >= m_Threshold ){
      hits_Counter = hits_Counter + 1;
      switch ((*iHit)->time()){
      case -1:
        ++hits_CounterBC_1;
        break;
      case 0:
        ++hits_CounterBC0 ;
        break;
      case 1:
        ++hits_CounterBC1 ;
        break;
      case 2:
        ++hits_CounterBC2 ;
        break;
      case 3:
        ++hits_CounterBC3;
        break;
      case 4:
        ++hits_CounterBC4;
        break;
      }
    }
  } // end for

  plot1D(hits_Counter,14,m_hName4,0,1000,100,1.);
  plot1D(hits_CounterBC_1,140,m_hName40,0,1000,100,1.);
  plot1D(hits_CounterBC0,141,m_hName41,0,1000,100,1.);
  plot1D(hits_CounterBC1,142,m_hName42,0,1000,100,1.);
  plot1D(hits_CounterBC2,143,m_hName43,0,1000,100,1.);
  plot1D(hits_CounterBC3,144,m_hName44,0,1000,100,1.);
  plot1D(hits_CounterBC4,145,m_hName45,0,1000,100,1.);
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MCCaloMonitor::finalize() {

  info()<< "Finalize Monitoring " << m_Detector << endreq;
  return GaudiHistoAlg::finalize();  // must be called after all other actions
}


// ============================================================================
// The END
// ============================================================================
