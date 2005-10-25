
//-----------------------------------------------------------------------------
/** @file IRichConditionsOverall.h
 *
 *  Header file for tool interface : IRichConditionsOverall
 *
 *  CVS Log :-
 *  $Id: IRichConditionsOverall.h,v 1.5 2005-10-25 17:25:02 jonrob Exp $
 *
 *  @author Sajan EASO
 *  @date   2005-08-26
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_IRICHCONDITIONSOVERALL_H
#define RICHKERNEL_IRICHCONDITIONSOVERALL_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IRichConditionsOverall ( "IRichConditionsOverall", 1, 0 );

//-----------------------------------------------------------------------------------------
/** @class IRichConditionsOverall IRichConditionsOverall.h RichKernel/IRichConditionsOverall.h
 *
 *  Simple tool to simply instantiate various objects to allow Gauss to use various
 *  conditions data
 *
 *  @author Sajan EASO
 *  @date   2005-08-29
 */
//-----------------------------------------------------------------------------------------

class IRichConditionsOverall : virtual public IAlgTool
{

public:

  /// Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IRichConditionsOverall; }

};

#endif // RICHKERNEL_IRICHCONDITIONSOVERALL_H
