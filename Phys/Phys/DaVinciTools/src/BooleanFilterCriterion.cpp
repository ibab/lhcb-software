// $Id: BooleanFilterCriterion.cpp,v 1.1 2004-03-23 17:30:39 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h" 

// local
#include "BooleanFilterCriterion.h"

//-----------------------------------------------------------------------------
// Implementation file for class : BooleanFilterCriterion
//
// 18/03/2004 : Gerhard Raven
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<BooleanFilterCriterion>          s_factory ;
const        IToolFactory& BooleanFilterCriterionFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BooleanFilterCriterion::BooleanFilterCriterion( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent )
  : AlgTool ( type, name , parent ) {

  // declare additional interface
  declareInterface<IFilterCriterion>(this);

  // declare properties
  declareProperty( "OrList", m_orListNames );  
  declareProperty( "AndList", m_andListNames );  
  declareProperty( "FlipResult", m_flipResult = false );  
}

//=============================================================================
// initialize() method
//=============================================================================
StatusCode BooleanFilterCriterion::initialize() {

 
  StatusCode sc = getCriteria(m_andListNames,m_andList," AND list ");
  if (!sc.isFailure()) {
             sc = getCriteria(m_orListNames,m_orList," OR list ");
  }
  return sc;
}

StatusCode BooleanFilterCriterion::getCriteria(const std::vector<std::string>& names,
                                               std::vector<IFilterCriterion*>& list,
                                               const char* listname) const {
  MsgStream          log( msgSvc(), name() );
  std::vector<std::string>::const_iterator i;
  for ( i = names.begin(); i != names.end(); ++i ) {
    IFilterCriterion* icrit;
    StatusCode sc = toolSvc()->retrieveTool( *i, icrit, this );
    if( sc.isFailure() ) return StatusCode::FAILURE;
    list.push_back( icrit );
    log << MSG::DEBUG << ">>> " << listname << ": added " << *i << " successfully" 
        << endreq;
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Test if filter is satisfied
//=============================================================================
bool BooleanFilterCriterion::isSatisfied( const Particle* const & part ) {
        bool pass=true;
        std::vector<IFilterCriterion*>::const_iterator i;
        for ( i=m_andList.begin(); pass && i!=m_andList.end(); ++i) 
                pass = (*i)->isSatisfied(part);
        if (pass && !m_orList.empty()) {
                pass = false;
                for ( i=m_orList.begin(); !pass && i!=m_orList.end(); ++i) 
                        pass = (*i)->isSatisfied(part);
        }
        if ( m_flipResult ) pass = !pass;
        return pass;
}
//=============================================================================
// Finalize
//=============================================================================
StatusCode BooleanFilterCriterion::finalize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << ">>> Release criteria tools" << endreq;
  
  std::vector<IFilterCriterion*>::iterator i;
  for (i=m_andList.begin();i!=m_andList.end();++i) toolSvc()->releaseTool(*i);
  m_andList.clear();
  for (i=m_orList.begin();i!=m_orList.end();++i) toolSvc()->releaseTool(*i);
  m_orList.clear();

  return StatusCode::SUCCESS;
}

//=============================================================================
