// $Id: STProcessInfo.h,v 1.1 2008-01-30 14:22:30 akeune Exp $
#ifndef EVENT_STPROCESSINFO_H 
#define EVENT_STPROCESSINFO_H 1

// Include files
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/ObjectVector.h"
#include "Kernel/STDecodeConf.h"

/** @class STProcessInfo STProcessInfo.h Event/STProcessInfo.h
 *  
 *
 *  @author Tomasz Szumlak, Anne Keune
 *  @date   2006-07-10
 */

using namespace STTELL1;

namespace STProcessInfoLocation{
  static const std::string& ITProcInfo="Raw/IT/ProcInfo";
  static const std::string& TTProcInfo="Raw/TT/ProcInfo";
}

class STProcessInfo: public ContainedObject{
public: 

  typedef std::vector<unsigned int> dataVec;
  
  /** Standard constructor **/
  STProcessInfo( ): m_dataProcessInfo ( 0 ),
                      m_vecProcessInfo ( ),
                      m_procType ( 99 ),
                      m_convergenceLimit ( 0 ) 
  { }

  /** Destructor **/
  virtual ~STProcessInfo( ) { }

  bool isEnable(unsigned int proc);
  void setDataProcessInfo(unsigned int inValue);
  void setVecProcessInfo(dataVec inVec);
  void setProcType(unsigned int inValue);
  unsigned int procType();
  void setConvLimit(unsigned int inValue);
  unsigned int convLimit();
  void setRunType(unsigned int inValue);
  unsigned int runType();
  void setDetType(std::string inString);
  std::string detType();
  void setConfigType(unsigned int inValue);
  unsigned int configType();
  
protected:

private:

  unsigned int m_dataProcessInfo;
  dataVec m_vecProcessInfo;
  unsigned int m_procType;
  unsigned int m_convergenceLimit;
  unsigned int m_runType;
  std::string m_detType;
  unsigned int m_configType;
  
};

typedef ObjectVector<STProcessInfo> STProcessInfos;
//
inline bool STProcessInfo::isEnable(unsigned int proc)
{
  if(m_vecProcessInfo.size()) return ( *(m_vecProcessInfo.begin()+proc) );
  else {
    if(m_dataProcessInfo==0)
      std::cout << "STProcessInfo: No process info available or all processes disabled" << std::endl;
    return ( (m_dataProcessInfo>>proc)&1 );
  }
  
}
//
inline void STProcessInfo::setDataProcessInfo(unsigned int inValue)
{
  m_dataProcessInfo=inValue;
}
//
inline void STProcessInfo::setVecProcessInfo(dataVec inVec)
{
  m_vecProcessInfo=inVec;
}
//  
inline void STProcessInfo::setProcType(unsigned int inValue)
{
  m_procType=inValue;
}
//
inline unsigned int STProcessInfo::procType()
{
  return ( m_procType );
}
//
inline void STProcessInfo::setConvLimit(unsigned int inValue)
{
  m_convergenceLimit=inValue;
}
//
inline unsigned int STProcessInfo::convLimit()
{
  return ( m_convergenceLimit );
}
//
inline void STProcessInfo::setRunType(unsigned int inValue)
{
  m_runType=inValue;
}
//
inline unsigned int STProcessInfo::runType()
{
  return ( m_runType );
}
//
inline void STProcessInfo::setDetType(std::string inString)
{
  m_detType=inString;
}
//
inline std::string STProcessInfo::detType()
{
  return ( m_detType );
}
//
inline void STProcessInfo::setConfigType(unsigned int inValue)
{
  m_configType=inValue;
}
//
inline unsigned int STProcessInfo::configType()
{
  return ( m_configType );
}
//

#endif // EVENT_STPROCESSINFO_H
