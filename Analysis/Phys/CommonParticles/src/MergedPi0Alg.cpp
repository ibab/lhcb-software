// ============================================================================
// Include files
// STD & STL 
#include <algorithm>
#include <functional>
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "AIDA/IHistogram1D.h"
// from Event 
#include "Event/EventHeader.h"
#include "Event/PrimVertex.h"
#include "Event/Vertex.h"
#include "Event/ProtoParticle.h" 
#include "Event/Particle.h"
// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Geometry/Point3D.h"
//DaVinci
#include "DaVinciTools/IPhysDesktop.h"
#include "DaVinciTools/IParticleStuffer.h"

#include "Event/CaloHypo.h"
#include "CaloDet/DeCalorimeter.h" 
#include "CaloUtils/ClusterFunctors.h"

#include "MergedPi0Alg.h"

static const  AlgFactory<MergedPi0Alg>         s_Factory ;
const        IAlgFactory&MergedPi0AlgFactory = s_Factory ; 
// ============================================================================

// ============================================================================
/** Standard constructor
 *  @param name algorithm name 
 *  @param pSvc service locator 
 */
// ============================================================================
MergedPi0Alg::MergedPi0Alg
( const std::string& name ,
  ISvcLocator*       pSvc )
  : DVAlgorithm ( name , pSvc ),
    m_nEvents(0)
  , m_pi0Count(0)
  , m_ParMas()
{
  declareProperty( "HistogramFlag", m_produceHistogram = false );
  declareProperty( "Pi0MassWindow", m_pi0MassWin = 100. * MeV);
  declareProperty( "GammaPtCut", m_gPtCut = 0. * MeV);
  declareProperty( "Pi0PtCut", m_pi0PtCut = 0. * MeV);
  declareProperty( "GammaGammaDistCut", m_ggDistCut = 2.5 );
  declareProperty( "ParMas", m_ParMas );
};
// ============================================================================

// ============================================================================
/// destructor (protected and virtual)
// ============================================================================
MergedPi0Alg::~MergedPi0Alg() {}; 
// ============================================================================

// ============================================================================
/**  standard Algorithm initialization
 *   @return status code 
 */
// ============================================================================
StatusCode
MergedPi0Alg::initialize() 
{
  MsgStream  logbk( msgSvc(), name() );
  logbk << MSG::INFO <<  " == > Initialize " << endreq;
  
  StatusCode sc=StatusCode::SUCCESS;
  
  //  sc=loadTools(); 
  
  // if( sc.isFailure() ) {
  //   logbk << MSG::ERROR << "   Unable to load tools" << endreq;
  //   return StatusCode::FAILURE;
  // }


  // check vectors of paramters 
  if( 3 != m_ParMas.size() ) 
    { 
      logbk << MSG::ERROR <<  "Invalid number of parameters" << endreq ; 
      return StatusCode::FAILURE; 
    }
  


  
  // Access the ParticlePropertySvc to retrieve pID for wanted particles
  logbk << MSG::INFO << "    Looking for Particle Property Service." << endreq;
  
  IParticlePropertySvc* ppSvc = 0; 
  sc = service("ParticlePropertySvc", ppSvc);
  if( sc.isFailure() ) {
    logbk << MSG::FATAL << "    Unable to locate Particle Property Service" 
	  << endreq;
    return sc;
  }

  ParticleProperty* partProp;
  
  partProp = ppSvc->find( "pi0" );
  m_pi0ID = (*partProp).jetsetID();
  m_pi0Mass = (*partProp).mass();

  partProp = ppSvc->find( "gamma" );
  m_gammaID = (*partProp).jetsetID();

  // If histograms are required initialize them
  if( m_produceHistogram )   {
    logbk << MSG::INFO << "    histograms will be produced" << endreq;

    // Create the histograms 
    m_hMassPi0 = histoSvc()->
      book("/stat/simple/2", "Reconstructed Pi0 Mass",
           100,m_pi0Mass-m_pi0MassWin,m_pi0Mass+m_pi0MassWin);
    if( 0 == m_hMassPi0 ) {
      logbk << MSG::ERROR << "    Cannot register histogram 1" << endreq;
      return StatusCode::FAILURE;
    }
  }    
  logbk<<MSG::WARNING<<"setFilterPassed is systematically TRUE"<<endreq;

  return StatusCode::SUCCESS;  
};
// ============================================================================

// ============================================================================
/**  standard Algorithm finalization
 *   @return status code 
 */
// ============================================================================
StatusCode 
MergedPi0Alg::finalize() 
{
  MsgStream  logbk( msgSvc(), name() );
  logbk << MSG::INFO <<  " == > Finalize " << endreq;
  
  // Print out counters
  logbk << MSG::INFO << "    Number of events processed      = " 
	<< m_nEvents << endreq;

  logbk << MSG::INFO << "    Number of selected Pi0          = " 
	<< m_pi0Count << endreq;
  // End of finalization step

  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
/**  standard Algorithm execution
 *   @return status code 
 */
// ============================================================================
StatusCode 
MergedPi0Alg::execute() 
{
  MsgStream  logbk( msgSvc(), name() );

  setFilterPassed(true);	
  // Counter of events processed
  logbk << MSG::DEBUG << "    processing event number " << ++m_nEvents <<endreq;
  
  // Retrieve informations about event
  SmartDataPtr<EventHeader> evt(eventSvc(), EventHeaderLocation::Default );
  
  if ( evt ) {   
    logbk << MSG::DEBUG << "    retrieved EVENT: " << evt->evtNum()
	  << " RUN: " << evt->runNum() << endreq;
  }
  else {
    logbk << MSG::ERROR << "    not able to retrieve event" << endreq;
    return StatusCode::FAILURE;
  }
  
#ifdef DVvnine
  StatusCode scDesktop = desktop()->getInput();
  if (!scDesktop) {
    logbk << MSG::WARNING<< "    not able to fill PhysDesktop " << endreq;
    return StatusCode::SUCCESS;
  }
#endif
  
  // Retrieve the particles  from PhysDesktop 
  const VertexVector& verts = desktop()->vertices();
  const ParticleVector& parts = desktop()->particles();
  
  logbk<<MSG::DEBUG<<"Number of Vertices  :"<<verts.size()<<endreq;
  logbk<<MSG::DEBUG<<"Number of Particles :"<<parts.size()<<endreq;

  ParticleVector selectedParts;
  StatusCode scFilter = particleFilter()->filter( parts, selectedParts );

  if ( !scFilter ) {
    logbk<< MSG::ERROR 
         << "Could not perform correct particle filtering" <<endreq;
  }

  logbk<<MSG::DEBUG<<"Number of Filtered Particles :"
       <<selectedParts.size()<<endreq;

  int pi0countperevt=0;

  ParticleVector::const_iterator ip,ip1,ip2;

  ParticleVector ptSelectedParts;

  for( ip = selectedParts.begin() ; ip != selectedParts.end() ; ++ip ) {
    unsigned int ind=ip-selectedParts.begin();
    double pt=(*ip)->momentum().et();
    if (pt>m_gPtCut) {
      ptSelectedParts.push_back(*ip);
      logbk << MSG::DEBUG << " Photon Candidate "<<
        ind<<" ["<<(*ip)->confLevel()<<"] : pt= "<< 
        pt << "MeV" << endreq;
    }
  }

  logbk<<MSG::DEBUG<<"Number of Particles passing Pt Cut :"
       <<ptSelectedParts.size()<<endreq;

  for( ip1 = ptSelectedParts.begin() ; ip1 != ptSelectedParts.end() ; ++ip1 ) {
    if( (*ip1)->particleID() == m_gammaID ) {
      for( ip2 = ip1+1 ; ip2 != ptSelectedParts.end() ; ++ip2 ) {
        if( (*ip2)->particleID() == m_gammaID ) {
	  // 
          unsigned int ind1=ip1-ptSelectedParts.begin();
          unsigned int ind2=ip2-ptSelectedParts.begin();

          ContainedObject* ori1 = (*ip1) -> origin();
          ContainedObject* ori2 = (*ip2) -> origin();
          if(ori1 != ori2){continue;}
          

          HepLorentzVector ggComb(0.0, 0.0, 0.0, 0.0);

          ggComb = (*ip1)->momentum() + (*ip2)->momentum() ;
          
        // mass miscalibrated hard coded correction (temporarly)
          //const double par1=1.0028;
          //const double par2=0.76286E-03;
          //const double par3=0.14989E-04;
          const double par1=m_ParMas[0];
           const double par2=m_ParMas[1];
          const double par3=m_ParMas[2];
          double umas=ggComb.m();
          double ene =ggComb.e()/GeV;
          double fact=par1+par2*ene+par3*ene*ene;
          double CorrectedMas=umas/fact;
          double mass=CorrectedMas;
          double pt = ggComb.et();
          // Comput the minimal gamma-gamma distance on Ecal.
          SmartDataPtr<DeCalorimeter> 
            detector( detSvc() , "/dd/Structure/LHCb/Ecal");
          // retrieve ProtoParticle
          const ProtoParticle* pp= dynamic_cast<const ProtoParticle*>(ori1);
          // retrieve first hypo
          typedef const SmartRefVector<CaloHypo>  Hypos ;
          const Hypos& hypos = pp->calo () ;
          Hypos::const_iterator ihypo =hypos.begin();
          const CaloHypo* hypo = *ihypo;
          // retrieve first cluster
          typedef const CaloHypo::Clusters Clusters ;
          const Clusters& clusters = hypo->clusters() ;
          Clusters::const_iterator icl = clusters.begin () ;
          const CaloCluster* cluster = *icl ;
          // retrieve seed
          const CaloCluster::Entries& entries = cluster->entries();
          CaloCluster::Entries::const_iterator iseed = 
            ClusterFunctors::locateDigit ( entries.begin () , 
                                          entries.end   () , 
                                           CaloDigitStatus::SeedCell );
          const CaloDigit*  seed    = iseed->digit();
          CaloCellID     cellID = seed->cellID() ;
          double CellSize =  detector->cellSize( cellID  );
          double epi0 = ggComb.e() ;
          double zpos = detector->cellZ   ( cellID ) ;
          double dmin = zpos * 2. * m_pi0Mass / epi0 / CellSize;

          logbk << MSG::DEBUG << " Dmin "<< dmin << " " << m_ggDistCut<< endreq;
          logbk << MSG::DEBUG << " PT "<< pt << " "  << m_pi0PtCut << endreq;

          if( m_pi0PtCut > pt ){continue;}

          if( m_ggDistCut < dmin  ){continue;}


          logbk << MSG::DEBUG << " E1 "<< (*ip1)->momentum().e() << endreq;
          logbk << MSG::DEBUG << " E2 "<< (*ip2)->momentum().e() << endreq;
          

          logbk << MSG::DEBUG << " Combinaison "<<
            ind1<<" ["<<(*ip1)->confLevel()<<"] + "<<
            ind2<<" ["<<(*ip2)->confLevel()<<"]"<<
            "  -> Pi0 hypo Mass = " << mass << " MeV" << endreq;
          if (fabs(mass-m_pi0Mass)<m_pi0MassWin) {
            logbk << MSG::DEBUG << " * Kept Pi0 candidate - (m=" << 
              m_pi0Mass << "+/-" <<
              m_pi0MassWin<<"MeV)"<<
              endreq;
            Vertex vtx;
            HepPoint3D coord(0.,0.,0.);
            vtx.setPosition( coord );
            vtx.addToProducts(*ip1);
            vtx.addToProducts(*ip2);
            vtx.setType(Vertex::Decay);

            
            
            Particle candPi0;
            ParticleID pi0PID( m_pi0ID );
            StatusCode scStuff = 
              particleStuffer()->fillParticle( vtx, candPi0, pi0PID );
            if ( !scStuff )  {
              logbk << MSG::ERROR << ">>> Particle stuffing failed" 
                    << endreq;
              return StatusCode::FAILURE;
            }
            

      Particle* pInDesktop  = desktop()->createParticle(&candPi0);

      //Fill corrected mass !!
	    if( pInDesktop ) {pInDesktop->setMass(CorrectedMas); } 

	    pi0countperevt++;
      

	    if( pInDesktop ) {
	      if( m_produceHistogram ) {
          m_hMassPi0->fill( pInDesktop->mass()/MeV, 1. );
	      }
	      logbk << MSG::DEBUG << "Pi0 added to PhysDesktop "
		    << "Pid and momentum = " 
		    << pInDesktop->particleID().pid() << ", "
		    << pInDesktop->mass()             << ", "
		    << pInDesktop->momentum().px() << ", "
		    << pInDesktop->momentum().py() << ", "
		    << pInDesktop->momentum().pz() << endreq;
	      logbk << MSG::DEBUG << "Its decay vertex position = " 
		    <<(pInDesktop->endVertex())->position().x()<< ", "
		    <<(pInDesktop->endVertex())->position().y()<< ", "
		    <<(pInDesktop->endVertex())->position().z()
		    << endreq;
		    }
		    else {
	      logbk <<MSG::DEBUG<< "not able to save Pi0 in desktop"
		    << endreq;  
	    }
	  }
	}
      }
    }
  }
	  
  // Log number of Combinations found by previous step
  logbk << MSG::INFO << "  Number of Pi0 found  = " 
	<< pi0countperevt<<endreq;
  
  logbk << MSG::INFO << "  Number of particles in desktop = " << parts.size() 
	<< endreq;

  StatusCode scDesktop = desktop()->saveDesktop(); // save all new particles
  if (scDesktop) {
    logbk << MSG::INFO << " PhysDeskTop Saved to TES"<<endreq;
  }
  else {
    logbk << MSG::ERROR << "not able to save desktop in TES"
	  << endreq;
    return StatusCode::FAILURE;
  }

  m_pi0Count+=pi0countperevt;

  return StatusCode::SUCCESS;
};
// ============================================================================


// ============================================================================
// The End 
// ============================================================================


