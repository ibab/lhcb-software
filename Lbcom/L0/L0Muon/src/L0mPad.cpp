#include "GaudiKernel/MsgStream.h"

#include "L0Muon/L0mPad.h"

const CLID& CLID_L0mPad = 99996;  // User defined  

L0mPad::L0mPad() : m_station(0) {}

L0mPad::L0mPad(int st, int iq, int ir, int nx, int ny) :
              MuonTile(iq, ir, nx, ny),
	      m_station(st) {}

L0mPad::~L0mPad() {
}	

void L0mPad::print(MsgStream log ) {

   log << MSG::DEBUG << " # Pad station/quarter/region/nX/nY " 
       << station() << "/" << quarter() << "/" << region() << "/" 
       << nX() << "/" << nY() << endreq;

}	   
		   
bool operator==(L0mPad lpd, MuonTile mt) {
    return lpd.quarter()==mt.quarter() &&
           lpd.region()==mt.region() &&
           lpd.nX()==mt.nX() &&
           lpd.nY()==mt.nY();
}
