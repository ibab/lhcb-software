// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Muon/L0Muon/MuonTile.h,v 1.3 2001-05-03 12:08:35 atsareg Exp $

#ifndef L0MUON_MUONTILE_H
#define L0MUON_MUONTILE_H     1

/** @class MuonTile MuonTile L0Muon/MuonTile.h

   Base class for various granullarities used in the L0Muon trigger.
   This can be pads, PU, logical units, etc.
   
   @author  Andrei Tsaregorodtsev
   @date  06 April 2001
*/ 

#include "L0Muon/MuonLayout.h"

class MsgStream;

class MuonTile {

public:

    /// Default constructor
    MuonTile();
    /// Constructor setting initial values
    MuonTile(int iq, int ir, int nx, int ny, MuonLayout ml);
	      
    ~MuonTile();	       
    
    /// Accessor to the tile quarter
    long quarter() const {return m_quarter;}
    /// Accessor to the tile region
    long region() const {return m_region;}
    /// Accessor to the tile X index
    long nX() const {return m_indexX;}
    /// Accessor to the tile Y index
    long nY() const {return m_indexY;}
    /// Accessor to the tile layout
    MuonLayout layout() const { return m_layout; }
    
    /// Set tile quarter
    void setQuarter(long value) {m_quarter = value;}
    /// Set tile region
    void setRegion(long value) {m_region = value;}
    /// Set tile region X index
    void setNX(long value) {m_indexX = value;}
    /// Set tile region Y index
    void setNY(long value) {m_indexY = value;}
    
    /// Check if the MuonTile touches the other MuonTile
    bool touches(MuonTile& mt);
    
private:

    long m_quarter;
    long m_region;    
    long m_indexX;
    long m_indexY;
    MuonLayout m_layout;

};
 
MsgStream& operator<<(MsgStream& log, const MuonTile& mt); 

#endif      // L0MUON_MUONTILE_H  
