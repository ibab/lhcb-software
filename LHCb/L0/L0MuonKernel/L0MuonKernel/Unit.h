// $Id: Unit.h,v 1.1.1.1 2004-03-10 11:38:15 ltocco Exp $

#ifndef L0MUONKERNEL_UNIT_H
#define L0MUONKERNEL_UNIT_H     1

/** @class Unit Unit L0MuonKernel/Unit.h

   Class representing a unit of data processing logic 
   of the L0Muon Trigger for hardware simulations
   
   @author  Andrei Tsaregorodtsev
   @date  12 January 2002
*/ 

#include <string>
#include <map>
#include "L0MuonKernel/Register.h"
#include "GaudiKernel/MsgStream.h"

namespace L0Muon {

class Unit {

public:
  Unit();
  virtual ~Unit();
  
  void releaseRegisters();
  void dumpRegisters(MsgStream *log);
  virtual void setParent(L0Muon::Unit * unit);
  Unit * parent(){ return m_parent;}
  
    
  virtual void addInputRegister(L0Muon::Register* in);
  virtual void addInputRegister(L0Muon::Register* in, std::string rname);
  virtual void addOutputRegister(L0Muon::Register* out);
  virtual void addOutputRegister(L0Muon::Register* out, std::string rname);
  void addUnit(L0Muon::Unit* unit);
  void addUnit(L0Muon::Unit* unit, std::string uname);
 
  //void setParent(Unit * pu){m_parent = pu ;} 

  virtual void initialize() =0;
  virtual void execute()    =0;
  virtual void finalize()   =0;

protected:

  Unit * m_parent;
  std::map<std::string,L0Muon::Register*> m_inputs;    // input registers    
  std::map<std::string,L0Muon::Register*> m_outputs;   // output registers   
  
  std::map<std::string,L0Muon::Unit*> m_units;            // subunits  
  
};

};  // namespace L0Muon

#endif      // L0MUONKERNEL_UNIT_H  
