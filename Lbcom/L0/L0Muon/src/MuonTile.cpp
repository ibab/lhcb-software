// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Muon/src/MuonTile.cpp,v 1.2 2001-05-03 09:12:30 atsareg Exp $
#include "GaudiKernel/MsgStream.h"
#include <algorithm>

#include "MuonTile.h"

MuonTile::MuonTile() :  m_quarter(0), m_region(0),
                   m_indexX(0), m_indexY(0) {
}

MuonTile::MuonTile(int iq, int ir, int nx, int ny, MuonLayout ml) {
   m_quarter = iq;
   m_region = ir;
   m_indexX = nx;
   m_indexY = ny;
   m_layout = ml;
}

MuonTile::~MuonTile() {
}

bool MuonTile::touches(MuonTile& mt) {
    std::vector<MuonTile> vmt = m_layout.tiles(mt);
    std::vector<MuonTile>::iterator imt = find(vmt.begin(),vmt.end(),*this);
    return imt!=vmt.end();
}	   
		   
bool operator==(MuonTile mt1, MuonTile mt2) {
    return mt1.quarter()==mt2.quarter() &&
           mt1.region()==mt2.region() &&
	   mt1.nX()==mt2.nX() &&
	   mt1.nY()==mt2.nY();
}		   

MsgStream& operator<<(MsgStream& log, const MuonTile& mt) {
    log << "pad quarter: " << mt.quarter() << endreq;
    log << "pad region: " << mt.region() << endreq;
    log << "pad X index: " << mt.nX() << endreq;
    return log << "pad Y index: " << mt.nY() << endreq;
} 
