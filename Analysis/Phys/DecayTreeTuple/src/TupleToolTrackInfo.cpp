// $Id: TupleToolTrackInfo.cpp,v 1.12 2010-04-23 12:01:26 jhe Exp $
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
DECLARE_TOOL_FACTORY( TupleToolTrackInfo )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  TupleToolTrackInfo::TupleToolTrackInfo( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
    : TupleToolBase ( type, name , parent )
{
  declareInterface<IParticleTupleTool>(this);
}

//=============================================================================

StatusCode TupleToolTrackInfo::fill( const LHCb::Particle*
                                     , const LHCb::Particle* P
                                     , const std::string& head
                                     , Tuples::Tuple& tuple )
{
  const std::string prefix = fullName(head);

  bool test = true;
  if( !P ) return StatusCode::FAILURE;

  //first just return if the particle isn't supposed to have a track
  if( !P->isBasicParticle() ) return StatusCode::SUCCESS;

  const LHCb::ProtoParticle* protop = P->proto();
  if(!protop) return StatusCode::SUCCESS;
  const LHCb::Track* track = protop->track();
  if(!track) return StatusCode::SUCCESS;


  if (msgLevel(MSG::DEBUG)) debug() << prefix << " " << track->type()
                                    << " "+prefix+"_TRACK_CHI2 " << track->chi2() << endmsg ;
  if (msgLevel(MSG::VERBOSE)) verbose() << *track << endmsg ;
  test &= tuple->column( prefix+"_TRACK_Type",  track->type() );
  test &= tuple->column( prefix+"_TRACK_Key",   track->key() );

  if(isVerbose()) test &= tuple->column( prefix+"_TRACK_CHI2",  track->chi2() );
  int nDoF = track->nDoF();
  if(isVerbose()) test &= tuple->column( prefix+"_TRACK_NDOF",  nDoF );
  if (nDoF)
  {
    test &= tuple->column( prefix+"_TRACK_CHI2NDOF", track->chi2()/nDoF );
    test &= tuple->column( prefix+"_TRACK_PCHI2", track->probChi2() );
    if(isVerbose())
    {

      if ( track->info(LHCb::Track::FitVeloNDoF,0) >0)
      {
        test &= tuple->column( prefix+"_TRACK_VeloCHI2NDOF",
                               track->info(LHCb::Track::FitVeloChi2, -1.)/
                               track->info(LHCb::Track::FitVeloNDoF, 0) );
      }
      else test &= tuple->column( prefix+"_TRACK_VeloCHI2NDOF",-1.);
      if ( track->info(LHCb::Track::FitTNDoF,0) >0)
      {
        test &= tuple->column( prefix+"_TRACK_TCHI2NDOF",
                               track->info(LHCb::Track::FitTChi2, -1.)/
                               track->info(LHCb::Track::FitTNDoF, 0) );
      }
      else test &= tuple->column( prefix+"_TRACK_TCHI2NDOF",-1.);
    }

  }
  else
  {
    if (msgLevel(MSG::VERBOSE)) verbose() << "No NDOF" << endmsg ;
    test &= tuple->column( prefix+"_TRACK_PCHI2",-1.);
    test &= tuple->column( prefix+"_TRACK_CHI2NDOF", -1 );
    if(isVerbose()) test &= tuple->column( prefix+"_TRACK_VeloCHI2NDOF",-1.);
    if(isVerbose()) test &= tuple->column( prefix+"_TRACK_TCHI2NDOF",-1.);
  }
  if(isVerbose())
  {
    //hopefully unique double constructed from multiplying all Velo hit IDs
    double veloUTID=1.;
    //std::vector< unsigned int > veloIDs;
    const std::vector< LHCb::LHCbID > & lhcbIDs = track->lhcbIDs();
    std::vector< LHCb::LHCbID >::const_iterator itID = lhcbIDs.begin();
    for ( ; itID != lhcbIDs.end(); itID++ )
    {
      if ( (*itID).isVelo() )
      {
        //veloIDs.push_back( (*itID).veloID().channelID() );
        veloUTID*=(double((*itID).veloID().channelID())/1000000.);
      }
    }
    //veloIDs.push_back( 0 );
    test &= tuple->column( prefix+"_VELO_UTID", veloUTID );
  }

  //}
  test &= tuple->column( prefix+"_TRACK_MatchCHI2", track->info(LHCb::Track::FitMatchChi2,-1) );
  double ghostProbability = -1.0;
  if (track->ghostProbability() != 0)
    ghostProbability = track->ghostProbability();

  test &= tuple->column( prefix+"_TRACK_GhostProb",ghostProbability);
  test &= tuple->column( prefix+"_TRACK_CloneDist", track->info(LHCb::Track::CloneDist, -1.) );
  test &= tuple->column( prefix+"_TRACK_Likelihood", track->likelihood() );


  return StatusCode(test);
}
