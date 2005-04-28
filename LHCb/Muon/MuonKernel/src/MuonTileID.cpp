// $Id: MuonTileID.cpp,v 1.8 2005-04-28 09:39:15 jucogan Exp $
// Include files

#include "MuonKernel/MuonTileID.h"
#include "MuonKernel/MuonStationLayout.h"
#include "MuonKernel/MuonSystemLayout.h"
#include <numeric>

MuonTileID::MuonTileID(const MuonTileID& id,
        		   const IMuonLayout& lay,
			   const unsigned int x,
			   const unsigned int y) {
			   
   m_muonid = id.m_muonid;
   MuonLayout this_layout(lay.grid(id));
   setLayout(this_layout);   
   setX(this_layout.xGrid()/id.layout().xGrid()*id.nX()+x);
   setY(this_layout.yGrid()/id.layout().yGrid()*id.nY()+y);			   
}	

MuonTileID MuonTileID::neighbourID(int dirX, int dirY) const {
  MuonTileID result(*this);
  switch (dirX) {
    case MuonBase::RIGHT : 
      result.setX(nX()+1);
      break;
    case MuonBase::LEFT :  
      if(nX() == 0) break;
      result.setX(nX()-1);
      break;   
  }
  switch (dirY) {
    case MuonBase::UP : 
      result.setY(nY()+1);
      break;
    case MuonBase::DOWN :  
      if(nY() == 0) break;
      result.setY(nY()-1);
      break;   
  }
  return result;
}

bool MuonTileID::isDefined() const {
  return m_muonid != 0;
}		   

bool MuonTileID::isValid() const {

  if ( ! isDefined() ) return false;

  MuonLayout ml = layout(); 
  
  return ml.isValidID(*this);
}

MuonTileID MuonTileID::intercept(const MuonTileID& otherID) const {

  // check first that the two strips are really intercepting
  
  if ( station() != otherID.station() ||
       quarter() != otherID.quarter()     )  return MuonTileID();
       
  int thisGridX = layout().xGrid();
  int thisGridY = layout().yGrid();
  
  int otherGridX = otherID.layout().xGrid();
  int otherGridY = otherID.layout().yGrid();
  if (otherID.region()>region()) {
    int rfactor    = (1<<otherID.region())/(1<<region());
    otherGridX = otherID.layout().xGrid()/rfactor;
    otherGridY = otherID.layout().yGrid()/rfactor;
  } else if (otherID.region()<region()) {
    int rfactor    = (1<<region())/(1<<otherID.region());
    otherGridX = otherID.layout().xGrid()*rfactor;
    otherGridY = otherID.layout().yGrid()*rfactor;
  } 

  if ( thisGridX > otherGridX ) {
    unsigned int calcX = nX()*otherGridX/thisGridX;
    if (calcX != otherID.nX() ) return MuonTileID();
  } else {
    unsigned int calcX = otherID.nX()*thisGridX/otherGridX;
    if (calcX != nX() ) return MuonTileID();
  }
  if ( thisGridY > otherGridY ) {
    unsigned int calcY = nY()*otherGridY/thisGridY;
    if (calcY != otherID.nY() ) return MuonTileID();
  } else {
    unsigned int calcY = otherID.nY()*thisGridY/otherGridY;
    if (calcY != nY() ) return MuonTileID();
  }
  
  // Now the strips are intercepting - get it !
  
  int indX = thisGridX < otherGridX ? otherID.nX() : nX();
  int indY = thisGridY < otherGridY ? otherID.nY() : nY();
  
  MuonTileID resultID(*this);
  resultID.setX(indX);
  resultID.setY(indY);
  int lx = std::max(thisGridX,otherGridX);
  int ly = std::max(thisGridY,otherGridY);
  resultID.setLayout(MuonLayout(lx,ly));
  
  return resultID;
}

MuonTileID MuonTileID::containerID(const IMuonLayout& lay) const {
  
  MuonLayout containerLayout(lay.grid(*this));
  MuonTileID containerID(*this);
  containerID.setX(nX()*containerLayout.xGrid()/layout().xGrid());
  containerID.setY(nY()*containerLayout.yGrid()/layout().yGrid());
  containerID.setLayout(containerLayout);
  
  return containerID;
}

int MuonTileID::localX(const IMuonLayout& lay) const {

  MuonLayout padLayout(lay.grid(*this));
  return nX() % padLayout.xGrid() ;

}

int MuonTileID::localY(const IMuonLayout& lay) const {

  MuonLayout padLayout(lay.grid(*this));
  return nY() % padLayout.yGrid() ;

} 

std::string MuonTileID::toString() const {

  char bufnm[64];
 
  sprintf(bufnm,"S%d(%d,%d)Q%d,R%d,%d,%d",
        	station(),
		layout().xGrid(),
		layout().yGrid(),
		quarter(),
		region(),
		nX(),
		nY()); 
  std::string result = bufnm;
  return result;  		

}
