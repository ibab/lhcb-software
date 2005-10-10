// $Id: IRichConditionsOverall.h,v 1.4 2005-10-10 08:11:11 jonrob Exp $
#ifndef RICHCONDITION_IRICHCONDITIONSOVERALL_H
#define RICHCONDITION_IRICHCONDITIONSOVERALL_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IRichConditionsOverall ( "IRichConditionsOverall", 1, 0 );

/** @class IRichConditionsOverall IRichConditionsOverall.h RichKernel/IRichConditionsOverall.h
 *
 *  Simple tool to simply instantiate various objects to allow Gauss to use various
 *  conditions data
 *
 *  @author Sajan EASO
 *  @date   2005-08-29
 */

class IRichConditionsOverall : virtual public IAlgTool
{

public:

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IRichConditionsOverall; }

};

#endif // RICHCONDITION_IRICHCONDITIONSOVERALL_H
