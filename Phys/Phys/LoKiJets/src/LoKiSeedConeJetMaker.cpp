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

  m_combiner = tool<IParticleCombiner> ( m_combinerName , this ) ;

  return sc ;
}


// ===========================================================================
// find the jets via a Seed Finder
// ===========================================================================

StatusCode LoKi::SeedConeJetMaker::makeJets
( const IJetMaker::Input& input_ , IJetMaker::Jets& jets_ ) const
{



  const LHCb::RecVertex::Container* verts = get<LHCb::RecVertex::Container>(LHCb::RecVertexLocation::Primary);
  if(verts->size()!=1) return StatusCode::SUCCESS;
  LHCb::RecVertex::Container::const_iterator iv =verts->begin() ;

  const LHCb::RecVertex vtx = **iv ;

  makeJets(input_,  vtx, jets_ );

  return StatusCode::SUCCESS ;

}


StatusCode LoKi::SeedConeJetMaker::makeJets
( const IJetMaker::Input& input_ ,  const LHCb::RecVertex& vtx_ , IJetMaker::Jets& jets_ ) const
{

  IJetMaker::Jets Seeds;


  //create seeds with the input particles
  m_seedFinder->makeJets(input_, vtx_, Seeds);


  if ( Seeds.empty() ) {
    Warning ( "No Seeds from: "+ m_seedFinderName) ;
    return StatusCode::SUCCESS ;
  }

  //sort the seeds according to the selected creteria (SortSeed opt)
  if(m_sortSeed == 1) std::stable_sort(Seeds.begin(), Seeds.end(),sortPt() );
  if(m_sortSeed == 2) std::stable_sort(Seeds.begin(), Seeds.end(),sortE() );
  if(m_sortSeed == 3) std::stable_sort(Seeds.begin(), Seeds.end(),sortEta() );
  if(m_sortSeed == 4) std::stable_sort(Seeds.begin(), Seeds.end(),sortDauPt() );


  LHCb::Particle::Vector  output  ;
  LHCb::Particle::Vector  outputP  ;

  output.reserve( Seeds.size() ) ;
  outputP.reserve( Seeds.size() ) ;


  //create jets arrount the seeds directions, use the RParameter opt, PtJetMin and the jetID.
  output = JetCone(m_r, Seeds, input_);
  //sort the jets according to the selected creteria (SortJet opt)
  if(m_sort == 1) std::stable_sort(output.begin(), output.end(),sortPt() );
  if(m_sort == 2) std::stable_sort(output.begin(), output.end(),sortE() );
  if(m_sort == 3) std::stable_sort(output.begin(), output.end(),sortEta() );
  if(m_sort == 4) std::stable_sort(output.begin(), output.end(),sortDauPt() );
  int myj= 0;
  for (   LHCb::Particle::Vector::iterator ijet = output.begin() ; output.end() != ijet ; ++ijet )
  {
    LHCb::Particle* pJet = *ijet;

    myj++;
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "ConeJets "<<myj<<"| nb of combin Seeds:  "<<  pJet->weight()<< "  nb part in the jet:  " << pJet->daughtersVector().size() <<"  Pt:  "<< pJet->pt() << endmsg;
      debug() <<           "         | m  : "<<  pJet->momentum() << endmsg;
      debug() <<           "         | eta: "<<  pJet->momentum().eta() << "  / phi: " <<   pJet->momentum().phi() <<endmsg;
    }

  }


  if(m_uniquetrk){
    outputP = JetConePurged(m_r, output, input_);
    if(m_sort == 1) std::stable_sort(outputP.begin(), outputP.end(),sortPt() );
    if(m_sort == 2) std::stable_sort(outputP.begin(), outputP.end(),sortE() );
    if(m_sort == 3) std::stable_sort(outputP.begin(), outputP.end(),sortEta() );
    if(m_sort == 4) std::stable_sort(outputP.begin(), outputP.end(),sortDauPt() );

  } else
    outputP = output;


  myj= 0;
  for (   LHCb::Particle::Vector::iterator ijet = outputP.begin() ; outputP.end() != ijet ; ++ijet )
  {
    LHCb::Particle* pJet = *ijet;

    myj++;
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "ConeJetsP"<<myj<<"| nb of combin Seeds:  "<<  pJet->weight()<< "  nb part in the jet:  " << pJet->daughtersVector().size() <<"  Pt:  "<< pJet->pt() << endmsg;
      debug() <<           "         | m  : "<<  pJet->momentum() << endmsg;
      debug() <<           "         | eta: "<<  pJet->momentum().eta() << "  / phi: " <<   pJet->momentum().phi() <<endmsg;
    }

  }


  if ( statPrint() || msgLevel ( MSG::INFO ) )
  { counter ( "#jets" ) += outputP.size() ; }

  jets_ = outputP ;




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
    pJet.setEndVertex ( pSeed->endVertex() ) ;
    double  phiSeed =0;
    double  etaSeed =0;

    if(pSeed->endVertex() != NULL){
      Gaudi::XYZVector seedd = Gaudi::XYZVector(
                                                Gaudi::XYZPoint(pSeed->endVertex()->position()) -
                                                Gaudi::XYZPoint(pSeed->referencePoint())
                                                );
      phiSeed = seedd.phi();
      etaSeed = seedd.eta();

    }else{
      phiSeed = pSeed->momentum().phi();
      etaSeed = pSeed->momentum().eta();
    }



    const double PI=3.14159265;


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



//=============================================================================
// JETCONE: THE HEART OF EVERYTHING, BUILDS A JET AROUND A SEED
//=============================================================================
IJetMaker::Jets LoKi::SeedConeJetMaker::JetConePurged( const  double &Rmax,
                                                       IJetMaker::Jets &seeds,
                                                       const IJetMaker::Input &inputs) const{

  LHCb::Particle::Vector pJetVec ;
  IJetMaker::Jets::const_iterator iseeds;


  LHCb::Particle::Vector inputs2;

  LHCb::Particle::ConstVector::const_iterator ip;
  //loop over the particles
  for ( ip=inputs.begin(); ip != inputs.end(); ++ip)  inputs2.push_back((*ip)->clone());


  //loop over the seeds
  for ( iseeds=seeds.begin(); iseeds != seeds.end(); ++iseeds) {

    const LHCb::Particle* pSeed = *iseeds;

    LHCb::Particle              pJet ;
    LHCb::Vertex                vJet      ;
    LHCb::Particle::ConstVector daughters ;
    daughters.reserve( inputs.size() ) ;

    pJet.setParticleID     (LHCb::ParticleID(m_jetID)) ;
    pJet.setReferencePoint ( pSeed->referencePoint() ) ;
    pJet.setEndVertex ( pSeed->endVertex() ) ;
    double  phiSeed =0;
    double  etaSeed =0;

    if(pSeed->endVertex() != NULL){
      Gaudi::XYZVector seedd = Gaudi::XYZVector(
                                                Gaudi::XYZPoint(pSeed->endVertex()->position()) -
                                                Gaudi::XYZPoint(pSeed->referencePoint())
                                                );
      phiSeed = seedd.phi();
      etaSeed = seedd.eta();

    }else{
      phiSeed = pSeed->momentum().phi();
      etaSeed = pSeed->momentum().eta();
    }



    const double PI=3.14159265;


    Gaudi::LorentzVector ptot =  Gaudi::LorentzVector(0., 0., 0. ,0.);


    LHCb::Particle::Vector inputs3; //part not used!

    LHCb::Particle::Vector::iterator ip;
    //loop over the particles
    for ( ip=inputs2.begin(); ip != inputs2.end(); ++ip) {

      LHCb::Particle* myPart = *ip;
      Gaudi::LorentzVector p1 =  myPart->momentum();

      double phiPart = myPart->momentum().phi();
      double etaPart   = myPart->momentum().eta();
      double Dphi = fabs(phiSeed-phiPart);
      if (Dphi>PI) { Dphi = 2*PI - Dphi; };
      double R = sqrt( Dphi*Dphi + (etaSeed-etaPart)*(etaSeed-etaPart) );

      //continue if not in DeltaR (Cone geom in eta-phi plan)
      if (R>Rmax){
        inputs3.push_back(myPart);
      }
      else{
        ptot = ptot + p1;

        daughters.push_back ( myPart );
      }
    }
    inputs2 = inputs3; //remove the particle already assingned to a jet

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
