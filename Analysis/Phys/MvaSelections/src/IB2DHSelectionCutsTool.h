// $Id: IB2DHSelectionCutsTool.h,v 1.1 2010/06/10 12:42:22 seaso Exp $
#ifndef IB2DHSELECTIONCUTSTOOL_H 
#define IB2DHSELECTIONCUTSTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IB2DHSelectionCutsTool ( "IB2DHSelectionCutsTool", 1, 0 );

/** @class IB2DHSelectionCutsTool IB2DHSelectionCutsTool.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2010-02-23
 */
class IB2DHSelectionCutsTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IB2DHSelectionCutsTool; }


  virtual void setAllCutFlags()=0;
  

  virtual bool ApplyPreTrainSel()=0;
  virtual bool ApplyFisherSel(int channelNum) =0;
  virtual bool ApplyAuxCuts( int aChannelNum) =0;
  virtual bool ApplyMassWindowCuts(int aChannelNum)=0;


protected:

private:

};
#endif // IB2DHSELECTIONCUTSTOOL_H
