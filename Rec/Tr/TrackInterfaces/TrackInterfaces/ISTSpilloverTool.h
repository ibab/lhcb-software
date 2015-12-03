#ifndef TRACKINTERFACES_ISTSPILLOVERTOOL_H 
#define TRACKINTERFACES_ISTSPILLOVERTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
namespace LHCb 
{
  class FitNode;
}

static const InterfaceID IID_ISTSpilloverTool ( "ISTSpilloverTool", 1, 0 );

/** @class ISTSpilloverTool ISTSpilloverTool.h TrackInterfaces/ISTSpilloverTool.h
 *  
 *
 *  @author Vincenzo Battista
 *  @date   2015-08-13
 */
class ISTSpilloverTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ISTSpilloverTool; }

  /// Return PDF(ADC,costheta|central)/PDF(ADC|spillover) starting from a track node 
  virtual double pdfRatio( const LHCb::FitNode* fNode ) = 0;
  

protected:

private:

};
#endif // TRACKINTERFACES_ISTSPILLOVERTOOL_H
