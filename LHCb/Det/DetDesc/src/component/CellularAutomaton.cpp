// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/component/CellularAutomaton.cpp,v 1.1 2001-02-03 19:49:17 ibelyaev Exp $
//#define CALORECONS_CELLULARAUTOMATON_CPP

/// Gaudi interfaces
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/IConverter.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/ILVolume.h"
#include "GaudiKernel/IGeometryInfo.h"

/// Gaudi factories for algorithms and converters
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/CnvFactory.h"


/// Gaudi base classes
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/RegistryEntry.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/SmartRef.h"

#include "DetDesc/DetectorElement.h"

/// Utility classes
#include "GaudiKernel/MsgStream.h"

/// For working with Monte-Carlo data objects
#include "LHCbEvent/EventModel.h"
#include "LHCbEvent/Event.h"

// Private includes             
#include "CellularAutomaton.h"
#include "HitsHolder.h"
#include "DetecHolder.h"
#include "SeedsHolder.h"
#include "TaggedCellsHolder.h"
#include "PreClustersHolder.h"
#include "ClustersHolder.h"

//--------------------------------------------------------------------
//
//  CellularAutomaton : Make clusters
//    Input  : CaloDigit data and geometry of detector
//    Output : CelAutoCluster data
//
//  Author  : Nicole Brun
//  Date    : 12 october 2000
//  Changes :   
//
//--------------------------------------------------------------------


static const AlgFactory<CellularAutomaton>    Factory;
const IAlgFactory& CellularAutomatonFactory = Factory;


// Constructor
// Algorithm parameters which can be set at run time must be declared here.

CellularAutomaton::CellularAutomaton( const std::string& name,
                                      ISvcLocator* pSvcLocator )
  : Algorithm(name, pSvcLocator) {
  declareProperty( "InputPathHits", m_pathHits );
  declareProperty( "InputPathGeom", m_pathGeom );
  declareProperty( "OutputPathClusters", m_pathClusters );
  declareProperty( "Calorimeter", m_calorimeter );
  declareProperty( "Steps", m_steps ); 
  m_pathHits = "";
  m_pathGeom = "";
  m_pathClusters = "";
  m_calorimeter = "";
  m_steps = "seeds; taggedCells; preClusters; clusters";
}

// Initialize
// Call by the application manager before reading any event data

StatusCode CellularAutomaton::initialize() {
  MsgStream log(messageService(),name());
  log << MSG::INFO << "Initialization with :" << endreq;
  log << MSG::INFO << "  path of hits = " << m_pathHits << endreq;
  log << MSG::INFO << "  path of geom = " << m_pathGeom << endreq;
  log << MSG::INFO << "  path of clusters = " << m_pathClusters << endreq;
  log << MSG::INFO << "  for all regions of calorimeter " << m_calorimeter << endreq;
  StatusCode sc = StatusCode::SUCCESS;
  if ( m_pathHits.empty() || 
       m_pathGeom.empty() ||
       m_pathClusters.empty() ||
       m_calorimeter.empty() ) {
    log << MSG::ERROR << "Path hits, path geom, path clusters and calorimeter must be defined in job options file" << endreq; 
    sc = StatusCode::FAILURE;
  }
  if ( m_pathHits.find( m_calorimeter ) == string::npos ||
       m_pathGeom.find( m_calorimeter ) == string::npos ||
       m_pathClusters.find( m_calorimeter ) == string::npos ) {
    log << MSG::ERROR << "Path hits, path geom, path clusters must contain the name of calorimeter" << endreq;
    sc = StatusCode::FAILURE;
  }
  log << MSG::INFO << "  for steps " << m_steps << endreq;
  if ( ( "seeds; taggedCells; preClusters; clusters" != m_steps ) &&
       ( "taggedCells; preClusters; clusters" != m_steps ) &&
       ( "taggedCells; clusters" != m_steps ) ) {
    log << MSG::ERROR << "Bad choice of clustering steps in job options file" << endreq;
    sc = StatusCode::FAILURE;
  }
  if ( sc.isSuccess() ) {
    log << MSG::INFO << "Initalization completed successfully" << endreq;
  }
  return sc;
}

// Execute
// Call by the application manager once per event

StatusCode CellularAutomaton::execute() {
  MsgStream log(messageService(),name());
  MSG::Level messLevel = log.level();
  log << MSG::DEBUG << "Execution" << endreq;
 
  /// Retrieve hits

  log << MSG::DEBUG << "Retrieve hits" << endreq;
  HitsHolder hitsBox ( eventSvc(),
                       m_pathHits,
                       m_calorimeter,
                       messageService() );
  if ( messLevel <= MSG::DEBUG ) {
    hitsBox.printCaloDigitHits();
  }

  /// Retrieve geometry of detector
   
  log << MSG::DEBUG << "Retrieve geometry" << endreq;  
  DetecHolder geomBox ( detSvc(),
                        m_pathGeom,
                        m_calorimeter );
  if ( messLevel <= MSG::DEBUG ) {
    hitsBox.printCaloCellIDNeighborsHits( geomBox );
  }
  if ( messLevel <= MSG::DEBUG ) {
    hitsBox.printCaloDigitNeighborsHits( geomBox );
  }

  /// Clusterize

  log << MSG::DEBUG << "Steps to obtain clusters = " << m_steps << endreq;
  
  if ( m_steps == "seeds; taggedCells; preClusters; clusters" ) {
    log << MSG::DEBUG << "Step seeds" << endreq;
    SeedsHolder seedsBox ( hitsBox,
                           geomBox );
    if ( MSG::DEBUG >= messLevel) {
      seedsBox.printCaloCellIDSeeds();
      seedsBox.printCaloDigitSeeds();
    } 
    log << MSG::DEBUG << "Step tagged cells" << endreq;
    bool infoStepByStep = false;
    if ( MSG::DEBUG == messLevel ) { 
      infoStepByStep = true;
    }
    TaggedCellsHolder taggedCellsBox( seedsBox,
                                      infoStepByStep );
    if ( MSG::DEBUG >= messLevel ) {
      taggedCellsBox.printCelAutoTaggedCellCells();
    }
    if ( 0 != taggedCellsBox.numberNotTaggedCells() ) {
      log << MSG::INFO << "TAGGING NOT FULL - Remain " << taggedCellsBox.numberNotTaggedCells() << " not edge not clustered " << endreq; 
    }
    log << MSG::DEBUG << "Step preClusters" << endreq;
    PreClustersHolder preClusters( taggedCellsBox );
    if ( MSG::DEBUG >= messLevel ) {
      preClusters.printCelAutoPreClusters();
    }
    log << MSG::DEBUG << "Step clusters" << endreq;
    ClustersHolder clusters( preClusters,
                             eventSvc(),
                             m_pathClusters );
    if ( MSG::DEBUG >= messLevel ) {
      clusters.printCelAutoClusters();
    }
  }
  else {
    if ( m_steps == "taggedCells; preClusters; clusters" ) {
      log << MSG::DEBUG << "Step tagged cells" << endreq;
      TaggedCellsHolder taggedCellsBox( hitsBox,
                                        geomBox );
      if ( MSG::DEBUG >= messLevel ) {
        taggedCellsBox.printCelAutoTaggedCellCells();
      }
      if ( 0 != taggedCellsBox.numberNotTaggedCells() ) {
        log << MSG::INFO << "TAGGING NOT FULL - Remain " << taggedCellsBox.numberNotTaggedCells() << " not edge not clustered " << endreq; 
      }
      log << MSG::DEBUG << "Step preClusters" << endreq;
      PreClustersHolder preClusters( taggedCellsBox );
      if ( MSG::DEBUG >= messLevel ) {
        preClusters.printCelAutoPreClusters();
      }
      log << MSG::DEBUG << "Step clusters" << endreq;
      ClustersHolder clusters( preClusters,
                               eventSvc(),
                               m_pathClusters );
      if ( MSG::DEBUG >= messLevel ) {
        clusters.printCelAutoClusters();
      }
    }
    else {
      if ( m_steps == "taggedCells; clusters" ) {
        log << MSG::DEBUG << "Step tagged cells" << endreq;
        TaggedCellsHolder taggedCellsBox( hitsBox,
                                          geomBox );
        if ( MSG::DEBUG >= messLevel ) {
          taggedCellsBox.printCelAutoTaggedCellCells();
        }
        if ( 0 != taggedCellsBox.numberNotTaggedCells() ) {
          log << MSG::INFO << "TAGGING NOT FULL - Remain " << taggedCellsBox.numberNotTaggedCells() << " not edge not clustered " << endreq; 
        }
        log << MSG::DEBUG << "Step clusters" << endreq;
        ClustersHolder clusters( taggedCellsBox,
                                 eventSvc(),
                                 m_pathClusters );
        if ( MSG::DEBUG >= messLevel ) {
          clusters.printCelAutoClusters();
        }
      }
    }
  }
  log << MSG::INFO << "Execution clusterization manager completed successfully" << endreq;
  return StatusCode::SUCCESS; 
}

// Finalize
// Call by the application manager at the end of the application

StatusCode CellularAutomaton::finalize() {
  MsgStream log(messageService(),name());
  log << MSG::INFO << "finalize" << endreq;
  return StatusCode::SUCCESS;
}




















































































