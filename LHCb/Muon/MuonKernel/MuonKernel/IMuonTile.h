// $Id: IMuonTile.h,v 1.1.1.1 2002-01-28 08:48:24 atsareg Exp $

#ifndef MUONKERNEL_IMUONTILE_H
#define MUONKERNEL_IMUONTILE_H     1

/** @class IMuonTile IMuonTile MuonKernel/IMuonTile.h

   Interface for Muon System entities which are identifiable
   with a MuonSystemID.
   
   @author  Andrei Tsaregorodtsev
   @date  12 January 2002
*/ 

#include "MuonKernel/MuonSystemID.h"

class IMuonTile {

public:       
    
    /// Accessor to the tile's MuonSystemID
    virtual MuonSystemID id() const = 0 ;

    /// Accessor to the tile's key
    virtual long key() const = 0 ;

    /// comparison to a MuonSystemID
    virtual inline bool operator==(const MuonSystemID& id) const = 0;

};

#endif      // MUONKERNEL_IMUONTILE_H  
