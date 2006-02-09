// $Id: MCVeloFEType.cpp,v 1.1 2006-02-09 12:20:41 szumlat Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "MCVeloFEType.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MCVeloFEType
//
// 2005-11-22 : Tomasz Szumlak
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<MCVeloFEType>          s_factory ;
const        IToolFactory& MCVeloFETypeFactory = s_factory ; 


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
MCVeloFEType::~MCVeloFEType() {};
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
