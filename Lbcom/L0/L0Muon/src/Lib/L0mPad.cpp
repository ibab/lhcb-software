// $Id: L0mPad.cpp,v 1.4 2001-07-26 13:05:37 cattanem Exp $

#include "GaudiKernel/MsgStream.h"

#include "L0Muon/L0mPad.h"
 

L0mPad::L0mPad() : m_station(0) {}

L0mPad::L0mPad(int st, int iq, int ir, int nx, int ny, MuonLayout ml) :
              MuonTile(iq, ir, nx, ny, ml),
	      m_station(st) {}

L0mPad::~L0mPad() {
}	

void L0mPad::print(MsgStream log ) {

   log << MSG::DEBUG << " # Pad station/quarter/region/nX/nY " 
       << station() << "/" << quarter() << "/" << region() << "/" 
       << nX() << "/" << nY() 
       << " x=" << x() << " y=" << y() << endreq;

}	   
