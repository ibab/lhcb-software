// $Id: FlavourTaggingAlgorithm.cpp,v 1.2 2002-09-03 08:22:07 odie Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "Event/Particle.h"
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

  m_n_B_events = 0;
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

  StatusCode sc = m_pDesktop->getInput();
  if (sc.isFailure()) {
    log << MSG::ERROR << "Unable to fill desktop" << endreq;
    return StatusCode::SUCCESS;
  }

  ParticleVector hypothesis(0);
  std::vector<std::string>::const_iterator loc_iter;
  for( loc_iter = m_hypothesis_locations.begin();
       loc_iter != m_hypothesis_locations.end(); loc_iter++ )
  {
    SmartDataPtr<Particles> keyed_hypothesis(eventSvc(),*loc_iter);
    if( !keyed_hypothesis )
    {
      log << MSG::ERROR << "Unable to find the B hypothesis at '"
          << *loc_iter << "'. Skipping it!" << endreq;
      continue;
    }
    hypothesis.insert( hypothesis.end(),
                       keyed_hypothesis->begin(), keyed_hypothesis->end() );
  }
  if( hypothesis.size() == 0 )
  {
    log << MSG::DEBUG << "No hypothesis found. Giving up!" << endreq;
    return StatusCode::SUCCESS;
  }

  SmartDataPtr<Vertices> primvtxs(eventSvc(),m_primVertices_location);
  if( !primvtxs )
  {
    log << MSG::ERROR << "Unable to find the primary vertices at '"
        << m_primVertices_location << "'" << endreq;
    return StatusCode::SUCCESS;
  }
  Vertex *thePrimVtx = NULL;
  if( primvtxs->size() )
    thePrimVtx = *primvtxs->begin();
  else
    thePrimVtx = new Vertex;

  log << MSG::DEBUG << "Event initialization successful" << endreq;

  FlavourTags *tags = new FlavourTags;
  const ParticleVector& parts = m_pDesktop->particles();
  log << MSG::DEBUG << "About to tag " << hypothesis.size() << " B s" << endreq;
  ParticleVector::const_iterator hi;
  for( hi=hypothesis.begin(); hi!=hypothesis.end(); hi++ )
  {
    m_n_B++;
    log << MSG::DEBUG << "About to tag a " << (*hi)->particleID().pid()
        << endreq;
    FlavourTag *theTag = new FlavourTag;
    m_taggingTool->tagThisB( *(*hi), parts, *thePrimVtx, *theTag );
    log << MSG::DEBUG << "Result is ";
    switch( theTag->decision() )
    {
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
  }
  if( parts.size() )
    m_n_B_events++;
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

  log << MSG::INFO << "Number of events with a B: " << m_n_B_events << endreq;
  log << MSG::INFO << "Number of B:               " << m_n_B << endreq;
  log << MSG::INFO << "Number of b tag:           " << m_n_b_tags << endreq;
  log << MSG::INFO << "Number of bbar tag:        " << m_n_bbar_tags << endreq;
  log << MSG::INFO << "Efficency:                 "
      << float(m_n_b_tags+m_n_bbar_tags)/m_n_B << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
