#ifndef L0MUONKERNEL_BUILDL0BUFFERUNIT_H
#define L0MUONKERNEL_BUILDL0BUFFERUNIT_H 

/* class L0Buffer L0Buffer L0MuonKernel/BuildL0Buffer.h
 
   author Luisanna Tocco
   date 2 September 2003

*/

#include <boost/dynamic_bitset.hpp>
#include <vector>
#include "L0MuonKernel/Unit.h"
//#include "MuonKernel/MuonTileID.h"
//#include "L0MuonKernel/L0mProcUnit.h"
#include "L0mConf/L0MPuNodeBase.h"
#include "GaudiKernel/MsgStream.h"
namespace L0Muon {
  
  class BuildL0BufferUnit : public L0Muon::Unit {

  public:
    BuildL0BufferUnit();
    virtual ~BuildL0BufferUnit();
    
    void setPU( MuonTileID pu){ m_pu = pu;}     
    void setIdField();    
    void setInputField();
    void setOutputField();  
    void setL0buf();


    boost::dynamic_bitset<> getIdField();
    boost::dynamic_bitset<> getInputField(); 
    boost::dynamic_bitset<> getOutputField();

    void initialize();
    void execute();
    void finalize();
  

  private:
    MuonTileID m_pu;
    std::map<std::string, Register*> m_addrfield;
    boost::dynamic_bitset<> m_l0buf;
    boost::dynamic_bitset<> m_idfield;
    boost::dynamic_bitset<> m_inpfield;
    boost::dynamic_bitset<> m_outfield;

  };
}; // namespace L0Muon
 
#endif    // L0MUONKERNEL_BUILDL0BUFFERUNIT_H
