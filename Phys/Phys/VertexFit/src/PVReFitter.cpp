// Include files from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"

// Include files from Event
#include "Event/Particle.h"
#include "Event/Vertex.h"
#include "Event/ProtoParticle.h"
#include "Event/PrimVertex.h"

//Kernel

// local file
#include "PVReFitter.h"

//--------------------------------------------------------------------
//
//  ClassName  : PVReFitter
// 
//  Description: remove (B) tracks from a primary vertex and refit
//                           
//  @Author     : Yuehong Xie
//
//  @Date 17/08/2008
//
//--------------------------------------------------------------------

//==================================================================
// Instantiation of a static factory class used by clients to create
// instances of this service
//==================================================================
static ToolFactory<PVReFitter> s_factory;
const IToolFactory& PVReFitterFactory = s_factory;

//==================================================================
// Standard Constructor
//==================================================================
PVReFitter::PVReFitter(const std::string& type, 
                     const std::string& name, 
                     const IInterface* parent) 
  : GaudiTool( type, name, parent )
{

  declareInterface<IPVReFitter>(this);
  declareProperty( "MaxIter", m_maxIter = 10);
  declareProperty( "MaxDeltaChiSq", m_maxDeltaChiSq = 0.001);
  declareProperty( "FullExtrapolator", 
                   m_fullextrapolatorName = "TrFirstCleverExtrapolator"  );
  declareProperty( "LinearExtrapolator",
                   m_linearextrapolatorName = "TrLinearExtrapolator"  );
  
}

//==================================================================
// Initialize
//==================================================================
StatusCode PVReFitter::initialize() {

  StatusCode sc = GaudiTool::initialize();
  if (!sc) return sc;

  m_pFullExtrap = tool<ITrExtrapolator>( m_fullextrapolatorName,this );
  if (!m_pFullExtrap){
    fatal() <<"Unable to retrieve FullExtrapolator Tool "<<endreq;
    return StatusCode::FAILURE;
  }

  m_pLinearExtrap = tool<ITrExtrapolator>( m_linearextrapolatorName,this );
  if (!m_pLinearExtrap){
    fatal() <<"Unable to retrieve LinearExtrapolator Tool "<<endreq;
    return StatusCode::FAILURE;
  }


  return StatusCode::SUCCESS;  
}

//==================================================================
//  refit a PV
//==================================================================

StatusCode PVReFitter::reFit( Vertex* pv ) {

  debug() <<"Now reFit PV"<< endreq;

  PrimVertex* primvtx=dynamic_cast<PrimVertex*>(pv);
  if(!primvtx) return StatusCode::FAILURE;

  std::vector<TrStoredTrack*> tracks;
  SmartRefVector<TrStoredTrack> pvtracks = primvtx->tracks();
  for( SmartRefVector<TrStoredTrack>::iterator trIt = pvtracks.begin();
       pvtracks.end() != trIt; trIt++) {
    TrStoredTrack* track = *trIt;
    tracks.push_back(track);
  } 

  StatusCode sc=fitPV( pv , tracks);
  if(!sc.isSuccess()) {
    debug() << "fitPV fails" <<endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//==================================================================
//  remove tracks and refit
//==================================================================

StatusCode PVReFitter::remove(Particle* part, Vertex* pv ) {

  debug() <<"Now remove and reFit "<< endreq;

  PrimVertex* primvtx=dynamic_cast<PrimVertex*>(pv);
  if(!primvtx) return StatusCode::FAILURE;

  std::vector<TrStoredTrack*> dautracks;
  getFinalTracks(part, dautracks);

  std::vector<TrStoredTrack*> tracks;
  SmartRefVector<TrStoredTrack> pvtracks = primvtx->tracks();
  for( SmartRefVector<TrStoredTrack>::iterator trIt = pvtracks.begin();
       pvtracks.end() != trIt; trIt++) {
    TrStoredTrack* track = *trIt;
    bool drop=false;
    for( std::vector<TrStoredTrack*>::iterator idtrack = dautracks.begin();
         dautracks.end() != idtrack; idtrack++ ) {
      if(track==*idtrack) { drop=true; break; }
    }
    if(!drop) tracks.push_back(track);
  }

  StatusCode sc=fitPV( pv , tracks);
  if(!sc.isSuccess()) {
    debug() << "fitPV fails" <<endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//==================================================================


StatusCode PVReFitter::fitPV(Vertex* PV, 
                             std::vector<TrStoredTrack*>& tracks )
{
  StatusCode sc;
  
  debug() <<"Now entering fitPV!"<<endreq;

  if(tracks.size()<2) {
   debug() << "number of track left for the PV "<< tracks.size() <<endreq;
   return StatusCode::FAILURE;
  }
  HepVector Vpv(3,0);
  HepSymMatrix Cpv(3,0);

  double totalChi2=0;
  double chi2=0;
  TrStoredTrack* firtDau=NULL; 
  int iDau=0; 
  for( std::vector<TrStoredTrack*>::iterator itrack = tracks.begin();
       tracks.end() != itrack; itrack++ ) {
    iDau++;   
    TrStoredTrack* dau=*itrack;
    if(iDau==1) {
      firtDau=dau;      
    } else if(iDau==2) {
      debug() << "about to fitTwoTracks "<<endreq;
      sc=fitTwoTracks(firtDau, dau, Vpv, Cpv, chi2);
      if(sc.isSuccess()) debug() << "fitTwoTracks successfull! "<<endreq;
      else debug() << "fitTwoTracks fails "<<endreq;
      if(sc.isFailure ()) return sc;
      totalChi2+=chi2;
    } else {
      sc=addTr(dau, Vpv, Cpv, chi2);
      if(sc.isSuccess()) debug() << "addTr successfull! "<<endreq;
      else debug() << "addTr fails "<<endreq;
      if(sc.isFailure ()) return sc;
      totalChi2+=chi2;
    }
  }

  sc=setPara(PV, Vpv, Cpv, totalChi2);
  if(sc.isFailure ()) {
    debug() << "setPara fails! "<<endreq;
    return sc;
  }

  return StatusCode::SUCCESS;
}


StatusCode  PVReFitter::fitTwoTracks( TrStoredTrack* tr1, TrStoredTrack* tr2,
                                       HepVector& par, HepSymMatrix& cov, double & chi2)
{
  StatusCode sc=StatusCode::SUCCESS;

  TrStateP stateP1;
  sc= getInitialStateP(tr1, stateP1);
  if(!sc.isSuccess()) return StatusCode::FAILURE;

  TrStateP stateP2;
  sc= getInitialStateP(tr2, stateP2);
  if(!sc.isSuccess()) return StatusCode::FAILURE;

  double zbyXY=getZEstimate(stateP1,stateP2);

  double zfit=zbyXY;
  double zestimate=-999999.;

  {
     zestimate=zfit;

     TrStateP workStateP1=stateP1;
     sc=m_pFullExtrap->propagate(&workStateP1, zestimate);
     if(!sc.isSuccess()) return StatusCode::FAILURE;
 
     TrStateP workStateP2=stateP2;
     sc=m_pFullExtrap->propagate(&workStateP2, zestimate);
     if(!sc.isSuccess()) return StatusCode::FAILURE;

     HepVector X(8);
     X(1)=workStateP1.x();
     X(2)=workStateP1.y();
     X(3)=workStateP1.tx();
     X(4)=workStateP1.ty();
     X(5)=workStateP2.x();
     X(6)=workStateP2.y();
     X(7)=workStateP2.tx();
     X(8)=workStateP2.ty();
                                                                                                                                   
     HepSymMatrix Cx(8,0);
     Cx*=0.;
     Cx.sub(1,workStateP1.stateCov().sub(1,4));
     Cx.sub(5,workStateP2.stateCov().sub(1,4));

     verbose() << "X=  " << X << endreq;
     //verbose() << "Cx=  " << Cx << endreq;
                                                                                                                                   
     HepVector  vfit=X;
     HepSymMatrix cfit=Cx;

     HepMatrix D(1,8,0);
     D(1,1)=-(vfit(8)-vfit(4));
     D(1,2)=vfit(7)-vfit(3);
     D(1,3)=vfit(6)-vfit(2);
     D(1,4)=-(vfit(5)-vfit(1));
     D(1,5)=vfit(8)-vfit(4);
     D(1,6)=-(vfit(7)-vfit(3));
     D(1,7)=-(vfit(6)-vfit(2));
     D(1,8)=vfit(5)-vfit(1);

     HepVector f(1);
     f(1)=(vfit(5)-vfit(1))*(vfit(8)-vfit(4))-(vfit(6)-vfit(2))*(vfit(7)-vfit(3));
     HepVector d= f - D*vfit;
                                                                                                                                   
     verbose() << "initial constraint values   " << f << endreq;
                                                                                                                                   
     double chi2Previous=9999.;
     chi2=999999.;
                                                                                                                                   
     bool converged=false;
     int iter=0;
                                                                                                                                   
     while(!converged && iter< m_maxIter)  {
       iter++;
       verbose() << ":-) Iteration   " << iter << endreq;
       //verbose() << "D=  " << D << endreq;
                                                                                                                                   
       int ier=0;
       HepSymMatrix VD=Cx.similarity(D);
       //verbose() << "VD before inversion  " << VD << endreq;
       verbose() << "VD determinant before inversion  "
           << VD.determinant() <<endreq;
       VD.invert(ier);
       if(ier!=0) {
         err() << "could not invert matrix VD in fitTwoTracks"
             <<endreq;
         return StatusCode::FAILURE;
       }

       //verbose() << "VD after inversion  " << VD << endreq;
                                                                                                                                   
       HepVector alpha=D*X+d;
       HepVector lamda=VD*alpha;
       vfit=X-Cx*D.T()*lamda;
       HepSymMatrix delataC1=VD.similarityT(D);
       HepSymMatrix delataC2=delataC1.similarity(Cx);
       cfit=Cx -delataC2;

       HepSymMatrix Q=VD.similarityT(alpha);
       D(1,1)=-(vfit(8)-vfit(4));
       D(1,2)=vfit(7)-vfit(3);
       D(1,3)=vfit(6)-vfit(2);
       D(1,4)=-(vfit(5)-vfit(1));
       D(1,5)=vfit(8)-vfit(4);
       D(1,6)=-(vfit(7)-vfit(3));
       D(1,7)=-(vfit(6)-vfit(2));
       D(1,8)=vfit(5)-vfit(1);
       f(1)=(vfit(5)-vfit(1))*(vfit(8)-vfit(4))-(vfit(6)-vfit(2))*(vfit(7)-vfit(3));
       d= f - D*vfit;
       verbose() << "constraint values   " << f << endreq;
       HepVector S=lamda.T()*f;
       chi2=Q(1,1)+2.*S(1);
       //chi2=Q(1,1);
                                                                                                                                   
       verbose() << "New fitTwoTracks chi2=  " << chi2 << endreq;
       verbose() << "Previous fitTwoTracks chi2=  " << chi2Previous << endreq;
                                                                                                                                   
       verbose() << "vfit= "<<vfit << endreq;
       verbose() << "z vertex "<< zestimate - (vfit(5)-vfit(1))/(vfit(7)-vfit(3)) << endreq;
                                                                                                                                   
       if(fabs(chi2-chi2Previous)<m_maxDeltaChiSq) {
         converged=true;
       } else {
         chi2Previous=chi2;
       }
     }
                                                                                                                                   
     if(!converged)  {
       debug()<<"fitTwoTracks not converged"<<endreq;
       return StatusCode::FAILURE;
     }
 
     verbose() <<"final vfit= "<< vfit<<endreq;

     par(1)=vfit(1) - vfit(3)*(vfit(5)-vfit(1))/(vfit(7)-vfit(3));
     par(2)=vfit(2) - vfit(4)*(vfit(5)-vfit(1))/(vfit(7)-vfit(3));
     par(3)=zestimate - (vfit(5)-vfit(1))/(vfit(7)-vfit(3));

     verbose() <<"par= "<< par<<endreq;
                                                                                                                                   
     HepMatrix JA(3,8,0);
     JA(1,1)=1. + vfit(3)/(vfit(7)-vfit(3));
     JA(1,3)= - (vfit(5)-vfit(1))/(vfit(7)-vfit(3))
              - vfit(3)*(vfit(5)-vfit(1))/(vfit(7)-vfit(3))/(vfit(7)-vfit(3));
     JA(1,5)= - vfit(3)/(vfit(7)-vfit(3));
     JA(1,7)=  vfit(3)*(vfit(5)-vfit(1))/(vfit(7)-vfit(3))/(vfit(7)-vfit(3));
     JA(2,1)=  vfit(4)/(vfit(7)-vfit(3));
     JA(2,2)=1. ;
     JA(2,3)= - vfit(4)*(vfit(5)-vfit(1))/(vfit(7)-vfit(3))/(vfit(7)-vfit(3));
     JA(2,4)= - (vfit(5)-vfit(1))/(vfit(7)-vfit(3));
     JA(2,5)= - vfit(4)/(vfit(7)-vfit(3));
     JA(2,7)=   vfit(4)*(vfit(5)-vfit(1))/(vfit(7)-vfit(3))/(vfit(7)-vfit(3));
     JA(3,1)= 1./(vfit(7)-vfit(3));
     JA(3,3)= - (vfit(5)-vfit(1))/(vfit(7)-vfit(3))/(vfit(7)-vfit(3));
     JA(3,5)= - 1./(vfit(7)-vfit(3));
     JA(3,7)= (vfit(5)-vfit(1))/(vfit(7)-vfit(3))/(vfit(7)-vfit(3));
                                                                                                                                   
     cov=cfit.similarity(JA);

     //verbose() <<"Ce= "<< Ce<<endreq;
                                                                                                                                   
     zfit=zestimate - (vfit(5)-vfit(1))/(vfit(7)-vfit(3));
     verbose() <<"zestimate= "<< zestimate<<endreq;
     verbose() <<"zfit= "<< zfit<<endreq;
                                                                                                                                   
  }

  debug() <<" fitTwoTracks successful " <<endreq;

  return StatusCode::SUCCESS;
}

StatusCode   PVReFitter::addTr( TrStoredTrack* tr,
                                 HepVector& par, HepSymMatrix& cov, double& chi2)
{
  StatusCode sc=StatusCode::SUCCESS;                                                                                                                     
  TrStateP stateP;
  sc= getInitialStateP(tr, stateP);
  if(!sc.isSuccess()) {
    debug()<<"getInitialStateP fails in addTr"<<endreq;
    return StatusCode::FAILURE;
  }
  double zestimate=par(3);
  TrStateP workStateP=stateP;
  sc=m_pFullExtrap->propagate(&workStateP, zestimate);
  if(!sc.isSuccess()) {
    debug()<<"extrapolation fails in addTr"<<endreq;
    return StatusCode::FAILURE;
  }

  HepVector X(7);
  X(1)=par(1);
  X(2)=par(2);
  X(3)=par(3);
  X(4)=workStateP.x();
  X(5)=workStateP.y();
  X(6)=workStateP.tx();
  X(7)=workStateP.ty();

  HepSymMatrix Cx(7,0);
  Cx*=0.;
  Cx.sub(1,cov);
  Cx.sub(4,workStateP.stateCov().sub(1,4));

  HepVector  vfit=X;
  HepSymMatrix cfit=Cx;

  double z2=zestimate;                                                                                
  HepMatrix D(2,7,0);
  D(1,1)=1.;
  D(1,3)=-vfit(6);
  D(1,4)=-1.;
  D(1,6)=(z2-vfit(3));
                                                                                
  D(2,2)=1.;
  D(2,3)=-vfit(7);
  D(2,5)=-1.;
  D(2,7)=(z2-vfit(3));

  HepVector f(2);
  f(1)=(vfit(1)-vfit(4))-vfit(6)*(vfit(3)-z2);
  f(2)=(vfit(2)-vfit(5))-vfit(7)*(vfit(3)-z2);
  HepVector d= f - D*vfit;

  verbose() << "initial constraint values   " << f << endreq;
                                                                                
  double chi2Previous=9999.;
  chi2=999999.;

  bool converged=false;
  int iter=0;
                                                                                
  while(!converged && iter< m_maxIter)  {
    iter++;
    verbose() << ":-) Iteration   " << iter << endreq;
                                                                                
    //verbose() << "D=  " << D << endreq;
                                                                                
    int ier=0;
    HepSymMatrix VD=Cx.similarity(D);
    //verbose() << "VD before inversion  " << VD << endreq;
    verbose() << "VD determinant before inversion  "
        << VD.determinant() <<endreq;
    VD.invert(ier);
    if(ier!=0) {
      err() << "could not invert matrix VD in KFFitTool::addTr"
          <<endreq;
      return StatusCode::FAILURE;
    }

    //verbose() << "VD after inversion  " << VD << endreq;
                                                                                
    HepVector alpha=D*X+d;
    HepVector lamda=VD*alpha;
    vfit=X-Cx*D.T()*lamda;
    HepSymMatrix delataC1=VD.similarityT(D);
    HepSymMatrix delataC2=delataC1.similarity(Cx);
    cfit=Cx -delataC2;

    HepSymMatrix Q=VD.similarityT(alpha);

    D(1,1)=1.;
    D(1,3)=-vfit(6);
    D(1,4)=-1.;
    D(1,6)=(z2-vfit(3));
                                                                                                                     
    D(2,2)=1.;
    D(2,3)=-vfit(7);
    D(2,5)=-1.;
    D(2,7)=(z2-vfit(3));

    f(1)=(vfit(1)-vfit(4))-vfit(6)*(vfit(3)-z2);
    f(2)=(vfit(2)-vfit(5))-vfit(7)*(vfit(3)-z2);
                                                                                
    d= f - D*vfit;
                                                                                
    verbose() << "constraint values   " << f << endreq;
                                                                                
    HepVector S=lamda.T()*f;
    chi2=Q(1,1)+2.*S(1);
    verbose() << "New chi2=  " << chi2 << endreq;
    verbose() << "Previous chi2=  " << chi2Previous << endreq;
                                                                                
    verbose() << "vfit= "<<vfit << endreq;
                                                                                
    if(fabs(chi2-chi2Previous)<m_maxDeltaChiSq) {
      converged=true;
    } else {
      chi2Previous=chi2;
    }
  }

  if(!converged)  {
    debug()<<"addTr not converged"<<endreq;
    return StatusCode::FAILURE;
  }

  par(1)=vfit(1);
  par(2)=vfit(2);
  par(3)=vfit(3);

  cov=cfit.sub(1,3);
                                                                                
  return StatusCode::SUCCESS;
}

StatusCode  PVReFitter::setPara(Vertex* PV, 
                                HepVector& par, 
                                HepSymMatrix& cov,
                                double & chi2 )
{
  HepPoint3D position;
  position[0]=par(1);
  position[1]=par(2);
  position[2]=par(3);
  PV->setPosition(position);
  PV->setPositionErr(cov);
  PV->setChi2(chi2);

  return StatusCode::SUCCESS;
}

double PVReFitter::getZEstimate(TrStateP& state1, TrStateP& state2)
{
  double tx1=state1.tx();
  double ty1=state1.ty();
  double tx2=state2.tx();
  double ty2=state2.ty();

  double x1= state1.x();
  double y1= state1.y();
  double z1= state1.z();
  double x2= state2.x();
  double y2= state2.y();
  double z2= state2.z();

  double  sumSquaredSlopes = tx1*tx1 + ty1*ty1+ tx2*tx2 + ty2*ty2;
  double  sumCrossedProduct = tx1*(x1-tx1*z1) + ty1*(y1-ty1*z1) +
                              tx2*(x2-tx2*z2) + ty2*(y2-ty2*z2);

  double sumX=x1-tx1*z1 + x2-tx2*z2;
  double sumY=y1-ty1*z1 + y2-ty2*z2;

  double sumSlopeX= tx1+tx2;
  double sumSlopeY= ty1+ty2;

  double det = sumSquaredSlopes - ((sumSlopeX*sumSlopeX + sumSlopeY*sumSlopeY))/2.;

  double zEstimate = 0;
  if (det != 0) {
    return zEstimate = (((sumX*sumSlopeX + sumY*sumSlopeY)/2.)
                        - sumCrossedProduct) /det;
  }
  else {
    err() << "Unable to make z estimate " << endreq;
    if(z1<z2) return z1-.001;
    else return z2-0.001;
  }
}

StatusCode PVReFitter::getInitialStateP(TrStoredTrack* tr, TrStateP & stateP)
{

  double veloFirstZ=1000.;
  TrState* state=NULL;

  if (tr->veloBack()) {
    // Get most downstream state and VELO point
    TrStoredMeasurement* firstMeas = *(tr->measurements().rbegin());
    veloFirstZ = firstMeas->z();
  } else {
    TrStoredMeasurement* firstMeas = *(tr->measurements().begin());
    veloFirstZ = firstMeas->z();
  }


  state = const_cast<TrState*>(tr->closestState(veloFirstZ));  

  TrStateP* pStateP = dynamic_cast<TrStateP*>(state);
  if(0!=pStateP) {
    TrStateP workStateP(*pStateP);
    stateP= workStateP;
  } else { 
    TrStateL* stateL = dynamic_cast<TrStateL*>(state);
    if(0!=stateL) {
       TrStateL workStateL(*stateL);
       StatusCode sc=m_pLinearExtrap->propagate(&workStateL, veloFirstZ);
       if(!sc.isSuccess()) return StatusCode::FAILURE;
       stateP.setZ( workStateL.z() );
       HepVector stateV(5);
       stateV.sub(1, workStateL.stateVector() ) ;
       double p = 400.0 *  MeV *
                  sqrt( 1. + 1./(pow(workStateL.tx(),2) + pow(workStateL.ty(),2)));
       double q=1;
       if(workStateL.tx()<0) q=-1;
       stateV[4] = q*1. / p;
       stateP.setStateVector( stateV );
       HepSymMatrix cov(5,0);
       cov.sub(1, workStateL.stateCov() );
       cov[4][4] = pow( 0.2 /(p*p), 2) ;
       stateP.setStateCov( cov );       
    } else {
      return StatusCode::FAILURE;
    }
  }
  
  return StatusCode::SUCCESS;
}


void PVReFitter::getFinalTracks(Particle* part, std::vector<TrStoredTrack*> & tracks) 
{
  Vertex* endVertex = part->endVertex();
  if(!endVertex || endVertex->products().size()==0 ) {
    ContainedObject* obj = part->origin();
    ProtoParticle* pporig = 0;
    if(obj) {
      pporig = dynamic_cast<ProtoParticle*>(obj);
      if( pporig ) {
        TrStoredTrack* thetrack = pporig->track();
        if(thetrack) tracks.push_back(thetrack);
      }
    } 
  } else {
    SmartRefVector< Particle > & Prods = endVertex->products();
    SmartRefVector< Particle >::iterator iProd;
    for (iProd=Prods.begin(); iProd !=Prods.end(); ++iProd){
      Particle* daughter = *iProd;
      getFinalTracks(daughter, tracks);       
    }
  }

}

