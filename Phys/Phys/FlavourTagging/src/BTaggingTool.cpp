// Include files 
#include "BTaggingTool.h"

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
GaudiTool ( type, name, parent )
{

  declareInterface<IBTaggingTool>(this);

  declareProperty( "ChoosePVCriterium",     m_ChoosePV    = "bestPV");
  declareProperty( "UseReFitPV",            m_UseReFitPV  = true );
  declareProperty( "PVReFitterAlgName",     m_PVReFitterAlgName = "LoKi::PVReFitter");
  declareProperty( "VetoFailedRefits",      m_VetoFailedRefits = false);

  //preselction of tagging candidates
  declareProperty( "IPPU_cut",              m_IPPU_cut    = 3.0 );
  declareProperty( "thetaMin_cut",          m_thetaMin    = 0.012 );
  declareProperty( "distphi_cut",           m_distphi_cut = 0.005 );

  declareProperty( "CombineTaggersName",      m_CombineTaggersName = "CombineTaggersProbability" );
  declareProperty( "TaggerLocation",          m_taggerLocation = "Phys/TaggingParticles" );
  declareProperty( "CombineWithNNetTagger",   m_CombineWithNNetTagger = false );
  declareProperty( "CombineWithCharmTagger",  m_CombineWithCharmTagger = false );

  //choose active taggers
  declareProperty( "EnableMuonTagger",        m_EnableMuon    = true );
  declareProperty( "EnableElectronTagger",    m_EnableElectron= true );
  declareProperty( "EnableKaonOSTagger",      m_EnableKaonOS  = true );
  declareProperty( "EnableKaonSSTagger",      m_EnableKaonSS  = true );
  declareProperty( "EnablePionTagger",        m_EnablePionSS  = true );
  declareProperty( "EnablePionBDTTagger",     m_EnablePionBDTSS  = true );
  declareProperty( "EnableVertexChargeTagger",m_EnableVertexCharge= true);
  declareProperty( "EnableJetSameTagger",     m_EnableJetSame = false );
  // NNet tagging algorithms
  declareProperty( "EnableNNetKaonOSTagger",  m_EnableNNetKaonOS  = true );
  declareProperty( "EnableNNetKaonSSTagger",  m_EnableNNetKaonSS  = true );
  // SSproton taggin
  declareProperty( "EnableProtonTagger",      m_EnableProtonSS  = true );
  declareProperty( "EnableCharmTagger",       m_EnableCharm     = true );

  declareProperty( "ForceSignalID",           m_ForceSignalID  = " "); //force signal B as Bu, Bd, B

  declareProperty( "Personality",             m_personality = "Reco14");
  declareProperty( "ghostprob_cut",           m_ghostprob_cut = 0.5); // default preselection for Reco 14

  m_util= NULL;
  m_descend = NULL;
  m_pvReFitter = NULL;
  m_dva = NULL;
  m_combine = NULL;
  m_taggerMu = m_taggerEle=m_taggerKaon = NULL;
  m_taggerKaonS = m_taggerPionS = m_taggerVtxCh = NULL;
  m_taggerNNetKaonS = m_taggerNNetKaon = NULL;
  m_taggerProtonS = NULL;   m_taggerPionBDTS = NULL;
  m_taggerCharm = NULL;
}

BTaggingTool::~BTaggingTool() { }

//==========================================================================
StatusCode BTaggingTool::initialize()
{
  const StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;

  m_util = tool<ITaggingUtils> ( "TaggingUtils", this );

  m_descend    = m_util->ParticleDescendants();

  m_taggerMu   = tool<ITagger> ("TaggerMuonTool", this);

  m_taggerEle  = tool<ITagger> ("TaggerElectronTool", this);

  m_taggerKaon = tool<ITagger> ("TaggerKaonOppositeTool", this);

  m_taggerKaonS = tool<ITagger> ("TaggerKaonSameTool", this);

  m_taggerPionS = tool<ITagger> ("TaggerPionSameTool", this);

  m_taggerPionBDTS = tool<ITagger> ("TaggerPionBDTSameTool", this);

  m_taggerVtxCh = tool<ITagger> ("TaggerVertexChargeTool", this);

  m_taggerJetS  = tool<ITagger> ("TaggerJetSameTool", this);

  m_taggerNNetKaon  = tool<ITagger> ("TaggerNEWKaonOppositeTool", this);

  m_taggerNNetKaonS = tool<ITagger> ("TaggerNEWKaonSameTool", this);

  m_taggerProtonS = tool<ITagger> ("TaggerProtonSameTool", this);

  m_taggerCharm   = tool<ITagger> ("TaggerCharmTool", this);

  m_combine = tool<ICombineTaggersTool> (m_CombineTaggersName, this);


  if(m_UseReFitPV)
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << "BTaggingTool: ReFitPV selected " << endreq;
    m_pvReFitter = m_util->PVReFitter();
  }
  else
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << "BTaggingTool: ReFitPV NOT selected " << endreq;
  }

  m_dva = Gaudi::Utils::getIDVAlgorithm ( contextSvc(), this ) ;
  if (!m_dva) return Error("Couldn't get parent DVAlgorithm",
                           StatusCode::FAILURE);

  if ( msgLevel(MSG::DEBUG) )
    debug()<<" BTaggingTool configured to run with tuning "<<m_personality<<endreq;

  // register (multiple) personalities of chooseCandidates
  m_chooseCandidates.registerPersonality("Reco12",this, &BTaggingTool::chooseCandidatesReco12);
  m_chooseCandidates.registerPersonality("Reco14",this, &BTaggingTool::chooseCandidatesReco14);

  // select personality
  // (this throws an exception if the personality chosen by the user is not known)
  try
  {
    m_chooseCandidates.setPersonality(m_personality);
  }
  catch ( const std::exception& e )
  {
    error() << "Caught exception while setting chooseCandidates "
            << "personality: " << e.what() << endmsg;
    return StatusCode::FAILURE;
  }

  return sc;
}

//==========================================================================
StatusCode BTaggingTool::tag( FlavourTag& theTag,
                              const Particle* AXB,
                              const RecVertex* RecVert,
                              Particle::ConstVector& vtags )
{
  m_extraInfoToClear.clear();

  theTag.setDecision( FlavourTag::none );
  if ( !AXB )
  {
    return Error( "No B hypothesis to tag!" );
  }

  //Load vertices and particles
  const Particle::Range  parts = get<Particle::Range>(m_taggerLocation+"/Particles");
  const RecVertex::Range verts = get<RecVertex::Range>(RecVertexLocation::Primary);
  if(msgLevel(MSG::DEBUG)) debug() << " Nr Vertices: "<< verts.size()
                                   << " Nr Particles: "<<parts.size()<<endreq;

  const int allVtx=verts.size();
  if ( msgLevel(MSG::DEBUG) ){    
    for ( RecVertex::Range::const_iterator iv = verts.begin();
          iv != verts.end(); ++iv )
     debug()<<"PV found at z="<<(*iv)->position().z()/mm<<endreq;
  }  
  //////////////////////////////////////////////////////////////////////////////
  //choose RecVert PV and pileup vertices taking into account refitting ///////
  RecVertex RefitRecVert(0);
  const RecVertex::ConstVector PileUpVtx = choosePrimary(AXB, verts, RecVert, RefitRecVert);
  if ( RecVert )
  {
    if( msgLevel(MSG::DEBUG) )
    {
      debug()<<"--> RecVert (x,y,z)=(" 
             << RecVert->position().x()/mm<<","
             << RecVert->position().y()/mm<<","
             << RecVert->position().z()/mm <<")  "<<m_ChoosePV<<endreq;
      if(m_UseReFitPV)
        debug() <<"-->     refitRecVert z=" << RefitRecVert.position().z()/mm <<endreq;
      for(RecVertex::ConstVector::const_iterator iv=PileUpVtx.begin();
          iv!=PileUpVtx.end(); ++iv) 
      {
        debug()<<"--> PileUpPV at (x,y,z)=("
               <<(*iv)->position().x()/mm<<","
               <<(*iv)->position().y()/mm<<","
               <<(*iv)->position().z()/mm<<") "<<endreq;
      }
    }
  }
  else
  {
    clearExtraInfo();
    return Error( "No Vertex found! Skip." );
  }

  ////////////////////////////////////////////////////////
  //loop over Particles, preselect candidates ///////////
  theTag.setTaggedB( AXB );
  if(vtags.empty()) vtags = chooseCandidates(AXB, parts, PileUpVtx);
  if (msgLevel(MSG::DEBUG)) debug() <<"vtags.size()="<<vtags.size()<<endreq;

  //AXB is the signal B from selection
  bool isBd = AXB->particleID().hasDown();
  bool isBs = AXB->particleID().hasStrange();
  bool isBu = AXB->particleID().hasUp();

  ///ForceSignalID
  if      (m_ForceSignalID=="Bd") { isBd = true;  isBu = false; isBs = false; }
  else if (m_ForceSignalID=="Bu") { isBd = false; isBu = true;  isBs = false; }
  else if (m_ForceSignalID=="Bs") { isBd = false; isBu = false; isBs = true;  }

  ///Choose Taggers ------------------------------------------------------
  if (msgLevel(MSG::DEBUG)) debug() <<"evaluate taggers" <<endreq;

  Tagger muon, elec, kaon, kaonS, pionS, vtxCh, jetS, nnetkaon, nnetkaonS, protonS, charm, pionBDTS;

  if(m_EnableMuon)            muon = m_taggerMu    -> tag(AXB, RecVert, allVtx, vtags);
  if(m_EnableKaonOS)          kaon = m_taggerKaon  -> tag(AXB, RecVert, allVtx, vtags);
  if(m_EnableNNetKaonOS)  nnetkaon = m_taggerNNetKaon -> tag(AXB, RecVert, allVtx, vtags);
  if(m_EnableElectron)        elec = m_taggerEle   -> tag(AXB, RecVert, allVtx, vtags);
  if(m_EnableKaonSS)         kaonS = m_taggerKaonS -> tag(AXB, RecVert, allVtx, vtags);
  if(m_EnableNNetKaonSS) nnetkaonS = m_taggerNNetKaonS-> tag(AXB, RecVert, allVtx, vtags);
  if(m_EnablePionSS)         pionS = m_taggerPionS-> tag(AXB, RecVert, allVtx, vtags);
  if(m_EnablePionBDTSS)         pionBDTS = m_taggerPionBDTS-> tag(AXB, RecVert, allVtx, vtags);
  if(m_EnableJetSame)         jetS = m_taggerJetS  -> tag(AXB, RecVert, allVtx, vtags);
  if(m_EnableVertexCharge)   vtxCh = m_taggerVtxCh -> tag(AXB, RecVert, allVtx, vtags);
  if(m_EnableProtonSS)     protonS = m_taggerProtonS-> tag(AXB, RecVert, allVtx, vtags);
  if(m_EnableCharm)          charm = m_taggerCharm -> tag(AXB, RecVert, allVtx, vtags);

  std::vector<Tagger*> taggers;
  taggers.reserve(11);
  taggers.push_back(&muon);
  taggers.push_back(&elec);
  taggers.push_back(&kaon);
  taggers.push_back(&kaonS);
  taggers.push_back(&pionS);
  taggers.push_back(&pionBDTS);
  taggers.push_back(&vtxCh);
  taggers.push_back(&nnetkaon);
  taggers.push_back(&nnetkaonS);
  taggers.push_back(&protonS);
  taggers.push_back(&charm);

  //----------------------------------------------------------------------
  //Now combine the individual tagger decisions into one final B flavour tagging decision.
  //There are different methods that can be used.
  //CombineTaggersProbability.cpp  --> combines according individual probabilities (default)
  //CombineTaggersNN.cpp ---> combines using a NNEt that used individual probabilities
  //CombineTaggersOSTDR.cpp
  //CombineTaggersTDR.cpp
  //CombineTaggersPID.cpp --> based on the particle type

  int signalType =0;
  if(isBu || isBd ) signalType=1;
  else if(isBs) signalType=2;

  m_combine -> combineTaggers( theTag, taggers , signalType,  m_CombineWithNNetTagger, m_CombineWithCharmTagger);
  if (msgLevel(MSG::DEBUG))
  {
    debug() << "combine taggers "<< taggers.size()
            << "omega: "<<theTag.omega()<<"omegaOS: "<<theTag.omegaOS()<<endreq;
  }

  ///OUTPUT to Logfile ---------------------------------------------------

  if (msgLevel(MSG::DEBUG))
  {
    const RecHeader* evt = get<RecHeader> (RecHeaderLocation::Default);
    debug() << "BTAGGING TAG   "
            << std::setw(9) << evt->runNumber()
            << std::setw(9) << evt->evtNumber()
            << std::setw(3) << theTag.decision()<<" "
            << std::setw(6) << theTag.omega()
            << std::setw(3) << theTag.decisionOS()<<" "
            << std::setw(6) << theTag.omegaOS()
            << " * "
            << std::setw(3) << muon.decision()<<" "
            << std::setw(6) << muon.omega()
            << std::setw(3) << elec.decision()<<" "
            << std::setw(6) << elec.omega()
            << std::setw(3) << kaon.decision()<<" "
            << std::setw(6) << kaon.omega()
            << std::setw(3) << kaonS.decision()<<" "
            << std::setw(6) << kaonS.omega()
            << std::setw(3) << pionS.decision()<<" "
            << std::setw(6) << pionS.omega()
            << std::setw(3) << pionBDTS.decision()<<" "
            << std::setw(6) << pionBDTS.omega()
            << std::setw(3) << vtxCh.decision()<<" "
            << std::setw(6) << vtxCh.omega()
            << std::setw(3) << nnetkaon.decision()<<" "
            << std::setw(6) << nnetkaon.omega()
            << std::setw(3) << nnetkaonS.decision()<<" "
            << std::setw(6) << nnetkaonS.omega()
            << std::setw(3) << protonS.decision()<<" "
            << std::setw(6) << protonS.omega()
            << std::setw(3) << charm.decision()<<" "
            << std::setw(6) << charm.omega()
            << endmsg;
  }

  ///Cancel vtags info-------
  for ( Particle::ConstVector::const_iterator i = vtags.begin(); i != vtags.end(); ++i )
  {
    Particle* c = const_cast<Particle*>(*i);
    if( c->hasInfo(LHCb::Particle::FlavourTaggingIPPUs) ) c->eraseInfo(LHCb::Particle::FlavourTaggingIPPUs);
    if( c->hasInfo(LHCb::Particle::FlavourTaggingTaggerID) ) c->eraseInfo(LHCb::Particle::FlavourTaggingTaggerID);
  }

  clearExtraInfo();

  return StatusCode::SUCCESS;
}
//==========================================================================
StatusCode BTaggingTool::tag( FlavourTag& theTag, const Particle* AXB )
{
  Particle::ConstVector p(0);
  return tag( theTag, AXB, 0, p ) ;
}
//==========================================================================
StatusCode BTaggingTool::tag( FlavourTag& theTag, const Particle* AXB,
                              const RecVertex* RecVert )
{
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

  bool hasRefitFailed = false;
  RecVertex::ConstVector PileUpVtx(0); //will contain all the other primary vtx's
  if( RecVert )
  {                                      //PV was given by the user
    if( msgLevel(MSG::DEBUG) )
      debug()<<"Will use the PV given by the user."<<endreq;
    if(m_UseReFitPV)
    {  //----------------------------- Refit PV without B tracks
      RecVertex newPV(*RecVert);
      Particle newPart(*AXB);
      const StatusCode sc = m_pvReFitter->remove(&newPart, &newPV);
      if (!sc) { Error("ReFitter fails!").ignore(); hasRefitFailed = true; } // Should we stop the execution?
      if( msgLevel(MSG::DEBUG) ) debug()<<" Refitted PV "<<endreq;
      RefitRecVert = newPV;
    }
  }
  else if (m_ChoosePV == "bestPV")
  { //choose bestPV according IRelatedPVFinder
    RecVert = (const RecVertex*) m_dva->bestVertex(AXB);
    if(RecVert)
    {
      if( msgLevel(MSG::DEBUG) )
        debug()<<"Will use the bestPV criteria found z="<<RecVert->position().z()<<endreq;
    }
    else
    {
      Error( "No bextPV vertex found! Skip." ).ignore();
      return PileUpVtx;
    }

    if(m_UseReFitPV)
    {  //----------------------------- Refit PV without B tracks
      RecVertex newPV(*RecVert);
      Particle newPart(*AXB);
      const StatusCode sc = m_pvReFitter->remove(&newPart, &newPV);
      if(!sc) { Error("ReFitter fails!").ignore(); hasRefitFailed = true; } // Should we stop the execution?
      if( msgLevel(MSG::DEBUG) ) debug()<<" Refitted PV "<<endreq;
      RefitRecVert = newPV;
    }
  }
  else
  {
    if( msgLevel(MSG::DEBUG) )
      debug()<<"Will NOT use the bestPV criteria. Loop over the "
             <<verts.size()<<" vertices"<<endreq;
    double kdmin = 1000000;
    // -------------------------------------------------- Loop over the PV-vertices
    for ( RecVertex::Range::const_iterator iv = verts.begin(); 
          iv != verts.end(); ++iv )
    {
      RecVertex newPV(**iv);
      double var, ip, iperr;
      if(m_UseReFitPV)
      {  //----------------------------- Refit PV without B tracks
        Particle newPart(*AXB);
        const StatusCode sc = m_pvReFitter->remove(&newPart, &newPV);
        if( msgLevel(MSG::DEBUG) ) debug()<<" Refitted PV "<<endreq;
        if(!sc) { Error( "ReFitter fails!" ).ignore(); continue; }
        m_util->calcIP(AXB, &newPV, ip, iperr);
        var=fabs(ip);
      }
      else
      {
        m_util->calcIP(AXB, *iv, ip, iperr);
      }
      if(m_ChoosePV=="PVbyIP")
      { //cheated sel needs this
        if( msgLevel(MSG::DEBUG) ) debug()<<"Will use the PVbyIP criteria "<<endreq;
        var=fabs(ip);
      }
      else if(m_ChoosePV=="PVbyIPs")
      {
        if( msgLevel(MSG::DEBUG) ) debug()<<"Will use the PVbyIPs criteria "<<endreq;
        if(!iperr)
        {
          Error( "IPerror zero or nan, skip vertex" ).ignore();
          continue;
        }
        var=fabs(ip/iperr);
      }
      else
      {
        Error( "Invalid option ChoosePVCriterium: " + m_ChoosePV ).ignore();
        return PileUpVtx;
      }
      if( var < kdmin )
      {
        kdmin = var;
        RecVert = (*iv);
        if(m_UseReFitPV) RefitRecVert = newPV;
      }
      if( !RecVert )
      {
        Error( "No Reconstructed Vertex! Skip. "
               "(If using CheatedSelection, set ChoosePVCriterium to PVbyIP !)").ignore();
        return PileUpVtx;
      }
    }
  }//else bestPV

  //build a vector of pileup vertices --------------------------
  double min_chi2PV=1000;
  double the_chi2PV=1000;
  int nPV=0;

  if( !RecVert )
  {
    Error( "No Reconstructed Vertex! -> Skip" ).ignore();
    return PileUpVtx;
  }

  const double c00 = RecVert->covMatrix()(0,0);
  const double c11 = RecVert->covMatrix()(1,1);
  const double c22 = RecVert->covMatrix()(2,2);

  for ( RecVertex::Range::const_iterator jv=verts.begin(); jv!=verts.end(); jv++)
  {
    const double dx = RecVert->position().x()-(*jv)->position().x();
    const double dy = RecVert->position().y()-(*jv)->position().y();
    const double dz = RecVert->position().z()-(*jv)->position().z();

    const double chi2PV = ( ( fabs(c00) > 0 ? dx * dx / c00 : 9e9 ) +
                            ( fabs(c11) > 0 ? dy * dy / c11 : 9e9 ) +
                            ( fabs(c22) > 0 ? dz * dz / c22 : 9e9 ) );
    
    if(chi2PV < min_chi2PV) min_chi2PV = chi2PV;

    if(chi2PV < 3)
    {
      the_chi2PV = chi2PV;
      nPV++;
      continue;

    }
    else
    {
      PileUpVtx.push_back(*jv);
    }
  }

  if ( ( fabs(the_chi2PV) > 0 && fabs( min_chi2PV/the_chi2PV - 1.)>1.e-5 ) || nPV > 1 )
  {
    PileUpVtx.clear();
    for(RecVertex::Range::const_iterator jv=verts.begin(); jv!=verts.end(); jv++)
    {
      const double dxx = RecVert->position().x()-(*jv)->position().x();
      const double dyy = RecVert->position().y()-(*jv)->position().y();
      const double dzz = RecVert->position().z()-(*jv)->position().z();
      const double chi2PV = ( ( fabs(c00) > 0 ? dxx * dxx / c00 : 9e9 ) +
                              ( fabs(c11) > 0 ? dyy * dyy / c11 : 9e9 ) +
                              ( fabs(c22) > 0 ? dzz * dzz / c22 : 9e9 ) );

      if(fabs(chi2PV/min_chi2PV -1.)<1.e-5) continue;  // this is the PV
      else PileUpVtx.push_back(*jv);
    }
  }

  //UseReFitPV means that it will use the refitted pV for the ip calculation
  //of taggers and SV building. Do not move this line above PileUpVtx building
  if (not m_VetoFailedRefits) {
    hasRefitFailed = false;
  }
  
  if( m_UseReFitPV and (not hasRefitFailed)) RecVert = (&RefitRecVert);
  if( m_UseReFitPV and hasRefitFailed) RecVert = NULL;

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
                               const RecVertex::ConstVector& PileUpVtx) {
  return m_chooseCandidates(AXB, parts, PileUpVtx);
}
//=============================================================================
const Particle::ConstVector
BTaggingTool::chooseCandidatesReco12(const Particle* AXB,
                                     const Particle::Range& parts,
                                     const RecVertex::ConstVector& PileUpVtx) {

  double distphi;
  Particle::ConstVector vtags(0);
  Particle::Range::iterator ip;
  Particle::ConstVector axdaugh = m_descend->descendants( AXB );
  axdaugh.push_back( AXB );
  for ( ip = parts.begin(); ip != parts.end(); ip++ ){

    const ProtoParticle* proto = (*ip)->proto();
    if( !proto )                                 continue;
    if( !proto->track() )                        continue;
    if( proto->track()->type() < 3 )             continue;
    if( proto->track()->type() > 4 )             continue;
    if( (*ip)->p()/GeV < 2.0 )                   continue;
    if( (*ip)->momentum().theta() < m_thetaMin ) continue;
    if( (*ip)->charge() == 0 )                   continue;
    if( (*ip)->p()/GeV  > 200 )                  continue;
    if( (*ip)->pt()/GeV >  10 )                  continue;
    if( m_util->isinTree( *ip, axdaugh, distphi ) ) continue ;//exclude signal
    if( distphi < m_distphi_cut ) continue;

    //calculate the min IP wrt all pileup vtxs
    double ippu, ippuerr;
    m_util->calcIP( *ip, PileUpVtx, ippu, ippuerr );
    //eliminate from vtags all parts coming from a pileup vtx
    if(ippuerr) {
      if( ippu/ippuerr<m_IPPU_cut ) continue; //preselection cuts
      Particle* c = const_cast<Particle*>(*ip);
      if( c->hasInfo(LHCb::Particle::FlavourTaggingIPPUs) )
      {
        Error("FlavourTaggingIPPUs info already set: erasing it");
        c->eraseInfo(LHCb::Particle::FlavourTaggingIPPUs);
      }
      c->addInfo(LHCb::Particle::FlavourTaggingIPPUs, ippu/ippuerr); //store the information on the IPPU of the tagging particle
      m_extraInfoToClear.push_back( c );
      if (msgLevel(MSG::VERBOSE))
        verbose()<<"particle p="<<(*ip)->p()<<" ippu_sig "<<ippu/ippuerr<<endmsg;
    }
    else
    {
      if (msgLevel(MSG::DEBUG))
        debug()<<"particle p="<<(*ip)->p()<<" ippu NAN ****"<<endmsg; // happens only when there is 1 PV
    }

    bool dup=false;
    Particle::ConstVector::const_iterator ik;
    double partp = (*ip)->p();
    for ( ik = ip+1; ik != parts.end(); ik++) {
      if((*ik)->proto() == proto) {
        dup=true;
        break;
      }
      //more duplicates
      if((*ik)->p()==partp){
        warning()<<"Same particle momentum but different protoparticle --> duplicate"<<endreq;
        dup=true;
        break;
      }
    }
    if(dup) continue;

    ////////////////////////////////
    vtags.push_back(*ip);         // store tagger candidate
    ////////////////////////////////

    if (msgLevel(MSG::DEBUG))
      debug() <<"part ID="<<(*ip)->particleID().pid()
              <<" p="<<(*ip)->p()/GeV
              <<" pt="<<(*ip)->pt()/GeV
              <<" PIDm="<<(*ip)->proto()->info( ProtoParticle::CombDLLmu, 0)
              <<" PIDe="<<(*ip)->proto()->info( ProtoParticle::CombDLLe, 0)
              <<" PIDk="<<(*ip)->proto()->info( ProtoParticle::CombDLLk, 0)
              <<endreq;
  }
  return vtags;
}

//=============================================================================
const Particle::ConstVector
BTaggingTool::chooseCandidatesReco14(const Particle* AXB,
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
    const LHCb::Particle*    p = *ip;
    const ProtoParticle* proto = p->proto();
    if ( !proto || !proto->track() )                          continue;
    if ( 0 == p->charge() )                                   continue;
    // for now, only allow long tracks
    const bool trackTypeOK =
      (proto->track()->type() == LHCb::Track::Long) ||
      (proto->track()->type() == LHCb::Track::Upstream);
    if (!trackTypeOK)                                         continue;
    if( p->p()/GeV < 2.0 || p->p()/GeV  > 200. )              continue;
    if( p->pt()/GeV >  10. )                                  continue;
    // exclude tracks too close to the beam line
    if( p->momentum().theta() < m_thetaMin )                  continue;
    if( proto->track()->ghostProbability() > m_ghostprob_cut )continue;
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
    BOOST_FOREACH(const LHCb::Particle* q, vtags)
    {
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
    double distphi(0);
    if( m_util->isinTree( p, axdaugh, distphi ) ) continue ;
    // exclude tracks too close to the signal
    if( distphi < m_distphi_cut ) continue;

    // CRJ : This is not really allowed -- SV reintroduced for Local FT use
    Particle* c = const_cast<Particle*>(p);
    if( c->hasInfo(LHCb::Particle::FlavourTaggingIPPUs) )
    {
      Error("FlavourTaggingIPPUs info already set: erasing it");
      c->eraseInfo(LHCb::Particle::FlavourTaggingIPPUs);
    }
    if( c->hasInfo(LHCb::Particle::FlavourTaggingTaggerID) )
    {
      Error("FlavourTaggingTaggerID info already set: erasing it");
      c->eraseInfo(LHCb::Particle::FlavourTaggingTaggerID);
    }

    // calculate the min IP wrt all pileup vtxs
    double ippu(0), ippuerr(0);
    m_util->calcIP( p, PileUpVtx, ippu, ippuerr );
    // eliminate from vtags all parts coming from a pileup vtx
    if(ippuerr)
    {
      if( ippu/ippuerr<m_IPPU_cut ) continue; //preselection cuts

      c->addInfo(LHCb::Particle::FlavourTaggingIPPUs, ippu/ippuerr);
      m_extraInfoToClear.push_back( c );
      if( msgLevel(MSG::VERBOSE) )
        verbose()<<"particle p="<<p->p()<<" ippu_sig "<<ippu/ippuerr<<endmsg;
    }
    else
    {
      if( msgLevel(MSG::DEBUG) )
        debug()<<"particle p="<<p->p()<<" ippu NAN ****"<<endmsg; // happens only when there is 1 PV
    }

    // ok, if p is a potential clone, we need to find the "best" track and keep
    // only that one
    if (clones.empty())
    {
      // no clone, so just store tagger candidate
      vtags.push_back(p);
    }
    else
    {
      // complete list of clones
      clones.push_back(p);
#ifdef DEBUG_CLONES
      counter("clones.size()") += clones.size();
#endif
      // functor to sort by quality (want to keep "best" track)
      struct dummy
      {
        static bool byIncreasingQual(const LHCb::Particle* p, const LHCb::Particle* q)
        {
          if (p->proto() && q->proto())
          {
            if (p->proto()->track() && q->proto()->track())
            {
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
                  -0. <= ghProb2 && ghProb2 <= 1.)
              {
                if (ghProb1 > ghProb2) return true;
                if (ghProb1 < ghProb2) return false;
              }
              // prefer longer tracks
              if (t1.nLHCbIDs() < t2.nLHCbIDs()) return true;
              if (t1.nLHCbIDs() > t2.nLHCbIDs()) return false;
              // for same length tracks, have chi^2/ndf decide
              const double chi1 = t1.chi2() / double(t1.nDoF());
              const double chi2 = t2.chi2() / double(t2.nDoF());
              return (chi1 > chi2);
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
      BOOST_FOREACH(const LHCb::Particle* q, clones)
      {
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
      if (2 < clones.size())
      {
        debugclones = true;
        // print overview in order
        std::printf("\n\nClone track list:\n");
        for (std::vector<const LHCb::Particle*>::const_iterator it = clones.begin();
             clones.end() != it; ++it)
        {
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
           clones.end() != it; )
      {
        bool elim = false;
        for (std::vector<const LHCb::Particle*>::iterator jt = it + 1;
             clones.end() != jt; ++jt)
        {
          const SameTrackStatus status = isSameTrack(*(*it), *(*jt));
          if (status)
          {
            // it is a redundant track, remove it from clones and
            // start over
            if (status == TaggingHelpers::ConvertedGamma)
            {
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
      if (debugclones)
      {
        // print overview in order
        std::printf("Clone track list after filtering:\n");
        for (std::vector<const LHCb::Particle*>::const_iterator it = clones.begin();
             clones.end() != it; ++it)
        {
          const LHCb::Track* t = (*it)->proto() ? ((*it)->proto()->track() ?
                                                   (*it)->proto()->track() : 0) : 0;
          if (t)
          {
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

void BTaggingTool::clearExtraInfo()
{
  for ( std::vector<LHCb::Particle*>::iterator iC = m_extraInfoToClear.begin();
        iC != m_extraInfoToClear.end(); ++iC )
  {
    if ( (*iC)->hasInfo(LHCb::Particle::FlavourTaggingIPPUs) ) (*iC)->eraseInfo(LHCb::Particle::FlavourTaggingIPPUs);
    if ( (*iC)->hasInfo(LHCb::Particle::FlavourTaggingTaggerID) ) (*iC)->eraseInfo(LHCb::Particle::FlavourTaggingTaggerID);
  }
  m_extraInfoToClear.clear();
}

//=========================================================================
