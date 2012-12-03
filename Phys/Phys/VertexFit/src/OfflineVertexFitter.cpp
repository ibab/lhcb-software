// $Id: OfflineVertexFitter.cpp,v 1.40 2010-07-07 11:12:07 jonrob Exp $

// local
#include "OfflineVertexFitter.h"

using namespace LHCb ;
using namespace Gaudi::Units;

//-----------------------------------------------------------------------------
// Implementation file for class : OfflineVertexFitter
//
// 2006-03-15 : Yuehong Xie
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
OfflineVertexFitter::OfflineVertexFitter( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : GaudiTool  ( type, name , parent )
  , m_photonID ( 22                  )
  , m_pi0ID    ( 111                 )
{
  declareInterface<IVertexFit>       (this);
  declareInterface<IParticleCombiner>(this);
  declareInterface<IParticleReFitter>(this);

  declareProperty( "useResonanceVertex", m_useResonanceVertex = true);
  declareProperty( "includeDauVertexChi2", m_includeDauVertexChi2 = true);
  declareProperty( "applyDauMassConstraint", m_applyDauMassConstraint = false);
  declareProperty( "applyMomMassConstraint", m_applyMomMassConstraint = false);
  declareProperty( "widthThreshold", m_widthThreshold = 2.0 * MeV);
  declareProperty( "maxIter", m_maxIter = 10);
  declareProperty( "maxDeltaChi2", m_maxDeltaChi2 = 0.001);
  declareProperty( "maxDeltaZ",  m_maxDeltaZ = 1.0 * mm) ;
  declareProperty( "Transporter", m_transporterName = "ParticleTransporter:PUBLIC" );

  //setProperty( "OutputLevel", 1 );
}

//=============================================================================
// Destructor
//=============================================================================
OfflineVertexFitter::~OfflineVertexFitter() {}

//=============================================================================
// Initialize
//=============================================================================
StatusCode OfflineVertexFitter::initialize()
{
  const StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;

  m_transporter = tool<IParticleTransporter>(m_transporterName, this);
  m_ppSvc = svc<LHCb::IParticlePropertySvc>("LHCb::ParticlePropertySvc");

  const LHCb::ParticleProperty* partProp;
  partProp = m_ppSvc->find( "gamma" );
  m_photonID = partProp->particleID().pid();

  partProp = m_ppSvc->find( "pi0" );
  m_pi0ID  = partProp->particleID().pid();

  return sc;
}

//=============================================================================
// Fit the vertex from a vector of Particles
//=============================================================================
StatusCode OfflineVertexFitter::fit( const LHCb::Particle::ConstVector& parts,
                                     LHCb::Vertex&   V ,
                                     LHCb::Particle& P ) const
{
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "start fit " <<endmsg;
    debug() << "using " << parts.size() <<" particles" <<endmsg;
  }

  P.clearDaughters();
  V.clearOutgoingParticles();
  V.setChi2(0.0);
  V.setNDoF(0);
  P.setEndVertex( &V );

  LHCb::Particle::ConstVector FlyingParticles;
  LHCb::Particle::ConstVector VertexedParticles; // they are the ones with a vertex.
  LHCb::Particle::ConstVector Photons;
  LHCb::Particle::ConstVector MergedPi0s;
  LHCb::Particle::ConstVector PhotonPairs;

  //  FlyingParticles.clear();
  //  VertexedParticles.clear();
  //  Photons.clear();
  //  PhotonPairs.clear();

  StatusCode sc = StatusCode::SUCCESS;

  if (msgLevel(MSG::DEBUG)) debug() << "starting classifying particles to fit " <<endmsg;

  for ( Particle::ConstVector::const_iterator iPart = parts.begin();
        iPart != parts.end(); ++iPart )
  {
    const Particle* parPointer = *iPart;
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << parPointer->particleID().pid() << " at "
                << parPointer->referencePoint()
                << " with " << parPointer->momentum() << endmsg ;
    }
    sc = classify(parPointer, FlyingParticles, VertexedParticles, Photons, MergedPi0s, PhotonPairs);
    if ( sc.isFailure() )
    {
      if ( msgLevel(MSG::DEBUG) ) debug() << "Fail to classify a particle" << endmsg;
      return sc;
    }
  }

  if ( msgLevel(MSG::DEBUG ))
  {
    debug() << "classfication result " <<endmsg;
    debug() << "flying particles " << FlyingParticles.size() <<endmsg;
    debug() << "vertexed particles " << VertexedParticles.size() <<endmsg;
    debug() << "photons " << Photons.size() <<endmsg;
    debug() << "merged pi0s " << MergedPi0s.size() <<endmsg;
    debug() << "photon pairs " << PhotonPairs.size() <<endmsg;
  }

  // add to particle and vertex
  addToDaughters( P, parts );
  /* Done when each one is used
     addToOutgoingParticles( V, FlyingParticles );
     addToOutgoingParticles( V, VertexedParticles );
     addToOutgoingParticles( V, Photons );
     addToOutgoingParticles( V, PhotonPairs );
  */

  sc = seeding(P, FlyingParticles, VertexedParticles);
  if ( sc.isFailure() )
  {
    if (msgLevel(MSG::DEBUG)) debug() << "Fail to seed the vertex" << endmsg;
    return sc;
  }

  for ( Particle::ConstVector::const_iterator iterP = VertexedParticles.begin();
        iterP != VertexedParticles.end(); ++iterP )
  {
    const Particle* daughter = *iterP;
    sc = addVertexed(P, daughter);
    if ( sc.isFailure() )
    {
      if (msgLevel(MSG::DEBUG)) debug() << "Fail to add a vertexed particle" << endmsg;
      return sc;
    }
  }

  for ( Particle::ConstVector::const_iterator iterP = FlyingParticles.begin();
        iterP != FlyingParticles.end(); ++iterP )
  {
    const Particle* daughter = *iterP;
    sc = addFlying(P, daughter);
    if ( sc.isFailure() )
    {
      if (msgLevel(MSG::DEBUG)) debug() << "Fail to add a flying particle" << endmsg;
      return sc;
    }
  }

  for ( Particle::ConstVector::const_iterator iterP = Photons.begin();
        iterP != Photons.end(); ++iterP )
  {
    const Particle* daughter = *iterP;
    sc = addPhoton(P, daughter);
    if ( sc.isFailure() )
    {
      if (msgLevel(MSG::DEBUG)) debug() << "Fail to add a photon" << endmsg;
      return sc;
    }
  }

  for ( Particle::ConstVector::const_iterator iterP = MergedPi0s.begin();
        iterP != MergedPi0s.end(); ++iterP )
  {
    const Particle* daughter = *iterP;
    sc = addMergedPi0(P, daughter);
    if ( sc.isFailure() )
    {
      if (msgLevel(MSG::DEBUG)) debug() << "Fail to add a merged pi0" << endmsg;
      return sc;
    }
  }

  for ( Particle::ConstVector::const_iterator iterP = PhotonPairs.begin();
        iterP != PhotonPairs.end(); ++iterP )
  {
    const Particle* daughter = *iterP;
    sc = addPhotonPair(P, daughter);
    if ( sc.isFailure() )
    {
      if (msgLevel(MSG::DEBUG)) debug() << "Fail to add a photon pair" << endmsg;
      return sc;
    }
  }

  if ( m_applyMomMassConstraint )
  {
    Gaudi::Vector7 V7;
    Gaudi::SymMatrix7x7 C7;
    double chi2 = 0.;
    int NDoF = 0;
    getParticleInfo(P, V7, C7, chi2, NDoF);
    const double mm    = P.measuredMass();
    const double mmerr = P.measuredMassErr();
    //const int pid = P.particleID().pid();
    //ParticleProperty*  partProp = m_ppSvc->findByStdHepID( pid  );
    const LHCb::ParticleProperty*  partProp = m_ppSvc->find(P.particleID());

    double nominalMass = partProp->mass();
    sc = constrainMass(V7, C7, nominalMass);
    if ( sc.isFailure() )
    {
      if (msgLevel(MSG::DEBUG)) debug() << "Fail to apply mass constraint on the mother particle" << endmsg;
      return sc;
    }
    sc = updateParticle(P, V7, C7, chi2, NDoF);
    P.setMeasuredMass(mm);
    P.setMeasuredMassErr(mmerr);
  }

  if ( msgLevel(MSG::DEBUG) )
  {
    if ( P.daughters().size() != P.endVertex()->outgoingParticles().size() )
      debug() << "Did not use exactly particle daughters to fit vertex. Probably nothing to worry about." << endmsg;

    debug() << "############## Fitted a vertex for a " << P.particleID().pid() ;
    if ( P.daughters().size() !=  P.endVertex()->outgoingParticles().size()) debug() << " and got different sizes !";
    debug() << " P= " << P.momentum() << endmsg ;
    debug() <<  P.daughters().size() << " Particle daughters are :" << endmsg ;
    for ( SmartRefVector< LHCb::Particle >::const_iterator iterP = P.daughters().begin() ;
          iterP!=P.daughters().end() ; ++iterP){
      debug() << "       " << (*iterP)->particleID().pid() << " " << (*iterP)->momentum() << " " << endmsg ;
    }
    debug() << P.endVertex()->outgoingParticles().size() << " Vertex outgoing particles are :" ;
    for ( SmartRefVector< LHCb::Particle >::const_iterator iterP = P.endVertex()->outgoingParticles().begin() ;
          iterP!=P.endVertex()->outgoingParticles().end() ; ++iterP){
      debug() << " " << (*iterP)->particleID().pid() ;
    }
    debug() << endmsg ;
    debug() << "Vertex chi2 is " << V.chi2() << " with " << V.nDoF() << " DoF" << endmsg  ;
  }

  return sc;
}

//=============================================================================
// Fit the vertex from a vector of Particles
//=============================================================================
StatusCode
OfflineVertexFitter::fit( LHCb::Vertex& V ,
                          const LHCb::Particle::ConstVector& parts ) const
{
  LHCb::Particle tPart;
  return fit(parts, V , tPart );
}

//==================================================================
//  method to determine if a particle has non-photon final decay
//  products or not
//==================================================================
bool OfflineVertexFitter::isPurePhotonDecay(const LHCb::Particle* part) const
{
  const Particle::ConstVector & Prods = part->daughtersVector();
  for ( Particle::ConstVector::const_iterator iProd = Prods.begin();
        iProd != Prods.end(); ++iProd )
  {
    const Particle* daughter = *iProd;
    if ( daughter->particleID().pid() != m_photonID &&
         !isPurePhotonDecay(daughter) ) return false;
  }
  return true;
}

//==================================================================
//  method to determine if a particle decays to gamma gamma
//==================================================================
bool OfflineVertexFitter::isPhotonPair(const LHCb::Particle* part)  const
{
  const Particle::ConstVector & Prods = part->daughtersVector();
  if ( Prods.size() != 2 ) return false;
  for ( Particle::ConstVector::const_iterator iProd = Prods.begin();
        iProd != Prods.end(); ++iProd )
  {
    const Particle* daughter = *iProd;
    if ( daughter->particleID().pid() != m_photonID ) return false;
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
                                         LHCb::Particle::ConstVector& FlyingParticles,
                                         LHCb::Particle::ConstVector& VertexedParticles,
                                         LHCb::Particle::ConstVector& Photons,
                                         LHCb::Particle::ConstVector& MergedPi0s,
                                         LHCb::Particle::ConstVector& PhotonPairs) const
{
  StatusCode sc = StatusCode::SUCCESS;

  const int id = part->particleID().pid();

  if(part->isBasicParticle())
  {
    if      (id==m_photonID)    { Photons.push_back(part); }
    else if (isMergedPi0(part)) { MergedPi0s.push_back(part); }
    else                        { FlyingParticles.push_back(part); }
  }
  else
  {
    if(!isResonance(part))
    {
      FlyingParticles.push_back(part);
    }
    else
    {
      if(isVertexed(part))
      {
        if (m_useResonanceVertex)
        {
          VertexedParticles.push_back(part);
        }
        else
        {
          const Particle::ConstVector& Prods = part->daughtersVector();
          for ( Particle::ConstVector::const_iterator iProd=Prods.begin(); iProd!=Prods.end(); ++iProd )
          {
            const Particle* daughter = *iProd;
            sc = classify(daughter, FlyingParticles, VertexedParticles, Photons, MergedPi0s, PhotonPairs);
            if(sc.isFailure()) return sc;
          }
        }
      }
      else
      {
        if (isPhotonPair(part))
        {
          PhotonPairs.push_back(part);
        }
        else
        {
          const Particle::ConstVector & Prods = part->daughtersVector();
          for ( Particle::ConstVector::const_iterator iProd=Prods.begin(); iProd!=Prods.end(); ++iProd )
          {
            const Particle* daughter = *iProd;
            sc = classify(daughter, FlyingParticles, VertexedParticles, Photons, MergedPi0s, PhotonPairs);
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

StatusCode
OfflineVertexFitter::seeding(LHCb::Particle& part,
                             LHCb::Particle::ConstVector& FlyingParticles,
                             LHCb::Particle::ConstVector& VertexedParticles) const
{
  StatusCode sc = StatusCode::FAILURE;

  if ( VertexedParticles.size() < 1 && 
       FlyingParticles.size()   < 2 ) 
  {
    if ( msgLevel(MSG::VERBOSE) ) verbose() << "seeding:: Input particles error" << endmsg;
    return StatusCode::FAILURE;
  }

  if ( !VertexedParticles.empty() )
  {
    const LHCb::Particle* vertpart = VertexedParticles.back();
    VertexedParticles.pop_back();
    addToOutgoingParticles( *(part.endVertex()), vertpart );

    Gaudi::Vector7 V7;
    Gaudi::SymMatrix7x7 C7;
    double chi2(0);
    int NDoF(0);
    getParticleInfo( *vertpart, V7, C7, chi2, NDoF );
    double nominalMass(0.0);
    const bool constrainM = requireMassConstraint(vertpart,nominalMass);
    if ( constrainM ) 
    {
      sc = constrainMass(V7, C7, nominalMass);
      if ( sc.isFailure() ) 
      {
        if ( msgLevel(MSG::VERBOSE) ) verbose() << "seeding:: Failed to constrainMass" << endmsg;
        return sc;
      }
    }
    if ( !m_includeDauVertexChi2 )
    {
      //don't include resonance veretx chi2 and NDoF!
      chi2 = 0.;
      NDoF = 0;
    }
    sc = updateParticle(part, V7, C7, chi2, NDoF);
    if ( sc.isFailure() && msgLevel(MSG::VERBOSE) )
    {
      verbose() << "seeding:: VertexedParticles updateParticle failed" << endmsg;
    }

  }
  else if ( !FlyingParticles.empty() )
  {

    const LHCb::Particle* flypart1 = FlyingParticles.back();
    FlyingParticles.pop_back();
    addToOutgoingParticles(*(part.endVertex()),flypart1);
    const LHCb::Particle* flypart2 = FlyingParticles.back();
    FlyingParticles.pop_back();
    addToOutgoingParticles(*(part.endVertex()),flypart2);

    Gaudi::Vector7 V7;
    Gaudi::SymMatrix7x7 C7;
    double chi2(0);
    const int NDoF(1);

    sc = fitTwo(flypart1, flypart2, V7, C7, chi2);
    if ( sc.isFailure() )
    {
      if ( msgLevel(MSG::VERBOSE) ) verbose() << "seeding:: fitTwo failed" << endmsg;
      return sc;
    }

    sc = updateParticle(part, V7, C7, chi2, NDoF);
    if ( sc.isFailure() && msgLevel(MSG::VERBOSE) )
    {
      verbose() << "seeding:: FlyingParticles updateParticle failed" << endmsg;
    }

  }

  return sc;
}

//==================================================================
//  method to add a flying particle
//==================================================================
StatusCode OfflineVertexFitter::addFlying(LHCb::Particle& part,
                                          const LHCb::Particle* dau) const
{
  StatusCode sc = StatusCode::SUCCESS;

  Gaudi::Vector7 V7;
  Gaudi::SymMatrix7x7 C7;
  double chi2 = 0.;
  int NDoF = 0;
  getParticleInfo(part, V7, C7, chi2, NDoF);

  Gaudi::Vector7 Vm7;
  Gaudi::SymMatrix7x7 Cm7;
  convertE2M(V7, C7, Vm7, Cm7);

  const double z2=Vm7[2];

  double nominalMassDau    = 0.0;
  const bool constrainMDau = requireMassConstraint(dau,nominalMassDau);

  LHCb::Particle transParticleDau;
  sc = m_transporter->transportAndProject(dau, z2, transParticleDau);
  if( sc.isFailure ())
  {
    if (msgLevel(MSG::DEBUG))
      debug() << "transport of dau failed in addFlying(" << endmsg;
    return sc;
  }

  Gaudi::Vector7 daupara;
  Gaudi::SymMatrix7x7 daucov;
  double dauchi2 = 0.;
  int dauNDoF = 0;
  getParticleInfo(transParticleDau, daupara, daucov, dauchi2, dauNDoF);

  if(constrainMDau)
  {
    sc=constrainMass(daupara, daucov, nominalMassDau);
    if(sc.isFailure()) return StatusCode::FAILURE;
  }

  Gaudi::Vector7 daumpara;
  Gaudi::SymMatrix7x7 daumcov;
  convertE2M(daupara,daucov, daumpara, daumcov);
  const int id1=abs(dau->particleID().pid());
  if(id1==11||id1==13||id1==211||id1==321||id1==2212)
  {
    for(int k=0; k<=6; k++) daumcov(6,k)=0.0;
  }

  //temporary fix for MCParticleMaker
  //for(int i=0;i<=6;i++) daumcov(6,i)=0.0;

  ROOT::Math::SVector<double, 13> X;
  for(int i=0;i<7;i++)
  {
    X[i]=Vm7[i];
  }
  X[7]=daumpara[0];
  X[8]=daumpara[1];
  X[9]=daumpara[3];
  X[10]=daumpara[4];
  X[11]=daumpara[5];
  X[12]=daumpara[6];

  Gaudi::SymMatrix6x6 newcovdau;
  for(int l1=0;l1<6;l1++)
  {
    int n1=l1;
    if(n1>=2) n1++;
    for(int l2=0;l2<=l1;l2++)
    {
      int n2=l2;
      if(n2>=2) n2++;
      newcovdau(l1,l2)=daumcov(n1,n2);
    }
  }

  SymMatrix13x13 Cx;
  for(int l1=0; l1<7; l1++)
  {
    for(int l2=0; l2<=l1; l2++)
    {
      Cx(l1,l2)=Cm7(l1,l2);
    }
  }
  for(int l1=7; l1<13; l1++)
  {
    for(int l2=7; l2<=l1; l2++)
    {
      Cx(l1,l2)=newcovdau(l1-7,l2-7);
    }
  }
  for(int l1=7; l1<13; l1++)
  {
    for(int l2=0; l2<7; l2++)
    {
      Cx(l1,l2)=0.0;
    }
  }

  // x1,y1,z1, px1,py1,pz1,m1 and x2,y2,px2,py2,pz2,m2 at z2
  ROOT::Math::SVector<double, 13> vfit=X;
  SymMatrix13x13 cfit = Cx;

  bool converged=false;
  int iter=0;

  double chi2PreviousFit=9999.;
  double chi2Fit=999.;

  while(!converged && iter< m_maxIter)
  {
    iter++;

    if ( msgLevel(MSG::VERBOSE) )
      verbose() << ":-) Iteration   " << iter << endmsg;

    //f(0)=(x2-x1)*pz2-(z2-z1)*px2
    //f(1)=(y2-y1)*pz2-(z2-z1)*py2

    ROOT::Math::SVector<double, 2> f;
    f(0)= (vfit[7]-vfit[0])*vfit[11]- (z2-vfit[2])*vfit[9];
    f(1)= (vfit[8]-vfit[1])*vfit[11]- (z2-vfit[2])*vfit[10];

    //D is the derivative matrix
    ROOT::Math::SMatrix<double, 2, 13> D;
    D(0,0) = - vfit[11];
    D(0,1) = 0.0;
    D(0,2) = vfit[9];
    D(0,3) = 0.0;
    D(0,4) = 0.0;
    D(0,5) = 0.0;
    D(0,6) = 0.0;
    D(0,7) = vfit[11];
    D(0,8) = 0.0;
    D(0,9) =  - (z2-vfit[2]);
    D(0,10) = 0.0;
    D(0,11) = (vfit[7]-vfit[0]);
    D(0,12) = 0.0;
    D(1,0) = 0.0;
    D(1,1) = - vfit[11];
    D(1,2) = vfit[10];
    D(1,3) = 0.0;
    D(1,4) = 0.0;
    D(1,5) = 0.0;
    D(1,6) = 0.0;
    D(1,7) = 0.0;
    D(1,8) = vfit[11];
    D(1,9) = 0.0;
    D(1,10) = - (z2-vfit[2]);
    D(1,11) =  (vfit[8]-vfit[1]);
    D(1,12) = 0.0;

    ROOT::Math::SVector<double, 2> d = f - D*vfit;

    Gaudi::SymMatrix2x2 VD=ROOT::Math::Similarity<double,2,13>(D, Cx);
    if(!VD.Invert()) {
      if (msgLevel(MSG::DEBUG))
        debug() << "could not invert matrix VD in addFlying! " <<endmsg;
      return StatusCode::FAILURE;
    }

    ROOT::Math::SVector<double, 2> alpha=D*X+d;

    ROOT::Math::SVector<double, 2> lambda=VD*alpha;

    ROOT::Math::SMatrix<double, 13,2> DT = ROOT::Math::Transpose(D);

    vfit=X-Cx*DT*lambda;

    SymMatrix13x13 delataC1=ROOT::Math::Similarity<double,13,2>(DT, VD);

    SymMatrix13x13 delataC2=ROOT::Math::Similarity<double,13,13>(Cx, delataC1);

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

  Gaudi::Vector7 V7new;

  double chi2new = chi2 + chi2Fit ;
  int NDoFnew = NDoF + 2;

  V7new[0]=vfit[0];
  V7new[1]=vfit[1];
  V7new[2]=vfit[2];
  V7new[3]=vfit[3] + vfit[9];
  V7new[4]=vfit[4] + vfit[10];
  V7new[5]=vfit[5] + vfit[11];

  const double e1= std::sqrt(vfit[3]*vfit[3]+vfit[4]*vfit[4]+vfit[5]*vfit[5]+vfit[6]*vfit[6]);
  const double e2= std::sqrt(vfit[9]*vfit[9]+vfit[10]*vfit[10]+vfit[11]*vfit[11]+vfit[12]*vfit[12]);
  V7new[6]=e1 + e2;

  ROOT::Math::SMatrix<double, 7, 13> JA;
  JA(0,0) = 1.;
  JA(1,1) = 1.;
  JA(2,2) = 1.;
  JA(3,3) = 1.;
  JA(3,9) = 1.;
  JA(4,4) = 1.;
  JA(4,10) = 1.;
  JA(5,5) = 1.;
  JA(5,11) = 1.;
  JA(6,3) = vfit[3]/e1;
  JA(6,4) = vfit[4]/e1;
  JA(6,5) = vfit[5]/e1;
  JA(6,6) = vfit[6]/e1;
  JA(6,9) = vfit[9]/e2;
  JA(6,10) = vfit[10]/e2;
  JA(6,11) = vfit[11]/e2;
  JA(6,12) = vfit[12]/e2;

  const Gaudi::SymMatrix7x7 C7new = ROOT::Math::Similarity<double,7,13>(JA, cfit);

  sc = updateParticle(part, V7new, C7new, chi2new, NDoFnew);
  addToOutgoingParticles(*(part.endVertex()),dau);

  return sc;

}

//==================================================================
//  method to add a vertexed particle
//==================================================================
StatusCode OfflineVertexFitter::addVertexed(LHCb::Particle& part,
                                            const LHCb::Particle * dau) const
{
  StatusCode sc = StatusCode::SUCCESS;

  Gaudi::Vector7 V7;
  Gaudi::SymMatrix7x7 C7;
  double chi2 = 0.;
  int NDoF = 0;
  getParticleInfo(part, V7, C7, chi2, NDoF);

  Gaudi::Vector7 Vm7;
  Gaudi::SymMatrix7x7 Cm7;
  convertE2M(V7, C7, Vm7, Cm7);

  double nominalMassDau=0.0;
  const bool constrainMDau=requireMassConstraint(dau,nominalMassDau);

  Gaudi::Vector7 daupara;
  Gaudi::SymMatrix7x7 daucov;
  double dauchi2 = 0.;
  int dauNDoF = 0;
  getParticleInfo(*dau, daupara, daucov, dauchi2, dauNDoF);

  if ( constrainMDau )
  {
    sc = constrainMass(daupara, daucov, nominalMassDau);
    if ( sc.isFailure() ) return StatusCode::FAILURE;
  }

  Gaudi::Vector7 daumpara;
  Gaudi::SymMatrix7x7 daumcov;
  convertE2M(daupara,daucov, daumpara, daumcov);

  //temporary fix for MCParticleMaker
  //for(int i=0;i<=6;i++) daumcov(6,i)=0.0;

  ROOT::Math::SVector<double, 14> X;
  for(int i=0;i<7;i++) {
    X[i]=Vm7[i];
    X[i+7]=daumpara[i];
  }

  SymMatrix14x14 Cx;
  for(int l1=0;l1<7;++l1)
  {
    for(int l2=0;l2<=l1;++l2)
    {
      Cx(l1,l2)=Cm7(l1,l2);
      Cx(l1+7,l2+7)=daumcov(l1,l2);
    }
    for(int l2=0;l2<7;++l2) Cx(l1+7,l2)=0.0;
  }

  // x1,y1,z1, px1,py1,pz1,m1, x2,y2,z2,px1,py1,pz1,m2
  ROOT::Math::SVector<double, 14> vfit=X;
  SymMatrix14x14 cfit = Cx;

  bool converged=false;
  int iter=0;

  double chi2PreviousFit=9999.;
  double chi2Fit=999.;

  while(!converged && iter< m_maxIter)
  {
    ++iter;

    if ( msgLevel(MSG::VERBOSE) )
      verbose() << ":-) Iteration   " << iter << endmsg;

    //f(0)=x2-x1;
    //f(1)=y2-y1;
    //f(2)=z2-z1;

    ROOT::Math::SVector<double, 3> f;
    f(0)=vfit(7)-vfit(0);
    f(1)=vfit(8)-vfit(1);
    f(2)=vfit(9)-vfit(2);

    //D is the derivative matrix
    ROOT::Math::SMatrix<double, 3, 14> D;

    D(0,0) = - 1.0;
    D(0,7) =   1.0;
    D(1,1) = - 1.0;
    D(1,8) =   1.0;
    D(2,2) = - 1.0;
    D(2,9) =   1.0;

    ROOT::Math::SVector<double, 3> d = f - D*vfit;

    Gaudi::SymMatrix3x3 VD=ROOT::Math::Similarity<double,3,14>(D, Cx);
    if(!VD.Invert())
    {
      if (msgLevel(MSG::DEBUG))
        debug() << "could not invert matrix VD in addVertexed! " <<endmsg;
      return StatusCode::FAILURE;
    }

    ROOT::Math::SVector<double, 3> alpha=D*X+d;

    ROOT::Math::SVector<double, 3> lambda=VD*alpha;

    ROOT::Math::SMatrix<double, 14,3> DT = ROOT::Math::Transpose(D);

    vfit=X-Cx*DT*lambda;

    SymMatrix14x14 delataC1=ROOT::Math::Similarity<double,14,3>(DT, VD);

    SymMatrix14x14 delataC2=ROOT::Math::Similarity<double,14,14>(Cx, delataC1);

    cfit=Cx -delataC2;

    chi2Fit=ROOT::Math::Dot(alpha,lambda);
    //chi2Fit+= 2*ROOT::Math::Dot(lambda,f);

    if ( fabs(chi2Fit-chi2PreviousFit)<m_maxDeltaChi2 )
    {
      converged=true;
    }
    else
    {
      chi2PreviousFit=chi2Fit;
    }

  }  // end chi2 minimization iteration

  if(!converged)  return StatusCode::FAILURE;

  Gaudi::Vector7 V7new;

  double chi2new = chi2 + chi2Fit ;
  int NDoFnew = NDoF + 3;

  if(m_includeDauVertexChi2) {
    chi2new += dauchi2;
    NDoFnew += dauNDoF;
  }

  V7new[0]=vfit[0];
  V7new[1]=vfit[1];
  V7new[2]=vfit[2];
  V7new[3]=vfit[3]+vfit[10];
  V7new[4]=vfit[4]+vfit[11];
  V7new[5]=vfit[5]+vfit[12];

  double e1= std::sqrt(vfit[3]*vfit[3]+vfit[4]*vfit[4]+vfit[5]*vfit[5]+vfit[6]*vfit[6]);
  double e2= std::sqrt(vfit[10]*vfit[10]+vfit[11]*vfit[11]+vfit[12]*vfit[12]+vfit[13]*vfit[13]);
  V7new[6]=e1+e2;

  ROOT::Math::SMatrix<double, 7, 14> JA;
  JA(0,0) = 1.;
  JA(1,1) = 1.;
  JA(2,2) = 1.;
  JA(3,3) = 1.;
  JA(3,10) = 1.;
  JA(4,4) = 1.;
  JA(4,11) = 1.;
  JA(5,5) = 1.;
  JA(5,12) = 1.;
  JA(6,3) = vfit[3]/e1;
  JA(6,4) = vfit[4]/e1;
  JA(6,5) = vfit[5]/e1;
  JA(6,6) = vfit[6]/e1;
  JA(6,10) = vfit[10]/e2;
  JA(6,11) = vfit[11]/e2;
  JA(6,12) = vfit[12]/e2;
  JA(6,13) = vfit[13]/e2;

  const Gaudi::SymMatrix7x7 C7new = ROOT::Math::Similarity<double,7,14>(JA, cfit);

  addToOutgoingParticles(*(part.endVertex()),dau);
  sc = updateParticle(part, V7new, C7new, chi2new, NDoFnew);

  return sc;
}

//==================================================================
//  method to add a photon
//==================================================================
StatusCode
OfflineVertexFitter::addPhoton(LHCb::Particle& part,
                               const LHCb::Particle * dau) const
{
  StatusCode sc = StatusCode::SUCCESS;

  Gaudi::Vector7 V7;
  Gaudi::SymMatrix7x7 C7;
  double chi2 = 0.;
  int NDoF = 0;
  getParticleInfo(part, V7, C7, chi2, NDoF);

  double zg=-9999.;
  Gaudi::Vector3 gammapara;
  Gaudi::SymMatrix3x3 gammacov;

  sc=getPhotonParameter(*dau, zg, gammapara, gammacov);
  if(sc.isFailure()) {
    if (msgLevel(MSG::DEBUG))
      debug() << "Fail to getPhotonParameter in  addPhoton" << endmsg;
    return StatusCode::FAILURE;
  }

  Gaudi::Vector7 V7new;

  double chi2new = chi2 ;
  int NDoFnew = NDoF;

  const double dx= gammapara[0] - V7[0];
  const double dy= gammapara[1] - V7[1];
  const double dz= zg - V7[2];
  const double r= std::sqrt(dx*dx+dy*dy+dz*dz);
  const double eg= gammapara[2];
  const double pxg= eg*dx/r;
  const double pyg= eg*dy/r;
  const double pzg= eg*dz/r;

  V7new[0]= V7[0];
  V7new[1]= V7[1];
  V7new[2]= V7[2];
  V7new[3]= V7[3] + pxg;
  V7new[4]= V7[4] + pyg;
  V7new[5]= V7[5] + pzg;
  V7new[6]= V7[6] + eg;

  SymMatrix10x10 Cold;

  for(int l1=0; l1<=6; l1++)
    for (int l2=0; l2<=l1; l2++) Cold(l1,l2) = C7(l1,l2);

  for(int l1=0; l1<=2; l1++)
    for (int l2=0; l2<=l1; l2++) Cold(l1+7,l2+7) = gammacov(l1,l2);

  for(int l1=7; l1<=9;l1++)
    for(int l2=0; l2<7; l2++) Cold(l1,l2)=0.0;

  ROOT::Math::SMatrix<double, 7, 10> JA;
  for(int i=0; i<7 ;i++) JA(i,i)=1.;

  JA(3,0) = eg*(dx*dx/r/r/r-1./r);
  JA(3,1) = eg*(dx*dy/r/r/r);
  JA(3,2) = eg*(dx*dz/r/r/r);
  JA(3,7)=eg*(-dx*dx/r/r/r+1./r);
  JA(3,8)=eg*(-dx*dy/r/r/r);

  JA(4,0)=eg*(dy*dx/r/r/r);
  JA(4,1)=eg*(dy*dy/r/r/r-1./r);
  JA(4,2)=eg*(dy*dz/r/r/r);
  JA(4,7)=eg*(-dy*dx/r/r/r);
  JA(4,8)=eg*(-dy*dy/r/r/r+1./r);

  JA(5,0)=eg*(dz*dx/r/r/r);
  JA(5,1)=eg*(dz*dy/r/r/r);
  JA(5,2)=eg*(dz*dz/r/r/r-1./r);
  JA(5,7)=eg*(-dz*dx/r/r/r);
  JA(5,8)=eg*(-dz*dy/r/r/r);

  JA(3,9)=dx/r;
  JA(4,9)=dy/r;
  JA(5,9)=dz/r;
  JA(6,9)=1.;

  const Gaudi::SymMatrix7x7 C7new = ROOT::Math::Similarity<double,7,10>(JA, Cold);

  addToOutgoingParticles(*(part.endVertex()),dau);
  sc = updateParticle(part, V7new, C7new, chi2new, NDoFnew);

  return sc;
}

//==================================================================
//  method to add a photon pair
//==================================================================
StatusCode OfflineVertexFitter::addPhotonPair(LHCb::Particle& part,
                                              const LHCb::Particle * dau) const
{
  StatusCode sc = StatusCode::SUCCESS;

  Gaudi::Vector7 V7;
  Gaudi::SymMatrix7x7 C7;
  double chi2 = 0.;
  int NDoF = 0;
  getParticleInfo(part, V7, C7, chi2, NDoF);

  //get "pair" daughters
  const Particle* gamma1 =0;
  const Particle* gamma2 =0;
  Particle::ConstVector Prods =dau->daughtersVector();
  int ig=0;
  for ( Particle::ConstVector::const_iterator iProd=Prods.begin(); iProd!=Prods.end(); ++iProd )
  {
    const Particle* daughter = *iProd;
    if( daughter->particleID().pid()!=m_photonID ) return StatusCode::FAILURE;
    ++ig;
    if(ig==1) gamma1 = daughter;
    else gamma2 = daughter;
  }
  if(ig!=2) return StatusCode::FAILURE;

  double zg1=-9999.;
  Gaudi::Vector3 gamma1para;
  Gaudi::SymMatrix3x3 gamma1cov;

  sc=getPhotonParameter(*gamma1, zg1, gamma1para, gamma1cov);
  if ( sc.isFailure() )
  {
    if (msgLevel(MSG::DEBUG))
      debug() << "Fail to getPhotonParameter in  addPhotonPair" << endmsg;
    return StatusCode::FAILURE;
  }

  double zg2=-9999.;
  Gaudi::Vector3 gamma2para;
  Gaudi::SymMatrix3x3 gamma2cov;

  sc = getPhotonParameter(*gamma2, zg2, gamma2para, gamma2cov);
  if ( sc.isFailure() )
  {
    if (msgLevel(MSG::DEBUG))
      debug() << "Fail to getPhotonParameter in  addPhotonPair" << endmsg;
    return StatusCode::FAILURE;
  }

  Gaudi::Vector8 Vnew;
  Gaudi::SymMatrix8x8 Cnew;

  const double dx1= gamma1para[0] - V7[0];
  const double dy1= gamma1para[1] - V7[1];
  const double dz1= zg1 - V7[2];
  const double r1= std::sqrt(dx1*dx1+dy1*dy1+dz1*dz1);
  const double eg1= gamma1para[2];
  const double pxg1= eg1*dx1/r1;
  const double pyg1= eg1*dy1/r1;
  const double pzg1= eg1*dz1/r1;

  const double dx2= gamma2para[0] - V7[0];
  const double dy2= gamma2para[1] - V7[1];
  const double dz2= zg2 - V7[2];
  const double r2= std::sqrt(dx2*dx2+dy2*dy2+dz2*dz2);
  const double eg2= gamma2para[2];
  const double pxg2= eg2*dx2/r2;
  const double pyg2= eg2*dy2/r2;
  const double pzg2= eg2*dz2/r2;

  const double mpair= std::sqrt( (eg1+eg2)*(eg1+eg2)-(pxg1+pxg2)*(pxg1+pxg2) -
                                 (pyg1+pyg2)*(pyg1+pyg2)-(pzg1+pzg2)*(pzg1+pzg2) );

  Vnew[0]= V7[0];
  Vnew[1]= V7[1];
  Vnew[2]= V7[2];
  Vnew[3]= V7[3] + pxg1 + pxg2;
  Vnew[4]= V7[4] + pyg1 + pyg2;
  Vnew[5]= V7[5] + pzg1 + pzg2;
  Vnew[6]= V7[6] + eg1  + eg2;
  Vnew[7]= mpair;

  SymMatrix13x13 Cold;

  for(int l1=0; l1<=6; ++l1)
    for (int l2=0; l2<=l1; ++l2) Cold(l1,l2) = C7(l1,l2);

  for(int l1=0; l1<=2; ++l1)
    for (int l2=0; l2<=l1; ++l2)
    {
      Cold(l1+7,l2+7) = gamma1cov(l1,l2);
      Cold(l1+10,l2+10) = gamma2cov(l1,l2);
    }

  for(int l1=7; l1<=9;l1++)
    for(int l2=0; l2<7; l2++) Cold(l1,l2)=0.0;

  for(int l1=10; l1<=12;l1++)
    for(int l2=0; l2<10; l2++) Cold(l1,l2)=0.0;

  ROOT::Math::SMatrix<double, 8, 13> JA;
  for ( int i=0; i<7; ++i ) JA(i,i)=1.;

  const double dinvr1_dx1=-1./(r1*r1)*(-dx1/r1);
  const double dinvr1_dy1=-1./(r1*r1)*(-dy1/r1);
  const double dinvr1_dz1=-1./(r1*r1)*(-dz1/r1);
  const double dinvr1_dxg1=-1./(r1*r1)*(dx1/r1);
  const double dinvr1_dyg1=-1./(r1*r1)*(dy1/r1);

  const double dinvr2_dx1=-1./(r2*r2)*(-dx2/r2);
  const double dinvr2_dy1=-1./(r2*r2)*(-dy2/r2);
  const double dinvr2_dz1=-1./(r2*r2)*(-dz2/r2);
  const double dinvr2_dxg2=-1./(r2*r2)*(dx2/r2);
  const double dinvr2_dyg2=-1./(r2*r2)*(dy2/r2);

  const double dpxg1_dx1=eg1*(-1./r1+dx1*dinvr1_dx1);
  const double dpyg1_dx1=eg1*dy1*dinvr1_dx1;
  const double dpzg1_dx1=eg1*dz1*dinvr1_dx1;
  const double dpxg2_dx1=eg2*(-1./r2+dx2*dinvr2_dx1);
  const double dpyg2_dx1=eg2*dy2*dinvr2_dx1;
  const double dpzg2_dx1=eg2*dz2*dinvr2_dx1;

  const double dpxg1_dy1=eg1*dx1*dinvr1_dy1;
  const double dpyg1_dy1=eg1*(-1./r1+dy1*dinvr1_dy1);
  const double dpzg1_dy1=eg1*dz1*dinvr1_dy1;
  const double dpxg2_dy1=eg2*dx2*dinvr2_dy1;
  const double dpyg2_dy1=eg2*(-1./r2+dy2*dinvr2_dy1);
  const double dpzg2_dy1=eg2*dz2*dinvr2_dy1;

  const double dpxg1_dz1=eg1*dx1*dinvr1_dz1;
  const double dpyg1_dz1=eg1*dy1*dinvr1_dz1;
  const double dpzg1_dz1=eg1*(-1./r1+dz1*dinvr1_dz1);
  const double dpxg2_dz1=eg2*dx2*dinvr2_dz1;
  const double dpyg2_dz1=eg2*dy2*dinvr2_dz1;
  const double dpzg2_dz1=eg2*(-1./r2+dz2*dinvr2_dz1);

  const double dpxg1_dxg1=eg1*(1./r1+dx1*dinvr1_dxg1);
  const double dpyg1_dxg1=eg1*dy1*dinvr1_dxg1;
  const double dpzg1_dxg1=eg1*dz1*dinvr1_dxg1;

  const double dpxg1_dyg1=eg1*dx1*dinvr1_dyg1;
  const double dpyg1_dyg1=eg1*(1./r1+dy1*dinvr1_dyg1);
  const double dpzg1_dyg1=eg1*dz1*dinvr1_dyg1;

  const double dpxg2_dxg2=eg2*(1./r2+dx2*dinvr2_dxg2);
  const double dpyg2_dxg2=eg2*dy2*dinvr2_dxg2;
  const double dpzg2_dxg2=eg2*dz2*dinvr2_dxg2;

  const double dpxg2_dyg2=eg2*dx2*dinvr2_dyg2;
  const double dpyg2_dyg2=eg2*(1./r2+dy2*dinvr2_dyg2);
  const double dpzg2_dyg2=eg2*dz2*dinvr2_dyg2;

  JA(3,0)=dpxg1_dx1+dpxg2_dx1;
  JA(3,1)=dpxg1_dy1+dpxg2_dy1;
  JA(3,2)=dpxg1_dz1+dpxg2_dz1;
  JA(3,7)=dpxg1_dxg1;
  JA(3,8)=dpxg1_dyg1;
  JA(3,9)=dx1/r1;
  JA(3,10)=dpxg2_dxg2;
  JA(3,11)=dpxg2_dyg2;
  JA(3,12)=dx2/r2;

  JA(4,0)=dpyg1_dx1+dpyg2_dx1;
  JA(4,1)=dpyg1_dy1+dpyg2_dy1;
  JA(4,2)=dpyg1_dz1+dpyg2_dz1;
  JA(4,7)=dpyg1_dxg1;
  JA(4,8)=dpyg1_dyg1;
  JA(4,9)=dy1/r1;
  JA(4,10)=dpyg2_dxg2;
  JA(4,11)=dpyg2_dyg2;
  JA(4,12)=dy2/r2;

  JA(5,0)=dpzg1_dx1+dpzg2_dx1;
  JA(5,1)=dpzg1_dy1+dpzg2_dy1;
  JA(5,2)=dpzg1_dz1+dpzg2_dz1;
  JA(5,7)=dpzg1_dxg1;
  JA(5,8)=dpzg1_dyg1;
  JA(5,9)=dz1/r1;
  JA(5,10)=dpzg2_dxg2;
  JA(5,11)=dpzg2_dyg2;
  JA(5,12)=dz2/r2;

  JA(6,9)=1.;
  JA(6,12)=1.;

  JA(7,0)=-(pxg1+pxg2)/mpair*dpxg1_dx1 -(pyg1+pyg2)/mpair*dpyg1_dx1 -(pzg1+pzg2)/mpair*dpzg1_dx1
    -(pxg1+pxg2)/mpair*dpxg2_dx1 -(pyg1+pyg2)/mpair*dpyg2_dx1 -(pzg1+pzg2)/mpair*dpzg2_dx1;
  JA(7,1)=-(pxg1+pxg2)/mpair*dpxg1_dy1 -(pyg1+pyg2)/mpair*dpyg1_dy1 -(pzg1+pzg2)/mpair*dpzg1_dy1
    -(pxg1+pxg2)/mpair*dpxg2_dy1 -(pyg1+pyg2)/mpair*dpyg2_dy1 -(pzg1+pzg2)/mpair*dpzg2_dy1;
  JA(7,2)=-(pxg1+pxg2)/mpair*dpxg1_dz1 -(pyg1+pyg2)/mpair*dpyg1_dz1 -(pzg1+pzg2)/mpair*dpzg1_dz1
    -(pxg1+pxg2)/mpair*dpxg2_dz1 -(pyg1+pyg2)/mpair*dpyg2_dz1 -(pzg1+pzg2)/mpair*dpzg2_dz1;

  JA(7,7)=-(pxg1+pxg2)/mpair*dpxg1_dxg1 -(pyg1+pyg2)/mpair*dpyg1_dxg1 -(pzg1+pzg2)/mpair*dpzg1_dxg1;
  JA(7,8)=-(pxg1+pxg2)/mpair*dpxg1_dyg1 -(pyg1+pyg2)/mpair*dpyg1_dyg1 -(pzg1+pzg2)/mpair*dpzg1_dyg1;
  JA(7,9)=(eg1+eg2)/mpair-(pxg1+pxg2)/mpair*dx1/r1-(pyg1+pyg2)/mpair*dy1/r1-(pzg1+pzg2)/mpair*dz1/r1;

  JA(7,10)=-(pxg1+pxg2)/mpair*dpxg2_dxg2 -(pyg1+pyg2)/mpair*dpyg2_dxg2 -(pzg1+pzg2)/mpair*dpzg2_dxg2;
  JA(7,11)=-(pxg1+pxg2)/mpair*dpxg2_dyg2 -(pyg1+pyg2)/mpair*dpyg2_dyg2 -(pzg1+pzg2)/mpair*dpzg2_dyg2;
  JA(7,12)=(eg1+eg2)/mpair-(pxg1+pxg2)/mpair*dx2/r2-(pyg1+pyg2)/mpair*dy2/r2-(pzg1+pzg2)/mpair*dz2/r2;

  Cnew = ROOT::Math::Similarity<double,8,13>(JA, Cold);

  //not allowed becasue pair is const LHCb::Particle
  //dau->setMeasuredMass(mpair);
  //dau->measuredMassErr(sqrt(Cnew(7,7)));

  double nominalMass=0.0;
  bool constrainM=requireMassConstraint(dau, nominalMass);

  if ( constrainM )
  {
    ROOT::Math::SMatrix<double,1,8> DD;
    Gaudi::Vector1 dd;
    DD(0,7)=1.;
    dd[0]=-nominalMass;
    Gaudi::SymMatrix1x1 Cd = ROOT::Math::Similarity<double, 1, 8>(DD,Cnew);
    if ( !Cd.Invert() ) {
      if (msgLevel(MSG::DEBUG))
        debug() << "could not invert matrix Cd in addPhotonPair" << endmsg;
      return StatusCode::FAILURE;
    }
    Vnew-= Cnew*ROOT::Math::Transpose(DD)*Cd*(DD*Vnew+dd);
    Gaudi::SymMatrix8x8 deltaCnew1=ROOT::Math::SimilarityT<double,1,8>(DD,Cd);
    Gaudi::SymMatrix8x8 deltaCnew2=ROOT::Math::Similarity<double,8,8>(Cnew,deltaCnew1);
    Cnew -= deltaCnew2;

    for(int i=0;i<=7;i++) Cnew(7,i)=0.0;
  }

  Gaudi::Vector7 V7final;
  Gaudi::SymMatrix7x7 C7final;
  const double chi2final = chi2 ;
  const int NDoFfinal = NDoF;

  for(int i=0;i<7;++i) V7final(i)=Vnew(i);
  for(int l1=0; l1<7; ++l1)
    for(int l2=0; l2<=l1; ++l2) C7final(l1,l2) = Cnew(l1,l2);

  addToOutgoingParticles(*(part.endVertex()),gamma1);
  addToOutgoingParticles(*(part.endVertex()),gamma2);
  sc = updateParticle(part, V7final, C7final, chi2final, NDoFfinal);

  return sc;
}

//==================================================================
//  method to add a meregd pi0
//==================================================================
StatusCode OfflineVertexFitter::addMergedPi0(LHCb::Particle& part,
                                             const LHCb::Particle * dau) const
{
  StatusCode sc = StatusCode::SUCCESS;

  Gaudi::Vector7 V7;
  Gaudi::SymMatrix7x7 C7;
  double chi2 = 0.;
  int NDoF = 0;
  getParticleInfo(part, V7, C7, chi2, NDoF);

  double zg1=-9999.;
  Gaudi::Vector3 gamma1para;
  Gaudi::SymMatrix3x3 gamma1cov;

  double zg2=-9999.;
  Gaudi::Vector3 gamma2para;
  Gaudi::SymMatrix3x3 gamma2cov;

  sc = getMergedPi0Parameter(*dau, zg1, gamma1para, gamma1cov, zg2, gamma2para, gamma2cov);
  if ( sc.isFailure() )
  {
    if (msgLevel(MSG::DEBUG))
      debug() << "Fail to getMeregdPi0Parameter in  addMeregdPi0Pair" << endmsg;
    return StatusCode::FAILURE;
  }

  Gaudi::Vector8 Vnew;
  Gaudi::SymMatrix8x8 Cnew;

  const double dx1= gamma1para[0] - V7[0];
  const double dy1= gamma1para[1] - V7[1];
  const double dz1= zg1 - V7[2];
  const double r1= std::sqrt(dx1*dx1+dy1*dy1+dz1*dz1);
  const double eg1= gamma1para[2];
  const double pxg1= eg1*dx1/r1;
  const double pyg1= eg1*dy1/r1;
  const double pzg1= eg1*dz1/r1;

  const double dx2= gamma2para[0] - V7[0];
  const double dy2= gamma2para[1] - V7[1];
  const double dz2= zg2 - V7[2];
  const double r2= std::sqrt(dx2*dx2+dy2*dy2+dz2*dz2);
  const double eg2= gamma2para[2];
  const double pxg2= eg2*dx2/r2;
  const double pyg2= eg2*dy2/r2;
  const double pzg2= eg2*dz2/r2;

  const double mpair= std::sqrt((eg1+eg2)*(eg1+eg2)-(pxg1+pxg2)*(pxg1+pxg2)-(pyg1+pyg2)*(pyg1+pyg2)-(pzg1+pzg2)*(pzg1+pzg2));

  Vnew[0]= V7[0];
  Vnew[1]= V7[1];
  Vnew[2]= V7[2];
  Vnew[3]= V7[3] + pxg1 + pxg2;
  Vnew[4]= V7[4] + pyg1 + pyg2;
  Vnew[5]= V7[5] + pzg1 + pzg2;
  Vnew[6]= V7[6] + eg1  + eg2;
  Vnew[7]= mpair;

  SymMatrix13x13 Cold;

  for(int l1=0; l1<=6; ++l1)
    for (int l2=0; l2<=l1; ++l2) Cold(l1,l2) = C7(l1,l2);

  for(int l1=0; l1<=2; ++l1)
    for (int l2=0; l2<=l1; ++l2) {
      Cold(l1+7,l2+7) = gamma1cov(l1,l2);
      Cold(l1+10,l2+10) = gamma2cov(l1,l2);
    }

  for(int l1=7; l1<=9;++l1)
    for(int l2=0; l2<7; ++l2) Cold(l1,l2)=0.0;

  for(int l1=10; l1<=12;++l1)
    for(int l2=0; l2<10; ++l2) Cold(l1,l2)=0.0;

  ROOT::Math::SMatrix<double, 8, 13> JA;
  for(int i=0;i<7;i++) JA(i,i)=1.;

  const double dinvr1_dx1=-1./(r1*r1)*(-dx1/r1);
  const double dinvr1_dy1=-1./(r1*r1)*(-dy1/r1);
  const double dinvr1_dz1=-1./(r1*r1)*(-dz1/r1);
  const double dinvr1_dxg1=-1./(r1*r1)*(dx1/r1);
  const double dinvr1_dyg1=-1./(r1*r1)*(dy1/r1);

  const double dinvr2_dx1=-1./(r2*r2)*(-dx2/r2);
  const double dinvr2_dy1=-1./(r2*r2)*(-dy2/r2);
  const double dinvr2_dz1=-1./(r2*r2)*(-dz2/r2);
  const double dinvr2_dxg2=-1./(r2*r2)*(dx2/r2);
  const double dinvr2_dyg2=-1./(r2*r2)*(dy2/r2);

  const double dpxg1_dx1=eg1*(-1./r1+dx1*dinvr1_dx1);
  const double dpyg1_dx1=eg1*dy1*dinvr1_dx1;
  const double dpzg1_dx1=eg1*dz1*dinvr1_dx1;
  const double dpxg2_dx1=eg2*(-1./r2+dx2*dinvr2_dx1);
  const double dpyg2_dx1=eg2*dy2*dinvr2_dx1;
  const double dpzg2_dx1=eg2*dz2*dinvr2_dx1;

  const double dpxg1_dy1=eg1*dx1*dinvr1_dy1;
  const double dpyg1_dy1=eg1*(-1./r1+dy1*dinvr1_dy1);
  const double dpzg1_dy1=eg1*dz1*dinvr1_dy1;
  const double dpxg2_dy1=eg2*dx2*dinvr2_dy1;
  const double dpyg2_dy1=eg2*(-1./r2+dy2*dinvr2_dy1);
  const double dpzg2_dy1=eg2*dz2*dinvr2_dy1;

  const double dpxg1_dz1=eg1*dx1*dinvr1_dz1;
  const double dpyg1_dz1=eg1*dy1*dinvr1_dz1;
  const double dpzg1_dz1=eg1*(-1./r1+dz1*dinvr1_dz1);
  const double dpxg2_dz1=eg2*dx2*dinvr2_dz1;
  const double dpyg2_dz1=eg2*dy2*dinvr2_dz1;
  const double dpzg2_dz1=eg2*(-1./r2+dz2*dinvr2_dz1);

  const double dpxg1_dxg1=eg1*(1./r1+dx1*dinvr1_dxg1);
  const double dpyg1_dxg1=eg1*dy1*dinvr1_dxg1;
  const double dpzg1_dxg1=eg1*dz1*dinvr1_dxg1;

  const double dpxg1_dyg1=eg1*dx1*dinvr1_dyg1;
  const double dpyg1_dyg1=eg1*(1./r1+dy1*dinvr1_dyg1);
  const double dpzg1_dyg1=eg1*dz1*dinvr1_dyg1;

  const double dpxg2_dxg2=eg2*(1./r2+dx2*dinvr2_dxg2);
  const double dpyg2_dxg2=eg2*dy2*dinvr2_dxg2;
  const double dpzg2_dxg2=eg2*dz2*dinvr2_dxg2;

  const double dpxg2_dyg2=eg2*dx2*dinvr2_dyg2;
  const double dpyg2_dyg2=eg2*(1./r2+dy2*dinvr2_dyg2);
  const double dpzg2_dyg2=eg2*dz2*dinvr2_dyg2;

  JA(3,0)=dpxg1_dx1+dpxg2_dx1;
  JA(3,1)=dpxg1_dy1+dpxg2_dy1;
  JA(3,2)=dpxg1_dz1+dpxg2_dz1;
  JA(3,7)=dpxg1_dxg1;
  JA(3,8)=dpxg1_dyg1;
  JA(3,9)=dx1/r1;
  JA(3,10)=dpxg2_dxg2;
  JA(3,11)=dpxg2_dyg2;
  JA(3,12)=dx2/r2;

  JA(4,0)=dpyg1_dx1+dpyg2_dx1;
  JA(4,1)=dpyg1_dy1+dpyg2_dy1;
  JA(4,2)=dpyg1_dz1+dpyg2_dz1;
  JA(4,7)=dpyg1_dxg1;
  JA(4,8)=dpyg1_dyg1;
  JA(4,9)=dy1/r1;
  JA(4,10)=dpyg2_dxg2;
  JA(4,11)=dpyg2_dyg2;
  JA(4,12)=dy2/r2;

  JA(5,0)=dpzg1_dx1+dpzg2_dx1;
  JA(5,1)=dpzg1_dy1+dpzg2_dy1;
  JA(5,2)=dpzg1_dz1+dpzg2_dz1;
  JA(5,7)=dpzg1_dxg1;
  JA(5,8)=dpzg1_dyg1;
  JA(5,9)=dz1/r1;
  JA(5,10)=dpzg2_dxg2;
  JA(5,11)=dpzg2_dyg2;
  JA(5,12)=dz2/r2;

  JA(6,9)=1.;
  JA(6,12)=1.;

  JA(7,0)=-(pxg1+pxg2)/mpair*dpxg1_dx1 -(pyg1+pyg2)/mpair*dpyg1_dx1 -(pzg1+pzg2)/mpair*dpzg1_dx1
    -(pxg1+pxg2)/mpair*dpxg2_dx1 -(pyg1+pyg2)/mpair*dpyg2_dx1 -(pzg1+pzg2)/mpair*dpzg2_dx1;
  JA(7,1)=-(pxg1+pxg2)/mpair*dpxg1_dy1 -(pyg1+pyg2)/mpair*dpyg1_dy1 -(pzg1+pzg2)/mpair*dpzg1_dy1
    -(pxg1+pxg2)/mpair*dpxg2_dy1 -(pyg1+pyg2)/mpair*dpyg2_dy1 -(pzg1+pzg2)/mpair*dpzg2_dy1;
  JA(7,2)=-(pxg1+pxg2)/mpair*dpxg1_dz1 -(pyg1+pyg2)/mpair*dpyg1_dz1 -(pzg1+pzg2)/mpair*dpzg1_dz1
    -(pxg1+pxg2)/mpair*dpxg2_dz1 -(pyg1+pyg2)/mpair*dpyg2_dz1 -(pzg1+pzg2)/mpair*dpzg2_dz1;

  JA(7,7)=-(pxg1+pxg2)/mpair*dpxg1_dxg1 -(pyg1+pyg2)/mpair*dpyg1_dxg1 -(pzg1+pzg2)/mpair*dpzg1_dxg1;
  JA(7,8)=-(pxg1+pxg2)/mpair*dpxg1_dyg1 -(pyg1+pyg2)/mpair*dpyg1_dyg1 -(pzg1+pzg2)/mpair*dpzg1_dyg1;
  JA(7,9)=(eg1+eg2)/mpair-(pxg1+pxg2)/mpair*dx1/r1-(pyg1+pyg2)/mpair*dy1/r1-(pzg1+pzg2)/mpair*dz1/r1;

  JA(7,10)=-(pxg1+pxg2)/mpair*dpxg2_dxg2 -(pyg1+pyg2)/mpair*dpyg2_dxg2 -(pzg1+pzg2)/mpair*dpzg2_dxg2;
  JA(7,11)=-(pxg1+pxg2)/mpair*dpxg2_dyg2 -(pyg1+pyg2)/mpair*dpyg2_dyg2 -(pzg1+pzg2)/mpair*dpzg2_dyg2;
  JA(7,12)=(eg1+eg2)/mpair-(pxg1+pxg2)/mpair*dx2/r2-(pyg1+pyg2)/mpair*dy2/r2-(pzg1+pzg2)/mpair*dz2/r2;

  Cnew=ROOT::Math::Similarity<double,8,13>(JA, Cold);

  //not allowed becasue pair is const LHCb::Particle
  //dau->setMeasuredMass(mpair);
  //dau->measuredMassErr(sqrt(Cnew(7,7)));

  double nominalMass=0.0;
  bool constrainM=requireMassConstraint(dau, nominalMass);

  if(constrainM) {
    ROOT::Math::SMatrix<double,1,8> DD;
    Gaudi::Vector1 dd;
    DD(0,7)=1.;
    dd[0]=-nominalMass;
    Gaudi::SymMatrix1x1 Cd = ROOT::Math::Similarity<double, 1, 8>(DD,Cnew);
    if ( !Cd.Invert() )
    {
      if (msgLevel(MSG::DEBUG))
        debug() << "could not invert matrix Cd in addPhotonPair" << endmsg;
      return StatusCode::FAILURE;
    }
    Vnew-= Cnew*ROOT::Math::Transpose(DD)*Cd*(DD*Vnew+dd);
    Gaudi::SymMatrix8x8 deltaCnew1=ROOT::Math::SimilarityT<double,1,8>(DD,Cd);
    Gaudi::SymMatrix8x8 deltaCnew2=ROOT::Math::Similarity<double,8,8>(Cnew,deltaCnew1);
    Cnew -= deltaCnew2;

    for ( int i=0;i<=7;++i) Cnew(7,i)=0.0;
  }

  Gaudi::Vector7 V7final;
  Gaudi::SymMatrix7x7 C7final;
  const double chi2final = chi2 ;
  const int NDoFfinal = NDoF;

  for ( int i=0;i<7;++i) V7final(i)=Vnew(i);
  for ( int l1=0; l1<7; ++l1 )
    for ( int l2=0; l2<=l1; ++l2 ) C7final(l1,l2) = Cnew(l1,l2);

  addToOutgoingParticles(*(part.endVertex()), dau);
  sc = updateParticle(part, V7final, C7final, chi2final, NDoFfinal);

  return sc;
}

//=============================================================================
// Fit two flying particles
//=============================================================================
StatusCode OfflineVertexFitter::fitTwo(const LHCb::Particle* dau1,
                                       const LHCb::Particle* dau2,
                                       Gaudi::Vector7& V7,
                                       Gaudi::SymMatrix7x7& C7,
                                       double& chi2) const
{

  StatusCode sc = StatusCode::SUCCESS;

  double nominalM1=0.0;
  const bool constrainM1=requireMassConstraint(dau1,nominalM1);

  double nominalM2=0.0;
  const bool constrainM2=requireMassConstraint(dau2,nominalM2);

  double zfit=getZEstimate(dau1,dau2);
  double zPreviousFit=-999999.;
  //  zfit=300.;

  int iterTransport=0;

  while ( fabs(zPreviousFit-zfit)>m_maxDeltaZ &&
          iterTransport< m_maxIter )
  {

    if ( msgLevel(MSG::VERBOSE) )
      verbose() << "Fit z position is now " << zfit << endmsg;

    zPreviousFit = zfit;
    ++iterTransport;

    LHCb::Particle transParticle1;
    sc = m_transporter->transportAndProject(dau1,
                                            zPreviousFit,
                                            transParticle1);
    if ( sc.isFailure() )
    {
      if (msgLevel(MSG::DEBUG))
        debug() << "transport of dau1 failed in fitTwo!" << endmsg;
      return sc;
    }

    LHCb::Particle transParticle2;
    sc = m_transporter->transportAndProject(dau2,
                                            zPreviousFit,
                                            transParticle2);
    if ( sc.isFailure() )
    {
      if (msgLevel(MSG::DEBUG))
        debug() << "transport of dau2 failed in fitTwo!" << endmsg;
      return sc;
    }

    Gaudi::Vector7 dau1para;
    Gaudi::SymMatrix7x7 dau1cov;
    double dau1chi2 = 0.;
    int dau1NDoF = 0;
    getParticleInfo(transParticle1, dau1para, dau1cov, dau1chi2, dau1NDoF);
    if ( constrainM1 )
    {
      sc = constrainMass(dau1para, dau1cov, nominalM1);
      if ( sc.isFailure() ) return sc;
    }
    Gaudi::Vector7 dau1mpara;
    Gaudi::SymMatrix7x7 dau1mcov;
    //    dau1cov=dau1->covMatrix()  ;  // trick
    convertE2M(dau1para,dau1cov, dau1mpara, dau1mcov);
    const int id1=abs(dau1->particleID().pid());
    if ( id1==11 || id1==13 || id1==211 || id1==321 || id1==2212 )
    {
      for ( int k=0; k<=6; ++k ) dau1mcov(6,k)=0.0;
    }

    Gaudi::Vector7 dau2para;
    Gaudi::SymMatrix7x7 dau2cov;
    double dau2chi2 = 0.;
    int dau2NDoF = 0;
    getParticleInfo( transParticle2, dau2para, dau2cov, dau2chi2, dau2NDoF );
    if ( constrainM2 )
    {
      sc = constrainMass(dau2para, dau2cov, nominalM2);
      if ( sc.isFailure() ) return sc;
    }
    Gaudi::Vector7 dau2mpara;
    Gaudi::SymMatrix7x7 dau2mcov;
    //    dau2cov=dau2->covMatrix()  ;  // trick
    convertE2M(dau2para,dau2cov, dau2mpara, dau2mcov);
    const int id2=abs(dau2->particleID().pid());
    if ( id2==11 || id2==13 || id2==211 || id2==321 || id2==2212 )
    {
      for ( int k=0; k<=6; ++k ) dau2mcov(6,k)=0.0;
    }

    //temporary fix for MCParticleMaker
    //for(int i=0;i<=6;i++) {dau1mcov(6,i)=0.0; dau2mcov(6,i)=0.0;}

    ROOT::Math::SVector<double, 12> X;
    X[0]=dau1mpara[0];
    X[1]=dau1mpara[1];
    X[2]=dau1mpara[3];
    X[3]=dau1mpara[4];
    X[4]=dau1mpara[5];
    X[5]=dau1mpara[6];
    X[6]=dau2mpara[0];
    X[7]=dau2mpara[1];
    X[8]=dau2mpara[3];
    X[9]=dau2mpara[4];
    X[10]=dau2mpara[5];
    X[11]=dau2mpara[6];

    if ( msgLevel(MSG::VERBOSE) )
      verbose() << "X vector is " << X << endmsg ;

    Gaudi::SymMatrix6x6 newcov1;
    Gaudi::SymMatrix6x6 newcov2;
    for ( int l1=0; l1<6; ++l1 )
    {
      int n1=l1;
      if (n1>=2) ++n1;
      for ( int l2=0; l2<=l1; ++l2 )
      {
        int n2=l2;
        if ( n2>=2 ) ++n2;
        newcov1(l1,l2)=dau1mcov(n1,n2);
        newcov2(l1,l2)=dau2mcov(n1,n2);
      }
    }

    SymMatrix12x12 Cx;
    for ( int l1 = 0; l1<6; ++l1 )
    {
      for(int l2 = 0; l2<=l1; ++l2 )
      {
        Cx(l1,l2)=newcov1(l1,l2);
        Cx(l1+6,l2+6)=newcov2(l1,l2);
      }
      for ( int l2=0; l2<6; ++l2 )  Cx(l1+6,l2)=0.0;
    }


    //(x1,y1,px1,py1,pz1,m1,x2,y2,px2,py2,pz2,m2) at the same z
    ROOT::Math::SVector<double, 12> vfit=X;
    SymMatrix12x12 cfit = Cx;

    bool converged=false;
    int iter=0;

    double chi2Previous=9999.;
    chi2=999.;

    while ( !converged && iter< m_maxIter )
    {
      ++iter;

      if ( msgLevel(MSG::VERBOSE) )
        verbose() << ":-) Iteration   " << iter << endmsg;

      //f=(x2-x1)*(py2*pz1-py1*pz2)-(y2-y1)*(px2*pz1-px1*pz2)
      ROOT::Math::SVector<double, 1> f;
      f(0)=(vfit(6)-vfit(0))*(vfit(9)*vfit(4)-vfit(3)*vfit(10))-
        (vfit(7)-vfit(1))*(vfit(8)*vfit(4)-vfit(2)*vfit(10));

      if ( msgLevel(MSG::VERBOSE) )
        verbose() << "constraint values   " << f(0) << endmsg;

      //D is the derivative matrix
      ROOT::Math::SMatrix<double, 1, 12> D;
      D(0,0)=-(vfit(9)*vfit(4)-vfit(3)*vfit(10));
      D(0,1)=(vfit(8)*vfit(4)-vfit(2)*vfit(10));
      D(0,2)=(vfit(7)-vfit(1))*vfit(10);
      D(0,3)=-(vfit(6)-vfit(0))*vfit(10);
      D(0,4)=(vfit(6)-vfit(0))*vfit(9)-(vfit(7)-vfit(1))*vfit(8);
      D(0,5)=0.0;
      D(0,6)=(vfit(9)*vfit(4)-vfit(3)*vfit(10));
      D(0,7)=-(vfit(8)*vfit(4)-vfit(2)*vfit(10));
      D(0,8)=-(vfit(7)-vfit(1))*vfit(4);
      D(0,9)=(vfit(6)-vfit(0))*vfit(4);
      D(0,10)=-(vfit(6)-vfit(0))*vfit(3)+(vfit(7)-vfit(1))*vfit(2);
      D(0,11)=0.0;

      const ROOT::Math::SVector<double, 1> d = f - D*vfit;

      Gaudi::SymMatrix1x1 VD = ROOT::Math::Similarity<double,1,12>(D, Cx);

      if ( !VD.Invert() )
      {
        if ( msgLevel(MSG::DEBUG) )
          debug() << "could not invert matrix VD in fitTwo! " <<endmsg;
        return StatusCode::FAILURE;
      }

      const ROOT::Math::SVector<double, 1> alpha=D*X+d;

      const ROOT::Math::SVector<double, 1> lambda=VD*alpha;

      const ROOT::Math::SMatrix<double, 12,1> DT = ROOT::Math::Transpose(D);
      vfit=X-Cx*DT*lambda;

      const SymMatrix12x12 delataC1=ROOT::Math::Similarity<double,12,1>(DT, VD);

      const SymMatrix12x12 delataC2=ROOT::Math::Similarity<double,12,12>(Cx, delataC1);

      cfit=Cx -delataC2;

      chi2=ROOT::Math::Dot(alpha,lambda);
      //chi2+= 2*ROOT::Math::Dot(lambda,f);

      if ( fabs(chi2-chi2Previous) < m_maxDeltaChi2 )
      {
        converged = true;
      }
      else
      {
        chi2Previous = chi2;
      }

    } // end chi2 minimization iteration

    if (!converged)  return StatusCode::FAILURE;

    V7(0)=vfit(0)-vfit(2)*(vfit(6)-vfit(0))*vfit(10)/(vfit(8)*vfit(4)-vfit(2)*vfit(10));
    V7(1)=vfit(1)-vfit(3)*(vfit(7)-vfit(1))*vfit(10)/(vfit(9)*vfit(4)-vfit(3)*vfit(10));
    V7(2)=zPreviousFit-(vfit(6)-vfit(0))*vfit(4)*vfit(10)/(vfit(8)*vfit(4)-vfit(2)*vfit(10));
    V7(3)=vfit(2)+vfit(8);
    V7(4)=vfit(3)+vfit(9);
    V7(5)=vfit(4)+vfit(10);
    const double e1=std::sqrt(vfit(2)*vfit(2)+vfit(3)*vfit(3)+vfit(4)*vfit(4)+vfit(5)*vfit(5));
    const double e2=std::sqrt(vfit(8)*vfit(8)+vfit(9)*vfit(9)+vfit(10)*vfit(10)+vfit(11)*vfit(11));
    V7(6)=e1+e2;

    ROOT::Math::SMatrix<double, 7, 12> JA;
    JA(0,0)=1.+vfit(2)*vfit(10)/(vfit(8)*vfit(4)-vfit(2)*vfit(10));
    JA(0,2)=-(vfit(6)-vfit(0))*vfit(10)/(vfit(8)*vfit(4)-vfit(2)*vfit(10))
      -vfit(2)*(vfit(6)-vfit(0))*vfit(10)/(vfit(8)*vfit(4)-vfit(2)*vfit(10))
      /(vfit(8)*vfit(4)-vfit(2)*vfit(10))*vfit(10);
    JA(0,4)=vfit(2)*(vfit(6)-vfit(0))*vfit(10)/(vfit(8)*vfit(4)-vfit(2)*vfit(10))
      /(vfit(8)*vfit(4)-vfit(2)*vfit(10))*vfit(8);
    JA(0,6)=-vfit(2)*vfit(10)/(vfit(8)*vfit(4)-vfit(2)*vfit(10));
    JA(0,8)=vfit(2)*(vfit(6)-vfit(0))*vfit(10)/(vfit(8)*vfit(4)-vfit(2)*vfit(10))
      /(vfit(8)*vfit(4)-vfit(2)*vfit(10))*vfit(4);
    JA(0,10)=-vfit(2)*(vfit(6)-vfit(0))/(vfit(8)*vfit(4)-vfit(2)*vfit(10))
      -vfit(2)*(vfit(6)-vfit(0))*vfit(10)/(vfit(8)*vfit(4)-vfit(2)*vfit(10))
      /(vfit(8)*vfit(4)-vfit(2)*vfit(10))*vfit(2);

    JA(1,1)=1.+vfit(3)*vfit(10)/(vfit(9)*vfit(4)-vfit(3)*vfit(10));
    JA(1,3)=-(vfit(7)-vfit(1))*vfit(10)/(vfit(9)*vfit(4)-vfit(3)*vfit(10))
      -vfit(3)*(vfit(7)-vfit(1))*vfit(10)/(vfit(9)*vfit(4)-vfit(3)*vfit(10))
      /(vfit(9)*vfit(4)-vfit(3)*vfit(10))*vfit(10);
    JA(1,4)=vfit(3)*(vfit(7)-vfit(1))*vfit(10)/(vfit(9)*vfit(4)-vfit(3)*vfit(10))
      /(vfit(9)*vfit(4)-vfit(3)*vfit(10))*vfit(9);
    JA(1,7)=-vfit(3)*vfit(10)/(vfit(9)*vfit(4)-vfit(3)*vfit(10));
    JA(1,9)=vfit(3)*(vfit(7)-vfit(1))*vfit(10)/(vfit(9)*vfit(4)-vfit(3)*vfit(10))
      /(vfit(9)*vfit(4)-vfit(3)*vfit(10))*vfit(4);
    JA(1,10)=-vfit(3)*(vfit(7)-vfit(1))/(vfit(9)*vfit(4)-vfit(3)*vfit(10))
      -vfit(3)*(vfit(7)-vfit(1))*vfit(10)/(vfit(9)*vfit(4)-vfit(3)*vfit(10))
      /(vfit(9)*vfit(4)-vfit(3)*vfit(10))*vfit(3);

    JA(2,0)=vfit(4)*vfit(10)/(vfit(8)*vfit(4)-vfit(2)*vfit(10));
    JA(2,2)=-(vfit(6)-vfit(0))*vfit(4)*vfit(10)/(vfit(8)*vfit(4)-vfit(2)*vfit(10))
      /(vfit(8)*vfit(4)-vfit(2)*vfit(10))*vfit(10);
    JA(2,4)=-(vfit(6)-vfit(0))*vfit(10)/(vfit(8)*vfit(4)-vfit(2)*vfit(10))
      +(vfit(6)-vfit(0))*vfit(4)*vfit(10)/(vfit(8)*vfit(4)-vfit(2)*vfit(10))
      /(vfit(8)*vfit(4)-vfit(2)*vfit(10))*vfit(8);
    JA(2,6)=-vfit(4)*vfit(10)/(vfit(8)*vfit(4)-vfit(2)*vfit(10));
    JA(2,8)=(vfit(6)-vfit(0))*vfit(4)*vfit(10)/(vfit(8)*vfit(4)-vfit(2)*vfit(10))
      /(vfit(8)*vfit(4)-vfit(2)*vfit(10))*vfit(4);
    JA(2,10)=-(vfit(6)-vfit(0))*vfit(4)/(vfit(8)*vfit(4)-vfit(2)*vfit(10))
      -(vfit(6)-vfit(0))*vfit(4)*vfit(10)/(vfit(8)*vfit(4)-vfit(2)*vfit(10))
      /(vfit(8)*vfit(4)-vfit(2)*vfit(10))*vfit(2);

    JA(3,2)=1.;
    JA(3,8)=1.;

    JA(4,3)=1.;
    JA(4,9)=1.;

    JA(5,4)=1.;
    JA(5,10)=1.;

    JA(6,2)=vfit(2)/e1;
    JA(6,3)=vfit(3)/e1;
    JA(6,4)=vfit(4)/e1;
    JA(6,5)=vfit(5)/e1;
    JA(6,8)=vfit(8)/e2;
    JA(6,9)=vfit(9)/e2;
    JA(6,10)=vfit(10)/e2;
    JA(6,11)=vfit(11)/e2;

    C7=ROOT::Math::Similarity<double,7,12>(JA, cfit);

    zfit=V7(2);

  } // end transport iteration

  if ( fabs(zPreviousFit-zfit) > m_maxDeltaZ ) return StatusCode::FAILURE;
  if ( C7(2,2) < 0                           ) return StatusCode::FAILURE;

  return sc;
}

//=============================================================================
// count the number of daughters that will contribute to vertexing
//=============================================================================
int OfflineVertexFitter::countTraj(const LHCb::Particle* part) const
{
  int nt = 0;
  if ( part->isBasicParticle() )
  {
    const int id = part->particleID().pid();
    if ( id != m_photonID && id != m_pi0ID ) ++nt;
  }
  else
  {
    if ( !isResonance(part) )
    {
      ++nt;
    }
    else
    {
      const Particle::ConstVector& Prods = part->daughtersVector();
      for ( Particle::ConstVector::const_iterator iProd = Prods.begin();
            iProd != Prods.end(); ++iProd )
      {
        nt += countTraj( *iProd );
      }
    }
  }
  return nt;
}

//=============================================================================
// update particle momentum and vertex information
//=============================================================================
StatusCode OfflineVertexFitter::updateParticle( LHCb::Particle& part,
                                                const Gaudi::Vector7& V7,
                                                const Gaudi::SymMatrix7x7& C7,
                                                const double chi2,
                                                const int NDoF ) const
{
  StatusCode sc = StatusCode::SUCCESS;

  const Gaudi::XYZPoint refPoint(V7[0], V7[1], V7[2]);

  // @todo JPP check LorentzVector constructors vis a vis CLHEP ones!
  const Gaudi::LorentzVector lmom(V7[3], V7[4], V7[5], V7[6]);

  // JPP this is the translation.
  const Gaudi::SymMatrix3x3 posCov    = C7.Sub<Gaudi::SymMatrix3x3>(0,0);
  const Gaudi::SymMatrix4x4 momCov    = C7.Sub<Gaudi::SymMatrix4x4>(3,3);
  const Gaudi::Matrix4x3    posMomCov = C7.Sub<Gaudi::Matrix4x3>(3,0);

  Gaudi::Vector7 Vm7;
  Gaudi::SymMatrix7x7 Cm7;
  convertE2M( V7, C7, Vm7, Cm7 );

  const double measuredMass = Vm7[6];
  double measuredMassErr    = Cm7(6,6);

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Vm7 = " << Vm7 << endmsg;
    debug() << "Cm7 = " << Cm7 << endmsg;
  }

  if ( measuredMassErr < -999 )
  {
    // 'Very' negative mass error, so declare fit failed ...
    sc = StatusCode::FAILURE;
  }
  if ( measuredMassErr < 0 )
  {
    if ( msgLevel(MSG::DEBUG) ) 
      debug() << " -> Negative MeasuredMassError^2 = " <<  measuredMassErr 
              << " reset to 0" << endmsg;
    measuredMassErr = 0;
  }

  // CRJ : Stripping packing cannot handle negative errors,
  //       nor do they really make any physical sense ..
  measuredMassErr = std::sqrt( std::fabs(measuredMassErr) );

  LHCb::Vertex * tmpvert = part.endVertex();

  if ( tmpvert )
  {
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

//=============================================================================
// retrieve partile information
//=============================================================================
void OfflineVertexFitter::getParticleInfo( const LHCb::Particle& part,
                                           Gaudi::Vector7& V7,
                                           Gaudi::SymMatrix7x7& C7,
                                           double& chi2,
                                           int& NDoF) const
{
  const LHCb::Vertex* tmpvert = part.endVertex();

  const Gaudi::XYZPoint& refPoint  = part.referencePoint();
  const Gaudi::LorentzVector& lmom = part.momentum();

  V7(0)=refPoint.x();
  V7(1)=refPoint.y();
  V7(2)=refPoint.z();

  V7(3)=lmom.X();
  V7(4)=lmom.Y();
  V7(5)=lmom.Z();
  V7(6)=lmom.T();

  if ( msgLevel(MSG::VERBOSE) )
    verbose() << "A " << part.particleID().pid() << " has V " << V7 << endmsg ;

  C7 = part.covMatrix();

  if ( tmpvert )
  {
    chi2 = tmpvert->chi2();
    NDoF = tmpvert->nDoF();
  }
  else
  {
    chi2 = 0;
    NDoF = 0;
  }
}

//=============================================================================
// conversion of of vector and matrix from energy basis to mass basis
//=============================================================================
void OfflineVertexFitter::convertE2M(const Gaudi::Vector7& V7,
                                     const Gaudi::SymMatrix7x7& C7,
                                     Gaudi::Vector7& Vm7,
                                     Gaudi::SymMatrix7x7& Cm7) const
{
  Gaudi::Matrix7x7 Te2m = ROOT::Math::SMatrixIdentity();

  const double px     = V7[3];
  const double py     = V7[4];
  const double pz     = V7[5];
  const double energy = V7[6];
  const double mass   = std::sqrt( (energy*energy) - (px*px) - (py*py) - (pz*pz) );

  Te2m(6,3) = -px/mass;
  Te2m(6,4) = -py/mass;
  Te2m(6,5) = -pz/mass;
  Te2m(6,6) = energy/mass;

  // JPP translation
  Cm7 = ROOT::Math::Similarity<double,7,7>( Te2m, C7 );

  Vm7    = V7;
  Vm7[6] = mass;
}

//=============================================================================
// conversion of of vector and matrix from mass basis to energy basis
//=============================================================================
void OfflineVertexFitter::convertM2E(const Gaudi::Vector7& Vm7,
                                     const Gaudi::SymMatrix7x7& Cm7,
                                     Gaudi::Vector7& V7,
                                     Gaudi::SymMatrix7x7& C7) const
{
  Gaudi::Matrix7x7 Tm2e = ROOT::Math::SMatrixIdentity();

  double px   = Vm7[3];
  double py   = Vm7[4];
  double pz   = Vm7[5];
  double mass = Vm7[6];
  double energy = std::sqrt(mass*mass+px*px+py*py+pz*pz);

  Tm2e(6,3)=px/energy;
  Tm2e(6,4)=py/energy;
  Tm2e(6,5)=pz/energy;
  Tm2e(6,6)=mass/energy;

  // JPP translation
  C7 = ROOT::Math::Similarity<double,7,7>( Tm2e, Cm7);
  V7=Vm7;
  V7(6)= energy;
}


//=============================================================================
// applying the mass constraint and updating the 7x7 matrix
//=============================================================================
StatusCode OfflineVertexFitter::constrainMass(Gaudi::Vector7& V7,
                                              Gaudi::SymMatrix7x7& C7,
                                              double& nominalMass) const
{

  StatusCode sc = StatusCode::SUCCESS;

  Gaudi::Vector7 Vm;
  Gaudi::SymMatrix7x7 Cm;
  convertE2M(V7,C7,Vm,Cm);

  // CRJ : floating point comparison is not a good idea
  if ( Cm(6,6) != 0.0 )
  {
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
    if ( !Cd.Invert() )
    {
      if ( msgLevel(MSG::DEBUG) )
        debug() << "could not invert matrix Cd in constrainMass" << endmsg;
      return StatusCode::FAILURE;
    }
    Vm -= Cm*ROOT::Math::Transpose(DD)*Cd*(DD*Vm+dd);

    const Gaudi::SymMatrix7x7 deltaCm1=ROOT::Math::SimilarityT<double,1,7>(DD,Cd);
    const Gaudi::SymMatrix7x7 deltaCm2=ROOT::Math::Similarity<double,7,7>(Cm,deltaCm1);
    Cm -= deltaCm2;
  }

  for ( int i=0; i<7; ++i ) Cm(6,i)=0.0;

  convertM2E(Vm,Cm,V7,C7);

  return sc;
}

//=============================================================================
// check if a Particle should be mass constrained or not
//=============================================================================
bool OfflineVertexFitter::requireMassConstraint(const LHCb::Particle* part,
                                                double& nominalMass ) const
{
  bool require=false;
  nominalMass=0.0;

  if ( m_applyDauMassConstraint && !part->isBasicParticle() )
  {
    //const int pid = part->particleID().pid();
    //ParticleProperty*  partProp = m_ppSvc->findByStdHepID( pid  );
    const LHCb::ParticleProperty*  partProp = m_ppSvc->find(part->particleID());

    const double wid = hbar_Planck/partProp->lifetime();
    if ( wid < m_widthThreshold )
    {
      require=true;
      nominalMass=partProp->mass();
    }
  }

  return require;
}

//=============================================================================
// first estimate of z of of two track vertex
//=============================================================================
double OfflineVertexFitter::getZEstimate(const LHCb::Particle* part1,
                                         const LHCb::Particle* part2) const
{
  const double tx1=part1->slopes().X();
  const double ty1=part1->slopes().Y();
  const double tx2=part2->slopes().X();
  const double ty2=part2->slopes().Y();
  const Gaudi::XYZPoint& pos1=part1->referencePoint();
  const double x1= pos1.x();
  const double y1= pos1.y();
  const double z1= pos1.z();
  const Gaudi::XYZPoint& pos2=part2->referencePoint();
  const double x2= pos2.x();
  const double y2= pos2.y();
  const double z2= pos2.z();

  //return (y2-y1+ty1*z1-ty2*z2)/(ty1-ty2);
  const double  sumSquaredSlopes = tx1*tx1 + ty1*ty1+ tx2*tx2 + ty2*ty2;
  const double  sumCrossedProduct = tx1*(x1-tx1*z1) + ty1*(y1-ty1*z1) +
    tx2*(x2-tx2*z2) + ty2*(y2-ty2*z2);

  const double sumX=x1-tx1*z1 + x2-tx2*z2;
  const double sumY=y1-ty1*z1 + y2-ty2*z2;

  const double sumSlopeX= tx1+tx2;
  const double sumSlopeY= ty1+ty2;
  const double det = sumSquaredSlopes - ((sumSlopeX*sumSlopeX + sumSlopeY*sumSlopeY))/2.;
  double zEstimate = 0;
  // CRJ : This floating point comparison isn't a good idea....
  if ( det != 0 )
  {
    zEstimate = (((sumX*sumSlopeX + sumY*sumSlopeY)/2.)
                 - sumCrossedProduct) /det;
  }
  else
  {
    Error ( "Unable to make z estimate" ).ignore() ;
    zEstimate = ( z1<z2 ? z1-.001 : z2-0.001 );
  }
  return zEstimate;
}

//=============================================================================
// get photon (x,y,E) and covariance matrix at zg
//=============================================================================
StatusCode OfflineVertexFitter::getPhotonParameter(const LHCb::Particle& photon,
                                                   double& zg,
                                                   Gaudi::Vector3& para ,
                                                   Gaudi::SymMatrix3x3& cov ) const
{
  StatusCode sc = StatusCode::SUCCESS;

  int pid=photon.particleID().pid();
  if ( pid != m_photonID )
  { return Error ( "Particle is not a photon!" ) ; }

  const LHCb::ProtoParticle*   proto  = photon.proto() ;
  if( 0 == proto  )
  { return Error( "ProtoParticle points to NULL!" ) ; }

  if( proto->calo().empty() )
  { return Error ( "ProtoParticle has no CaloHypos " ) ; }

  const LHCb::CaloHypo* hypo  = *( (proto->calo()).begin() );
  if(LHCb::CaloHypo::Photon != hypo->hypothesis() ) return StatusCode::FAILURE;

  // get the position
  const CaloPosition* pos = hypo->position() ;
  if( 0 == pos    )
  { return Error ( "CaloPosition* points to NULL! " ); }

  zg=pos->z();
  para(0)=pos->x();
  para(1)=pos->y();
  para(2)=pos->e();
  cov=pos -> covariance();

  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() <<"Photon parameters: " <<para<<endmsg;
    verbose() <<"Photon cov : " <<cov<<endmsg;
  }

  return sc;
}

//=============================================================================
// get merged pi0 (x_i,y_i,E_i) and covariance matrix at zg_i
//=============================================================================
StatusCode OfflineVertexFitter::getMergedPi0Parameter(const LHCb::Particle& pi0,
                                                      double& zg1,
                                                      Gaudi::Vector3& para1 ,
                                                      Gaudi::SymMatrix3x3& cov1,
                                                      double& zg2,
                                                      Gaudi::Vector3& para2 ,
                                                      Gaudi::SymMatrix3x3& cov2 ) const
{
  StatusCode sc = StatusCode::SUCCESS;

  const int pid = pi0.particleID().pid();
  if ( pid != m_pi0ID )
  { return Error ( "Particle is not a photon!" ) ; }

  const LHCb::ProtoParticle*   proto  = pi0.proto() ;
  if ( !proto )
  { return Error ( "ProtoParticle points to NULL!" ) ; }

  if ( proto->calo().empty() )
  { return Error ( "ProtoParticle has no CaloHypos " ) ; }

  const LHCb::CaloHypo * hypo = *( ( proto->calo()).begin() );
  if ( LHCb::CaloHypo::Pi0Merged != hypo->hypothesis() ) return StatusCode::FAILURE;

  // Split Photons
  const SmartRefVector<LHCb::CaloHypo>& hypos = hypo->hypos();
  const LHCb::CaloHypo* g1 = *(hypos.begin() );
  const LHCb::CaloHypo* g2 = *(hypos.begin()+1 );

  // get the positions
  const CaloPosition* pos1 = g1->position() ;
  if ( !pos1 )
  { return Error ( "CaloPosition* points to NULL! " ) ; }

  zg1      = pos1->z();
  para1(0) = pos1->x();
  para1(1) = pos1->y();
  para1(2) = pos1->e();
  cov1     = pos1->covariance();
  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() <<"Photon1 parameters: " <<para1<<endmsg;
    verbose() <<"Photon1 cov : " <<cov1<<endmsg;
  }

  const CaloPosition* pos2 = g2->position() ;
  if ( !pos2 )
  { return Error ( "CaloPosition* points to NULL! ") ; }

  zg2      = pos2->z();
  para2(0) = pos2->x();
  para2(1) = pos2->y();
  para2(2) = pos2->e();
  cov2     = pos2->covariance();
  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() <<"Photon2 parameters: " <<para2<<endmsg;
    verbose() <<"Photon2 cov : " <<cov2<<endmsg;
  }

  return sc;
}

//=============================================================================
// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( OfflineVertexFitter )
//=============================================================================
