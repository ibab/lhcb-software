// $Id: MCTupleToolDalitz.cpp,v 1.1 2009-02-17 18:07:31 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "MCTupleToolDalitz.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MCTupleToolDalitz
//
// 2009-02-17 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MCTupleToolDalitz );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCTupleToolDalitz::MCTupleToolDalitz( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IMCParticleTupleTool>(this);

}
//=============================================================================
// Destructor
//=============================================================================
MCTupleToolDalitz::~MCTupleToolDalitz() {} 

//=============================================================================
//=============================================================================
// Fill
//=============================================================================
StatusCode MCTupleToolDalitz::fill( const LHCb::MCParticle* mother
                                  , const LHCb::MCParticle* part
                                  , const std::string& head
                                  , Tuples::Tuple& tuple ){
  
  if (0==part) return StatusCode::FAILURE ;
  LHCb::MCParticle::ConstVector	dauts ;
  for ( SmartRefVector<LHCb::MCVertex>::const_iterator iv = part->endVertices().begin() ;
       iv != part->endVertices().end() ; ++iv){
    if (!(*iv)->isDecay()) continue ;
    const SmartRefVector< LHCb::MCParticle > pr = (*iv)->products() ;
    for ( SmartRefVector< LHCb::MCParticle >::const_iterator ip = pr.begin() ; ip != pr.end() ; ++ip){
      dauts.push_back(*ip);
    }
    break ;
  }

  if ( 2>=dauts.size() ){
    return Warning("Will not fill Dalitz of two body decay "+head,StatusCode::SUCCESS,0);
  }

  return fill(dauts,"MC",tuple) ;
  
}
