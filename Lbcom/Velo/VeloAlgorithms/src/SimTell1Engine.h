// $Id: SimTell1Engine.h,v 1.2 2009-12-18 08:12:33 szumlat Exp $
#ifndef SIMTELL1ENGINE_H 
#define SIMTELL1ENGINE_H 1

#include <iostream>

// Include files
#include "Tell1Kernel/VeloTell1Core.h"
#include "Tell1Kernel/VeloDecodeConf.h"

/** @class SimTell1Engine SimTell1Engine.h SimTell1Engine.h
 *  
 *
 *  @author Tomasz Szumlak
 *  @date   2009-08-01
 */

class SimTell1Engine {
public: 
  /// Standard constructor
  SimTell1Engine( );
  virtual ~SimTell1Engine( ); ///< Destructor
  size_t dataSize() const { return m_dataSize; } 
  size_t linkPedestalSize() const { return m_linkPedestalSize; }
  size_t linkMaskSize() const { return m_linkMaskSize; }
  void setProcessEnable(const int inValue) { m_processEnable=inValue; }
  int processEnable() const { return m_processEnable; }
  void setInData(const VeloTELL1::EngineVec& inVec);
  const VeloTELL1::EngineVec& inData()  { return *m_inData; }
  VeloTELL1::EngineVec& outData() { return m_outData; }

protected:
  
  virtual void initTabMembers() { };
 
  template<class T>
  void tableInit(T* inData, const size_t size, int init=0)
  {
    // the tables are meant to store integers
    T* begin=inData;
    T* end=begin+size;
    std::fill(begin, end, init);
  }

private:

  int m_processEnable;
  size_t m_dataSize;
  size_t m_linkPedestalSize;
  size_t m_linkMaskSize;
  const VeloTELL1::EngineVec* m_inData;
  VeloTELL1::EngineVec m_outData;

};
#endif // SIMTELL1ENGINE_H
