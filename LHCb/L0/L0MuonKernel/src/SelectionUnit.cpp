#include "L0MuonKernel/SelectionUnit.h"
#include "L0MuonKernel/CrateUnit.h"

L0Muon::SelectionUnit::SelectionUnit(){}

L0Muon::SelectionUnit::SelectionUnit(LHCb::MuonTileID id):L0MUnit(id){}

L0Muon::SelectionUnit::SelectionUnit(DOMNode* pNode):L0MUnit(pNode){}



L0Muon::SelectionUnit::~SelectionUnit() {}

void L0Muon::SelectionUnit::select()
{

  std::map<int, CandRegisterHandler>::iterator itHandlerMap;

  m_candRegHandlerOut.clear();

  // Extract the candidates with highest Pt;
  // Fill the output register with candidates
  std::pair<int,int> usedCand(-1,-1);
  for (int ibest=0; ibest<2; ibest++) {
    int bestPT=-1;
    std::pair<int,int> bestCand(-1,-1);
    // Loop over input registers
    for (itHandlerMap=m_candRegHandlerIn.begin();itHandlerMap!=m_candRegHandlerIn.end();itHandlerMap++){
      // Loop over input candidates in register
      for (int icand = 0;icand<2;icand++) {
        int index = (*itHandlerMap).first;
        if (m_candRegHandlerIn[index].isEmpty(icand)) continue;
        if (index==usedCand.first && icand==usedCand.second) continue;
        int pt = m_candRegHandlerIn[index].getCandPT(icand);
        if (pt>bestPT ) {
          bestPT = pt;
          std::pair<int,int> tmppair(index,icand);
          bestCand = tmppair;
        }
      } // End of Loop over input candidates in register
    } // End of Loop over input registers
    if (bestCand.first==-1 || bestCand.second==-1) break;
    PMuonCandidate bestCandidate = m_candRegHandlerIn[bestCand.first].getMuonCandidate(bestCand.second);
    m_candRegHandlerOut.setMuonCandidate(bestCandidate,ibest);
    usedCand = bestCand;
  }
  
  
  // Set the output status
  int statusORed=0;
  for (itHandlerMap=m_candRegHandlerIn.begin();itHandlerMap!=m_candRegHandlerIn.end();itHandlerMap++){
    int index = (*itHandlerMap).first;
    int status = m_candRegHandlerIn[index].getCandStatus();
    statusORed = status ==3 ? status:statusORed;
  }
  m_candRegHandlerOut.setCandStatus(statusORed);

}


void L0Muon::SelectionUnit::postexecute()
{
  releaseOutputRegisters();
  L0Muon::Unit::postexecute(); 
}

