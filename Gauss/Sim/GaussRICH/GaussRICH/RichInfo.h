// $Id: RichInfo.h,v 1.2 2003-07-28 10:27:11 witoldp Exp $
#ifndef GaussRICH_RICHINFO_H 
#define GaussRICH_RICHINFO_H 1

// Include files

/** @class RichInfo RichInfo.h Misc/RichInfo.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2003-04-16
 */
#include "RichPhotInfo.h"
#include "RichPEInfo.h"
#include "GaussTools/DetTrackInfo.h"

class RichInfo : public DetTrackInfo{
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
#endif // GaussRICH_RICHINFO_H
