//  ===========================================================================
#define GaussTools_PrintEventAlg_CPP
// ============================================================================
/// STL
#include <string>
#include <vector>
#include <list>
/// GaudiKernel 
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/Stat.h"
#include "GaudiKernel/MsgStream.h"
// Using Particle properties
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
// GiGa 
#include "GiGa/IGiGaSvc.h"
/// from LHCbEvent 
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
#include "Event/MCHit.h"
#include "Event/GenHeader.h"
#include "Event/Collision.h"
/// local 
#include "PrintEventAlg.h"

// ============================================================================
/** @file PrintEventAlg.cpp
 *  
 *  implementation of class GiGaMiscGetEvent 
 *
 *  @author Witold Pokorski 
 */
// ============================================================================

// ============================================================================
/** mandatory factory stuff
 */
// ============================================================================
static const  AlgFactory<PrintEventAlg>         s_Factory;
const        IAlgFactory&PrintEventAlgFactory = s_Factory;

// ============================================================================
/** standard constructor
 *  @param name name of teh algorithm 
 *  @param SvcLoc pointer to service locator 
 */
// ============================================================================
PrintEventAlg::PrintEventAlg(const std::string& name, 
                                         ISvcLocator* pSvcLocator) 
  ///
  : Algorithm( name , pSvcLocator) 
  , m_particles   ( MCParticleLocation::Default )
  , m_vertices    ( MCVertexLocation::Default   )
  , m_hits        ( MCHitLocation::OTHits   )
    , m_licznik(0)
    , m_liczevent(0)
{ 
  declareProperty( "Particles" , m_particles); 
  declareProperty( "Vertices"  , m_vertices); 
  declareProperty( "Hits"      , m_hits); 
  declareProperty( "DecayDepth" , m_depth = 99);
};

// ============================================================================
/** destructor 
 */
// ============================================================================
PrintEventAlg::~PrintEventAlg(){};

// ============================================================================
/** the standard Algorithm initialization method 
 *  @see Algorithm
 *  @return status code
 */
// ============================================================================
StatusCode PrintEventAlg::initialize() 
{
  MsgStream log( msgSvc() , name() ) ;

  // Accessing particle properties
  StatusCode sc = service("ParticlePropertySvc", m_ppSvc );
  if( !sc.isSuccess() ) {
    log << MSG::FATAL << "Unable to locate Particle Property Service" << endreq;
    return sc;
  }
 return StatusCode::SUCCESS; 
};

// ============================================================================
/** the standard Algorithm execution method 
 *  @see Algorithm
 *  @return status code
 */
// ============================================================================
StatusCode PrintEventAlg::execute() 
{
  m_liczevent++;
  
  ///
  typedef MCParticles Particles ;
  typedef MCVertices  Vertices  ;
  ///
  int licz=0;

  MsgStream log( msgSvc() , name() ) ;

  SmartDataPtr<GenHeader> hobj( eventSvc() , GenHeaderLocation::Default ) ;
  
  if( hobj ) 
    {
      log << MSG::INFO
              << "Event type "
          << hobj->evType() 
          << endreq ;
    }  
  
  SmartDataPtr<Collisions> colobj(eventSvc(), CollisionLocation::Default);  
  
  if(colobj)
    {
      log << MSG::INFO<<"Number of collisions " << colobj->size() 
          << endreq;
      
      for(Collisions::const_iterator citer=colobj->begin();colobj->end()!=citer;
          citer++)
        {
          log << MSG::INFO << "Process type " << (*citer)->processType()
              << " signal " << (*citer)->isSignal() <<
            " vertex " << (*citer)->primVtxPosition()<< endreq;
        }
    }
  
  
  if( !m_particles.empty() )
    {
      SmartDataPtr<Particles> obj( eventSvc() , m_particles ) ;
      if( obj ) 
        {
          log << MSG::INFO
              << "Number of extracted particles '"
              << m_particles << "' \t"
              << obj->size() 
              << endreq ;
          // Stat stat( chronoSvc() , "#particles" , obj->size() ) ; 
          
          MCParticles::const_iterator ipart;
          for ( ipart = obj->begin(); ipart != obj->end(); ipart++ )  
            {
              if ( !((*ipart)->mother()))            
                {
                  licz++;
                  log << MSG::INFO << " "  << endreq;
                  printDecayTree( 0, " |", *ipart );
                }
            }
          log << MSG::INFO << "Number of 'primary' particles " 
              << licz << endreq;
          m_licznik = m_licznik+licz;
          
        } 
    }
  else 
    { 
      log << MSG::ERROR 
          << " Could not extract 'Particles' from '"
          << m_particles << "'" 
          << endreq ;
      ///
      return StatusCode::FAILURE;
    } 

  ///
  if( !m_vertices.empty() )
    {
      SmartDataPtr<Vertices> obj( eventSvc() , m_vertices ) ;
      if( obj ) 
        { 
          log << MSG::INFO
              << "Number of extracted vertices  '"
              << m_vertices << "'  \t" 
              << obj->size() 
              << endreq ;
          // Stat stat( chronoSvc() , "#vertices" , obj->size() ) ;
        }
      else 
        { 
          log << MSG::ERROR 
              << " Could not extract 'Vertices' from '"
              << m_vertices << "' \t" 
              << endreq ;
          ///
          return StatusCode::FAILURE;
        } 
    }

  ///
  return StatusCode::SUCCESS;
};

// ============================================================================
/** the standard Algorithm finalization method 
 *  @see Algorithm
 *  @return status code
 */
// ============================================================================
StatusCode PrintEventAlg::finalize()
{ 
  MsgStream log( msgSvc() , name() ) ;

  log << MSG::INFO << "Average number of 'primary' particles "
      << m_licznik/m_liczevent << endreq;
  

  return StatusCode::SUCCESS; 
}

// printing the decay tree of a given mother particle
void PrintEventAlg::printDecayTree
(long depth, const std::string& prefix, const MCParticle* mother)   {
  MsgStream log(msgSvc(), name());
  const SmartRefVector<MCVertex>& decays = mother->endVertices();
  ParticleProperty* p = m_ppSvc->findByStdHepID( mother->particleID().pid() );
  
  std::string name;
  
  if(!p) 
    {
      log << MSG::INFO << " Particle not recognized " 
          << mother->particleID().pid() << endreq;
      name="XXXX";
    }
  else
    {
      name= p->particle();
    }
  double x,y,z;
  enum MCVertexType {Unknown=0, ppCollision, Decay, Hadronic, Brem, Pair, 
                     Compton, DeltaRay, MuonBackground, MuonBackgroundFlat, 
                     MuonBackgroundSpillover};
  std::string vertype;
  
  if(mother->originVertex())
    {
      x=mother->originVertex()->position().x();
      y=mother->originVertex()->position().y();
      z=mother->originVertex()->position().z();
      switch(mother->originVertex()->type())
        {
        case 0:
          vertype="Unknown";
          break;
        case 1:
          vertype="ppCollision";
          break;
        case 2:
          vertype="Decay";
          break;
        case 3:
          vertype="Hadronic";
          break;
        case 4:
          vertype="Brem";
          break;
        case 5:
          vertype="Pair";
          break;
        case 6:
          vertype="Compton";
          break;
        case 7:
          vertype="DeltaRay";
          break;
        case 8:
          vertype="MuonBackground";
          break;
        case 9:
          vertype="MuonBackgroundFlat";
          break;
        case 10:
          vertype="MuonBackgroundSpillover";
          break;
          
        default:
          vertype="Unset";
          break;
        }
    }
  else
    {
      x=-99999.9;
      y=-99999.9;
      z=-99999.9;
      vertype=MCVertex::Unknown;
    }

  if (mother->collision())
    {
  log << MSG::INFO << depth << prefix.substr(0, prefix.length()-1)
      << "+--->" << std::setw(12) << std::setiosflags(std::ios::left) 
      << name 
      << "    En(MeV):"   << std::setw(12) << mother->momentum().e()
      << " x:" << std::setw(12) << x
      << " y:" << std::setw(12) << y
      << " z:" << std::setw(12) << z 
      << " vertexType " << vertype << " process " 
      << mother->collision()->processType() 
      << " signal " << mother->collision()->isSignal()
      << " primary vertex " 
      << mother->collision()->primVtxPosition()
      << endreq;
    }
  else
    {
      log << MSG::INFO << depth << prefix.substr(0, prefix.length()-1)
      << "+--->" << std::setw(12) << std::setiosflags(std::ios::left) 
      << name 
      << "    En(MeV):"   << std::setw(12) << mother->momentum().e()
      << " x:" << std::setw(12) << x
      << " y:" << std::setw(12) << y
      << " z:" << std::setw(12) << z << " no collision!"
          << endreq;
    }
  
  if ( depth < m_depth )  
    {
      SmartRefVector<MCVertex>::const_iterator ivtx;
      for ( ivtx = decays.begin(); ivtx != decays.end(); ivtx++ )  {
      const SmartRefVector<MCParticle>& daughters = (*ivtx)->products();
      SmartRefVector<MCParticle>::const_iterator idau;
      for ( idau   = daughters.begin(); idau != daughters.end(); idau++ )  {
        printDecayTree( depth+1, prefix+" |", *idau );
      }
      }
    }
}

// ============================================================================
// The End 
// ============================================================================








