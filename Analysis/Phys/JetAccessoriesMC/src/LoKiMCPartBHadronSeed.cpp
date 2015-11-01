// $Id: LoKiMCPartBHadronSeed.cpp,v 1.2 2009-12-14 12:34:33 potterat Exp $
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/ILoKiSvc.h"
// ============================================================================
// Local 
// ============================================================================
#include "LoKiMCPartBHadronSeed.h"
// ============================================================================
/** @file 
 *  Implementation file for class  LoKi::MCPartBHadronSeed
 *  @author Cedric POTTERAT   cedric.potterat@cern.ch
 *  @date   2011-01-31
 */
// ============================================================================
/*  standard initialization of the tool
 *  @return status code 
 */
// ============================================================================
StatusCode LoKi::MCPartBHadronSeed::initialize () 
{
  StatusCode sc = GaudiTool::initialize() ;
  if ( sc.isFailure() ) { return sc ; }
  // check the parameters 
  //  sc = check() ;
  if ( sc.isFailure() ) { return sc ; }
  //
  svc<LoKi::ILoKiSvc>( "LoKiSvc" , true ) ;


  m_combiner = tool<IParticleCombiner> ( m_combinerName , this ) ;
  if ( !m_combiner) {
    err() << "Unable to Retrieve Default  ParticleCombiner" << endmsg;
    return StatusCode::FAILURE;
  }
  


  std::cout<<" ------------------------------------------------------ "<<std::endl;
  std::cout<<" |                                                    | "<<std::endl;
  std::cout<<" |         SeedsFinder Tool for Phys Particles        | "<<std::endl;
  std::cout<<" |  Gives back the MCPart B-Hadrons as seeds (Part)   |"<<std::endl;
  std::cout<<" |                                                    | "<<std::endl;
  std::cout<<" ------------------------------------------------------ "<<std::endl;
  std::cout<<" |                                                    | "<<std::endl;
  std::cout<<" |                                                      "<<std::endl;
  std::cout<<" |   SeedsFinder parameters: "                           <<std::endl;
  std::cout<<" |   SeedID            "<<   m_seedID                    <<std::endl;	
  std::cout<<" |   PtSeedsMin        "<<   m_PtSeedsMin                <<std::endl;  
  std::cout<<" |   inLHCbAcc         "<<   m_inAcc                     <<std::endl;  
  std::cout<<" |                                                      "<<std::endl;
  std::cout<<" |                                                    | "<<std::endl;
  std::cout<<" ------------------------------------------------------ "<<std::endl;

  return StatusCode::SUCCESS ;
} 
// ===========================================================================
// find seedsthe jets via a Seed Finder
// ===========================================================================
StatusCode LoKi::MCPartBHadronSeed::makeJets 
( const IJetMaker::Input& input_ ,const LHCb::RecVertex& vtx_ ,  IJetMaker::Jets& jets_ ) const 
{ 
  debug()<<" could not be used with a vtx"<<endmsg;
  debug()<<" vtx: "<<vtx_<<endmsg;
  makeJets(input_, jets_);
  return StatusCode::SUCCESS ;
}
StatusCode LoKi::MCPartBHadronSeed::makeJets 
( const IJetMaker::Input& input_ , IJetMaker::Jets& jets_ ) const 
{

  //Select evt with only 1 PV !
  const LHCb::RecVertex::Container* verts = get<LHCb::RecVertex::Container>(LHCb::RecVertexLocation::Primary);
  if(verts->size()!=1) return StatusCode::SUCCESS;
   
  IJetMaker::Jets Seeds;



  debug()<<"input_ not needed "<<input_<<endmsg;

  //Creats seeds with the MCParticle B-hadrons that decays

 LHCb::MCParticles* mcparts = get< LHCb::MCParticles>( LHCb::MCParticleLocation::Default );
  if( !mcparts ){
    warning() << "Unable to find MC particles at '"
	      <<  LHCb::MCParticleLocation::Default << "'" << endmsg;
  }else{   
    for(  LHCb::MCParticle::Vector::iterator i = mcparts->begin(); i != mcparts->end();  i++ ){     
      if((*i)->particleID().hasBottom()){
	bool testB = false;
	LHCb::MCParticle* myP = *i;

	const SmartRefVector < LHCb::MCVertex > myVs = myP->endVertices();
	Gaudi::LorentzVector tmp = (Gaudi::LorentzVector((*i)->momentum()));
	LHCb::Particle::ConstVector Bdaughters ;

	if(m_inAcc)
	  if(tmp.theta()<0.012 || tmp.theta()>0.35) continue;
	if(tmp.Pt()<m_PtSeedsMin) continue;

	if((*i)->endVertices().size() != 0){
	  const SmartRefVector<LHCb::MCVertex>& decays = (*i)->endVertices();
	  SmartRefVector<LHCb::MCVertex>::const_iterator ivtx;
	  for ( ivtx = decays.begin(); ivtx != decays.end(); ivtx++ )  {
	    if((*ivtx)->products().size()!=0){
	      const SmartRefVector<LHCb::MCParticle> daughters=(*ivtx)->products();
	      SmartRefVector<LHCb::MCParticle>::const_iterator idau;
	      for( idau = daughters.begin(); idau != daughters.end(); idau++ ) {
		if((*idau)->particleID().hasBottom()){
		  testB = true;
		  break;
		}
		LHCb::Particle             pDau;
		pDau.setParticleID     ((*idau)->particleID()) ;      
		pDau.setReferencePoint ( (*idau)->originVertex()->position() ) ;    
		pDau.setMomentum ( (*idau)->momentum()  ) ;    
		Bdaughters.push_back(pDau.clone());
		if(testB) {
	    	  for(LHCb::Particle::ConstVector::iterator op = Bdaughters.begin(); op != Bdaughters.end(); op++){ delete *op; }
		  Bdaughters.clear();		  
		  break; 
		}
	      }
	    }   
	    if(testB) break;    
	  }	  
	}
	if(testB) continue;		
	


	LHCb::Particle              pSeed ;
	LHCb::Particle              pB ;
	LHCb::Vertex                vSeed      ;

	
	pSeed.setParticleID     ( LHCb::ParticleID( m_seedID )) ;      
	pSeed.setReferencePoint (  myP->originVertex()->position()  ) ;    
	pSeed.setMomentum ( tmp  ) ;    
	StatusCode sc = m_combiner->combine ( Bdaughters , pSeed , vSeed ) ;
	if ( sc.isFailure())   Error ( "Error from momentum combiner, skip" , sc ) ; 
	Seeds.push_back(pSeed.clone());
	
      }   
    }
  }




  if ( Seeds.empty() ) {
    Warning ( "No Seeds from LoKiMCPartBHadronSeed") ;
    return StatusCode::SUCCESS ;
  }

  //=============================================================================
  // SORT THE SEEDS
  //=============================================================================  
  if(m_sort == 1) std::sort(Seeds.begin(), Seeds.end(),sortPt() );  
  if(m_sort == 2) std::sort(Seeds.begin(), Seeds.end(),sortE() );
  if(m_sort == 3) std::sort(Seeds.begin(), Seeds.end(),sortEta() );
  if(m_sort == 4) std::sort(Seeds.begin(), Seeds.end(),sortDauPt() );
  


  if ( statPrint() || msgLevel ( MSG::INFO ) ) 
    { counter ( "#seeds" ) += Seeds.size() ; }
  
  jets_ = Seeds ;




int  myj= 0;   
  for (   IJetMaker::Jets::iterator ijet = Seeds.begin() ; Seeds.end() != ijet ; ++ijet ) 
    {
      LHCb::Particle* pJet = *ijet;

      myj++;
      debug() << "GSeeds "<<myj<<"| nb of combin Seeds:  "<<  pJet->weight()<< "  nb part in the jet:  " << pJet->daughtersVector().size() <<"  Pt:  "<< pJet->pt() << endmsg;
      debug() <<         "        | m  : "<<  pJet->momentum() << endmsg;
      debug() <<         "        | eta: "<<  pJet->momentum().eta() << "  / phi: " <<   pJet->momentum().phi() <<endmsg;
  
    }





  return StatusCode::SUCCESS ;
}

// ============================================================================
/// The factory
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi,MCPartBHadronSeed)
// ============================================================================
// The END 
// ============================================================================
