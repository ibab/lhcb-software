// $Id: MuonTile.h,v 1.1.1.1 2002-01-28 08:48:24 atsareg Exp $

#ifndef MUONKERNEL_MUONTILE_H
#define MUONKERNEL_MUONTILE_H     1

/** @class IMuonTile IMuonTile MuonKernel/IMuonTile.h

   Basic implementation of the IMuonTile interface for Muon System 
   entities which are identifiable with a MuonSystemID.
   
   @author  Andrei Tsaregorodtsev
   @date  12 January 2002
*/ 

#include "MuonKernel/MuonSystemID.h"
#include "MuonKernel/IMuonTile.h"

class MuonTile : virtual public IMuonTile {

public:       
    
    /// Constructors
    MuonTile() {}
    MuonTile(const MuonSystemID& mid) {
      m_muonid = mid;
    }
    
    /// Destructor
    virtual ~MuonTile() {}
    
    /// Accessor to the tile's MuonSystemID
    virtual MuonSystemID id() const {return m_muonid; }
    
    /// Accessor to the tile's key
    virtual long key() const {return m_muonid.key(); }

    virtual bool operator==(const MuonSystemID& mid) const {
      return id() == mid;
    }
    
private:

   MuonSystemID m_muonid;   

};

#endif      // MUONKERNEL_MUONTILE_H  
