// $Header: 

/// Include files

/// Gaudi factories for algorithms and converters
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IDataProviderSvc.h"   

/// Gaudi base classes
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartDataPtr.h"

/// Utility classes
#include "GaudiKernel/MsgStream.h"

/// Private classes to the example
#include "L0mTowerBuilder.h"
#include "MuonLayout.h"

//------------------------------------------------------------------------------
//
/// Implementation of class :  L0mTowerBuilder
//
/// Author: A.Tsaregorodtsev
//
//------------------------------------------------------------------------------


static const AlgFactory<L0mTowerBuilder>  Factory;
const IAlgFactory& L0mTowerBuilderFactory = Factory;

/// Algorithm parameters which can be set at run time must be declared.
/// This should be done in the constructor.
L0mTowerBuilder::L0mTowerBuilder(const std::string& name, ISvcLocator* pSvcLocator)
: Algorithm(name, pSvcLocator)
{
//  declareProperty("HistogramFlag", m_produceHistogram);
//  declareProperty("Option", m_option);
}


StatusCode L0mTowerBuilder::initialize()   { 

    MsgStream log(msgSvc(), name());

    log << MSG::INFO << "Initialization starting..." << endreq;
    log << MSG::INFO << "Retrieving Muon Logical Layout" << endreq;
 
    m_layout1 = MuonLayout(24,8);
    m_layout2 = MuonLayout(48,8);
    m_layout3 = MuonLayout(48,8);
    m_layout4 = MuonLayout(12,8);
    m_layout5 = MuonLayout(12,8);
 
    return StatusCode::SUCCESS;
}

StatusCode L0mTowerBuilder::execute() {

  MsgStream log(msgSvc(), name());
  
  log << MSG::DEBUG << "execute" << endreq;
  
// create the collection of L0mPads

  StatusCode sc;
  
    
  ObjectVector<L0mTower>* towers = new ObjectVector<L0mTower>;
  log << MSG::DEBUG << "Registering pc ...  "  ;  
  sc = eventSvc()->registerObject("/Event/MC/L0mTowers",towers);
  log << MSG::DEBUG << "done, status " << sc << endreq;  
    
// get Muon digitisation
    
  log << MSG::DEBUG << "Retrieving L0mPads...  " ;  
  
  SmartDataPtr< ObjectVector<L0mPad> > pads(eventSvc(),"/Event/MC/L0mPads");
  ObjectVector<L0mPad>::const_iterator ip;
  
//**************************    
// create uTowers  
//**************************

  L0mTower* lt; 
  
  int i = 0;
  for(ip=pads->begin(); ip != pads->end(); ip++ ) {
      if((*ip)->station() == 3) {
          i++;
     //     cout << "Creating Tower number " << i << endl;
          lt = createTower(*ip, pads);
	  towers->push_back(lt);
      }       
  }    
  return StatusCode::SUCCESS;
}

StatusCode L0mTowerBuilder::finalize()  {
  return StatusCode::SUCCESS;
}

L0mTower* L0mTowerBuilder::createTower(L0mPad* pad, ObjectVector<L0mPad>* pads) {

    L0mTower* lt = new L0mTower(pad); 

    int nx = pad->nX();
    int ny = pad->nY();
    int st = pad->station();
    if ( st != 3 ) {
        cout << "!!! CreateTower: Wrong station " << st << endl;
	return 0;
    } 
    
    std::vector<MuonTile> vmt;
    std::vector<MuonTile> vmt3;
    std::vector<MuonTile>::iterator ivmt;
    std::vector<MuonTile>::iterator ivmt3;
    
    ObjectVector<L0mPad>::const_iterator ind;
    
    
   //  Looking for tiles in station 5
    
    vmt = m_layout5.tiles( *pad , m_layout3, 10 );
	
   //	cout << "Number of tiles " << vmt.size() << endl;
	
    for ( ind = pads->begin(); ind != pads->end(); ind++ ) {
        if ( (*ind)->station() == 5 ) {
            for (ivmt = vmt.begin(); ivmt != vmt.end(); ivmt++ ) {
    		if (**ind == *ivmt  ) {
	            vmt3 = m_layout3.tiles( *ivmt , m_layout5);
		    for (ivmt3 = vmt3.begin(); ivmt3 != vmt3.end(); ivmt3++ ) {
			if( (*ivmt3).nY() == ny) {
		            int inx = (*ivmt3).nX();
    		            lt->addBit(inx-nx, 5, *ind);
	        	}
		    }    
    		}
	    }	
    	}
    }
        
   //  Looking for tiles in station 4
    
    vmt = m_layout4.tiles( *pad , m_layout3, 8 );
    for ( ind = pads->begin(); ind != pads->end(); ind++ ) {
        if ( (*ind)->station() == 4 ) {
            for (ivmt = vmt.begin(); ivmt != vmt.end(); ivmt++ ) {
    		if (**ind == *ivmt  ) {
	            vmt3 = m_layout3.tiles( *ivmt , m_layout4);
		    for (ivmt3 = vmt3.begin(); ivmt3 != vmt3.end(); ivmt3++ ) {
			if( (*ivmt3).nY() == ny) {
		            int inx = (*ivmt3).nX();
    		            lt->addBit(inx-nx, 4, *ind);
	        	}
		    }    
    		}
	    }	
    	}
    }
         
    //  Looking for tiles in station 2

    vmt = m_layout2.tiles( *pad , m_layout3, 8 );
    for ( ind = pads->begin(); ind != pads->end(); ind++ ) {
        if ( (*ind)->station() == 2 ) {
            for (ivmt = vmt.begin(); ivmt != vmt.end(); ivmt++ ) {
    		if (**ind == *ivmt  ) {
	            vmt3 = m_layout3.tiles( *ivmt , m_layout2);
		    for (ivmt3 = vmt3.begin(); ivmt3 != vmt3.end(); ivmt3++ ) {
			if( (*ivmt3).nY() == ny) {
		            int inx = (*ivmt3).nX();
    		            lt->addBit(inx-nx, 2, *ind);
	        	}
		    }    
    		}
	    }	
    	}
    }
         
    //  Looking for tiles in station 1

    vmt = m_layout1.tiles( *pad , m_layout3, 16 );
    for ( ind = pads->begin(); ind != pads->end(); ind++ ) {
        if ( (*ind)->station() == 1 ) {
            for (ivmt = vmt.begin(); ivmt != vmt.end(); ivmt++ ) {
    		if (**ind == *ivmt  ) {
	            vmt3 = m_layout3.tiles( *ivmt , m_layout1);
		    for (ivmt3 = vmt3.begin(); ivmt3 != vmt3.end(); ivmt3++ ) {
			if( (*ivmt3).nY() == ny) {
		            int inx = (*ivmt3).nX();
    		            lt->addBit(inx-nx, 1, *ind);
	        	}
		    }    
    		}
	    }	
    	}
    }  
    
    return lt;
}
