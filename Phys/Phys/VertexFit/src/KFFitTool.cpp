
// Include files from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/GaudiException.h"
//

#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/DiagMatrix.h"

// Include files from Event
#include "Event/Particle.h"
#include "Event/Vertex.h"
#include "Event/ProtoParticle.h"

// CaloEvent
#include "Event/CaloHypo.h"
#include "Event/CaloPosition.h"


// local file
#include "KFFitTool.h"
#include "DaVinciTools/IParticleTransporter.h"

//--------------------------------------------------------------------
//
//  ClassName  : KFFitTool
// 
//  Description: Performs Kalman Filter type kinematic fit to a particle
//               decay chain
//                           
//  @Author     : Yuehong Xie
//
//  @Date 01/10/2004
//
//--------------------------------------------------------------------

//==================================================================
// Instantiation of a static factory class used by clients to create
// instances of this service
//==================================================================
static ToolFactory<KFFitTool> s_factory;
const IToolFactory& KFFitToolFactory = s_factory;

// ===========================================================================
namespace PhotonParametersLocal
{

  class IsHypo : public std::unary_function<const CaloHypo*,bool> {
    public:
      /// constructor
      IsHypo( CaloHypotheses::Hypothesis hypo ): m_hypo ( hypo ) {};
      /// functor interface
      bool operator() ( const CaloHypo* hypo ) const
      { return 0 != hypo && m_hypo == hypo->hypothesis() ? true : false ; }
    private:
      IsHypo();
    private:
      CaloHypotheses::Hypothesis m_hypo ;
  };

};
//==================================================================
// Standard Constructor
//==================================================================
KFFitTool::KFFitTool(const std::string& type, 
                               const std::string& name, 
                               const IInterface* parent) 
  : AlgTool( type, name, parent )
  , m_ppSvc(0)
  , m_pTransporter(0)
  , m_transporterType("CombinedTransporter")
{
  declareInterface<IKFFitTool>(this);
  declareProperty( "MaxIter", m_maxIter = 10);
  declareProperty( "MaxDeltaChiSq", m_maxDeltaChiSq = 0.001);
  declareProperty( "Transporter", m_transporterType);
  declareProperty( "WidthCut", m_widthCut = 2.0*MeV);
  declareProperty( "PhotonPairMassConstraint", m_PhotonPairMassConstraint = true 
);

}

//==================================================================
// Initialize
//==================================================================
StatusCode KFFitTool::initialize() {
  MsgStream log(msgSvc(), name());

  StatusCode sc = StatusCode::FAILURE;
  sc = service( "ParticlePropertySvc", m_ppSvc );
  if( sc.isFailure ()) {
    log << MSG::FATAL << "ParticlePropertySvc Not Found" << endreq;
    return StatusCode::FAILURE;
  }

  sc = toolSvc()->retrieveTool(m_transporterType, m_pTransporter, this);
  if(sc.isFailure()) {
    log << MSG::FATAL << "Unable to retrieve " << m_transporterType
        << "tool" << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;  
}

//==================================================================
//  refit a particle
//==================================================================

StatusCode KFFitTool::reFit( Particle& part ) {
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG <<"Now reFit "<< part.particleID().pid()<< endreq;
  
  // handle basic particles: e, mu, pi, K, P and gamma
  if(!isComposite(part)) {
    StatusCode sc=StatusCode::SUCCESS;
    if(part.charge()!=0) {
      sc=resetTrackParameters(part);
      HepVector para(7,0);
      HepSymMatrix cov(7,0);
      sc=getMParameter(part, para, cov);
      log << MSG::VERBOSE << "get cov  "<< cov << endmsg;
    }
    return sc;
  }

  //comment next line to keep phi(1020) untouched
  //if(part.particleID().pid()==333) return StatusCode::SUCCESS;

  //this version does nothing to Ks 
  if(part.particleID().pid()==310) return StatusCode::SUCCESS;

  //should not do anything here to particles which only decay to photons
  if(isPurePhotonDecay(part)) return StatusCode::SUCCESS;


  Vertex* endVertex = part.endVertex();
  SmartRefVector< Particle > & Prods = endVertex->products();
  SmartRefVector< Particle >::iterator iProd;

  ParticleVector  vDaughter; 
  for (iProd=Prods.begin(); iProd !=Prods.end(); ++iProd){
     Particle* daughter = *iProd;
     int pidDau = daughter->particleID().pid();

     reFit(*daughter);

     double dauWidth = ParticleWidth(*daughter);
     bool isDauComp= isComposite(*daughter);
     bool photonsOnly = isPurePhotonDecay(*daughter);

     double chi2Mass=-9999.;
     if(isDauComp && dauWidth<m_widthCut && !photonsOnly) {
       StatusCode okMass=massConstrain(*daughter, chi2Mass);
        if(okMass.isFailure()) {
          log << MSG::DEBUG << "massConstrain fails!"<<endreq;
          return okMass;        
        }
      }
     vDaughter.push_back(daughter);
  }

  StatusCode scdecay=fitDecay(part, vDaughter);
  if(scdecay.isFailure()) {
     log << MSG::DEBUG << "fitDecay fails in reFit!"<<endreq;
     return scdecay;
  }

  return StatusCode::SUCCESS;
}

//==================================================================
//  fit a decay process
//==================================================================
StatusCode KFFitTool::fitDecay( Particle& mother, 
                                ParticleVector& daughters) {
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG <<"Now entering fitDecay!"<<endreq;

  Vertex* endVertex = mother.endVertex();           
  ParticleVector::iterator iDau;
  StatusCode sc;

  ParticleVector vPhoton, vPhotonPair, vRes, vNonRes;
  for (iDau=daughters.begin(); iDau !=daughters.end(); ++iDau){
    Particle* dau = *iDau;
    int daupid = dau->particleID().pid();
    bool isDauRes=dau->isResonance();
    bool isPhoton=(daupid==22);
    bool isPhotonPair = (daupid==111 || daupid==221);
    if(isPhoton) vPhoton.push_back(dau);
    else if(isPhotonPair) vPhotonPair.push_back(dau);
    else if(isDauRes) vRes.push_back(dau);
    else vNonRes.push_back(dau);
  }

  HepVector Ve(7,0);
  HepSymMatrix Ce(7,0);

  bool hasVertex=false;
  int iRes=0;
  for (iDau=vRes.begin(); iDau !=vRes.end(); ++iDau){  
    iRes++;
    Particle* dau = *iDau;
    if(iRes==1) {
      sc=getEParameter(*dau, Ve, Ce);
      if(sc.isFailure ()) return sc;
    } else {
      sc=mergeTwoVertices(mother, *dau, Ve, Ce);      
      if(sc.isFailure ()) return sc;
    }
      sc=setEParameter(mother,Ve,Ce);
      if(sc.isFailure ()) return sc;
  }
  if(iRes>0) hasVertex=true;

  int iNonRes=0;
  Particle* dauFirst=NULL;

  for (iDau=vNonRes.begin(); iDau !=vNonRes.end(); ++iDau){
    iNonRes++;
    Particle* dau = *iDau;
    if(hasVertex) {
      sc=addParticle(mother, *dau, Ve, Ce);
      if(sc.isFailure ()) return sc;
      sc=setEParameter(mother,Ve,Ce);
      if(sc.isFailure ()) return sc; 
    } else {
      if(iNonRes==1) dauFirst=dau;
      if(iNonRes==2) {
        sc=fitWithTwoTrajectories(*dauFirst,*dau,Ve,Ce);
        if(sc.isFailure ()) return sc;
        sc=setEParameter(mother,Ve,Ce);
        if(sc.isFailure ()) return sc;
        hasVertex=true; 
      }
    }
  }

  if(!hasVertex) {
    log << MSG::DEBUG <<"No vertex found! "<<endreq;
    return StatusCode::FAILURE;
  }

  for (iDau=vPhoton.begin(); iDau !=vPhoton.end(); ++iDau){
    Particle* dau = *iDau;
    sc=addPhoton(mother, *dau, Ve,Ce);
    if(sc.isFailure ()) return sc;
    sc=setEParameter(mother,Ve,Ce);
    if(sc.isFailure ()) return sc;
  }

  for (iDau=vPhotonPair.begin(); iDau !=vPhotonPair.end(); ++iDau){
    Particle* dau = *iDau;
    sc=addPhotonPair(mother, *dau, Ve,Ce);
    if(sc.isFailure ()) return sc;
    sc=setEParameter(mother,Ve,Ce);
    if(sc.isFailure ()) return sc;
  }

  return StatusCode::SUCCESS;
}

//==================================================================
//  apply mass constraint to a Particle
//==================================================================
                                                                                
StatusCode KFFitTool::massConstrain( Particle& part, double& chi2) {
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << " Starting massConstrain "<< endmsg;

  ParticleID pid=part.particleID();
  int stdHepID = pid.pid();
  ParticleProperty*  partProp = m_ppSvc->findByStdHepID(stdHepID  );
  double nominalMass=(*partProp).mass();

  log << MSG::VERBOSE << "massConstrain: nominalMass  "<< nominalMass<< endmsg;
                         
  HepVector Ve(7,0), Vm(7,0);
  HepSymMatrix Ce(7,0), Cm(7,0);
  StatusCode sc=getMParameter(part, Vm, Cm);
  double alpha=Vm[6]-nominalMass;
  double olderrm2=Cm(7,7);                    
                                   
  // mass constraint
  log << MSG::VERBOSE << " 1111 mass before "<< Vm[6] << endmsg;
  log << MSG::VERBOSE << "Cm   "<< Cm << endmsg;
  log << MSG::VERBOSE << "Vm   "<< Vm << endmsg;

  getEParameter(part, Ve, Ce);
  log << MSG::VERBOSE << " direct Ce   "<< Ce << endmsg;

  if(Cm(7,7)!=0.0) {
    HepMatrix DD(1,7,0);
    HepVector dd(1,0);

    DD(1,7)=1.;
    dd[0]=-nominalMass;

    HepSymMatrix Cd=Cm.similarity(DD);
    int er=0;
    Cd.invert(er);
    if (er!=0) {
       log << MSG::ERROR << "could not invert matrix Cd in KFFitTool::massConstrain" << endmsg;
       return StatusCode::FAILURE;
    }     
    Vm-= Cm*DD.T()*Cd*(DD*Vm+dd);

    HepSymMatrix delatCm1=Cd.similarityT(DD);
    HepSymMatrix delatCm2=delatCm1.similarity(Cm);
                                                  
    Cm-= delatCm2;
    chi2=alpha/olderrm2;
  }
  for(int i=1;i<=7;i++) Cm(7,i)=0.0;

  log << MSG::VERBOSE << " 2222 mass after "<< Vm[6] << endmsg;
  log << MSG::VERBOSE << "Cm   "<< Cm << endmsg;
  log << MSG::VERBOSE << "Vm   "<< Vm << endmsg;

  sc=setMParameter(part, Vm, Cm);

  Vm2Ve(Vm,Ve);
  log << MSG::VERBOSE << "Ve   "<< Ve << endmsg;

  return sc;
}

//==================================================================
//  get Particle parameters with energy
//==================================================================

StatusCode KFFitTool::getEParameter(Particle& part, 
                                   HepVector& vpara, 
                                   HepSymMatrix& cov) {  
  HepPoint3D point=part.pointOnTrack();
  HepLorentzVector momentum = part.momentum();
  HepSymMatrix posErr=part.pointOnTrackErr();
  HepSymMatrix momErr=part.momentumErr();
  HepMatrix posMomCorr=part.posMomCorr();

  vpara[0]=point.x();
  vpara[1]=point.y();
  vpara[2]=point.z();
  vpara[3]=momentum.vect().x();
  vpara[4]=momentum.vect().y();
  vpara[5]=momentum.vect().z();
  vpara[6]=momentum.e();

  cov.sub(1,posErr);
  cov.sub(4,momErr);
  for (unsigned i=0;i<4;++i) {
    for (unsigned j=0;j<3;++j) cov[3+i][j] = posMomCorr[i][j];
  }

  return StatusCode::SUCCESS;
}

//==================================================================
//  set Particle parameters with energy
//==================================================================

StatusCode KFFitTool::setEParameter(Particle& part,
                                   HepVector& vpara,
                                   HepSymMatrix& cov) {
  HepPoint3D newPOT;
  HepLorentzVector newMom;
  
  newPOT[0]=vpara[0];   
  newPOT[1]=vpara[1];
  newPOT[2]=vpara[2];
  newMom(0)=vpara[3];
  newMom(1)=vpara[4];
  newMom(2)=vpara[5];
  newMom(3)=vpara[6];

  HepSymMatrix newPOTErr=cov.sub(1,3);
  HepSymMatrix newMomErr=cov.sub(4,7);
  HepMatrix covCopy=cov;
  HepMatrix newPosMomCorr=covCopy.sub(4,7,1,3);

  double mass=newMom.mag();
  HepMatrix Te2m=MatrixE2M(part);
  HepSymMatrix covm=cov.similarity(Te2m);  
  double massErr=sqrt(covm(7,7));

  part.setPointOnTrack(newPOT);
  part.setPointOnTrackErr(newPOTErr);
  part.setMomentum(newMom);
  part.setMomentumErr(newMomErr);
  part.setPosMomCorr(newPosMomCorr);
  part.setMass(mass);
  part.setMassErr(massErr);
  Vertex* endV= part.endVertex();
  if(endV!=0) {
   endV->setPosition(newPOT);
   endV->setPositionErr(newPOTErr);
  }
  return StatusCode::SUCCESS;
}

//==================================================================
//  get Particle parameters with mass
//==================================================================
StatusCode KFFitTool::getMParameter(Particle& part,
                                   HepVector& vpara,
                                   HepSymMatrix& cov) {
  StatusCode sc=getEParameter(part, vpara, cov);
  if(sc.isFailure ()) return sc;

  HepMatrix Te2m=MatrixE2M(part);  

  vpara[6]=sqrt(vpara[6]*vpara[6]
               -vpara[3]*vpara[3]-vpara[4]*vpara[4]-vpara[5]*vpara[5]);  
  cov=cov.similarity(Te2m);

  double massErr=part.massErr();
  if(massErr==0)  for(int i=1;i<=7;i++) cov(7,i)=0.;

  return StatusCode::SUCCESS;
}


//==================================================================
//  set Particle parameters with mass
//==================================================================
StatusCode KFFitTool::setMParameter(Particle& part,
                                   HepVector& vpara,
                                   HepSymMatrix& cov) {
  MsgStream log(msgSvc(), name());

  HepPoint3D newPOT;
  HepLorentzVector newMom;

  newPOT[0]=vpara[0];
  newPOT[1]=vpara[1];
  newPOT[2]=vpara[2];
  newMom(0)=vpara[3];
  newMom(1)=vpara[4];
  newMom(2)=vpara[5];
  newMom(3)=sqrt(vpara[6]*vpara[6]+
                 vpara[3]*vpara[3]+vpara[4]*vpara[4]+vpara[5]*vpara[5]);

  HepMatrix Tm2e=MatrixM2E(part);
  HepSymMatrix cove=cov.similarity(Tm2e);

  HepSymMatrix newPOTErr=cove.sub(1,3);
  HepSymMatrix newMomErr=cove.sub(4,7);
  HepMatrix coveCopy=cove;
  HepMatrix newPosMomCorr=coveCopy.sub(4,7,1,3);

  double mass=vpara[6];

  double massErr=sqrt(cov(7,7));

  part.setPointOnTrack(newPOT);
  part.setPointOnTrackErr(newPOTErr);
  part.setMomentum(newMom);
  part.setMomentumErr(newMomErr);
  part.setPosMomCorr(newPosMomCorr);
  part.setMass(mass);
  part.setMassErr(massErr);
  Vertex* endV= part.endVertex();
  if(endV!=0) {
    endV->setPosition(newPOT);
    endV->setPositionErr(newPOTErr);
  }

  return StatusCode::SUCCESS;
}

//==================================================================
//  fit two non-resonance Particles
//==================================================================

StatusCode KFFitTool::fitWithTwoTrajectories(Particle& part1, 
                                             Particle& part2,
                                             HepVector& Ve, 
                                             HepSymMatrix& Ce ) {
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "fitWithTwoTrajectories!!! " << endmsg;

  HepVector para1(7,0);
  HepSymMatrix cov1(7,0);
  StatusCode sc=getMParameter(part1, para1, cov1);
  if(sc.isFailure ()) return sc;
  log << MSG::VERBOSE <<"cov1 "<<cov1<<endreq;

  HepVector para2(7,0);
  HepSymMatrix cov2(7,0);
  sc=getMParameter(part2, para2, cov2);
  if(sc.isFailure ()) return sc;
  log << MSG::VERBOSE <<"cov2 "<<cov2<<endreq;

  double zfit=(para1(2)-para2(2)+para2(5)*para2(3)-para1(5)*para1(3))/(para2(5)-para1(5));
  double zestimate=-999999.;

  int iterTransport=0;
//  while(fabs(zestimate-zfit)>10. && iterTransport< m_maxIter) {   
  {
     iterTransport++;
     zestimate=zfit;
     Particle transParticle1;
     sc = m_pTransporter->transport(part1, zestimate, transParticle1);
     if( sc.isFailure ()) {
       log << MSG::ERROR << "transport of part1 failed in fitWithTwoTrajectories!" << endreq;
       return sc;
     }
     sc=resetTrackParameters(transParticle1);

     Particle transParticle2;
     sc = m_pTransporter->transport(part2, zestimate, transParticle2);
     if( sc.isFailure ()) {
       log << MSG::ERROR << "transport of part1 failed in fitWithTwoTrajectories!" << endreq;
       return sc;
     }
     sc=resetTrackParameters(transParticle2);

     sc=getMParameter(transParticle1, para1, cov1);
     if(sc.isFailure ()) return sc;
     log << MSG::VERBOSE <<"After transport ... "<<endreq;
     log << MSG::VERBOSE <<"para1= "<< para1<<endreq;
     log << MSG::VERBOSE <<"cov1 "<<cov1<<endreq;

     sc=getMParameter(transParticle2, para2, cov2);
     if(sc.isFailure ()) return sc;
     log << MSG::VERBOSE <<"para2= "<< para2<<endreq;
     log << MSG::VERBOSE <<"cov2 "<<cov2<<endreq;

     HepVector X(12);
     X(1)=para1(1);
     X(2)=para1(2);
     X(3)=para1(4)/para1(6);
     X(4)=para1(5)/para1(6);
     X(5)=para1(6);
     X(6)=para1(7);
     X(7)=para2(1);
     X(8)=para2(2);
     X(9)=para2(4)/para2(6);
     X(10)=para2(5)/para2(6);
     X(11)=para2(6);
     X(12)=para2(7);

     HepMatrix mom2slope1(6,7,0);
     mom2slope1(1,1)=1.;
     mom2slope1(2,2)=1.;
     mom2slope1(3,4)=1./para1(6);
     mom2slope1(3,6)=-para1(4)/para1(6)/para1(6);
     mom2slope1(4,5)=1./para1(6);
     mom2slope1(4,6)=-para1(5)/para1(6)/para1(6);
     mom2slope1(5,6)=1.;
     mom2slope1(6,7)=1.;
     HepSymMatrix newcov1=cov1.similarity(mom2slope1);
     log << MSG::VERBOSE <<"newcov1 "<<newcov1<<endreq;

     HepMatrix mom2slope2(6,7,0);
     mom2slope2(1,1)=1.;
     mom2slope2(2,2)=1.;
     mom2slope2(3,4)=1./para2(6);
     mom2slope2(3,6)=-para2(4)/para2(6)/para2(6);
     mom2slope2(4,5)=1./para2(6);
     mom2slope2(4,6)=-para2(5)/para2(6)/para2(6);
     mom2slope2(5,6)=1.;
     mom2slope2(6,7)=1.;
     HepSymMatrix newcov2=cov2.similarity(mom2slope2);
     log << MSG::VERBOSE <<"newcov2 "<<newcov2<<endreq;

     HepSymMatrix Cx(12,0);
     Cx*=0.;
     Cx.sub(1,newcov1);
     Cx.sub(7,newcov2);

     log << MSG::VERBOSE << "X=  " << X << endreq;
     log << MSG::VERBOSE << "Cx=  " << Cx << endreq;

     HepVector  vfit=X;
     HepSymMatrix cfit=Cx;

     double m1=vfit(6);
     double m2=vfit(12);
     log << MSG::VERBOSE <<"initial m1= "<< m1<<endreq;
     log << MSG::VERBOSE <<"initial m2= "<< m2<<endreq;

     HepMatrix D(1,12,0);
     D(1,1)=-(vfit(10)-vfit(4));
     D(1,2)=vfit(9)-vfit(3);
     D(1,3)=vfit(8)-vfit(2);
     D(1,4)=-(vfit(7)-vfit(1));
     D(1,7)=vfit(10)-vfit(4);
     D(1,8)=-(vfit(9)-vfit(3));
     D(1,9)=-(vfit(8)-vfit(2));
     D(1,10)=vfit(7)-vfit(1);


     HepVector f(1);
     f(1)=(vfit(7)-vfit(1))*(vfit(10)-vfit(4))-(vfit(8)-vfit(2))*(vfit(9)-vfit(3));
     HepVector d= f - D*vfit;

     log << MSG::VERBOSE << "initial constraint values   " << f << endreq;

     double chi2Previous=9999.;
     double chi2=999.;

     bool converged=false;
     int iter=0;

     while(!converged && iter< m_maxIter)  {
       iter++;
       log << MSG::VERBOSE << ":-) Iteration   " << iter << endreq;
       log << MSG::VERBOSE << "D=  " << D << endreq;

       int ier=0;
       HepSymMatrix VD=Cx.similarity(D);
       log << MSG::VERBOSE << "VD before inversion  " << VD << endreq;
       log << MSG::VERBOSE << "VD determinant before inversion  "
           << VD.determinant() <<endreq;
       VD.invert(ier);
       if(ier!=0) {
         log << MSG::ERROR << "could not invert matrix VD in KFFitTool::fitWithTwoTrajectories"
             <<endreq;
         return StatusCode::FAILURE;
       }

       log << MSG::VERBOSE << "VD after inversion  " << VD << endreq;

       HepVector alpha=D*X+d;
       HepVector lamda=VD*alpha;
       vfit=X-Cx*D.T()*lamda;
       HepSymMatrix delataC1=VD.similarityT(D);
       HepSymMatrix delataC2=delataC1.similarity(Cx);
       cfit=Cx -delataC2;

       HepSymMatrix Q=VD.similarityT(alpha);
       D(1,1)=-(vfit(10)-vfit(4));
       D(1,2)=vfit(9)-vfit(3);
       D(1,3)=vfit(8)-vfit(2);
       D(1,4)=-(vfit(7)-vfit(1));
       D(1,7)=vfit(10)-vfit(4);
       D(1,8)=-(vfit(9)-vfit(3));
       D(1,9)=-(vfit(8)-vfit(2));
       D(1,10)=vfit(7)-vfit(1);
       f(1)=(vfit(7)-vfit(1))*(vfit(10)-vfit(4))-(vfit(8)-vfit(2))*(vfit(9)-vfit(3));
       d= f - D*vfit;
       log << MSG::VERBOSE << "constraint values   " << f << endreq;
       HepVector S=lamda.T()*f;
       chi2=Q(1,1)+2.*S(1);

       log << MSG::VERBOSE << "New chi2=  " << chi2 << endreq;
       log << MSG::VERBOSE << "Previous chi2=  " << chi2Previous << endreq;

       log << MSG::VERBOSE << "vfit= "<<vfit << endreq;

       if(fabs(chi2-chi2Previous)<m_maxDeltaChiSq) {
         converged=true;
       } else {
         chi2Previous=chi2;
       }

     }
     log << MSG::VERBOSE <<"vfit= "<< vfit<<endreq;
   
     m1=vfit(6);
     m2=vfit(12);

     log << MSG::VERBOSE <<"m1= "<< m1<<endreq;
     log << MSG::VERBOSE <<"m2= "<< m2<<endreq;


     Ve(1)=vfit(1) - vfit(3)*(vfit(7)-vfit(1))/(vfit(9)-vfit(3));
     Ve(2)=vfit(2) - vfit(4)*(vfit(7)-vfit(1))/(vfit(9)-vfit(3));
     Ve(3)=zestimate - (vfit(7)-vfit(1))/(vfit(9)-vfit(3));
     Ve(4)=vfit(3)*vfit(5)+vfit(9)*vfit(11);
     Ve(5)=vfit(4)*vfit(5)+vfit(10)*vfit(11);
     Ve(6)=vfit(5)+vfit(11);
     double e1=sqrt( vfit(6)*vfit(6) + (vfit(3)*vfit(3)+vfit(4)*vfit(4)+1.)*vfit(5)*vfit(5) );
     double e2=sqrt( vfit(12)*vfit(12) + (vfit(9)*vfit(9)+vfit(10)*vfit(10)+1.)*vfit(11)*vfit(11) );
     Ve(7)=e1+e2;

     log << MSG::VERBOSE <<"Ve= "<< Ve<<endreq;

     HepMatrix JA(7,12,0);
     JA(1,1)=1. + vfit(3)/(vfit(9)-vfit(3));
     JA(1,3)= - (vfit(7)-vfit(1))/(vfit(9)-vfit(3)) 
              - vfit(3)*(vfit(7)-vfit(1))/(vfit(9)-vfit(3))/(vfit(9)-vfit(3));
     JA(1,7)= - vfit(3)/(vfit(9)-vfit(3));
     JA(1,9)=  vfit(3)*(vfit(7)-vfit(1))/(vfit(9)-vfit(3))/(vfit(9)-vfit(3));
     JA(2,1)=  vfit(4)/(vfit(9)-vfit(3));
     JA(2,2)=1. ;
     JA(2,3)= - vfit(4)*(vfit(7)-vfit(1))/(vfit(9)-vfit(3))/(vfit(9)-vfit(3));
     JA(2,4)= - (vfit(7)-vfit(1))/(vfit(9)-vfit(3));
     JA(2,7)= - vfit(4)/(vfit(9)-vfit(3));
     JA(2,9)=   vfit(4)*(vfit(7)-vfit(1))/(vfit(9)-vfit(3))/(vfit(9)-vfit(3));
     JA(3,1)= 1./(vfit(9)-vfit(3)); 
     JA(3,3)= - (vfit(7)-vfit(1))/(vfit(9)-vfit(3))/(vfit(9)-vfit(3));
     JA(3,7)= - 1./(vfit(9)-vfit(3));
     JA(3,9)= (vfit(7)-vfit(1))/(vfit(9)-vfit(3))/(vfit(9)-vfit(3));
     JA(4,3)= vfit(5);
     JA(4,5)= vfit(3);
     JA(4,9)= vfit(11);
     JA(4,11)= vfit(9);
     JA(5,4)= vfit(5);
     JA(5,5)= vfit(4);
     JA(5,10)= vfit(11);
     JA(5,11)= vfit(10);
     JA(6,5)= 1.;
     JA(6,11)= 1.;
     JA(7,3)= 1./e1*(vfit(3)*vfit(5)*vfit(5));
     JA(7,4)= 1./e1*(vfit(4)*vfit(5)*vfit(5));
     JA(7,5)= 1./e1*(vfit(3)*vfit(3)+vfit(4)*vfit(4)+1.)*vfit(5);
     JA(7,6)= 1./e1* vfit(6);
     JA(7,9)= 1./e2*(vfit(9)*vfit(11)*vfit(11));
     JA(7,10)= 1./e2*(vfit(10)*vfit(11)*vfit(11));
     JA(7,11)= 1./e2*(vfit(9)*vfit(9)+vfit(10)*vfit(10)+1.)*vfit(11);
     JA(7,12)= 1./e2* vfit(12);

     Ce=cfit.similarity(JA);

     log << MSG::VERBOSE <<"Ce= "<< Ce<<endreq;     

     zfit=zestimate - (vfit(7)-vfit(1))/(vfit(9)-vfit(3));
     log << MSG::VERBOSE <<"zestimate= "<< zestimate<<endreq;
     log << MSG::VERBOSE <<"zfit= "<< zfit<<endreq;

  }


  return StatusCode::SUCCESS;
}

//==================================================================
//  merge vertices of two resonance Particles
//==================================================================
                                                                                
StatusCode KFFitTool::mergeTwoVertices(Particle& part1, Particle& part2,
                            HepVector& Ve, HepSymMatrix& Ce) {
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "mergeTwoVertices!!! " << endmsg;

  HepVector para1(7,0);
  HepSymMatrix cov1(7,0);
  StatusCode sc=getEParameter(part1, para1, cov1);
  if(sc.isFailure ()) return sc;
  log << MSG::VERBOSE <<"para1= "<< para1<<endreq;


  HepVector para2(7,0);
  HepSymMatrix cov2(7,0);
  sc=getEParameter(part2, para2, cov2);
  if(sc.isFailure ()) return sc;
  log << MSG::VERBOSE <<"para2= "<< para2<<endreq;

  HepVector X(14);
  for(int i=1;i<=7;i++) {
    X(i)=para1(i);
    X(i+7)=para2(i);
  }
  log << MSG::VERBOSE <<"X= "<< X<<endreq;


  HepSymMatrix Cx(14,0);
  Cx*=0.;
  Cx.sub(1,cov1);
  Cx.sub(8,cov2);
  log << MSG::VERBOSE <<"Cx= "<< Cx<<endreq;

  HepMatrix D(3,14,0);
  D(1,1)=1.;
  D(1,8)=-1.;
  D(2,2)=1.;
  D(2,9)=-1.;
  D(3,3)=1.;
  D(3,10)=-1.;

  int ier=0;
  HepSymMatrix VD=Cx.similarity(D);
  log << MSG::VERBOSE << "VD determinant before inversion  "
      << VD.determinant() <<endreq;      
  VD.invert(ier);
  if(ier!=0) {
    log << MSG::ERROR << "could not invert matrix VD in KFFitTool::mergeTwoVertices"
        <<endreq;
    return StatusCode::FAILURE;
  }

  log << MSG::VERBOSE <<"VD= "<< VD<<endreq;

  HepVector lamda=VD*(D*X);
  HepVector  vfit=X-Cx*D.T()*lamda;
  HepSymMatrix delataC1=VD.similarityT(D);
  HepSymMatrix delataC2=delataC1.similarity(Cx);
  HepSymMatrix cfit=Cx -delataC2; 

  log << MSG::VERBOSE <<"vfit= "<< vfit<<endreq;
  

  Ve(1)=vfit(1);
  Ve(2)=vfit(2);
  Ve(3)=vfit(3);
  Ve(4)=vfit(4)+vfit(11);
  Ve(5)=vfit(5)+vfit(12);
  Ve(6)=vfit(6)+vfit(13);
  Ve(7)=vfit(7)+vfit(14);

  log << MSG::VERBOSE <<"Ve= "<< Ve<<endreq;

  HepMatrix JA(7,14,0);
  JA(1,1)=1.;
  JA(2,2)=1.;
  JA(3,3)=1.;
  JA(4,4)=1.;
  JA(4,11)=1.;
  JA(5,5)=1.;
  JA(5,12)=1.;
  JA(6,6)=1.;
  JA(6,13)=1.;
  JA(7,7)=1.;
  JA(7,14)=1.;

  Ce=cfit.similarity(JA);

  log << MSG::VERBOSE <<"Ce= "<< Ce<<endreq;

  return StatusCode::SUCCESS;
}

//==================================================================
//  add a non-photon Particle to a decay vertex
//==================================================================
StatusCode KFFitTool::addParticle(Particle& part1, Particle& part2,
                       HepVector& Ve, HepSymMatrix& Ce) {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG <<"Now addParticle "
      << part2.particleID().pid()<< endreq;

  HepVector para1(7,0);
  HepSymMatrix cov1(7,0);
  StatusCode sc=getMParameter(part1, para1, cov1);
  if(sc.isFailure ()) {
    log << MSG::ERROR <<"fail to getMParameter!"<<endreq;
    return sc;
  }
  double z1=para1(3);

  log << MSG::VERBOSE <<"para1 : " <<para1<<endreq;
  log << MSG::VERBOSE <<"cov1 : " <<cov1<<endreq;

  HepVector para2(7,0);
  HepSymMatrix cov2(7,0);

  sc=getMParameter(part2, para2, cov2);
  if(sc.isFailure ()) {
    log << MSG::ERROR <<"fail to getMParameter!"<<endreq;
    return sc;
  }
  log << MSG::VERBOSE <<"para2 : " <<para2<<endreq;
  log << MSG::VERBOSE <<"cov2 : " <<cov2<<endreq;


  log << MSG::VERBOSE <<"!!!! z2 : " <<para2(3)<<endreq;
  log << MSG::VERBOSE <<"!!!! transport to "<< z1 <<endreq;


  Particle transParticle;
  StatusCode sctrans = m_pTransporter->transport(part2, z1, transParticle);
  if( sctrans.isFailure ()) {
    log << MSG::ERROR << "transport failed in addParticle!" << endreq;
    return StatusCode::FAILURE;
  }
  sc=resetTrackParameters(transParticle);

  sc=getMParameter(transParticle, para2, cov2);
  if(sc.isFailure ()) {
    log << MSG::ERROR <<"fail to getMParameter!"<<endreq;
    return sc;
  }
  double z2=para2(3);

  log << MSG::VERBOSE <<"After transport para2 : " <<para2<<endreq;
  log << MSG::VERBOSE <<"cov2 : " <<cov2<<endreq;

  HepVector X(13);
  for(int i=1;i<=7;i++) {
    X(i)=para1(i);
  }
  X(8)=para2(1);
  X(9)=para2(2);
  X(10)=para2(4)/para2(6);
  X(11)=para2(5)/para2(6);
  X(12)=para2(6);
  X(13)=para2(7);

  HepMatrix mom2slope(6,7,0);
  mom2slope(1,1)=1.;
  mom2slope(2,2)=1.;
  mom2slope(3,4)=1./para2(6);
  mom2slope(3,6)=-para2(4)/para2(6)/para2(6);
  mom2slope(4,5)=1./para2(6);
  mom2slope(4,6)=-para2(5)/para2(6)/para2(6);
  mom2slope(5,6)=1.;
  mom2slope(6,7)=1.;


  HepSymMatrix newcov2=cov2.similarity(mom2slope);
  log << MSG::VERBOSE <<"newcov2 : " <<newcov2<<endreq;

  HepSymMatrix Cx(13,0);
  Cx*=0.;
  Cx.sub(1,cov1);
  Cx.sub(8,newcov2);

  log << MSG::VERBOSE << "X=  " << X << endreq;
  log << MSG::VERBOSE << "Cx=  " << Cx << endreq;

  HepVector  vfit=X;
  HepSymMatrix cfit=Cx;

  double m2=vfit(13);
  log << MSG::VERBOSE <<"initial m2= "<< m2<<endreq;
  
  HepMatrix D(2,13,0);
  D(1,1)=1.;
  D(1,3)=-vfit(10);
  D(1,8)=-1.;
  D(1,10)=(z2-vfit(3));

  D(2,2)=1.;
  D(2,3)=-vfit(11);
  D(2,9)=-1.;
  D(2,11)=(z2-vfit(3));

  HepVector f(2);
  f(1)=(vfit(1)-vfit(8))-vfit(10)*(vfit(3)-z2);
  f(2)=(vfit(2)-vfit(9))-vfit(11)*(vfit(3)-z2);
  HepVector d= f - D*vfit;

  log << MSG::VERBOSE << "initial constraint values   " << f << endreq;

  double chi2Previous=9999.;
  double chi2=999.;

  bool converged=false;
  int iter=0;

  while(!converged && iter< m_maxIter)  {
    iter++;
    log << MSG::VERBOSE << ":-) Iteration   " << iter << endreq;

    log << MSG::VERBOSE << "D=  " << D << endreq;

    int ier=0;
    HepSymMatrix VD=Cx.similarity(D);
    log << MSG::VERBOSE << "VD before inversion  " << VD << endreq;
    log << MSG::VERBOSE << "VD determinant before inversion  "
        << VD.determinant() <<endreq;
    VD.invert(ier);
    if(ier!=0) {
      log << MSG::ERROR << "could not invert matrix VD in KFFitTool::addParticle"
          <<endreq;
      return StatusCode::FAILURE;
    }

    log << MSG::VERBOSE << "VD after inversion  " << VD << endreq;

    HepVector alpha=D*X+d;
    HepVector lamda=VD*alpha;
    vfit=X-Cx*D.T()*lamda;
    HepSymMatrix delataC1=VD.similarityT(D);
    HepSymMatrix delataC2=delataC1.similarity(Cx);
    cfit=Cx -delataC2;  
 
    HepSymMatrix Q=VD.similarityT(alpha);

    D(1,1)=1.;
    D(1,3)=-vfit(10);
    D(1,8)=-1.;
    D(1,10)=(z2-vfit(3));

    D(2,2)=1.;
    D(2,3)=-vfit(11);
    D(2,9)=-1.;
    D(2,11)=(z2-vfit(3));

    f(1)=(vfit(1)-vfit(8))-vfit(10)*(vfit(3)-z2);
    f(2)=(vfit(2)-vfit(9))-vfit(11)*(vfit(3)-z2);
    d= f - D*vfit;

    log << MSG::VERBOSE << "constraint values   " << f << endreq;

    HepVector S=lamda.T()*f;    
    chi2=Q(1,1)+2.*S(1);

    log << MSG::VERBOSE << "New chi2=  " << chi2 << endreq;
    log << MSG::VERBOSE << "Previous chi2=  " << chi2Previous << endreq;

    log << MSG::VERBOSE << "vfit= "<<vfit << endreq;

    if(fabs(chi2-chi2Previous)<m_maxDeltaChiSq) {
      converged=true;      
    } else {
      chi2Previous=chi2;
    } 
  }

  log << MSG::VERBOSE <<"vfit= "<< vfit<<endreq;
  m2=vfit(13);;
  log << MSG::VERBOSE <<"m2= "<< m2<<endreq;

  Ve(1)=vfit(1);
  Ve(2)=vfit(2);
  Ve(3)=vfit(3);
  Ve(4)=vfit(4)+vfit(10)*vfit(12);
  Ve(5)=vfit(5)+vfit(11)*vfit(12);
  Ve(6)=vfit(6)+vfit(12);
  double e1=sqrt(vfit(7)*vfit(7) + vfit(4)*vfit(4) + vfit(5)*vfit(5) + vfit(6)*vfit(6)  );
  double e2=sqrt(vfit(13)*vfit(13) + (vfit(10)*vfit(10)+vfit(11)*vfit(11)+1.)*vfit(12)*vfit(12) );
  Ve(7)=e1+e2;
                                                                
  log << MSG::VERBOSE <<"Ve= "<< Ve<<endreq;

  HepMatrix JA(7,13,0);
  JA(1,1)=1.;
  JA(2,2)=1.;
  JA(3,3)=1.;
  JA(4,4)=1.;
  JA(4,10)=vfit(12);
  JA(4,12)=vfit(10);
  JA(5,5)=1.;
  JA(5,11)=vfit(12);
  JA(5,12)=vfit(11);
  JA(6,6)=1.;
  JA(6,12)=1.;
  JA(7,4)=1./e1*vfit(4);
  JA(7,5)=1./e1*vfit(5);
  JA(7,6)=1./e1*vfit(6);
  JA(7,7)=1./e1*vfit(7);
  JA(7,10)=1./e2*vfit(10)*vfit(12)*vfit(12) ;
  JA(7,11)=1./e2*vfit(11)*vfit(12)*vfit(12) ;
  JA(7,12)=1./e2*(vfit(10)*vfit(10)+vfit(11)*vfit(11)+1.)*vfit(12) ;
  JA(7,13)=1./e2*vfit(13);

                                                                                
  Ce=cfit.similarity(JA);
 
  return StatusCode::SUCCESS;
}

//==================================================================
//  add a photon to a decay vertex
//==================================================================
                                                                                
StatusCode KFFitTool::addPhoton(Particle& part1, Particle& photon,
                     HepVector& Ve, HepSymMatrix& Ce) {
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG <<"Now entering addPhoton!"<<endreq;

  int photonPid=photon.particleID().pid();
  if(photonPid!=22) {
    log << MSG::ERROR <<"Particle is not a photon!"<<endreq;
    return StatusCode::FAILURE;
  }

  HepVector para1(7,0);
  HepSymMatrix cov1(7,0);

  StatusCode sc=getEParameter(part1, para1, cov1);
  if(sc.isFailure ()) {
    log << MSG::ERROR <<"fail to getEParameter!"<<endreq;
    return sc;
  }
  log << MSG::VERBOSE <<"para1= "<< para1<<endreq;


  HepVector para2(3,0);
  HepSymMatrix cov2(3,0);
  double zg=-9999.;
  sc=getPhotonParameter(photon, zg, para2, cov2);
  if(sc.isFailure ()) {
    log << MSG::ERROR <<"fail to getPhotonParameter!"<<endreq;
    return sc;
  }

  double dx=para2(1)-para1(1);
  double dy=para2(2)-para1(2);
  double dz=zg-para1(3);
  double r=sqrt(dx*dx+dy*dy+dz*dz);
  double eg=para2(3);
  double pxg=eg*dx/r;
  double pyg=eg*dy/r;
  double pzg=eg*dz/r;

  Ve(1)=para1(1);
  Ve(2)=para1(2);
  Ve(3)=para1(3);
  Ve(4)=para1(4)+ pxg;
  Ve(5)=para1(5)+ pyg;
  Ve(6)=para1(6)+ pzg;
  Ve(7)=para1(7)+ eg;

  HepSymMatrix Cold(10,0);
  Cold.sub(1,cov1);
  Cold.sub(8,cov2);

  HepMatrix JA(7,10,0); 
  for(int i=1;i<=7;i++) JA(i,i)=1.;

  JA(4,1)=eg*(dx*dx/r/r/r-1./r);
  JA(4,2)=eg*(dx*dy/r/r/r);
  JA(4,3)=eg*(dx*dz/r/r/r);
  JA(4,8)=eg*(-dx*dx/r/r/r+1./r);
  JA(4,9)=eg*(-dx*dy/r/r/r);

  JA(5,1)=eg*(dy*dx/r/r/r);
  JA(5,2)=eg*(dy*dy/r/r/r-1./r);
  JA(5,3)=eg*(dy*dz/r/r/r);
  JA(5,8)=eg*(-dy*dx/r/r/r);
  JA(5,9)=eg*(-dy*dy/r/r/r+1./r);

  JA(6,1)=eg*(dz*dx/r/r/r);
  JA(6,2)=eg*(dz*dy/r/r/r);
  JA(6,3)=eg*(dz*dz/r/r/r-1./r);
  JA(6,8)=eg*(-dz*dx/r/r/r);
  JA(6,9)=eg*(-dz*dy/r/r/r);

  JA(4,10)=dx/r;
  JA(5,10)=dy/r;
  JA(6,10)=dz/r;
  JA(7,10)=1.;

  Ce=Cold.similarity(JA);
  log << MSG::VERBOSE <<"Ce after addPhoton"<< Ce<<endreq;
  log << MSG::VERBOSE <<"Ve after addPhoton"<< Ve<<endreq;
 
  return StatusCode::SUCCESS;
}

//==================================================================
//  add a photon pair to a decay vertex
//==================================================================
                                                                                
StatusCode KFFitTool::addPhotonPair(Particle& part1, Particle& pair,
                     HepVector& Ve, HepSymMatrix& Ce) {
  MsgStream log(msgSvc(), name());

  int pairPid=pair.particleID().pid();
  log << MSG::DEBUG <<"Now addPhotonPair for !"<<pairPid <<endreq;
  if(pairPid!=111 && pairPid!=221) {
    log << MSG::ERROR <<"Particle is not a pi0 or eta!"<<endreq;
    return StatusCode::FAILURE;
  }
                                                           
  HepVector para1(7,0);
  HepSymMatrix cov1(7,0);

  StatusCode sc=getEParameter(part1, para1, cov1);
  if(sc.isFailure ()) {
    log << MSG::ERROR <<"fail to getEParameter!"<<endreq;
    return sc;
  }
  log << MSG::VERBOSE <<"para1= "<< para1<<endreq;

  //get "pair" daughters
  Vertex* endVertex = pair.endVertex();
  if(!endVertex) {
    log << MSG::ERROR <<"pair has no decay vertex!" <<endreq;
    return StatusCode::FAILURE;
  }

  SmartRefVector< Particle > & Prods = endVertex->products();
  SmartRefVector< Particle >::iterator iProd;
  if(Prods.size()!=2) {
    log << MSG::ERROR <<"number of daughters is not 2 but "<<Prods.size()  <<endreq;
    return StatusCode::FAILURE;
  } 

  HepVector paragm1(3,0);
  HepSymMatrix covgm1(3,0);
  double zg1=-9999.;
  HepVector paragm2(3,0);
  HepSymMatrix covgm2(3,0);
  double zg2=-9999.;

  int ig=0;
  for (iProd=Prods.begin(); iProd !=Prods.end(); ++iProd){
    Particle* daughter = *iProd;
    int daupid = daughter->particleID().pid();
    if(daupid!=22) {
      log << MSG::ERROR <<"PhotonPair has non-photon daughter "<< daupid <<endreq;
      return StatusCode::FAILURE;
    }
    ig++;

    HepVector paragm(3,0);
    HepSymMatrix covgm(3,0);
    double zg=-9999.;
    sc=getPhotonParameter(*daughter, zg, paragm, covgm);
    if(sc.isFailure ()) {
      log << MSG::ERROR <<"fail to getPhotonParameter!"<<endreq;
      return sc;
    }
    if(ig==1) { 
      paragm1=paragm;
      covgm1=covgm;
      zg1=zg;
    } else {
      paragm2=paragm;
      covgm2=covgm;
      zg2=zg;
    }
  }

  double dx1=paragm1(1)-para1(1);
  double dy1=paragm1(2)-para1(2);
  double dz1=zg1-para1(3);
  double r1=sqrt(dx1*dx1+dy1*dy1+dz1*dz1);
  double eg1=paragm1(3);
  double pxg1=eg1*dx1/r1;
  double pyg1=eg1*dy1/r1;
  double pzg1=eg1*dz1/r1;

  double dx2=paragm2(1)-para1(1);
  double dy2=paragm2(2)-para1(2);
  double dz2=zg2-para1(3);
  double r2=sqrt(dx2*dx2+dy2*dy2+dz2*dz2);
  double eg2=paragm2(3);
  double pxg2=eg2*dx2/r2;
  double pyg2=eg2*dy2/r2;
  double pzg2=eg2*dz2/r2;

  HepVector paraOld(13,0);
  HepSymMatrix covOld(13,0);
  
  for(int i=1; i<=7; i++) paraOld(i)=para1(i);
  for(int i=1; i<=3; i++) paraOld(7+i)=paragm1(i);
  for(int i=1; i<=3; i++) paraOld(10+i)=paragm2(i);
 
  covOld.sub(1, cov1);  
  covOld.sub(8, covgm1);
  covOld.sub(11, covgm2);

  HepVector paraNew(8,0);
  HepSymMatrix covNew(8,0);

  paraNew(1)=paraOld(1);
  paraNew(2)=paraOld(2);
  paraNew(3)=paraOld(3);
  paraNew(4)=paraOld(4)+pxg1+pxg2;
  paraNew(5)=paraOld(5)+pyg1+pyg2;
  paraNew(6)=paraOld(6)+pzg1+pzg2;
  paraNew(7)=paraOld(7)+eg1+eg2;
  double mpi0=sqrt((eg1+eg2)*(eg1+eg2)-(pxg1+pxg2)*(pxg1+pxg2)-(pyg1+pyg2)*(pyg1+pyg2)-(pzg1+pzg2)*(pzg1+pzg2));
  paraNew(8)=mpi0;
  log << MSG::VERBOSE <<"mass of PhotonPair "<<mpi0 <<endreq;

  HepMatrix JA(8,13,0);
  for(int i=1;i<=7;i++) JA(i,i)=1.;

  double dinvr1_dx1=-1./(r1*r1)*(-dx1/r1);
  double dinvr1_dy1=-1./(r1*r1)*(-dy1/r1);
  double dinvr1_dz1=-1./(r1*r1)*(-dz1/r1);
  double dinvr1_dxg1=-1./(r1*r1)*(dx1/r1);
  double dinvr1_dyg1=-1./(r1*r1)*(dy1/r1);

  double dinvr2_dx1=-1./(r2*r2)*(-dx2/r2);
  double dinvr2_dy1=-1./(r2*r2)*(-dy2/r2);
  double dinvr2_dz1=-1./(r2*r2)*(-dz2/r2);
  double dinvr2_dxg2=-1./(r2*r2)*(dx2/r2);
  double dinvr2_dyg2=-1./(r2*r2)*(dy2/r2);

  double dpxg1_dx1=eg1*(-1./r1+dx1*dinvr1_dx1);
  double dpyg1_dx1=eg1*dy1*dinvr1_dx1;
  double dpzg1_dx1=eg1*dz1*dinvr1_dx1;
  double dpxg2_dx1=eg2*(-1./r2+dx2*dinvr2_dx1);
  double dpyg2_dx1=eg2*dy2*dinvr2_dx1;
  double dpzg2_dx1=eg2*dz2*dinvr2_dx1;

  double dpxg1_dy1=eg1*dx1*dinvr1_dy1;
  double dpyg1_dy1=eg1*(-1./r1+dy1*dinvr1_dy1);
  double dpzg1_dy1=eg1*dz1*dinvr1_dy1;
  double dpxg2_dy1=eg2*dx2*dinvr2_dy1;
  double dpyg2_dy1=eg2*(-1./r2+dy2*dinvr2_dy1);
  double dpzg2_dy1=eg2*dz2*dinvr2_dy1;

  double dpxg1_dz1=eg1*dx1*dinvr1_dz1;
  double dpyg1_dz1=eg1*dy1*dinvr1_dz1;
  double dpzg1_dz1=eg1*(-1./r1+dz1*dinvr1_dz1);
  double dpxg2_dz1=eg2*dx2*dinvr2_dz1;
  double dpyg2_dz1=eg2*dy2*dinvr2_dz1;
  double dpzg2_dz1=eg2*(-1./r2+dz2*dinvr2_dz1);

  double dpxg1_dxg1=eg1*(1./r1+dx1*dinvr1_dxg1);
  double dpyg1_dxg1=eg1*dy1*dinvr1_dxg1;
  double dpzg1_dxg1=eg1*dz1*dinvr1_dxg1;

  double dpxg1_dyg1=eg1*dx1*dinvr1_dyg1;
  double dpyg1_dyg1=eg1*(1./r1+dy1*dinvr1_dyg1);
  double dpzg1_dyg1=eg1*dz1*dinvr1_dyg1;

  double dpxg2_dxg2=eg2*(1./r2+dx2*dinvr2_dxg2);
  double dpyg2_dxg2=eg2*dy2*dinvr2_dxg2;
  double dpzg2_dxg2=eg2*dz2*dinvr2_dxg2;
                                                                                                                                                
  double dpxg2_dyg2=eg2*dx2*dinvr2_dyg2;
  double dpyg2_dyg2=eg2*(1./r2+dy2*dinvr2_dyg2);
  double dpzg2_dyg2=eg2*dz2*dinvr2_dyg2;

  JA(4,1)=dpxg1_dx1+dpxg2_dx1;
  JA(4,2)=dpxg1_dy1+dpxg2_dy1;
  JA(4,3)=dpxg1_dz1+dpxg2_dz1;
  JA(4,8)=dpxg1_dxg1;
  JA(4,9)=dpxg1_dyg1;
  JA(4,10)=dx1/r1;
  JA(4,11)=dpxg2_dxg2;
  JA(4,12)=dpxg2_dyg2;
  JA(4,13)=dx2/r2;

  JA(5,1)=dpyg1_dx1+dpyg2_dx1;
  JA(5,2)=dpyg1_dy1+dpyg2_dy1;
  JA(5,3)=dpyg1_dz1+dpyg2_dz1;
  JA(5,8)=dpyg1_dxg1;
  JA(5,9)=dpyg1_dyg1;
  JA(5,10)=dy1/r1;
  JA(5,11)=dpyg2_dxg2;
  JA(5,12)=dpyg2_dyg2;
  JA(5,13)=dy2/r2;

  JA(6,1)=dpzg1_dx1+dpzg2_dx1;
  JA(6,2)=dpzg1_dy1+dpzg2_dy1;
  JA(6,3)=dpzg1_dz1+dpzg2_dz1;
  JA(6,8)=dpzg1_dxg1;
  JA(6,9)=dpzg1_dyg1;
  JA(6,10)=dz1/r1;
  JA(6,11)=dpzg2_dxg2;
  JA(6,12)=dpzg2_dyg2;
  JA(6,13)=dz2/r2;

  JA(7,10)=1.;
  JA(7,13)=1.;

  JA(8,1)=-(pxg1+pxg2)/mpi0*dpxg1_dx1 -(pyg1+pyg2)/mpi0*dpyg1_dx1 -(pzg1+pzg2)/mpi0*dpzg1_dx1
          -(pxg1+pxg2)/mpi0*dpxg2_dx1 -(pyg1+pyg2)/mpi0*dpyg2_dx1 -(pzg1+pzg2)/mpi0*dpzg2_dx1;
  JA(8,2)=-(pxg1+pxg2)/mpi0*dpxg1_dy1 -(pyg1+pyg2)/mpi0*dpyg1_dy1 -(pzg1+pzg2)/mpi0*dpzg1_dy1
          -(pxg1+pxg2)/mpi0*dpxg2_dy1 -(pyg1+pyg2)/mpi0*dpyg2_dy1 -(pzg1+pzg2)/mpi0*dpzg2_dy1;
  JA(8,3)=-(pxg1+pxg2)/mpi0*dpxg1_dz1 -(pyg1+pyg2)/mpi0*dpyg1_dz1 -(pzg1+pzg2)/mpi0*dpzg1_dz1
          -(pxg1+pxg2)/mpi0*dpxg2_dz1 -(pyg1+pyg2)/mpi0*dpyg2_dz1 -(pzg1+pzg2)/mpi0*dpzg2_dz1;

  JA(8,8)=-(pxg1+pxg2)/mpi0*dpxg1_dxg1 -(pyg1+pyg2)/mpi0*dpyg1_dxg1 -(pzg1+pzg2)/mpi0*dpzg1_dxg1;
  JA(8,9)=-(pxg1+pxg2)/mpi0*dpxg1_dyg1 -(pyg1+pyg2)/mpi0*dpyg1_dyg1 -(pzg1+pzg2)/mpi0*dpzg1_dyg1;
  JA(8,10)=(eg1+eg2)/mpi0-(pxg1+pxg2)/mpi0*dx1/r1-(pyg1+pyg2)/mpi0*dy1/r1-(pzg1+pzg2)/mpi0*dz1/r1;

  JA(8,11)=-(pxg1+pxg2)/mpi0*dpxg2_dxg2 -(pyg1+pyg2)/mpi0*dpyg2_dxg2 -(pzg1+pzg2)/mpi0*dpzg2_dxg2;
  JA(8,12)=-(pxg1+pxg2)/mpi0*dpxg2_dyg2 -(pyg1+pyg2)/mpi0*dpyg2_dyg2 -(pzg1+pzg2)/mpi0*dpzg2_dyg2;
  JA(8,13)=(eg1+eg2)/mpi0-(pxg1+pxg2)/mpi0*dx2/r2-(pyg1+pyg2)/mpi0*dy2/r2-(pzg1+pzg2)/mpi0*dz2/r2;

  log << MSG::VERBOSE << "JA   "<< JA<< endmsg;

  covNew=covOld.similarity(JA);

  log << MSG::VERBOSE << "covNew   "<< covNew << endmsg;
  log << MSG::VERBOSE << "paraNew   "<< paraNew << endmsg;

  double pairWid = ParticleWidth(pair);

  if(m_PhotonPairMassConstraint && pairWid<m_widthCut) {
    ParticleID pid=pair.particleID();
    int stdHepID = pid.pid();
    ParticleProperty*  partProp = m_ppSvc->findByStdHepID(stdHepID  );
    double nominalMass=(*partProp).mass();

    HepMatrix DD(1,8,0);
    HepVector dd(1,0);
                                                                                                                                     
    DD(1,8)=1.;
    dd[0]=-nominalMass;
                                                                                                                                     
    HepSymMatrix Cd=covNew.similarity(DD);
    int er=0;
    Cd.invert(er);
    if (er!=0) {
      log << MSG::ERROR << "could not invert matrix Cd in KFFitTool::addPhotonPair" << endmsg;
      return StatusCode::FAILURE;
    }
    paraNew-= covNew*DD.T()*Cd*(DD*paraNew+dd);
    HepSymMatrix delatC1=Cd.similarityT(DD);
    HepSymMatrix delatC2=delatC1.similarity(covNew);
                                                                                                                                     
    covNew-= delatC2;
    log << MSG::VERBOSE << "after PhotonPair mass constraint!!!"<<endreq; 
    log << MSG::VERBOSE << "covNew   "<< covNew << endmsg;
    log << MSG::VERBOSE << "paraNew   "<< paraNew << endmsg;

    for(int i=1;i<=8;i++) covNew(8,i)=0.0;
  }

  log << MSG::VERBOSE <<"final mass of PhotonPair "<< paraNew(8) <<endreq;

  for(int i=1;i<=7;i++) Ve(i)=paraNew(i);
  Ce=covNew.sub(1,7);
                     
  return StatusCode::SUCCESS;

}

//==================================================================
//  convert energy parameter vector to mass parameter vector
//==================================================================

void KFFitTool::Ve2Vm(HepVector& Ve,HepVector& Vm) {
  for(int i=0;i<=5;i++) Vm[i]=Ve[i];    
  Vm[6]=sqrt(Ve[6]*Ve[6]-Ve[3]*Ve[3]-Ve[4]*Ve[4]-Ve[5]*Ve[5]);
}

//==================================================================
//  convert energy parameter vector to mass parameter vector
//==================================================================
void KFFitTool::Vm2Ve(HepVector& Vm,HepVector& Ve) {
  for(int i=0;i<=5;i++) Ve[i]=Vm[i];
  Ve[6]=sqrt(Vm[6]*Vm[6]+Vm[3]*Vm[3]+Vm[4]*Vm[4]+Vm[5]*Vm[5]);
}


//==================================================================
//  transformation matrix from energy parameters to mass parameters
//==================================================================
HepMatrix KFFitTool::MatrixE2M(Particle& part) {

  HepLorentzVector momentum = part.momentum();
  double px=momentum.vect().x();
  double py=momentum.vect().y();
  double pz=momentum.vect().z();
  double energy=momentum.e();
  double mass=momentum.mag();

  HepMatrix Te2m(7,7,1);
  Te2m(7,4)=-px/mass;
  Te2m(7,5)=-py/mass;
  Te2m(7,6)=-pz/mass;
  Te2m(7,7)=energy/mass;
  return Te2m;
}

//==================================================================
//  transformation matrix from mass parameters to energy parameters
//==================================================================
HepMatrix KFFitTool::MatrixM2E(Particle& part) {

  HepLorentzVector momentum = part.momentum();
  double px=momentum.vect().x();
  double py=momentum.vect().y();
  double pz=momentum.vect().z();  
  double energy=momentum.e();
  double mass=momentum.mag();

  HepMatrix Tm2e(7,7,1);
  Tm2e(7,4)=px/energy;
  Tm2e(7,5)=py/energy;
  Tm2e(7,6)=pz/energy;
  Tm2e(7,7)=mass/energy;
  return Tm2e;
}

//==================================================================
//  get particle width
//==================================================================
double KFFitTool::ParticleWidth(Particle& part)
{
  int pid = part.particleID().pid();
  ParticleProperty*  partProp = m_ppSvc->findByStdHepID(pid  );
  double hbar = 6.58211889*pow(10,-22);
  double partWidthHBar = hbar/(pow(10,-9)*((*partProp).lifetime()));
  return partWidthHBar;
}

//==================================================================
//  method to determine if a particle is composite
//==================================================================
bool KFFitTool::isComposite(Particle& part) {
  Vertex* endVertex = part.endVertex();
  if(!endVertex) return false;
  if(endVertex->products().size()<=0) return false;     
  return true;
}

//==================================================================
//  method to determine if a particle has non-photon final decay
//  products or not
//==================================================================
bool KFFitTool::isPurePhotonDecay(Particle& part) {
  Vertex* endVertex = part.endVertex();
  if(!endVertex) return false;
  SmartRefVector< Particle > & Prods = endVertex->products();
  SmartRefVector< Particle >::iterator iProd;
  for (iProd=Prods.begin(); iProd !=Prods.end(); ++iProd){
     Particle* daughter = *iProd;
     if( daughter->particleID().pid()!=22 &&
         !isPurePhotonDecay(*daughter) ) return false;
  }

  return true;
}


//==================================================================
// Method to get photon parameters: z, x,y,E  cov(x,y,E)
//==================================================================
StatusCode KFFitTool::getPhotonParameter(Particle& photon, double& z,
                                HepVector& var, HepSymMatrix& cov) 
{
  // access to local utilities
  using namespace PhotonParametersLocal ;
  using namespace CaloHypotheses        ;

  MsgStream log(msgSvc(), name());
  int pid=photon.particleID().pid();
  if(pid!=22) {
    log << MSG::ERROR <<"Particle is not a photon!"<<endreq;
    return StatusCode::FAILURE;
  }

  const ContainedObject* origin = photon.origin();
  if( 0 == origin) {
    log << MSG::ERROR <<"Photon origin points to NULL!"<<endreq;
    return StatusCode::FAILURE;
  }

  const ProtoParticle*   proto  = dynamic_cast<const ProtoParticle*>(origin) ;
  if( 0 == proto  ) {
    log << MSG::ERROR <<"ProtoParticle points to NULL!"<<endreq;
    return StatusCode::FAILURE;
  }

  if( proto->calo().empty() ) {           
    log << MSG::ERROR <<"ProtoParticle has no CaloHypos "<<endreq;
    return StatusCode::FAILURE;
  }

  typedef const SmartRefVector<CaloHypo> Hypos;
  const Hypos& hypos = proto->calo();
  Hypos::const_iterator ihypo =
    std::find_if( hypos.begin () , hypos.end () , IsHypo( Photon ) ) ;
  if( hypos.end() == ihypo )  { 
    log << MSG::ERROR <<" CaloHypothesis 'Photon' is not found "; 
    return StatusCode::FAILURE;
  }
  const CaloHypo* hypo = *ihypo ;

  // get the position
  const CaloPosition* pos = hypo->position() ;
  if( 0 == pos    ) { 
    log << MSG::ERROR <<"CaloPosition* points to NULL! "<<endreq;
    return StatusCode::FAILURE;
  }

  typedef CaloPosition CP;
  const CP::Covariance& caloCov = pos -> covariance();

  z=pos->z();
  var(1)=pos->x();
  var(2)=pos->y();
  var(3)=pos->e();
  cov=pos -> covariance();
  log << MSG::VERBOSE <<"Photon parameters: " <<var<<endreq;
  log << MSG::VERBOSE <<"Photon cov : " <<cov<<endreq;


  return StatusCode::SUCCESS;
}

//==================================================================
// Method to get photon parameters: z, x,y,E  cov(x,y,E)
//==================================================================
StatusCode KFFitTool::resetTrackParameters(Particle& part)
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG <<"Now resetTrackParameters "
      << part.particleID().pid()<< endreq;

  HepVector para(7,0);
  HepSymMatrix cov(7,0);
  StatusCode sc=getMParameter(part, para, cov);
  log << MSG::VERBOSE << "para before reset "<< para << endmsg;
  log << MSG::VERBOSE << "cov before reset "<< cov << endmsg;

  ParticleID pid=part.particleID();
  int stdHepID = pid.pid();
  ParticleProperty*  partProp = m_ppSvc->findByStdHepID(stdHepID  );
  double nominalMass=(*partProp).mass();
  log << MSG::VERBOSE << "set to nominal pass "<< nominalMass << endmsg;

  para(7)=nominalMass;
  for(int i=1;i<=7;i++) cov(7,i)=0.;
  log << MSG::VERBOSE << "new cov " << cov << endreq;
  
  sc=setMParameter(part, para, cov);

  log << MSG::VERBOSE << "para after reset "<< para << endmsg;
  log << MSG::VERBOSE << "cov after reset "<< cov << endmsg;

  return StatusCode::SUCCESS;
}

//==================================================================

double KFFitTool::zFirstMeasurement(Particle* part)
{
  double zmin=999999.;
  ContainedObject* obj = part->origin();
  ProtoParticle* pporig = 0;
  if( obj ) {
    pporig = dynamic_cast<ProtoParticle*>(obj);
    if( pporig ) {
      TrStoredTrack* thetrack = pporig->track();
      if(thetrack) {
                                                                                                                                                          
        SmartRefVector<TrStoredMeasurement>& StoredMeasurements=
                                           thetrack->measurements();
        SmartRefVector<TrStoredMeasurement>::iterator itm;
        for(itm=StoredMeasurements.begin();
                itm!=StoredMeasurements.end(); itm++ ) {
          double z=(*itm)->z();
          if(z<zmin) zmin=z;
        }
      }
    }
  }
  return zmin;
}

//==================================================================
                                                                                                                                                          

