#ifndef L0MUONKERNEL_BUILDL0BUFFERUNIT_H
#define L0MUONKERNEL_BUILDL0BUFFERUNIT_H 

/** @class L0Buffer L0Buffer.h  L0MuonKernel/BuildL0Buffer.h
 
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
#include "L0MuonKernel/Unit.h"
#include "L0mConf/L0MPuNodeBase.h"
#include "GaudiKernel/MsgStream.h"

namespace L0Muon {
  
  class BuildL0BufferUnit : public L0Muon::Unit {

  public:

    /// Constructor
    BuildL0BufferUnit();

    /// Destructor
    ~BuildL0BufferUnit();
    
    /// Set the MuonTileID of the parent PU
    void setPU( MuonTileID pu){ m_pu = pu;}

    /// Set the identification field
    void setIdField();    

    /// Set the Input field
    void setInputField();

    /// Set the output field
    void setOutputField();  

    /// group identification, input and output field
    void setL0buf();

    
    /// Return the identification field 
    boost::dynamic_bitset<> getIdField();

    /// Return the input field 
    boost::dynamic_bitset<> getInputField();

    /// Return the output field 
    boost::dynamic_bitset<> getOutputField();

    /// Open the output file 
    void setOutputFile(MuonTileID puid);
 
    /// Write the output file
    void dump(FILE *l0bufferFile);

    
    void initialize();
    void initialize(MsgStream & log);

    void execute();
    
    /** Call methods to built and write L0Buffer

        @param log   : MSG::DEBUG
    */
    void execute(MsgStream & log);

    void finalize();


  


  private:

    MuonTileID m_pu;
    std::map<std::string, Register*> m_addrfield;
    boost::dynamic_bitset<> m_l0buf;
    boost::dynamic_bitset<> m_idfield;
    boost::dynamic_bitset<> m_inpfield;
    boost::dynamic_bitset<> m_outfield;


    FILE *m_l0bufferFile;
    unsigned long int m_l0EventNumber;

  };
}; // namespace L0Muon
 
#endif    // L0MUONKERNEL_BUILDL0BUFFERUNIT_H
