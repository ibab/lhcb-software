// $Id: CategoryTaggingTool.cpp,v 1.9 2003-07-01 09:37:33 odie Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/SmartDataPtr.h"
#include "MCTools/IVisPrimVertTool.h"
#include "Event/EventHeader.h"
#include "Event/Collision.h"
#include "Event/L0DUReport.h"
#include "Event/L1Report.h"

// local
#include "CategoryTaggingTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CategoryTaggingTool
//
// 24/04/2002 : Olivier Dormond
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<CategoryTaggingTool>          s_factory ;
const        IToolFactory& CategoryTaggingToolFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CategoryTaggingTool::CategoryTaggingTool( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : AlgTool ( type, name , parent ), m_eventSvc(0), m_visTool(0),
    m_nUntrigEvents(0), m_nL0Events(0), m_nL0L1Events(0),
    m_untrigCateg(), m_L0Categ(), m_L0L1Categ()
{
  declareInterface<IFlavourTaggingTool>(this);

  declareProperty( "MuonTaggingTool",
                   m_muonTTN = "SingleParticleTaggingTool/MuonTagger" );
  declareProperty( "ElectronTaggingTool",
                   m_electronTTN = "SingleParticleTaggingTool/ElectronTagger" );
  declareProperty( "KaonOSTaggingTool",
                   m_kaonOSTTN = "SingleParticleTaggingTool/KaonOSTagger" );
  declareProperty( "KaonSSTaggingTool",
                   m_kaonSSTTN = "KaonSameSideTaggingTool/KaonSSTagger" );
  declareProperty( "VtxChargeTaggingTool",
                   m_vtxChargeTTN = "VertexChargeTaggingTool/VtxChargeTagger" );
}

IFlavourTaggingTool *CategoryTaggingTool::getTT(std::string tname)
{
  MsgStream msg(msgSvc(), name());
  
  std::string::size_type type_len = tname.find('/');
  std::string type(tname,0,type_len);
  std::string name;
  if( type_len < tname.length() )
    name = tname.substr(type_len+1);
  msg << MSG::DEBUG << "Retrieving tool named '" << name << "' of type '"
      << type << "'" << endreq;
  IFlavourTaggingTool *tool;
  StatusCode sc = toolSvc()->retrieveTool(type, name, tool, this);
  if( !sc.isSuccess() ) {
    msg << MSG::FATAL << "Unable to find the requested tagging tool '"
        << tname << "'" << endreq;
    return NULL;
  }
  return tool;
}

StatusCode CategoryTaggingTool::initialize()
{
  MsgStream msg(msgSvc(), name());

  StatusCode sc = service("EventDataSvc", m_eventSvc, true);
  if( sc.isFailure() ) {
    msg << MSG::FATAL << " Unable to locate Event Data Service" << endreq;
    return sc;
  }

  sc = toolSvc()->retrieveTool( "VisPrimVertTool", m_visTool, this );
  if( sc.isFailure() ) {
     msg << MSG::FATAL << "Unable to retrieve the VisPrimVertTool" << endreq;
     return sc;
  }

  m_muonTT      = getTT(m_muonTTN);
  m_electronTT  = getTT(m_electronTTN);
  m_kaonOSTT    = getTT(m_kaonOSTTN);
  m_kaonSSTT    = getTT(m_kaonSSTTN);
  m_vtxChargeTT = getTT(m_vtxChargeTTN);

  if( m_muonTT && m_electronTT && m_kaonOSTT && m_kaonSSTT )
    return StatusCode::SUCCESS;
  else
    return StatusCode::FAILURE;
}

CategoryTaggingTool::~CategoryTaggingTool()
{}

void CategoryTaggingTool::tagThisB( const Particle &theB,
                                   const Particles &theEvent,
                                   const Vertex &thePrimVtx,
                                   FlavourTag &theTag )
{
  MsgStream log(msgSvc(), name());

  FlavourTag mu_tag, e_tag, kos_tag, kss_tag, vtx_tag;
  m_muonTT     ->tagThisB( theB, theEvent, thePrimVtx, mu_tag );
  m_electronTT ->tagThisB( theB, theEvent, thePrimVtx, e_tag );
  m_kaonOSTT   ->tagThisB( theB, theEvent, thePrimVtx, kos_tag );
  m_kaonSSTT   ->tagThisB( theB, theEvent, thePrimVtx, kss_tag );
  m_vtxChargeTT->tagThisB( theB, theEvent, thePrimVtx, vtx_tag );

  combine(mu_tag, e_tag, kos_tag, kss_tag, vtx_tag, theTag);
}

void CategoryTaggingTool::tagThisB( const Particle &theB,
                                   const ParticleVector &theEvent,
                                   const Vertex &thePrimVtx,
                                   FlavourTag &theTag )
{
  FlavourTag mu_tag, e_tag, kos_tag, kss_tag, vtx_tag;
  m_muonTT     ->tagThisB( theB, theEvent, thePrimVtx, mu_tag );
  m_electronTT ->tagThisB( theB, theEvent, thePrimVtx, e_tag );
  m_kaonOSTT   ->tagThisB( theB, theEvent, thePrimVtx, kos_tag );
  m_kaonSSTT   ->tagThisB( theB, theEvent, thePrimVtx, kss_tag );
  m_vtxChargeTT->tagThisB( theB, theEvent, thePrimVtx, vtx_tag );

  combine(mu_tag, e_tag, kos_tag, kss_tag, vtx_tag, theTag);
}

void CategoryTaggingTool::tagFromList( const Particle &theB,
                                       const ParticleVector &theCandidates,
                                       const Vertex &thePrimVtx,
                                       FlavourTag &theTag )
{
  FlavourTag mu_tag, e_tag, kos_tag, kss_tag, vtx_tag;
  m_muonTT     ->tagFromList( theB, theCandidates, thePrimVtx, mu_tag );
  m_electronTT ->tagFromList( theB, theCandidates, thePrimVtx, e_tag );
  m_kaonOSTT   ->tagFromList( theB, theCandidates, thePrimVtx, kos_tag );
  m_kaonSSTT   ->tagFromList( theB, theCandidates, thePrimVtx, kss_tag );
  m_vtxChargeTT->tagFromList( theB, theCandidates, thePrimVtx, vtx_tag );

  combine(mu_tag, e_tag, kos_tag, kss_tag, vtx_tag, theTag);
}

void CategoryTaggingTool::tagExcludingFromList(const Particle &theB,
                                              const Particles &theEvent,
                                              const ParticleVector &theExcluded,
                                              const Vertex &thePrimVtx,
                                              FlavourTag &theTag )
{
  FlavourTag mu_tag, e_tag, kos_tag, kss_tag, vtx_tag;
  m_muonTT     ->tagExcludingFromList( theB, theEvent, theExcluded, thePrimVtx,
                                       mu_tag );
  m_electronTT ->tagExcludingFromList( theB, theEvent, theExcluded, thePrimVtx,
                                       e_tag );
  m_kaonOSTT   ->tagExcludingFromList( theB, theEvent, theExcluded, thePrimVtx,
                                       kos_tag );
  m_kaonSSTT   ->tagExcludingFromList( theB, theEvent, theExcluded, thePrimVtx,
                                       kss_tag );
  m_vtxChargeTT->tagExcludingFromList( theB, theEvent, theExcluded, thePrimVtx,
                                       vtx_tag);

  combine(mu_tag, e_tag, kos_tag, kss_tag, vtx_tag, theTag);
}

void CategoryTaggingTool::tagExcludingFromList(const Particle &theB,
                                              const ParticleVector &theEvent,
                                              const ParticleVector &theExcluded,
                                              const Vertex &thePrimVtx,
                                              FlavourTag &theTag )
{
  FlavourTag mu_tag, e_tag, kos_tag, kss_tag, vtx_tag;
  m_muonTT     ->tagExcludingFromList( theB, theEvent, theExcluded, thePrimVtx,
                                       mu_tag );
  m_electronTT ->tagExcludingFromList( theB, theEvent, theExcluded, thePrimVtx,
                                       e_tag );
  m_kaonOSTT   ->tagExcludingFromList( theB, theEvent, theExcluded, thePrimVtx,
                                       kos_tag );
  m_kaonSSTT   ->tagExcludingFromList( theB, theEvent, theExcluded, thePrimVtx,
                                       kss_tag );
  m_vtxChargeTT->tagExcludingFromList( theB, theEvent, theExcluded, thePrimVtx,
                                       vtx_tag );

  combine(mu_tag, e_tag, kos_tag, kss_tag, vtx_tag, theTag);
}

void CategoryTaggingTool::combine( const FlavourTag &muon_tag,
                                   const FlavourTag &electron_tag,
                                   const FlavourTag &kaonOS_tag,
                                   const FlavourTag &kaonSS_tag,
                                   const FlavourTag &vtxCharge_tag,
                                   FlavourTag &theTag )
{
  MsgStream msg(msgSvc(), name());

  msg << MSG::DEBUG
      << "Entering active region of the CategoryTaggingTool"
      << endreq;
  
  SmartDataPtr<EventHeader> evtHead(m_eventSvc, EventHeaderLocation::Default );
  SmartDataPtr<L0DUReport> l0(m_eventSvc, L0DUReportLocation::Default);
  SmartDataPtr<L1Report> l1(m_eventSvc, L1ReportLocation::Default);
  SmartDataPtr<Collisions> collisions(m_eventSvc, CollisionLocation::Default);
  SmartDataPtr<Vertices> primvtxs(m_eventSvc,VertexLocation::Primary);

  int nprim = 0;
  Vertices::const_iterator vi;
  if( primvtxs )
    for( vi=primvtxs->begin(); vi!=primvtxs->end(); vi++ )
      if( (*vi)->type() == Vertex::Primary )
        nprim++;
  
  msg << MSG::INFO << "TAGGING SUMMARY "
      << evtHead->runNum() << ' '
      << std::setw(3) << evtHead->evtNum() << " : ";
  if( l0 )
    msg << (l0->decision() ? '1' : '0');
  else
    msg << '-';
  msg << ' ';
  if( l1 )
    msg << (l1->decision() ? '1' : '0');
  else
    msg << '-';
  msg << " : ";
  msg << nprim;
  msg << ' ';
  long v;
  if( m_visTool->countVertices(v).isSuccess() )
    msg << v;
  else
    msg << '-';

  msg << " : ";
  msg << std::setw(2) << int(muon_tag.decision()) << ' '
      << std::setw(2) << int(electron_tag.decision()) << ' '
      << std::setw(2) << int(kaonOS_tag.decision()) << ' '
      << std::setw(2) << int(kaonSS_tag.decision()) << ' '
      << std::setw(2) << int(vtxCharge_tag.decision()) << " : ";

  struct TagSet {
    bool mu;
    bool e;
    bool kos;
    bool kss;
  } ts = { false, false, false, false };
  
  if( muon_tag.decision() != FlavourTag::none )
    ts.mu = true;
  if( electron_tag.decision() != FlavourTag::none )
    ts.e = true;
  if( kaonOS_tag.decision() != FlavourTag::none )
    ts.kos = true;
  if( kaonSS_tag.decision() != FlavourTag::none )
    ts.kss = true;
  // If 2 leptons select the one with the highest P.
  if( (muon_tag.decision() != FlavourTag::none) &&
      (electron_tag.decision() != FlavourTag::none) )
    if( (muon_tag.tagger()->p() >= electron_tag.tagger()->p()) )
      ts.e = false;
    else
      ts.mu = false;
  // If the tagged B is not strange, forget the existence of KSS.
  if( !kaonSS_tag.taggedB()->particleID().hasStrange() )
    ts.kss = false;

  m_nUntrigEvents++;
  if( l0 && l0->decision() ) {
    m_nL0Events++;
    if( l1 && l1->decision() )
      m_nL0L1Events++;
  }
  int comb = 0;
  unsigned int key = (ts.mu?1:0)+(ts.e?2:0)+(ts.kos?10:0)+(ts.kss?100:0);
  switch( key ) {
    case 0: // no tag use vtx as a last resort
      theTag.setDecision(vtxCharge_tag.decision());
      theTag.setTagger(vtxCharge_tag.tagger());
      theTag.setType(vtxCharge_tag.type());
      theTag.setTaggedB(vtxCharge_tag.taggedB());
      fillCategory( "vtx charge", theTag );
      msg << 6;
      break;
    case 1: // mu only
      theTag.setDecision(muon_tag.decision());
      theTag.setTagger(muon_tag.tagger());
      theTag.setType(muon_tag.type());
      theTag.setTaggedB(muon_tag.taggedB());
      fillCategory( "mu only", theTag );
      msg << 1;
      break;
    case 2: // e only
      theTag.setDecision(electron_tag.decision());
      theTag.setTagger(electron_tag.tagger());
      theTag.setType(electron_tag.type());
      theTag.setTaggedB(electron_tag.taggedB());
      fillCategory( "e only", theTag );
      msg << 2;
      break;
    case 10: // kos only
      theTag.setDecision(kaonOS_tag.decision());
      theTag.setTagger(kaonOS_tag.tagger());
      theTag.setType(kaonOS_tag.type());
      theTag.setTaggedB(kaonOS_tag.taggedB());
      fillCategory( "K OS only", theTag );
      msg << 3;
      break;
    case 100: // kss only
      theTag.setDecision(kaonSS_tag.decision());
      theTag.setTagger(kaonSS_tag.tagger());
      theTag.setType(kaonSS_tag.type());
      theTag.setTaggedB(kaonSS_tag.taggedB());
      fillCategory( "K SS only", theTag );
      msg << 7;
      break;
    case 11: // mu + kos
      comb = int(muon_tag.decision()) + int(kaonOS_tag.decision());
      if( comb == 0 )
        theTag.setDecision(FlavourTag::none);
      else {
        if( comb > 0 )
          theTag.setDecision(FlavourTag::bbar);
        else 
          theTag.setDecision(FlavourTag::b);
        theTag.setTagger(muon_tag.tagger());
        theTag.setType(muon_tag.type());
        theTag.setTaggedB(muon_tag.taggedB());
      } 
      fillCategory( "mu + K OS", theTag );
      msg << 4;
      break;
    case 12: // e + kos
      comb = int(electron_tag.decision()) + int(kaonOS_tag.decision());
      if( comb == 0 )
        theTag.setDecision(FlavourTag::none);
      else {
        if( comb > 0 )
          theTag.setDecision(FlavourTag::bbar);
        else 
          theTag.setDecision(FlavourTag::b);
        theTag.setTagger(electron_tag.tagger());
        theTag.setType(electron_tag.type());
        theTag.setTaggedB(electron_tag.taggedB());
      } 
      fillCategory( "e + K OS", theTag );
      msg << 5;
      break;
    case 101: // mu + kss
      comb = int(muon_tag.decision()) + int(kaonSS_tag.decision());
      if( comb == 0 )
        theTag.setDecision(FlavourTag::none);
      else {
        if( comb > 0 )
          theTag.setDecision(FlavourTag::bbar);
        else 
          theTag.setDecision(FlavourTag::b);
        theTag.setTagger(muon_tag.tagger());
        theTag.setType(muon_tag.type());
        theTag.setTaggedB(muon_tag.taggedB());
      } 
      fillCategory( "mu + K SS", theTag );
      msg << 8;
      break;
    case 102: // e + kss
      comb = int(electron_tag.decision()) + int(kaonSS_tag.decision());
      if( comb == 0 )
        theTag.setDecision(FlavourTag::none);
      else {
        if( comb > 0 )
          theTag.setDecision(FlavourTag::bbar);
        else 
          theTag.setDecision(FlavourTag::b);
        theTag.setTagger(electron_tag.tagger());
        theTag.setType(electron_tag.type());
        theTag.setTaggedB(electron_tag.taggedB());
      } 
      fillCategory( "e + K SS", theTag );
      msg << 9;
      break;
    case 110: // kos + kss
      comb = int(kaonOS_tag.decision()) + int(kaonSS_tag.decision());
      if( comb == 0 )
        theTag.setDecision(FlavourTag::none);
      else {
        if( comb > 0 )
          theTag.setDecision(FlavourTag::bbar);
        else 
          theTag.setDecision(FlavourTag::b);
        theTag.setTagger(kaonOS_tag.tagger());
        theTag.setType(kaonOS_tag.type());
        theTag.setTaggedB(kaonOS_tag.taggedB());
      } 
      fillCategory( "K OS + K SS", theTag );
      msg << 10;
      break;
    case 111: // mu + kos + kss
      comb = int(kaonOS_tag.decision()) + int(kaonSS_tag.decision()) +
                 int(muon_tag.decision());
      if( ((comb > 0) && (int(muon_tag.decision()) > 0)) ||
          ((comb < 0) && (int(muon_tag.decision()) < 0)) ) {
        theTag.setDecision(muon_tag.decision());
        theTag.setTagger(muon_tag.tagger());
        theTag.setType(muon_tag.type());
        theTag.setTaggedB(muon_tag.taggedB());
      } else {
        theTag.setDecision(kaonOS_tag.decision());
        theTag.setTagger(kaonOS_tag.tagger());
        theTag.setType(kaonOS_tag.type());
        theTag.setTaggedB(kaonOS_tag.taggedB());
      }
      fillCategory( "mu + K OS + K SS", theTag );
      msg << 11;
      break;
    case 112: // e + kos + kss
      comb = int(kaonOS_tag.decision()) + int(kaonSS_tag.decision()) +
                 int(electron_tag.decision());
      if( ((comb > 0) && (int(electron_tag.decision()) > 0)) ||
          ((comb < 0) && (int(electron_tag.decision()) < 0)) ) {
        theTag.setDecision(electron_tag.decision());
        theTag.setTagger(electron_tag.tagger());
        theTag.setType(electron_tag.type());
        theTag.setTaggedB(electron_tag.taggedB());
      } else {
        theTag.setDecision(kaonOS_tag.decision());
        theTag.setTagger(kaonOS_tag.tagger());
        theTag.setType(kaonOS_tag.type());
        theTag.setTaggedB(kaonOS_tag.taggedB());
      }
      fillCategory( "e + K OS + K SS", theTag );
      msg << 12;
      break;
    default: // Unexpected case
      msg << MSG::WARNING << "An unexpected combination of tag occured!"
          << " Setting to 'none'." << endreq;
      theTag.setDecision(FlavourTag::none);
      theTag.setTagger((Particle*)NULL);
      theTag.setType(FlavourTag::singleParticle);
      theTag.setTaggedB(muon_tag.taggedB());
      msg << 0;
      break;
  }
  msg << " : " << std::setw(2) << int(theTag.decision()) << endreq;
}

void CategoryTaggingTool::fillCategory( std::string categ,
                                        const FlavourTag &theTag )
{
  SmartDataPtr<L0DUReport> l0(m_eventSvc, L0DUReportLocation::Default);
  SmartDataPtr<L1Report> l1(m_eventSvc, L1ReportLocation::Default);
    
  m_untrigCateg[categ].add(theTag.decision());
  if( l0 && l0->decision() ) {
    m_L0Categ[categ].add(theTag.decision());
    if( l1 && l1->decision() )
      m_L0L1Categ[categ].add(theTag.decision());
  }
}

StatusCode CategoryTaggingTool::finalize()
{
  MsgStream msg(msgSvc(), name());

  std::map<std::string,Category>::iterator ci;
  double teff, tseff;

  msg << MSG::INFO << "Results per categories (no trigger applied) "
      << m_nUntrigEvents << " events:\n"
//        12345678901234567890 123456 +/- 123456   123456 +/- 123456   123456 +/- 123456   1234  1234  1234 
      << "            Category     efficiency     wrong-tag fraction effective efficiency    #b #bbar #none\n";
  teff = tseff = 0.0;
  for( ci=m_untrigCateg.begin(); ci!=m_untrigCateg.end(); ci++ ) {
    double e, se, w, sw, eff, seff;
    ci->second.results(m_nUntrigEvents, e, se, w, sw, eff, seff);
    msg << format("%20s %-6.4f +/- %-6.4f   %-6.4f +/- %-6.4f   %-6.4f +/- %-6.4f   %4d  %4d  %4d\n",
      ci->first.c_str(), e, se, w, sw, eff, seff, ci->second.count(FlavourTag::b),
      ci->second.count(FlavourTag::bbar), ci->second.count(FlavourTag::none));
    if( e != 0.0 && eff != 0.0 ) {
      teff += eff;
      tseff += seff*seff;
    }
  }
  tseff = sqrt(tseff);
  msg << format( "%60s %-6.4f +/- %-6.4f\n", "Total:", teff, tseff) << endreq;

  msg << MSG::INFO << "Results per categories (L0 trigger applied) "
      << m_nL0Events << " events:\n"
      << "            Category     efficiency     wrong-tag fraction effective efficiency    #b #bbar #none\n";
  teff = tseff = 0.0;
  for( ci=m_L0Categ.begin(); ci!=m_L0Categ.end(); ci++ ) {
    double e, se, w, sw, eff, seff;
    ci->second.results(m_nL0Events, e, se, w, sw, eff, seff);
    msg << format("%20s %-6.4f +/- %-6.4f   %-6.4f +/- %-6.4f   %-6.4f +/- %-6.4f   %4d  %4d  %4d\n",
      ci->first.c_str(), e, se, w, sw, eff, seff, ci->second.count(FlavourTag::b),
      ci->second.count(FlavourTag::bbar), ci->second.count(FlavourTag::none));
    if( e != 0.0 && eff != 0.0 ) {
      teff += eff;
      tseff += seff*seff;
    }
  }
  tseff = sqrt(tseff);
  msg << format("%60s %-6.4f +/- %-6.4f\n", "Total:", teff, tseff) << endreq;

  msg << MSG::INFO << "Results per categories (L0*L1 trigger applied) "
      << m_nL0L1Events << " events:\n"
      << "            Category     efficiency     wrong-tag fraction effective efficiency    #b #bbar #none\n";
  teff = tseff = 0.0;
  for( ci=m_L0L1Categ.begin(); ci!=m_L0L1Categ.end(); ci++ ) {
    double e, se, w, sw, eff, seff;
    ci->second.results(m_nL0L1Events, e, se, w, sw, eff, seff);
    msg << format("%20s %-6.4f +/- %-6.4f   %-6.4f +/- %-6.4f   %-6.4f +/- %-6.4f   %4d  %4d  %4d\n",
      ci->first.c_str(), e, se, w, sw, eff, seff, ci->second.count(FlavourTag::b),
      ci->second.count(FlavourTag::bbar), ci->second.count(FlavourTag::none));
    if( e != 0.0 && eff != 0.0 ) {
      teff += eff;
      tseff += seff*seff;
    }
  }
  tseff = sqrt(tseff);
  msg << format("%60s %-6.4f +/- %-6.4f\n", "Total:", teff, tseff) << endreq;;

  return StatusCode::SUCCESS;
}

//=============================================================================
