// $Id: L0mTower.h,v 1.7 2002-08-02 10:43:54 atsareg Exp $

#ifndef L0MTOWER_H     
#define L0MTOWER_H     1 

#include <map>
#include <utility>
#include "L0Muon/L0mPad.h"
#include "MuonKernel/MuonTileID.h"
#include "MuonTools/IMuonTileXYZTool.h"   

class L0MuonCandidate;
class MsgStream;
class L0mProcUnit;


/** @class L0mTower L0mTower.h L0mTower.h 

   Defines L0mTower - a set of bit patterns cooresponding
   to a single trigger seed in M3. This also has associated
   methods to find a trigger candidate and calculate its parameters
   
   @author Andrei Tsaregorodtsev
   @date 6 April 2001 
*/

// Const to denote invalid tower index	

class L0mTower {

public:

    /// Default constructor
    L0mTower();    
    /// Constructor from the seed pad in M3
    L0mTower(MuonTileID pad);
	      
    ~L0mTower();	       

    /// Crude Tower display
    void draw(MsgStream& log);
    /// add a bit to a station pad bit pattern
    void addBit(int ix, int iy, int st, MuonTileID pp); 
    /// set pointer to containing Processing Unit
    void setProcUnit(L0mProcUnit* plpu) { m_procUnit = plpu; }   
    /// set pointer to Muon system geometry tool
    void setMuonTool(IMuonTileXYZTool* pmto) { m_iTileXYZTool = pmto; } 
    /// check if the Tower has bits in all the stations
    bool isFull();
    /// check if a track was found
    bool trackFound();
    /// accessor to the resulting pad in M3
    MuonTileID padM3() { return m_pad3; }
    /// accessor to the resulting pads
    MuonTileID pad(int station);
    /// find track given the Fields of interest
    MuonTileID findTrack();
    /// Full processing with creating L0MuonCandidate
    L0MuonCandidate* createCandidate();
    /// Pt of the found candidate
    double pt() { return m_pt; }
    /// Theta at IP
    double theta() { return m_theta; }
    /// Phi at IP
    double phi() { return m_phi; }
    /// Associated Processing unit
    L0mProcUnit* procUnit() { return m_procUnit; };
    /// Nearest pad
    int nearest(int sta, int foiX, int yindex);
    /// Get M1 extrapolation offset
    int getExtra1() { return m_extra1; }
    /// Do the "limited" Y smearing of pads falling out of the PU
    void limitedY();
    /// set the flag to ignore or not M1
    void setFlagIgnoreM1(bool flag){ m_ignoreM1 = flag ;}
    
    typedef std::pair<int,int> HitIndex;
    typedef std::map< HitIndex, MuonTileID > StationMap;	
       
private:
    
    // Reference to seed pad in M3
    MuonTileID m_pad3;
    
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
    // Ignore M1 flag
    bool m_ignoreM1;
    
    
    // Calculated track parameters
    double m_pt;
    double m_theta;
    double m_phi;
    
    // Containing Processing Unit    
    L0mProcUnit* m_procUnit;
    // Muon geometry tool
    IMuonTileXYZTool *m_iTileXYZTool;
        
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
