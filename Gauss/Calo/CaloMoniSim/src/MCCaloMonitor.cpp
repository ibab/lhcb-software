// $Id: MCCaloMonitor.cpp,v 1.1.1.1 2004-10-25 08:53:25 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files
// ============================================================================
/// STL
// ============================================================================
#include <stdio.h>
#include <string>
#include <algorithm>
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/Stat.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "Kernel/CaloCellID.h"
// ============================================================================
/// from Event
// ============================================================================
#include "Event/MCHit.h"
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
#include "Event/MCCaloDigit.h"
#include "Event/CaloDigit.h"
#include "Event/MCCaloSensPlaneHit.h"
// ============================================================================
// Gaudi Histogramas:
// ============================================================================
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
// ============================================================================
/// CaloKernel
// ============================================================================
#include "CaloKernel/CaloException.h"
#include "CaloKernel/CaloCollection.h"
// ============================================================================
// local
// ============================================================================
#include "MCCaloMonitor.h"
// ============================================================================

//-----------------------------------------------------------------------------
/** @file
 *  Implementation file for class MCCaloMonitor
 * 
 *  @date 24-11-2003 
 *  @author Carlos Gonzalez
 */
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<MCCaloMonitor>          s_factory ;
const        IAlgFactory& MCCaloMonitorFactory = s_factory ;


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCCaloMonitor::MCCaloMonitor( const std::string& name,
			      ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator )
    , m_nameOfMCHits              ( "" )
    , m_GeometryRoot        ( "/dd/Structure/LHCb/" )
    , m_DivMonitor                ( false      )
    , m_MaxE                 (  10 * MeV  )
    , m_MinE                 (   0 * MeV  )
    , m_Threshold                 ( 1.5 * MeV  )
    , m_MaxT                   (  7   )
    , m_MinT                   (  -1   )
    , m_hDir                  ( " "   )
    , m_nEvents                   (   0   )
    , m_DetectorName              ( " "   ) 
    , m_textNorm ("Norm")
{
  declareProperty("MCHits"          , m_nameOfMCHits          );
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
}

//=============================================================================
// Destructor
//=============================================================================
MCCaloMonitor::~MCCaloMonitor() {};

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode MCCaloMonitor::initialize() {
  
  MsgStream msg(msgSvc(), name());
  msg << MSG::INFO << "==> Initialise Monitoring " << m_Detector << endreq;

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
  

  m_hSHitEnergy=histoSvc()->book(m_hDir,11,m_hName1,100,m_MinE,m_MaxE);
  m_hEnergyEnergy=histoSvc()->book(m_hDir,12,m_hName2,100,m_MinE,m_MaxE);
  m_hEnergyTime=histoSvc()->book(m_hDir,13,m_hName3,m_Bin,MinT,MaxT);

  if ( m_DivMonitor ) {  
    m_hSHitEnergyInn=histoSvc()->book(m_hDir,111,m_hName1a,100,m_MinE,m_MaxE);
    m_hSHitEnergyMid=histoSvc()->book(m_hDir,112,m_hName1b,100,m_MinE,m_MaxE);
    m_hSHitEnergyOut=histoSvc()->book(m_hDir,113,m_hName1c,100,m_MinE,m_MaxE);
    m_hEnergyEnergyInn=histoSvc()->book(m_hDir,121,m_hName2a,100,m_MinE,m_MaxE);
    m_hEnergyEnergyMid=histoSvc()->book(m_hDir,122,m_hName2b,100,m_MinE,m_MaxE);
    m_hEnergyEnergyOut=histoSvc()->book(m_hDir,123,m_hName2c,100,m_MinE,m_MaxE);
    m_hEnergyTimeInn=histoSvc()->book(m_hDir,131,m_hName3a,m_Bin,MinT,MaxT);
    m_hEnergyTimeMid=histoSvc()->book(m_hDir,132,m_hName3b,m_Bin,MinT,MaxT);
    m_hEnergyTimeOut=histoSvc()->book(m_hDir,133,m_hName3c,m_Bin,MinT,MaxT);
  }

  m_hDistribution=histoSvc()->book(m_hDir,14,m_hName4,300,0,3000);
  
  m_hDistributionBC=histoSvc()->book(m_hDir,140,m_hName40,100,0,1000);
  m_hDistributionBC0=histoSvc()->book(m_hDir,141,m_hName41,100,0,1000);
  m_hDistributionBC1=histoSvc()->book(m_hDir,142,m_hName42,100,0,1000);
  m_hDistributionBC2=histoSvc()->book(m_hDir,143,m_hName43,100,0,1000);
  m_hDistributionBC3=histoSvc()->book(m_hDir,144,m_hName44,100,0,1000);
  m_hDistributionBC4=histoSvc()->book(m_hDir,145,m_hName45,100,0,1000);
  
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode MCCaloMonitor::execute() {
  
  ++m_nEvents;
  m_nEvents  = 1;
  MsgStream msg( msgSvc(), name() );
  msg << MSG::INFO << "Execute Monitoring " << m_Detector << endreq;


  MCCaloHits::const_iterator MCcaloDig;
  
  // Get the MCHits
  SmartDataPtr< MCCaloHits > MCDigs (eventSvc(),m_nameOfMCHits);
  if( 0 == MCDigs ) {
    msg << MSG::ERROR << "Cannot locate MCDigs in " << m_Detector << endreq;
    return StatusCode::FAILURE ;
  }  
  // Get the MCParticles
  SmartDataPtr< MCParticles > mcParts (eventSvc(),MCParticleLocation::Default);
  if( 0 == mcParts ) {
    msg << MSG::ERROR << "Cannot locate mcParts in "<< m_Detector << endreq;
    return StatusCode::FAILURE ;
  }
  SmartDataPtr<DeCalorimeter> detector( detSvc() ,m_GeometryRoot + m_Detector );
  if( 0 == detector ) {
    msg << MSG::ERROR << 
      "Cannot locate Detector Element ="<< m_GeometryRoot+m_Detector << endreq;
    return StatusCode::FAILURE ;
  }
  int hits_Counter = 0;

  int hits_CounterBC_1 = 0;
  int hits_CounterBC0 = 0;
  int hits_CounterBC1 = 0;
  int hits_CounterBC2 = 0;
  int hits_CounterBC3 = 0;
  int hits_CounterBC4 = 0;
  
  for ( MCcaloDig = MCDigs->begin(); MCcaloDig != MCDigs->end() ; ++MCcaloDig ){
    
    // NORMAL ENERGY SUBHITS HISTOGRAMS
    int binTime;
    if ( (*MCcaloDig)->time() >= 7){ 
      binTime = 7 ;
    } 
    else {
      binTime=(*MCcaloDig)->time();
    }
    CaloCellID ID = (*MCcaloDig)->cellID();
    int zone = ID.area();
    m_hSHitEnergy->fill((*MCcaloDig)->e(),1./m_nEvents) ;
    m_hEnergyEnergy->fill((*MCcaloDig)->e(),((*MCcaloDig)->e())/m_nEvents);
    m_hEnergyTime->fill( binTime,((*MCcaloDig)->e())/m_nEvents);
    if (m_DivMonitor) {
      if ( 0 == zone ){
	if ( detector->valid(ID) ){
	  m_hSHitEnergyInn->fill((*MCcaloDig)->e(),1./m_nEvents) ;
	  m_hEnergyEnergyInn->fill((*MCcaloDig)->e(),((*MCcaloDig)->e())/m_nEvents);
	  m_hEnergyTimeInn->fill(binTime,((*MCcaloDig)->e())/m_nEvents);
	}
      }
      if ( 1 == zone ){
	if ( detector->valid(ID) ){
	  m_hSHitEnergyMid->fill((*MCcaloDig)->e(),1./m_nEvents) ;
	  m_hEnergyEnergyMid->fill((*MCcaloDig)->e(),((*MCcaloDig)->e())/m_nEvents);
	  m_hEnergyTimeMid->fill(binTime,((*MCcaloDig)->e())/m_nEvents);
	}
      }
      if ( 2 == zone ) {
	if ( detector->valid(ID) ){
	  m_hSHitEnergyOut->fill((*MCcaloDig)->e(),1./m_nEvents) ;
	  m_hEnergyEnergyOut->fill((*MCcaloDig)->e(),((*MCcaloDig)->e())/m_nEvents);
	  m_hEnergyTimeOut->fill(binTime,((*MCcaloDig)->e())/m_nEvents);
	}
      }
    }
    // BUNCH CROSSING HISTOGRAMS
    if ( (*MCcaloDig)->e() >= m_Threshold ){
      hits_Counter = hits_Counter + 1;
      switch ((*MCcaloDig)->time()){
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
  
  m_hDistribution->fill(hits_Counter,1.);
  m_hDistributionBC->fill(hits_CounterBC_1,1.);
  m_hDistributionBC0->fill(hits_CounterBC0,1.);
  m_hDistributionBC1->fill(hits_CounterBC1,1.);
  m_hDistributionBC2->fill(hits_CounterBC2,1.);
  m_hDistributionBC3->fill(hits_CounterBC3,1.);
  m_hDistributionBC4->fill(hits_CounterBC4,1.); 
 
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MCCaloMonitor::finalize() {
  
  MsgStream msg(msgSvc(), name());
  msg << MSG::INFO << "Finalize Monitoring " << m_Detector << endreq;
  return StatusCode::SUCCESS;
}

//=============================================================================

// ============================================================================
// The END 
// ============================================================================
