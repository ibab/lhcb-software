#ifndef I_HPDUKL1DISABLETOOL_H 
#define I_HPDUKL1DISABLETOOL 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/IIncidentListener.h"

// RICH
#include <Kernel/RichSmartID.h>

static const InterfaceID IID_IHpdUkL1DisableTool ( "Rich::Mon::IHpdUkL1DisableTool", 1, 0 );

/** @class IHpdUkL1DisableTool IHpdUkL1DisableTool.h IHpdUkL1DisableTool.h
 *   
 *
 *  @author Ulrich Kerzel
 *  @date   2008-06-30
 */

namespace Rich
{
  namespace Mon
  {

    class IHpdUkL1DisableTool : virtual public IAlgTool,
                                virtual public IIncidentListener   {
    public: 
      
      // Return the interface ID
      static const InterfaceID& interfaceID() { return IID_IHpdUkL1DisableTool; }
      
      virtual StatusCode initialize         ()                                 = 0;
      virtual StatusCode finalize           ()                                 = 0; 
      virtual void       SetDisableThreshold(int threshold = 10)               = 0;
      virtual void       DisableHPD         (const LHCb::RichSmartID &smartID) = 0; 
      virtual void       DisableHPD         (const Rich::DAQ::Level1LogicalID      &l1LogicalID,
                                             const Rich::DAQ::L1InputWithinIngress &l1InputIngress,
                                             const Rich::DAQ::L1IngressID          &ingressID,
                                             const Rich::DetectorType              &richDetector) =0;
      virtual void       ClearHPDList       ()                                 = 0;

                   
    protected:
      
    private:
    }; // class
  } // namespace Mon
} // namespace Rich
#endif // ICAMERATOOL_H
