// $Id: $
//#ifndef JETUTILS_H
//#define JETUTILS_H 1
// ============================================================================
// LoKi
// ============================================================================
//#include "LoKi/LoKi.h"
//#include "Kernel/IParticle2MCAssociator.h"
//#include "LoKi/GenTypes.h"
//#include "LoKi/GenParticleCuts.h"

// ============================================================
// JetMaker.h
// ============================================================
//#include "LoKiFastJetMaker.h"
//#include <Kernel/IJetMaker.h>
// Trigger Tool
//#include "Kernel/ITriggerTisTos.h"
// ===========================================================================
#include <cmath>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/SmartIF.h"
// ============================================================================
// PartProp
// ============================================================================
#include "Kernel/Nodes.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/valid.h"
#include "LoKi/MoreFunctions.h"
#include "LoKi/ParticleProperties.h"
#include "LoKi/Trees.h"
#include "LoKi/Services.h"
// ============================================================================
// LoKiGen
// ============================================================================
#include "LoKi/GenExtract.h"
#include "LoKi/GenAlgs.h"
#include "LoKi/GenOscillated.h"
#include "LoKi/IGenDecay.h"

#include "JetUtils.h"

#include "TMath.h"
using namespace TMath;

LoKi::JetUtils::MCNEndVertices* 
 LoKi::JetUtils::MCNEndVertices::clone() const
 { return new LoKi::JetUtils::MCNEndVertices ( *this ) ; }
 // ============================================================================
 //  MANDATORY: virtual destructor 
 // ============================================================================
 LoKi::JetUtils::MCNEndVertices::~MCNEndVertices() {}
 // ============================================================================
 //  MANDATORY: the only one essential method 
 // ============================================================================
 LoKi::JetUtils::MCNEndVertices::result_type 
 LoKi::JetUtils::MCNEndVertices::operator() 
   ( LoKi::JetUtils::MCNEndVertices:: argument p ) const 
 {
   if ( 0 != p ) { return (int) p->endVertices().size(); }
   Error ( "LHCb::MCParticle* points to NULL, return InvalidID" ) ;
   return LoKi::Constants::InvalidID ;
 }
 // ============================================================================
 //  "SHORT" representation, @see LoKi::AuxFunBase 
 // ============================================================================
 std::ostream& 
 LoKi::JetUtils::MCNEndVertices::fillStream 
 ( std::ostream& s ) const { return s << "MCNENDVERTICES" ; }

LoKi::JetUtils::Info4Jet* 
 LoKi::JetUtils::Info4Jet::clone() const
 { return new LoKi::JetUtils::Info4Jet ( *this ) ; }
 // ============================================================================
 //  MANDATORY: virtual destructor 
 // ============================================================================
 LoKi::JetUtils::Info4Jet::~Info4Jet() {}
 // ============================================================================
 //  MANDATORY: the only one essential method 
 // ============================================================================
 LoKi::JetUtils::Info4Jet::result_type 
 LoKi::JetUtils::Info4Jet::operator() 
   ( LoKi::JetUtils::Info4Jet:: argument p ) const 
 {
   if ( 0 != p ) { return (int) p->info(951,-10); }
   Error ( "LHCb::Particle* points to NULL, return InvalidID" ) ;
   return LoKi::Constants::InvalidID ;
 }
 // ============================================================================
 //  "SHORT" representation, @see LoKi::AuxFunBase 
 // ============================================================================
 std::ostream& 
 LoKi::JetUtils::Info4Jet::fillStream 
 ( std::ostream& s ) const { return s << "INFO4JET" ; }

 // =======================================================================

 LHCb::CaloCluster::Position * CaloClusterPosition(const LHCb::Particle * p){

    LHCb::CaloHypo::Hypos hypos = p->proto()->calo();
    LHCb::CaloHypo::Hypos::iterator ih = hypos.begin();
    LHCb::CaloHypo *ch = *ih;
    LHCb::CaloCluster::Position *pos = ch->position();
    return pos;
 }

// ======================================================================

 LoKi::JetUtils::CaloPositionX* 
 LoKi::JetUtils::CaloPositionX::clone() const
 { return new LoKi::JetUtils::CaloPositionX ( *this ) ; }

 // ============================================================================
 //  MANDATORY: virtual destructor 
 // ============================================================================
 LoKi::JetUtils::CaloPositionX::~CaloPositionX() {}
 // ============================================================================
 //  MANDATORY: the only one essential method 
 // ============================================================================


 LoKi::JetUtils::CaloPositionX::result_type 
 LoKi::JetUtils::CaloPositionX::operator() 
   ( LoKi::JetUtils::CaloPositionX:: argument p ) const 
 {
   if ( 0 != p ) { 
    return (int) CaloClusterPosition(p)->x(); }
   Error ( "LHCb::Particle* points to NULL, return InvalidID" ) ;
   return LoKi::Constants::InvalidID ;
 }
 // ============================================================================
 //  "SHORT" representation, @see LoKi::AuxFunBase 
 // ============================================================================
 std::ostream& 
 LoKi::JetUtils::CaloPositionX::fillStream 
 ( std::ostream& s ) const { return s << "CALOPOSITIONX" ; }

 // ============================================================================
 // ============================================================================

 LoKi::JetUtils::CaloPositionY* 
 LoKi::JetUtils::CaloPositionY::clone() const
 { return new LoKi::JetUtils::CaloPositionY ( *this ) ; }

 // ============================================================================
 //  MANDATORY: virtual destructor 
 // ============================================================================
 LoKi::JetUtils::CaloPositionY::~CaloPositionY() {}
 // ============================================================================
 //  MANDATORY: the only one essential method 
 // ============================================================================


 LoKi::JetUtils::CaloPositionY::result_type 
 LoKi::JetUtils::CaloPositionY::operator() 
   ( LoKi::JetUtils::CaloPositionY:: argument p ) const 
 {
   if ( 0 != p ) { 
    return (int) CaloClusterPosition(p)->y(); }
   Error ( "LHCb::Particle* points to NULL, return InvalidID" ) ;
   return LoKi::Constants::InvalidID ;
 }
 // ============================================================================
 //  "SHORT" representation, @see LoKi::AuxFunBase 
 // ============================================================================
 std::ostream& 
 LoKi::JetUtils::CaloPositionY::fillStream 
 ( std::ostream& s ) const { return s << "CALOPOSITIONY" ; }

 // ============================================================================
 // ============================================================================

 LoKi::JetUtils::CaloPositionZ* 
 LoKi::JetUtils::CaloPositionZ::clone() const
 { return new LoKi::JetUtils::CaloPositionZ ( *this ) ; }

 // ============================================================================
 //  MANDATORY: virtual destructor 
 // ============================================================================
 LoKi::JetUtils::CaloPositionZ::~CaloPositionZ() {}
 // ============================================================================
 //  MANDATORY: the only one essential method 
 // ============================================================================

 LoKi::JetUtils::CaloPositionZ::result_type 
 LoKi::JetUtils::CaloPositionZ::operator() 
   ( LoKi::JetUtils::CaloPositionZ:: argument p ) const 
 {
   if ( 0 != p ) { 
    return (int) CaloClusterPosition(p)->z(); }
   Error ( "LHCb::Particle* points to NULL, return InvalidID" ) ;
   return LoKi::Constants::InvalidID ;
 }
 // ============================================================================
 //  "SHORT" representation, @see LoKi::AuxFunBase 
 // ============================================================================
 std::ostream& 
 LoKi::JetUtils::CaloPositionZ::fillStream 
 ( std::ostream& s ) const { return s << "CALOPOSITIONZ" ; }

// ============================================================================
LoKi::JetUtils::GNDaughters* 
 LoKi::JetUtils::GNDaughters::clone() const
 { return new LoKi::JetUtils::GNDaughters ( *this ) ; }
 // ============================================================================
 //  MANDATORY: virtual destructor 
 // ============================================================================
 LoKi::JetUtils::GNDaughters::~GNDaughters() {}
 // ============================================================================
 //  MANDATORY: the only one essential method 
 // ============================================================================
 LoKi::JetUtils::GNDaughters::result_type 
LoKi::JetUtils::GNDaughters::operator() 
	(LoKi::GenParticles::Identifier:: argument p ) const 
{
	if ( 0 != p ) { if(p->end_vertex()) return (int) p->end_vertex()->particles_out_size ();

		Error ( "HepMC::GenParticle* -> end_vertex() points to NULL, return InvalidID" ) ;
	}
	Error ( "HepMC::GenParticle* points to NULL, return InvalidID" ) ;
	return LoKi::Constants::InvalidID ;
}
 // ============================================================================
 //  "SHORT" representation, @see LoKi::AuxFunBase 
 // ============================================================================
 std::ostream& 
 LoKi::JetUtils::GNDaughters::fillStream 
 ( std::ostream& s ) const { return s << "GNDAUGHTERS" ; }

void LoKi::JetUtils::getHltObjSVPos(const LHCb::HltObjectSummary *obj, double &x, double &y, double &z){
  LHCb::HltObjectSummary::Info info = obj->numericalInfo();
  x = info["3#Particle.referencePoint.x"];
  y = info["4#Particle.referencePoint.y"];
  z = info["2#Particle.referencePoint.z"];
}

 // =======================================================================
void LoKi::JetUtils::getHltObjP4(const LHCb::HltObjectSummary *obj,Gaudi::LorentzVector &p4){
  LHCb::HltObjectSummary::Info info = obj->numericalInfo();
  double p = 1/info["7#Particle.1/p"];
  double slopex = info["5#Particle.slopes.x"];
  double slopey = info["6#Particle.slopes.y"];
  double slopez = 1/Sqrt(slopex*slopex + slopey*slopey + 1.0);
  double pz = slopez*p;
  double mm = info["1#Particle.measuredMass"];
  p4.SetPxPyPzE(slopex*pz,slopey*pz,pz,Sqrt(p*p+mm*mm));
}

double LoKi::JetUtils::getHltObjsAngle(const LHCb::HltObjectSummary *obj1,
		       const LHCb::HltObjectSummary *obj2){
  Gaudi::LorentzVector p41,p42;
  getHltObjP4(obj1,p41);
  getHltObjP4(obj2,p42);
  //return p41.Vect().Angle(p42.Vect());
  return Abs(ACos(p41.Vect().Unit().Dot(p42.Vect().Unit())));
}

double LoKi::JetUtils::getHltObjsMass(const LHCb::HltObjectSummary *obj1,
		      const LHCb::HltObjectSummary *obj2){
  Gaudi::LorentzVector p41,p42;
  getHltObjP4(obj1,p41);
  getHltObjP4(obj2,p42);
  return (p41+p42).M();
}

bool LoKi::JetUtils::addLHCbIDs(SmartRefVector< LHCb::HltObjectSummary >::const_iterator iter,
		std::vector<int> &ids){
  unsigned int len = iter->target()->lhcbIDs().size();
  if(len > 0){
    for(unsigned int i = 0; i < len; i++){
      ids.push_back(iter->target()->lhcbIDs()[i].lhcbID());
    }
    return true;
  }
  return false;
}

void LoKi::JetUtils::getLHCbIDs(const LHCb::HltObjectSummary *obj, std::vector<int> &ids){
  const SmartRefVector< LHCb::HltObjectSummary > &sub1 = obj->substructure();
  SmartRefVector< LHCb::HltObjectSummary >::const_iterator iter1,iter2,iter3,
    iter4;
  int n = 0;
  for(iter1=sub1.begin();iter1!=sub1.end();++iter1){
    const SmartRefVector< LHCb::HltObjectSummary > &sub2 
      = iter1->target()->substructure();
    for(iter2=sub2.begin(); iter2!=sub2.end();++iter2){
      n++; //std::cout << "a";
      if(!addLHCbIDs(iter2,ids)){
	n--; //std::cout << "b";
	const SmartRefVector< LHCb::HltObjectSummary > &sub3 
	  = iter2->target()->substructure();
	for(iter3=sub3.begin(); iter3!=sub3.end();++iter3){
	  n++; //std::cout << "c";
	  if(!addLHCbIDs(iter3,ids)){
	    n--; //std::cout << "d";
	    const SmartRefVector< LHCb::HltObjectSummary > &sub4 
	      = iter3->target()->substructure();
	    for(iter4=sub4.begin(); iter4!=sub4.end();++iter4){
	      n++; //std::cout << "e";
	      addLHCbIDs(iter4,ids);
	    }
	  }
	}
      }
    }
  }
  /*
  std::cout << std::endl;
  int len = ids.size();
  std::cout << "LHCb IDs: (" << n << ":" << len << ") [ ";
  //for(int i = 0; i < len; i++) std::cout << ids[i] << " ";
  std::cout << "]" << std::endl;
  */
}

bool LoKi::JetUtils::doHltObjsOverlap(const LHCb::HltObjectSummary *obj1,
		      const LHCb::HltObjectSummary *obj2){
  std::vector<int> ids1, ids2;
  getLHCbIDs(obj1,ids1);
  getLHCbIDs(obj2,ids2);
  unsigned int l1 = ids1.size(), l2 = ids2.size();
  for(unsigned int i = 0; i < l1; i++){
    for(unsigned int j = 0; j < l2; j++){
      if(ids1[i] == ids2[j]) return true;
    }
  }
  return false;
}

bool LoKi::JetUtils::doLHCbIDsOverlap(const std::vector<int> &ids1,
		      const std::vector<int> &ids2){
  unsigned int l1 = ids1.size(), l2 = ids2.size();
  for(unsigned int i = 0; i < l1; i++){
    for(unsigned int j = 0; j < l2; j++){
      if(ids1[i] == ids2[j]) return true;
    }
  }
  return false;
}
//#endif /*JETUTILS_H*/ 
