// $Id: LifetimeSignificanceCriterion.cpp,v 1.1.1.1 2004-08-24 06:35:15 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "Event/Particle.h"
#include "Event/Vertex.h"

#include "DaVinciTools/ILifetimeFitter.h"
#include "DaVinciTools/IGeomDispCalculator.h"

#include <assert.h>

// local
#include "LifetimeSignificanceCriterion.h"

//-----------------------------------------------------------------------------
// Implementation file for class : LifetimeSignificanceCriterion
//
// 22/06/2003 : Gerhard Raven
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<LifetimeSignificanceCriterion>          s_factory ;
const        IToolFactory& LifetimeSignificanceCriterionFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
LifetimeSignificanceCriterion::LifetimeSignificanceCriterion( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : AlgTool ( type, name , parent ),
    m_vAsct(0) ,m_ipTool(0)
{

  // declare additional interface
  declareInterface<IFilterCriterion>(this);

  // declare properties
  declareProperty( "UseAssociator", m_useAssoc = true );  
  declareProperty( "MinSignificance", m_minSig = -100000. );  
  declareProperty( "MinLifetime", m_minLife = -100000. );  
  declareProperty( "MaxLifetimeResolution", m_maxRes = +100000. );  
  declareProperty( "MaxChisq", m_maxChisq = 10000000. );  
  declareProperty( "MaxAssocWeight", m_maxAssocWeight = 10000000. );  
  declareProperty( "MinAssocWeight", m_minAssocWeight = -10000000. );  
}

//=============================================================================
// initialize() method
//=============================================================================
StatusCode LifetimeSignificanceCriterion::initialize() {

  MsgStream          log( msgSvc(), name() );
  StatusCode sc;
  if (m_useAssoc) {
          sc = toolSvc()->retrieveTool("Particle2VertexAsct", m_vAsct, this);
          if(sc.isFailure()){
             log << MSG::FATAL << " Unable to retrieve vertex Associator tool" << endreq;
             return sc;
          }
  } else {
          sc = toolSvc()->retrieveTool("GeomDispCalculator", m_ipTool, this);
          if(sc.isFailure()){
              log << MSG::FATAL << " Unable to retrieve GeomDispCalculator tool" << endreq;
              return sc;
          }
          StatusCode sc = service( "EventDataSvc", m_EDS, true );
          if( sc.isFailure() ) {
              log << MSG::FATAL << " Unable to retrieve EventDataSvc " << endreq;
              return sc;
          }
  }
  sc = toolSvc()->retrieveTool("LifetimeFitter", m_iLife, this);
  if(sc.isFailure()){
      log << MSG::FATAL << " Unable to retrieve LifetimeFitter" << endreq;
      return sc;
  }
     

  log << MSG::DEBUG << ">>>   LifetimeSignificanceCriterion::initialize() " 
      << endmsg;
  log << MSG::DEBUG << ">>>   Cuts are " << endmsg;
  log << MSG::DEBUG << ">>>   min. signif      " << m_minSig << endmsg;    
  log << MSG::DEBUG << ">>>   min. lifetime    " << m_minLife << endmsg;    
  log << MSG::DEBUG << ">>>   max. resol       " << m_maxRes << endmsg;    
  log << MSG::DEBUG << ">>>   max. chisq       " << m_maxChisq << endmsg;    
  log << MSG::DEBUG << ">>>   max. assoc. w    " << m_maxAssocWeight << endmsg;    
  log << MSG::DEBUG << ">>>   min. assoc. w    " << m_minAssocWeight << endmsg;    
  return StatusCode::SUCCESS;
}

//=============================================================================
// Test if filter is satisfied
//=============================================================================
bool 
LifetimeSignificanceCriterion::isSatisfied( const Particle* const & part ) 
{
        Vertex *v(0);
        double w=0;
        if (m_useAssoc) {
             assert(m_vAsct!=0);
             VerticesFromParticle range = m_vAsct->rangeFrom( part );
             for( VerticesFromParticleIterator i = range.begin();
                  i != range.end(); i++) {
                 double wi = i->weight();
                 Vertex *vi = i->to();
                 if (vi!=0 && (v==0 || wi<w)) { v = vi; w=wi; }
             }
         } else {
             assert(m_ipTool!=0);
             SmartDataPtr<Vertices> primaries(m_EDS,VertexLocation::Primary);
              for (VertexVector::const_iterator iv=primaries->begin();iv!=primaries->end();++iv) {
                  Vertex *vi = *iv;
                  if( vi->type()!=Vertex::Primary) continue;
                  double x,sx;
                  StatusCode sc = m_ipTool->calcImpactPar(*part, *vi, x, sx);
                  if (!sc.isSuccess()) continue;
                  double wi = fabs(x/sx);
                  if (v==0 || wi<w) { v = vi; w=wi; }
              }
         }
         if (v==0 || w>m_maxAssocWeight || w<m_minAssocWeight) return false;
         double ct,sct,chisq;
         StatusCode sc = m_iLife->fit(*v,*part,ct,sct,chisq);
         return !sc.isFailure()
             && chisq < m_maxChisq 
             && sct   < m_maxRes
             && ct    > std::max(m_minLife,m_minSig*sct );
}
