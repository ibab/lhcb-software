// $Id: VeloProcessInfo.h,v 1.6 2008-02-27 14:43:18 jonrob Exp $
#ifndef EVENT_VELOPROCESSINFO_H 
#define EVENT_VELOPROCESSINFO_H 1

// Include files
#include <string>
#include <ostream>
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/ObjectVector.h"
#include "Tell1Kernel/VeloDecodeConf.h"
#include "Tell1Kernel/VeloDecodeCore.h"

/** @class VeloProcessInfo VeloProcessInfo.h Event/VeloProcessInfo.h
 *  
 *
 *  @author Tomasz Szumlak
 *  @date   2006-07-10
 */

namespace VeloProcessInfoLocation{
  static const std::string& Default="Raw/Velo/ProcInfo";
}

class VeloProcessInfo: public ContainedObject{
public: 
  
  /// Standard constructor
  VeloProcessInfo( ): m_dataProcessInfo ( 0 ),
                      m_simProcessInfo ( ),
                      m_procType ( 99 ),
                      m_convergenceLimit ( 0 ) 
  { }

  virtual ~VeloProcessInfo( ) { }   ///< Destructor

  bool isEnable(unsigned int proc);
  void setDataProcessInfo(unsigned int inValue);
  void setSimProcessInfo(VeloTELL1::dataVec inVec);
  void setDevProcessInfo(VeloTELL1::dataVec inVec);
  void setProcType(unsigned int inValue);
  unsigned int procType();
  void setConvLimit(unsigned int inValue);
  unsigned int convLimit();
  void setRunType(unsigned int inValue);
  unsigned int runType();
  
protected:

private:
  
  unsigned int m_dataProcessInfo;
  VeloTELL1::dataVec m_simProcessInfo;
  unsigned int m_procType;
  unsigned int m_convergenceLimit;
  unsigned int m_runType;
  VeloTELL1::dataVec m_devProcessInfo;

};
//
inline bool VeloProcessInfo::isEnable(unsigned int proc)
{
  if(m_procType==VeloTELL1::REAL_DATA){
    return ( (m_dataProcessInfo>>proc)&1 );
  }else if(m_procType==VeloTELL1::SIM_DATA){
    return ( *(m_simProcessInfo.begin()+proc) );
  }else if(m_procType==VeloTELL1::DEV_DATA){
    return ( *(m_devProcessInfo.begin()+proc) );
  }else{
    std::cout<< " ---------------------- " <<std::endl;
    std::cout<< " ==> Unknown data type! " <<std::endl;
    std::cout<< " ---------------------- " <<std::endl;
    return ( false );
  }
}
//
inline void VeloProcessInfo::setDataProcessInfo(unsigned int inValue)
{
  m_dataProcessInfo=inValue;
}
//
inline void VeloProcessInfo::setSimProcessInfo(VeloTELL1::dataVec inVec)
{
  m_simProcessInfo=inVec;
}
//  
inline void VeloProcessInfo::setDevProcessInfo(VeloTELL1::dataVec inVec)
{
  m_devProcessInfo=inVec;
}
//  
inline void VeloProcessInfo::setProcType(unsigned int inValue)
{
  m_procType=inValue;
}
//
inline unsigned int VeloProcessInfo::procType()
{
  return ( m_procType );
}
//
inline void VeloProcessInfo::setConvLimit(unsigned int inValue)
{
  m_convergenceLimit=inValue;
}
//
inline unsigned int VeloProcessInfo::convLimit()
{
  return ( m_convergenceLimit );
}
//
inline void VeloProcessInfo::setRunType(unsigned int inValue)
{
  m_runType=inValue;
}
//
inline unsigned int VeloProcessInfo::runType()
{
  return ( m_runType );
}
//
typedef ObjectVector<VeloProcessInfo> VeloProcessInfos;
#endif // EVENT_VELOPROCESSINFO_H
