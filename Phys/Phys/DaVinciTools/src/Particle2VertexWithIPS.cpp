// $Id: Particle2VertexWithIPS.cpp,v 1.5 2006-01-20 07:54:08 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IDataProviderSvc.h"

// from DaVinciTools
#include "Kernel/IGeomDispCalculator.h"

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
  : GaudiAlgorithm ( name , pSvcLocator )
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
StatusCode Particle2VertexWithIPS::initialize() 
{
  StatusCode sc = GaudiAlgorithm::initialize();
  if (!sc) return sc;
  m_ipTool = tool<IGeomDispCalculator>("GeomDispCalculator", m_ipTool, this);
  return sc;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode Particle2VertexWithIPS::execute() {

  // create an association table and register it in the TES
  Particle2VertexAsct::Table* table = new Particle2VertexAsct::Table();
  if (table==0) {
    fatal() << "     *** Could not create table " 
        << outputTable()
        << endmsg;
  }
  
  SmartDataPtr<Vertices> primaries(eventSvc(),VertexLocation::Primary);
  debug() << " got vertices from " << VertexLocation::Primary << endmsg;
  debug() << " particles from " << m_inputData.size() << " places " << endmsg;
  for( std::vector<std::string>::iterator i= m_inputData.begin(); 
       i!=m_inputData.end(); ++i) {
    debug() << " getting particles from " << *i<< endmsg;
    // Get Particles
    SmartDataPtr<Particles> particles (eventSvc(), *i);
    if( !particles ) {
      err() << "    *** Could not retrieve Particles from " << *i << endmsg;
      continue;
    }
    for (ParticleVector::const_iterator ip=particles->begin();ip!=particles->end();++ip){
      for (VertexVector::const_iterator iv=primaries->begin();iv!=primaries->end();++iv) {
        // @todo all but the next few lines are generic for all particles->vertex
        //       associators. Maybe we should template this class in a 'policy pattern'
        //       which, as template argument, defines a class which does the actual
        //       1->1 association... Alternatively add a pure virtual which does
        //       again the actual 1->1 association
        if( (*iv)->type()!=Vertex::Primary) continue;
        double x,sx;
        StatusCode sc = m_ipTool->calcImpactPar(**ip,**iv, x, sx);
        if (!sc.isSuccess()) continue;
        double w = fabs(m_useSignificance?x/sx:x);
        debug() << " p = @" << *ip << " v = @ " << *iv << " w = " << w << endmsg;
        if (m_max<=0 || w<m_max ) table->relate(*ip,*iv,w);
      } }
  }
  // Register the table on the TES
  StatusCode sc = put( table, outputTable());
  if( sc.isFailure() ) {
    fatal() << "     *** Could not register table " << outputTable() << endmsg;
    delete table;
  } else {
    debug() << "     Registered table " << outputTable() << endmsg;
  }
  return sc;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Particle2VertexWithIPS::finalize() {

  return GaudiAlgorithm::finalize();

}
