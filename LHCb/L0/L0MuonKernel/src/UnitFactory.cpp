#include "L0MuonKernel/UnitFactory.h"
#include "L0MuonKernel/MuonTriggerUnit.h"
#include "L0MuonKernel/CtrlUnit.h"
#include "L0MuonKernel/CrateUnit.h"
#include "L0MuonKernel/BoardUnit.h"
#include "L0MuonKernel/BCSUnit.h"
#include "L0MuonKernel/ProcUnit.h"
#include "L0MuonKernel/CoreUnit.h"
#include "L0MuonKernel/FormattingUnit.h"
#include "L0MuonKernel/L0BufferProcUnit.h"
#include "L0MuonKernel/L0BufferBCSUnit.h"
#include "L0MuonKernel/L0BufferSlaveUnit.h"
#include "L0MuonKernel/L0BufferCtrlUnit.h"
#include "L0MuonKernel/Tell1CtrlUnit.h"
#include "L0MuonKernel/Tell1ProcUnit.h"
#include "L0MuonKernel/RawBufferCtrlUnit.h"
#include "L0MuonKernel/RawBufferProcUnit.h"

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
  } else if (type == "CtrlUnit"){
    // // //std::cout <<"<UnitFactory::createUnit> CtrlUnit"<<std::endl;
    CtrlUnit* pu = new CtrlUnit(pNode);
    return pu;
  } else if (type == "CrateUnit"){
    // // //std::cout <<"<UnitFactory::createUnit> CrateUnit"<<std::endl;
    CrateUnit* pu = new CrateUnit(pNode);
    // // //std::cout <<"<UnitFactory::createUnit> CrateUnit done"<<std::endl;
    return pu;
  } else if (type == "BoardUnit"){
    // // //std::cout <<"<UnitFactory::createUnit> BoardUnit"<<std::endl;
    BoardUnit* pu = new BoardUnit(pNode);
    return pu;
  } else if (type == "BCSUnit"){
    // // //std::cout <<"<UnitFactory::createUnit> BCSUnit"<<std::endl;
    BCSUnit* pu = new BCSUnit(pNode);
    return pu;
  } else if (type == "ProcUnit"){
    // // //std::cout <<"<UnitFactory::createUnit> ProcUnit"<<std::endl;
    ProcUnit* pu = new ProcUnit(pNode);
    return pu;
  } else if (type == "CoreUnit"){
    // // //std::cout <<"<UnitFactory::createUnit> CoreUnit"<<std::endl;
    CoreUnit* pu = new CoreUnit(pNode);
    return pu;
  } else if (type == "FormattingUnit"){
    // // //std::cout <<"<UnitFactory::createUnit> FormattingUnit"<<std::endl;
    FormattingUnit* pu = new FormattingUnit(pNode);
    return pu;
  } else if (type == "L0BufferProcUnit"){
    // // //std::cout <<"<UnitFactory::createUnit> L0BufferUnit"<<std::endl;
    L0BufferProcUnit* pu = new L0BufferProcUnit(pNode);
    return pu;
  } else if (type == "L0BufferBCSUnit"){
    // // //std::cout <<"<UnitFactory::createUnit> L0BufferUnit"<<std::endl;
    L0BufferBCSUnit* pu = new L0BufferBCSUnit(pNode);
    return pu;
  } else if (type == "L0BufferSlaveUnit"){
    // // //std::cout <<"<UnitFactory::createUnit> L0BufferUnit"<<std::endl;
    L0BufferSlaveUnit* pu = new L0BufferSlaveUnit(pNode);
    return pu;
  } else if (type == "L0BufferCtrlUnit"){
    // // //std::cout <<"<UnitFactory::createUnit> L0BufferUnit"<<std::endl;
    L0BufferCtrlUnit* pu = new L0BufferCtrlUnit(pNode);
    return pu;
  } else if (type == "Tell1CtrlUnit"){
    // // //std::cout <<"<UnitFactory::createUnit> t"<<std::endl;
    Tell1CtrlUnit* pu = new Tell1CtrlUnit(pNode);
    return pu;
  } else if (type == "Tell1ProcUnit"){
    // // //std::cout <<"<UnitFactory::createUnit> Tell1Unit"<<std::endl;
    Tell1ProcUnit* pu = new Tell1ProcUnit(pNode);
    return pu;
  } else if (type == "RawBufferCtrlUnit"){
    // // //std::cout <<"<UnitFactory::createUnit> RawBufferCtrlUnit"<<std::endl;
    RawBufferCtrlUnit* pu = new RawBufferCtrlUnit(pNode);
    return pu;
  } else if (type == "RawBufferProcUnit"){
    // // //std::cout <<"<UnitFactory::createUnit> RawBufferProcUnit"<<std::endl;
    RawBufferProcUnit* pu = new RawBufferProcUnit(pNode);
    return pu;
  } else {
    std::cout <<"<L0Muon::UnitFactory::createUnit> ERROR: UNKNOWN UNIT TYPE "<<type<<"\n";
    return 0;
  }
  
}

