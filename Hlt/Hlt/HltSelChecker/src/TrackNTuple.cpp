// $Id: TrackNTuple.cpp,v 1.5 2005-03-21 15:11:38 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"

// From LHCb
#include "Event/L1Score.h"
#include "Event/MCVertex.h"
#include "Event/TrgCaloParticle.h"

// from DaVinci
#include "Kernel/IDecayFinder.h"
#include "DaVinciAssociators/Particle2MCLinksAsct.h"

// local
#include "TrackNTuple.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackNTuple
//
// 2005-02-08 : Patrick KOPPENBURG
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<TrackNTuple>          s_factory ;
const        IAlgFactory& TrackNTupleFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackNTuple::TrackNTuple( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator )
    , m_decayFinder()
    , m_pAsct()
    , m_sr()
    , m_pattern()
{
  declareProperty( "UseTruth", m_truth = false ) ;
}
//=============================================================================
// Destructor
//=============================================================================
TrackNTuple::~TrackNTuple() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackNTuple::initialize() {

  debug() << "==> Initialize" << endmsg;

  m_decayFinder = tool<IDecayFinder>("DecayFinder",this);
  if (!m_decayFinder) {
    err() << "Cannot get tool DecayFinder" << endmsg ;
    return StatusCode::FAILURE ;
  }  

  if ( m_truth ){
    m_pAsct = tool<Particle2MCLinksAsct::IAsct>("Particle2MCLinksAsct", this);
    if(!m_pAsct){
      fatal() << "Unable to retrieve the Particle2MCLinks associator" << endreq;
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode TrackNTuple::execute() {

  debug() << "==> Execute" << endmsg;
  setFilterPassed(false);   // Mandatory. Set to true if event is accepted.

  m_sr = NULL ;
  if ( exist<SelResults>(SelResultLocation::Default)){
    m_sr = get<SelResults>(SelResultLocation::Default);
  }
  if (!m_sr) err() << "No selresults" << endmsg;
  m_pattern = bitPattern();

  // get B (also j/psi)
  int nStables = 0 ;
  ParticleVector allB = desktop()->particles();

  StatusCode sc = fillStatsTuple(allB.size(),nStables);
  if (!sc) return sc;
  
  if (!allB.empty()){

    // get stables
    ParticleVector stables;
    StatusCode sc = getStables(allB,stables);
    nStables = stables.size();
    if (!sc) return sc;

    // Fill tuple
    sc = fillTuple(stables);
    if (!sc) return sc;
    setFilterPassed(true);   // Mandatory. Set to true if event is accepted.
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Fill StatsTuple
//=============================================================================
StatusCode TrackNTuple::fillStatsTuple(const int& nB,const int& nT){
  Tuple tuple = nTuple( "PVs" ) ;
  tuple->column( "B" , nB );
  tuple->column( "BTracks" , nT );

  StatusCode sc = fillPV(tuple);
  if (!sc) return sc;

  if (exist<Particles>("Phys/HLTPions/Particles")){
    Particles* all = get<Particles>("Phys/HLTPions/Particles");
    if ( all ) tuple->column( "Tracks", all->size() );
    else tuple->column( "Tracks", 0 );  
  } else tuple->column( "Tracks", -1 );  
  
  sc = fillHlt(tuple);
  if (!sc) return sc ;

  tuple->write();
  return StatusCode::SUCCESS;
}
//=============================================================================
//  get MCPVs
//=============================================================================
std::vector<const MCVertex*> TrackNTuple::getMCPVs(){
  std::vector<const MCVertex*> PVs;
  MCVertices* AllMC = get<MCVertices>(MCVertexLocation::Default);
  for ( MCVertices::const_iterator imc = AllMC->begin() ; 
        imc!= AllMC->end(); ++imc){
    if ((*imc)->type()==MCVertex::ppCollision){
      PVs.push_back(*imc);
    }
  }
  return PVs;
}
//=============================================================================
//  PV
//=============================================================================
StatusCode TrackNTuple::fillPV(Tuple& tuple){

  VertexVector OnPVs = desktop()->primaryVertices(); // online
  std::vector<const MCVertex*> MCPVs = getMCPVs() ;
 
  std::vector<double> x,y,z,tx,ty,tz;
  std::vector<int> rctrue ;
  std::vector<int> pctrue ;
  std::vector<double> dtrue ;
  std::vector<int> products ;
  for ( VertexVector::const_iterator pv = OnPVs.begin(); pv != OnPVs.end(); ++pv){
    verbose() << "PV loop" << endmsg ;
    x.push_back((*pv)->position().x());
    y.push_back((*pv)->position().y());
    z.push_back((*pv)->position().z());
    double dd = 999999.; int i = 0; int rci = -1; int pct = -1; int prod = -1 ;
    double rtx = -999.; double rty = -999.; double rtz = -999.;
    for ( std::vector<const MCVertex*>::const_iterator c = MCPVs.begin(); 
          c != MCPVs.end(); ++c){
      ++i;
      double d = ((*c)->position()-(*pv)->position()).mag();
      if ( d<dd  ){ 
        rci=i ; 
        dd = d ; 
        if ((*c)->collision()) pct = (*c)->collision()->processType();
        prod = ((*c)->products()).size();
        rtx = (*c)->position().x();
        rty = (*c)->position().y();
        rtz = (*c)->position().z();
      } 
    }
    if (rctrue.size()>20) continue;
    rctrue.push_back(rci);
    pctrue.push_back(pct);
    tx.push_back(rtx);
    ty.push_back(rty);
    tz.push_back(rtz);
    dtrue.push_back(dd);
    products.push_back(prod);
    info() << "Found PV  at " << (*pv)->position() << " reconstructing " 
           << rtx << " " << rty << " " << rtz << endmsg ;
  }
  
  verbose() << "Filling PV" << endmsg ;
  //  tuple->column( "OnPVs", OnPVs.size() );
  tuple->farray( "PVx"  , x.begin(),  x.end(), "OnPVs", 20  );
  tuple->farray( "PVy"  , y.begin(),  y.end(), "OnPVs", 20  );
  tuple->farray( "PVz"  , z.begin(),  z.end(), "OnPVs", 20  );
  tuple->farray( "PVT"  , rctrue.begin(),  rctrue.end(), "OnPVs", 20  );
  tuple->farray( "PVTPT", pctrue.begin(),  pctrue.end(), "OnPVs", 20  );
  tuple->farray( "PVTx",  tx.begin(),  tx.end(), "OnPVs", 20  );
  tuple->farray( "PVTy",  ty.begin(),  ty.end(), "OnPVs", 20  );
  tuple->farray( "PVTz",  tz.begin(),  tz.end(), "OnPVs", 20  );
  tuple->farray( "PVTdist",  dtrue.begin(),  dtrue.end(), "OnPVs", 20  );
  tuple->farray( "PVTprod",  products.begin(),  products.end(), "OnPVs", 20  );

  // collisions
  int i = 0 ;
  std::vector<int> reco, ptype;
  std::vector<double> ntx,nty,ntz;
  products.clear();
  for ( std::vector<const MCVertex*>::const_iterator c = MCPVs.begin(); c !=  MCPVs.end(); ++c){
    ++i;
    verbose() << "Collisions loop " << i << endmsg ;
    int rr = -1 ; int j=0; 
    double dd = 999999. ;
    for (std::vector<int>::const_iterator ll = rctrue.begin(); ll!=rctrue.end();++ll){
      ++j;
      if (( *ll == i ) && (dtrue[j-1] < dd)){
        dd = dtrue[j-1] ;
        rr = j; 
      }
    }
    if (reco.size()>20) continue;
    reco.push_back(rr);
    ntx.push_back((*c)->position().x());
    nty.push_back((*c)->position().y());
    ntz.push_back((*c)->position().z());
    products.push_back(((*c)->products()).size());
    if ((*c)->collision()) ptype.push_back(((*c)->collision())->processType());
    else  ptype.push_back(-1);
    if (rr>0) info() << "Collision at " << (*c)->position() << " with " 
                     << ((*c)->products()).size() << " tracks best reconstructed by " 
                     << rr << endmsg ;
    else info() << "Collision at " << (*c)->position()  << " with " 
                << ((*c)->products()).size() << " tracks not reconstructed" << endmsg ;
  }
  verbose() << "Filling Collisions" << endmsg ;
  tuple->farray( "TPVx"  , ntx.begin(),  ntx.end(), "Coll", 20  );
  tuple->farray( "TPVy"  , nty.begin(),  nty.end(), "Coll", 20  );
  tuple->farray( "TPVz"  , ntz.begin(),  ntz.end(), "Coll", 20  );
  tuple->farray( "TPtype", ptype.begin(),  ptype.end(), "Coll", 20  );
  tuple->farray( "TPreco", reco.begin(),  reco.end(), "Coll", 20  );
  tuple->farray( "TPprod",  products.begin(),  products.end(), "Coll", 20  );

  verbose() << "Done" << endmsg ;
  // offline

  Vertices* OffPVs = get<Vertices>( VertexLocation::Primary ); 
  if ( OffPVs ) tuple->column( "OffPVs", OffPVs->size() );
  else tuple->column( "OffPVs", 0. );

  info() << "There are " << OnPVs.size() << " online PVs and " 
           << MCPVs.size() << " collisions" << endmsg ;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Fill HLT
//=============================================================================
StatusCode TrackNTuple::fillHlt(Tuple& tuple){

  StatusCode sc = fillL1Score(tuple);
  if (!sc) return sc;
  tuple->column("Generic" ,  Hlt("CheckHLTGeneric"));
  tuple->column("TwoKHz" ,   Hlt("HLT2kHzFlag"));
  tuple->column("Exclusive", Hlt("HLTExclusiveSelectionsFlag"));
  tuple->column("Dimuons" ,  Hlt("HLTUnbiasedJpsis"));
  tuple->column("Inclusive", Hlt("HLTSingleMuons"));
  tuple->column("Pattern",   m_pattern);
  tuple->column("Dstar" ,    Hlt("HLTFilterDstar"));
  tuple->column("HH" ,       Hlt("HLTFilterB2HH"));
  tuple->column("D0Kstar" ,  Hlt("HLTFilterBd2D0Kstar"));
  tuple->column("DstarPi" ,  Hlt("HLTFilterBd2DstarPi"));
  tuple->column("DsH" ,      Hlt("HLTFilterBs2DsH"));
  tuple->column("PhiPhi" ,   Hlt("HLTFilterBs2PhiPhi"));
  tuple->column("PhiGamma",  Hlt("HLTFilterBs2PhiGamma"));
  tuple->column("MuMu" ,     Hlt("HLTFilterB2MuMu"));
  tuple->column("MuMuKstar", Hlt("HLTFilterBd2MuMuKstar"));
  tuple->column("Jpsi" ,     Hlt("HLTHotJpsis"));
  tuple->column("isB" ,      Hlt("IsBEvent"));
  tuple->column("isC" ,      Hlt("IsbcEvent"));
  return StatusCode::SUCCESS ;
}
//=============================================================================
//  Fill L1Score
//=============================================================================
StatusCode TrackNTuple::fillL1Score(Tuple& tuple){

  if (exist<L1Score>(L1ScoreLocation::Default)){
    L1Score* L1 = get<L1Score>(L1ScoreLocation::Default);
    tuple->column("L1",L1->decision());
    tuple->column("L1Gen",L1->decisionGen());
    tuple->column("L1Mu",L1->decisionMu());
    tuple->column("L1Jpsi",L1->decisionDiMuJPsi());
    tuple->column("L1DiMu",L1->decisionDiMu());
    tuple->column("L1Phot",L1->decisionPhot());
    tuple->column("L1Elec",L1->decisionElec());
    tuple->column("L1pt1",L1->pt1());
    tuple->column("L1pt2",L1->pt2());
    int npv = 0 ;
    if (L1->pV1().z()>-990) ++npv;
    if (L1->pV2().z()>-990) ++npv;
    tuple->column("L1PV",npv);
  } else {
    tuple->column("L1",false);
    tuple->column("L1Gen",false);
    tuple->column("L1Mu",false);
    tuple->column("L1Jpsi",false);
    tuple->column("L1DiMu",false);
    tuple->column("L1Phot",false);
    tuple->column("L1Elec",false);
    tuple->column("L1pt1",-1.);
    tuple->column("L1pt2",-1.);
    tuple->column("L1PV",-1);
  }
  return StatusCode::SUCCESS ;
}
//=============================================================================
//  BitPattern
//=============================================================================
int TrackNTuple::bitPattern(){
  int p = 0;
  p +=     Hlt("HLTFilterB2HH");
  p +=   2*Hlt("HLTFilterBd2D0Kstar");
  p +=   4*Hlt("HLTFilterBd2DstarPi");
  p +=   8*Hlt("HLTFilterBs2DsH");
  p +=  16*Hlt("HLTFilterBs2PhiPhi");
  p +=  32*Hlt("HLTFilterBs2PhiGamma");
  p +=  64*Hlt("HLTFilterB2MuMu");
  p += 128*Hlt("HLTFilterBd2MuMuKstar");
  p += 256*Hlt("HLTHotJpsis");
  verbose() << "Pattern says " << p << endmsg ;
  return p ;
}
//=============================================================================
//  Fill Tuple
//=============================================================================
int TrackNTuple::Hlt(const std::string& Algo){
  if (!m_sr) return 0;
  std::string location = "/Event/Phys/"+Algo ;
  for ( SelResults::const_iterator isr  = m_sr->begin() ;
        isr != m_sr->end(); ++isr ) {
    if ( location == (*isr)->location() ){
      if ((*isr)->found()) return 1;
      else return 0 ;
    }
  }
  debug() << location << " not found" << endmsg ;
  return 0 ;
}    
//=============================================================================
//  Fill Tuple
//=============================================================================
StatusCode TrackNTuple::fillTuple(const ParticleVector& stables){
  Tuple tuple = nTuple( "Tracks" ) ;
  for ( ParticleVector::const_iterator S = stables.begin() ; S!=stables.end();++S ){
    StatusCode sc = fillTuple(*S,tuple);
    if (!sc) return sc;
    if ( m_truth ){
      const MCParticle* MC = m_pAsct->associatedFrom( *S );
      sc = fillTuple(MC,tuple);
      if (!sc) return sc;
    }
    tuple->write();
  }
  return StatusCode::SUCCESS;
}
//=============================================================================
//  Fill Tuple
//=============================================================================
StatusCode TrackNTuple::fillTuple(const Particle* S,Tuple& tuple){
    
  debug() << "Stable particle " << S->particleID().pid() << " " 
          << S->momentum() << endmsg ;    
  
  tuple->column( "Type" , trackType(S) );
  tuple->column( "P"    , S->momentum().rho()  );
  tuple->column( "Pt"   , S->pt()  );
  tuple->column( "theta", S->momentum().theta()  );
  tuple->column( "phi"  , S->momentum().phi()  );
  tuple->column( "PID"  , shortPID( S->particleID().pid() ));
  
  // IP
  const VertexVector pv = desktop()->primaryVertices(); // online PV
  //  tuple->column( "PVs"  , pv.size() ); // DO NOT UNCOMMENT!
    
  double minr=9999999.;
  std::vector<double> ips, ipes;
  for ( VertexVector::const_iterator P = pv.begin() ; P!=pv.end();++P){
    double ip=0., ipe=0.;
    StatusCode sc = geomDispCalculator()->calcImpactPar(*S,*(*P),ip,ipe);
    if (!sc) return sc;
    if (ip/ipe<minr){
      minr = ip/ipe;
      ips.insert(ips.begin(),ip); // first!
      ipes.insert(ipes.begin(),ipe); // first!     
    } else {
      ips.push_back(ip);
      ipes.push_back(ipe);
    }    
  }
  tuple->farray( "IP"   , ips.begin(),  ips.end(),  "PVs", 20  );
  tuple->farray( "IPE"  , ipes.begin(), ipes.end(), "PVs", 20  );
  
  bool passed = false ;
  if (exist<L1Score>("/Event/Trig/HltGenericScore")){
    L1Score* score = get<L1Score>("/Event/Trig/HltGenericScore");
    passed = score->decision();
  }
  if (passed) tuple->column( "Generic"  , 1. );
  else tuple->column( "Generic"  , 0. );
  tuple->column("BitPattern", (double)m_pattern );
  return StatusCode::SUCCESS;
}
//=============================================================================
//  Fill Tuple from truth
//=============================================================================
StatusCode TrackNTuple::fillTuple(const MCParticle* S,Tuple& tuple){

  std::vector<double> ips;
  if (!S) {
    debug() << "No truth associated" << endmsg ;
    tuple->column( "trueP"    , -1.  );
    tuple->column( "truePt"   , -1.  );
    tuple->column( "truethet", -1.  );
    tuple->column( "truephi", -5.  );
    tuple->column( "truePID"  , 0 );
    tuple->column( "motherID" , 0 );  
    tuple->column( "Quark" , -1. );  
    tuple->farray( "trueIP"   , ips.begin(),  ips.end(),  "truePVs", 20  );
  } else {

    debug() << "linked to particle " << S->particleID().pid() << " " 
            << S->momentum() << endmsg ;
    
    tuple->column( "trueP"    , S->momentum().rho()  );
    tuple->column( "truePt"   , S->pt()  );
    tuple->column( "truethet", S->momentum().theta()  );
    tuple->column( "truephi",   S->momentum().phi()  );
    tuple->column( "truePID"  , shortPID( S->particleID().pid() ));
    if (S->mother()) tuple->column( "motherID" , S->mother()->particleID().pid() );  
    else tuple->column( "motherID" , 0 );
    tuple->column( "Quark" , getQuark(S) ); 
    
    // vertices
    MCVertices* pv= get<MCVertices>(MCVertexLocation::Default);
    if (!pv) {
      info() << "Failed to retrieve MCVertices" << endreq;
      return StatusCode::FAILURE;
    }
    
    double minip=9999999.;
    std::vector<double> ips;
    for ( MCVertices::const_iterator P = pv->begin() ; P!=pv->end();++P){
      if ( (*P)->type()!=MCVertex::ppCollision ) continue ;
      double ip = mcIP(S->momentum().v(), S->originVertex()->position(), 
                       (*P)->position());
      if (ip<minip){
        minip = ip;
        ips.insert(ips.begin(),ip); // first!
      } else {
        ips.push_back(ip);
      }    
    }
    tuple->farray( "trueIP"   , ips.begin(),  ips.end(),  "truePVs", 20  );
  }
  // both cases
  Collisions* coll = get<Collisions>(CollisionLocation::Default);
  if ( coll ) tuple->column( "Coll", coll->size() );
  else tuple->column( "Coll", 0. );
  
  return StatusCode::SUCCESS;
}
//=============================================================================
//  get quark contents of ancestors
//=============================================================================
double TrackNTuple::getQuark(const MCParticle* S)
{
  const MCParticle* G = S;
  double quark = 0 ;
  while ( G->mother()){
    G = G->mother();
    ParticleID pid = G->particleID() ;
    int apid = abs(pid.pid());
    if ( pid.hasTop()){
      quark = 6. ;
    } else if ( pid.hasBottom()){
      if (apid==511 || apid==512 || apid==521){
        quark =(double)apid/100.  ;
      } else if (quark<5.) quark = 5. ;
    } else if ( pid.hasCharm()){
      if (apid==411 || apid==412 || apid==421){
        quark = (double)apid/100. ;
      } else if (quark<4.) quark = 4. ;
    } else if ( pid.hasStrange()){
      quark = 3. ;
    } else quark = 0. ;
  }
  return quark ;
  
}  
//=============================================================================
//  short PID
//=============================================================================
int TrackNTuple::shortPID(int pid){
  int abspid = abs(pid);
  int s = pid/abspid;
  switch ( abspid ){
  case 0 : return 0 ;
  case 11 : return 7*s ;
  case 13 : return 6*s ;
  case 22 : return 10*s ;
  case 130 : return 11*s ;
  case 211 : return s ;
  case 310 : return 12*s ;
  case 321 : return 2*s ;
  case 2212 : return 3*s ;
  case 2112 : return 15*s ;
  case 3122 : return 16*s ;
  }
  warning() << "Unforeseen stable particle " << pid << endmsg ;
  return pid ;
}
//=============================================================================
//  short PID
//=============================================================================
int TrackNTuple::trackType(const Particle* P){
  const TrgTrack* T = dynamic_cast<const TrgTrack*>(P->origin());
  // TrgTrack
  if ( T ){
    if ( T->backward()                         ) return 7;
    if ( T->type() == TrgTrack::TypeLong       ) return 1;
    if ( T->type() == TrgTrack::TypeVeloTT     ) return 2;
    if ( T->type() == TrgTrack::TypeDownstream ) return 3;
    if ( T->type() == TrgTrack::TypeVelo3D     ) return 5;
    if ( T->type() == TrgTrack::TypeVeloR      ) return 6;
    if ( T->type() == TrgTrack::TypeKShort     ) return 4;
    warning() << "Unknown Trg track type " << T->type() << endmsg ;
    return -2 ;
  }  
  // Calo
  const TrgCaloParticle* C = dynamic_cast<const TrgCaloParticle*>(P->origin());
  if ( C ) return 10 ;
  // Offline track
  const ProtoParticle* R = dynamic_cast<const ProtoParticle*>(P->origin());
  if ( R ){
    const TrStoredTrack* S =  R->track() ;
    if ( S ){
      if ( S->isBackward()   ) return 17 ;
      if ( S->isDownstream() ) return 13 ;
      if ( S->isUpstream()   ) return 12 ;
      if ( S->isVelotrack()  ) return 15 ;
      if ( S->isTtrack()     ) return 14 ;
      warning() << "Unknown TrStored track type " << endmsg ;
    }
  // Calo cluster
    const SmartRefVector< CaloHypo > H = R->calo() ;
    if ( !H.empty() ) return 20 ;
  }
  warning() << "No track type found" << endmsg ;
  return -1 ;
}
//=============================================================================
//  Some math
//=============================================================================
double TrackNTuple::mcIP(const Hep3Vector& d,const HepPoint3D& a,const HepPoint3D& pv){
  HepPoint3D TMP = a-pv;
  Hep3Vector dist(TMP.x(),TMP.y(),TMP.z());
  Hep3Vector Kross = dist.cross(d.unit());
  double ip = Kross.mag();
  verbose() << d << " " << a << " " << pv << " -> " << ip << endmsg ;
  return ip ;
};
//=============================================================================
//  Get stables
//=============================================================================
StatusCode TrackNTuple::getStables(const ParticleVector& allB,ParticleVector& stables) {
  int nd = 0 ;
  for ( ParticleVector::const_iterator B = allB.begin() ; B!=allB.end();++B){
    ParticleVector daughters ;   
    m_decayFinder->descendants(*B,daughters,true) ;// only end leafs
    nd += daughters.size() ;
    for ( ParticleVector::const_iterator D = daughters.begin() ; 
          D!=daughters.end();++D ){
      bool found = false ;
      for ( ParticleVector::const_iterator S = stables.begin() ; 
            S!=stables.end();++S ){
        if (*D==*S){
          found = true ;
          verbose() << "Particle already found" << endmsg ;
          break ;
        }
      }
      if (!found) stables.push_back(*D);
    } 
  }
  debug() << allB.size() << " final particles are made from " 
          << nd << " stables, " << stables.size() << " being unique" << endmsg ;
  return StatusCode::SUCCESS ;
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode TrackNTuple::finalize() {

  debug() << "==> Finalize" << endmsg;

  return  StatusCode::SUCCESS;
}

//=============================================================================
