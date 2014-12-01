// $Id: MCTruthFullMonitor.cpp,v 1.4 2008-05-07 09:54:20 gcorti Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// from Event/Event
#include "Event/MCParticle.h"

// local
#include "MCTruthFullMonitor.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MCTruthFullMonitor
//
// 2004-01-27 : Gloria CORTI
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MCTruthFullMonitor )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCTruthFullMonitor::MCTruthFullMonitor( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
//   declareProperty("ZRangeMin", m_zVolMin = 11830.0*mm);
//   declareProperty("ZRangeMAx", m_zVolMax = 11880.0*mm);
}
//=============================================================================
// Destructor
//=============================================================================
MCTruthFullMonitor::~MCTruthFullMonitor() {} 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode MCTruthFullMonitor::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize: Booking Ntuple" << endmsg;

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
        if( status.isSuccess() ) nt1->addItem("TOrigin",     m_tOvtx     );
        if( status.isSuccess() ) nt1->addItem("ParentType",  m_parent    );
        if( status.isSuccess() ) nt1->addItem("NDaughVtx",   m_daughVtx  );
        if( status.isSuccess() ) nt1->addItem("NDaughPart",  m_daughPart );
        if( !status.isSuccess() ) {
          error() << "Failure booking ntuples" << endmsg;
          return StatusCode::FAILURE;
        } 
        m_ntuple = nt1;
      } else {  
        error() << "Ntuple already exist" << endmsg;
        return StatusCode::FAILURE;
      }
    }
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MCTruthFullMonitor::execute() {

  debug() << "==> Execute" << endmsg;

  LHCb::MCParticles* parts =
    get<LHCb::MCParticles>( LHCb::MCParticleLocation::Default );

  for( LHCb::MCParticles::iterator aPart = parts->begin();
       parts->end() != aPart; ++aPart ) {
    m_pType  = (*aPart)->particleID().pid();
    m_pxOvtx = (*aPart)->momentum().x();
    m_pyOvtx = (*aPart)->momentum().y();
    m_pzOvtx = (*aPart)->momentum().z();
    m_typeOvtx = -1;
    m_xOvtx    = -9999.0;
    m_yOvtx    = -9999.0;
    m_zOvtx    = -9999.0;
    m_tOvtx    = -9999.0;
    if( (*aPart)->originVertex() ) {
      m_typeOvtx = (*aPart)->originVertex()->type();
      m_xOvtx    = (*aPart)->originVertex()->position().x();
      m_yOvtx    = (*aPart)->originVertex()->position().y();
      m_zOvtx    = (*aPart)->originVertex()->position().z();
      m_tOvtx    = (*aPart)->originVertex()->time();
    }
    m_parent = -1000000;
    if( (*aPart)->mother() ) { 
      m_parent = ((*aPart)->mother())->particleID().pid();
    }
    m_daughVtx = ((*aPart)->endVertices()).size();
    int nTotDau = 0;
    for( SmartRefVector<LHCb::MCVertex>::const_iterator 
           aVert = ((*aPart)->endVertices()).begin();
         ((*aPart)->endVertices()).end() != aVert; ++aVert ) {
      nTotDau += ((*aVert)->products()).size();
    }
    m_daughPart = nTotDau;
    m_ntuple->write();
  }

    
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MCTruthFullMonitor::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
