// $Id: FlavourTaggingAlgorithm.cpp,v 1.10 2003-03-25 09:58:52 odie Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "Event/EventHeader.h"
#include "Event/Particle.h"
#include "Event/L0DUReport.h"
#include "DaVinciTools/IPhysDesktop.h"
#include "FlavourTagging/IFlavourTaggingTool.h"

// local
#include "FlavourTaggingAlgorithm.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FlavourTaggingAlgorithm
//
// 24/04/2002 : Olivier Dormond
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<FlavourTaggingAlgorithm>          s_factory ;
const        IAlgFactory& FlavourTaggingAlgorithmFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FlavourTaggingAlgorithm::FlavourTaggingAlgorithm( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator ), m_hypothesis_locations(0), m_pDesktop(0),
    m_taggingTool(0)
{
  declareProperty("TagLocation", m_tags_location = FlavourTagLocation::User);
  m_hypothesis_locations.clear();
  declareProperty("HypothesisLocations", m_hypothesis_locations );
  declareProperty("PrimaryVerticesLocation",
                  m_primVertices_location = VertexLocation::Primary );
  declareProperty("DesktopName", m_pDesktop_name = "PhysDesktop");
  declareProperty("TaggingTool", m_taggingTool_name = "OrderedTaggingTool");
  declareProperty("FirstOnly", m_only_one = false);
  declareProperty("CheckL0", m_checkL0 = true);
}

//=============================================================================
// Destructor
//=============================================================================
FlavourTaggingAlgorithm::~FlavourTaggingAlgorithm() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode FlavourTaggingAlgorithm::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialize" << endreq;

  StatusCode sc;
  sc = toolSvc()->retrieveTool(m_pDesktop_name, m_pDesktop, this);
  if( sc.isFailure() ) {
    log << MSG::FATAL << "Unable to retrieve Desktop tool '"
        << m_pDesktop_name << "'" << endreq;
    return sc;
  }

  sc = toolSvc()->retrieveTool(m_taggingTool_name, m_taggingTool, this);
  if( sc.isFailure() ) {
    log << MSG::FATAL << "Unable to retrieve Tagging tool '"
        << m_taggingTool_name << "'" << endreq;
    return sc;
  }

  // Ensure the HypothesisLocations ends with "Particles"
  std::vector<std::string>::iterator hi;
  for( hi=m_hypothesis_locations.begin();
       hi!=m_hypothesis_locations.end(); hi++ )
    if( hi->rfind("/Particles") == std::string::npos )
      hi->append("/Particles");

  m_n_events = 0;
  m_n_B = 0;
  m_n_b_tags = 0;
  m_n_bbar_tags = 0;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode FlavourTaggingAlgorithm::execute() {

  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;

  SmartDataPtr<EventHeader> evt_head(eventSvc(), EventHeaderLocation::Default );
  if( !evt_head ) {
    log << MSG::ERROR << "Couldn't retrieve the event head!" << endreq;
    return StatusCode::SUCCESS;
  }

  SmartDataPtr<L0DUReport> l0(eventSvc(), L0DUReportLocation::Default);
  if( l0 && m_checkL0 && (l0->decision() == false) ) {
    log << MSG::DEBUG << "Event rejected by L0" << endreq;
    return StatusCode::SUCCESS;
  }

  ParticleVector hypothesis(0);
  std::vector<std::string>::const_iterator loc_iter;
  for( loc_iter = m_hypothesis_locations.begin();
       loc_iter != m_hypothesis_locations.end(); loc_iter++ )
  {
    SmartDataPtr<Particles> keyed_hypothesis(eventSvc(),*loc_iter);
    if( !keyed_hypothesis ) {
      log << MSG::DEBUG << "Unable to find the B hypothesis at '"
          << *loc_iter << "'. Skipping it!" << endreq;
      continue;
    }
    Particles::const_iterator pi;
    bool include_it = true;
    for( pi = keyed_hypothesis->begin(); pi != keyed_hypothesis->end(); pi++ ) {
      if( !(*pi)->particleID().hasBottom() ) {
        log << MSG::VERBOSE << "Skipping a " << (*pi)->particleID().pid()
            << endreq;
        continue;
      }
      if( include_it ) {
        hypothesis.push_back( *pi );
        if( m_only_one )
          include_it = false;
      }
    }
  }
  if( hypothesis.size() == 0 ) {
    log << MSG::DEBUG << "No hypothesis found. Giving up!" << endreq;
    return StatusCode::SUCCESS;
  }

  SmartDataPtr<Vertices> primvtxs(eventSvc(),m_primVertices_location);
  if( !primvtxs ) {
    log << MSG::ERROR << "Unable to find the primary vertices at '"
        << m_primVertices_location << "'" << endreq;
    return StatusCode::SUCCESS;
  }
  Vertex *thePrimVtx = NULL;
  if( primvtxs->size() )
    thePrimVtx = *primvtxs->begin();
  else
    thePrimVtx = new Vertex;

  StatusCode sc = m_pDesktop->getInput();
  if (sc.isFailure()) {
    log << MSG::ERROR << "Unable to fill desktop" << endreq;
    return StatusCode::SUCCESS;
  }

  m_n_events++;

  const ParticleVector& parts = m_pDesktop->particles();

  FlavourTags *tags = new FlavourTags;

  log << MSG::DEBUG << "Event initialization successful" << endreq;

  log << MSG::DEBUG << "About to tag " << hypothesis.size() << " B s" << endreq;
  ParticleVector::const_iterator hi;
  for( hi=hypothesis.begin(); hi!=hypothesis.end(); hi++ ) {
    m_n_B++;
    log << MSG::DEBUG << "About to tag a " << (*hi)->particleID().pid()
        << endreq;
    FlavourTag *theTag = new FlavourTag;
    m_taggingTool->tagThisB( *(*hi), parts, *thePrimVtx, *theTag );
    log << MSG::DEBUG << "Result is ";
    switch( theTag->decision() ) {
    case FlavourTag::b:
      m_n_b_tags++;
      log << "'b'" << theTag->tagger()->particleID().pid();
      break;
    case FlavourTag::bbar:
      m_n_bbar_tags++;
      log << "'bbar'" << theTag->tagger()->particleID().pid();
      break;
    case FlavourTag::none:
      log << "'none'";
      break;
    default:
      break;
    }
    log << endreq;
    tags->insert(theTag);
    if( theTag->decision() != FlavourTag::none ) {
      std::list<Particle *> parts(0);
      parts.push_back(theTag->taggedB());
      std::list<Particle *>::iterator pi = parts.begin();
      while( pi != parts.end() ) {
        if( *pi == theTag->tagger() ) {
          log << MSG::WARNING 
              << "***** The tagger is a member of the B decay! *****" << endreq;
          break;
        }
        Vertex *vtx = (*pi)->endVertex();
        if( vtx != 0 ) {
          SmartRefVector<Particle>::const_iterator d_i;
          for( d_i=vtx->products().begin(); d_i!=vtx->products().end(); d_i++ )
            parts.push_back(const_cast<Particle*>(d_i->target()));
        }
        pi++;
      }
    }
  }
  FlavourTags::const_iterator tagi;
  for( tagi = tags->begin(); tagi != tags->end(); tagi++ ) {
    log << MSG::INFO << "TAGGING "
        << evt_head->runNum() << ' ' << evt_head->evtNum() << ' ';
    switch( (*tagi)->decision() ) {
    case FlavourTag::none:
      log << 0;
      break;
    case FlavourTag::b:
      log << -1;
      break;
    case FlavourTag::bbar:
      log << +1;
      break;
    }
    log << endreq;
  }

  sc = eventSvc()->registerObject(m_tags_location,tags);
  if (sc.isFailure())
    log << MSG::ERROR << "Unable to register the tags under '"
        << m_tags_location << "'" << endreq;

  if( primvtxs->size() == 0 )
    delete thePrimVtx;

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode FlavourTaggingAlgorithm::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;

  log << MSG::INFO << "Number of events processed: " << m_n_events << endreq;
  log << MSG::INFO << "Number of B:                " << m_n_B << endreq;
  log << MSG::INFO << "Number of b tag:            " << m_n_b_tags << endreq;
  log << MSG::INFO << "Number of bbar tag:         " << m_n_bbar_tags << endreq;

  double e    = double(m_n_b_tags+m_n_bbar_tags)/double(m_n_B);
  double w    = double(m_n_b_tags)/double(m_n_b_tags+m_n_bbar_tags);
  double eff  = e*pow(1-2*w,2);
  double se   = sqrt(e*(1-e)/double(m_n_B));
  double sw   = sqrt(w*(1-w)/double(m_n_b_tags+m_n_bbar_tags));
  double seff = sqrt(eff/double(m_n_B)*(4-eff*(1+3/eff)));

  log << MSG::INFO << "Efficency:                  "
      << e << " +/- " << se << endreq;
  log << MSG::INFO << "Wrong-tag fraction:         "
      << w << " +/- " << sw << endreq;
  log << MSG::INFO << "Effective efficiency:       "
      << eff << " +/- " << seff << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
