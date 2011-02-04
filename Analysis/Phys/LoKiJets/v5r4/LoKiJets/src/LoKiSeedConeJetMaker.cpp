// $Id: LoKiSeedConeJetMaker.cpp,v 1.2 2009-12-14 12:34:33 cocov Exp $
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/ILoKiSvc.h"
// ============================================================================
// Local 
// ============================================================================
#include "LoKiSeedConeJetMaker.h"
// ============================================================================
/** @file 
 *  Implementation file for class  LoKi::SeedConeJetMaker
 *  @author Cedric POTTERAT   cedric.potterat@cern.ch
 *  @date   2011-01-31
 */
// ============================================================================
/*  standard initialization of the tool
 *  @return status code 
 */
// ============================================================================
StatusCode LoKi::SeedConeJetMaker::initialize () 
{
  StatusCode sc = GaudiTool::initialize() ;
  if ( sc.isFailure() ) { return sc ; }
  
  svc<LoKi::ILoKiSvc>( "LoKiSvc" , true ) ;

  //select your prefered SeedFinder
  m_seedFinder = tool<IJetMaker>( m_seedFinderName, this );
  if(!m_seedFinder){
    err() << "Unable to Retrieve Default SeedFinder" << endreq;
    return StatusCode::FAILURE;
  }
  
  m_combiner = tool<IParticleCombiner> ( m_combinerName , this ) ;
  if(!m_combiner){
    err() << "Unable to Retrieve Default ParticleCombiner" << endreq;
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS ;
} 


// ===========================================================================
// find the jets via a Seed Finder
// ===========================================================================
StatusCode LoKi::SeedConeJetMaker::makeJets 
( const IJetMaker::Input& input_ , IJetMaker::Jets& jets_ ) const 
{
  
  IJetMaker::Jets Seeds;


  //create seeds with the input particles
  m_seedFinder->makeJets(input_, Seeds);

  
  if ( Seeds.empty() ) {
    Warning ( "No Seeds from: "+ m_seedFinderName) ;
    return StatusCode::SUCCESS ;
  }

  //sort the seeds according to the selected creteria (SortSeed opt)
  if(m_sortSeed == 1) std::sort(Seeds.begin(), Seeds.end(),sortPt() );  
  if(m_sortSeed == 2) std::sort(Seeds.begin(), Seeds.end(),sortE() );
  if(m_sortSeed == 3) std::sort(Seeds.begin(), Seeds.end(),sortEta() );
  if(m_sortSeed == 4) std::sort(Seeds.begin(), Seeds.end(),sortDauPt() );


  LHCb::Particle::Vector  output  ;
  output.reserve( Seeds.size() ) ;
  

  //create jets arrount the seeds directions, use the RParameter opt, PtJetMin and the jetID.
  output = JetCone(m_r, Seeds, input_);

  //sort the jets according to the selected creteria (SortJet opt)
  if(m_sort == 1) std::sort(output.begin(), output.end(),sortPt() );  
  if(m_sort == 2) std::sort(output.begin(), output.end(),sortE() );
  if(m_sort == 3) std::sort(output.begin(), output.end(),sortEta() );
  if(m_sort == 4) std::sort(output.begin(), output.end(),sortDauPt() );



  if ( statPrint() || msgLevel ( MSG::INFO ) ) 
    { counter ( "#jets" ) += output.size() ; }
  
  jets_ = output ;


  int myj= 0;   
  for (   IJetMaker::Jets::iterator ijet = jets_.begin() ; jets_.end() != ijet ; ++ijet ) 
    {
      LHCb::Particle* pJet = *ijet;

      myj++;
      debug() << "SeedJets "<<myj<<"| nb of combin Seeds:  "<<  pJet->weight()<< "  nb part in the jet:  " << pJet->daughtersVector().size() <<"  Pt:  "<< pJet->pt() << endmsg;
      debug() <<           "         | m  : "<<  pJet->momentum() << endmsg;
      debug() <<           "         | eta: "<<  pJet->momentum().eta() << "  / phi: " <<   pJet->momentum().phi() <<endmsg;
  
    }


  //Delete the pointer to the seed
  for (   IJetMaker::Jets::iterator ijet = Seeds.begin() ; Seeds.end() != ijet ; ++ijet ) delete *ijet;



  return StatusCode::SUCCESS ;
}






//=============================================================================
// JETCONE: THE HEART OF EVERYTHING, BUILDS A JET AROUND A SEED
//=============================================================================
IJetMaker::Jets LoKi::SeedConeJetMaker::JetCone( const  double &Rmax,
						 IJetMaker::Jets &seeds,
						 const IJetMaker::Input &inputs) const{

  LHCb::Particle::Vector pJetVec ;
  IJetMaker::Jets::const_iterator iseeds;
  
  //loop over the seeds
  for ( iseeds=seeds.begin(); iseeds != seeds.end(); ++iseeds) {
    
    const LHCb::Particle* pSeed = *iseeds;
    
    LHCb::Particle              pJet ;
    LHCb::Vertex                vJet      ;
    LHCb::Particle::ConstVector daughters ;
    daughters.reserve( inputs.size() ) ;

    pJet.setParticleID     (LHCb::ParticleID(m_jetID)) ;    
    pJet.setReferencePoint ( pSeed->referencePoint() ) ;

    const double PI=3.14159265;   
    double  phiSeed = pSeed->momentum().phi();  double etaSeed = pSeed->momentum().eta();
    Gaudi::LorentzVector ptot =  Gaudi::LorentzVector(0., 0., 0. ,0.);
    
 
    
    LHCb::Particle::ConstVector::const_iterator ip;
    //loop over the particles
    for ( ip=inputs.begin(); ip != inputs.end(); ++ip) {   
    
      const LHCb::Particle* myPart = *ip;
      Gaudi::LorentzVector p1 =  myPart->momentum();
    
      double phiPart = myPart->momentum().phi();
      double etaPart   = myPart->momentum().eta();
      double Dphi = fabs(phiSeed-phiPart);
      if (Dphi>PI) { Dphi = 2*PI - Dphi; };
      double R = sqrt( Dphi*Dphi + (etaSeed-etaPart)*(etaSeed-etaPart) );
    
      //continue if not in DeltaR (Cone geom in eta-phi plan)
      if (R>Rmax)      continue;
      ptot = ptot + p1;
      
      daughters.push_back ( myPart );    
    }
  
    if ( daughters.empty())  Warning("Empty list of of daughter particles, skip it");

    //continue if the jet is below PtJetMin opt
    if(ptot.Pt() < m_ptmin) continue;
    //save the particles involved as the daugther of the jet
    StatusCode sc = m_combiner->combine ( daughters , pJet , vJet ) ;
    if ( sc.isFailure())   Error ( "Error from momentum combiner, skip" , sc ) ;

    pJet.setMomentum        ( Gaudi::LorentzVector ( ptot )  ) ;  
    pJetVec.push_back( pJet.clone() );

  }
  //return your jets
  return pJetVec;
}



// ============================================================================
/// The factory
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi,SeedConeJetMaker)
// ============================================================================
// The END 
// ============================================================================
