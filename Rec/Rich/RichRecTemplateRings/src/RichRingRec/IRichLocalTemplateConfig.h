// $Id: IRichLocalTemplateConfig.h,v 1.1.1.1 2009-03-04 12:01:45 jonrob Exp $
#ifndef RICHRINGREC_IRICHLOCALTEMPLATECONFIG_H 
#define RICHRINGREC_IRICHLOCALTEMPLATECONFIG_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IRichLocalTemplateConfig ( "IRichLocalTemplateConfig", 1, 0 );

/** @class IRichLocalTemplateConfig IRichLocalTemplateConfig.h RichRingRec/IRichLocalTemplateConfig.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2007-06-14
 */
class IRichLocalTemplateConfig : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IRichLocalTemplateConfig; }

  virtual StatusCode ConfigureTemplateHitsForFF(int rad)=0;

  virtual VD RpnTemplate()=0;
  virtual VD PpnTemplate()=0;
  virtual StatusCode ScaleTemplateHits(VI aVec, int itk , int irad )=0;
  

    
protected:

private:

};
#endif // RICHRINGREC_IRICHLOCALTEMPLATECONFIG_H
