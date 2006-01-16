#ifndef ALIGNMENTTOOLS_IMILLEPEDE_H 
#define ALIGNMENTTOOLS_IMILLEPEDE_H 1

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "MilleConfig.h"          
#include "Event/AlignTrack.h"

/** @class IMillepede IMillepede.h AlignmentTools/IMillepede.h
 * 
 *
 *  @author Sebastien Viret
 *  @date   2005-07-29
 */

//--------------------------------------------------------------
// This tool is used to perform the Alignment with Millepede
//--------------------------------------------------------------

static const InterfaceID IID_IMillepede ( "IMillepede", 1, 0 );

class IMillepede : virtual public IAlgTool {
public: 
  // Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IMillepede; }

  virtual StatusCode InitMille(MilleConfig* my_config) = 0;
  virtual StatusCode MakeGlobalFit(double par[], double error[], double pull[]) = 0;
  virtual StatusCode ParGlo(int index, double param) = 0;

  virtual StatusCode PutTrack(AlignTrack* atrack, MilleConfig* my_config, int after) = 0;
  virtual StatusCode PutPVTrack(AlignTracks* aPV, MilleConfig* my_config, int nPV) = 0;
};
#endif // ALIGNMENTTOOLS_IMILLEPEDE_H
