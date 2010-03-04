// $Id: TupleToolPid.cpp,v 1.6 2010-03-04 14:02:03 xieyu Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "TupleToolPid.h"

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
DECLARE_TOOL_FACTORY( TupleToolPid );

using namespace LHCb;
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolPid::TupleToolPid( const std::string& type,
				      const std::string& name,
				      const IInterface* parent )
  : TupleToolBase ( type, name , parent )
  ,m_photonID(22)
  ,m_pi0ID(111)
{
  declareInterface<IParticleTupleTool>(this);
}

//=============================================================================

StatusCode TupleToolPid::fill( const Particle* 
				    , const Particle* P
				    , const std::string& head
				    , Tuples::Tuple& tuple )
{
  const std::string prefix=fullName(head);
  

  if( P ){
    bool test = true;
    test &= tuple->column( prefix+"_ID", P->particleID().pid() );

    if( !P->isBasicParticle() ) return StatusCode(test); // no rich info for composite!
    if( P->particleID().pid() == m_photonID  || 
        P->particleID().pid() == m_pi0ID ) return StatusCode(test); // no rich infrmation for neutrals
 
    const ProtoParticle* proto = P->proto();
    if( proto ){
      test &= tuple->column( prefix+"_PIDe"
			     ,proto->info(LHCb::ProtoParticle::CombDLLe,-1000));
      
      test &= tuple->column( prefix+"_PIDmu"
			     ,proto->info(LHCb::ProtoParticle::CombDLLmu,-1000));
      
      test &= tuple->column( prefix+"_PIDK"
			     ,proto->info(LHCb::ProtoParticle::CombDLLk,-1000));
      test &= tuple->column( prefix+"_PIDp"
			     ,proto->info(LHCb::ProtoParticle::CombDLLp,-1000));
      // uncomment it if one days set to something non zero..
      //       test &= tuple->column( prefix+"_PIDpi"
      // 			     ,proto->info(LHCb::ProtoParticle::CombDLLpi,-1000));
      test &= tuple->column( prefix+"_CaloEcalE"
			     ,proto->info(LHCb::ProtoParticle::CaloEcalE,-10000.));
      test &= tuple->column( prefix+"_CaloHcalE"
			     ,proto->info(LHCb::ProtoParticle::CaloHcalE,-10000.));

      bool hasMuon = false;
      bool isMuon = false;
      const MuonPID * muonPID = proto->muonPID();
      if(muonPID) {
        hasMuon = true;
        isMuon = muonPID->IsMuon();
      }
      test &= tuple->column( prefix+"_hasMuon", hasMuon);
      test &= tuple->column( prefix+"_isMuon", isMuon);

      bool hasRich = false;
      if(proto->richPID()) hasRich = true;
      test &= tuple->column( prefix+"_hasRich", hasRich);

      bool hasCalo = false;
      if(proto->calo().size()>0) hasCalo = true;
      test &= tuple->column( prefix+"_hasCalo", hasCalo);

      
      return StatusCode(test);
    }
  }
  return StatusCode::FAILURE;
}
