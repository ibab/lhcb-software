// $Id: TupleToolMuonPid.cpp,v 1.6 2011-09-01 14:02:03 kakiba Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "TupleToolMuonPid.h"

#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"

#include "Event/Particle.h"
//-----------------------------------------------------------------------------
// Implementation file for class : EventInfoTupleTool
//
// 2007-11-07 : Jeremie Borel
//-----------------------------------------------------------------------------

using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolMuonPid::TupleToolMuonPid( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : TupleToolBase ( type, name , parent )
{
  declareInterface<IParticleTupleTool>(this);
}

//=============================================================================

StatusCode TupleToolMuonPid::fill( const Particle*
                                   , const Particle* P
                                   , const std::string& head
                                   , Tuples::Tuple& tuple )
{
  const std::string prefix=fullName(head);

  if( P ){
    bool test = true;
    test &= tuple->column( prefix+"_ID", P->particleID().pid() );
    if( !P->isBasicParticle() ) return StatusCode(test); // no rich info for composite!
    if( isPureNeutralCalo(P)) return StatusCode(test); // no rich information for calo neutrals
    const ProtoParticle* proto = P->proto();
    if( proto ){
      // Combined DLLs
      test &= tuple->column( prefix+"_CombDLLMu"
                             ,proto->info(LHCb::ProtoParticle::CombDLLmu,-1000));
      test &= tuple->column( prefix+"_ProbNNmu"
                             ,proto->info(LHCb::ProtoParticle::ProbNNmu,-1000));
      test &= tuple->column( prefix+"_ProbNNghost"
                             ,proto->info(LHCb::ProtoParticle::ProbNNghost,-1000));
      test &= tuple->column( prefix+"_InMuonAcc"
                             ,proto->info(LHCb::ProtoParticle::InAccMuon, -1000));
      bool hasMuon = false;
      bool isMuon = false;
      bool isMuonLoose = false;
      //bool isMuonTight = false;
      double muonLLmu = -99999;
      double muonLLbg = -99999;
      const MuonPID * muonPID = proto->muonPID();
      int NShared  = -1;
      if(muonPID) {
        hasMuon = true;
        isMuon = muonPID->IsMuon();
        isMuonLoose = muonPID->IsMuonLoose();
        //isMuonTight = muonPID->IsMuonTight();
        NShared = muonPID->nShared();
        muonLLmu= muonPID->MuonLLMu();
        muonLLbg= muonPID->MuonLLBg();
      }
      test &= tuple->column( prefix+"_hasMuon", hasMuon);
      test &= tuple->column( prefix+"_isMuon", isMuon);
      test &= tuple->column( prefix+"_isMuonLoose", isMuonLoose);
      //test &= tuple->column( prefix+"_isMuonTight", isMuonTight);
      test &= tuple->column( prefix+"_NShared", NShared);
      test &= tuple->column( prefix+"_MuonLLmu", muonLLmu);
      test &= tuple->column( prefix+"_MuonLLbg", muonLLbg);

      return StatusCode(test);
    }
  }
  return StatusCode::FAILURE;
}

// Declaration of the Tool Factory
// actually acts as a using namespace TupleTool
DECLARE_TOOL_FACTORY( TupleToolMuonPid )
