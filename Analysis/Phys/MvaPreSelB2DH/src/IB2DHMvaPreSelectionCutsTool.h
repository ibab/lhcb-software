// $Id: $
#ifndef IB2DHMVAPRESELECTIONCUTSTOOL_H 
#define IB2DHMVAPRESELECTIONCUTSTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IB2DHMvaPreSelectionCutsTool ( "IB2DHMvaPreSelectionCutsTool", 1, 0 );

/** @class IB2DHMvaPreSelectionCutsTool IB2DHMvaPreSelectionCutsTool.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2010-07-16
 */
class IB2DHMvaPreSelectionCutsTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IB2DHMvaPreSelectionCutsTool; }
  virtual void setAllCutFlags()=0;
  

  virtual bool ApplyPreTrainSel()=0;
  virtual bool ApplyFisherSel(int channelNum) =0;
  virtual bool ApplyAuxCuts( int aChannelNum) =0;
  virtual bool ApplyMassWindowCuts(int aChannelNum)=0;


protected:

private:

};
#endif // IB2DHMVAPRESELECTIONCUTSTOOL_H
