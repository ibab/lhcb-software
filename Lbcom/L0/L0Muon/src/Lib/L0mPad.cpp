// $Id: L0mPad.cpp,v 1.5 2002-05-07 07:29:09 atsareg Exp $

#include "GaudiKernel/MsgStream.h"

#include "L0Muon/L0mPad.h"
#include "MuonKernel/MuonLayout.h"
 

L0mPad::L0mPad() {}


L0mPad::~L0mPad() {
}	

void L0mPad::print(MsgStream log ) {

   log << MSG::DEBUG << " # Pad station/quarter/region/nX/nY " 
       << padId().station() << "/" << padId().quarter() << "/" 
       << padId().region() << "/" 
       << padId().nX() << "/" << padId().nY() << endreq;

}	   
