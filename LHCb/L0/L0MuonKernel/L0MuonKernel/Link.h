// $Id: Link.h,v 1.4 2004-12-21 14:33:02 ltocco Exp $

#ifndef L0MUONKERNEL_LINK_H
#define L0MUONKERNEL_LINK_H     1

/** @class Unit Unit.h L0MuonKernel/Unit.h

   Class representing a unit of data processing logic 
   of the level-0 muon trigger in order to create a link between 
   the units
   
   @author  Andrei Tsaregorodtsev
   @date  12 January 2002
*/ 

#include "L0MuonKernel/Unit.h"
namespace L0Muon {

class Link : public L0Muon::Unit {

public:

  /// Default constructor
  Link();

  /** Constructor

      @param read  : register for reading data
      @param write : register for writing data

   */
  Link(L0Muon::Register* read, L0Muon::Register* write);

  /// Destructor
  ~Link();
  
  /// Add input register 
  void addInputRegister(L0Muon::Register* in) ;

  /// Add output register
  void addOutputRegister(L0Muon::Register* out) ;

  /// Transfer data from input to output register
  void transfer() { execute(); }

  /// Overload from Unit
  void initialize();
  
  /// execute the transfer of data 
  void execute();

  /// Overload from Unit
  void finalize();

};

};  // namespace L0Muon

#endif      // L0MUONKERNEL_LINK_H  
