// $Id: PVIPFilterCriterion.cpp,v 1.1 2004-06-25 08:00:21 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

// local
#include "PVIPFilterCriterion.h"
#include "DaVinciTools/IGeomDispCalculator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PVIPFilterCriterion
//
// 24/06/2004 : P. Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<PVIPFilterCriterion>          s_factory ;
const        IToolFactory& PVIPFilterCriterionFactory = s_factory ;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PVIPFilterCriterion::PVIPFilterCriterion( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : AlgTool ( type, name , parent ) {

  // declare additional interface
  declareInterface<IFilterCriterion>(this);

  // declare properties
  declareProperty( "MinIP", m_minIP = 0. );  
  declareProperty( "MaxIP", m_maxIP = -1. );  
  declareProperty( "MinIPsignif", m_minIPsignif = 0. );  
  declareProperty( "MaxIPsignif", m_maxIPsignif = -1. );  
}

//=============================================================================
// initialize() method
//=============================================================================
StatusCode PVIPFilterCriterion::initialize() {

  MsgStream msg( msgSvc(), name() );

  msg << MSG::DEBUG << ">>>   PVIPFilterCriterion::initialize() " 
      << endreq;
  StatusCode sc = service( "EventDataSvc", m_EDS, true );
  if( sc.isFailure() ) {
    msg << MSG::FATAL << " Unable to retrieve EventDataSvc " << endreq;
    return sc;
  }
  sc = toolSvc()->retrieveTool("GeomDispCalculator", m_ipTool, this);
  if(sc.isFailure()){
    msg << MSG::FATAL << " Unable to retrieve GeomDispCalculator tool" 
        << endreq;
    return sc;
  }

  msg << MSG::DEBUG << ">>>   Cuts are " << endreq;
  if (m_minIP>0.) msg << MSG::DEBUG << ">>>   Minimum IP: " << m_minIP 
                      << " mm" << endreq;    
  if (m_minIPsignif>0.) msg << MSG::DEBUG << ">>>   Minimum IP: " 
                            << m_minIPsignif << " sigma" << endreq;    
  if (m_maxIP>0.) msg << MSG::DEBUG << ">>>   Maximum IP: " << m_minIP 
                      << " mm" << endreq;    
  if (m_maxIPsignif>0.) msg << MSG::DEBUG << ">>>   Maximum IP: " 
                            << m_minIPsignif << " sigma" << endreq;    
  return StatusCode::SUCCESS;
}
//=============================================================================
// Test if filter is satisfied
//=============================================================================
bool PVIPFilterCriterion::isSatisfied( const Particle* const & part ) {

  MsgStream msg( msgSvc(), name() );

  SmartDataPtr<Vertices> PV(m_EDS,VertexLocation::Primary);

  msg << MSG::VERBOSE << ">>>> Looping on " << PV->size() << " PVs" << endreq;
  VertexVector::const_iterator iv;
  bool min_happy = true;
  // set to false if needed, true if not
  bool max_happy = (!(( m_maxIP > 0. ) || ( m_maxIPsignif > 0. )));
  for (iv=PV->begin();iv!=PV->end();++iv) {
    Vertex* v = *iv;
    if( v->type()!=Vertex::Primary) continue;
    double ip = -1 ,ipe = -1.;
    StatusCode sc = m_ipTool->calcImpactPar(*part, *v, ip, ipe);
    if (!sc.isSuccess()) continue;
    double ipr = fabs(ip/ipe);
    msg << MSG::VERBOSE << "IP is " << ip << "+/-" << ipe << " mm (" << ipr 
        << "sigma)" << endreq ;
    // MIN: special case when both are set -> both should be OK
    if ((( m_minIP > 0. ) && ( ip < m_minIP )) || 
        (( m_minIPsignif > 0. ) && ( ipr < m_minIPsignif ))) {
      min_happy = false ;
      msg << MSG::VERBOSE << "Breaking because of bad min IP" << endreq ;
      break ;    // found one bad PV
    }
    // MAX: special case when both are set -> both should be OK
    if (( m_maxIP > 0. ) && ( m_maxIPsignif > 0. )){
      if (( ip < m_maxIP ) && ( ipr < m_maxIPsignif )) max_happy = true;
    // normal case
    } else {
      if ((( m_maxIP > 0. ) && ( ip < m_maxIP )) ||
          (( m_maxIPsignif > 0. ) && ( ipr < m_maxIPsignif ))){
        max_happy = true ;
        msg << MSG::VERBOSE << "Breaking because of good max IP" << endreq;
        break ;
      }
    }   
  }
  
  msg << MSG::VERBOSE << "Happy for min: " << min_happy << " and for max: " 
      << max_happy << endreq ;
  return ( min_happy && max_happy );
}

//=============================================================================
// Test if filter is satisfied
//=============================================================================
bool PVIPFilterCriterion::operator()( const Particle* const & part ) {
  return this->isSatisfied( part );
}

//=============================================================================
