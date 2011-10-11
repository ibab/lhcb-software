// $Id: ICloneSwimmingReport.h,v 1.1 2009-07-29 21:28:36 jpalac Exp $
#ifndef MICRODST_ICLONESWIMMINGREPORT_H 
#define MICRODST_ICLONESWIMMINGREPORT_H 1

// from MicroDST
#include "MicroDST/ICloner.h"

// Forward declarations
namespace LHCb 
{
  class SwimmingReport;
}

/** @class ICloneSwimmingReport ICloneSwimmingReport.h MicroDST/ICloneSwimmingReport.h
 *  
 *
 *  @author Roel Aaij
 *  @date   2011-10-09
 */
class GAUDI_API ICloneSwimmingReport : virtual public MicroDST::ICloner<LHCb::SwimmingReport> {
public: 

  DeclareInterfaceID(ICloneSwimmingReport, 1, 0 );

};
#endif // MICRODST_ICLONESWIMMINGREPORT_H
