// $Id: CoreUnit.h,v 1.12 2010-07-21 12:24:37 jucogan Exp $
#ifndef L0MUONKERNEL_COREUNIT_H
#define L0MUONKERNEL_COREUNIT_H     1


// STL includes
#include <vector>
//#include "GaudiKernel/MsgStream.h"
#include "L0MuonKernel/MuonCandidate.h"
//#include "Event/L0Muon.h"
#include "L0MuonKernel/L0MUnit.h"
#include "L0MuonKernel/Tower.h"
#include "ProcessorKernel/TileRegister.h"
#include "L0MuonKernel/CandRegisterHandler.h"

namespace L0Muon {

  /** @class CoreUnit CoreUnit.h  L0MuonKernel/CoreUnit.h

  Class representing the kernel of the processing.
  
  It inherits from L0MUnit.

  There is one such unit per PU. Its parent is a ProcUnit.

  Unit's registers are :

  - IN  : - TileRegisters with optical link
                 - name: OL_Q<q>R<r><x,y>_M<sta>_<ind>, where 
                     - q(=1,...,4), r=(1,...,4), x,y refers to the PU attached to this unit (receiver),
                     - sta(=1,...,5) refers to the muon station.
                 - example: OL_Q1R120_M1_0
          - TileRegisters with neighbour data
                 - name: <function>_M<sta>_Q<q0>R<r0><x0,y0>_Q<q1>R<r1><x1,y1>, where : 
                     - function(=Backplane,Crossing,Horizontal,Vertical) caracterises the type of exchange,
                     - q0(=1,...,4), r0=(1,...,4), x0,y0 refers to the receiving PU,
                     - q1(=1,...,4), r1=(1,...,4), x1,y1 refers to the emitting PU,
                     - sta(=1,...,5) refers to the muon station.
                 - example: Crossing_M2_Q1R131_Q1R120
                 - note: these register are filled in the preexecute phase in the FormattingUnit attached to the emitting PU. 

  - OUT : Register with candidates
             - name: CAND_PUQ<q>R<r><x,y>, 
                 where q(=1,...,4), r=(1,...,4), x,y refers to the PU attached to this unit.
             - example: CAND_PUQ1R120 
             - aliases: 
                 - used by converter (BCSUL0BufferCnv): PUCAND_BCSUQ<q>R<r><x,y>_<pu>, 
                   where q(=1,...,4), r=(1,...,4), x,y refers to the BCSU receving the candidates 
                   and pu(=0,...4) is the index of PU in the board (e.g. PUCAND_BCSUQ1R110_0)
                 - used by converter (ProcDataCnv, ProcRawCnv): PUCAND_Q<q>_<board>_<pu>, 
                   where q(=1,...,4) is the quarter, 
                   board(=0,...,11) is the index of the processing board of the PU 
                   and pu(=0,...4) is the index of PU in the board (e.g. PUCAND_Q1_0_0)

  This class holds a Tower where the candidate search is actually performed.

  During the execute phase, it : 
  - transforms strips into logical pads (makePads method),
  - constructs the tower (makeTower method),
  - executes algorithms for searching candidates (processTower method of Tower),
  - fills a register with up to 2 candidates (using a CandRegisterHandler)

  @author  Andrei Tsaregorodtsev, Julien Cogan
  @date  12 June 2003
  */ 
  class CoreUnit : public L0MUnit {

  public:

    /// Default Constructor
    CoreUnit();

    /// Constructor with a MuonTileID
    CoreUnit(LHCb::MuonTileID id);

    /// Destructor
    ~CoreUnit();

    /// Constructor from a xml node
    CoreUnit(DOMNode* pNode);

    /// Set the unit properties
    void setProperties(std::map<std::string,L0Muon::Property>properties);

    /// Return x foi in station sta
    int xFoi(int sta);
    
    /// Return y foi in station sta
    int yFoi(int sta);
    
    /// Return the flag for searching candidates without M1 
    bool ignoreM1();

    /// Return the emulator version
    int procVersion();

    /// Construct logical pads; return false is the pad list is empty
    bool makePads();

    /** Construct the tower (from optical links + neighbours) 
        in the granularity of M3;
        Return true id a seed has been found.
    */
    bool makeTower();

    /// Draw the Tower
    void drawTower() {m_tower.draw();}

    /// Initialize a correspondance table used in case M1 is ignored
    void initializeM1TowerMap();

    /// Set the DEBUG level flag
    void setDebugMode(bool debug = true) ;

    /// Overloads from Unit : set parameters, initialize the CandRegisterHandler with pointer to the output register
    void initialize(); 
    /// Overloads from Unit : search candidates, fill the CandRegisterHandler 
    void execute();
    /// Overloads from Unit : clear the input and output registers 
    void postexecute();

    /// Special method to emulate part of the execute method : construct the tower
    bool preprocess();

    /** Special method to emulate part of the execute method : search candidates (the tower has been contrcuted before hand)
 
        @param yfoi : vector of 5 elements with FOI in X (1/station) 
    */
    std::vector<L0Muon::PMuonCandidate> process(std::vector<int> xfoi ,std::vector<int> yfoi);

    /** Special method to emulate part of the execute method : search candidates (the tower has been contrcuted before hand)

        @param xfoi : pointer to a vector of 5 elements with FOI in X (1/station) 
        @param yfoi : pointer to a vector of 5 elements with FOI in X (1/station) 
    */
    std::vector<L0Muon::PMuonCandidate> process(int * xfoi ,int * yfoi);

    /** Special method to emulate part of the execute method : search candidates (the tower has been contrcuted before hand)

        @param xfoiM1 :  FOI in X  in M1
        @param xfoiM2 :  FOI in X  in M2
        @param xfoiM4 :  FOI in X  in M4
        @param xfoiM5 :  FOI in X  in M5
        @param yfoiM4 :  FOI in Y  in M4
        @param yfoiM5 :  FOI in Y  in M5
    */
    std::vector<L0Muon::PMuonCandidate> process(int xfoiM1,int xfoiM2,int xfoiM4,int xfoiM5,int yfoiM4,int yfoiM5);
  
    /// Give a static type name to the unit
    std::string type() {
      return "CoreUnit";
    }
     
  private:

    Tower  m_tower;                          ///< Object where the candidate search algorithm is performed 
    std::vector<LHCb::MuonTileID> m_pads;    ///< List of fired pads
    CandRegisterHandler m_candRegHandlerOut; ///< CandRegisterHandler (pointing to the output register)  

    bool m_ignoreM1; ///< Flag to ignore M1 in the processing
  
  };

};  // namespace L0Muon

#endif      // L0MUONKERNEL_COREUNIT_H  
