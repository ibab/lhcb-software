// $Id: RichG4RunConditionAction.cpp,v 1.1 2005-09-30 12:14:29 seaso Exp $
// Include files 
/// GaudiKernel
#include "GaudiKernel/PropertyMgr.h"
/// GiGa
#include "GiGa/GiGaMACROs.h"



// local
#include "RichG4RunConditionAction.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichG4RunConditionAction
//
// 2005-08-29 : Sajan EASO
//-----------------------------------------------------------------------------
/// Factory business
// ============================================================================
IMPLEMENT_GiGaFactory( RichG4RunConditionAction ) ;
// ============================================================================

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichG4RunConditionAction::RichG4RunConditionAction
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : GiGaRunActionBase( type , name , parent )
  , m_FirstTimeOfBeginRichCondRun(true)
  ,  m_ActivateRichSimCondition(false)
{
  
  declareProperty("ActivateRichSimCondition",m_ActivateRichSimCondition);
  
}


//=============================================================================
// Destructor
//=============================================================================
RichG4RunConditionAction::~RichG4RunConditionAction() {
}; 

//=============================================================================
StatusCode RichG4RunConditionAction::initialize()
{
  /// initialize the base class
  StatusCode sc = GiGaRunActionBase::initialize();
  if( sc.isFailure() )
    { return Error("RichG4RunConditionAction: Could not initialize the RunAction base class!");}
 
    if( m_ActivateRichSimCondition ) {        

       if(m_FirstTimeOfBeginRichCondRun) {

         m_RichConditionsOverall=
          tool<IRichConditionsOverall>("RichConditionsOverall",this);      
        
         m_FirstTimeOfBeginRichCondRun=false;
       }
       
    }
  
  return StatusCode::SUCCESS;
  
}
