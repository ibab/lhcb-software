// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Muon/src/L0mTriggerProc.cpp,v 1.2 2001-05-03 09:12:25 atsareg Exp $

/// Include files
/// Gaudi interfaces
#include "GaudiKernel/IDataProviderSvc.h"

/// Gaudi factories for algorithms 
#include "GaudiKernel/AlgFactory.h"

/// Gaudi base classes
#include "GaudiKernel/SmartDataPtr.h"

/// Utility classes
#include "GaudiKernel/MsgStream.h"

#include "CLHEP/Geometry/Vector3D.h"

/// Private classes to the example
#include "L0Muon/L0MuonCandidate.h"
#include "L0mTriggerProc.h"
#include "L0mTower.h"

//------------------------------------------------------------------------------
//
/// Implementation of class :  L0mTriggerProc
//
/// Author: A.Tsaregorodtsev
//
//------------------------------------------------------------------------------


static const AlgFactory<L0mTriggerProc>  Factory;
const IAlgFactory& L0mTriggerProcFactory = Factory;

/// Algorithm parameters which can be set at run time must be declared.
/// This should be done in the constructor.
L0mTriggerProc::L0mTriggerProc(const std::string& name, ISvcLocator* pSvcLocator)
: Algorithm(name, pSvcLocator)
{
   declareProperty("FoiXSize", m_foiXSize);
   declareProperty("FoiYSize", m_foiYSize);
   declareProperty("PtParameters", m_ptParameters);
   m_outputCandidates = "/Event/MC/L0MuonCandidates";
   declareProperty("outputCandidates", m_outputCandidates);
}


StatusCode L0mTriggerProc::initialize()   { 

    MsgStream log(msgSvc(), name());

    log << MSG::INFO << "Initialization starting..." << endreq;
    
    m_layout.push_back(MuonLayout(24,8));
    m_layout.push_back(MuonLayout(48,8));
    m_layout.push_back(MuonLayout(48,8));
    m_layout.push_back(MuonLayout(12,8));
    m_layout.push_back(MuonLayout(12,8));
    
    return StatusCode::SUCCESS;
}

StatusCode L0mTriggerProc::execute() {

  MsgStream log(msgSvc(), name());
  
  log << MSG::DEBUG << "execute" << endreq;
  
  //===============================
  // get Muon digitisations
  //===============================  
  
  log << MSG::DEBUG << "Retrieving L0mPads...  " << endreq;  
  
  SmartDataPtr< ObjectVector<L0mPad> > pads(eventSvc(),"/Event/MC/L0mPads");
  if(!pads) {
    return StatusCode::FAILURE;
  }
  ObjectVector<L0mPad>::const_iterator ip;
  
  //======================================
  // create the collection of L0mTowers
  //======================================
  
  log << MSG::DEBUG << "Creating towers...  " << endreq;  
  
  L0mTower* lt; 
  
  for(ip=pads->begin(); ip != pads->end(); ip++ ) {
    if((*ip)->station() == 2) {
      lt = createTower(*ip, pads);
      m_towers.push_back(lt);
    }       
  }
  
  //======================================
  // register trigger candidates  
  //======================================

  StatusCode sc;

  ObjectVector<L0MuonCandidate>* cand = new ObjectVector<L0MuonCandidate>;
  if( cand == 0 ) {
      log << MSG::ERROR << "Unable to create L0MuonCandidates" << endreq;
      return StatusCode::FAILURE ;
  }
  log << MSG::DEBUG << "Registering candidates ...  "  ;  
  sc = eventSvc()->registerObject(m_outputCandidates,cand);
  if(sc.isFailure() ) {
      log << MSG::ERROR << "Unable register L0MuonCandidates in " 
                        << m_outputCandidates << endreq;
      delete cand;
      return StatusCode::FAILURE ;
  }
  log << MSG::DEBUG << "done, status " << sc << endreq;  
  
  //===============================
  // Find trigger candidates
  //===============================
  
  L0MuonCandidate* lcd;
  std::vector<L0mTower*>::iterator it;
 
  for ( it=m_towers.begin(); it != m_towers.end(); it++ ) {
    if ((*it)->isFull()) {
      lcd = (*it)->createCandidate(m_ptParameters,
	                           m_foiXSize,
				   m_foiYSize);
      if(lcd) {

	//  Track found ! 

	log << MSG::DEBUG << "Track found" << endreq;
	(*it)->draw(log << MSG::DEBUG);
	(*it)->padM3()->print(log );
	(*it)->padM2()->print(log );
	(*it)->padM1()->print(log );
	log << MSG::DEBUG << " Pt= "    << lcd->pt() 
	                  << " Theta= " << lcd->theta() 
			  << " Phi= "   << lcd->phi() << endreq; 	      
	cand->push_back(lcd); 	
      }
    }
    // This tower is no more needed. All the towers should be deleted
    delete *it;	  
  }
  
  // Make the final clean-up
  m_towers.clear();
  
  return StatusCode::SUCCESS;
}

StatusCode L0mTriggerProc::finalize()  {
  return StatusCode::SUCCESS;
}

L0mTower* L0mTriggerProc::createTower(L0mPad* pad, ObjectVector<L0mPad>* pads) {

  MsgStream log(msgSvc(), name());
        
  int st = pad->station();
  if ( st != 2 ) {
      log << MSG::DEBUG << "!!! Wrong station in createTower " 
	                << st << endreq;
      return 0;
  } 
  
  int nx = pad->nX();
  int ny = pad->nY();
  L0mTower* lt = new L0mTower(pad); 
  
  std::vector<std::vector<MuonTile> > vtiles(5);
  std::vector<MuonTile> vmt3;
  std::vector<MuonTile>::iterator ivmt;
  std::vector<MuonTile>::iterator ivmt3;
  ObjectVector<L0mPad>::const_iterator ind;
    
  // Find all the tiles touched by the tower
  vtiles[0] = m_layout[0].tiles( *pad , 16 );
  vtiles[1] = m_layout[1].tiles( *pad , 8 );
  vtiles[3] = m_layout[3].tiles( *pad , 8 );
  vtiles[4] = m_layout[4].tiles( *pad , 10 );
    
  // look through all the pads
  int nbit = 0;
  for ( ind = pads->begin(); ind != pads->end(); ind++ ) {
    int st = (*ind)->station();
    if(st !=2 ) {   // for all the pads except those in M3
      // Check if the pad coinsides with some tile
      for (ivmt = vtiles[st].begin(); ivmt != vtiles[st].end(); ivmt++ ) {
	if (**ind == *ivmt  ) {
	  // Check how many M3 pads covers the touched pad in other station
	  vmt3 = m_layout[2].tiles( *ivmt );
	  
	  // cout << " After vmt3 " << endl;
	  
	  for (ivmt3 = vmt3.begin(); ivmt3 != vmt3.end(); ivmt3++ ) {
	    if( (*ivmt3).nY() == ny) {
	      int inx = (*ivmt3).nX();
    	      lt->addBit(inx-nx, 0, st, *ind);	      
	    }
          }
	}
      }
    }
  }
  return lt;
}    

