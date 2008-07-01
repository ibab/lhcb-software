// $Id: TupleToolGeneration.cpp,v 1.2 2008-07-01 15:13:00 pkoppenb Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "TupleToolGeneration.h"

#include "Event/GenHeader.h" 

#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/ITupleTool.h"

#include "GaudiKernel/IRegistry.h" // IOpaqueAddress

//-----------------------------------------------------------------------------
// Implementation file for class : GenerationTupleTool
//
// 2008-07-01 : Patrick Koppenburg
//-----------------------------------------------------------------------------

using namespace Gaudi;
using namespace LHCb;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TupleToolGeneration );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolGeneration::TupleToolGeneration( const std::string& type,
					const std::string& name,
					const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IEventTupleTool>(this);
}

//=============================================================================

StatusCode TupleToolGeneration::initialize() {
  if( ! GaudiTool::initialize() ) return StatusCode::FAILURE;

  return StatusCode::SUCCESS;
}

//=============================================================================

StatusCode TupleToolGeneration::fill( Tuples::Tuple& tuple ) {

  const LHCb::GenHeader* mch = get<LHCb::GenHeader>(LHCb::GenHeaderLocation::Default);
 
  unsigned int nc = mch->collisions().size() ;
  
  if( msgLevel( MSG::DEBUG ) ) debug() <<  nc << " Collisions " << endreq;

  std::vector<unsigned int> heaviestQuark ;
  std::vector<int> processType ;
  unsigned int hqEvent = 0 ;
  
  // quarks                                                                             
  for ( SmartRefVector< LHCb::GenCollision >::const_iterator ic = mch->collisions().begin() ;
        ic != mch->collisions().end() ; ++ic){
    if (0==*ic) Exception("Null collision pointer");
    const LHCb::HepMCEvent* gene = (*ic)->event() ;
    if (0==*ic) Exception("Null event pointer");
    processType.push_back((*ic)->processType());
    unsigned int hq = 0 ;
    for ( HepMC::GenEvent::particle_const_iterator p = gene->pGenEvt()->particles_begin();
          p != gene->pGenEvt()->particles_end();   ++p ) {
      if (msgLevel(MSG::VERBOSE)) verbose() << "Gen particle " << (*p)->pdg_id() << endmsg ;
      for ( unsigned int q = 6 ; q!=0 ; q--){
        if (  abs((*p)->pdg_id())==q) {
          if (q>hq) hq = q ;
          break ;
        }
      }
      if (hq==6) break ; // top heaviest
    }
    heaviestQuark.push_back(hq);
    if ( hq > hqEvent) hqEvent = hq ;
    if (msgLevel(MSG::VERBOSE)) verbose() << "Process type is " << (*ic)->processType() << ". Heaviest quark " << hq << endmsg ;
  }
  
  bool test = true;
  test &= tuple->farray( "ProcessType" ,  processType , "Collisions" , 20 );
  test &= tuple->farray( "HeaviestQuark", heaviestQuark, "Collisions" , 20 );
  test &= tuple->column( "HeaviestQuarkInEvent" , hqEvent );

  return StatusCode(test);
}
