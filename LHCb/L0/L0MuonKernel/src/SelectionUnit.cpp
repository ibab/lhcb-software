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

  int ncandidates = 0;

  // Extract the candidates with highest Pt;
  // Fill the output register with candidates
  std::pair<int,int> usedPair(-1,-1);
  for (int ibest=0; ibest<2; ibest++) {
    PMuonCandidate best(new MuonCandidate());
    std::pair<int,int> bestPair(-1,-1);
    // Loop over input registers
    for (itHandlerMap=m_candRegHandlerIn.begin();itHandlerMap!=m_candRegHandlerIn.end();itHandlerMap++){
      int index = (*itHandlerMap).first;
      // Loop over input candidates in register
      for (int icand = 0;icand<2;icand++) {
        if (m_candRegHandlerIn[index].isEmpty(icand)) continue;
        if (index==usedPair.first && icand==usedPair.second) continue; // candidate has already been used
        PMuonCandidate cand = m_candRegHandlerIn[index].getMuonCandidate(icand);
        if (cand->pT()>best->pT() ) {
          best = cand;
          std::pair<int,int> tmppair(index,icand);
          bestPair = tmppair;
        }
      } // End of Loop over input candidates in register (icand)
    } // End of Loop over input registers (index)
    if (bestPair.first==-1 || bestPair.second==-1) break;
    m_candRegHandlerOut.setMuonCandidate(best,ibest);
    usedPair = bestPair;
    ++ncandidates;
  }
  
  
  // Set the output status
  int status_ncand=0;
  int status_error=0;
  for (itHandlerMap=m_candRegHandlerIn.begin();itHandlerMap!=m_candRegHandlerIn.end();itHandlerMap++){
    int index = (*itHandlerMap).first;
    int ncand = (m_candRegHandlerIn[index].getStatus())&0x3;
    int error = ((m_candRegHandlerIn[index].getStatus())>>2)&0x3;
    status_ncand = (ncand==3) ? 3 : status_ncand;
    status_error|=error;
  }
  status_ncand = (status_ncand==3) ? 3 : ncandidates;
  int status=((status_ncand)&0x3) + ((status_error<<2)&0xC);
  m_candRegHandlerOut.setStatus(status);
  
//   int statusORed=0;
//   for (itHandlerMap=m_candRegHandlerIn.begin();itHandlerMap!=m_candRegHandlerIn.end();itHandlerMap++){
//     int index = (*itHandlerMap).first;
//     int status = m_candRegHandlerIn[index].getStatus();
//     statusORed = status ==3 ? status:statusORed;
//   }
//   m_candRegHandlerOut.setStatus(statusORed);

}


void L0Muon::SelectionUnit::postexecute()
{
  releaseOutputRegisters();
  L0Muon::Unit::postexecute(); 
}

