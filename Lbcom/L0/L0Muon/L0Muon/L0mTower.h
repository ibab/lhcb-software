// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Muon/L0Muon/L0mTower.h,v 1.2 2001-07-09 19:39:55 atsareg Exp $

#ifndef L0MTOWER_H     
#define L0MTOWER_H     1 

#include <map>
#include <utility>
#include "GaudiKernel/ContainedObject.h" 
#include "GaudiKernel/SmartRef.h" 
#include "L0Muon/L0mPad.h"

class L0MuonCandidate;
class MsgStream;
class L0mProcUnit;

extern const CLID& CLID_L0mTower;    

/** @class L0mTower L0mTower.h L0mTower.h 

   Defines L0mTower - a set of bit patterns cooresponding
   to a single trigger seed in M3. This also has associated
   methods to find a trigger candidate and calculate its parameters
   
   @author Andrei Tsaregorodtsev
   @date 6 April 2001 
*/

// Const to denote invalid tower index	

class L0mTower : virtual public ContainedObject {

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
    /// set pointer to containing Processing Unit
    void setProcUnit(L0mProcUnit* plpu) { m_procUnit = plpu; }   
    /// check if the Tower has bits in all the stations
    bool isFull();
    /// check if a track was found
    bool trackFound();
    /// accessor to the resulting pad in M3
    L0mPad* padM3() { return m_pad3; }
    /// accessor to the resulting pads
    L0mPad* pad(int station);
    /// find track given the Fields of interest
    L0mPad* findTrack();
    /// Full processing with creating L0MuonCandidate
    L0MuonCandidate* createCandidate();
    /// Pt of the found candidate
    double pt() { return m_pt; }
    /// Theta at IP
    double theta() { return m_theta; }
    /// Phi at IP
    double phi() { return m_phi; }
    /// Nearest pad
    int nearest(int sta, int foiX, int yindex);
    /// Do the "limited" Y smearing of pads falling out of the PU
    void limitedY();
    
    /// Retrieve pointer to class defininition structure
    virtual const CLID& clID() const { return L0mTower::classID(); }
    static const CLID& classID()     { return CLID_L0mTower; }          
    
    typedef std::pair<int,int> HitIndex;
    typedef std::map< HitIndex, SmartRef<L0mPad> > StationMap;	
       
private:
    
    // Reference to seed pad in M3
    SmartRef<L0mPad> m_pad3;
    
    // Container of a bitmap image of fired pads
    
    //std::vector<StationMap> m_bitmap;
    StationMap m_bitmap[5];
    
    // Indices of the pads chosen while track finding
    //std::vector<HitIndex> m_indices;
    HitIndex m_indices[5];
    
    // Extrapolation to M1
    int m_extra1;
    // Track found flag
    bool m_found;
    // LimitedY was applied
    bool m_limited;
    
    // Calculated track parameters
    double m_pt;
    double m_theta;
    double m_phi;
    
    // Containing Processing Unit
    
    L0mProcUnit* m_procUnit;
        
    // Internal utility functions
    
    /// Draw a line(s) representing a station
    void drawStation(MsgStream& log, int st, int xrange, int yrange);
    /// Search station for a hit and return its index if found
    HitIndex searchStation(bool& found, int st, int foiX, 
                           int cindex = 0, int yindex = 0 );
     /// Pt calculation of the found candidate
    double ptcalc();   
};

#endif
