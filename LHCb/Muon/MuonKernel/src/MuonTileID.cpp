// $Id: MuonTileID.cpp,v 1.3 2002-03-04 16:52:09 atsareg Exp $
// Include files

#include "MuonKernel/MuonTileID.h"
#include "MuonKernel/MuonStationLayout.h"
#include "MuonKernel/MuonSystemLayout.h"

// Hack to get round usage of max
#ifdef WIN32
#include <xutility>
#else
#define _MIN min
#define _MAX max
#endif // WIN32

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
  
  if ( region()  != otherID.region() ||
       quarter() != otherID.quarter()     )  return MuonTileID();
       
  int thisGridX = layout().xGrid();
  int thisGridY = layout().yGrid();
  
  int otherGridX = otherID.layout().xGrid();
  int otherGridY = otherID.layout().yGrid();
  
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
