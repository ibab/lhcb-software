// $Id: SingleParticleTaggingTool.cpp,v 1.1.1.1 2002-05-23 23:25:51 gcorti Exp $
#include <algorithm>
#include <iomanip>

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

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
    m_Filter(0), m_GeomDisp(0), m_EventSvc(0)
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
  int size = 0;
  if( m_AllVertices )
  {
    std::vector<std::string>::const_iterator loc;
    for( loc = m_PrimVerticesLocations.begin();
         loc != m_PrimVerticesLocations.end();
         loc++ )
    {
      SmartDataPtr<VertexVector> prim_vtxs( m_EventSvc, *loc );
      if( prim_vtxs )
        size += prim_vtxs->size();
    }
  }
  else
    size = 1;

  std::vector<const Vertex *> vtxs(size);
  if( m_AllVertices )
  {
    std::vector<std::string>::const_iterator loc;
    for( loc = m_PrimVerticesLocations.begin();
         loc != m_PrimVerticesLocations.end();
         loc++ )
    {
      SmartDataPtr<VertexVector> prim_vtxs( m_EventSvc, *loc );
      if( !prim_vtxs )
        continue;
      VertexVector::iterator i;
      for( i = prim_vtxs->begin(); i != prim_vtxs->end(); i++ )
        vtxs.push_back(*i);
    }
  }
  else
    vtxs.push_back(&thePrimVtx);

  log << MSG::DEBUG
      << "Filtering the remaining particles with the impact param" << endreq;
  // Find the one with the highest pt.
  ParticleVector::const_iterator i;
  double max_pt = -1.0;
  for( i = candidates.begin(); i != candidates.end(); i++ )
  {
    // First do the missing cut on the impact parameter.
    bool bad = false;
    std::vector<const Vertex *>::const_iterator v;
    for( v = vtxs.begin(); v != vtxs.end(); v++ )
    {
      double impact, error;
      m_GeomDisp->calcImpactPar( **i, **v, impact, error );
      if( ((m_ISMin > 0) && (impact/error < m_ISMin)) ||
          ((m_ISMax > 0) && (impact/error > m_ISMax)) )
      {
        bad = true;
        break;
      }
    }
    if( !bad && ((*i)->pt() > max_pt) )
    {
      theTag.setTagger(*i);
      max_pt = (*i)->pt();
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
  theTag.setTaggedB( &theB );
  theTag.setType( FlavourTag::singleParticle );
}
