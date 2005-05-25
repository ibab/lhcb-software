#ifndef L0MUONKERNEL_L0BUFFERUNIT_H
#define L0MUONKERNEL_L0BUFFERUNIT_H 

/** @class L0BufferUnit L0BufferUnit.h  L0MuonKernel/L0BufferUnit.h
 
  class for constructing the L0Buffer 
  It is a register containing the field: 
  identification field  (32 bits)
  input field (432 bits) : optical link and neighbours 
  output field (48 bits) : address for candidates + status word  

  @author Luisanna Tocco
  @date 2 September 2003

*/
#include <stdio.h>
#include <boost/dynamic_bitset.hpp>
#include <vector>
#include "ProcessorKernel/Unit.h"
#include "L0mConf/L0MPuNodeBase.h"
#include "L0mConf/L0MTile.h"

namespace L0Muon {
  
  class L0BufferUnit : public Unit {

  public:

    /// Default Constructor
    L0BufferUnit();

    /// Constructor
    L0BufferUnit(int & type);
    /// Destructor
    ~L0BufferUnit();
    
    /// Set the MuonTileID of the PU 
    void setPU( MuonTileID pu ) { m_pu = pu; }

    /// Return x foi in station sta
    int xFoi(int sta);
  
    /// Return y foi in station sta
    int yFoi(int sta);

    /// Return x foi in station sta
    int xHardFoi(int sta);
  
    /// Return y foi in station sta
    int yHardFoi(int sta);
    
    /// Fill the bits of the output register
    void setL0buf();
    void setL0bufStd();
    void setL0bufPLL();
    void setOLPLL();

    /// Open the output file 
    void setOutputFile(MuonTileID puid, std::string suffixe);
 
    /// Write the l0buffer on the output file
    void writeEvent();
    void writeHeader();
    
    void initialize();
    void execute();

    /// Give a static type name to the unit
    std::string type() {
      return "L0BufferUnit";
    }

  


  private:

    /// PU ID
    MuonTileID m_pu;
    
    /// Type of the Unit : "standard" or PLL type.
    int m_bufferType;

    /// Pointer to the file where to write the L0Buffer
    FILE *m_l0bufferFile;
    
    /// Event counter
    unsigned long int m_l0EventNumber;

    bool m_buildL0Buffer;

  };
}; // namespace L0Muon
 
#endif    // L0MUONKERNEL_L0BUFFERUNIT_H
