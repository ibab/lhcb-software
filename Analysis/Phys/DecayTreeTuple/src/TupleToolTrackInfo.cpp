// $Id: TupleToolTrackInfo.cpp,v 1.6 2009-06-01 15:40:27 pkoppenb Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "TupleToolTrackInfo.h"

#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"

#include "Event/Particle.h"
//-----------------------------------------------------------------------------
// Implementation file for class : TupleToolTrackInfo
//
// 2008-03-17 : Stephane Poss
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
// actually acts as a using namespace TupleTool
DECLARE_TOOL_FACTORY( TupleToolTrackInfo );
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolTrackInfo::TupleToolTrackInfo( const std::string& type,
				    const std::string& name,
				    const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IParticleTupleTool>(this);
}

//=============================================================================

StatusCode TupleToolTrackInfo::fill( const LHCb::Particle* 
				   , const LHCb::Particle* P
				   , const std::string& head
				   , Tuples::Tuple& tuple ){
  bool test = true;
  if( P ){
    if( P->isBasicParticle() ){
      const LHCb::ProtoParticle* protop = P->proto();
      if(protop){
        const LHCb::Track* track = protop->track();
        if(track){
          //          if (msgLevel(MSG::VERBOSE)) verbose() << *track << endmsg ;
          info() << *track << endmsg ;
          test &= tuple->column( head+"_TRACK_Type",  track->type() );
          test &= tuple->column( head+"_TRACK_CHI2",  track->chi2() );
          int nDoF = track->nDoF();
          test &= tuple->column( head+"_TRACK_NDOF",  nDoF );
          if (nDoF){
            test &= tuple->column( head+"_TRACK_PCHI2", track->probChi2() );
            if ( track->info(LHCb::Track::FitVeloNDoF,0) >0) {
              test &= tuple->column( head+"_TRACK_VeloCHI2NDOF",  
                                     track->info(LHCb::Track::FitVeloChi2, -1.)/
                                     track->info(LHCb::Track::FitVeloNDoF, 0) );
            } else test &= tuple->column( head+"_TRACK_VeloCHI2NDOF",-1.);
            if ( track->info(LHCb::Track::FitTNDoF,0) >0) {
              test &= tuple->column( head+"_TRACK_TCHI2NDOF",  
                                     track->info(LHCb::Track::FitTChi2, -1.)/
                                     track->info(LHCb::Track::FitTNDoF, 0) );
            } else test &= tuple->column( head+"_TRACK_TCHI2NDOF",-1.);
          } else {
            test &= tuple->column( head+"_TRACK_PCHI2",-1);
          }
          test &= tuple->column( head+"_TRACK_GhostProb", track->info(LHCb::Track::GhostProbability, -1.) );
          test &= tuple->column( head+"_TRACK_CloneDist", track->info(LHCb::Track::CloneDist, -1.) );
        }
      }
    }
  } else {
    return StatusCode::FAILURE;
  }
  return StatusCode(test);
}
