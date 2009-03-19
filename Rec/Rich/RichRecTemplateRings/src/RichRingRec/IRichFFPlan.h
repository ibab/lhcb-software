// $Id: IRichFFPlan.h,v 1.2 2009-03-19 17:14:50 seaso Exp $
#ifndef RICHRINGREC_IRICHFFPLAN_H 
#define RICHRINGREC_IRICHFFPLAN_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "RichRingRec/RichDataArrayDefinitions.h"

static const InterfaceID IID_IRichFFPlan ( "IRichFFPlan", 1, 0 );

/** @class IRichFFPlan IRichFFPlan.h RichRingRec/IRichFFPlan.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2007-06-15
 */
class IRichFFPlan : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IRichFFPlan; }

  virtual  void  InitFFPlan()=0;
  virtual  void  ReleaseFFTWArrays()=0;
  virtual   VD ConvertToFF2d( VD A, VD B)=0;
  virtual   VVD ConvertToInvFF2d(VD F )=0;
  


protected:

private:

};
#endif // RICHRINGREC_IRICHFFPLAN_H
