// $Header:

#ifndef L0MTOWER_H     
#define L0MTOWER_H     1 

#include <map>
#include "GaudiKernel/ContainedObject.h" 
#include "GaudiKernel/SmartRef.h" 
#include "L0Muon/L0mPad.h"

// Externals
extern const CLID& CLID_L0mTower;           


class L0mFoI;
class MsgStream;

/** @class L0mTower L0mTower.h L0mTower.h 

   Defines L0mTower - a set of bit patterns cooresponding
   to a single trigger seed in M3.
   
   @author Andrei Tsaregorodtsev
   @date 6 April 2001 
*/

class L0mTower : virtual public ContainedObject {

public:

    /// Default constructor
    L0mTower();    
    /// Constructor from the seed pad in M3
    L0mTower(L0mPad* pad);
	      
    ~L0mTower();	       

    /// Crude Tower display
    void draw(MsgStream log);
    /// add a bit to a station pad bit pattern
    void addBit(int i, int st, L0mPad* pp); 
    /// check if the Tower has bits in all the stations
    bool isFull();
    /// check if a track was found
    bool trackFound();
    /// accessor to the resulting pad in M3
    L0mPad* padM3() { return m_pad3; }
    /// accessor to the resulting pad in M2
    L0mPad* padM2() { return m_pad2; }
    /// accessor to the resulting pad in M1
    L0mPad* padM1() { return m_pad1; }
    /// find track given the Fields of interest
    L0mPad* findTrack(L0mFoI* foi);
    

/// Retrieve pointer to class defininition structure
    virtual const CLID& clID() const { return L0mTower::classID(); }
    static const CLID& classID()     { return CLID_L0mTower; }          
    
private:
    
    int m_ind1;
    int m_ind2;
    
    SmartRef<L0mPad> m_pad1;
    SmartRef<L0mPad> m_pad2;
    SmartRef<L0mPad> m_pad3;
    
    std::map< int, SmartRef<L0mPad> > m_st1;
    std::map< int, SmartRef<L0mPad> > m_st2; 
    std::map< int, SmartRef<L0mPad> > m_st4;
    std::map< int, SmartRef<L0mPad> > m_st5; 
};

typedef ObjectVector<L0mTower> L0mTowers;

#endif
