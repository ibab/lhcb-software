#include "L0MuonKernel/UnitFactory.h"
#include "L0MuonKernel/MuonTriggerUnit.h"
#include "L0MuonKernel/CrateUnit.h"
#include "L0MuonKernel/BoardUnit.h"
#include "L0MuonKernel/BCSUnit.h"
#include "L0MuonKernel/ProcUnit.h"
#include "L0MuonKernel/CoreUnit.h"
#include "L0MuonKernel/FormattingUnit.h"
#include "L0MuonKernel/L0BufferUnit.h"

L0Muon::UnitFactory* L0Muon::UnitFactory::m_instance=0;

L0Muon::UnitFactory::UnitFactory() {
}

L0Muon::UnitFactory::~UnitFactory() {}

L0Muon::UnitFactory* L0Muon::UnitFactory::instance() {
  
  if ( m_instance == 0 ) {
    m_instance = new L0Muon::UnitFactory;
  } 
  return m_instance;
}

L0Muon::Unit* L0Muon::UnitFactory::createUnit(DOMNode* pNode, std::string type) {
  
  // // //std::cout << "<UnitFactory::createUnit> has node any attribute ?"<<pNode->hasAttributes()<<std::endl;
  if (type == "MuonTriggerUnit"){
    // // //std::cout <<"<UnitFactory::createUnit> MuonTriggerUnit"<<std::endl;
    MuonTriggerUnit* pu = new MuonTriggerUnit(pNode);
    // // //std::cout <<"<UnitFactory::createUnit> MuonTriggerUnit done"<<std::endl;
    return pu;
  }
  if (type == "CrateUnit"){
    // // //std::cout <<"<UnitFactory::createUnit> CrateUnit"<<std::endl;
    CrateUnit* pu = new CrateUnit(pNode);
    // // //std::cout <<"<UnitFactory::createUnit> CrateUnit done"<<std::endl;
    return pu;
  }
  if (type == "BoardUnit"){
    // // //std::cout <<"<UnitFactory::createUnit> BoardUnit"<<std::endl;
    BoardUnit* pu = new BoardUnit(pNode);
    return pu;
  }
  if (type == "BCSUnit"){
    // // //std::cout <<"<UnitFactory::createUnit> BCSUnit"<<std::endl;
    BCSUnit* pu = new BCSUnit(pNode);
    return pu;
  }
  if (type == "ProcUnit"){
    // // //std::cout <<"<UnitFactory::createUnit> ProcUnit"<<std::endl;
    ProcUnit* pu = new ProcUnit(pNode);
    return pu;
  }
  if (type == "CoreUnit"){
    // // //std::cout <<"<UnitFactory::createUnit> CoreUnit"<<std::endl;
    CoreUnit* pu = new CoreUnit(pNode);
    return pu;
  }
  if (type == "FormattingUnit"){
    // // //std::cout <<"<UnitFactory::createUnit> FormattingUnit"<<std::endl;
    FormattingUnit* pu = new FormattingUnit(pNode);
    return pu;
  }
  if (type == "L0BufferUnit"){
    // // //std::cout <<"<UnitFactory::createUnit> L0BufferUnit"<<std::endl;
    L0BufferUnit* pu = new L0BufferUnit(pNode);
    return pu;
  }
}

