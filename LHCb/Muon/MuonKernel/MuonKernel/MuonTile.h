// $Id: MuonTile.h,v 1.2 2002-02-18 09:22:54 atsareg Exp $

#ifndef MUONKERNEL_MUONTILE_H
#define MUONKERNEL_MUONTILE_H     1

/** @class IMuonTile IMuonTile MuonKernel/IMuonTile.h

   Basic implementation of the IMuonTile interface for Muon System 
   entities which are identifiable with a MuonTileID.
   
   @author  Andrei Tsaregorodtsev
   @date  12 January 2002
*/ 

#include "MuonKernel/MuonTileID.h"
#include "MuonKernel/IMuonTile.h"

class MuonTile : virtual public IMuonTile {

public:       
    
    /// Constructors
    MuonTile() {}
    MuonTile(const MuonTileID& mid) {
      m_muonid = mid;
    }
    
    /// Destructor
    virtual ~MuonTile() {}
    
    /// Accessor to the tile's MuonTileID
    virtual MuonTileID id() const {return m_muonid; }
    
    /// Accessor to the tile's key
    virtual long key() const {return m_muonid.key(); }

    virtual bool operator==(const MuonTileID& mid) const {
      return id() == mid;
    }
    
    /// Accessor to the tile's MuonTileID station
    virtual unsigned int station() const {return id().station();}
    
    /// Accessor to the tile's MuonTileID quarter
    virtual unsigned int quarter() const {return id().quarter();}
    
    /// Accessor to the tile's MuonTileID region
    virtual unsigned int region() const {return id().region();}
    
    /// Accessor to the tile's MuonTileID nX
    virtual unsigned int nX() const {return id().nX();}
    
    /// Accessor to the tile's MuonTileID nY
    virtual unsigned int nY() const {return id().nY();}
    
private:

   MuonTileID m_muonid;   

};

#endif      // MUONKERNEL_MUONTILE_H  
