// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Muon/src/Lib/L0mPad.cpp,v 1.3 2001-07-12 20:28:12 atsareg Exp $

#include "GaudiKernel/MsgStream.h"

#include "L0Muon/L0mPad.h"

const CLID& CLID_L0mPad = 6003;  // User defined  

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
