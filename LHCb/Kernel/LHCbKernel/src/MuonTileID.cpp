// Include files

#include "Kernel/MuonTileID.h"
#include <numeric>

LHCb::MuonTileID::MuonTileID(const MuonTileID& id,
                             const IMuonLayout& lay,
                             const unsigned int x,
                             const unsigned int y) {

   m_muonid = id.m_muonid;
   MuonLayout this_layout(lay.grid(id));
   setLayout(this_layout);
   setX(this_layout.xGrid()/id.layout().xGrid()*id.nX()+x);
   setY(this_layout.yGrid()/id.layout().yGrid()*id.nY()+y);
}

LHCb::MuonTileID LHCb::MuonTileID::neighbourID(int dirX, int dirY) const {
  LHCb::MuonTileID result(*this);
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


bool LHCb::MuonTileID::isValid() const {

  if ( ! isDefined() ) return false;

  MuonLayout ml = layout();

  return ml.isValidID(*this);
}

LHCb::MuonTileID
LHCb::MuonTileID::intercept(const LHCb::MuonTileID& otherID) const {

  // check first that the two strips are really intercepting

  if ( station() != otherID.station() ||
       quarter() != otherID.quarter()     )  return LHCb::MuonTileID();

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
    if (calcX != otherID.nX() ) return LHCb::MuonTileID();
  } else {
    unsigned int calcX = otherID.nX()*thisGridX/otherGridX;
    if (calcX != nX() ) return LHCb::MuonTileID();
  }
  if ( thisGridY > otherGridY ) {
    unsigned int calcY = nY()*otherGridY/thisGridY;
    if (calcY != otherID.nY() ) return LHCb::MuonTileID();
  } else {
    unsigned int calcY = otherID.nY()*thisGridY/otherGridY;
    if (calcY != nY() ) return LHCb::MuonTileID();
  }

  // Now the strips are intercepting - get it !

  int indX = thisGridX < otherGridX ? otherID.nX() : nX();
  int indY = thisGridY < otherGridY ? otherID.nY() : nY();

  LHCb::MuonTileID resultID(*this);
  resultID.setX(indX);
  resultID.setY(indY);
  int lx = std::max(thisGridX,otherGridX);
  int ly = std::max(thisGridY,otherGridY);
  resultID.setLayout(MuonLayout(lx,ly));

  return resultID;
}

LHCb::MuonTileID LHCb::MuonTileID::containerID(const IMuonLayout& lay) const {

  MuonLayout containerLayout(lay.grid(*this));
  LHCb::MuonTileID containerID(*this);
  containerID.setX(nX()*containerLayout.xGrid()/layout().xGrid());
  containerID.setY(nY()*containerLayout.yGrid()/layout().yGrid());
  containerID.setLayout(containerLayout);

  return containerID;
}

int LHCb::MuonTileID::localX(const IMuonLayout& lay) const {

  MuonLayout padLayout(lay.grid(*this));
  return nX() % padLayout.xGrid() ;

}

int LHCb::MuonTileID::localY(const IMuonLayout& lay) const {

  MuonLayout padLayout(lay.grid(*this));
  return nY() % padLayout.yGrid() ;

}

std::string LHCb::MuonTileID::toString() const {

  char bufnm[64];

  sprintf(bufnm,"S%u(%u,%u)Q%u,R%u,%u,%u",
        	station(),
		layout().xGrid(),
		layout().yGrid(),
		quarter(),
		region(),
		nX(),
		nY());
  return std::string ( bufnm );

}

std::ostream& LHCb::MuonTileID::fillStream(std::ostream& s) const
{
  s << "{ ";
  if      ( !isValid()   ) { s << "WARNING : Unvalid MuonTileID"; }
  else if ( !isDefined() ) { s << "WARNING : Undefined MuonTileID"; }
  else
  {
    s << "MuonTileID : " << (int)(*this)
      << " : station=" << station()
      << " region=" << region()
      << " quarter=" << quarter()
      << " nX=" << nX()
      << " nY=" << nY();
  }
  return s << " }";
}
