// $Id: IImpactPar.h,v 1.1 2002-02-08 18:05:37 gcorti Exp $
#ifndef DAVINCITOOLS_IIMPACTPAR_H 
#define DAVINCITOOLS_IIMPACTPAR_H 1

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
class AxPartCandidate;
class VtxCandidate;

static const InterfaceID IID_IImpactPar("IImpactPar", 1 , 0); 

/** @class IImpactPar IImpactPar.h DaVinciTools/IImpactPar.h
 *  Impact parameter Algorithm Tool interface.
 *  Calculate impact parameter of a "track" with respect
 *  to a "vertex": take AxPartCandidate and MyAxVertex as
 *  input arguments.  
 *
 *  @author Gloria Corti
 *  @date   05/02/2002
 */
class IImpactPar : virtual public IAlgTool {
 public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IImpactPar; }
  /// Calulate Impact parameter
  virtual double calculate( AxPartCandidate*, VtxCandidate* ) = 0;
};
#endif // DAVINCITOOLS_IIMPACTPAR_H
