// $Id: SingleParticleTaggingTool.cpp,v 1.4 2002-09-06 07:06:04 odie Exp $
#include <algorithm>
#include <iomanip>

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/NTuple.h"

#include "Event/Vertex.h"

// from CLHEP
#include "CLHEP/Units/SystemOfUnits.h"

// from DaVinciTools
#include "DaVinciTools/IParticleFilter.h"
#include "DaVinciTools/IGeomDispCalculator.h"

// local
#include "SingleParticleTaggingTool.h"
#include "Event/FlavourTag.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SingleParticleTaggingTool
//
// 14/03/2002 : Olivier Dormond
//-----------------------------------------------------------------------------

// Declaration of the AlgTool Factory
static const ToolFactory<SingleParticleTaggingTool> s_factory;
const IToolFactory& SingleParticleTaggingToolFactory = s_factory;

//=============================================================================
// Standard creator, initializes variables
//=============================================================================
SingleParticleTaggingTool::SingleParticleTaggingTool( const std::string &type,
                                                      const std::string &name,
                                                      const IInterface *parent)
  : AlgTool( type, name, parent ), m_PrimVerticesLocations(0),
    m_Filter(0), m_GeomDisp(0), m_EventSvc(0), m_NTupleSvc(0)
{
  declareInterface<IFlavourTaggingTool>(this);

  m_PrimVerticesLocations.push_back(VertexLocation::Primary);

  declareProperty( "FilterName", m_FilterName = "" );
  declareProperty( "GeomDispCalculator", m_GeomDispName="GeomDispCalculator" );
  declareProperty( "PrimaryVertices", m_PrimVerticesLocations );
  declareProperty( "InverseCharge", m_InverseCharge = true );
  declareProperty( "AllVertices", m_AllVertices = true );
  declareProperty( "MinImpactSignificance", m_ISMin = -1 );
  declareProperty( "MaxImpactSignificance", m_ISMax = -1 );
  declareProperty( "NtupleName", m_NTupleName = "auto" );
  declareProperty( "Monitored", m_Monitor = false );
  declareProperty( "MonitorLocation", m_MonitorLocation = "auto" );
}

StatusCode SingleParticleTaggingTool::initialize()
{
  MsgStream log(msgSvc(), name());
  IToolSvc* toolsvc = NULL;
  if( serviceLocator() )
  {
    serviceLocator()->service("ToolSvc", toolsvc);
    serviceLocator()->service("EventDataSvc", m_EventSvc);
  }
  if( !toolsvc )
  {
    log << MSG::FATAL << "Unable to locate Tool Service" << endreq;
    return StatusCode::FAILURE;
  }
  if( !m_EventSvc )
  {
    log << MSG::FATAL << "Unable to find the requested Event Service '"
        << "EventDataSvc" << "'" << endreq;
    return StatusCode::FAILURE;
  }
  StatusCode sc = toolsvc->retrieveTool(m_FilterName, m_Filter, this);
  if( !sc.isSuccess() )
  {
    log << MSG::FATAL << "Unable to find the requested filter '"
        << m_FilterName << "'" << endreq;
    return StatusCode::FAILURE;
  }
  sc = toolsvc->retrieveTool(m_GeomDispName, m_GeomDisp);
  if( !sc.isSuccess() )
  {
    log << MSG::FATAL << "GeomDispCalculator tool named '" << m_GeomDispName
        << "' could not be found" << endreq;
    return StatusCode::FAILURE;
  }

  std::string name_tail =
    (name().rfind('.') == string::npos) ? name()
                                        : name().substr(name().rfind('.')+1); 

  if( m_Monitor && ( m_MonitorLocation == "auto" || m_MonitorLocation == "Auto"
                     || m_MonitorLocation == "AUTO" ) )
    m_MonitorLocation = "/Event/Phys/Monitor/"+name_tail+"/Tags";

  if(m_NTupleName == "none" || m_NTupleName == "None" || m_NTupleName == "NONE")
  {
    m_DoNTuple = false;
    return StatusCode::SUCCESS;
  }
  m_DoNTuple = true;

  serviceLocator()->service("NTupleSvc", m_NTupleSvc);
  if( !m_NTupleSvc )
  {
    log << MSG::FATAL << "The NTupleSvc could not be found." << endreq;
    return StatusCode::FAILURE;
  }

  if(m_NTupleName == "auto" || m_NTupleName == "Auto" || m_NTupleName == "AUTO")
    m_NTupleName = "FILE1/FlavourTagging/"+name_tail;

  log << MSG::DEBUG << "Booking an ntuple named: " << m_NTupleName << endreq;

  StatusCode status;
  NTuplePtr nt(m_NTupleSvc, m_NTupleName);
  if( !nt )    // Check if already booked
  {
    nt = m_NTupleSvc->book( m_NTupleName, CLID_ColumnWiseTuple, name() );
     
    if( nt )  // ntuple sucessfully booked
    {
      status = nt->addItem ("nCands", m_n_cands, 0, 1000 );
      if ( status.isSuccess() )
        status = nt->addItem ("nVtxs", m_n_vtxs, 0, 5);
      if ( status.isSuccess() )
        status = nt->addItem ("iTagger",  m_i_selected, 0, 1000 );
      if ( status.isSuccess() )
        status = nt->addItem ("Tag",  m_tag, -1, 1 );

      if ( status.isSuccess() )
        status = nt->addIndexedItem ("px", m_n_cands, m_px);
      if ( status.isSuccess() )
        status = nt->addIndexedItem ("py", m_n_cands, m_py);
      if ( status.isSuccess() )
        status = nt->addIndexedItem ("pz", m_n_cands, m_pz);
      if ( status.isSuccess() )
        status = nt->addIndexedItem ("vx", m_n_cands, m_vx);
      if ( status.isSuccess() )
        status = nt->addIndexedItem ("vy", m_n_cands, m_vy);
      if ( status.isSuccess() )
        status = nt->addIndexedItem ("vz", m_n_cands, m_vz);
      if ( status.isSuccess() )
        status = nt->addIndexedItem ("CL", m_n_cands, m_cl);
      if ( status.isSuccess() )
        status = nt->addIndexedItem ("ID", m_n_cands, m_id);
      if ( status.isSuccess() )
        status = nt->addIndexedItem ("ip", m_n_cands, 5, m_ip);
      if ( status.isSuccess() )
        status = nt->addIndexedItem ("iperr", m_n_cands, 5, m_iperr);      
    }
    else
    {   // did not manage to book the N tuple....
      log << MSG::ERROR << "Failed to book the col_wise N-tuple" << endreq; 
      return StatusCode::FAILURE;
    }
  }
  else
  {  // Just reconnect to existing items
    status = nt->item ("nCands", m_n_cands );
    if ( status.isSuccess() ) status = nt->item ("nVtxs", m_n_vtxs );
    if ( status.isSuccess() ) status = nt->item ("iTagger", m_i_selected );
    if ( status.isSuccess() ) status = nt->item ("Tag",  m_tag);
    if ( status.isSuccess() ) status = nt->item ("px", m_px );
    if ( status.isSuccess() ) status = nt->item ("py", m_py);
    if ( status.isSuccess() ) status = nt->item ("pz", m_pz);
    if ( status.isSuccess() ) status = nt->item ("vx", m_vx);
    if ( status.isSuccess() ) status = nt->item ("vy", m_vy);
    if ( status.isSuccess() ) status = nt->item ("vz", m_vz);
    if ( status.isSuccess() ) status = nt->item ("CL", m_cl);
    if ( status.isSuccess() ) status = nt->item ("ID", m_id);
    if ( status.isSuccess() ) status = nt->item ("ip", m_ip);
    if ( status.isSuccess() ) status = nt->item ("iperr", m_iperr);
  }
  if ( !status )
  {
    log << MSG::ERROR << "Failed to add items to col_wise N-tuple" << endreq; 
    return status;
  }
  return StatusCode::SUCCESS;
}

void SingleParticleTaggingTool::FlattenTree( Particle *part,
                                             ParticleVector &flat )
{
  flat.push_back( part );
  if( part->endVertex() == NULL )
    return;
  SmartRefVector<Particle> products = part->endVertex()->products();
  SmartRefVector<Particle>::iterator i;
  for( i = products.begin(); i != products.end(); i++ )
    FlattenTree( *i, flat );
}

void SingleParticleTaggingTool::tagThisB( const Particle &theB,
                                          const Particles &theEvent,
                                          const Vertex &thePrimVtx,
                                          FlavourTag &theTag )
{
  MsgStream log(msgSvc(), name());
  // The list of the particles the B is made of (including the B) (recursive).
  ParticleVector toExclude(0);
  log << MSG::DEBUG << "Flattening the decay of the B" << endreq;
  FlattenTree( const_cast<Particle *>(&theB), toExclude );

  tagExcludingFromList( theB, theEvent, toExclude, thePrimVtx, theTag );
}

void SingleParticleTaggingTool::tagThisB( const Particle &theB,
                                          const ParticleVector &theEvent,
                                          const Vertex &thePrimVtx,
                                          FlavourTag &theTag )
{
  MsgStream log(msgSvc(), name());
  // The list of the particles the B is made of (including the B) (recursive).
  ParticleVector toExclude(0);
  log << MSG::DEBUG << "Flattening the decay of the B" << endreq;
  FlattenTree( const_cast<Particle *>(&theB), toExclude );

  tagExcludingFromList( theB, theEvent, toExclude, thePrimVtx, theTag );
}

void
SingleParticleTaggingTool::
tagExcludingFromList( const Particle &theB,
                      const Particles &theEvent,
                      const ParticleVector &theExcluded,
                      const Vertex &thePrimVtx,
                      FlavourTag &theTag )
{
  MsgStream log(msgSvc(), name());
  ParticleVector parts(theEvent.begin(),theEvent.end());
  ParticleVector::const_iterator i;
  ParticleVector::iterator k;
  log << MSG::DEBUG << "Building prefiltered list of candidates" << endreq;
  for( i = theExcluded.begin(); i != theExcluded.end(); i++ )
    if( (k = std::find(parts.begin(), parts.end(), *i)) != parts.end() )
      parts.erase( k );

  tagFromList( theB, parts, thePrimVtx, theTag );
}

void
SingleParticleTaggingTool::
tagExcludingFromList( const Particle &theB,
                      const ParticleVector &theEvent,
                      const ParticleVector &theExcluded,
                      const Vertex &thePrimVtx,
                      FlavourTag &theTag )
{
  MsgStream log(msgSvc(), name());
  ParticleVector parts(theEvent.begin(),theEvent.end());
  ParticleVector::const_iterator i;
  ParticleVector::iterator k;
  log << MSG::DEBUG << "Building prefiltered list of candidates" << endreq;
  for( i = theExcluded.begin(); i != theExcluded.end(); i++ )
    if( (k = std::find(parts.begin(), parts.end(), *i)) != parts.end() )
      parts.erase( k );

  tagFromList( theB, parts, thePrimVtx, theTag );
}

void SingleParticleTaggingTool::tagFromList( const Particle &theB,
                                             const ParticleVector &theParts,
                                             const Vertex &thePrimVtx, 
                                             FlavourTag &theTag )
{
  MsgStream log(msgSvc(), name());
  // Only keep particles which passe the cuts.
  ParticleVector candidates(0);
  log << MSG::DEBUG << "Filtering the candidates" << endreq;
  m_Filter->filter(theParts, candidates);

  // All the primary vertices in the event.
  std::vector<const Vertex *> vtxs(0);
  if( m_AllVertices )
  {
    std::vector<std::string>::const_iterator loc;
    for( loc = m_PrimVerticesLocations.begin();
         loc != m_PrimVerticesLocations.end();
         loc++ )
    {
      SmartDataPtr<Vertices> prim_vtxs( m_EventSvc, *loc );
      if( !prim_vtxs )
        continue;
      Vertices::iterator i;
      for( i = prim_vtxs->begin(); i != prim_vtxs->end(); i++ )
        vtxs.push_back(*i);
    }
  }
  else
    vtxs.push_back(&thePrimVtx);

  if( m_DoNTuple )
  {
    m_n_cands = 0;
    m_n_vtxs = vtxs.size();
    m_i_selected = 0;
    m_tag = 0;
  }

  log << MSG::DEBUG
      << "Filtering the remaining particles with the impact param" << endreq;
  // Find the one with the highest pt.
  ParticleVector::const_iterator c;
  double max_pt = -1.0;
  int i = 0;
  for( c = candidates.begin(); c != candidates.end(); c++ )
  {
    // First do the missing cut on the impact parameter.
    bool bad = false;
    std::vector<const Vertex *>::const_iterator v;
    int vi;
    for( vi = 0, v = vtxs.begin(); v != vtxs.end(); vi++, v++ )
    {
      double impact, error;
      m_GeomDisp->calcImpactPar( **c, **v, impact, error );
      if( m_DoNTuple && vi<5 )
      {
        m_ip[i][vi] = impact;
        m_iperr[i][vi] = error;
      }
      if( ((m_ISMin > 0) && (impact/error < m_ISMin)) ||
          ((m_ISMax > 0) && (impact/error > m_ISMax)) )
      {
        bad = true;
        break;
      }
    }
    if( !bad && ((*c)->pt() > max_pt) )
    {
      if( m_DoNTuple )
        m_i_selected = i;
      theTag.setTagger(*c);
      max_pt = (*c)->pt();
    }
    if( !bad && m_DoNTuple )
    {
      m_px[i] = (*c)->momentum().x();
      m_py[i] = (*c)->momentum().y();
      m_pz[i] = (*c)->momentum().z();
      m_vx[i] = (*c)->pointOnTrack().x();
      m_vy[i] = (*c)->pointOnTrack().y();
      m_vz[i] = (*c)->pointOnTrack().z();
      m_cl[i] = (*c)->confLevel();
      m_id[i] = (*c)->particleID().pid();
      i++;
      m_n_cands = i;
    }
  }
  log << MSG::DEBUG << "Computing tag" << endreq;
  // Use it's charge to determine the charge of the b quark.
  // Note: e- coming directly from a b means the tag should be a bbar.
  //       We are tagging the compagnion of the b quark this tagger comes from.
  int sign = m_InverseCharge ? -1 : 1;
  if( max_pt > 0 )
    if( theTag.tagger()->charge()*sign > 0) 
      theTag.setDecision( FlavourTag::bbar );
    else
      theTag.setDecision( FlavourTag::b );
  else
    theTag.setDecision( FlavourTag::none );
  if( m_DoNTuple )
  {
    switch( theTag.decision() )
    {
    case FlavourTag::b:
      m_tag = -1;
      break;
    case FlavourTag::bbar:
      m_tag = 1;
      break;
    case FlavourTag::none:
      m_tag = 0;
      break;
    }
    m_NTupleSvc->writeRecord( m_NTupleName );
  }
  theTag.setTaggedB( &theB );
  theTag.setType( FlavourTag::singleParticle );
  if( m_Monitor )
  {
    FlavourTag *mon = new FlavourTag(theTag);
    SmartDataPtr<FlavourTags> tags(m_EventSvc, m_MonitorLocation);
    if( tags )
      tags->insert(mon);
    else
    {
      FlavourTags *tags = new FlavourTags;
      tags->insert(mon);
      StatusCode sc = m_EventSvc->registerObject(m_MonitorLocation,tags);
      if( sc.isFailure() )
        log << MSG::ERROR << "Unable to register the tags under '"
            << m_MonitorLocation << "'" << endreq;
    }
  }
}
