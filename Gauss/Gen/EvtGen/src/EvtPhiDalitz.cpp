 
#include <stdlib.h>
#include <math.h>
#include "EvtGenBase/EvtVector4R.hh"
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtGenKine.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenModels/EvtPhiDalitz.hh"
#include "EvtGenBase/EvtReport.hh"
#include <string>
 
EvtPhiDalitz::~EvtPhiDalitz() {}

void EvtPhiDalitz::getName(std::string& model_name){

  model_name="PHI_DALITZ";     

}


EvtDecayBase* EvtPhiDalitz::clone(){

  return new EvtPhiDalitz;

}

void EvtPhiDalitz::init(){

  // check that there are 0 arguments
  checkNArg(0);
  checkNDaug(3);

  checkSpinParent(EvtSpinType::VECTOR);

  checkSpinDaughter(0,EvtSpinType::SCALAR);
  checkSpinDaughter(1,EvtSpinType::SCALAR);
  checkSpinDaughter(2,EvtSpinType::SCALAR);
}



void EvtPhiDalitz::decay( EvtParticle *p){

  EvtId PIP=EvtPDL::getId("pi+");
  EvtId PIM=EvtPDL::getId("pi-");
  EvtId PIZ=EvtPDL::getId("pi0");
  EvtId RHOP=EvtPDL::getId("rho+");
  EvtId RHOM=EvtPDL::getId("rho-");
  EvtId RHOZ=EvtPDL::getId("rho0");
  EvtId OMEGA=EvtPDL::getId("omega");

  p->initializePhaseSpace(getNDaug(),getDaugs());

  EvtVector4R Ppip = p->getDaug(0)->getP4();
  EvtVector4R Ppim = p->getDaug(1)->getP4();
  EvtVector4R Ppi0 = p->getDaug(2)->getP4();
  EvtVector4R Qp = (Ppim + Ppi0);
  EvtVector4R Qm = (Ppip + Ppi0);
  EvtVector4R Q0 = (Ppip + Ppim);
  double m2_pip = pow(EvtPDL::getMeanMass(PIP),2); 
  double m2_pim = pow(EvtPDL::getMeanMass(PIM),2);
  double m2_pi0 = pow(EvtPDL::getMeanMass(PIZ),2);
  double M2rhop = pow(EvtPDL::getMeanMass(RHOP),2);
  double M2rhom = pow(EvtPDL::getMeanMass(RHOM),2);
  double M2rho0 = pow(EvtPDL::getMeanMass(RHOZ),2);
  double M2omega = pow(EvtPDL::getMeanMass(OMEGA),2);

  double Wrhop = EvtPDL::getWidth(RHOP);
  double Wrhom = EvtPDL::getWidth(RHOM);
  double Wrho0 = EvtPDL::getWidth(RHOZ);
  double Womega = EvtPDL::getWidth(OMEGA);
    
  EvtComplex Atot(0,0);

  //Rho+ Risonance Amplitude
  double Gp = Wrhop*pow(((Qp.mass2()-m2_pim-m2_pi0)/2-M2rhop/4)/(M2rhop/4-(m2_pim+m2_pi0)/2),3/2)*(M2rhop/Qp.mass2());
  EvtComplex Drhop((Qp.mass2()-M2rhop),Qp.mass()*Gp);
  EvtComplex A1(M2rhop/Drhop);

  //Rho- Risonance Amplitude
  double Gm = Wrhom*pow(((Qm.mass2()-m2_pip-m2_pi0)/2-M2rhom/4)/(M2rhom/4-(m2_pip+m2_pi0)/2),3/2)*(M2rhom/Qm.mass2());
  EvtComplex Drhom((Qm.mass2()-M2rhom),Qm.mass()*Gm);
  EvtComplex A2(M2rhom/Drhom);

  //Rho0 Risonance Amplitude
  double G0 = Wrho0*pow(((Q0.mass2()-m2_pip-m2_pim)/2-M2rho0/4)/(M2rho0/4-(m2_pip+m2_pim)/2),3/2)*(M2rho0/Q0.mass2());
  EvtComplex Drho0((Q0.mass2()-M2rho0),Q0.mass()*G0);
  EvtComplex A3(M2rho0/Drho0);
 
  //Omega Risonance Amplitude
  EvtComplex OmegaPhase(0,-0.22);    
  EvtComplex DOmega((Q0.mass2()-M2omega),Q0.mass()*Womega);
  EvtComplex A4(0.0071*M2omega*exp(OmegaPhase)/DOmega);

  //Direct Decay Amplitude
  EvtComplex DirPhase(0,2.47);
  EvtComplex A5(0.78*exp(DirPhase));

  Atot=A1+A2+A3+A4+A5;

  vertex(0,Atot);
  vertex(1,Atot);
  vertex(2,Atot);

  return ;
   
}







