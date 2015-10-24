// $Id: CheatedPVReFitter.cpp,v 1.7 2009-10-09 10:15:53 xieyu Exp $
// Include files

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "Event/VertexBase.h"
#include "Event/RecVertex.h"

#include "Linker/LinkedFrom.h"
#include "Linker/LinkedTo.h"

#include "TrackInterfaces/ITrackExtrapolator.h"

#include "Event/MCHeader.h"
#include "Event/ProtoParticle.h"
#include "Event/Particle.h"

// local
#include "CheatedPVReFitter.h"

using namespace LHCb ;
using namespace Gaudi::Units;
//-----------------------------------------------------------------------------
// Implementation file for class : CheatedPVReFitter
//
// 2006-06-08 : Yuehong Xie
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CheatedPVReFitter::CheatedPVReFitter( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IPVReFitter>(this);

  declareProperty( "maxIter", m_maxIter = 10);
  declareProperty( "maxDeltaChi2", m_maxDeltaChi2 = 0.001);
  declareProperty( "maxDeltaZ", m_maxDeltaZ = 1.0 *  mm  );
  declareProperty( "fullExtrapolatorName",
                   m_fullExtrapolatorName = "TrackMasterExtrapolator");
  declareProperty( "veloExtrapolatorName",
                   m_veloExtrapolatorName = "TrackLinearExtrapolator");

}

//=============================================================================
// Destructor
//=============================================================================
CheatedPVReFitter::~CheatedPVReFitter() {}

//=============================================================================
// Initialize
//=============================================================================
StatusCode CheatedPVReFitter::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if (!sc) return sc;

  m_fullExtrapolator = tool<ITrackExtrapolator>( m_fullExtrapolatorName );
  m_veloExtrapolator = tool<ITrackExtrapolator>( m_veloExtrapolatorName );

  return sc;
}

//=============================================================================
// refit PV
//=============================================================================
StatusCode CheatedPVReFitter::reFit(LHCb::VertexBase* PV) const {

  debug() <<"Now reFit PV"<< endmsg;

  StatusCode sc = StatusCode::SUCCESS;

  LinkedTo<LHCb::MCParticle,LHCb::Track> directLink( evtSvc(), msgSvc(), TrackLocation::Default );

  if(!(PV->isPrimary())) {
    debug() <<"CheatedPVReFitter is used to reFit a  non-PV"<< endmsg;
    return StatusCode::FAILURE;
  }

  RecVertex* primvtx=dynamic_cast<RecVertex*>(PV);
  if(!primvtx) return StatusCode::FAILURE;

  std::vector<LHCb::Track*> tracks;

  SmartRefVector<LHCb::Track> pvtracks = primvtx->tracks();
  for( SmartRefVector<LHCb::Track>::iterator trIt = pvtracks.begin();
       pvtracks.end() != trIt; trIt++) {
    LHCb::Track* track = *trIt;
    bool fromPV = true;
    const LHCb::MCVertex* mcPV = PV2MCVertex(primvtx);
    if(mcPV) {
      for( LHCb::MCParticle* mcPart = directLink.first(track);
           NULL != mcPart;
           mcPart = directLink.next() ) {
        const LHCb::MCVertex* originv2=mcPart->originVertex();
        if(!originv2) continue;
        Gaudi::XYZVector tmpL = originv2->position() - mcPV->position();
        double disv = tmpL.R();
        if( fabs(disv)>0.000000001*mm ) fromPV = false;
      }
    }
    if(fromPV) tracks.push_back(track);
  }

  sc=fitPV( primvtx , tracks);
  if(!sc.isSuccess()) {
    debug() << "fitPV fails" <<endmsg;
    return StatusCode::FAILURE;
  }

  return sc;
}

//=============================================================================
// remove track used for a LHCb::Particle and refit PV
//=============================================================================
StatusCode CheatedPVReFitter::remove(const LHCb::Particle* /*part*/,
                                     LHCb::VertexBase* /*PV*/) const {

  warning() <<"remove method not implemented! "<< endmsg;

  return StatusCode::SUCCESS;

}


//=============================================================================
// fit PV from a vector of tracks
//=============================================================================
StatusCode CheatedPVReFitter::fitPV(LHCb::RecVertex* PV,
                                    std::vector<LHCb::Track*> & tracks) const {
  StatusCode sc = StatusCode::SUCCESS;

  debug() <<"Now entering fitPV!"<<endmsg;

  PV->clearTracks();

  if(tracks.size()<2) {
    debug() << "number of track left for the PV "<< tracks.size() <<endmsg;
    return StatusCode::FAILURE;
  }

  LHCb::Track* tr1 =  tracks.back();
  tracks.pop_back();
  LHCb::Track* tr2 =  tracks.back();
  tracks.pop_back();

  if(!tr1 || !tr2 ) {
    debug() << "Null track pointer found ! " << endmsg;
    return StatusCode::FAILURE;
  }

  sc = seedPV(PV, tr1, tr2);
  if(sc.isFailure()) {
    debug() << "Fail to seedPV" << endmsg;
    return StatusCode::FAILURE;
  }

  for( std::vector<LHCb::Track*>::iterator itrack = tracks.begin(); tracks.end() != itrack; itrack++ )
  {
    LHCb::Track* tr = *itrack;
    sc = addTr(PV, tr);
    if(sc.isFailure()) {
      debug() << "Fail to addTr" << endmsg;
      return StatusCode::FAILURE;
    }
  }

  return sc;
}


//=============================================================================
// get final tracks of a particle
//=============================================================================
void CheatedPVReFitter::getFinalTracks(LHCb::Particle* part,
                                       LHCb::Track::ConstVector& tracks) const
{
  const LHCb::ProtoParticle*   proto  = part->proto() ;

  if( proto ) {
    const LHCb::Track* thetrack = proto->track();
    if(thetrack) tracks.push_back(thetrack);
  } else {
    const SmartRefVector< LHCb::Particle > & Prods = part->daughters();
    SmartRefVector< LHCb::Particle >::const_iterator iProd;
    for (iProd=Prods.begin(); iProd !=Prods.end(); ++iProd){
      const LHCb::Particle* daughter = *iProd;
      getFinalTracks(const_cast<LHCb::Particle*>(daughter), tracks);
    }
  }
}

//=============================================================================
// seeding a PV with two tracks
//=============================================================================

StatusCode CheatedPVReFitter::seedPV(LHCb::RecVertex* PV,
                                     LHCb::Track* tr1,
                                     LHCb::Track* tr2) const
{
  StatusCode sc = StatusCode::SUCCESS;

  bool isVelo1 = tr1->checkType( Track::Velo );
  bool isVelo2 = tr2->checkType( Track::Velo );

  bool isVeloB1 = tr1->checkType( Track::VeloR );
  bool isVeloB2 = tr2->checkType( Track::VeloR );

  double chi2Fit=999.;

  Gaudi::Vector3 V3;
  Gaudi::SymMatrix3x3 Cov3;

  LHCb::State statetr1 =  tr1->firstState();
  LHCb::State statetr2 =  tr2->firstState();

  double tx1=statetr1.tx();
  double ty1=statetr1.ty();
  double x1=statetr1.x();
  double y1=statetr1.y();
  double z1=statetr1.z();

  double tx2=statetr2.tx();
  double ty2=statetr2.ty();
  double x2=statetr2.x();
  double y2=statetr2.y();
  double z2=statetr2.z();

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
    zEstimate = (((sumX*sumSlopeX + sumY*sumSlopeY)/2.)
                 - sumCrossedProduct) /det;
  }  else {
    err() << "Unable to make z estimate " << endmsg;
    if(z1<z2) zEstimate = z1-.001;
    else zEstimate = z2-0.001;
  }

  double zfit = zEstimate;
  double zPreviousFit=-999999.;

  int iterTransport=0;

  while(fabs(zPreviousFit-zfit)>m_maxDeltaZ && iterTransport< m_maxIter)
  {
    zPreviousFit=zfit;
    iterTransport++;

    LHCb::State newstate1 =  statetr1;
    if(isVelo1 || isVeloB1) sc = m_veloExtrapolator->propagate( newstate1, zPreviousFit );
    else  sc = m_fullExtrapolator->propagate( newstate1, zPreviousFit );
    if( sc.isFailure ()) {
      debug() << "failed to propagate tarck state in seedPV!" << endmsg;
      return sc;
    }

    LHCb::State newstate2 =  statetr2;
    if(isVelo2 || isVeloB2) sc = m_veloExtrapolator->propagate( newstate2, zPreviousFit );
    else  sc = m_fullExtrapolator->propagate( newstate2, zPreviousFit );
    if( sc.isFailure ()) {
      debug() << "failed to propagate tarck state in seedPV!" << endmsg;
      return sc;
    }

    ROOT::Math::SVector<double, 10> X;
    X[0] = newstate1.x();
    X[1] = newstate1.y();
    X[2] = newstate1.momentum().x();
    X[3] = newstate1.momentum().y();
    X[4] = newstate1.momentum().z();
    X[5] = newstate2.x();
    X[6] = newstate2.y();
    X[7] = newstate2.momentum().x();
    X[8] = newstate2.momentum().y();
    X[9] = newstate2.momentum().z();

    Gaudi::SymMatrix6x6 covtr1 = newstate1.posMomCovariance();
    Gaudi::SymMatrix6x6 covtr2 = newstate2.posMomCovariance();
    Gaudi::SymMatrix5x5 newcovtr1;
    Gaudi::SymMatrix5x5 newcovtr2;

    for(int l1=0; l1<=4; l1++) {
      int n1=l1;
      if(n1>=2) n1++;
      for(int l2=0; l2<=l1; l2++) {
        int n2=l2;
        if(n2>=2) n2++;
        newcovtr1(l1,l2) =  covtr1(n1,n2);
        newcovtr2(l1,l2) =  covtr2(n1,n2);
      }
    }

    SymMatrix10x10 Cx;
    for(int l1=0;l1<5;l1++) {
      for(int l2=0;l2<=l1;l2++) {
        Cx(l1,l2)=newcovtr1(l1,l2);
        Cx(l1+5,l2+5)=newcovtr2(l1,l2);
      }
      for(int l2=0;l2<5;l2++)  Cx(l1+5,l2)=0.0;
    }

    //(x1,y1,px1,py1,pz1,x2,y2,px2,py2,pz2) at the same z
    ROOT::Math::SVector<double, 10> vfit=X;
    SymMatrix10x10 cfit = Cx;

    bool converged=false;
    int iter=0;

    double chi2Previous=9999.;

    while(!converged && iter< m_maxIter)  {
      iter++;
      verbose() << ":-) Iteration   " << iter << endmsg;

      //f=(x2-x1)*(py2*pz1-py1*pz2)-(y2-y1)*(px2*pz1-px1*pz2)
      ROOT::Math::SVector<double, 1> f;
      f(0)=(vfit(5)-vfit(0))*(vfit(8)*vfit(4)-vfit(3)*vfit(9))-
        (vfit(6)-vfit(1))*(vfit(7)*vfit(4)-vfit(2)*vfit(9));
      verbose() << "constraint values   " << f << endmsg;
      //D is the derivative matrix
      ROOT::Math::SMatrix<double, 1, 10> D;
      D(0,0)=-(vfit(8)*vfit(4)-vfit(3)*vfit(9));
      D(0,1)=(vfit(7)*vfit(4)-vfit(2)*vfit(9));
      D(0,2)=(vfit(6)-vfit(1))*vfit(9);
      D(0,3)=-(vfit(5)-vfit(0))*vfit(9);
      D(0,4)=(vfit(5)-vfit(0))*vfit(8)-(vfit(6)-vfit(1))*vfit(7);
      D(0,5)=(vfit(8)*vfit(4)-vfit(3)*vfit(9));
      D(0,6)=-(vfit(7)*vfit(4)-vfit(2)*vfit(9));
      D(0,7)=-(vfit(6)-vfit(1))*vfit(4);
      D(0,8)=(vfit(5)-vfit(0))*vfit(4);
      D(0,9)=-(vfit(5)-vfit(0))*vfit(3)+(vfit(6)-vfit(1))*vfit(2);

      ROOT::Math::SVector<double, 1> d = f - D*vfit;

      Gaudi::SymMatrix1x1 VD=ROOT::Math::Similarity<double,1,10>(D, Cx);

      if(!VD.Invert()) {
        debug() << "could not invert matrix VD in seedPV! " <<endmsg;
        return StatusCode::FAILURE;
      }

      ROOT::Math::SVector<double, 1> alpha=D*X+d;

      ROOT::Math::SVector<double, 1> lambda=VD*alpha;

      ROOT::Math::SMatrix<double, 10,1> DT = ROOT::Math::Transpose(D);
      vfit=X-Cx*DT*lambda;

      SymMatrix10x10 delataC1=ROOT::Math::Similarity<double,10,1>(DT, VD);

      SymMatrix10x10 delataC2=ROOT::Math::Similarity<double,10,10>(Cx, delataC1);

      cfit=Cx -delataC2;

      chi2Fit=ROOT::Math::Dot(alpha,lambda);
      //chi2Fit+= 2*ROOT::Math::Dot(lambda,f);

      if(fabs(chi2Fit-chi2Previous)<m_maxDeltaChi2) {
        converged=true;
      } else {
        chi2Previous=chi2Fit;
      }
    } // end chi2 minimization iteration

    if(!converged)  return StatusCode::FAILURE;

    V3[0] = vfit(0)-vfit(2)*(vfit(5)-vfit(0))*vfit(9)/(vfit(7)*vfit(4)-vfit(2)*vfit(9));
    V3[1] = vfit(1)-vfit(3)*(vfit(6)-vfit(1))*vfit(9)/(vfit(8)*vfit(4)-vfit(3)*vfit(9));
    V3[2] = zPreviousFit-(vfit(5)-vfit(0))*vfit(4)*vfit(9)/(vfit(7)*vfit(4)-vfit(2)*vfit(9));

    ROOT::Math::SMatrix<double, 3, 10> JA;
    JA(0,0)=1.+vfit(2)*vfit(9)/(vfit(7)*vfit(4)-vfit(2)*vfit(9));
    JA(0,2)=-(vfit(5)-vfit(0))*vfit(9)/(vfit(7)*vfit(4)-vfit(2)*vfit(9))
      -vfit(2)*(vfit(5)-vfit(0))*vfit(9)/(vfit(7)*vfit(4)-vfit(2)*vfit(9))
      /(vfit(7)*vfit(4)-vfit(2)*vfit(9))*vfit(9);
    JA(0,4)=vfit(2)*(vfit(5)-vfit(0))*vfit(9)/(vfit(7)*vfit(4)-vfit(2)*vfit(9))
      /(vfit(7)*vfit(4)-vfit(2)*vfit(9))*vfit(7);
    JA(0,5)=-vfit(2)*vfit(9)/(vfit(7)*vfit(4)-vfit(2)*vfit(9));
    JA(0,7)=vfit(2)*(vfit(5)-vfit(0))*vfit(9)/(vfit(7)*vfit(4)-vfit(2)*vfit(9))
      /(vfit(7)*vfit(4)-vfit(2)*vfit(9))*vfit(4);
    JA(0,9)=-vfit(2)*(vfit(5)-vfit(0))/(vfit(7)*vfit(4)-vfit(2)*vfit(9))
      -vfit(2)*(vfit(5)-vfit(0))*vfit(9)/(vfit(7)*vfit(4)-vfit(2)*vfit(9))
      /(vfit(7)*vfit(4)-vfit(2)*vfit(9))*vfit(2);

    JA(1,1)=1.+vfit(3)*vfit(9)/(vfit(8)*vfit(4)-vfit(3)*vfit(9));
    JA(1,3)=-(vfit(6)-vfit(1))*vfit(9)/(vfit(8)*vfit(4)-vfit(3)*vfit(9))
      -vfit(3)*(vfit(6)-vfit(1))*vfit(9)/(vfit(8)*vfit(4)-vfit(3)*vfit(9))
      /(vfit(8)*vfit(4)-vfit(3)*vfit(9))*vfit(9);
    JA(1,4)=vfit(3)*(vfit(6)-vfit(1))*vfit(9)/(vfit(8)*vfit(4)-vfit(3)*vfit(9))
      /(vfit(8)*vfit(4)-vfit(3)*vfit(9))*vfit(8);
    JA(1,6)=-vfit(3)*vfit(9)/(vfit(8)*vfit(4)-vfit(3)*vfit(9));
    JA(1,8)=vfit(3)*(vfit(6)-vfit(1))*vfit(9)/(vfit(8)*vfit(4)-vfit(3)*vfit(9))
      /(vfit(8)*vfit(4)-vfit(3)*vfit(9))*vfit(4);
    JA(1,9)=-vfit(3)*(vfit(6)-vfit(1))/(vfit(8)*vfit(4)-vfit(3)*vfit(9))
      -vfit(3)*(vfit(6)-vfit(1))*vfit(9)/(vfit(8)*vfit(4)-vfit(3)*vfit(9))
      /(vfit(8)*vfit(4)-vfit(3)*vfit(9))*vfit(3);

    JA(2,0)=vfit(4)*vfit(9)/(vfit(7)*vfit(4)-vfit(2)*vfit(9));
    JA(2,2)=-(vfit(5)-vfit(0))*vfit(4)*vfit(9)/(vfit(7)*vfit(4)-vfit(2)*vfit(9))
      /(vfit(7)*vfit(4)-vfit(2)*vfit(9))*vfit(9);
    JA(2,4)=-(vfit(5)-vfit(0))*vfit(9)/(vfit(7)*vfit(4)-vfit(2)*vfit(9))
      +(vfit(5)-vfit(0))*vfit(4)*vfit(9)/(vfit(7)*vfit(4)-vfit(2)*vfit(9))
      /(vfit(7)*vfit(4)-vfit(2)*vfit(9))*vfit(7);
    JA(2,5)=-vfit(4)*vfit(9)/(vfit(7)*vfit(4)-vfit(2)*vfit(9));
    JA(2,7)=(vfit(5)-vfit(0))*vfit(4)*vfit(9)/(vfit(7)*vfit(4)-vfit(2)*vfit(9))
      /(vfit(7)*vfit(4)-vfit(2)*vfit(9))*vfit(4);
    JA(2,9)=-(vfit(5)-vfit(0))*vfit(4)/(vfit(7)*vfit(4)-vfit(2)*vfit(9))
      -(vfit(5)-vfit(0))*vfit(4)*vfit(9)/(vfit(7)*vfit(4)-vfit(2)*vfit(9))
      /(vfit(7)*vfit(4)-vfit(2)*vfit(9))*vfit(2);

    Cov3=ROOT::Math::Similarity<double,3,10>(JA, cfit);

    zfit = V3[2];

  }

  Gaudi::XYZPoint position(V3[0],V3[1],V3[2]);
  PV->setPosition(position);
  PV->setCovMatrix(Cov3);

  double chi2new = chi2Fit;
  PV->setChi2(chi2new);

  int nDoFnew = 1;
  PV->setNDoF(nDoFnew);

  PV->addToTracks(tr1);
  PV->addToTracks(tr2);

  return sc;
}

//=============================================================================
// add a track to a PV
//=============================================================================

StatusCode  CheatedPVReFitter::addTr(LHCb::RecVertex* PV,
                                     LHCb::Track* tr) const
{
  StatusCode sc= StatusCode::SUCCESS;

  bool isVelo  = tr->checkType( Track::Velo );
  bool isVeloB = tr->checkType( Track::VeloR );

  Gaudi::SymMatrix3x3 covPV = PV->covMatrix();
  Gaudi::XYZPoint PosPV = PV->position();

  double z2 = PosPV.z();

  LHCb::State statetr =  tr->firstState();
  LHCb::State newstate =  statetr;

  if(isVelo || isVeloB) sc = m_veloExtrapolator->propagate( newstate, z2 );
  else  sc = m_fullExtrapolator->propagate( newstate, z2 );
  if( sc.isFailure ()) {
    debug() << "failed to propagate tarck state in addTr!" << endmsg;
    return sc;
  }

  ROOT::Math::SVector<double, 8> X;
  X[0] = PosPV.x();
  X[1] = PosPV.y();
  X[2] = PosPV.z();
  X[3] = newstate.x();
  X[4] = newstate.y();
  X[5] = newstate.momentum().x();
  X[6] = newstate.momentum().y();
  X[7] = newstate.momentum().z();

  Gaudi::SymMatrix6x6 covtr = newstate.posMomCovariance();

  SymMatrix8x8 Cx;
  for(int l1=0;l1<3;l1++) {
    for(int l2=0;l2<=l1;l2++) {
      Cx(l1,l2)=covPV(l1,l2);
    }
  }
  for(int l1=3;l1<8;l1++) {
    int n1=l1-3;
    if(n1>=2) n1++;
    for(int l2=3;l2<=l1;l2++) {
      int n2=l2-3;
      if(n2>=2) n2++;
      Cx(l1,l2)=covtr(n1,n2);
    }
  }
  for(int l1=3;l1<8;l1++) {
    for(int l2=0;l2<3;l2++) {
      Cx(l1,l2)=0.0;
    }
  }

  // xpv,ypv,zpv, and xtr,ytr,pxtr,pytr,pztr at z2
  ROOT::Math::SVector<double, 8> vfit=X;
  SymMatrix8x8 cfit = Cx;

  double chi2PreviousFit=9999.;
  double chi2Fit=999.;

  bool converged=false;
  int iter=0;

  while(!converged && iter< m_maxIter)  {
    iter++;
    verbose() << ":-) Iteration   " << iter << endmsg;

    //f(0)=(xtr-xpv)*pztr-(z2-zpv)*pxtr
    //f(1)=(ytr-ypv)*pztr-(z2-zpv)*pytr

    ROOT::Math::SVector<double, 2> f;
    f(0)= (vfit[3]-vfit[0])*vfit[7]- (z2-vfit[2])*vfit[5];
    f(1)= (vfit[4]-vfit[1])*vfit[7]- (z2-vfit[2])*vfit[6];

    //D is the derivative matrix
    ROOT::Math::SMatrix<double, 2, 8> D;
    D(0,0) = - vfit[7];
    D(0,1) = 0.0;
    D(0,2) = vfit[5];
    D(0,3) = vfit[7];
    D(0,4) = 0.0;
    D(0,5) = - (z2-vfit[2]);
    D(0,6) = 0.0;
    D(0,7) = (vfit[3]-vfit[0]);

    D(1,0) = 0.0;
    D(1,1) = - vfit[7];
    D(1,2) = vfit[6];
    D(1,3) = 0.0;
    D(1,4) = vfit[7];
    D(1,5) = 0.0;
    D(1,6) = - (z2-vfit[2]);
    D(1,7) = (vfit[4]-vfit[1]);

    ROOT::Math::SVector<double, 2> d = f - D*vfit;

    Gaudi::SymMatrix2x2 VD=ROOT::Math::Similarity<double,2,8>(D, Cx);
    if(!VD.Invert()) {
      debug() << "could not invert matrix VD in addTr! " <<endmsg;
      return StatusCode::FAILURE;
    }

    ROOT::Math::SVector<double, 2> alpha=D*X+d;

    ROOT::Math::SVector<double, 2> lambda=VD*alpha;

    ROOT::Math::SMatrix<double, 8,2> DT = ROOT::Math::Transpose(D);

    vfit=X-Cx*DT*lambda;

    SymMatrix8x8 delataC1=ROOT::Math::Similarity<double,8,2>(DT, VD);

    SymMatrix8x8 delataC2=ROOT::Math::Similarity<double,8,8>(Cx, delataC1);

    cfit=Cx -delataC2;

    chi2Fit=ROOT::Math::Dot(alpha,lambda);
    //chi2Fit+= 2*ROOT::Math::Dot(lambda,f);

    if(fabs(chi2Fit-chi2PreviousFit)<m_maxDeltaChi2) {
      converged=true;
    } else {
      chi2PreviousFit=chi2Fit;
    }
  } // end chi2 minimization iteration

  if(!converged)  return StatusCode::FAILURE;

  Gaudi::XYZPoint position(vfit[0], vfit[1], vfit[2]);
  Gaudi::SymMatrix3x3 cov = cfit.Sub<Gaudi::SymMatrix3x3>(0,0);

  PV->setPosition(position);
  PV->setCovMatrix(cov);

  double chi2new = chi2Fit + PV->chi2();
  PV->setChi2(chi2new);

  int nDoFnew = 2 + PV->nDoF() ;
  PV->setNDoF(nDoFnew);

  PV->addToTracks(tr);

  return sc;
}


StatusCode CheatedPVReFitter::kalman_remove(LHCb::RecVertex* PV, LHCb::Track* tr) const
{
  StatusCode sc = StatusCode::SUCCESS;

  bool isVelo  = tr->checkType( Track::Velo );
  bool isVeloB = tr->checkType( Track::VeloR );

  Gaudi::SymMatrix3x3 covPV = PV->covMatrix();
  Gaudi::XYZPoint PosPV = PV->position();

  double z2 = PosPV.z();

  LHCb::State statetr =  tr->firstState();
  LHCb::State newstate =  statetr;

  if(isVelo || isVeloB) sc = m_veloExtrapolator->propagate( newstate, z2 );
  else  sc = m_fullExtrapolator->propagate( newstate, z2 );
  if( sc.isFailure ()) {
    debug() << "failed to propagate tarck state in addTr!" << endmsg;
    return sc;
  }

  ROOT::Math::SVector<double, 8> X;
  X[0] = PosPV.x();
  X[1] = PosPV.y();
  X[2] = PosPV.z();
  X[3] = newstate.x();
  X[4] = newstate.y();
  X[5] = newstate.momentum().x();
  X[6] = newstate.momentum().y();
  X[7] = newstate.momentum().z();

  Gaudi::SymMatrix6x6 covtr = newstate.posMomCovariance();

  SymMatrix8x8 Cx;
  for(int l1=0;l1<3;l1++) {
    for(int l2=0;l2<=l1;l2++) {
      Cx(l1,l2)=covPV(l1,l2);
    }
  }
  for(int l1=3;l1<8;l1++) {
    int n1=l1-3;
    if(n1>=2) n1++;
    for(int l2=3;l2<=l1;l2++) {
      int n2=l2-3;
      if(n2>=2) n2++;
      Cx(l1,l2)=covtr(n1,n2);
    }
  }
  for(int l1=3;l1<8;l1++) {
    for(int l2=0;l2<3;l2++) {
      Cx(l1,l2)=0.0;
    }
  }


  return sc;
}

//=============================================================================
//  associate a reconstructed PV to a MCVertex
//=============================================================================

const LHCb::MCVertex* CheatedPVReFitter::PV2MCVertex(const LHCb::RecVertex* pv ) const
{
  int max=0;
  const LHCb::MCVertex* best=0;
  if(!pv) return best;

  LHCb::MCHeader* mch = get<LHCb::MCHeader>(LHCb::MCHeaderLocation::Default);
  for ( SmartRefVector< LHCb::MCVertex >::const_iterator imcpv = mch->primaryVertices().begin() ;
        imcpv != mch->primaryVertices().end() ; ++imcpv ){
    const LHCb::MCVertex* mcPV = *imcpv;
    if( mcPV->mother() != NULL ) continue;
    if( mcPV->position().z() > 1000.0 ) continue;
    if(mcPV->type()!=LHCb::MCVertex::ppCollision) continue;
    int same=countMatchedPVTrks(pv,mcPV);
    double diff = fabs(pv->position().z() - mcPV->position().z());
    if( same>max && diff < 2.0*mm ) {
      max=same;
      best=mcPV;
    }
  }

  return best;

}

int CheatedPVReFitter::countMatchedPVTrks(const RecVertex* pv,
                                          const LHCb::MCVertex* mcPV) const
{
  int count=0;
  if(!mcPV) return count;
  if(mcPV->type()!=LHCb::MCVertex::ppCollision) return count;
  if(!pv) return count;
  //  if(!(pv->isPrimary()))return count;

  if( mcPV->mother() != NULL ) return count;

  if( mcPV->position().z() > 1000.0 ) return count;

  SmartRefVector<LHCb::Track> pvtracks = pv->tracks();
  for( SmartRefVector<LHCb::Track>::iterator trIt = pvtracks.begin();
       pvtracks.end() != trIt; trIt++) {
    LHCb::Track* track = *trIt;
    if(!track) continue;
    LinkedTo<LHCb::MCParticle,LHCb::Track> directLink( evtSvc(), msgSvc(), TrackLocation::Default );

    for( LHCb::MCParticle* mcPart = directLink.first(track);
         NULL != mcPart;
         mcPart = directLink.next() ) {
      const LHCb::MCVertex* originv2=mcPart->originVertex();
      if(!originv2) continue;
      Gaudi::XYZVector tmpL = originv2->position() - mcPV->position();
      double disv = tmpL.R();
      if( fabs(disv)<0.000000001*mm ) {count++;break;}
    }
  }
  return count;

}

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY(CheatedPVReFitter)
