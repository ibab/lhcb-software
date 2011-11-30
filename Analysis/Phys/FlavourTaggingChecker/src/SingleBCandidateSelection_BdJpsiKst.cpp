// Include files 
#include "SingleBCandidateSelection_BdJpsiKst.h"
// from LHCb core
#include "Event/ODIN.h"

#include "DecayTreeFitter/Fitter.h"
#include "LoKi/Particles36.h"
#include "LoKi/ParticleCuts.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SingleBCandidateSelection_BdJpsiKst
//
// Author: Marco Musy
//-----------------------------------------------------------------------------

using namespace LHCb ;
using namespace Gaudi::Units;

using namespace LoKi::Cuts;


// Declaration of Factory
DECLARE_ALGORITHM_FACTORY( SingleBCandidateSelection_BdJpsiKst );

//=============================================================================
SingleBCandidateSelection_BdJpsiKst::SingleBCandidateSelection_BdJpsiKst(const std::string& name,
                                                                         ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator ),
    m_descend(0),
    m_util(0)
{
  declareProperty( "InputLocation",
                   m_inputLocation = "/Event/Dimuon/Phys/Bd2JpsiKstarUnbiasedLine");
}

//=============================================================================
StatusCode SingleBCandidateSelection_BdJpsiKst::execute() {

  setFilterPassed( false );

  debug() << "Getting particles saved in "<<m_inputLocation<< endmsg ;
  
  if(!exist<Particle::Container>(m_inputLocation) &&
     !exist<Particle::Selection>(m_inputLocation) ){
    debug()<<"nosignal found in : "<< m_inputLocation<<endmsg;
    
    return StatusCode::SUCCESS; //unselected
  }
  
  const Particle* AXBS  = 0;
  const Particles* ptmp = get<Particles>( m_inputLocation );
  double minchi2B = 9999;
  for( Particles::const_iterator ip=ptmp->begin(); ip!=ptmp->end(); ip++){
    debug()<<"signal part pt: "<< (*ip)->pt()
           << "  ID: "<< (*ip)->particleID().pid()<<endreq;

    if((*ip)->particleID().hasBottom()) {

      ////////////////////////////////////////////////////

      const Particle* Bd = (*ip);
      Particle::ConstVector parts = m_descend->descendants(Bd);

      const Particle* Kstar = findID(313, parts);//find K*0
      const Particle* Jpsi = findID(443, parts);//find Jpsi
      const Particle* muon  = findID( 13, parts);//find mu
      const Particle* muonplus  = findID( -13, parts);//find mu+
      const Particle* pion  = findID(-211, parts);//find pion
      const Particle* kaon  = findID(321, parts);//find kaon

      if(!Kstar)  return StatusCode::SUCCESS;
      if(!Jpsi)  return StatusCode::SUCCESS;
      if(!muon)   return StatusCode::SUCCESS;
      if(!muonplus)   return StatusCode::SUCCESS;
      if(!kaon)   return StatusCode::SUCCESS;
      if(!pion)   return StatusCode::SUCCESS;
      

      //work out physical variables
      double kaon_TRACK_Type    = kaon->proto()->track()->type();
      double kaon_TRACK_CHI2NDOF= kaon->proto()->track()->chi2()/kaon->proto()->track()->nDoF();
      double kaon_MINIPCHI2     = get_MINIPCHI2(kaon,0);
      double kaon_P    = kaon->p();
      double kaon_M    = 493.677;
      double kaon_E    = sqrt(kaon_P*kaon_P + kaon_M*kaon_M);
      double kaon_PT   = kaon->pt();
      double kaon_PIDK = kaon->proto()->info(LHCb::ProtoParticle::CombDLLk,-1000);
      double kaon_PIDp = kaon->proto()->info(LHCb::ProtoParticle::CombDLLp,-1000);

      double pion_TRACK_Type    = pion->proto()->track()->type();
      double pion_TRACK_CHI2NDOF= pion->proto()->track()->chi2()/pion->proto()->track()->nDoF();
      double pion_MINIPCHI2     = get_MINIPCHI2(pion,0);
      double pion_P    = pion->p();
      double pion_PT   = pion->pt();
      double pion_PIDK = pion->proto()->info(LHCb::ProtoParticle::CombDLLk,-1000);
      double pion_PIDp = pion->proto()->info(LHCb::ProtoParticle::CombDLLp,-1000);

      double Jpsi_MM = Jpsi->measuredMass();
      double Jpsi_MMerr = Jpsi->measuredMassErr();
      double Jpsi_PT = Jpsi->pt();
      double Jpsi_P = Jpsi->p();
      double Jpsi_E = sqrt(Jpsi_P*Jpsi_P + Jpsi_MM*Jpsi_MM);
      double Jpsi_ENDVERTEX_CHI2 = Jpsi->endVertex()->chi2();
      double Jpsi_ENDVERTEX_NDOF = Jpsi->endVertex()->nDoF();

      double Kstar_MM = Kstar->measuredMass();
      double Kstar_PT = Kstar->pt();
      double Kstar_ENDVERTEX_CHI2 = Kstar->endVertex()->chi2();
      double Kstar_ENDVERTEX_NDOF = Kstar->endVertex()->nDoF();

      double muon_TRACK_Type = muon->proto()->track()->type();
      double muon_PT    = muon->pt();
      double muon_PIDmu = muon->proto()->info(LHCb::ProtoParticle::CombDLLmu,-1000);
      double muon_MINIPCHI2 = get_MINIPCHI2(muon,0);
      double muon_TRACK_CHI2NDOF= muon->proto()->track()->chi2()/muon->proto()->track()->nDoF();

      double muonplus_TRACK_Type = muonplus->proto()->track()->type();
      double muonplus_PT    = muonplus->pt();
      double muonplus_PIDmu = muonplus->proto()->info(LHCb::ProtoParticle::CombDLLmu,-1000);
      double muonplus_MINIPCHI2 = get_MINIPCHI2(muon,0);
      double muonplus_TRACK_CHI2NDOF= muonplus->proto()->track()->chi2()/muonplus->proto()->track()->nDoF();

      double Bd_MM  = Bd->measuredMass();
      double Bd_PT  = Bd->pt();
      double Bd_ENDVERTEX_CHI2 = Bd->endVertex()->chi2();
      double Bd_ENDVERTEX_NDOF = Bd->endVertex()->nDoF();
      double Bd_ENDVERTEX_Z    = Bd->endVertex()->position().z();
      double Bd_MINIPCHI2 = get_MINIPCHI2(Bd,0);
      double Bd_MINIPCHI2NEXTBEST = get_MINIPCHI2(Bd,1);
      int nPV = (int) get_MINIPCHI2(Bd,2); // see the function below


 
  
      const VertexBase* aPV = bestPV( Bd );
      if( !aPV )return StatusCode::FAILURE;
      double Bd_OWNPV_Z = aPV->position().z();

      const Vertex* evtx = Bd->endVertex();
      if( !evtx ) return StatusCode::FAILURE;
      Gaudi::XYZVector A = Bd->momentum().Vect();
      Gaudi::XYZVector B = evtx->position() - aPV->position ();  
      double cosPFD = A.Dot( B ) / std::sqrt( A.Mag2()*B.Mag2() );
      double Bd_DIRA_OWNPV = cosPFD;

      debug()<<"Start DecayVertexFitter"<<endmsg;


      const DTF_CHI2NDOF fun = DTF_CHI2NDOF(true,"J/psi(1S)");
      const double DTF_chi2ndof = fun(Bd);

      const DTF_CTAU fun_tau = DTF_CTAU(0, true);
      const double DTF_tau = fun_tau(Bd);

      const DTF_CTAUERR fun_tau_err = DTF_CTAUERR(0, true);
      const double DTF_tau_err = fun_tau_err(Bd);

      
      const DTF_FUN fun2 = DTF_FUN(M, true,"J/psi(1S)");
      const double DTF_Bmass = fun2(Bd);

      debug()<<DTF_chi2ndof<<"   "<<endmsg;
      debug()<<DTF_Bmass<<"   "<<endmsg;

      ////////////////////////////////////////////////////
      //      put here your selection cuts              //
      ////////////////////////////////////////////////////

      bool Strip12 = false;

      LHCb::ODIN* odin(0);

      if( exist<ODIN>( LHCb::ODINLocation::Default ) ){
        odin = get<ODIN>( LHCb::ODINLocation::Default );
      }
      
      if(
         // muon
         muon_PIDmu >  0. && muonplus_PIDmu > 0 // ok
         && muon_TRACK_CHI2NDOF < 4. && muonplus_TRACK_CHI2NDOF < 4. // ok
         && (muon_PT >500.*MeV || muonplus_PT >500.*MeV) // ok
         && (Jpsi_ENDVERTEX_CHI2/Jpsi_ENDVERTEX_NDOF) < 16. //ok
         && abs( Jpsi_MM - 3096.916) < 80. //ok
         && abs( Jpsi_MM - 3096.916)/Jpsi_MMerr < 1.4*3

         && pion_TRACK_CHI2NDOF < 4. //ok
         && kaon_TRACK_CHI2NDOF < 4. //ok
         && kaon_PIDK > 0. // ok
         && kaon_PIDK - kaon_PIDp > -2. // ok

         && Kstar_PT > 1000
         && abs(Kstar_MM - 891.660 )< 70.
         && Kstar_ENDVERTEX_CHI2/Kstar_ENDVERTEX_NDOF < 16.

         // Bd
         && Bd_MM  > 5100
         && Bd_MM  < 5450.
         && Bd_ENDVERTEX_CHI2/Bd_ENDVERTEX_NDOF   < 10. // was 10.
         && Bd_MINIPCHI2 < 25. //ok
         && Bd_PT > 1000

         && DTF_chi2ndof < 5
         && ( nPV==1 || (nPV>1 && Bd_MINIPCHI2NEXTBEST > 50 ) )
         ) Strip12 = true ;

      if(!Strip12) continue;
      

      double chi2 = (*ip)->endVertex()->chi2PerDoF();
      if(minchi2B > chi2) {
        minchi2B = chi2;
        AXBS = (*ip);
      }
    }
    
  }
  
    
  if(!AXBS) return StatusCode::SUCCESS; //unselected


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
const Particle* SingleBCandidateSelection_BdJpsiKst::findID(unsigned int id, 
                                                            Particle::ConstVector& v,
                                                            std::string opts ){
  const Particle* p=0;
  debug() <<"searching for signal id: "<<id<<endmsg;
  
  for( Particle::ConstVector::const_iterator ip=v.begin(); ip!=v.end(); ip++){
    if( abs(id)==13 && (*ip)->particleID().pid() == id ) {
      const Particle* mater = m_util->motherof(*ip, v);
      if(mater->particleID().abspid()!=443) continue;
      
      p = (*ip);
      break;
    }
    else if( ( abs(id)==211 || abs(id) == 321) && (*ip)->particleID().abspid() == abs(id) ) {
      const Particle* mater = m_util->motherof(*ip, v);
      if(mater->particleID().abspid()!=313) continue;
      
      p = (*ip);
      break;
    }
    else if( (*ip)->particleID().abspid() == id ) {
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
/* double SingleBCandidateSelection_BdJpsiKst::get_MINIPCHI2(const Particle* p, int opt){  
  double minchi2 = -1 ;
  double minchi2nextbest = -1;

  const RecVertex::Range PV = primaryVertices();
  if ( !PV.empty() ){
    for (RecVertex::Range::const_iterator pv = PV.begin(); pv!=PV.end(); ++pv){
      double ip, chi2;
      StatusCode test2 = distanceCalculator()->distance ( p, *pv, ip, chi2 );
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
*/


 double SingleBCandidateSelection_BdJpsiKst::get_MINIPCHI2(const Particle* p, int opt){  
  double minchi2 = -1 ;
  double minchi2nextbest = -1;

  const RecVertex::Range PV = primaryVertices();

  IPVReFitter* m_pvReFitter = tool<IPVReFitter>("AdaptivePVReFitter", this );
  

  if ( !PV.empty() ){
    for (RecVertex::Range::const_iterator pv = PV.begin(); pv!=PV.end(); ++pv){
      
      RecVertex newPV(**pv);
      // refit PV (remove B signal tracks)
      StatusCode scfit = m_pvReFitter->remove(p, &newPV);
      if(!scfit) {
        Warning("ReFitter fails!",StatusCode::SUCCESS,10).ignore();
        continue;
      }

      double chi2;      
      double ip;
      
      LHCb::VertexBase* newPVPtr = (LHCb::VertexBase*)&newPV;
      StatusCode test2 = distanceCalculator()->distance ( p, newPVPtr, ip, chi2 );
      if( test2 ) {
        if ((chi2<minchi2) || (minchi2<0.))
        { 
          minchi2nextbest = minchi2; 
          minchi2 = chi2 ;
        }
        else
        { 
          if((chi2 < minchi2nextbest) || (minchi2nextbest < 0))
          {            
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
double SingleBCandidateSelection_BdJpsiKst::get_MIPDV(const Particle* p){
  
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
double SingleBCandidateSelection_BdJpsiKst::get_BPVVDCHI2(const Particle* B,
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
StatusCode SingleBCandidateSelection_BdJpsiKst::initialize() {

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
 
  return DVAlgorithm::initialize() ;
}
//=============================================================================
StatusCode SingleBCandidateSelection_BdJpsiKst::finalize() {
  return DVAlgorithm::finalize();
}
//=============================================================================
SingleBCandidateSelection_BdJpsiKst::~SingleBCandidateSelection_BdJpsiKst() {};

