
#include "GaudiKernel/MsgStream.h"

#include "L0mTower.h"
#include "L0Muon/L0mPad.h"
#include "L0mFoI.h"

const CLID& CLID_L0mTower = 99995;  // User defined  

L0mTower::L0mTower()  {
}

L0mTower::L0mTower(L0mPad* pad)  {

    m_pad3 = pad;

}

L0mTower::~L0mTower() {
}	

void L0mTower::draw(MsgStream log) {

   log << MSG::DEBUG << "-----" << endreq;
   log << MSG::DEBUG << " St.5              |";
   for(int i=-10; i<11; i++) {
       if(m_st5.find(i) != m_st5.end()) {
           log << MSG::DEBUG << "#|";
       } else {
           log << MSG::DEBUG << "0|";
       }	   
   }
   log << MSG::DEBUG << endreq;
   
   log << MSG::DEBUG << " St.4                  |";
   for(int i=-8; i<9; i++) {
       if(m_st4.find(i) != m_st4.end()) {
           log << MSG::DEBUG << "#|";
       } else {
           log << MSG::DEBUG << "0|";
       }	   
   }
   log << MSG::DEBUG << endreq;
   
   log << MSG::DEBUG << " St.3                                  |#|     "  
	<<  m_pad3->quarter() << "/" 
	<<  m_pad3->region() << "/" 
	<<  m_pad3->nX() << "/" 
	<<  m_pad3->nY() <<  endreq;
   
   log << MSG::DEBUG << " St.2                  |";
   for(int i=-8; i<9; i++) {
       if(m_st2.find(i) != m_st2.end()) {
           log << MSG::DEBUG << "#|";
       } else {
           log << MSG::DEBUG << "0|";
       }	   
   }
   log << MSG::DEBUG << endreq;
   
   log << MSG::DEBUG << " St.1  |";
   for(int i=-16; i<17; i++) {
       if(m_st1.find(i) != m_st1.end()) {
           log << MSG::DEBUG << "#|";
       } else {
           log << MSG::DEBUG << "0|";
       }	   
   }
   log << MSG::DEBUG << endreq;
   
}	   
		   
void L0mTower::addBit(int i, int st, L0mPad* srp ) {

   switch (st) {
       case 1:
           m_st1[i] = srp;
	   break;
       case 2:
           m_st2[i] = srp;
	   break;	
       case 4:
           m_st4[i] = srp;
	   break;
       case 5:
           m_st5[i] = srp;
	   break;	      
   } 

}

L0mPad* L0mTower::findTrack(L0mFoI* foi) {

    int foi5 = foi->xSize(5);
    int foi4 = foi->xSize(4);
    int foi2 = foi->xSize(2);
    int foi1 = foi->xSize(1);
    
    L0mPad* lpd = 0;

    // confirm the candidate in M4 & M5

    for (int inx=0; inx <= foi5; inx++ ) {
	if( m_st5.find(inx) != m_st5.end() ) {
	    lpd = m_st5.find(inx)->second;
	    break;  
	}
	if( m_st5.find(-inx) != m_st5.end() ) {
	    lpd = m_st5.find(-inx)->second;
	    break;  
	}
    }    
    if(!lpd) return 0;
    
    lpd = 0;
    for (int inx=0; inx <= foi4; inx++ ) {
	if( m_st4.find(inx) != m_st4.end() ) {
	    lpd = m_st4.find(inx)->second;
	    break;  
	}
	if( m_st4.find(-inx) != m_st4.end() ) {
	    lpd = m_st4.find(-inx)->second;
	    break;  
	}
    }	
    
    if(!lpd) return 0;
    
    // find the nearest hit in M2
    for (int inx=0; inx <= foi2; inx++ ) {
	if( m_st2.find(inx) != m_st2.end() ) {
	    m_pad2 = m_st2.find(inx)->second;
	    m_ind2 = inx;
	    break;  
	}
	if( m_st2.find(-inx) != m_st2.end() ) {
	    m_pad2 = m_st2.find(-inx)->second;
	    m_ind2 = -inx;
	    break;  
	}
    }
    
    if(!m_pad2) return 0;
    
    // extrapolate and find a hit in M1

    int indext = (int)(abs(m_ind2)*3.58+0.5);  // round to the nearest integer
    indext = (m_ind2<0) ? -indext : indext;
    int it;
    for (int inx=0; inx <= foi1; inx++ ) {
        it = indext+inx;
	if( m_st1.find(it) != m_st1.end() ) {
	    m_pad1 = m_st1.find(it)->second;
	    m_ind1 = it;
	    // log << MSG::DEBUG << " Pad in M1 found " << m_ind1 << endreq;
	    break;  
	}
	it = indext-inx;
	if( m_st1.find(it) != m_st1.end() ) {
	    m_pad1 = m_st1.find(it)->second;
	    m_ind1 = it;
	    // log << MSG::DEBUG << " Pad in M1 found " << m_ind1 << endreq;
	    break;  
	}
    }
        
    return m_pad1;
}

bool L0mTower::isFull() {

    return (m_st1.size() != 0) &&
           (m_st2.size() != 0) &&
	   (m_st4.size() != 0) &&
           (m_st5.size() != 0);

}

bool L0mTower::trackFound() {
// indicates that the track finding was succesful
    return isFull() && m_pad2 && m_pad1;
}
