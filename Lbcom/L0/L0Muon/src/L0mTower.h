// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Muon/src/L0mTower.h,v 1.2 2001-05-03 09:12:23 atsareg Exp $

#ifndef L0MTOWER_H     
#define L0MTOWER_H     1 

#include <map>
#include <utility>
#include "GaudiKernel/ContainedObject.h" 
#include "GaudiKernel/SmartRef.h" 
#include "L0Muon/L0mPad.h"

class L0MuonCandidate;
class MsgStream;

/** @class L0mTower L0mTower.h L0mTower.h 

   Defines L0mTower - a set of bit patterns cooresponding
   to a single trigger seed in M3. This also has associated
   methods to find a trigger candidate and calculate its parameters
   
   @author Andrei Tsaregorodtsev
   @date 6 April 2001 
*/

// Const to denote invalid tower index	
const std::pair<int,int> EMPTY_INDEX(111,111);

class L0mTower {

public:

    /// Default constructor
    L0mTower();    
    /// Constructor from the seed pad in M3
    L0mTower(L0mPad* pad);
	      
    ~L0mTower();	       

    /// Crude Tower display
    void draw(MsgStream& log);
    /// add a bit to a station pad bit pattern
    void addBit(int ix, int iy, int st, L0mPad* pp); 
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
    L0mPad* findTrack(std::vector<int> foiX, std::vector<int> foiY);
    /// Full processing with creating L0MuonCandidate
    L0MuonCandidate* createCandidate(const std::vector<double>& ptpara,
                        	     const std::vector<int>& foiX, 
				     const std::vector<int>& foiY);
    /// Pt calculation of the found candidate
    double pt(const std::vector<double>& ptpara);
    /// Theta at IP
    double theta(const std::vector<double>& ptpara);
    /// Phi at IP
    double phi(const std::vector<double>& ptpara);
    
    typedef std::pair<int,int> HitIndex;
    typedef std::map< HitIndex, SmartRef<L0mPad> > StationMap;	       
       
private:
    
    // References to L0mPads corresponding to the found track 
    SmartRef<L0mPad> m_pad1;
    SmartRef<L0mPad> m_pad2;
    SmartRef<L0mPad> m_pad3;
    
    // Container of a bitmap image of fired pads
    
    std::vector<StationMap> m_bitmap;
    
    HitIndex m_ind1;
    HitIndex m_ind2;
        
    // Internal utility functions
    
    /// Draw a line representing a station
    void drawStation(MsgStream& log, int st, int range);
    /// Search station for a hit and return its index if found
    HitIndex searchStation(int st, int foiX, int cindex = 0 );
    
};

#endif
