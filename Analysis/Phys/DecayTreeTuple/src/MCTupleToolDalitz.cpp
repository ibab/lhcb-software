// $Id: MCTupleToolDalitz.cpp,v 1.5 2009-11-24 17:22:25 pkoppenb Exp $
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
  , m_ppSvc(0)
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
  if (msgLevel(MSG::DEBUG)) debug() << "Decay of " << part->particleID().pid() << " with mother " << mother << endmsg ;
  LHCb::MCParticle::ConstVector	dauts ;
  for ( SmartRefVector<LHCb::MCVertex>::const_iterator iv = part->endVertices().begin() ;
       iv != part->endVertices().end() ; ++iv){
    if (!(*iv)->isDecay()) continue ;
    if (msgLevel(MSG::VERBOSE)) verbose() << "Decay vertex of type " << (*iv)->type() << " with " 
                                          << (*iv)->products().size() << " products" << endmsg ;
    const SmartRefVector< LHCb::MCParticle > pr = (*iv)->products() ;
    for ( SmartRefVector< LHCb::MCParticle >::const_iterator ip = pr.begin() ; ip != pr.end() ; ++ip){
      if (msgLevel(MSG::VERBOSE)) verbose() << "Pushing back " << (*ip)->particleID().pid() << endmsg ;
      dauts.push_back(*ip);
    }
    break ;
  }

  if ( 2>=dauts.size() ){
    return Warning("Will not fill Dalitz of two body decay "+head,StatusCode::SUCCESS,0);
  }

  return fill(dauts,"MC",tuple,(part->particleID().pid()<0)) ;
  
}
