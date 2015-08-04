// Include files 

// local
#include "MCVeloFEType.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MCVeloFEType
//
// 2005-11-22 : Tomasz Szumlak
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MCVeloFEType )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCVeloFEType::MCVeloFEType( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IMCVeloFEType>(this);
}
//=============================================================================
// Destructor
//=============================================================================
MCVeloFEType::~MCVeloFEType() {}
//
void MCVeloFEType::FEType(const LHCb::MCVeloFE* fe, int& feType)
{
  if(fabs(fe->addedNoise())>0.0&&(fe->addedSignal()==0.0)){
    feType=Noise;
  }
  if(fabs(fe->addedSignal())>0.0&&(fe->mcHits().size()!=0)){
    feType=Signal;
  }
  if(fabs(fe->addedSignal())>0.0&&(fe->mcHits().size()==0)){
    feType=Other;
  }
}
//
