// $Id: Tower.h,v 1.6 2004-11-17 08:35:00 cattanem Exp $

#ifndef L0MUONKERNEL_TOWER_H
#define L0MUONKERNEL_TOWER_H     1

/** @class Tower Tower.h L0MuonKernel/Tower.h

   Class representing a L0Muon Trigger uniform representation
   of data belonging to a PU for the hardware simulations
   
   @author  Andrei Tsaregorodtsev
   @date  12 June 2003
*/ 
// STL includes
#include <map>
#include <vector>
#include <utility>  // For std::pair
#include <boost/dynamic_bitset.hpp>
//=============L0MuonCandidate=====
#include "Event/L0MuonCandidate.h"
#include "Event/L0Muon.h"
//#include "L0MuonKernel/Unit.h"
//#include "L0MuonKernel/L0mProcUnit.h"

#include "L0MuonKernel/Cleaning.h"
#include "L0MuonKernel/CandidateTower.h"
#include "L0MuonKernel/CandidateSearch.h"
#include "L0MuonKernel/Register.h"
//#include "MuonKernel/MuonSystemLayout.h"
#include "MuonTools/IMuonTileXYZTool.h"
#include "GaudiKernel/MsgStream.h"


class L0MuonCandidate;

namespace L0Muon {

  class Tower  {
  public:
    Tower();
    ~Tower();
  
    typedef std::pair<int,int> HitIndex;
    typedef std::vector<boost::dynamic_bitset<> > StationMap;    
    typedef std::map< HitIndex, MuonTileID> IDMap;
    

    void reset();
  
    void setBit(int sta, int row, int col, MsgStream & log);  
    void setPadIdMap(int sta, std::pair<int, int> XY, MuonTileID mid); 
    MuonTileID getPadIdMap(int sta, std::pair<int,int> XY);
    
    boost::dynamic_bitset<> getBits(int sta, HitIndex seed, int xfoi);
    boost::dynamic_bitset<> getBits(int sta, int x, int y, int xfoi);
    
    void drawStation(int sta, MsgStream & log);
    int maxXFoi(int sta){ return m_maxXFoI[sta] ; }
    int maxYFoi(int sta){ return m_maxYFoI[sta] ; }

    void setXfoi(int sta, int xfoi) {m_xfoi[sta] = xfoi ;}
    void setYfoi(int sta, int yfoi) {m_yfoi[sta] = yfoi ;}
    
    void setPtparam(std::vector<double> ptparam) { m_ptparam= ptparam;}
    
    void setIgnoreM1(bool ignoreM1){ m_ignoreM1 = ignoreM1; }
    
      
      
    
    void draw(MsgStream & log);
    StationMap getTable(int sta){ return m_bittable[sta] ; }
    
    void processTower(MuonTileID & puID, MsgStream & log);

    // added for L0Buffer
    boost::dynamic_bitset<> addr(int i){ return m_addr[i];}
    


    // Processing with creating L0MuonCandidate
    L0MuonCandidate* createCandidate(double p, double th, double phi,int flag);
    std::vector<L0MuonCandidate*> puCandidates(){ return m_puCandidates;}
    
    //for offsets in ntuple
    std::vector< std::pair<L0MuonCandidate*, std::vector<int> > > 
    candOffset(){ return m_offForCand;}
      

    //functions to extract calculated track parameters
    double pt() { return m_pt; }
    double theta() { return m_theta; }
    double phi() { return m_phi; }
    

    /// set pointer to Muon system geometry tool
    void  setMuonToolInTower(IMuonTileXYZTool* pmto) { m_iTileXYZTool = pmto; }

    int numberOfCand(){ return m_ncand ;}

        
    
  private:

        
    Cleaning m_clean;
    CandidateTower m_ctower ;
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
    
  // Muon geometry tool
    IMuonTileXYZTool *m_iTileXYZTool;

    std::vector<L0MuonCandidate*> m_puCandidates;
    
    // for offset in ntuple
    
    std::pair<L0MuonCandidate*, std::vector<int> > m_offsetx;
    std::vector<std::pair<L0MuonCandidate*, std::vector<int> > > m_offForCand;
    

};

  std::ostream& operator<<(std::ostream, L0Muon::Tower& r);

};  // namespace L0Muon

#endif      // L0MUONKERNEL_TOWER_H  






