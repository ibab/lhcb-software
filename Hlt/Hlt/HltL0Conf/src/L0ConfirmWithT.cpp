// $Id: L0ConfirmWithT.cpp,v 1.2 2007-07-17 14:09:43 albrecht Exp $
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
  double xT3Pos = 0;double xT3Neg = 0;double yT3 = 0;
  double txPos = 0;double txNeg =0;double ty = 0;
  
  /*
   *  this is a preliminary solution, if track types for muon and calo 
   *  tracks are availble the algorithm will choose depending on this type! 
   *  JA 2007-07-04
   */
  if( zState > 13700 ){
    //muon seed, has sensible tx,ty

    xT3Pos = statePos.x() - statePos.tx() * deltaZ;
    yT3 = statePos.y() - statePos.ty() * deltaZ;
    txPos = statePos.tx();
    ty = statePos.ty();
  }
  else if(zState == 12700){
    //ecal seeding, extrapolate according to parametrized cluster energy
   
    double alpha = 1536443.236032;
    double eCluster = 1./double(statePos.qOverP());
    
    xT3Pos = statePos.x() * zT3 / zState - alpha/eCluster;
    yT3 = statePos.y() * zT3 / zState;
    txPos = (statePos.x()-xT3Pos)/deltaZ;
    ty = (statePos.y()-yT3)/deltaZ;

    xT3Neg = statePos.x() * zT3 / zState + alpha/eCluster;
    txNeg = (statePos.x()-xT3Neg)/deltaZ;
    
  }
  else if( 12830 == zState ){
    //HCal seeding with enough energy in ECal
    double eCluster = 1./double(statePos.qOverP());

    xT3Pos  = statePos.x()*zT3/zState - ( 11.5+1.585e6/eCluster );
    yT3  = statePos.y()*zT3/zState;
    txPos = (statePos.x()-xT3Pos)/deltaZ;
    ty = (statePos.y()-yT3)/deltaZ;
    xT3Neg  = statePos.x()*zT3/zState + ( 11.5+1.585e6/eCluster );
    txNeg = (statePos.x()-xT3Neg)/(deltaZ);
  }
  else if( 13690 == zState ){
    //HCal seeding only
    double eCluster = 1./double(statePos.qOverP());
    
    xT3Pos  = statePos.x()*zT3/zState - ( 17.+1.712e6/eCluster );
    yT3  = statePos.y()*zT3/zState;
    txPos = (statePos.x()-xT3Pos)/deltaZ;
    ty = (statePos.y()-yT3)/deltaZ;
    xT3Neg  = statePos.x()*zT3/zState + ( 17.+1.712e6/eCluster );
    txNeg = (statePos.x()-xT3Neg)/(deltaZ);

  }
  
  statePos.setState( xT3Pos , yT3 , zT3 , txPos , ty , statePos.qOverP()); 
  stateNeg.setState( xT3Neg , yT3 , zT3 , txNeg , ty , stateNeg.qOverP());
  
  return StatusCode::SUCCESS;
}
