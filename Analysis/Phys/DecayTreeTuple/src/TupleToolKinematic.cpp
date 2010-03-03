// $Id: TupleToolKinematic.cpp,v 1.4 2010-03-03 14:26:34 pkoppenb Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "TupleToolKinematic.h"

#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"

#include "Event/Particle.h"
//-----------------------------------------------------------------------------
// Implementation file for class : EventInfoTupleTool
//
// 2007-11-07 : Jeremie Borel
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
// actually acts as a using namespace TupleTool
DECLARE_TOOL_FACTORY( TupleToolKinematic );
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolKinematic::TupleToolKinematic( const std::string& type,
				    const std::string& name,
				    const IInterface* parent )
  : TupleToolBase ( type, name , parent )
{
  declareInterface<IParticleTupleTool>(this);
}

//=============================================================================

StatusCode TupleToolKinematic::fill( const LHCb::Particle* 
				   , const LHCb::Particle* P
				   , const std::string& head
				   , Tuples::Tuple& tuple )
{
  const std::string prefix=fullName(head);
  
  bool test = true;
  if( P )
  {
    test &= tuple->column( prefix+"_P", P->p() );
    test &= tuple->column( prefix+"_PT", P->pt() );

    // momentum components
    test &= tuple->column( prefix+"_P", P->momentum() );
    
    // reference point:
    if(isVerbose()) test &= tuple->column( prefix+"_REFP", P->referencePoint() );
    // mass before fit (what CombinationCut cuts on)
    if(isVerbose() && !(P->isBasicParticle()) ) test &= tuple->column( prefix+"_PreFitMass", preFitMass(P) );
    
    test &= tuple->column( prefix+"_MM", P->measuredMass() );
    if( !P->isBasicParticle() )
      test &= tuple->column( prefix+"_MMERR", P->measuredMassErr() );
    test &= tuple->column( prefix+"_M", P->momentum().M() );
  } 
  else 
  {
    return StatusCode::FAILURE;
  }
  return StatusCode(test);
}
//=============================================================================
double TupleToolKinematic::preFitMass(const LHCb::Particle* p) const {
  
  Gaudi::LorentzVector Mom ;
  for ( LHCb::Particle::ConstVector::const_iterator d = p->daughtersVector().begin();
        d != p->daughtersVector().end() ; ++d){
    Mom += (*d)->momentum();
  }
  return Mom.M() ;
}
