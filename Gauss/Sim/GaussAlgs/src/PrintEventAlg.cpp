// $Id: PrintEventAlg.cpp,v 1.5 2007-01-12 15:23:42 ranjard Exp $
// Include files 

// from STL
#include <string>
#include <vector>
#include <list>

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

// from GiGa 
#include "GiGa/IGiGaSvc.h"

// from LHCbEvent 
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
#include "Event/MCHeader.h"

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
  : GaudiAlgorithm( name , pSvcLocator) 
  , m_licznik(0)
  , m_liczevent(0)
{ 
  declareProperty( "Particles"  , m_particles = LHCb::MCParticleLocation::Default); 
  declareProperty( "Vertices"   , m_vertices = LHCb::MCVertexLocation::Default ); 
  declareProperty( "DecayDepth" , m_depth = 99 ) ;
}

//=============================================================================
// Destructor
//=============================================================================
PrintEventAlg::~PrintEventAlg(){}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PrintEventAlg::initialize() 
{
  StatusCode sc = GaudiAlgorithm::initialize () ;
  if( sc.isFailure() ) { return sc ; }
  
  m_ppSvc = svc<IParticlePropertySvc> ( "Gaudi::ParticlePropertySvc", true );
  return StatusCode::SUCCESS; 
}

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
           << " Event time " <<  hobj->evtTime() << endmsg;
    info() << "Number of primary vertices from MCHeader "
           << (hobj->primaryVertices()).size() << endmsg;  
    for( SmartRefVector<LHCb::MCVertex>::const_iterator iv = 
           hobj->primaryVertices().begin();
         hobj->primaryVertices().end() != iv; ++iv ) {
      info() << " vertex " << (*iv)->position() << endmsg;
    }
  }
    
  // MCParticles
  if( !m_particles.empty() ) {
    const LHCb::MCParticles* obj = get<const LHCb::MCParticles>( m_particles );
    
    for( LHCb::MCParticles::const_iterator ipart = obj->begin();
         ipart != obj->end(); ++ipart ) {
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
    const LHCb::MCVertices* obj = get<const LHCb::MCVertices>( m_vertices ) ;
    info() << "Number of extracted vertices  '"
           << m_vertices << "'  \t" 
           << obj->size() 
           << endmsg;
    unsigned int nPV = 0;
    for( LHCb::MCVertices::const_iterator iv = obj->begin(); obj->end() != iv;
         ++iv ) {
      if( (*iv)->isPrimary() ) {
        nPV++;
        info() << "Primary " << nPV << " outgoing particles " 
               << (*iv)->products().size() << endmsg;
        info() << "Primary position " << (*iv)->position() << endmsg;
      }
    } 
    info() << "Number of extracted primary vertices " << nPV << endmsg;
  }

  return StatusCode::SUCCESS;
}

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
  
  if( mother->originVertex()) {
    x=mother->originVertex()->position().x();
    y=mother->originVertex()->position().y();
    z=mother->originVertex()->position().z();
  }
  else {
    x=-99999.9;
    y=-99999.9;
    z=-99999.9;
  }

  info() << depth << prefix.substr(0, prefix.length()-1)
         << "+--->" << std::setw(12) << std::setiosflags(std::ios::left) 
         << name 
         << "    En(MeV):"   << std::setw(12) << mother->momentum().e();
  
  if( mother->originVertex() ) {
    info() << " origin Vertex " << mother->originVertex()->position()
           << " vertex Type " << mother->originVertex()->type();
  }
  if( mother->primaryVertex() ) {
    info() << " - from primary vertex " << mother->primaryVertex()->position();
  } else {
    info() << " - no primary vertex!";
  }
  info() << endmsg;
  
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








