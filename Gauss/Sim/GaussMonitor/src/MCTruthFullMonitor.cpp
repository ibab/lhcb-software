// $Id: MCTruthFullMonitor.cpp,v 1.2 2004-04-29 17:19:13 gcorti Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/SmartDataPtr.h"

// from Det/DetDesc
#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"

// from Event/Event
#include "Event/GenCollision.h"
#include "Event/MCParticle.h"
#include "Event/GenMCLink.h"

// local
#include "MCTruthFullMonitor.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MCTruthFullMonitor
//
// 2004-01-27 : Gloria CORTI
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<MCTruthFullMonitor>          s_factory ;
const        IAlgFactory& MCTruthFullMonitorFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCTruthFullMonitor::MCTruthFullMonitor( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator )
{
//   declareProperty("ZRangeMin", m_zVolMin = 11830.0*mm);
//   declareProperty("ZRangeMAx", m_zVolMax = 11880.0*mm);
}
//=============================================================================
// Destructor
//=============================================================================
MCTruthFullMonitor::~MCTruthFullMonitor() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode MCTruthFullMonitor::initialize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Booking ntuple" << endreq;

  std::string ntname = "/NTUPLES/FILE1";
  NTupleFilePtr ntfile(ntupleSvc(), ntname) ;
  if( ntfile ) {
  StatusCode status = StatusCode::SUCCESS;
    ntname += "/"+(this->name())+"/100";
    NTuplePtr nt1(ntupleSvc(), ntname);
    if( !nt1 ) {
      nt1 = ntupleSvc()->book(ntname,CLID_ColumnWiseTuple,"Truth");
      if( nt1 ) {
        if( status.isSuccess() ) nt1->addItem("PType",       m_pType     );
        if( status.isSuccess() ) nt1->addItem("PXOrigin",    m_pxOvtx    );
        if( status.isSuccess() ) nt1->addItem("PYOrigin",    m_pyOvtx    );
        if( status.isSuccess() ) nt1->addItem("PZOrigin",    m_pzOvtx    );
        if( status.isSuccess() ) nt1->addItem("VTypeOrigin", m_typeOvtx  );
        if( status.isSuccess() ) nt1->addItem("XOrigin",     m_xOvtx     );
        if( status.isSuccess() ) nt1->addItem("YOrigin",     m_yOvtx     );
        if( status.isSuccess() ) nt1->addItem("ZOrigin",     m_zOvtx     );
        if( status.isSuccess() ) nt1->addItem("ParentType",  m_parent    );
        if( status.isSuccess() ) nt1->addItem("NDaughVtx",   m_daughVtx  );
        if( status.isSuccess() ) nt1->addItem("NDaughPart",  m_daughPart );
        if( !status.isSuccess() ) {
          msg << MSG::ERROR << "Failure booking ntuples" << endreq;
          return StatusCode::FAILURE;
        } 
        m_ntuple = nt1;
      } else {  
        msg << MSG::ERROR << "Ntuple already exist" << endreq;
        return StatusCode::FAILURE;
      }
    }
  }
  
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode MCTruthFullMonitor::execute() {

  MsgStream  msg( msgSvc(), name() );

  SmartDataPtr<Collisions> colls(evtSvc(), CollisionLocation::Default);
  if( !colls ) {
    msg << MSG::ERROR << "Collisions not found at" 
        << CollisionLocation::Default
        << endreq;
  }
  // new way of storing info
  for( Collisions::iterator aColl = colls->begin(); colls->end() != aColl;
       ++aColl ) {
    msg << MSG::DEBUG << "Collision = " 
        << (*aColl)->processType()
        << endreq;
    GenCollision* genColl = dynamic_cast<GenCollision*> (*aColl);
    if( !genColl ) {
      msg << MSG::DEBUG << "Collision is not of type GenCollision"
          << endreq;
    }
    else {
      msg << MSG::DEBUG << "GenCollision = "
          << genColl->hardInfo()->event()->pGenEvt()->signal_process_id() << " " 
          << genColl->hardInfo()->sHat() << " " 
          << genColl->hardInfo()->tHat() << " " 
          << genColl->hardInfo()->uHat() << " " 
          << genColl->hardInfo()->ptHat() << " " 
          << genColl->hardInfo()->x1Bjorken() << " " 
          << genColl->hardInfo()->x2Bjorken()
          << endreq;
    }
  }

  SmartDataPtr<MCParticles> parts(eventSvc(),MCParticleLocation::Default);
  if( !parts ) {
    msg << MSG::ERROR << "MCParticles not found at" 
        << MCParticleLocation::Default
        << endreq;
  }
//   int icount = 0;
  for( MCParticles::iterator aPart = parts->begin();
       parts->end() != aPart; ++aPart ) {
//     icount++;
//     if( icount <= 20 ) {
//       msg << MSG::INFO << "Collision from part = "
//           << (*aPart)->collision()->processType()
//           << endreq;
//       Collision* thisColl = (*aPart)->collision();
//       GenCollision* genColl = dynamic_cast<GenCollision*> (thisColl);
//       if( !genColl ) {
//         msg << MSG::ERROR << "Collision is not of type GenCollision"
//             << endreq;
//       }
//       else {
//         msg << MSG::INFO << "GenCollision = "
//             << genColl->hardInfo()->event()->pGenEvt()->signal_process_id() << " " 
//             << genColl->hardInfo()->sHat() << " " 
//             << genColl->hardInfo()->tHat() << " " 
//             << genColl->hardInfo()->uHat() << " " 
//             << genColl->hardInfo()->ptHat() << " " 
//             << genColl->hardInfo()->x1Bjorken() << " " 
//             << genColl->hardInfo()->x2Bjorken()
//             << endreq;
//       }
//     }
        
    m_pType  = (*aPart)->particleID().pid();
    m_pxOvtx = (*aPart)->momentum().x();
    m_pyOvtx = (*aPart)->momentum().y();
    m_pzOvtx = (*aPart)->momentum().z();
    m_typeOvtx = -1;
    m_xOvtx    = -9999.0;
    m_yOvtx    = -9999.0;
    m_zOvtx    = -9999.0;
    if( (*aPart)->originVertex() ) {
      m_typeOvtx = (*aPart)->originVertex()->type();
      m_xOvtx    = (*aPart)->originVertex()->position().x();
      m_yOvtx    = (*aPart)->originVertex()->position().y();
      m_zOvtx    = (*aPart)->originVertex()->position().z();
    }
    m_parent = -1000000;
    if( (*aPart)->mother() ) { 
      m_parent = ((*aPart)->mother())->particleID().pid();
    }
    m_daughVtx = ((*aPart)->endVertices()).size();
    int nTotDau = 0;
    for( SmartRefVector<MCVertex>::const_iterator 
           aVert = ((*aPart)->endVertices()).begin();
         ((*aPart)->endVertices()).end() != aVert; ++aVert ) {
      nTotDau += ((*aVert)->products()).size();
    }
    m_daughPart = nTotDau;
    m_ntuple->write();
  }

    
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MCTruthFullMonitor::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Finalize" << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
