// $Id: MuonSystemID.cpp,v 1.1.1.1 2002-01-28 08:48:22 atsareg Exp $
// Include files

#include "MuonKernel/MuonSystemID.h"
#include "MuonKernel/MuonStationLayout.h"
#include "MuonKernel/MuonSystemLayout.h"

// Hack to get round usage of max
#ifdef WIN32
#include <xutility>
#else
#define _MIN min
#define _MAX max
#endif // WIN32

MuonSystemID::MuonSystemID(const MuonSystemID& id,
        		   const IMuonLayout& lay,
			   const unsigned int x,
			   const unsigned int y) {
			   
   m_muonid = id.m_muonid;
   MuonLayout this_layout(lay.grid(id));
   setLayout(this_layout);   
   setX(this_layout.xGrid()/id.layout().xGrid()*id.nX()+x);
   setY(this_layout.yGrid()/id.layout().yGrid()*id.nY()+y);			   
}			   

bool MuonSystemID::isValid() const {

  if (m_muonid == 0) return false;

  MuonLayout ml = layout(); 

  int nx = nX();
  int ny = nY();

  if(nx >= 0 &&
     nx < (2*ml.xGrid()) &&
     ny >= (ml.yGrid()) &&
     ny < 2*ml.yGrid()) {
    return true;
  }
  if(nx >= (ml.xGrid()) &&
     nx < 2*ml.xGrid() &&
     ny >= 0 &&
     ny < ml.yGrid()) {
    return true;
  }
  return false; 
}

MuonSystemID MuonSystemID::intercept(const MuonSystemID& otherID) const {

  // check first that the two strips are really intercepting
  
  if ( region()  != otherID.region() ||
       quarter() != otherID.quarter()     )  return MuonSystemID();
       
  int thisGridX = layout().xGrid();
  int thisGridY = layout().yGrid();
  
  int otherGridX = otherID.layout().xGrid();
  int otherGridY = otherID.layout().yGrid();
  
  if ( thisGridX > otherGridX ) {
    unsigned int calcX = nX()*otherGridX/thisGridX;
    if (calcX != otherID.nX() ) return MuonSystemID();
  } else {
    unsigned int calcX = otherID.nX()*thisGridX/otherGridX;
    if (calcX != nX() ) return MuonSystemID();
  }
  if ( thisGridY > otherGridY ) {
    unsigned int calcY = nY()*otherGridY/thisGridY;
    if (calcY != otherID.nY() ) return MuonSystemID();
  } else {
    unsigned int calcY = otherID.nY()*thisGridY/otherGridY;
    if (calcY != nY() ) return MuonSystemID();
  }
  
  // Now the strips are intercepting - get it !
  
  int indX = thisGridX < otherGridX ? otherID.nX() : nX();
  int indY = thisGridY < otherGridY ? otherID.nY() : nY();
  
  MuonSystemID resultID(*this);
  resultID.setX(indX);
  resultID.setY(indY);
  
  return resultID;
}

MuonSystemID MuonSystemID::getContainerID(const IMuonLayout& lay) const {
  
  MuonLayout containerLayout(lay.grid(*this));
  MuonSystemID containerID(*this);
  containerID.setX(nX()*containerLayout.xGrid()/layout().xGrid());
  containerID.setY(nY()*containerLayout.yGrid()/layout().yGrid());
  containerID.setLayout(containerLayout);
  
  return containerID;
}

int MuonSystemID::localX(const IMuonLayout& lay) const {

  MuonLayout padLayout(lay.grid(*this));
  return nX() % padLayout.xGrid() ;

}

int MuonSystemID::localY(const IMuonLayout& lay) const {

  MuonLayout padLayout(lay.grid(*this));
  return nY() % padLayout.yGrid() ;

} 

// MuonSystemID MuonSystemID::getContainerID(const MuonStationLayout& lay) const {
//   
//   MuonLayout ml = lay.layout(region()) ;  
//   return getContainerID(ml);
// }
// 
// MuonSystemID MuonSystemID::getContainerID(const MuonSystemLayout& lay) const {
//   
//   MuonLayout ml = lay.layout(station(),region()) ;  
//   return getContainerID(ml);
// }
