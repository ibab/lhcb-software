// $Id: RichInfo.h,v 1.1 2003-04-29 15:04:44 seaso Exp $
#ifndef MISC_RICHINFO_H 
#define MISC_RICHINFO_H 1

// Include files

/** @class RichInfo RichInfo.h Misc/RichInfo.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2003-04-16
 */
#include "RichPhotInfo.h"
#include "RichPEInfo.h"

class RichInfo {
public:
  /// Standard constructor 

  RichInfo();

  RichInfo(RichPhotInfo* );
  
  RichInfo(RichPEInfo* );
  
  virtual ~RichInfo( ); ///< Destructor

  RichPhotInfo* RichPhotInformation() 
  {
    return m_RichPhotInformation ;
  }
  void setRichPhotInformation( RichPhotInfo* aRichPhotInformation)
  {
    m_RichPhotInformation= aRichPhotInformation;
    
  }
  
  RichPEInfo* RichPEInformation() 
  {
    return m_RichPEInformation;
  }
  void setRichPEInformation( RichPEInfo* aRichPEInformation)
  {
    m_RichPEInformation= aRichPEInformation;
  }
  bool   HasUserPhotInfo()
  {
    return  m_HasUserPhotInfo;
  }

  void setUserPhotInfo() 
  {
    m_HasUserPhotInfo=true;
    
  }
  void unsetUserPhotInfo() 
  {
    m_HasUserPhotInfo=false;
    
  }
  
  bool HasUserPEInfo()
  {
    return m_HasUserPEInfo;
  }
  void setUserPEInfo() 
  {
    m_HasUserPEInfo=true;
    
  }
  void unsetUserPEInfo() 
  {
    m_HasUserPEInfo=false;
    
  }
  
protected:

private:

RichPhotInfo* m_RichPhotInformation;
  
RichPEInfo* m_RichPEInformation;

bool  m_HasUserPhotInfo;
bool  m_HasUserPEInfo;
  

};
#endif // MISC_RICHINFO_H
