// $Id: IRichConditionsOverall.h,v 1.3 2005-10-07 16:06:57 seaso Exp $
#ifndef RICHCONDITION_IRICHCONDITIONSOVERALL_H 
#define RICHCONDITION_IRICHCONDITIONSOVERALL_H 1

// Include files
// from Gaudi

#include "GaudiKernel/IAlgTool.h"
// Local
#include "IRich1PresTempMonitor.h"
#include "IRich2PresTempMonitor.h"

static const InterfaceID IID_IRichConditionsOverall ( "IRichConditionsOverall", 1, 0 );

/** @class IRichConditionsOverall IRichConditionsOverall.h RichCondition/IRichConditionsOverall.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2005-08-29
 */
class IRichConditionsOverall : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IRichConditionsOverall; }

  virtual IRich1PresTempMonitor* getRich1PresTempMonitor() = 0;
  virtual IRich2PresTempMonitor* getRich2PresTempMonitor() = 0;
  
 
protected:

private:

};
#endif // RICHCONDITION_IRICHCONDITIONSOVERALL_H
