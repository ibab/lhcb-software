// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Muon/src/component/L0mTrigger.cpp,v 1.3 2002-05-07 07:33:43 atsareg Exp $

/// Include files
/// Gaudi interfaces
#include "GaudiKernel/IDataProviderSvc.h"

/// Gaudi factories for algorithms 
#include "GaudiKernel/AlgFactory.h"

/// Gaudi base classes
#include "GaudiKernel/SmartDataPtr.h"

/// Utility classes
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"   
#include "MuonTools/IMuonTileXYZTool.h"   

/// Private classes to the example
#include "Event/MuonDigit.h"
#include "Event/L0MuonCandidate.h"
#include "L0Muon/L0mTrigger.h"
#include "L0Muon/L0mTower.h"

//------------------------------------------------------------------------------
//
/// Implementation of class :  L0mTrigger
//
/// Author: A.Tsaregorodtsev
//
//------------------------------------------------------------------------------


static const AlgFactory<L0mTrigger>  Factory;
const IAlgFactory& L0mTriggerFactory = Factory;

/// Algorithm parameters which can be set at run time must be declared.
/// This should be done in the constructor.
L0mTrigger::L0mTrigger(const std::string& name, ISvcLocator* pSvcLocator)
: Algorithm(name, pSvcLocator) {   
   
   declareProperty("FoiXSize", m_foiXSize);
   declareProperty("FoiYSize", m_foiYSize);
   declareProperty("PtParameters", m_ptParameters);
   declareProperty("M1Extrapolation", m_extraM1);
   m_outputCandidates = "/Event/MC/L0MuonCandidates";
   declareProperty("outputCandidates", m_outputCandidates);
   m_inputPads = "/Event/MC/MuonDigits";
   declareProperty("outputCandidates", m_inputPads);
   m_mode = "Standard";
   declareProperty("Mode", m_mode);
   declareProperty("LimitedY", m_limitedY = true );
   declareProperty("PtPrecision", m_precision=40.);
   declareProperty("PtMaxBins", m_bins=128);
}

StatusCode L0mTrigger::initialize()   { 

    MsgStream log(msgSvc(), name());

    log << MSG::INFO << "Initialized in " << m_mode << " mode" << endreq;
       
    m_layout=MuonSystemLayout(MuonStationLayout(MuonLayout(24,8)),
                              MuonStationLayout(MuonLayout(48,8)),
			      MuonStationLayout(MuonLayout(48,8)),
			      MuonStationLayout(MuonLayout(12,8)),
			      MuonStationLayout(MuonLayout(12,8)));
    m_lulayout=MuonSystemLayout(MuonStationLayout(MuonLayout(0,0)),
                        	MuonStationLayout(MuonLayout(8,1),
                                        	  MuonLayout(4,2),
                                        	  MuonLayout(2,2),
                                        	  MuonLayout(2,2)),
				MuonStationLayout(MuonLayout(8,1),
                                        	  MuonLayout(4,2),
                                        	  MuonLayout(2,2),
                                        	  MuonLayout(2,2)),
				MuonStationLayout(MuonLayout(0,0),
                                        	  MuonLayout(4,2),
                                        	  MuonLayout(2,2),
                                        	  MuonLayout(2,2)),
				MuonStationLayout(MuonLayout(0,0),
                                        	  MuonLayout(4,2),
                                        	  MuonLayout(2,2),
                                        	  MuonLayout(2,2)));			      
			     
    for( int iq=0; iq<4; iq++) {
      m_crates.push_back(new L0mCrate(iq,*this));
    }  
    /// get tile tool
    StatusCode sc;  
    sc = toolSvc()->retrieveTool("MuonTileIDXYZ", m_iTileXYZTool, this );
    if( sc.isFailure() ) {
      log << MSG::FATAL << " Unable to create MuonTileIDToXYZ tool" << endreq;
      return sc;
    }   
    
    //=================================
    // Debug printout
    //=================================
    std::vector<double>::const_iterator iv;    
    log << MSG::DEBUG << "Pt parameters: " ;
    for (iv = m_ptParameters.begin(); iv != m_ptParameters.end(); iv++) {
      log << *iv << " ";
    }
    log << endreq;
    
    std::vector<int>::const_iterator ii;  
    log << MSG::DEBUG << "Field of interest X: " ;
    for (ii = m_foiXSize.begin(); ii != m_foiXSize.end(); ii++) {
      log << *ii << " ";
    }
    log << endreq;
    log << MSG::DEBUG << "Field of interest Y: " ;
    for (ii = m_foiYSize.begin(); ii != m_foiYSize.end(); ii++) {
      log << *ii << " ";
    }
    log << endreq;
    log << MSG::DEBUG << "Extrapolation to M1: " ;
    for (ii = m_extraM1.begin(); ii != m_extraM1.end(); ii++) {
      log << *ii << " ";
    }
    log << endreq;
    log << MSG::DEBUG << "Pt precision: " << m_precision << endreq;
    log << MSG::DEBUG << "bins for Pt encoding: " << m_bins << endreq;
    return StatusCode::SUCCESS;
}

StatusCode L0mTrigger::execute() {

  MsgStream log(msgSvc(), name());
  
  log << MSG::DEBUG << "execute" << endreq;
    
  //===============================
  // get Muon digitisations
  //===============================
  log << MSG::DEBUG << "Retrieving MuonDigits...  ";  
  SmartDataPtr<MuonDigits> digits(eventSvc(),MuonDigitLocation::MuonDigit);
  if(!digits){
    log << MSG::ERROR 
        << "Could not retrieve " << MuonDigitLocation::MuonDigit
        << " from TDS" << endreq;
    return StatusCode::FAILURE;
  }    
  log << MSG::DEBUG << "Done, total digits " << digits->size() << endreq;
  
  //==========================================
  // construct muon pads for trigger needs
  //==========================================
  
  log << MSG::DEBUG << "Crossing the strips ...  ";
  
  m_pads.clear();
  MuonDigits::const_iterator id;
  std::vector<MuonTileID> hstrips;
  std::vector<MuonTileID> vstrips;
  
  for( id = digits->begin() ; id != digits->end() ; id++ ){
    MuonTileID mkey = (*id)->key();
    int sta = mkey.station();
    int reg = mkey.region();
    if( m_lulayout.regionLayout(sta,reg).isDefined() ) {
    
      // This is a strip, sort it in vertical and horizontal ones
      if(mkey.layout().xGrid() > mkey.layout().yGrid()) {
        vstrips.push_back(mkey);
      } else {
        hstrips.push_back(mkey);
      }
      // This is a pad, let it go as it is
    } else {
      m_pads.push_back(L0mPad(mkey));
      //cout << mkey << endl;
    }
  }
  //==============================
  //  Cross the strips now
  //==============================
  
  //cout << "crossing strips ... " << endl;
  
  std::vector<MuonTileID>::const_iterator ih;
  std::vector<MuonTileID>::const_iterator iv; 
  for ( ih = hstrips.begin(); ih != hstrips.end(); ih ++ ) {
    for ( iv = vstrips.begin(); iv != vstrips.end(); iv ++ ) {
      MuonTileID cross = ih->intercept(*iv);
      if ( cross.isValid() ) {
        m_pads.push_back(L0mPad(cross));
	//cout << cross << endl;
      }
    }
  }
  
  
  log << "Done, # of L0mPads " << m_pads.size() << endreq;  
  
  std::vector<L0mPad>::iterator ip;
  
  //=========================================
  // create the collection of L0mTowers
  //=========================================
  StatusCode sc;
  
  log << MSG::DEBUG << "Creating towers...  "; 
  
  L0mTower* lt; 
  
  for(ip=m_pads.begin(); ip != m_pads.end(); ip++ ) {
  
//     cout << " L0mTrigger: pads: " << (*ip)->station() << "/"
//                                        << (*ip)->quarter() << "/"  
// 				       << (*ip)->region() << "/" 
// 				       << (*ip)->nX() << "/" 
// 				       << (*ip)->nY() << "/" << endl;
  
    if((*ip).padId().station() == 2) {
        
      lt = createTower(ip, m_pads);
      lt->setMuonTool(m_iTileXYZTool);
      m_towers.push_back(lt);
      if (lt->isFull()) lt->draw(log);
    }     
  }
  log << "Done, # of towers "<< m_towers.size() << endreq;
  //=========================================
  // register trigger candidates  
  //=========================================
  
  L0MuonCandidates* cand = new L0MuonCandidates;
  if( cand == 0 ) {
    log << MSG::ERROR << "Unable to create L0MuonCandidates" << endreq;
    return StatusCode::FAILURE ;
  }
  log << MSG::DEBUG << "Registering candidates ...  "  ;  
  sc = eventSvc()->registerObject(L0MuonCandidateLocation::Default,cand);
  if(sc.isFailure() ) {
    log << MSG::ERROR << "Unable to register L0MuonCandidates in " 
                      << L0MuonCandidateLocation::Default << endreq;
    delete cand;
    return StatusCode::FAILURE ;
  }
  log << MSG::DEBUG << "done, status " << sc << endreq;  
  
  //===================
  // Build crates 
  //=================== 
  


  log << MSG::DEBUG << "Building crates "  << endreq;  

  std::vector<L0mCrate*>::iterator ic;

  for(ic=m_crates.begin(); ic!=m_crates.end(); ic++) {
    (*ic)->buildUnits(m_towers);
  }
  
  //=====================
  // Execute crates 
  //=====================   
  
  log << MSG::DEBUG << "Executing crates "  << endreq;  

  std::vector<L0MuonCandidate*> lmcrate;
  std::vector<L0MuonCandidate*>::const_iterator ilmc;
  
  for(ic=m_crates.begin(); ic!=m_crates.end(); ic++) {
    (*ic)->execute(log);
    lmcrate=(*ic)->candidates();
    
    if (lmcrate.size()>0) {
      for(ilmc=lmcrate.begin(); ilmc!=lmcrate.end(); ilmc++) {
        cand->insert(*ilmc);
      }  	
    }    
    // Make cleanup after the work is done
    (*ic)->clear();
  }
  
  //======================================
  // Execution is over. Clean up towers
  //======================================
    	  
  std::vector<L0mTower*>::iterator it;
  for (it = m_towers.begin(); it != m_towers.end(); it++ ) {
    delete *it;
  }
  m_towers.clear();
  m_pads.clear();
  
  L0MuonCandidates::iterator im;
  for(im = cand->begin(); im != cand->end(); im++ ) {
    if ( (*im)->status() == L0Muon::OK ) {
      log << MSG::DEBUG << "Pt of the candidate = " 
                        << (*im)->pt() << endreq;   
    }			 
  }
  
  return StatusCode::SUCCESS;
}

StatusCode L0mTrigger::finalize()  {

  std::vector<L0mCrate*>::iterator ip;
  for( ip=m_crates.begin(); ip != m_crates.end(); ip++) {
    delete *ip;
  }

  return StatusCode::SUCCESS;
}

L0mTower* L0mTrigger::createTower(L0mPad* pad, const std::vector<L0mPad>& pads){

  MsgStream log(msgSvc(), name());
          
  int st = pad->padId().station();
  int nq = pad->padId().quarter();
  int nr = pad->padId().region();
  if ( st != 2 ) {
      log << MSG::DEBUG << "!!! Wrong station in createTower " 
	                << st << endreq;
      return 0;
  } 
  
  
  int nx = pad->padId().nX();
  int ny = pad->padId().nY();
  //cout << "seed pad: " << st << " " << nr<< " " << nq << " " << nx << " " << ny << endl;
  
  L0mTower* lt = new L0mTower(pad->padId()); 
  
  std::vector<std::vector<MuonTileID> > vtiles(5);
  std::vector<MuonTileID> vmt3;
  std::vector<MuonTileID>::iterator ivmt;
  std::vector<MuonTileID>::iterator ivmt3;
  std::vector<L0mPad>::const_iterator ind;
    
  // Find all the tiles touched by the tower
  MuonTileID ptmp = pad->padId();
  ptmp.setStation(0);
  vtiles[0] = m_layout.tilesInArea(ptmp , 32, 0 );
  ptmp.setStation(1);
  vtiles[1] = m_layout.tilesInArea(ptmp , 8, 0 );
  ptmp.setStation(3);
  vtiles[3] = m_layout.tilesInArea(ptmp , 8, 1);
  ptmp.setStation(4);
  vtiles[4] = m_layout.tilesInArea(ptmp , 12, 1 );
  
  // cout << "vtiles: " << vtiles[0].size() << " " <<
//                         vtiles[1].size() << " " <<
// 		        vtiles[3].size() << " " <<
// 		        vtiles[4].size() << endl;
    
  // look through all the pads
  for ( ind = pads.begin(); ind != pads.end(); ind++ ) {
    int ist = (*ind).padId().station();
    int iq = (*ind).padId().quarter();
    int ir = (*ind).padId().region();
    // for all the pads in the same quarter and the nearest region 
    // except those in M3
    
    //cout << "Look for pads " << ist << " " << ir << " " << iq << endl;
    
    if(ist !=2 && iq==nq && (abs(ir-nr))<2  ) {   
      // Check if the pad coincides with some tile
      for (ivmt = vtiles[ist].begin(); ivmt != vtiles[ist].end(); ivmt++ ) {
        //cout << (*ind).padId() << "->" << *ivmt << endl;
	if ( (*ind).padId() == *ivmt  ) {
	  // Check how many M3 pads covers the touched pad in other station
	  // These the M3 pads defined in terms of nr region 
	  MuonTileID pvmt = *ivmt;
	  pvmt.setStation(2);
	  vmt3 = m_layout.tilesInRegion( pvmt, nr);
	  //cout << "vmt3 size " << vmt3.size() << endl;
	  for (ivmt3 = vmt3.begin(); ivmt3 != vmt3.end(); ivmt3++ ) {
	    int iny = (*ivmt3).nY() - ny;
	    if( abs(iny) < 2) {
	      int inx = (*ivmt3).nX();
    	      lt->addBit(inx-nx, iny, ist, (*ind).padId());	
	      //cout << "Adding bit to tower: " << (*ind).padId() << endl;      
	    }
          }
	}
      }
    }
  }
    
  if(m_limitedY) lt->limitedY();  
  return lt;
}    



