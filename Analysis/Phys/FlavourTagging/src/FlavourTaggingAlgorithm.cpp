// $Id: FlavourTaggingAlgorithm.cpp,v 1.1.1.1 2002-05-23 23:25:51 gcorti Exp $
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
  : Algorithm ( name , pSvcLocator ), m_pDesktop(0), m_taggingTool(0)
{
  declareProperty("TagLocation", m_tags_location = FlavourTagLocation::User);
  declareProperty("ParticlesLocation",
                  m_particles_location = ParticleLocation::Production );
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

  n_B_events = 0;
  n_B = 0;
  n_b_tags = 0;
  n_bbar_tags = 0;

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

  SmartDataPtr<Particles> theEvent(eventSvc(),m_particles_location);
  if( !theEvent )
  {
    log << MSG::ERROR << "Unable to find the particles at '"
        << m_particles_location << "'" << endreq;
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
  log << MSG::DEBUG << "About to tag " << parts.size() << " B s" << endreq;
  ParticleVector::const_iterator pi;
  for( pi=parts.begin(); pi!=parts.end(); pi++ )
  {
    n_B++;
    log << MSG::DEBUG << "About to tag a " << (*pi)->particleID().pid()
        << endreq;
    FlavourTag *theTag = new FlavourTag;
    m_taggingTool->tagThisB( *(*pi), *theEvent, *thePrimVtx, *theTag );
    log << MSG::DEBUG << "Result is ";
    switch( theTag->decision() )
    {
    case FlavourTag::b:
      n_b_tags++;
      log << "'b'" << theTag->tagger()->particleID().pid();
      break;
    case FlavourTag::bbar:
      n_bbar_tags++;
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
    n_B_events++;
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

  log << MSG::INFO << "Number of events with a B: " << n_B_events << endreq;
  log << MSG::INFO << "Number of B:               " << n_B << endreq;
  log << MSG::INFO << "Number of b tag:           " << n_b_tags << endreq;
  log << MSG::INFO << "Number of bbar tag:        " << n_bbar_tags << endreq;
  log << MSG::INFO << "Efficency:                 "
      << float(n_b_tags+n_bbar_tags)/n_B << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
