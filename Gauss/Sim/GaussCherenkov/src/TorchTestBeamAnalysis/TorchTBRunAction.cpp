// $Id: $
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h" 


// local
#include "TorchTBRunAction.h"
#include "TorchTBG4DefineHistSet6.h"

DECLARE_TOOL_FACTORY( TorchTBRunAction);

//-----------------------------------------------------------------------------
// Implementation file for class : TorchTBRunAction
//
// 2012-06-01 : Sajan Easo
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TorchTBRunAction::TorchTBRunAction(const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent   ) 
  : GiGaRunActionBase( type , name , parent ),
    m_ActivateDefineTorchTBHistoSet6(false),
    m_TorchTBG4DefineHistSet6(0)
{
  declareProperty("ActivateDefineHistoSet6", m_ActivateDefineTorchTBHistoSet6);
  

}
//=============================================================================
// Destructor
//=============================================================================
TorchTBRunAction::~TorchTBRunAction() {} 

//=============================================================================
void TorchTBRunAction::BeginOfRunAction( const G4Run* run ) 
{
  if( 0 == run )
  { Warning("BeginOfRunAction:: G4Run* points to NULL!") ; }
   
  if( m_ActivateDefineTorchTBHistoSet6 ) {
    m_TorchTBG4DefineHistSet6= new TorchTBG4DefineHistSet6(); 
    
  }
  
}

