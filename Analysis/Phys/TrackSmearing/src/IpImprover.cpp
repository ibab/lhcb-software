// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include <boost/assign/list_of.hpp>
// track interfaces
//#include "Event/Track.h"

#include "TrackInterfaces/ITrackManipulator.h"
#include "GaudiKernel/ToStream.h"

//mc linker
#include <Linker/LinkedTo.h>
#include "Event/MCParticle.h"

// local
#include "IpImprover.h"

//-----------------------------------------------------------------------------
// Implementation file for class : IpImprover
//
// 2012-11-26 : Sascha Stahl
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( IpImprover )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
IpImprover::IpImprover( const std::string& name,
                        ISvcLocator* pSvcLocator)
  : DaVinciAlgorithm ( name , pSvcLocator )
{
  declareProperty( "TrackLocations",m_trackLocations = 
                   boost::assign::list_of<std::string> (LHCb::TrackLocation::Default));
  //declareProperty("ImproverTool", m_improverToolName = "IpImprover");

  declareProperty("types", m_types = boost::assign::list_of(LHCb::Track::Long )(LHCb::Track::Velo )
                  (LHCb::Track::Upstream ));

  declareProperty("Improvement",m_improvement = -1.0); // set to bigger 0, 
                                                      // if one wants only one overall correction
  declareProperty("MomImprovement",m_pImprovement = 0.0);
  
  declareProperty( "xpar1_mc",m_xpar1_mc = 11.);
  declareProperty( "xpar2_mc",m_xpar2_mc = 25.);
  declareProperty( "xpar1_upgrade",m_xpar1_upgrade = 7. );
  declareProperty( "xpar2_upgrade",m_xpar2_upgrade = 13. );

  // currently the code uses only one parametrization
  // but in principle that can be changed
  declareProperty( "ypar1_mc",m_ypar1_mc = 11.);
  declareProperty( "ypar2_mc",m_ypar2_mc = 25.);
  declareProperty( "ypar1_upgrade",m_ypar1_upgrade = 7. );
  declareProperty( "ypar2_upgrade",m_ypar2_upgrade = 13. );

  
  m_funcx_upgrade = new TF1("funcx_upgrade","[0]/1000.0+[1]*x",0.0,1.0);                                                                                                                                                                            
  m_funcx_upgrade->SetParameter(0,m_xpar1_upgrade);
  m_funcx_upgrade->SetParameter(1,m_xpar2_upgrade);

  m_funcx_mc = new TF1("funcx_mc","[0]/1000.0+[1]*x",0.0,1.0);
  m_funcx_mc->SetParameter(0,m_xpar1_mc);
  m_funcx_mc->SetParameter(1,m_xpar2_mc);

  m_funcy_upgrade = new TF1("funcy_upgrade","[0]/1000.0+[1]*x",0.0,1.0);
  m_funcy_upgrade->SetParameter(0,m_ypar1_upgrade);
  m_funcy_upgrade->SetParameter(1,m_ypar2_upgrade);

  m_funcy_mc = new TF1("funcy_mc","[0]/1000.0+[1]*x",0.0,1.0);
  m_funcy_mc->SetParameter(0,m_ypar1_mc);
  m_funcy_mc->SetParameter(1,m_ypar2_mc);

}
//=============================================================================
// Destructor
//=============================================================================
IpImprover::~IpImprover() {
  delete m_funcx_upgrade;
  delete m_funcx_mc;

  delete m_funcy_upgrade;
  delete m_funcy_mc;
  

} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode IpImprover::initialize() {
  StatusCode sc = DaVinciAlgorithm::initialize(); 
  if ( sc.isFailure() ) return sc;

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  //std::vector<unsigned int>::const_iterator iterT = m_types.begin();
  //for (; iterT != m_types.end(); ++iterT){
  //  LHCb::Track::Types type = LHCb::Track::Types(*iterT);
  //  std::string aName = Gaudi::Utils::toString(type);
    //ITrackManipulator* aTool =  tool<ITrackManipulator>(m_improverToolName,
    //                                                    aName+"_improveTool", this );
    
  //  m_toolMap[type] = aTool; 
  //} // iterT
 
  
  return StatusCode::SUCCESS;
}

double IpImprover::getImprovement(double pt, bool isX = true)
{

  double inversept = pt > 0 ? 1.0/pt : 0.0;
  double improvement = 0;
  
  if (isX){
    improvement = (m_funcx_mc->Eval(inversept) - m_funcx_upgrade->Eval(inversept) )/ m_funcx_mc->Eval(inversept);
  }else{
    improvement = (m_funcy_mc->Eval(inversept) - m_funcy_upgrade->Eval(inversept) )/ m_funcy_mc->Eval(inversept);
     
  }
  
  
  return improvement;

}



StatusCode IpImprover::improveTrack(LHCb::Track& aTrack,const LHCb::MCParticle* mcp )
{

  LHCb::State * state = aTrack.stateAt( LHCb::State::ClosestToBeam );
  if (state==NULL){
    return Warning("no state found",StatusCode::SUCCESS);
  }


  
  

  double z = state->z();
  const Gaudi::XYZPoint origin = mcp->originVertex()->position();
  const Gaudi::LorentzVector momentum = mcp->momentum();

  double improvement = getImprovement(mcp->momentum().pt());
  if (m_improvement > 0.){
    improvement = m_improvement;
  }
  
  
  double tx = mcp->momentum().x()/ mcp->momentum().z();
  double ty = mcp->momentum().y()/ mcp->momentum().z();
  
  double alpha = atan(tx);
  double gamma = atan(ty);
  

  double trueX = origin.x()+(z-origin.z())*tx;
  double trueY = origin.y()+(z-origin.z())*ty;
  
  double recX = state->x();
  double recY = state->y();
  
  double diffX = recX-trueX;
  double diffY = recY-trueY;
 
  double correctionX = -1*improvement * diffX;
  double correctionY = -1*improvement * diffY;

  

  double newX = recX + correctionX;
  double newY = recY + correctionY;
 
  
  state->setX(newX);
  state->setY(newY);

  // do we want to improve the slope as well?
  double recTx = state->tx();
  double recTy = state->ty();
  double newTx = improvement*tx+(1-improvement)*recTx;
  double newTy = improvement*ty+(1-improvement)*recTy;

  state->setTx(newTx);
  state->setTy(newTy);
  
  
  
  if ( m_pImprovement>0){
    double trueMomentum = mcp->p();
    double recMomentum = state->p();
    
    double newMomentum = m_pImprovement*trueMomentum+(1-trueMomentum)*recMomentum;
    double q = state->qOverP()>0? 1. : -1.;
    state->setQOverP(q/newMomentum);
  }
  
  return StatusCode::SUCCESS;
  
}


//=============================================================================
// Main execution
//=============================================================================
StatusCode IpImprover::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // code goes here  
  std::vector<std::string>::const_iterator iterI = m_trackLocations.begin();
  for (; iterI != m_trackLocations.end(); ++iterI){
    
    LHCb::Tracks* tracks = getIfExists<LHCb::Tracks*>(*iterI) ;
    LinkedTo<LHCb::MCParticle, LHCb::Track> directLink( evtSvc(), msgSvc(), *iterI );

    if (NULL==tracks) {
      continue;
    }
    // loop over Track Container
    for (LHCb::Tracks::iterator iterT = tracks->begin(); iterT != tracks->end(); ++iterT) {
      unsigned int type = (*iterT)->type();
      if (std::find(m_types.begin(), m_types.end(),type) != m_types.end()){ 

        const LHCb::MCParticle* mcp = directLink.first(*iterT);
        if(mcp) improveTrack(**iterT,mcp);
        
        //m_toolMap[type]->execute(**iterT).ignore();
      }
    } // iterT
  }//iterI
  


  setFilterPassed(true);  // Mandatory. Set to true if event is accepted. 
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode IpImprover::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return DaVinciAlgorithm::finalize();
}

//=============================================================================
