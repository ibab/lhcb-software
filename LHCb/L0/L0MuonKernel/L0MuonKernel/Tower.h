// $Id: Tower.h,v 1.10 2005-04-04 07:58:43 atsareg Exp $

#ifndef PROCESSORKERNEL_TOWER_H
#define PROCESSORKERNEL_TOWER_H     1

/** @class Tower Tower.h L0MuonKernel/Tower.h

    Class representing a uniform representation
    of data belonging to a PU and corresponding to a tower
     
*/ 
// STL includes
#include <map>
#include <vector>
#include <utility>  // For std::pair
#include <boost/dynamic_bitset.hpp>
#include "L0MuonKernel/Candidate.h"
//#include "Event/L0Muon.h"
#include "L0MuonKernel/L0MuonStatus.h"
#include "L0MuonKernel/CandidateTower.h"
#include "L0MuonKernel/CandidateSearch.h"
#include "ProcessorKernel/Register.h"
#include "MuonKernel/MuonTileID.h"
//#include "MuonTools/IMuonTileXYZTool.h"
//#include "GaudiKernel/MsgStream.h"


namespace L0Muon {

  class Tower  {
  public:

    /// Constructor
    Tower();

    /// Destructor
    ~Tower();
  
    typedef std::pair<int,int> HitIndex;
    typedef std::vector<boost::dynamic_bitset<> > StationMap;    
    typedef std::map< HitIndex, MuonTileID> IDMap;
    
    /// Reset the tower
    void reset();
  
    /** Set bits in tower
        
        @param sta : station 
        @param row : row in the tower 
        @param col : column in the tower
        @param log : MSG::DEBUG
    */
    void setBit(int sta, int row, int col);  
 
    /** Map containing pads and their MuonTileID
        
        @param sta : station 
        @param XY  : raw and column in tower
        @param mid : MuonTileID
     */
    void setPadIdMap(int sta, std::pair<int, int> XY, MuonTileID mid); 
 
    /** Return the MuonTileID of a pad in tower
 
        @param sta : station 
        @param XY  : raw and column in tower
     */
    MuonTileID getPadIdMap(int sta, std::pair<int,int> XY);
    
    /** Return pads in the field of  interest for a given seed in M3
        
        @param sta  : station 
        @param seed : seed position 
        @param xfoi : foi 
     */
    boost::dynamic_bitset<> getBits(int sta, HitIndex seed, int xfoi);
    boost::dynamic_bitset<> getBits(int sta, int x, int y, int xfoi);
    
    /// Draw bits for station 
    void drawStation(int sta);
 
    /** Return foi for constructing the tower 
        (max. values in x direction)
   
        @param sta  : station 
    */
    int maxXFoi(int sta){ return m_maxXFoI[sta] ; }

    /** Return foi for constructing the tower 
        (max. values in y direction)
   
        @param sta  : station 
    */
    int maxYFoi(int sta){ return m_maxYFoI[sta] ; }

    /** Set foi (x direction) for searching candidates
 
        @param sta  : station 
        @param xfoi : foi value
    */ 
    void setFoi(int sta, int xfoi, int yfoi) {
      m_xfoi[sta] = xfoi ;
      m_yfoi[sta] = yfoi ;
    }
    
    /// set parameter for calculating pT
    void setPtparam(std::vector<double> ptparam) { m_ptparam= ptparam;}
    
    /// set flag for searching without M1
    void setIgnoreM1(bool ignoreM1){ 
      m_ignoreM1 = ignoreM1; 
      m_csearch.ignoreM1(m_ignoreM1);
    }
          
    /// Draw tower
    void draw();
  
    /// Return the tower
    StationMap getTable(int sta){ return m_bittable[sta] ; }
    
    /** Search candidates
        
        @param puID  : MuonTileID of the PU 
        @param log   : MSG::DEBUG  
     */
    void processTower(MuonTileID & puID);

    /** Return address of candidates 
        
        @param i: number of candidate
 
     */
    boost::dynamic_bitset<> addr(int i){ return m_addr[i];}
    
    /// Create a Candidate
    PCandidate createCandidate(double p, double th, double phi,int flag);
  
    /// Return the candidates 
    std::vector<PCandidate> puCandidates(){ return m_puCandidates;}
    
    /// return offsets for candidates (used for trigger studies)
    //std::vector< std::pair<PCandidate, std::vector<int> > > 
    //candOffset(){ return m_offForCand;}

    /// Function to extract the track parameter: pT
    double pt() { return m_pt; }

    /// Function to extract the track parameter: theta
    double theta() { return m_theta; }
 
    /// Function to extract the track parameter: phi
    double phi() { return m_phi; }
    
    /// Cleaning clusters of seeds in a tower
    void cleanSeed(StationMap & map) ; 
    
    /// Return the number of candidates
    int numberOfCand(){ return m_ncand ;}

    ///
    void setDebugMode() {
      m_debug = true;
    }    
    
  private:

    void xyFromPad(MuonTileID pad, double& x, double& y) ;
    
    CandidateTower m_ctower ;
    CandidateSearch m_csearch ;
    int m_maxXFoI[5];
    int m_maxYFoI[5];
    int m_xfoi[5];
    int m_yfoi[5];
    std::vector<double> m_ptparam;
    bool m_ignoreM1;
   
    StationMap m_bittable[5];
    
    unsigned long int m_ncand ;
    

    //Calculated track parameters
    double m_pt;
    double m_theta;
    double m_phi;
    
    boost::dynamic_bitset<> m_addr[2];

    IDMap m_idmap[5];

    MuonTileID pad;
    
    
    double ptcalc();
    double ptcalcIgnoreM1();
    
    std::vector<PCandidate> m_puCandidates;
    
    // for offset in ntuple
    
    //std::pair<PCandidate, std::vector<int> > m_offsetx;
    //std::vector<std::pair<PCandidate, std::vector<int> > > m_offForCand;

    bool m_debug;
    bool m_seeded;

    //std::vector<double> m_pts;
    //std::vector<double> m_th;
    //std::vector<double> m_ph;
    //std::vector<MuonTileID> m_tid;
};

  std::ostream& operator<<(std::ostream, L0Muon::Tower& r);

};  // namespace L0Muon

#endif      // PROCESSORKERNEL_TOWER_H  






