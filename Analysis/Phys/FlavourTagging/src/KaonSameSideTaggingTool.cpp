// $Id:
#include <algorithm>
#include <iomanip>

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "Event/EventHeader.h"
#include "Event/Vertex.h"

// from CLHEP
#include "CLHEP/Units/SystemOfUnits.h"

// from DaVinciTools
#include "DaVinciTools/IParticleFilter.h"
#include "DaVinciTools/IGeomDispCalculator.h"

// local
#include "KaonSameSideTaggingTool.h"
#include "Event/FlavourTag.h"

//-----------------------------------------------------------------------------
// Implementation file for class : KaonSameSideTaggingTool
//
// 28/03/2003 : Olivier Dormond based on Marco Musy fortran code
//-----------------------------------------------------------------------------

// Declaration of the AlgTool Factory
static const ToolFactory<KaonSameSideTaggingTool> s_factory;
const IToolFactory& KaonSameSideTaggingToolFactory = s_factory;

//=============================================================================
// Standard creator, initializes variables
//=============================================================================
KaonSameSideTaggingTool::KaonSameSideTaggingTool( const std::string &type,
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
  declareProperty( "InverseCharge", m_InverseCharge = false );
  declareProperty( "AllVertices", m_AllVertices = true );
  declareProperty( "MinImpactSignificance", m_ISMin = -1 );
  declareProperty( "MaxImpactSignificance", m_ISMax = -1 );
  declareProperty( "MaxPhi", m_dPhiCut = 0.9 );
  declareProperty( "MaxEta", m_dEtaCut = 1.0 );
  declareProperty( "MaxQ", m_dQCut = 1500 );
  declareProperty( "Monitored", m_Monitor = false );
  declareProperty( "MonitorLocation", m_MonitorLocation = "auto" );
}

StatusCode KaonSameSideTaggingTool::initialize()
{
  MsgStream log(msgSvc(), name());
  IToolSvc* toolsvc = NULL;
  if( serviceLocator() ) {
    serviceLocator()->service("ToolSvc", toolsvc);
    serviceLocator()->service("EventDataSvc", m_EventSvc);
  }
  if( !toolsvc ) {
    log << MSG::FATAL << "Unable to locate Tool Service" << endreq;
    return StatusCode::FAILURE;
  }
  if( !m_EventSvc ) {
    log << MSG::FATAL << "Unable to find the requested Event Service '"
        << "EventDataSvc" << "'" << endreq;
    return StatusCode::FAILURE;
  }
  StatusCode sc = toolsvc->retrieveTool(m_FilterName, m_Filter, this);
  if( !sc.isSuccess() ) {
    log << MSG::FATAL << "Unable to find the requested filter '"
        << m_FilterName << "'" << endreq;
    return StatusCode::FAILURE;
  }
  sc = toolsvc->retrieveTool(m_GeomDispName, m_GeomDisp);
  if( !sc.isSuccess() ) {
    log << MSG::FATAL << "GeomDispCalculator tool named '" << m_GeomDispName
        << "' could not be found" << endreq;
    return StatusCode::FAILURE;
  }

  std::string name_tail =
    (name().rfind('.') == std::string::npos) ? name()
                                        : name().substr(name().rfind('.')+1); 

  if( m_Monitor && ( m_MonitorLocation == "auto" || m_MonitorLocation == "Auto"
                     || m_MonitorLocation == "AUTO" ) )
    m_MonitorLocation = "/Event/Phys/Monitor/"+name_tail+"/Tags";

  log << MSG::DEBUG << "Monitoring data go to: " << m_MonitorLocation << endreq;

  return StatusCode::SUCCESS;
}

void KaonSameSideTaggingTool::FlattenTree( Particle *part,
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

void KaonSameSideTaggingTool::tagThisB( const Particle &theB,
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

void KaonSameSideTaggingTool::tagThisB( const Particle &theB,
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
KaonSameSideTaggingTool::
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
  int n = 0;
  for( i = theExcluded.begin(); i != theExcluded.end(); i++ ) {
    bool kill = false;
    for( k = parts.begin(); k != parts.end(); k++ )
      if( (*i)->origin() != NULL && (*i)->origin() == (*k)->origin() ) {
        n++;
        kill = true;
        break;
      }
    if( kill )
      parts.erase( k );
  }
  log << MSG::DEBUG << "Number of particles removed: " << n << endreq;

  tagFromList( theB, parts, thePrimVtx, theTag );
}

void
KaonSameSideTaggingTool::
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
  int n = 0;
  log << MSG::DEBUG << "Building prefiltered list of candidates" << endreq;
  for( i = theExcluded.begin(); i != theExcluded.end(); i++ ) {
    bool kill = false;
    for( k = parts.begin(); k != parts.end(); k++ )
      if( (*i)->origin() != NULL && (*i)->origin() == (*k)->origin() ) {
        n++;
        kill = true;
        break;
      }
    if( kill )
      parts.erase( k );
  }
  log << MSG::DEBUG << "Number of particles removed: " << n << endreq;

  tagFromList( theB, parts, thePrimVtx, theTag );
}

void KaonSameSideTaggingTool::tagFromList( const Particle &theB,
                                           const ParticleVector &theParts,
                                           const Vertex &thePrimVtx, 
                                           FlavourTag &theTag )
{
  MsgStream log(msgSvc(), name());

  log << MSG::DEBUG
      << "Entering the active region of the KaonSameSideTaggingTool"
      << endreq;
  
  // Only keep particles which passe the cuts.
  ParticleVector candidates(0);
  log << MSG::DEBUG << "Filtering the candidates" << endreq;
  m_Filter->filter(theParts, candidates);
  log << MSG::DEBUG << "Kept " << candidates.size() << " out of "
      << theParts.size() << endreq;

  // All the primary vertices in the event.
  std::vector<const Vertex *> vtxs(0);
  if( m_AllVertices ) {
    std::vector<std::string>::const_iterator loc;
    for( loc = m_PrimVerticesLocations.begin();
         loc != m_PrimVerticesLocations.end();
         loc++ ) {
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

  log << MSG::DEBUG
      << "Filtering the remaining particles with the impact param" << endreq;
  // Find the one with the highest pt.
  ParticleVector::const_iterator c;
  double max_pt = -1.0;
  for( c = candidates.begin(); c != candidates.end(); c++ ) {
    // First do the missing cut on the impact parameter.
    bool bad = false;
    std::vector<const Vertex *>::const_iterator v;
    int vi;
    for( vi = 0, v = vtxs.begin(); v != vtxs.end(); vi++, v++ ) {
      double impact, error;
      m_GeomDisp->calcImpactPar( **c, **v, impact, error );
      if( ((m_ISMin > 0) && (impact/error < m_ISMin)) ||
          ((m_ISMax > 0) && (impact/error > m_ISMax)) ) {
        bad = true;
        log << MSG::DEBUG << "Rejecting: ip/err = "
            << impact << '/' << error << '=' << impact/error
            << " not in [" << m_ISMin << ',' << m_ISMax << ']' << endreq;
        break;
      }
    }
    double deta = fabs(theB.momentum().pseudoRapidity()
                       - (*c)->momentum().pseudoRapidity());
    double dphi = fabs(theB.momentum().phi() - (*c)->momentum().phi());
    double dQ   = (theB.momentum()+(*c)->momentum()).m() - theB.mass();
    if( deta >= m_dEtaCut ) {
      bad = true;
      log << MSG::DEBUG << "Rejecting: deta = " << deta << ">=" << m_dEtaCut
          << endreq;
    } else if( dphi >= m_dPhiCut ) {
      bad = true;
      log << MSG::DEBUG << "Rejecting: dphi = " << dphi << ">=" << m_dPhiCut
          << endreq;
    } else if( dQ >= m_dQCut ) {
      bad = true;
      log << MSG::DEBUG << "Rejecting: deta = " << dQ << ">=" << m_dQCut
          << endreq;
    }
    if( !bad && ((*c)->pt() > max_pt) ) {
      theTag.setTagger(*c);
      max_pt = (*c)->pt();
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

  // Save a copy for the monitor
  if( m_Monitor ) {
    FlavourTag *mon = new FlavourTag(theTag);
    SmartDataPtr<FlavourTags> tags(m_EventSvc, m_MonitorLocation);
    if( tags )
      tags->insert(mon);
    else {
      FlavourTags *tags = new FlavourTags;
      tags->insert(mon);
      StatusCode sc = m_EventSvc->registerObject(m_MonitorLocation,tags);
      if( sc.isFailure() )
        log << MSG::ERROR << "Unable to register the tags under '"
            << m_MonitorLocation << "'" << endreq;
    }
  }
}
