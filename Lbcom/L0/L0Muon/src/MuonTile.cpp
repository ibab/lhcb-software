#include <iostream>
#include "L0Muon/MuonTile.h"

MuonTile::MuonTile() :  m_quarter(0), m_region(0),
                   m_indexX(0), m_indexY(0) {
}

MuonTile::MuonTile(int iq, int ir, int nx, int ny) {
   m_quarter = iq;
   m_region = ir;
   m_indexX = nx;
   m_indexY = ny;
}

MuonTile::~MuonTile() {
}	

void MuonTile::print() {

   cout << " #######  pad quarter: " << m_quarter << endl;
   cout << " #######  pad region: " << m_region << endl;
   cout << " #######  pad X index: " << m_indexX << endl;
   cout << " #######  pad Y index: " << m_indexY << endl;
}	   
		   
bool operator==(MuonTile mt1, MuonTile mt2) {
    return mt1.quarter()==mt2.quarter() &&
           mt1.region()==mt2.region() &&
	   mt1.nX()==mt2.nX() &&
	   mt1.nY()==mt2.nY();
}		   
