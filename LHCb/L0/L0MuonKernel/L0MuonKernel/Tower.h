// $Id: Tower.h,v 1.15 2007-08-27 09:32:24 jucogan Exp $

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
#include "L0MuonKernel/L0MuonStatus.h"
#include "ProcessorKernel/Register.h"
#include "Kernel/MuonTileID.h"
#include "L0MuonKernel/MuonCandidate.h"
//#include "MuonTools/IMuonTileXYZTool.h"
//#include "GaudiKernel/MsgStream.h"


namespace L0Muon {


//   const int ExtrapolationM1[6]={0,+4,+7,+11,+15,+18};

  class Tower  {
  public:

    /// Constructor
    Tower();

    /// Destructor
    ~Tower();
  
    typedef std::pair<int,int> HitIndex;
    typedef std::vector<boost::dynamic_bitset<> > StationMap;    
    typedef std::map< HitIndex, LHCb::MuonTileID> IDMap;
    
    /// Reset the tower
    void reset();
  
    /** Set bits in tower
        
        @param sta : station 
        @param row : row in the tower 
        @param col : column in the tower
    */
    void setBit(int sta, int row, int col);  
 
    /** Map containing pads and their MuonTileID
        
        @param sta : station 
        @param XY  : raw and column in tower
        @param mid : MuonTileID
     */
    void setPadIdMap(int sta, std::pair<int, int> XY, LHCb::MuonTileID mid); 
 
    /** Return the MuonTileID of a pad in tower
 
        @param sta : station 
        @param XY  : raw and column in tower
     */
    LHCb::MuonTileID getPadIdMap(int sta, std::pair<int,int> XY);
    
    /// Draw bits for station 
    void drawStation(int sta);
 
    /** Return foi for constructing the tower 
        (max. values in x direction)
   
        @param sta  : station 
    */
    int maxXFoi(int sta) const { return m_maxXFoI[sta] ; }

    /** Return foi for constructing the tower 
        (max. values in y direction)
   
        @param sta  : station 
    */
    int maxYFoi(int sta) const { return m_maxYFoI[sta] ; }

    /** Set foi for searching candidates
 
        @param sta  : station 
        @param xfoi : foi x value
        @param yfoi : foi y value
    */ 
    void setFoi(int sta, int xfoi, int yfoi) {
      m_xfoi[sta] = xfoi ;
      m_yfoi[sta] = yfoi ;
    }
    
    /// set parameter for calculating pT
    //     void setPtparam(std::vector<double> ptparam) { m_ptparam= ptparam;}
    void setPtparam(std::vector<double> ptparam) { 
      m_dpt= ptparam[0];
      m_nbits = int(ptparam[1]);
    }

    void setProcVersion(int version) {
      m_procVersion= version;
    }
    
    /// set flag for searching without M1
    void setIgnoreM1(bool ignoreM1){ 
      m_ignoreM1 = ignoreM1; 
    }
          
    /// Draw tower
    void draw();
  
    /// Return the tower
    StationMap getTable(int sta){ return m_bittable[sta] ; }
    
    /** Search candidates
        
        @param puID  : MuonTileID of the PU 
     */
    std::vector<PMuonCandidate> processTower(LHCb::MuonTileID & puID);

    /// Cleaning clusters of seeds in a tower
    void cleanSeed(StationMap & map) ; 
    
    ///
    void setDebugMode(bool debug=true) {if (debug) std::cout << "Tower:setDebugMode\n";m_debug=debug;}    
    
  private:

    
    int m_maxXFoI[5]; // FOI Max used to set the size of the tower (X)
    int m_maxYFoI[5]; // FOI Max used to set the size of the tower (Y)

    int m_xfoi[5]; // Foi in X
    int m_yfoi[5]; // Foi in Y
   
    StationMap m_bittable[5]; // Array of bits in each stations
    IDMap m_idmap[5];// Map relating the local coordinates and the MuonTileID of every fired pad
    
    
    // These parameters depend on the emulator version
    //     std::vector<double> m_ptparam; // Parameters used in the PT computation and encoding
    int m_procVersion;
    float m_dpt;
    int m_nbits;
    bool m_ignoreM1; // Ignore M1 flag   
    
    bool m_debug; // Debug flag

};

  std::ostream& operator<<(std::ostream, L0Muon::Tower& r);

};  // namespace L0Muon

#endif      // PROCESSORKERNEL_TOWER_H  






