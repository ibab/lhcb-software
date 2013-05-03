// Include files 
#include "BTaggingTool.h"
#include "Event/Track.h"

#include <boost/foreach.hpp>
#include <algorithm>
#include <cstdio>

#include "TaggingHelpers.h"

#undef DEBUG_CLONES
// uncomment line below to fill clone killing debug counters
//#define DEBUG_CLONES

//--------------------------------------------------------------------------
// Implementation file for class : BTaggingTool
//
//  First Author: Marco Musy
//  maintainers: Miriam Calvo, Marc Grabalosa, Stefania Vecchi
//--------------------------------------------------------------------------

using namespace LHCb ;
using namespace Gaudi::Units;

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( BTaggingTool )

//=========================================================================
  BTaggingTool::BTaggingTool( const std::string& type,
                              const std::string& name,
                              const IInterface* parent ) :
    GaudiTool ( type, name, parent ) {

    declareInterface<IBTaggingTool>(this);

    declareProperty( "ChoosePVCriterium",     m_ChoosePV    = "bestPV");
    declareProperty( "UseReFitPV",            m_UseReFitPV  = true );

    //preselction of tagging candidates
    declareProperty( "IPPU_cut",              m_IPPU_cut    = 3.0 );
    declareProperty( "thetaMin_cut",          m_thetaMin    = 0.012 );
    declareProperty( "distphi_cut",           m_distphi_cut = 0.005 );

    declareProperty( "CombineTaggersName",      m_CombineTaggersName = "CombineTaggersProbability" );
    declareProperty( "TaggerLocation",          m_taggerLocation = "Phys/TaggingParticles" );

    //choose active taggers
    declareProperty( "EnableMuonTagger",        m_EnableMuon    = true );
    declareProperty( "EnableElectronTagger",    m_EnableElectron= true );
    declareProperty( "EnableKaonOSTagger",      m_EnableKaonOS  = true );
    declareProperty( "EnableKaonSSTagger",      m_EnableKaonSS  = true );
    declareProperty( "EnablePionTagger",        m_EnablePionSS  = true );
    declareProperty( "EnableVertexChargeTagger",m_EnableVertexCharge= true);
    declareProperty( "EnableJetSameTagger",     m_EnableJetSame = false );

    declareProperty( "ForceSignalID",           m_ForceSignalID  = " "); //force signal B as Bu, Bd, Bs
    declareProperty( "UseVtxChargeWithoutOS",   m_UseVtxOnlyWithoutOS = false ); //use vtcxch only when no other OS tagger is active

    m_util    = 0;
    m_descend = 0;
    m_pvReFitter = 0;
    m_dva =0;
    m_combine = 0;
    m_taggerMu=m_taggerEle=m_taggerKaon=0;
    m_taggerKaonS=m_taggerPionS=m_taggerVtx=0 ;
  }

BTaggingTool::~BTaggingTool() {}

//==========================================================================
StatusCode BTaggingTool::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;

  m_util = tool<ITaggingUtils> ( "TaggingUtils", this );
  if( ! m_util ) {
    fatal() << "Unable to retrieve TaggingUtils tool "<< endreq;
    return StatusCode::FAILURE;
  }
  m_descend = tool<IParticleDescendants> ( "ParticleDescendants", this );
  if( ! m_descend ) {
    fatal() << "Unable to retrieve ParticleDescendants tool "<< endreq;
    return StatusCode::FAILURE;
  }
  m_taggerMu = tool<ITagger> ("TaggerMuonTool", this);
  if(! m_taggerMu) {
    fatal() << "Unable to retrieve TaggerMuonTool"<< endreq;
    return StatusCode::FAILURE;
  }
  m_taggerEle = tool<ITagger> ("TaggerElectronTool", this);
  if(! m_taggerEle) {
    fatal() << "Unable to retrieve TaggerElectronTool"<< endreq;
    return StatusCode::FAILURE;
  }
  m_taggerKaon = tool<ITagger> ("TaggerKaonOppositeTool", this);
  if(! m_taggerKaon) {
    fatal() << "Unable to retrieve TaggerKaonOppositeTool"<< endreq;
    return StatusCode::FAILURE;
  }
  m_taggerKaonS = tool<ITagger> ("TaggerKaonSameTool", this);
  if(! m_taggerKaonS) {
    fatal() << "Unable to retrieve TaggerKaonSameTool"<< endreq;
    return StatusCode::FAILURE;
  }
  m_taggerPionS = tool<ITagger> ("TaggerPionSameTool", this);
  if(! m_taggerPionS) {
    fatal() << "Unable to retrieve TaggerPionSameTool"<< endreq;
    return StatusCode::FAILURE;
  }
  m_taggerVtxCh= tool<ITagger> ("TaggerVertexChargeTool", this);
  if(! m_taggerVtxCh) {
    fatal() << "Unable to retrieve TaggerVertexChargeTool"<< endreq;
    return StatusCode::FAILURE;
  }
  m_taggerJetS = tool<ITagger> ("TaggerJetSameTool", this);
  if(! m_taggerJetS) {
    fatal() << "Unable to retrieve TaggerJetSameTool"<< endreq;
    return StatusCode::FAILURE;
  }
  m_combine = tool<ICombineTaggersTool> (m_CombineTaggersName, this);
  if(! m_combine) {
    fatal() << "Unable to retrieve "<< m_CombineTaggersName << endreq;
    return StatusCode::FAILURE;
  }
  if(m_UseReFitPV){
    debug() << "BTaggingTool: ReFitPV selected " << endreq;
    m_pvReFitter = tool<IPVReFitter>("AdaptivePVReFitter", this );
    if(! m_pvReFitter) {
      fatal() << "Unable to retrieve AdaptivePVReFitter" << endreq;
      return StatusCode::FAILURE;
    }
  }
  else debug() << "BTaggingTool: ReFitPV NOT selected " << endreq;
  m_dva = Gaudi::Utils::getIDVAlgorithm ( contextSvc(), this ) ;
  if (0==m_dva) return Error("Couldn't get parent DVAlgorithm",
                             StatusCode::FAILURE);


  return sc;
}

//==========================================================================
StatusCode BTaggingTool::tag( FlavourTag& theTag,
                              const Particle* AXB,
                              const RecVertex* RecVert,
                              Particle::ConstVector& vtags )
{

  theTag.setDecision( FlavourTag::none );
  if ( !AXB )
  {
    return Error( "No B hypothesis to tag!" );
  }

  //Load vertices and particles
  const Particle::Range  parts=get<Particle::Range>(m_taggerLocation+"/Particles");
  const RecVertex::Range verts=get<RecVertex::Range>(RecVertexLocation::Primary);
  if(msgLevel(MSG::DEBUG)) debug()<<" Nr Vertices: "<< verts.size()
                                  <<" Nr Particles: "<<parts.size()<<endreq;

  Vertex::ConstVector allVtx;
  allVtx.reserve( verts.size() );
  for ( RecVertex::Range::const_iterator iv = verts.begin(); 
        iv != verts.end(); ++iv ) 
  {
    if ( msgLevel(MSG::DEBUG) )
      debug()<<"PV found at z="<<(*iv)->position().z()/mm<<endreq;
    allVtx.push_back(AXB->endVertex());
  }

  //////////////////////////////////////////////////////////////////////////////
  //choose RecVert PV and pileup vertices taking into account refitting ///////
  RecVertex RefitRecVert(0);
  const RecVertex::ConstVector PileUpVtx = choosePrimary(AXB, verts, RecVert, RefitRecVert);
  if ( RecVert ) 
  {
    if( msgLevel(MSG::DEBUG) ) 
    {
      debug()<<"--> RecVert z=" << RecVert->position().z()/mm <<"  "<<m_ChoosePV<<endreq;
      if(m_UseReFitPV)
        debug() <<"-->     refitRecVert z=" << RefitRecVert.position().z()/mm <<endreq;
      for(RecVertex::ConstVector::const_iterator iv=PileUpVtx.begin();
          iv!=PileUpVtx.end(); iv++) {
        debug()<<"--> PileUpPV at z="<<(*iv)->position().z()/mm<<endreq;
      }
    }
  } 
  else
  {
    return Error( "No Vertex found! Skip." );
  }

  ////////////////////////////////////////////////////////
  //loop over Particles, preselect candidates ///////////
  theTag.setTaggedB( AXB );
  if(vtags.empty()) vtags = chooseCandidates(AXB, parts, PileUpVtx);


  //AXB is the signal B from selection
  bool isBd = AXB->particleID().hasDown();
  bool isBs = AXB->particleID().hasStrange();
  bool isBu = AXB->particleID().hasUp();

  ///ForceSignalID
  if (m_ForceSignalID=="Bd") { isBd = true; isBu = false; isBs = false; }
  if (m_ForceSignalID=="Bu") { isBd = false; isBu = true; isBs = false; }
  if (m_ForceSignalID=="Bs") { isBd = false; isBu = false; isBs = true; }

  ///Choose Taggers ------------------------------------------------------
  if (msgLevel(MSG::DEBUG)) debug() <<"evaluate taggers" <<endreq;

  Tagger muon, elec, kaon, kaonS, pionS, vtxCh, jetS;
  if(m_EnableMuon)     muon = m_taggerMu   -> tag(AXB, RecVert, allVtx, vtags);
  if(m_EnableElectron) elec = m_taggerEle  -> tag(AXB, RecVert, allVtx, vtags);
  if(m_EnableKaonOS)   kaon = m_taggerKaon -> tag(AXB, RecVert, allVtx, vtags);
  if(m_EnableKaonSS)   kaonS= m_taggerKaonS-> tag(AXB, RecVert, allVtx, vtags);
  if(m_EnablePionSS)   pionS= m_taggerPionS-> tag(AXB, RecVert, allVtx, vtags);
  if(m_EnableJetSame)  jetS = m_taggerJetS -> tag(AXB, RecVert, allVtx, vtags);
  if(m_EnableVertexCharge)
  {
    Particle::ConstVector vtagsPlusOS(0);
    for ( Particle::ConstVector::const_iterator i=vtags.begin(); i!=vtags.end(); ++i )
    {
      const ProtoParticle* iproto = (*i)->proto();
      if( m_UseVtxOnlyWithoutOS )
      {
        if(muon.type()!=0)
          if(muon.taggerParts().at(0)->proto() == iproto )
            vtagsPlusOS.push_back(*i);
        if(elec.type()!=0)
          if(elec.taggerParts().at(0)->proto() == iproto )
            vtagsPlusOS.push_back(*i);
        if(kaon.type()!=0)
          if(kaon.taggerParts().at(0)->proto() == iproto )
            vtagsPlusOS.push_back(*i);
      }
      vtagsPlusOS.push_back(*i);
    }
    vtxCh = m_taggerVtxCh -> tag(AXB, RecVert, allVtx, vtagsPlusOS);
  }

  std::vector<Tagger*> taggers;
  taggers.reserve(6);
  taggers.push_back(&muon);
  taggers.push_back(&elec);
  taggers.push_back(&kaon);
  taggers.push_back(&kaonS);
  taggers.push_back(&pionS);
  taggers.push_back(&vtxCh);

  //----------------------------------------------------------------------
  //Now combine the individual tagger decisions into
  //one final B flavour tagging decision. Such decision
  //can be made and categorised in two ways: the "PID"
  //approach is based on the particle type, while the
  //"Prob" approach is based on the wrong tag fraction
  int signalType =0;
  if(isBu || isBd ) signalType=1;
  else if(isBs) signalType=2;


  if (msgLevel(MSG::DEBUG)) debug() <<"combine taggers "<< taggers.size() <<endreq;
  m_combine -> combineTaggers( theTag, taggers , signalType);
  if (msgLevel(MSG::DEBUG))
  {
    debug()<<"omega: "<<theTag.omega()<<endreq;
    debug()<<"omegaOS: "<<theTag.omegaOS()<<endreq;
  }

  ///OUTPUT to Logfile ---------------------------------------------------

  RecHeader* evt = get<RecHeader> (RecHeaderLocation::Default);
  if (msgLevel(MSG::DEBUG)) debug() << "BTAGGING TAG   "
                                    << std::setw(9) << evt->runNumber()
                                    << std::setw(9) << evt->evtNumber()
                                    << std::setw(5) << theTag.decision()
                                    << std::setw(3) << theTag.category()
                                    << std::setw(5) << theTag.decisionOS()
                                    << std::setw(3) << theTag.categoryOS()
                                    << std::setw(5) << muon.decision()
                                    << std::setw(3) << elec.decision()
                                    << std::setw(3) << kaon.decision()
                                    << std::setw(3) << kaonS.decision()
                                    << std::setw(3) << pionS.decision()
                                    << std::setw(3) << vtxCh.decision()
                                    << endmsg;

  return StatusCode::SUCCESS;
}
//==========================================================================
StatusCode BTaggingTool::tag( FlavourTag& theTag, const Particle* AXB ) {
  Particle::ConstVector p(0);
  return tag( theTag, AXB, 0, p ) ;
}
//==========================================================================
StatusCode BTaggingTool::tag( FlavourTag& theTag, const Particle* AXB,
                              const RecVertex* RecVert ) {
  Particle::ConstVector p(0);
  return tag( theTag, AXB, RecVert, p );
}

///=========================================================================
const RecVertex::ConstVector
BTaggingTool::choosePrimary(const Particle* AXB,
                            const RecVertex::Range& verts,
                            const RecVertex*& RecVert,
                            RecVertex& RefitRecVert)
{


  RecVertex::ConstVector PileUpVtx(0); //will contain all the other primary vtx's
  if( RecVert ) {                                      //PV was given by the user
    debug()<<"Will use the PV given by the user."<<endreq;
    if(m_UseReFitPV) {  //----------------------------- Refit PV without B tracks
      RecVertex newPV(*RecVert);
      Particle newPart(*AXB);
      StatusCode sc = m_pvReFitter->remove(&newPart, &newPV);
      debug()<<" Refitted PV "<<endreq;
      RefitRecVert = newPV;
    }
  } else if (m_ChoosePV == "bestPV") { //choose bestPV according IRelatedPVFinder
    RecVert = (const RecVertex*) m_dva->bestVertex(AXB);
    if(RecVert)
      debug()<<"Will use the bestPV criteria found z="<<RecVert->position().z()<<endreq;
    else {
      err() <<"No bextPV vertex found! Skip. "<<endreq;
      return PileUpVtx;
    }


    if(m_UseReFitPV) {  //----------------------------- Refit PV without B tracks
      RecVertex newPV(*RecVert);
      Particle newPart(*AXB);
      StatusCode sc = m_pvReFitter->remove(&newPart, &newPV);
      debug()<<" Refitted PV "<<endreq;
      RefitRecVert = newPV;
    }
  } else {
    debug()<<"Will NOT use the bestPV criteria. Loop over the "<<verts.size()<<" vertices"<<endreq;
    double kdmin = 1000000;
    RecVertex::Range::const_iterator iv;
    // -------------------------------------------------- Loop over the PV-vertices
    for(iv=verts.begin(); iv!=verts.end(); iv++){
      RecVertex newPV(**iv);
      double var, ip, iperr;
      if(m_UseReFitPV) {  //----------------------------- Refit PV without B tracks
        Particle newPart(*AXB);
        StatusCode sc = m_pvReFitter->remove(&newPart, &newPV);
        debug()<<" Refitted PV "<<endreq;
        if(!sc) { err()<<"ReFitter fails!"<<endreq; continue; }
        m_util->calcIP(AXB, &newPV, ip, iperr);
        var=fabs(ip);
      } else {
        m_util->calcIP(AXB, *iv, ip, iperr);
      }
      if(m_ChoosePV=="PVbyIP") { //cheated sel needs this
        debug()<<"Will use the PVbyIP criteria "<<endreq;
        var=fabs(ip);
      } else if(m_ChoosePV=="PVbyIPs") {
        debug()<<"Will use the PVbyIPs criteria "<<endreq;
        if(!iperr){
          err()<<"IPerror zero or nan, skip vertex: "<<iperr<<endreq;
          continue;
        }
        var=fabs(ip/iperr);
      } else {
        err()<<"Invalid option ChoosePVCriterium: "<<m_ChoosePV<<endreq;
        return PileUpVtx;
      }
      if( var < kdmin ) {
        kdmin = var;
        RecVert = (*iv);
        if(m_UseReFitPV) RefitRecVert = newPV;
      }
      if( ! RecVert ) {
        err() <<"No Reconstructed Vertex! Skip. "
              <<"(If using CheatedSelection, set ChoosePVCriterium to PVbyIP !)"<<endreq;
        return PileUpVtx;
      }
    }
  }//else bestPV

  //build a vector of pileup vertices --------------------------
  double min_chiPV=1000;
  double the_chiPV=1000;
  int nPV=0;

  RecVertex::Range::const_iterator jv;
  for(jv=verts.begin(); jv!=verts.end(); jv++){
    const double dx = RecVert->position().x()-(*jv)->position().x();
    const double dy = RecVert->position().y()-(*jv)->position().y();
    const double dz = RecVert->position().z()-(*jv)->position().z();

    const double chiPV = sqrt(dx * dx / RecVert->covMatrix()(0,0) +
                              dy * dy / RecVert->covMatrix()(1,1) +
                              dz * dz / RecVert->covMatrix()(2,2));

    if(chiPV < min_chiPV) min_chiPV = chiPV;

    if(chiPV < 3) {
      the_chiPV = chiPV;
      nPV++;
      continue;

    } else
      PileUpVtx.push_back(*jv);
  }
  if(min_chiPV!=the_chiPV || nPV!=1 ) {
    PileUpVtx.clear();
    for(jv=verts.begin(); jv!=verts.end(); jv++){
      const double dxx = RecVert->position().x()-(*jv)->position().x();
      const double dyy = RecVert->position().y()-(*jv)->position().y();
      const double dzz = RecVert->position().z()-(*jv)->position().z();
      const double chiPV = sqrt(dxx * dxx / RecVert->covMatrix()(0,0) +
                                dyy * dyy / RecVert->covMatrix()(1,1) +
                                dzz * dzz / RecVert->covMatrix()(2,2));

      if(chiPV == min_chiPV) continue;
      else PileUpVtx.push_back(*jv);
    }
  }

  //UseReFitPV means that it will use the refitted pV for the ip calculation
  //of taggers and SV building. Do not move this line above PileUpVtx building
  if( m_UseReFitPV ) RecVert = (&RefitRecVert);

  if( ! RecVert ) 
  {
    Error( "No Reconstructed Vertex!! Skip." ).ignore();
    return PileUpVtx;
  }

  return PileUpVtx;
}

//=============================================================================
const Particle::ConstVector
BTaggingTool::chooseCandidates(const Particle* AXB,
                               const Particle::Range& parts,
                               const RecVertex::ConstVector& PileUpVtx) 
{

  Particle::ConstVector vtags;
  vtags.reserve(32);
  Particle::ConstVector axdaugh = m_descend->descendants( AXB );
  axdaugh.push_back( AXB );
  std::vector<const LHCb::Particle*> clones;
  for (Particle::Range::const_iterator ip = parts.begin(); parts.end() != ip; ++ip)
  {
    const LHCb::Particle* p = *ip;
    const ProtoParticle* proto = p->proto();
    if ( !proto || !proto->track() ) continue;
    if ( 0 == p->charge() ) continue;
    // for now, only allow long tracks
    const bool trackTypeOK =
      (proto->track()->type() == LHCb::Track::Long) ||
      (proto->track()->type() == LHCb::Track::Upstream);
    if (!trackTypeOK) continue;
    if( p->p()/GeV < 2.0 || p->p()/GeV  > 200. ) continue;
    if( p->pt()/GeV >  10. ) continue;
    // exclude tracks too close to the beam line
    if( p->momentum().theta() < m_thetaMin ) continue;
    // exclude "trivial" clones: particles with same protoparticle or same
    // underlying track
    //
    // FIXME: this uses the first such track for now, since they have the same
    // hit content, this should be ok...
    using TaggingHelpers::SameTrackStatus;
    using TaggingHelpers::isSameTrack;
    using TaggingHelpers::toString;
    SameTrackStatus isSame = TaggingHelpers::DifferentParticles;
    clones.clear();
    BOOST_FOREACH(const LHCb::Particle* q, vtags) {
      isSame = isSameTrack(*p, *q);
      if (!isSame) continue;
      // only skip all the rest if actually same track
      if (isSame >= TaggingHelpers::SameTrack) break;
      // otherwise, we may need some form of clone killing, because tracks
      // may be slightly different and we want to pick the "best" one, so
      // save the other contenders for "best track"
      clones.push_back(q);
    }
    if (isSame) {
#ifdef DEBUG_CLONES
      counter(std::string("clone/") + toString(isSame))++;
#endif
      // if it's actually the same underlying track object, we can throw away
      // this candidate because it is already in vtags
      if (isSame >= TaggingHelpers::SameTrack) continue;
    }
    // exclude pre-flagged clones we did not catch ourselves
    if (proto->track()->hasInfo(LHCb::Track::CloneDist)) {
      if (proto->track()->info(LHCb::Track::CloneDist, 999999.) < 5000.) {
#ifdef DEBUG_CLONES
        counter("clone/KL-clone")++;
#endif
        continue;
      }
    }

    // exclude signal tracks themselves
    double distphi;
    if( m_util->isinTree( p, axdaugh, distphi ) ) continue ;
    // exclude tracks too close to the signal
    if( distphi < m_distphi_cut ) continue;

    // calculate the min IP wrt all pileup vtxs
    double ippu, ippuerr;
    m_util->calcIP( p, PileUpVtx, ippu, ippuerr );
    // eliminate from vtags all parts coming from a pileup vtx
    if(ippuerr) {
      if( ippu/ippuerr<m_IPPU_cut ) continue; //preselection cuts
      Particle* c = const_cast<Particle*>(p);
      c->addInfo(1, ippu/ippuerr);
      verbose()<<"particle p="<<p->p()<<" ippu_sig "<<ippu/ippuerr<<endmsg;
    }else debug()<<"particle p="<<p->p()<<" ippu NAN ****"<<endmsg; // happens only when there is 1 PV

    // ok, if p is a potential clone, we need to find the "best" track and keep
    // only that one
    if (clones.empty()) {
      // no clone, so just store tagger candidate
      vtags.push_back(p);
    } else {
      // complete list of clones
      clones.push_back(p);
#ifdef DEBUG_CLONES
      counter("clones.size()") += clones.size();
#endif
      // functor to sort by quality (want to keep "best" track)
      struct dummy {
        static bool byIncreasingQual(
                                     const LHCb::Particle* p, const LHCb::Particle* q)
        {
          if (p->proto() && q->proto()) {
            if (p->proto()->track() && q->proto()->track()) {
              const LHCb::Track &t1 = *p->proto()->track();
              const LHCb::Track &t2 = *q->proto()->track();
              // prefer tracks which have more subdetectors in
              // where TT counts as half a subdetector
              const unsigned nSub1 = (t1.hasVelo() ? 2 : 0) +
                (t1.hasTT() ? 1 : 0) + (t1.hasT() ? 2 : 0);
              const unsigned nSub2 = (t2.hasVelo() ? 2 : 0) +
                (t2.hasTT() ? 1 : 0) + (t2.hasT() ? 2 : 0);
              if (nSub1 < nSub2) return true;
              if (nSub1 > nSub2) return false;
              // if available, prefer lower ghost probability
              const double ghProb1 = t1.ghostProbability();
              const double ghProb2 = t2.ghostProbability();
              if (-0. <= ghProb1 && ghProb1 <= 1. &&
                  -0. <= ghProb2 && ghProb2 <= 1.) {
                if (ghProb1 > ghProb2) return true;
                if (ghProb1 < ghProb2) return false;
              }
              // prefer longer tracks
              if (t1.nLHCbIDs() < t2.nLHCbIDs()) return true;
              if (t1.nLHCbIDs() > t2.nLHCbIDs()) return false;
              // for same length tracks, have chi^2/ndf decide
              const double chi1 = t1.chi2() / double(t1.nDoF());
              const double chi2 = t2.chi2() / double(t2.nDoF());
              if (chi1 > chi2) return true;
              if (chi1 < chi2) return false;
            }
          }
          // fall back on a pT comparison (higher is better) as last resort
          return p->pt() < q->pt();
        }
      };
      // sort clones by quality (stable_sort since byIncreasingQual does not
      // impose a total ordering, so use stable_sort to ensure that the
      // resulting order does not depend on sort implementation)
      std::stable_sort(clones.begin(), clones.end(),
                       dummy::byIncreasingQual);
      // remove all potential clones from vtags
      BOOST_FOREACH(const LHCb::Particle* q, clones) {
        // find potential clone in vtags
        Particle::ConstVector::iterator it =
          std::find(vtags.begin(), vtags.end(), q);
        // if in vtags, remove
        if (vtags.end() != it) vtags.erase(it);
      }
      // get rid of the clones
#undef DEBUGCLONES
#ifdef DEBUGCLONES
      bool debugclones = false;
      if (2 < clones.size()) {
        debugclones = true;
        // print overview in order
        std::printf("\n\nClone track list:\n");
        for (std::vector<const LHCb::Particle*>::const_iterator it = clones.begin();
             clones.end() != it; ++it) {
          const LHCb::Track* t = (*it)->proto() ? ((*it)->proto()->track() ?
                                                   (*it)->proto()->track() : 0) : 0;
          if (t) {
            std::printf("track p %6.3f pt %6.3f eta %6.3f phi %6.3f type %1u/%2u nmeas %2u chi2/ndf %6.3f ghostProb %6.3f sameness",
                        t->p() / GeV, t->pt() / GeV, t->pseudoRapidity(),
                        t->phi(), t->type(), t->history(), t->nLHCbIDs(),
                        t->chi2() / double(t->nDoF()), t->ghostProbability());
          } else {
            std::printf("part. p %6.3f pt %6.3f eta %6.3f phi %6.3f                                                     sameness",
                        (*it)->p() / GeV, (*it)->pt() / GeV, (*it)->momentum().eta(), (*it)->momentum().phi());
          }
          for (std::vector<const LHCb::Particle*>::const_iterator jt = clones.begin();
               clones.end() != jt; ++jt) {
            int isSame = isSameTrack(*(*it), *(*jt));
            printf(" %1u", isSame);
            if (jt == it) break;
          }
          printf("\n");
        }
      }
#endif
      // make a disjoint set of tracks by removing the worst track which
      // is a clone of another track until the set is clone-free
      for (std::vector<const LHCb::Particle*>::iterator it =
             clones.begin();
           clones.end() != it; ) {
        bool elim = false;
        for (std::vector<const LHCb::Particle*>::iterator jt = it + 1;
             clones.end() != jt; ++jt) {
          SameTrackStatus status = isSameTrack(*(*it), *(*jt));
          if (status) {
            // it is a redundant track, remove it from clones and
            // start over
            if (status == TaggingHelpers::ConvertedGamma) {
              // if we have a converted photon, remove the other leg
              // as well
              jt = clones.erase(jt);
            }
            it = clones.erase(it);
            elim = true;
            break;
          }
        }
        if (elim) continue;
        ++it;
      }
#ifdef DEBUGCLONES
      if (debugclones) {
        // print overview in order
        std::printf("Clone track list after filtering:\n");
        for (std::vector<const LHCb::Particle*>::const_iterator it = clones.begin();
             clones.end() != it; ++it) {
          const LHCb::Track* t = (*it)->proto() ? ((*it)->proto()->track() ?
                                                   (*it)->proto()->track() : 0) : 0;
          if (t) {
            std::printf("track p %6.3f pt %6.3f eta %6.3f phi %6.3f type %1u/%2u nmeas %2u chi2/ndf %6.3f ghostProb %6.3f sameness",
                        t->p() / GeV, t->pt() / GeV, t->pseudoRapidity(),
                        t->phi(), t->type(), t->history(), t->nLHCbIDs(),
                        t->chi2() / double(t->nDoF()), t->ghostProbability());
          } else {
            std::printf("part. p %6.3f pt %6.3f eta %6.3f phi %6.3f                                                     sameness",
                        (*it)->p() / GeV, (*it)->pt() / GeV, (*it)->momentum().eta(), (*it)->momentum().phi());
          }
          for (std::vector<const LHCb::Particle*>::const_iterator jt = clones.begin();
               clones.end() != jt; ++jt) {
            int isSame = isSameTrack(*(*it), *(*jt));
            printf(" %1u", isSame);
            if (jt == it) break;
          }
          printf("\n");
        }
      }
#endif
      // insert disjoint set of tracks into vtags
      if (!clones.empty())
        vtags.insert(vtags.end(), clones.begin(), clones.end());
    }

    if (msgLevel(MSG::DEBUG))
      debug() <<"part ID="<<p->particleID().pid()
              <<" p="<<p->p()/GeV
              <<" pt="<<p->pt()/GeV
              <<" PIDm="<<p->proto()->info( ProtoParticle::CombDLLmu, 0)
              <<" PIDe="<<p->proto()->info( ProtoParticle::CombDLLe, 0)
              <<" PIDk="<<p->proto()->info( ProtoParticle::CombDLLk, 0)
              <<endreq;
  }
#ifdef DEBUG_CLONES
  counter("nCands") += vtags.size();
#endif
  return vtags;
}

//=========================================================================
