// $Id: L0BufferCnvContainer.cpp,v 1.1 2007-08-27 09:32:25 jucogan Exp $
// Include files 

#include "L0MuonKernel/PUL0BufferCnv.h"
#include "L0MuonKernel/BCSUL0BufferCnv.h"
#include "L0MuonKernel/CUL0BufferCnv.h"
#include "L0MuonKernel/SUL0BufferCnv.h"
#include "L0MuonKernel/OLCnv.h"

// local
#include "L0MuonKernel/L0BufferCnvContainer.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0BufferCnvContainer
//
// 2007-08-22 : Julien Cogan
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0Muon::L0BufferCnvContainer::L0BufferCnvContainer() {

  for (int iq=0; iq<4; ++iq) {

    std::vector<LHCb::MuonTileID> lboards = MuonLayout(1,1).tiles(iq);
    std::vector<LHCb::MuonTileID>::iterator itboard;

    for (itboard=lboards.begin(); itboard<lboards.end(); ++itboard){
      std::vector<LHCb::MuonTileID> lpus = MuonLayout(2,2).tiles(*itboard);
      std::vector<LHCb::MuonTileID>::iterator ipu;

      for ( ipu = lpus.begin(); ipu != lpus.end(); ++ipu ) {
        L0Muon::PUL0BufferCnv * pcnvpu = new L0Muon::PUL0BufferCnv(*ipu);
        m_cnvs.push_back(pcnvpu);
        L0Muon::OLCnv * pcnvol = new L0Muon::OLCnv(*ipu);
        m_cnvs.push_back(pcnvol);
      }

      L0Muon::BCSUL0BufferCnv * pcnvbcsu = new L0Muon::BCSUL0BufferCnv(*itboard);
      m_cnvs.push_back(pcnvbcsu);

    }

    L0Muon::SUL0BufferCnv * pcnvsu = new L0Muon::SUL0BufferCnv(LHCb::MuonTileID(iq));
    m_cnvs.push_back(pcnvsu);
    L0Muon::CUL0BufferCnv * pcnvcu = new L0Muon::CUL0BufferCnv(LHCb::MuonTileID(iq));
    m_cnvs.push_back(pcnvcu);
    
  }
  
}

L0Muon::L0BufferCnvContainer::L0BufferCnvContainer(std::vector<LHCb::MuonTileID> lpus) {
  std::map<LHCb::MuonTileID , bool> boards;
  std::map<int , bool> quarters;
  
  std::vector<LHCb::MuonTileID>::iterator ipu;
  for ( ipu = lpus.begin(); ipu != lpus.end(); ++ipu ) {

    L0Muon::PUL0BufferCnv * pcnvpu = new L0Muon::PUL0BufferCnv(*ipu);
    m_cnvs.push_back(pcnvpu);

    L0Muon::OLCnv * pcnvol = new L0Muon::OLCnv(*ipu);
    m_cnvs.push_back(pcnvol);

    LHCb::MuonTileID board = ipu->containerID(MuonLayout(1,1));
    boards[board] = true;
    
    int quarter = ipu->quarter();
    quarters[quarter] = true;
  }

  std::map<LHCb::MuonTileID , bool>::iterator  iboard;
  for (iboard=boards.begin(); iboard!=boards.end(); ++iboard){
    L0Muon::BCSUL0BufferCnv * pcnvbcsu = new L0Muon::BCSUL0BufferCnv(iboard->first);
    m_cnvs.push_back(pcnvbcsu);
  }

  std::map<int , bool>::iterator  iquarter;
  for (iquarter=quarters.begin(); iquarter!=quarters.end(); ++iquarter){
    L0Muon::SUL0BufferCnv * pcnvsu = new L0Muon::SUL0BufferCnv(iquarter->first);
    m_cnvs.push_back(pcnvsu);
    L0Muon::CUL0BufferCnv * pcnvcu = new L0Muon::CUL0BufferCnv(iquarter->first);
    m_cnvs.push_back(pcnvcu);
  }
  
}

//=============================================================================
// Destructor
//=============================================================================
L0Muon::L0BufferCnvContainer::~L0BufferCnvContainer() {} 

//=============================================================================

void L0Muon::L0BufferCnvContainer::open(std::string path) {
  std::vector<L0Muon::L0BufferCnv *>::iterator itcnv;
  for (itcnv=m_cnvs.begin(); itcnv<m_cnvs.end(); ++itcnv){
    (*itcnv)->open(path);
  }
} 

void L0Muon::L0BufferCnvContainer::close() {
  std::vector<L0Muon::L0BufferCnv *>::iterator itcnv;
  for (itcnv=m_cnvs.begin(); itcnv<m_cnvs.end(); ++itcnv){
    (*itcnv)->close();
  }
}
 
void L0Muon::L0BufferCnvContainer::write(int ievt) {
  std::vector<L0Muon::L0BufferCnv *>::iterator itcnv;
  for (itcnv=m_cnvs.begin(); itcnv<m_cnvs.end(); ++itcnv){
    (*itcnv)->write(ievt);
  }
} 
