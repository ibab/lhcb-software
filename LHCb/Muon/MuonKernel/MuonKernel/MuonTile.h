// $Id: MuonTile.h,v 1.5 2005-12-16 15:57:59 asatta Exp $

#ifndef MUONKERNEL_MUONTILE_H
#define MUONKERNEL_MUONTILE_H     1

/** @class MuonTile MuonTile.h MuonKernel/MuonTile.h

   Basic implementation of the IMuonTile interface for Muon System 
   entities which are identifiable with a MuonTileID.
   
   @author  Andrei Tsaregorodtsev
   @date  12 January 2002
*/ 

#include "Kernel/MuonTileID.h"

class MuonTile {

public:       
    
    /// Constructors
    MuonTile() {}
    MuonTile(const LHCb::MuonTileID& mid) {
      m_muonid = mid;
    }
    
    /// Destructor
    virtual ~MuonTile() {}
    
    /// Accessor to the tile's MuonTileID
    virtual LHCb::MuonTileID& id()  {return m_muonid; }
    
    /// Accessor to the tile's key
    virtual long key() const {return m_muonid.key(); }

    virtual bool operator==(const LHCb::MuonTileID& mid) {
      return m_muonid == mid;
    }
    
    /// Accessor to the tile's MuonTileID station
    virtual unsigned int station() {return m_muonid.station();}
    
    /// Accessor to the tile's LHCb::MuonTileID quarter
    virtual unsigned int quarter() {return m_muonid.quarter();}
    
    /// Accessor to the tile's LHCb::MuonTileID region
    virtual unsigned int region() {return m_muonid.region();}
    
    /// Accessor to the tile's LHCb::MuonTileID nX
    virtual unsigned int nX() {return m_muonid.nX();}
    
    /// Accessor to the tile's LHCb::MuonTileID nY
    virtual unsigned int nY() {return m_muonid.nY();}
    
    /// Accessor to the tile's LHCb::MuonTileID station
    virtual void setStation(int sta) { m_muonid.setStation(sta);}
    
    /// Accessor to the tile's LHCb::MuonTileID quarter
    virtual void setQuarter(int q) { m_muonid.setQuarter(q);}
    
    /// Accessor to the tile's LHCb::MuonTileID region
    virtual void setRegion(int r) { m_muonid.setRegion(r);}
    
    /// Accessor to the tile's LHCb::MuonTileID nX
    virtual void setX(int x) { m_muonid.setX(x);}
    
    /// Accessor to the tile's LHCb::MuonTileID nY
    virtual void setY(int y) { m_muonid.setY(y);}
    
private:

   LHCb::MuonTileID m_muonid;   

};

#endif      // MUONKERNEL_MUONTILE_H  
