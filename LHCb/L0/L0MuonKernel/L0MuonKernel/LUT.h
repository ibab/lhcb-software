#ifndef PROCESSORKERNEL_LUT_H
#define PROCESSORKERNEL_LUT_H 

/* class LUT LUT L0MuonKernel/LUT.h

 L0MuonKernel/LUT.h
 author Luisanna Tocco 

*/

#include "ProcessorKernel/Unit.h"
#include "ProcessorKernel/RegisterFactory.h"
#include "ProcessorKernel/Register.h"
#include "MuonKernel/MuonTileID.h"
#include <vector>
//#include "GaudiKernel/MsgStream.h"
namespace L0Muon {
  
  class LUT : public L0Muon::Unit {

  public:
  
    LUT();
    ~LUT();
    

    void setPU(MuonTileID pu){ m_pu = pu;}     
    void calculateAddr(int ncand);
    boost::dynamic_bitset<> calculatePt(signed long int Pt);
    //void getPt();

    void initialize();
    void execute();
    void finalize();

    private:
    
    MuonTileID m_pu;
    
    
    
  };
}; // namespace L0Muon
 
#endif    // PROCESSORKERNEL_LUT_H
