// $Id: IMuonTile.h,v 1.2 2002-02-18 09:22:52 atsareg Exp $

#ifndef MUONKERNEL_IMUONTILE_H
#define MUONKERNEL_IMUONTILE_H     1

/** @class IMuonTile IMuonTile MuonKernel/IMuonTile.h

   Interface for Muon System entities which are identifiable
   with a MuonTileID.
   
   @author  Andrei Tsaregorodtsev
   @date  12 January 2002
*/ 

#include "MuonKernel/MuonTileID.h"

class IMuonTile {

public:       
    
    /// Accessor to the tile's MuonTileID
    virtual MuonTileID id() const = 0 ;

    /// Accessor to the tile's key
    virtual long key() const = 0 ;

    /// comparison to a MuonTileID
    virtual inline bool operator==(const MuonTileID& id) const = 0;
    
    /// Accessor to the tile's MuonTileID station
    virtual unsigned int station() const = 0 ;
    
    /// Accessor to the tile's MuonTileID quarter
    virtual unsigned int quarter() const = 0 ;
    
    /// Accessor to the tile's MuonTileID region
    virtual unsigned int region() const = 0 ;
    
    /// Accessor to the tile's MuonTileID nX
    virtual unsigned int nX() const = 0 ;
    
    /// Accessor to the tile's MuonTileID nY
    virtual unsigned int nY() const = 0 ;

};

#endif      // MUONKERNEL_IMUONTILE_H  
