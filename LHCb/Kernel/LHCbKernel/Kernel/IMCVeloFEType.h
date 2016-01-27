#ifndef VELOALGORITHMS_IMCVELOFETYPE_H
#define VELOALGORITHMS_IMCVELOFETYPE_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IMCVeloFEType ( "IMCVeloFEType", 1, 0 );

/** @class IMCVeloFEType IMCVeloFEType.h VeloAlgorithms/IMCVeloFEType.h
 *
 *
 *  @author Tomasz Szumlak
 *  @date   2005-11-16
 */

namespace LHCb
{
  class MCVeloFE;
}

class IMCVeloFEType : virtual public IAlgTool 
{

public:

    enum Types{ Signal=0,
                Noise,
                Other };
  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IMCVeloFEType; }

  virtual void FEType(const LHCb::MCVeloFE* fe, int& feType)=0;

};
#endif // VELOALGORITHMS_IMCVELOFETYPE_H
