// $Id: L0mPad.h,v 1.6 2002-05-07 07:17:04 atsareg Exp $ 

#ifndef L0MUON_L0MPAD_H     
#define L0MUON_L0MPAD_H     1 

#include "MuonKernel/MuonTileID.h"
#include "Event/Kernel.h"


class MsgStream;
class MuonLayout;

/** @class L0mPad L0mPad L0Muon/L0mPad.h

   Muon system pads to be used in the L0 trigger logic
   
   @author  Andrei Tsaregorodtsev
   @date  06 April 2001
*/   


class L0mPad   {

public:

    /// Default constructor
    L0mPad();
    /// Constructor taking the key
    L0mPad(const MuonTileID& id) : m_id(id) {}
	      
    ~L0mPad();	       
    /// accessor to the station number
    long station() const {return padId().station();}
    /// accessor to the pad ID
    MuonTileID padId() const {return m_id;}
    
    /// set pad ID
    void setPadId(MuonTileID value) {m_id = value;}
    
    /// dump the pad contents to the log stream
    void print(MsgStream log);          
    
private:
    
    MuonTileID m_id;
    
};

#endif  // L0MUON_L0MPAD_H 
