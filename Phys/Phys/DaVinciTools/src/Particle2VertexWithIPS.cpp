// $Id: Particle2VertexWithIPS.cpp,v 1.2 2004-01-14 19:00:06 gcorti Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

// from DaVinciTools
#include "DaVinciTools/IGeomDispCalculator.h"

// from Event
#include "Event/Particle.h"
#include "Event/Vertex.h"

// local
#include "Particle2VertexWithIPS.h"


//-----------------------------------------------------------------------------
// Implementation file for class : Particle2VertexWithIPS
//
// May 2003 : Gerhard Raven
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<Particle2VertexWithIPS>          s_factory ;
const        IAlgFactory& Particle2VertexWithIPSFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Particle2VertexWithIPS::Particle2VertexWithIPS( const std::string& name,
                                                ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator )
{
  declareProperty( "InputData", m_inputData );
  declareProperty( "OutputTable", m_outputTable );
  declareProperty( "UseSignificance", m_useSignificance=true );
  declareProperty( "MaxToBeAssociated", m_max=-1 );
  m_inputData.push_back("/Event/Phys/Bs2Jpsi2ee_phi2KK/Particles");
  m_inputData.push_back("/Event/Phys/Bs2Jpsi2mumu_phi2KK/Particles");
  m_inputData.push_back("/Event/Phys/Bd2Jpsi2mumu_KS0/Particles");
  m_inputData.push_back("/Event/Phys/Bu2Jpsi2mumu_K/Particles");
  m_inputData.push_back("/Event/Phys/Jpsi2mumu/Particles");
  m_inputData.push_back("/Event/Phys/Jpsi2ee/Particles");
  m_inputData.push_back("/Event/Phys/phi2KK/Particles");

  declareProperty( "InputData", m_inputData );
  declareProperty( "OutputTable", m_outputTable = "Phys/Relations/Particle2VertexWithIPS"  );
}

//=============================================================================
// Destructor
//=============================================================================
Particle2VertexWithIPS::~Particle2VertexWithIPS() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode Particle2VertexWithIPS::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endmsg;

  StatusCode sc = toolSvc()->retrieveTool("GeomDispCalculator", m_ipTool, this);
  if(sc.isFailure()){
    log << MSG::FATAL << " Unable to retrieve GeomDispCalculator tool" << endreq;
  }
  return sc;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode Particle2VertexWithIPS::execute() {

  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endmsg;

  // create an association table and register it in the TES
  Particle2VertexAsct::Table* table = new Particle2VertexAsct::Table();
  if (table==0) {
    log << MSG::FATAL << "     *** Could not create table " 
        << outputTable()
        << endmsg;
  }
  
  SmartDataPtr<Vertices> primaries(eventSvc(),VertexLocation::Primary);
          log << MSG::DEBUG << " got vertices from " << VertexLocation::Primary << endmsg;
          log << MSG::DEBUG << " particles from " << m_inputData.size() << " places " << endmsg;
  for( std::vector<std::string>::iterator i= m_inputData.begin(); 
       i!=m_inputData.end(); ++i) {
    log << MSG::DEBUG << " getting particles from " << *i<< endmsg;
    // Get Particles
    SmartDataPtr<Particles> particles (eventSvc(), *i);
    if( !particles ) {
      log << MSG::ERROR << "    *** Could not retrieve Particles from " << *i << endmsg;
      continue;
    }
    for (ParticleVector::const_iterator ip=particles->begin();ip!=particles->end();++ip){
    for (VertexVector::const_iterator iv=primaries->begin();iv!=primaries->end();++iv) {
            // FIXME: all but the next few lines are generic for all particles->vertex
            //        associators. Maybe we should template this class in a 'policy pattern'
            //        which, as template argument, defines a class which does the actual
            //        1->1 association... Alternatively add a pure virtual which does
            //        again the actual 1->1 association
          if( (*iv)->type()!=Vertex::Primary) continue;
          double x,sx;
          StatusCode sc = m_ipTool->calcImpactPar(**ip,**iv, x, sx);
          if (!sc.isSuccess()) continue;
          double w = fabs(m_useSignificance?x/sx:x);
          log << MSG::DEBUG << " p = @" << *ip << " v = @ " << *iv << " w = " << w << endmsg;
          if (m_max<=0 || w<m_max ) table->relate(*ip,*iv,w);
    } }
  }
  // Register the table on the TES
  StatusCode sc = eventSvc()->registerObject( outputTable(), table);
  if( sc.isFailure() ) {
    log << MSG::FATAL << "     *** Could not register table " 
        << outputTable()
        << endmsg;
    delete table;
  } else {
    log << MSG::DEBUG << "     Registered table " << outputTable() << endmsg;
  }
  return sc;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Particle2VertexWithIPS::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endmsg;

  return StatusCode::SUCCESS;
}
