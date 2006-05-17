// $Id: OfflineVertexFitter.cpp,v 1.2 2006-05-17 16:27:53 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 


#include "Event/Vertex.h"
#include "Event/Particle.h"

#include "Kernel/IParticleTransporter.h" 
#include "Kernel/IParticleStuffer.h" 

#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

//#include "gsl/gsl_vector.h"
//#include "gsl/gsl_matrix.h"
//#include "gsl/gsl_blas.h"

// local
#include "OfflineVertexFitter.h"

using namespace LHCb ;
using namespace Gaudi::Units;
//-----------------------------------------------------------------------------
// Implementation file for class : OfflineVertexFitter
//
// 2006-03-15 : Yuehong Xie
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<OfflineVertexFitter>          s_factory ;
const        IToolFactory& OfflineVertexFitterFactory = s_factory ; 

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
OfflineVertexFitter::OfflineVertexFitter( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_photonID(22)
  , m_stuffer()
  , m_transporter()
{
  declareInterface<IVertexFit>(this);

  declareProperty("useResonanceVertex", m_useResonanceVertex = true);
  declareProperty("applyDauMassConstraint", m_applyDauMassConstraint = true);
  declareProperty("widthThreshold", m_widthThreshold = 2.0 * MeV);

}

//=============================================================================
// Destructor
//=============================================================================
OfflineVertexFitter::~OfflineVertexFitter() {};

//=============================================================================
// Initialize
//=============================================================================
StatusCode OfflineVertexFitter::initialize(){
  StatusCode sc = GaudiTool::initialize();
  if (!sc) return sc;
  m_stuffer = tool<IParticleStuffer>("ParticleStuffer");
  m_transporter = tool<IParticleTransporter>("ParticleTransporter");
  m_ppSvc = svc<IParticlePropertySvc>("ParticlePropertySvc");

  ParticleProperty* partProp;
  partProp = m_ppSvc->find( "gamma" );
  m_photonID = (*partProp).jetsetID();
 
  return sc;
};

//=============================================================================
// Fit the vertex from a vector of Particles
//=============================================================================
StatusCode OfflineVertexFitter::fit( const LHCb::Particle::ConstVector& parts, 
                                 LHCb::Particle& P, LHCb::Vertex& V) const
{
  // xieyh
  debug() << "start fit " <<endreq;
  debug() << "using " << parts.size() <<" particles" <<endreq;
 
  P.clearDaughters();
  V.clearOutgoingParticles();
  V.setChi2(0.0);
  V.setNDoF(0);
  P.setEndVertex( &V );


  LHCb::Particle::ConstVector FlyingParticles;
  LHCb::Particle::ConstVector VertexedParticles;
  LHCb::Particle::ConstVector Photons;
  LHCb::Particle::ConstVector PhotonPairs;

  FlyingParticles.clear();
  VertexedParticles.clear();
  Photons.clear();
  PhotonPairs.clear();

  StatusCode sc = StatusCode::SUCCESS;

  debug() << "starting classfying partilces to fit " <<endreq;

  for ( Particle::ConstVector::const_iterator iPart=parts.begin(); iPart!=parts.end(); ++iPart ) {
    const Particle* parPointer = *iPart;
    sc= classify(parPointer, FlyingParticles, VertexedParticles, Photons, PhotonPairs);
    if(sc.isFailure()) {
      debug() << "Fail to classify a particle" << endreq;
      return StatusCode::FAILURE;
    }
  }
  
  debug() << "classfication result " <<endreq;
  debug() << "flying particles " << FlyingParticles.size() <<endreq;
  debug() << "vertexed particles " << VertexedParticles.size() <<endreq;
  debug() << "photons " << Photons.size() <<endreq;
  debug() << "photon pairs " << PhotonPairs.size() <<endreq;

  sc = seeding(P, FlyingParticles, VertexedParticles);
  if(sc.isFailure()) {
    debug() << "Fail to seed the vertex" << endreq;
    return StatusCode::FAILURE;
  }

  for(Particle::ConstVector::const_iterator iterP = VertexedParticles.begin(); iterP != VertexedParticles.end(); iterP++) {
    const Particle* daughter = *iterP;
    sc = addVertexed(P, daughter);
    if(sc.isFailure()) {
      debug() << "Fail to add a vertexed particle" << endreq;
      return StatusCode::FAILURE;
    }
  }


  for(Particle::ConstVector::const_iterator iterP = FlyingParticles.begin(); iterP != FlyingParticles.end(); iterP++) {
    const Particle* daughter = *iterP;
    sc = addFlying(P, daughter);
    if(sc.isFailure()) {
      debug() << "Fail to add a flying particle" << endreq;
      return StatusCode::FAILURE;
    }
  }

  for(Particle::ConstVector::const_iterator iterP = Photons.begin(); iterP != Photons.end(); iterP++) {
    const Particle* daughter = *iterP;
    addPhoton(P, daughter);
    if(sc.isFailure()) {
      debug() << "Fail to add a photon" << endreq;
      return StatusCode::FAILURE;
    }
  }

  for(Particle::ConstVector::const_iterator iterP = PhotonPairs.begin(); iterP != PhotonPairs.end(); iterP++) {
    const Particle* daughter = *iterP;
    addPhotonPair(P, daughter);
    if(sc.isFailure()) {
      debug() << "Fail to add a photon pair" << endreq;
      return StatusCode::FAILURE;
    }
  }

  for(Particle::ConstVector::const_iterator iterP = parts.begin(); iterP != parts.end(); iterP++) {
    P.addToDaughters(*iterP);
    V.addToOutgoingParticles(*iterP);
  }

  return StatusCode::SUCCESS;  
}

//=============================================================================
// Fit the vertex from a vector of Particles
//=============================================================================
StatusCode OfflineVertexFitter::fit( const LHCb::Particle::ConstVector& parts, 
                                 LHCb::Vertex& V) const{  
  LHCb::Particle tPart;
  return fit(parts, tPart, V);
}

//==================================================================
//  method to determine if a particle is a resonance
//==================================================================

bool OfflineVertexFitter::isResonance(const LHCb::Particle* part) const {
  bool isRes=false;
  int id=part->particleID().pid();  
  ParticleProperty*  partProp = m_ppSvc->findByStdHepID(id );
  if( (*partProp).lifetime()*pow(10,-9) < pow(10,-15)) isRes=true;
  return isRes;

}

//==================================================================
//  method to determine if a particle has a reconstructed vertex 
//==================================================================

bool OfflineVertexFitter::isVertexed(const LHCb::Particle* part) const{
  bool hasVertex=false;
  int nflying=countTraj(part);
  if(nflying>=2) hasVertex=true;
  return hasVertex;
}

//==================================================================
//  method to determine if a particle has non-photon final decay
//  products or not
//==================================================================
bool OfflineVertexFitter::isPurePhotonDecay(const LHCb::Particle* part) const{
  Particle::ConstVector Prods =part->daughtersVector();

  for ( Particle::ConstVector::const_iterator iProd=Prods.begin(); iProd!=Prods.end(); ++iProd ) {
     const Particle* daughter = *iProd;
     if( daughter->particleID().pid()!=m_photonID &&
         !isPurePhotonDecay(daughter) ) return false;
  }
  return true;
}

//==================================================================
//  method to determine if a particle decays to gamma gamma
//==================================================================
bool OfflineVertexFitter::isPhotonPair(const LHCb::Particle* part)  const{
  Particle::ConstVector Prods =part->daughtersVector();

  if(Prods.size()!=2) return false;                          
  for ( Particle::ConstVector::const_iterator iProd=Prods.begin(); iProd!=Prods.end(); ++iProd ) {
     const Particle* daughter = *iProd;
     if( daughter->particleID().pid()!=m_photonID ) return false;
  }

  return true;
}

//==================================================================
//  method to put a daughter particle in one of the four lists:
//  flying particles
//  vertexed particles
//  photons
//  photonpairs
//==================================================================

StatusCode OfflineVertexFitter::classify(const LHCb::Particle* part,
             LHCb::Particle::ConstVector & FlyingParticles,
             LHCb::Particle::ConstVector & VertexedParticles,
             LHCb::Particle::ConstVector & Photons,
             LHCb::Particle::ConstVector & PhotonPairs) const
{
  StatusCode sc = StatusCode::SUCCESS;

  int id=part->particleID().pid();

  if(part->isBasicParticle()) {
    if(id==m_photonID) Photons.push_back(part);
    else FlyingParticles.push_back(part);
  } else {
    if(!isResonance(part)) {
       FlyingParticles.push_back(part);
    }  else {
      if(isVertexed(part)) {
        if(m_useResonanceVertex) VertexedParticles.push_back(part);
        else {
          Particle::ConstVector Prods =part->daughtersVector();
          for ( Particle::ConstVector::const_iterator iProd=Prods.begin(); iProd!=Prods.end(); ++iProd ) {
            const Particle* daughter = *iProd;
            sc = classify(daughter, FlyingParticles, VertexedParticles, Photons, PhotonPairs);
            if(sc.isFailure()) return sc;
          }
        }
      } else {
        if(isPhotonPair(part)) PhotonPairs.push_back(part);
        else {
          Particle::ConstVector Prods =part->daughtersVector();
          for ( Particle::ConstVector::const_iterator iProd=Prods.begin(); iProd!=Prods.end(); ++iProd ) {
            const Particle* daughter = *iProd;
            sc=classify(daughter, FlyingParticles, VertexedParticles, Photons, PhotonPairs);
            if(sc.isFailure()) return sc;
          }
        }
      }
    }
  }

  return sc;
}

//==================================================================
//  method to seed a vertex
//==================================================================

StatusCode OfflineVertexFitter::seeding(LHCb::Particle& part,
             LHCb::Particle::ConstVector & FlyingParticles,
             LHCb::Particle::ConstVector & VertexedParticles) const
{
  StatusCode sc = StatusCode::FAILURE;

  if(VertexedParticles.size()<1 && FlyingParticles.size()<2) return StatusCode::FAILURE;

  if(VertexedParticles.size()>0) {
    const LHCb::Particle* vertpart = VertexedParticles.back();
    VertexedParticles.pop_back();

    Gaudi::Vector7 V7;
    Gaudi::SymMatrix7x7 C7;
    double chi2 = 0.;
    int NDoF = 0;
    getParticleInfo(*vertpart, V7, C7, chi2, NDoF);
    if(m_applyDauMassConstraint && !vertpart->isBasicParticle()) {
      int pid = vertpart->particleID().pid();
      ParticleProperty*  partProp = m_ppSvc->findByStdHepID(pid  );
      double hbar = 6.58211889*pow(10,-22);
      double wid = hbar/(pow(10,-9)*((*partProp).lifetime()));
      if(wid<m_widthThreshold) {
        double nominalMass = partProp->mass();
        sc=constrainMass(V7, C7, nominalMass);
        if(sc.isFailure()) return StatusCode::FAILURE;
      }
    }
    sc = updateParticle(part, V7, C7, chi2, NDoF);

  } else if (FlyingParticles.size()>1) {

    const LHCb::Particle* flypart1 = FlyingParticles.back();
    FlyingParticles.pop_back();  
    const LHCb::Particle* flypart2 = FlyingParticles.back();
    FlyingParticles.pop_back();

    Gaudi::Vector7 V7;
    Gaudi::SymMatrix7x7 C7;
    double chi2;
    int NDoF;

    sc =  fitTwo(flypart1, flypart2, V7, C7, chi2);
    if(sc.isFailure()) return StatusCode::FAILURE;

    sc = updateParticle(part, V7, C7, chi2, NDoF);

  }

  return sc;
}

//==================================================================
//  method to add a flying particle 
//==================================================================
StatusCode OfflineVertexFitter::addFlying(LHCb::Particle& part, 
             const LHCb::Particle * dau) const {
  StatusCode sc = StatusCode::SUCCESS;

  Gaudi::Vector7 V7;
  Gaudi::SymMatrix7x7 C7;
  double chi2 = 0.;
  int NDoF = 0;
  getParticleInfo(part, V7, C7, chi2, NDoF);

  Gaudi::Vector7 daupara;
  Gaudi::SymMatrix7x7 daucov;
  double dauchi2 = 0.;
  int dauNDoF = 0;
  getParticleInfo(*dau, daupara, daucov, dauchi2, dauNDoF);
  if(m_applyDauMassConstraint && !dau->isBasicParticle()) {
    int pid = dau->particleID().pid();
    ParticleProperty*  partProp = m_ppSvc->findByStdHepID(pid  );
    double hbar = 6.58211889*pow(10,-22);
    double wid = hbar/(pow(10,-9)*((*partProp).lifetime()));
    if(wid<m_widthThreshold) {
      double nominalMass = partProp->mass();
      sc=constrainMass(daupara, daucov, nominalMass);
      if(sc.isFailure()) return StatusCode::FAILURE;
    }
  }  


  return sc;
}

//==================================================================
//  method to add a vertexed particle 
//==================================================================
StatusCode OfflineVertexFitter::addVertexed(LHCb::Particle& part, 
             const LHCb::Particle * dau) const {
  StatusCode sc = StatusCode::SUCCESS;

  Gaudi::Vector7 V7;
  Gaudi::SymMatrix7x7 C7;
  double chi2 = 0.;
  int NDoF = 0;
  getParticleInfo(part, V7, C7, chi2, NDoF);

  Gaudi::Vector7 daupara;
  Gaudi::SymMatrix7x7 daucov;
  double dauchi2 = 0.;
  int dauNDoF = 0;
  getParticleInfo(*dau, daupara, daucov, dauchi2, dauNDoF);
  if(m_applyDauMassConstraint && !dau->isBasicParticle()) {
    int pid = dau->particleID().pid();
    ParticleProperty*  partProp = m_ppSvc->findByStdHepID(pid  );
    double hbar = 6.58211889*pow(10,-22);
    double wid = hbar/(pow(10,-9)*((*partProp).lifetime()));
    if(wid<m_widthThreshold) {
      double nominalMass = partProp->mass();
      sc=constrainMass(daupara, daucov, nominalMass);
      if(sc.isFailure()) return StatusCode::FAILURE;
    }
  }


  return sc;
}

//==================================================================
//  method to add a photon
//==================================================================
StatusCode OfflineVertexFitter::addPhoton(LHCb::Particle& part, 
             const LHCb::Particle * dau) const {
  StatusCode sc = StatusCode::SUCCESS;

  //adding code
  err() << "sorry, not implemented yet"<<endreq;

  return sc;
}

//==================================================================
//  method to add a photon pair
//==================================================================
StatusCode OfflineVertexFitter::addPhotonPair(LHCb::Particle& part, 
             const LHCb::Particle * dau) const {
  StatusCode sc = StatusCode::SUCCESS;

  //adding code
  err() << "sorry, not implemented yet"<<endreq;

  return sc;
}


//=============================================================================
// Fit two flying particles
//=============================================================================
StatusCode OfflineVertexFitter::fitTwo(const LHCb::Particle* dau1, 
             const LHCb::Particle* dau2,
             Gaudi::Vector7& V7,
             Gaudi::SymMatrix7x7& C7,
             double& chi2) const {

  StatusCode sc = StatusCode::SUCCESS;

  //adding code 

  Gaudi::Vector7 dau1para;
  Gaudi::SymMatrix7x7 dau1cov;
  double dau1chi2 = 0.;
  int dau1NDoF = 0;
  getParticleInfo(*dau1, dau1para, dau1cov, dau1chi2, dau1NDoF);
  if(m_applyDauMassConstraint && !dau1->isBasicParticle()) {
    int pid = dau1->particleID().pid();
    ParticleProperty*  partProp = m_ppSvc->findByStdHepID(pid  );
    double hbar = 6.58211889*pow(10,-22);
    double wid = hbar/(pow(10,-9)*((*partProp).lifetime()));
    if(wid<m_widthThreshold) {
      double nominalMass = partProp->mass();
      sc=constrainMass(dau1para, dau1cov, nominalMass);
      if(sc.isFailure()) return StatusCode::FAILURE;
    }
  }

  Gaudi::Vector7 dau2para;
  Gaudi::SymMatrix7x7 dau2cov;
  double dau2chi2 = 0.;
  int dau2NDoF = 0;
  getParticleInfo(*dau2, dau2para, dau2cov, dau2chi2, dau2NDoF);
  if(m_applyDauMassConstraint && !dau2->isBasicParticle()) {
    int pid = dau2->particleID().pid();
    ParticleProperty*  partProp = m_ppSvc->findByStdHepID(pid  );
    double hbar = 6.58211889*pow(10,-22);
    double wid = hbar/(pow(10,-9)*((*partProp).lifetime()));
    if(wid<m_widthThreshold) {
      double nominalMass = partProp->mass();
      sc=constrainMass(dau2para, dau2cov, nominalMass);
      if(sc.isFailure()) return StatusCode::FAILURE;
    }
  }
                                                                                                                       
  return sc;

}

//=============================================================================
// count the number of daughters that will contribute to vertexing
//=============================================================================
int OfflineVertexFitter::countTraj(const LHCb::Particle* part) const {
  int nt=0;

  if(part->isBasicParticle()) {
    int id=part->particleID().pid();
    if(id!=m_photonID) nt++;
  } else {
    if(!isResonance(part)) {
      nt++;
    } else {
      Particle::ConstVector Prods =part->daughtersVector();
      for ( Particle::ConstVector::const_iterator iProd=Prods.begin(); iProd!=Prods.end(); ++iProd ) {
        const Particle* daughter = *iProd;
        nt+=countTraj(daughter);
      }
    }
  }
  return nt;
}

//=============================================================================
// update particle momentum and vertex information
//=============================================================================
StatusCode OfflineVertexFitter::updateParticle(LHCb::Particle& part,
                                               Gaudi::Vector7& V7,
                                               Gaudi::SymMatrix7x7& C7,
                                               double& chi2, 
                                               int& NDoF) const 
{
  StatusCode sc = StatusCode::SUCCESS;
 
  Gaudi::XYZPoint refPoint(V7[0], V7[1], V7[2]);


  // @todo JPP check LorentzVector constructors vis a vis CLHEP ones! 
  Gaudi::LorentzVector lmom(V7[3], V7[4], V7[5], V7[6]);

  // JPP this is the translation.
  Gaudi::SymMatrix3x3 posCov    = C7.Sub<Gaudi::SymMatrix3x3>(0,0);
  Gaudi::SymMatrix4x4 momCov    = C7.Sub<Gaudi::SymMatrix4x4>(3,3);
  Gaudi::Matrix4x3    posMomCov = C7.Sub<Gaudi::Matrix4x3>(0,4);


  Gaudi::Vector7 Vm7;
  Gaudi::SymMatrix7x7 Cm7; 

  convertE2M(V7,C7,Vm7,Cm7);

  double measuredMass=Vm7[6];
  double measuredMassErr=Cm7(6,6);

  if(measuredMassErr>0) measuredMassErr=sqrt(measuredMassErr);

  LHCb::Vertex* tmpvert = part.endVertex();

  if(tmpvert!=NULL) {
    tmpvert->setChi2(chi2);
    tmpvert->setNDoF(NDoF);
    tmpvert->setPosition(refPoint);
    tmpvert->setCovMatrix(posCov);
  }

  part.setReferencePoint(refPoint);  
  part.setPosCovMatrix(posCov);
  part.setMomentum(lmom);
  part.setMomCovMatrix(momCov);
  part.setPosMomCovMatrix(posMomCov);

  part.setMeasuredMass(measuredMass);
  part.setMeasuredMassErr(measuredMassErr);

  return sc;
}

void OfflineVertexFitter::getParticleInfo(const LHCb::Particle& part,
                                          Gaudi::Vector7& V7,
                                          Gaudi::SymMatrix7x7& C7,
                                          double& chi2,
                                          int& NDoF) const 
{
  const LHCb::Vertex* tmpvert = part.endVertex();
                                                                                
  Gaudi::XYZPoint refPoint= part.referencePoint();
  Gaudi::LorentzVector lmom= part.momentum();

  V7(0)=refPoint.x();
  V7(1)=refPoint.y();
  V7(2)=refPoint.z();

  V7(3)=lmom.X();
  V7(4)=lmom.Y();
  V7(5)=lmom.Z();
  V7(6)=lmom.T();

  C7 = part.covMatrix();

  if(tmpvert!=NULL) {
    chi2 = tmpvert->chi2();
    NDoF = tmpvert->nDoF();
  } else {
    chi2 = 0;
    NDoF= 0;
  }
}

void OfflineVertexFitter::convertE2M(const Gaudi::Vector7& V7, 
                                     const Gaudi::SymMatrix7x7& C7,
                                     Gaudi::Vector7& Vm7, 
                                     Gaudi::SymMatrix7x7& Cm7) const
{
    Gaudi::Matrix7x7 Te2m = ROOT::Math::SMatrixIdentity();
    
    double px     = V7[3];
    double py     = V7[4];
    double pz     = V7[5];
    double energy = V7[6];
    double mass=sqrt(energy*energy-px*px-py*py-pz*pz);

    Te2m(6,3) = -px/mass;
    Te2m(6,4) = -py/mass;
    Te2m(6,5) = -pz/mass;
    Te2m(6,6) = energy/mass;

    // JPP translation
    Cm7 = ROOT::Math::Similarity<double,7,7>(Te2m, C7);
    
    Vm7=V7;
    Vm7[6]= mass;
}


void OfflineVertexFitter::convertM2E(const Gaudi::Vector7& Vm7, 
                                     const Gaudi::SymMatrix7x7& Cm7,
                                     Gaudi::Vector7& V7, 
                                     Gaudi::SymMatrix7x7& C7) const
{
    Gaudi::Matrix7x7 Tm2e = ROOT::Math::SMatrixIdentity();;

    double px   = Vm7[3];
    double py   = Vm7[4];
    double pz   = Vm7[5];
    double mass = Vm7[6];
    double energy=sqrt(mass*mass+px*px+py*py+pz*pz);

    Tm2e(6,3)=px/energy;
    Tm2e(6,4)=py/energy;
    Tm2e(6,5)=pz/energy;
    Tm2e(6,6)=mass/energy;

    // JPP translation
    C7 = ROOT::Math::Similarity<double,7,7>( Tm2e, Cm7);
    V7=Vm7;
    V7(6)= energy;
}



StatusCode OfflineVertexFitter::constrainMass(Gaudi::Vector7& V7, 
                                              Gaudi::SymMatrix7x7& C7, 
                                              double& nominalMass) const
{

  StatusCode sc = StatusCode::SUCCESS;

  Gaudi::Vector7 Vm;
  Gaudi::SymMatrix7x7 Cm;
  convertE2M(V7,C7,Vm,Cm);



  if(Cm(6,6)!=0.0) {
// need translation
/*
    HepMatrix DD(1,7,0);
    HepVector dd(1,0);
    
    DD(1,7)=1.;
    dd[0]=-nominalMass;
    HepSymMatrix Cd=Cm.similarity(DD); // return 1x1?
    int er=0;
    Cd.invert(er);
    if (er!=0) {
       debug() << "could not invert matrix Cd in constrainMass" << endmsg;
       return StatusCode::FAILURE;
    }
    Vm-= Cm*DD.T()*Cd*(DD*Vm+dd);
    HepSymMatrix delatCm1=Cd.similarityT(DD); // return 7x7? Should do.
    HepSymMatrix delatCm2=delatCm1.similarity(Cm);

    Cm-= delatCm2;
*/

// JPP Translation

    ROOT::Math::SMatrix<double,1,7> DD;
    Gaudi::Vector1 dd;

    DD(0,6)=1.;
    dd[0] = -nominalMass;
    Gaudi::SymMatrix1x1 Cd = ROOT::Math::Similarity<double, 1, 7>(DD,Cm);
    if ( !Cd.Invert() ) {
      debug() << "could not invert matrix Cd in constrainMass" << endmsg;
      return StatusCode::FAILURE;
    }
    Vm -= Cm*ROOT::Math::Transpose(DD)*Cd*(DD*Vm+dd);

    Gaudi::SymMatrix7x7 deltaCm1=ROOT::Math::SimilarityT<double,1,7>(DD,Cd);

    Gaudi::SymMatrix7x7 deltaCm2=ROOT::Math::Similarity<double,7,7>(Cm,deltaCm1);
    Cm -= deltaCm2;
  }

  for(int i=0; i<7; ++i) Cm(6,i)=0.0;

  convertM2E(Vm,Cm,V7,C7);

  return sc;
}


