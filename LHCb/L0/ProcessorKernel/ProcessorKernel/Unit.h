// $Id: Unit.h,v 1.1.1.1 2005-02-02 21:56:38 atsareg Exp $

#ifndef PROCESSORKERNEL_UNIT_H
#define PROCESSORKERNEL_UNIT_H     1

/** @class Unit Unit.h L0MuonKernel/Unit.h

   Class representing a unit of data processing logic 
   of the level-0 muon trigger for hardware simulations
   
*/ 

#include <string>
#include <map>
#include "ProcessorKernel/Register.h"

namespace L0Muon {

class Unit {

public:

  /// Default constructor
  Unit();

  /// Destructor
  virtual ~Unit();
  
  /// Reset input/output registers
  void releaseRegisters();
  
  /// Print the registers contents
  void dumpRegisters();

  /// set the pointer to the parent unit
  virtual void setParent(L0Muon::Unit * unit);

  /// return the parent unit
  Unit * parent(){ return m_parent;}
  
  /// return the parent of the given type
  Unit * parentByType(std::string type);
  
  /// Search for subunit 
  Unit * subUnit(std::string name){
    std::map<std::string,L0Muon::Unit*>::iterator iunit;
    iunit = m_units.find(name);
    return (*iunit).second;
  };
  
  
  /// Add input register  
  virtual void addInputRegister(L0Muon::Register* in);

  /** Add input register

      @param rname   : name of the input register
  */
  virtual void addInputRegister(L0Muon::Register* in, std::string rname);
 
  /// Add output register  
  virtual void addOutputRegister(L0Muon::Register* out);

  /** Add output register

      @param rname   : name of the output register
  */
  virtual void addOutputRegister(L0Muon::Register* out, std::string rname);
 
  /// Add subunit
  void addUnit(L0Muon::Unit* unit);

  /** Add sununit

      @param uname   : name of the subunit
  */
  void addUnit(L0Muon::Unit* unit, std::string uname);
 
  
  /// Virtual method to initialize the hierarchy of units
  virtual void initialize();

  /// Virtual method to execute the hierarchy of units
  virtual void execute();

  /// Virtual method to finalize the hierarchy of units
  virtual void finalize();

  /// Set the DEBUG level flag
  void setDebugMode() ;
  
  /// Get the unit type
  virtual std::string type() =0;
  
  /// Set the unit name
  void setName(std::string uname) { m_name = uname; }
  
  /// Get the unit name
  std::string name() { return m_name; }
  
  /// Print out the unit definition - to be overloaded in subclasses
  void dump(int offset=0 );
  
  /// Print out the unit definition for all the unit tree
  void dumpUnitTree(int offset=0 );

protected:

  std::string m_name;

  Unit * m_parent;
  std::map<std::string,L0Muon::Register*> m_inputs;    // input registers    
  std::map<std::string,L0Muon::Register*> m_outputs;   // output registers
  std::map<std::string,L0Muon::Unit*> m_units;         // subunits  
  
  bool m_debug;

};

};  // namespace L0Muon

#endif      // PROCESSORKERNEL_UNIT_H  
