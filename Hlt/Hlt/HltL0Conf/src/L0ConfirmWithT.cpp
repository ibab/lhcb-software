// $Id: L0ConfirmWithT.cpp,v 1.1 2007-07-04 12:36:45 albrecht Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "Event/State.h"
#include "Event/Track.h"

#include "HltBase/ITrackConfirmTool.h"

// local
#include "L0ConfirmWithT.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0ConfirmWithT
//
// 2007-07-04 : Johannes Albrecht
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( L0ConfirmWithT );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0ConfirmWithT::L0ConfirmWithT( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<ITracksFromTrack>(this);

}
//=============================================================================
// Destructor
//=============================================================================
L0ConfirmWithT::~L0ConfirmWithT() {} 

//=============================================================================
StatusCode L0ConfirmWithT::initialize() 
{
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
    return sc;
  }
  debug() << " Initialize L0ConfirmWithT" << endmsg;

  m_TsaConfirmTool=tool<ITrackConfirmTool>( "TsaConfirmTool" );

  return sc;
}



StatusCode L0ConfirmWithT::tracksFromTrack( const LHCb::Track& seed,
                                            std::vector<LHCb::Track*>& tracks )
{
  if(msgLevel(MSG::DEBUG)) debug()<<"enter tracksFromTrack method"<<endmsg;
  
  LHCb::State* seedStatePos = seed.firstState().clone();
  LHCb::State* seedStateNeg = seed.firstState().clone();
  StatusCode sc = extrapolateToT3( *seedStatePos, *seedStateNeg );
  
  //do the tracks
  m_TsaConfirmTool->tracks( *seedStatePos , tracks );

  if( 0 != seedStateNeg->x() ){
    //is only set for calo seed, set to 0 for muon
    //preliminary solution JA 2007-07-04
    m_TsaConfirmTool->tracks( *seedStateNeg , tracks );
  }

  delete seedStateNeg;
  delete seedStatePos;
  return sc;
}

StatusCode L0ConfirmWithT::extrapolateToT3( LHCb::State& statePos, LHCb::State& stateNeg )
{

  double zT3 = 9315.;//middle T3
  double zState = statePos.z();
  double deltaZ = zState - zT3;
  double xT3 = 0;
  double yT3 = 0;
  
  /*
   *  this is a preliminary solution, if track types for muon and calo 
   *  tracks are availble the algorithm will choose depending on this type! 
   *  JA 2007-07-04
   */
  if( zState > 13700 ){
    //muon seed, has sensible tx,ty

    xT3 = statePos.x() - statePos.tx() * deltaZ;
    yT3 = statePos.y() - statePos.ty() * deltaZ;
    
    stateNeg.setState(0,0,0,0,0,0);
  }
  else if(zState == 12700){
    //ecal seeding, extrapolate according to parametrized cluster energy
   
    double alpha = 1536443.236032;
    double eCluster = 1./double(statePos.qOverP());
    
    xT3 = statePos.x() * zT3 / zState - alpha/eCluster;
    yT3 = statePos.y() * zT3 / zState;
    double txPos = (statePos.x()-xT3)/deltaZ;
    double ty = (statePos.y()-yT3)/deltaZ;

    double xT3Neg = statePos.x() * zT3 / zState + alpha/eCluster;
    double txNeg = (statePos.x()-xT3Neg)/deltaZ;

    stateNeg.setX( xT3Neg );
    stateNeg.setY( yT3 );
    stateNeg.setZ( zT3 );
    stateNeg.setTx( txNeg );
    stateNeg.setTy( ty );
    statePos.setTx( txPos );
    statePos.setTy( ty );
  }
  
  statePos.setX( xT3 );
  statePos.setY( yT3 );
  statePos.setZ( zT3 );

  return StatusCode::SUCCESS;
}
