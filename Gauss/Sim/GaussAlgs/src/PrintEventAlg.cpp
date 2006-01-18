// $Id: PrintEventAlg.cpp,v 1.3 2006-01-18 09:12:46 gcorti Exp $
// Include files 

// from STL
#include <string>
#include <vector>
#include <list>

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
// #include "GaudiKernel/ISvcLocator.h"
// #include "GaudiKernel/AlgFactory.h"
// #include "GaudiKernel/IDataProviderSvc.h"
// #include "GaudiKernel/SmartDataPtr.h"
// #include "GaudiKernel/DataObject.h"
// #include "GaudiKernel/Stat.h"
// #include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

// from GiGa 
#include "GiGa/IGiGaSvc.h"

// from LHCbEvent 
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
#include "Event/MCHeader.h"
//#include "Event/GenCollision.h"

// local 
#include "PrintEventAlg.h"

// Boost 
#include "boost/lexical_cast.hpp"

//-----------------------------------------------------------------------------
// Implementation file for class : PrintEventAlg
//
//            : Witold POKORSKI
// 2006-01-16 : Gloria CORTI
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PrintEventAlg );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrintEventAlg::PrintEventAlg(const std::string& name, 
                                         ISvcLocator* pSvcLocator) 
  ///
  : GaudiAlgorithm( name , pSvcLocator) 
  , m_particles   ( LHCb::MCParticleLocation::Default )
  , m_vertices    ( LHCb::MCVertexLocation::Default   )
  , m_licznik(0)
  , m_liczevent(0)
{ 
  declareProperty( "Particles"  , m_particles  ) ; 
  declareProperty( "Vertices"   , m_vertices   ) ; 
  declareProperty( "DecayDepth" , m_depth = 99 ) ;
};

//=============================================================================
// Destructor
//=============================================================================
PrintEventAlg::~PrintEventAlg(){};

//=============================================================================
// Initialization
//=============================================================================
StatusCode PrintEventAlg::initialize() 
{
  StatusCode sc = GaudiAlgorithm::initialize () ;
  if( sc.isFailure() ) { return sc ; }
  
  m_ppSvc = svc<IParticlePropertySvc> ( "ParticlePropertySvc", true );
  return StatusCode::SUCCESS; 
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode PrintEventAlg::execute() 
{
  m_liczevent++;
  
  //  typedef MCParticles Particles ;
  //typedef MCVertices  Vertices  ;
  int licz=0;

  
  // Header information
  SmartDataPtr<LHCb::MCHeader> hobj( eventSvc(), 
                                     LHCb::MCHeaderLocation::Default );  
  if( hobj ) { 
    info() << "Event number " << hobj->evtNumber() 
           << "Event time " <<  hobj->evtTime() << endmsg;
    info() << "Number of primary vertices (i.e. collisions) "
           << (hobj->primaryVertices()).size() << endmsg;  
    for( SmartRefVector<LHCb::MCVertex>::const_iterator iv = 
           hobj->primaryVertices().begin();
         hobj->primaryVertices().end() != iv; ++iv ) {
      info() << " vertex " << (*iv)->position() << endmsg;
    }
  }  
    
  // MCParticles
  if( !m_particles.empty() ) {
    LHCb::MCParticles* obj = get<LHCb::MCParticles>( m_particles ) ;
    
    LHCb::MCParticles::const_iterator ipart;
    for( ipart = obj->begin(); ipart != obj->end(); ipart++ ) {
      if( !((*ipart)->mother())) {
        licz++;
        info() << " " << endmsg;
        printDecayTree( 0, " |", *ipart );
      }
    }
    
    info() << "Number of extracted particles '"
           << m_particles << "' \t"
           << obj->size() 
           << endmsg;
    info() << "Number of 'primary' particles " 
           << licz << endmsg;
    
    m_licznik = m_licznik+licz;
    
  }
  
  // MCVertices
  if( !m_vertices.empty() ) {
    LHCb::MCVertices* obj = get<LHCb::MCVertices>( m_vertices ) ;
    info() << "Number of extracted vertices  '"
           << m_vertices << "'  \t" 
           << obj->size() 
           << endreq ;
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PrintEventAlg::finalize()
{ 
  always () << "Average number of 'primary' particles "
            << m_licznik/m_liczevent << endmsg;
  
  return GaudiAlgorithm::finalize();
}

//=============================================================================
// Print the decay tree of a given mother particle
//=============================================================================
void PrintEventAlg::printDecayTree(long depth, const std::string& prefix,
                                   const LHCb::MCParticle* mother) {

  const SmartRefVector<LHCb::MCVertex>& decays = mother->endVertices();
  ParticleProperty* p = m_ppSvc->findByStdHepID( mother->particleID().pid() );
  
  std::string name;
  
  if(!p) {
    Warning( " Particle not recognized " +
             boost::lexical_cast<std::string>
             ( mother->particleID().pid() ) , StatusCode::FAILURE , 0 ) ;
    name="XXXX";
  }
  else {
    name= p->particle();
  }

  double x,y,z;
  std::string vertype;
  
  if(mother->originVertex()) {
    x=mother->originVertex()->position().x();
    y=mother->originVertex()->position().y();
    z=mother->originVertex()->position().z();
    vertype = mother->originVertex()->type();
  }
  else {
    x=-99999.9;
    y=-99999.9;
    z=-99999.9;
    vertype = LHCb::MCVertex::Unknown;
  }  

//   if( mother->collision() ) { // May put back this when tool is provided
  if( mother->primaryVertex() ) { 
    info() << depth << prefix.substr(0, prefix.length()-1)
         << "+--->" << std::setw(12) << std::setiosflags(std::ios::left) 
         << name 
         << "    En(MeV):"   << std::setw(12) << mother->momentum().e()
         << " x:" << std::setw(12) << x
         << " y:" << std::setw(12) << y
         << " z:" << std::setw(12) << z 
         << " vertexType " << vertype << " process " 
         << " primary vertex " 
         << mother->primaryVertex()->position()
//          << mother->collision()->processType() 
//          << " signal " << mother->collision()->isSignal()
         << endmsg;
  }
  else {
    info() << depth << prefix.substr(0, prefix.length()-1)
           << "+--->" << std::setw(12) << std::setiosflags(std::ios::left) 
           << name 
           << "    En(MeV):"   << std::setw(12) << mother->momentum().e()
           << " x:" << std::setw(12) << x
           << " y:" << std::setw(12) << y
           << " z:" << std::setw(12) << z << " no primary vertex!"
//            << " z:" << std::setw(12) << z << " no collision!"
           << endmsg;
  }
  
  if( depth < m_depth ) {
    SmartRefVector<LHCb::MCVertex>::const_iterator ivtx;
    for ( ivtx = decays.begin(); ivtx != decays.end(); ivtx++ )  {
      const SmartRefVector<LHCb::MCParticle>& daughters = (*ivtx)->products();
      SmartRefVector<LHCb::MCParticle>::const_iterator idau;
      for ( idau   = daughters.begin(); idau != daughters.end(); idau++ )  {
        printDecayTree( depth+1, prefix+" |", *idau );
      }
    }
  }
}

//=============================================================================








