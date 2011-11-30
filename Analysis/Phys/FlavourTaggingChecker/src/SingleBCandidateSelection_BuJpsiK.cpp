// Include files 
#include "SingleBCandidateSelection_BuJpsiK.h"

// from LHCb core
#include "Event/ODIN.h"
#include "DecayTreeFitter/Fitter.h"
#include "LoKi/Particles36.h"
#include "LoKi/ParticleCuts.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SingleBCandidateSelection_BuJpsiK
//
// Author: Marco Musy
//-----------------------------------------------------------------------------

using namespace LHCb ;
using namespace Gaudi::Units;
using namespace LoKi::Cuts;

// Declaration of Factory
DECLARE_ALGORITHM_FACTORY( SingleBCandidateSelection_BuJpsiK );

//=============================================================================
SingleBCandidateSelection_BuJpsiK::SingleBCandidateSelection_BuJpsiK(const std::string& name,
                                                                         ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator ),
    m_descend(0),
    m_util(0)
{
  declareProperty( "InputLocation",
                   m_inputLocation = "/Event/Dimuon/Phys/Bu2JpsiKUnbiasedLine");
}

//=============================================================================
StatusCode SingleBCandidateSelection_BuJpsiK::execute() {

  setFilterPassed( false );

  debug() << "Getting particles saved in "<<m_inputLocation<< endmsg ;
  
  if(!exist<Particle::Container>(m_inputLocation) &&
     !exist<Particle::Selection>(m_inputLocation) ){
    debug()<<"nosignal found in : "<< m_inputLocation<<endmsg;
    
    return StatusCode::SUCCESS; //unselected
  }
  double Mass_Bu(0), time_Bu(0);
  
  const Particle* AXBS  = 0;
  const Particles* ptmp = get<Particles>( m_inputLocation );
  double minchi2B = 9999;
  for( Particles::const_iterator ip=ptmp->begin(); ip!=ptmp->end(); ip++){
    debug()<<"signal part pt: "<< (*ip)->pt()
           << "  ID: "<< (*ip)->particleID().pid()<<endreq;

    if((*ip)->particleID().hasBottom()) {

      ////////////////////////////////////////////////////

      const Particle* Bu = (*ip);
      Particle::ConstVector parts = m_descend->descendants(Bu);

      const Particle* Jpsi = findID(443, parts);//find Jpsi
      const Particle* muon  = findID( 13, parts);//find mu
      const Particle* muonplus  = findID( -13, parts);//find mu+
      const Particle* kaon  = findID(321, parts);//find kaon

      if(!Jpsi)  return StatusCode::SUCCESS;
      if(!muon)   return StatusCode::SUCCESS;
      if(!muonplus)   return StatusCode::SUCCESS;
      if(!kaon)   return StatusCode::SUCCESS;
      

      //work out physical variables
      double kaon_TRACK_Type    = kaon->proto()->track()->type();
      double kaon_TRACK_CHI2NDOF= kaon->proto()->track()->chi2()/kaon->proto()->track()->nDoF();
      double kaon_MINIPCHI2     = get_MINIPCHI2(kaon,0);
      double kaon_P    = kaon->p();
      double kaon_PT   = kaon->pt();
      double kaon_PIDK = kaon->proto()->info(LHCb::ProtoParticle::CombDLLk,-1000);
      double kaon_PIDp = kaon->proto()->info(LHCb::ProtoParticle::CombDLLp,-1000);

      double Jpsi_MM = Jpsi->measuredMass();
      double Jpsi_MMerr = Jpsi->measuredMassErr();
      double Jpsi_PT = Jpsi->pt();
      double Jpsi_ENDVERTEX_CHI2 = Jpsi->endVertex()->chi2();
      double Jpsi_ENDVERTEX_NDOF = Jpsi->endVertex()->nDoF();

      double muon_TRACK_Type = muon->proto()->track()->type();
      double muon_PT    = muon->pt();
      double muon_PIDmu = muon->proto()->info(LHCb::ProtoParticle::CombDLLmu,-1000);
      double muon_MINIPCHI2 = get_MINIPCHI2(muon,0);
      double muon_TRACK_CHI2NDOF= muon->proto()->track()->chi2()/muon->proto()->track()->nDoF();

      double muonplus_TRACK_Type = muonplus->proto()->track()->type();
      double muonplus_PT    = muonplus->pt();
      double muonplus_PIDmu = muonplus->proto()->info(LHCb::ProtoParticle::CombDLLmu,-1000);
      double muonplus_MINIPCHI2 = get_MINIPCHI2(muonplus,0);
      double muonplus_TRACK_CHI2NDOF= muonplus->proto()->track()->chi2()/muonplus->proto()->track()->nDoF();

      double Bu_MM  = Bu->measuredMass();
      double BuMass = 5279.17;
      double Bu_ENDVERTEX_CHI2 = Bu->endVertex()->chi2();
      double Bu_ENDVERTEX_NDOF = Bu->endVertex()->nDoF();
      double Bu_ENDVERTEX_Z    = Bu->endVertex()->position().z();
      double Bu_MINIPCHI2 = get_MINIPCHI2(Bu,0);
      double Bu_MINIPCHI2NEXTBEST = get_MINIPCHI2(Bu,1);
      int nPV = (int) get_MINIPCHI2(Bu,2);

  
      const VertexBase* aPV = bestPV( Bu );
      if( !aPV )return StatusCode::FAILURE;
      double Bu_OWNPV_Z = aPV->position().z();

      const Vertex* evtx = Bu->endVertex();
      if( !evtx ) return StatusCode::FAILURE;
      Gaudi::XYZVector A = Bu->momentum().Vect();
      Gaudi::XYZVector B = evtx->position() - aPV->position ();  
      double cosPFD = A.Dot( B ) / std::sqrt( A.Mag2()*B.Mag2() );
      double Bu_DIRA_OWNPV = cosPFD;

      debug()<<"Start DecayVertexFitter"<<endmsg;
      
      
      const DTF_CHI2NDOF fun = DTF_CHI2NDOF(true,"J/psi(1S)");
      const double DTF_chi2ndof = fun(Bu);
      debug()<<DTF_chi2ndof<<"   "<<endmsg;

      
      double cLight = .299792458000;
      const DTF_CTAU fun_tau = DTF_CTAU(0, true);
      const double DTF_tau = fun_tau(Bu)/cLight;
      debug()<<DTF_tau<<"   "<<endmsg;
      
      /*

      const DTF_CTAUERR fun_tau_err = DTF_CTAUERR(0, true);
      const double DTF_tau_err = fun_tau_err(Bu);

      
      const DTF_FUN fun2 = DTF_FUN(M, true,"J/psi(1S)");
      const double DTF_Bmass = fun2(Bu);
      debug()<<DTF_Bmass<<"   "<<endmsg;
      */

      ////////////////////////////////////////////////////
      //      put here your selection cuts              //
      ////////////////////////////////////////////////////

      bool   Strip13b = false;

      LHCb::ODIN* odin(0);

      if( exist<ODIN>( LHCb::ODINLocation::Default ) ){
        odin = get<ODIN>( LHCb::ODINLocation::Default );
      }
      
      if(
         // muon
         muon_PIDmu >  0. && muonplus_PIDmu > 0 // ok
         && muon_TRACK_CHI2NDOF < 4. && muonplus_TRACK_CHI2NDOF < 4. // ok
         && muon_PT >500.*MeV && muonplus_PT >500.*MeV // ok
         && (Jpsi_ENDVERTEX_CHI2/Jpsi_ENDVERTEX_NDOF) < 11. //ok
         && abs( Jpsi_MM - 3096.7) < 80. //ok
         //&& Jpsi_MM > 3030. && Jpsi_MM< 3180.


         && kaon_TRACK_CHI2NDOF < 4. //ok
         && kaon_P  > 10000. //ok
         && kaon_PT  > 1000. //ok
         && kaon_PIDK > 0. // ok
         && kaon_PIDK - kaon_PIDp > -2. // ok ???

         // Bu
         && Bu_MM  > 5100
         && Bu_MM  < 5400.
         && Bu_MINIPCHI2 < 25. //ok
         && DTF_chi2ndof < 5
         && ( nPV==1 || (nPV>1 && (Bu_MINIPCHI2NEXTBEST > 50 || Bu_MINIPCHI2NEXTBEST<0  ) ))
         ) Strip13b = true ;

      if(!Strip13b) continue;
      
      
      //double chi2 = (*ip)->endVertex()->chi2PerDoF();
      double chi2 = DTF_chi2ndof; //(*ip)->endVertex()->chi2PerDoF();
      if(minchi2B > chi2) {
        minchi2B = chi2;
        AXBS = (*ip);
        Mass_Bu = Bu_MM;
        time_Bu = DTF_tau;        
      }
    }    
  }
  
    
  //  if(!AXBS || (Mass_Bu < 5185 || Mass_Bu > 5375. || time_Bu <0.28) ) return StatusCode::SUCCESS; //unselected
  //  if(!AXBS || (Mass_Bu < 5150 || Mass_Bu > 5400. || time_Bu <0.2) ) return StatusCode::SUCCESS; //unselected
  if(!AXBS || (Mass_Bu < 5150 || Mass_Bu > 5400.) ) return StatusCode::SUCCESS; //unselected
  //if(!AXBS ) return StatusCode::SUCCESS; //unselected


  //save to tes//////////////////////////////////////////////////
  Particle::ConstVector axdaughter(0);
  axdaughter.push_back(AXBS);
  debug()<<"Going to save this B hypo to TES  "<<endreq;
  this->cloneAndMarkTrees(axdaughter);

/* // Commentato
  debug()<<"Going to save this B hypo to TES  "<<endreq;
  StatusCode sc1 = desktop()->cloneTrees(axdaughter);
  if (sc1.isFailure()) {
    warning() << "Unable to clone Tree to TES" << endreq;
    return StatusCode::SUCCESS;
  }
*/
//   StatusCode sc2 = desktop()->saveTrees(axdaughter);
//   if (sc2.isFailure()) {
//     warning() << "Unable to save Tree to TES" << endreq;
//     return StatusCode::SUCCESS;
//   }

  setFilterPassed( true );
  return StatusCode::SUCCESS;
}

//=============================================================================
const Particle* SingleBCandidateSelection_BuJpsiK::findID(unsigned int id, 
                                                            Particle::ConstVector& v,
                                                            std::string opts ){
  const Particle* p=0;
  debug() <<"searching for signal id: "<<id<<endmsg;
  
  for( Particle::ConstVector::const_iterator ip=v.begin(); ip!=v.end(); ip++){

    debug() <<"  signal contains id: "<<(*ip)->particleID().pid()<<endmsg;
    

    if( ((abs(id)!=13) && (*ip)->particleID().abspid() == id) ||
        ((abs(id)==13) && (*ip)->particleID().pid() == id))
      {

        if(abs(id)==13){
          const Particle* mater = m_util->motherof(*ip, v);
          if(mater->particleID().abspid()!=443) continue;
        }
   
      
      p = (*ip);
      break;
    }
  }
  if(!p) {
    err()<<"particle not found id: "<<id<<endreq;
    return NULL;
  } 

  return p;

}

//=============================================================================
 double SingleBCandidateSelection_BuJpsiK::get_MINIPCHI2(const Particle* p, int opt){  
  double minchi2 = -1 ;
  double minchi2nextbest = -1;

  const RecVertex::Range PV = primaryVertices();
  //  std::cout<<" SingleBCandidate nPVs="<<PV.size()<<std::endl;
  
  if ( !PV.empty() ){
    for (RecVertex::Range::const_iterator pv = PV.begin(); pv!=PV.end(); ++pv){
      double ip, chi2, ipip;

      RecVertex newPV(**pv);
      Particle newPart(*p);
      StatusCode sc = m_pvReFitter->remove(&newPart, &newPV);     
      if(!sc) { 
        err()<<"ReFitter fails!"<<endreq; 
        continue; 
      }

      StatusCode test2 = distanceCalculator()->distance ( p, *pv, ipip, chi2 );
      test2 = distanceCalculator()->distance ( p, &newPV, ip, chi2 );
      /*
      std::cout<<"SingleBCandidate PV displacement ="<<
        sqrt(
             pow((*pv)->position().x()-newPV.position().x(),2)+
             pow((*pv)->position().y()-newPV.position().y(),2)+
             pow((*pv)->position().z()-newPV.position().z(),2) 
             ) 
               <<"ip ="<<ipip<<" ipReFit="<<ip<<std::endl;  
      */
      if( test2 ) {
        if ((chi2<minchi2) || (minchi2<0.)) {
          minchi2nextbest = minchi2 ;        
          minchi2 = chi2 ;     
        } else {
          if((chi2 < minchi2nextbest) || (minchi2nextbest < 0)) {
            minchi2nextbest = chi2;
          }
        }        
      }
    }
  }
  if(opt==0) return minchi2;
  else if(opt==1) return minchi2nextbest;
  else if(opt==2) return PV.size();
  else return -999;
  
}
//========================================================================
double SingleBCandidateSelection_BuJpsiK::get_MIPDV(const Particle* p){
  
  double minip = -1;
  const RecVertex::Range PV = primaryVertices();
  if ( !PV.empty() ){
    for (RecVertex::Range::const_iterator pv = PV.begin(); pv!=PV.end(); ++pv){
      double ip, chi2;
      StatusCode test = distanceCalculator()->distance ( p, *pv, ip, chi2 );
      if( test ){  
        if( ip<minip || minip<0.) minip = ip ;//sara
      }			
    }	
  }  return minip; 
}

//=============================================================================
double SingleBCandidateSelection_BuJpsiK::get_BPVVDCHI2(const Particle* B,
                                                          const Particle* P){
  const VertexBase* rV = getRelatedPV(B);

  double chi2 = 0 ;
  if ( 0==rV ){
    chi2 = -999. ;
  } else {
   // flight distance
    double dist = 0;
    StatusCode sc = distanceCalculator()->distance( rV, P->endVertex(), dist, chi2 );
    if (!sc) return -999.;
  }
  return chi2 ;
}


//=============================================================================
StatusCode SingleBCandidateSelection_BuJpsiK::initialize() {

  m_descend = tool<IParticleDescendants> ( "ParticleDescendants", this );
  if( ! m_descend ) {
    fatal() << "Unable to retrieve ParticleDescendants tool "<< endreq;
    return StatusCode::FAILURE;
  }
 
  m_util = tool<ITaggingUtilsChecker> ( "TaggingUtilsChecker", this );
  if( ! m_util ) {
    fatal() << "Unable to retrieve TaggingUtilsChecker tool "<< endreq;
    return StatusCode::FAILURE;
  } 
  m_pvReFitter = tool<IPVReFitter>("AdaptivePVReFitter", this );
  if(! m_pvReFitter) {
    fatal() << "Unable to retrieve AdaptivePVReFitter" << endreq;
    return StatusCode::FAILURE;
  }
  

  return DVAlgorithm::initialize() ;
}
//=============================================================================
StatusCode SingleBCandidateSelection_BuJpsiK::finalize() {
  return DVAlgorithm::finalize();
}
//=============================================================================
SingleBCandidateSelection_BuJpsiK::~SingleBCandidateSelection_BuJpsiK() {};

