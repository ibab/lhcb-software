// $Id: VertexChargeTaggingTool.cpp,v 1.4 2004-03-11 10:48:05 pkoppenb Exp $
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
#include "Event/ProtoParticle.h"

// from CLHEP
#include "CLHEP/Units/SystemOfUnits.h"

// from DaVinciTools
#include "DaVinciTools/IParticleFilter.h"
#include "DaVinciTools/IGeomDispCalculator.h"
#include "DaVinciTools/IVertexFitter.h"

// local
#include "VertexChargeTaggingTool.h"
#include "Event/FlavourTag.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VertexChargeTaggingTool
//
// 14/06/2003 : Olivier Dormond
//-----------------------------------------------------------------------------

// Declaration of the AlgTool Factory
static const ToolFactory<VertexChargeTaggingTool> s_factory;
const IToolFactory& VertexChargeTaggingToolFactory = s_factory;

//=============================================================================
// Standard creator, initializes variables
//=============================================================================
VertexChargeTaggingTool::VertexChargeTaggingTool( const std::string &type,
                                                      const std::string &name,
                                                      const IInterface *parent)
  : AlgTool( type, name, parent ), m_PrimVerticesLocations(0),
    m_vtxFitter(0), m_GeomDisp(0), m_EventSvc(0)
{
  declareInterface<IFlavourTaggingTool>(this);

  m_PrimVerticesLocations.push_back(VertexLocation::Primary);

  declareProperty( "GeomDispCalculator", m_GeomDispName="GeomDispCalculator" );
  declareProperty( "VertexFitter", m_vtxFitterName="UnconstVertexFitter" );
  declareProperty( "InverseCharge", m_InverseCharge = true );
  declareProperty( "PrimaryVertices", m_PrimVerticesLocations );
  declareProperty( "MinPCut", m_minP = 2.0*GeV );
  declareProperty( "MinThetaCut", m_minTheta = 0.0 );
  declareProperty( "MinSignificantIPPreCut", m_minSIPPreCut = 3.0 );
  declareProperty( "MaxSeedIPErrorCut", m_maxSeedIPErr = 1.0 );
  declareProperty( "MinSeedSignificantIPCut", m_minSeedSIP = 2.0 );
  declareProperty( "MaxSeedSignificantIPCut", m_maxSeedSIP = 100.0 );
  declareProperty( "MaxSeedChi2NDoFCut", m_maxSeedChi2NDoF = 10.0 );
  declareProperty( "MinSeedPrimDistanceCut", m_minSeedPrimDist = 1.0*mm );
  declareProperty( "MinKshortMassWindowCut", m_minKS0Mass = 0.49*GeV );
  declareProperty( "MaxKshortMassWindowCut", m_maxKS0Mass = 0.505*GeV );
  declareProperty( "MinSeedLikelyhoodCut", m_minSeedLikelyhood = 0.32 );
  declareProperty( "MaxIPErrorCut", m_maxIPErr = 1.5 );
  declareProperty( "MinSignificantIPCut", m_minSIP = 1.8 );
  declareProperty( "MaxSignificantIPCut", m_maxSIP = 100.0 );
  declareProperty( "MinLikelyhoodCut", m_minLikelyhood = 0.2 );
  declareProperty( "MaxChi2NDoFCut", m_maxChi2NDoF = 5.0 );
  declareProperty( "MinPrimDistCut", m_minPrimDist = 1.0*mm );
  declareProperty( "MaxAdditionalSignificantIPCut", m_maxAddSIP = 3.0 );
}

StatusCode VertexChargeTaggingTool::initialize()
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
  StatusCode sc = toolsvc->retrieveTool(m_GeomDispName, m_GeomDisp, this);
  if( !sc.isSuccess() )
  {
    log << MSG::FATAL << "GeomDispCalculator tool named '" << m_GeomDispName
        << "' could not be found" << endreq;
    return StatusCode::FAILURE;
  }
  sc = toolsvc->retrieveTool(m_vtxFitterName, m_vtxFitter, this);
  if( !sc.isSuccess() )
  {
    log << MSG::FATAL << "Vertex fitter tool named '" << m_vtxFitterName
        << "' could not be found" << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

void VertexChargeTaggingTool::FlattenTree( Particle *part,
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

void VertexChargeTaggingTool::tagThisB( const Particle &theB,
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

void VertexChargeTaggingTool::tagThisB( const Particle &theB,
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
VertexChargeTaggingTool::
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
  for( i = theExcluded.begin(); i != theExcluded.end(); i++ )
  {
    bool kill = false;
    for( k = parts.begin(); k != parts.end(); k++ )
      if( (*i)->origin() != NULL && (*i)->origin() == (*k)->origin() )
      {
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
VertexChargeTaggingTool::
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
  for( i = theExcluded.begin(); i != theExcluded.end(); i++ )
  {
    bool kill = false;
    for( k = parts.begin(); k != parts.end(); k++ )
      if( (*i)->origin() != NULL && (*i)->origin() == (*k)->origin() )
      {
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

double ipprob(double x) {
  if( x > 40. ) return 0.6;
  return - 0.535 + 0.3351*x - 0.03102*pow(x,2) + 0.001316*pow(x,3)
    - 0.00002598*pow(x,4) + 0.0000001919*pow(x,5);
}

double ptprob(double x) {
  if( x > 5.0 ) return 0.65;
  return 0.04332 + 0.9493*x - 0.5283*pow(x,2) + 0.1296*pow(x,3)
    - 0.01094*pow(x,4);
}

double aprob(double x) {
  if( x < 0.02 ) return 0.32;
  return 0.4516 - 1.033*x;
}

void VertexChargeTaggingTool::tagFromList( const Particle &theB,
                                           const ParticleVector &theParts,
                                           const Vertex &thePrimVtx, 
                                           FlavourTag &theTag )
{
  MsgStream log(msgSvc(), name());

  log << MSG::DEBUG
      << "Entering the active region of the VertexChargeTaggingTool"
      << endreq;

  // All the primary vertices in the event but thePrimVtx.
  std::vector<const Vertex *> vtxs(0);
  std::vector<std::string>::const_iterator loc;
  for( loc = m_PrimVerticesLocations.begin();
       loc != m_PrimVerticesLocations.end();
       loc++ ) {
    SmartDataPtr<Vertices> prim_vtxs( m_EventSvc, *loc );
    if( !prim_vtxs )
      continue;
    Vertices::iterator i;
    for( i = prim_vtxs->begin(); i != prim_vtxs->end(); i++ ) {
      if( (*i)->type() != Vertex::Primary )
        continue;
      if( (*i)->position().distance(thePrimVtx.position())/mm < 0.00001 )
        continue;
      vtxs.push_back(*i);
    }
  }

  log << MSG::DEBUG << "Prefiltering the particles" << endreq;
  ParticleVector candidates(0);
  ParticleVector::const_iterator c;
  for( c = theParts.begin(); c != theParts.end(); c++ ) {
    if( (*c)->particleID().abspid() == 13 ) continue; // remove muons
    if( (*c)->charge() == 0.0 ) continue; // remove neutrals
    if( (*c)->p() < m_minP ) continue; // remove soft particles
    if( (*c)->momentum().theta() < m_minTheta ) continue; // remove beam pipe
    bool isolated = true;
    std::vector<const Vertex *>::const_iterator vi;
    for( vi = vtxs.begin(); vi != vtxs.end(); vi++ ) {
      double impact, error;
      StatusCode sc = m_GeomDisp->calcImpactPar( **c, **vi, impact, error );
      if( sc && (error != 0.0) && (impact/error < m_minSIPPreCut) )
        isolated = false;
    }
    if( !isolated ) continue; // remove pile-up compatible parts
    if( !(*c)->origin() ) continue; // remove parts without tracks
    ProtoParticle *proto = dynamic_cast<ProtoParticle *>((*c)->origin());
    if( proto->track() && !proto->track()->unique() && 
        (proto->track()->veloTT() || proto->track()->velo() ||
         proto->track()->seed()) ) continue; // remove short tracks

    candidates.push_back(*c);
  }

  log << MSG::DEBUG << "Number of particles after prefiltering: "
      << candidates.size() << endreq;

  ParticleVector KshortsProds;

  double maxlikelyhood = 0.0;
  Particle *seed1 = NULL, *seed2 = NULL;
  ParticleVector::const_iterator c1, c2;
  for( c1 = candidates.begin(); c1 != candidates.end(); c1++ ) {
    ProtoParticle *proto = dynamic_cast<ProtoParticle *>((*c1)->origin());
    if( !proto->track()->forward() ) continue; // only accept forward tracks
    double impact1, error1;
    StatusCode sc = m_GeomDisp->calcImpactPar(**c1,thePrimVtx,impact1,error1);
    if( sc.isFailure() ) continue;
    if( error1 > m_maxSeedIPErr ) continue;
    if((impact1/error1<m_minSeedSIP) || (impact1/error1>m_maxSeedSIP)) continue;
    if( std::find(KshortsProds.begin(), KshortsProds.end(), *c1)
        != KshortsProds.end() )
      continue; // This is a pion from a Kshort

    for( c2 = c1+1; c2 != candidates.end(); c2++ ) {
      ProtoParticle *proto = dynamic_cast<ProtoParticle *>((*c2)->origin());
      if( !proto->track()->forward() ) continue; // only accept forward tracks
      double impact2, error2;
      StatusCode sc = m_GeomDisp->calcImpactPar(**c2,thePrimVtx,impact2,error2);
      if( sc.isFailure() ) continue;
      if( error2 > m_maxSeedIPErr ) continue;
      if((impact2/error2<m_minSeedSIP) || (impact2/error2>m_maxSeedSIP)) continue;
      if( std::find(KshortsProds.begin(), KshortsProds.end(), *c2)
          != KshortsProds.end() )
        continue; // This is a pion from a Kshort

      Vertex vtx;
      sc = m_vtxFitter->fitVertex(**c1, **c2, vtx);
      if( sc.isFailure() ) continue;
      if( vtx.chi2()/vtx.nDoF() > m_maxSeedChi2NDoF ) continue;
      if( vtx.position().z() - thePrimVtx.position().z() < m_minSeedPrimDist )
        continue;
      if( ((*c1)->particleID().abspid() == 211) &&
          ((*c2)->particleID().abspid() == 211) &&
          ((*c1)->particleID().pid() != (*c2)->particleID().pid()) &&
          (((*c1)->momentum() + (*c2)->momentum()).m() > m_minKS0Mass ) &&
          (((*c1)->momentum() + (*c2)->momentum()).m() < m_maxKS0Mass ) ) {
        KshortsProds.push_back(*c1);
        KshortsProds.push_back(*c2);
        c1 = candidates.begin();
        c2 = c1+1;
        seed1 = seed2 = NULL;
        maxlikelyhood = 0.0;
        continue;
      }
      double probip1 = ipprob(impact1/error1);
      double probip2 = ipprob(impact2/error2);
      double probpt1 = ptprob((*c1)->pt()/GeV);
      double probpt2 = ptprob((*c2)->pt()/GeV);
      double proba = aprob((*c1)->momentum().vect().angle((*c2)->momentum().vect()));
      double probs = probip1*probip2*probpt1*probpt2*proba;
      double probb = (1-probip1)*(1-probip2)*(1-probpt1)*(1-probpt2)*(1-proba);
      double likelyhood = probs/(probs+probb);
      if( likelyhood > maxlikelyhood ) {
        maxlikelyhood = likelyhood;
        seed1 = *c1;
        seed2 = *c2;
      }
    }
  }

  theTag.setDecision( FlavourTag::none );
  theTag.setTaggedB( &theB );
  theTag.setType( FlavourTag::vertexCharge );
  
  if( maxlikelyhood < m_minSeedLikelyhood )
    return;

  log << MSG::DEBUG << "Found a seed" << endreq;

  log << MSG::DEBUG << "Seed 1: P: " << seed1->p() << " Pt: " << seed1->pt() << " ID: " << seed1->particleID().pid() << endreq;
  log << MSG::DEBUG << "Seed 2: P: " << seed2->p() << " Pt: " << seed2->pt() << " ID: " << seed2->particleID().pid() << endreq;
  
  ParticleVector vtxProds;
  vtxProds.clear();
  vtxProds.push_back(seed1);
  vtxProds.push_back(seed2);
  for( c = candidates.begin(); c != candidates.end(); c++ ) {
    int pid = (*c)->particleID().abspid();
    if( (pid != 321) && (pid != 211) && (pid != 11) ) continue;
    if( ((*c) == seed1) || ((*c) == seed2) ) continue;
    ProtoParticle *proto = dynamic_cast<ProtoParticle *>((*c)->origin());
    if( proto->track()->isDownstream() ) continue;
    double impact, error;
    StatusCode sc = m_GeomDisp->calcImpactPar(**c,thePrimVtx,impact,error);
    if( sc.isFailure() ) continue;
    if( error > m_maxIPErr ) continue;
    if( (impact/error<m_minSIP ) || (impact/error>m_maxSIP)) continue;
    double probip = ipprob(impact/error);
    double probpt = ptprob((*c)->pt()/GeV);
    if( probip < 0.0 ) probip = 0.0;
    if( probpt < 0.0 ) probpt = 0.0;
    double probs = probip*probpt;
    double probb = (1-probip)*(1-probpt);
    double likelyhood = probs/(probs+probb);
    if( likelyhood < m_minLikelyhood ) continue;
    // Check it's not the decay products of a Kshort.
    if( std::find(KshortsProds.begin(), KshortsProds.end(), *c) !=
        KshortsProds.end() )
      continue;
    vtxProds.push_back(*c);
    Vertex vtx;
    sc = m_vtxFitter->fitVertex(vtxProds, vtx);
    if( sc.isFailure() ) {
      vtxProds.pop_back();
      continue;
    }
    if( (vtx.chi2()/vtx.nDoF() > m_maxChi2NDoF) ) {
      vtxProds.pop_back();
      continue;
    }
    if( (vtx.position().z() - thePrimVtx.position().z() < m_minPrimDist) ) {
      vtxProds.pop_back();
      continue;
    }
    ParticleVector::const_iterator wi;
    int i, worst = -1;
    double sipmax = -1.0;
    for( i = 0, wi = vtxProds.begin();
         (wi != vtxProds.end()) && (vtxProds.size() > 2); i++, wi++){
      ParticleVector remains = vtxProds;
      remains.erase(remains.begin()+i);
      StatusCode sc = m_vtxFitter->fitVertex(remains, vtx);
      if( sc.isFailure() ) {
        worst = i;
        sipmax = 100.0;
        break;
      }
      double impact, error;
      sc = m_GeomDisp->calcImpactPar(**wi, vtx, impact, error);
      if( sc.isFailure() ) {
        worst = i;
        sipmax = 100.0;
        break;
      }
      if( impact/error > sipmax ) {
        sipmax = impact/error;
        worst = i;
      }
    }
    if( (worst != -1) && (vtxProds.size()>2) && (sipmax > m_maxAddSIP) )
      vtxProds.erase(vtxProds.begin()+worst);
  }
  Vertex vtx;
  StatusCode sc = m_vtxFitter->fitVertex(vtxProds,vtx);
  if( sc.isFailure() )
    vtxProds.clear();
  
  log << MSG::DEBUG << "Computing tag" << endreq;
  double charge = 0.0;
  for( c = vtxProds.begin(); c != vtxProds.end(); c++ )
    charge += (*c)->charge();

  int sign = m_InverseCharge ? -1 : 1;
  if( charge*sign > 0 ) 
    theTag.setDecision( FlavourTag::bbar );
  else if( charge*sign < 0 )
    theTag.setDecision( FlavourTag::b );
}
