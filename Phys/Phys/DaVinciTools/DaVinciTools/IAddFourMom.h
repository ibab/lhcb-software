// $Id: IAddFourMom.h,v 1.1 2002-02-08 18:05:36 gcorti Exp $
#ifndef DAVINCITOOLS_IADDFOURMOM_H 
#define DAVINCITOOLS_IADDFOURMOM_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
class AxPartCandidate;
class IParticlePropertySvc;

static const InterfaceID IID_IAddFourMom("IAddFourMom", 1 , 0); 

/** @class IAddFourMom IAddFourMom.h DaVinciTools/IAddFourMom.h
 *  Add four momentum interface.
 *
 *  @author Gloria Corti
 *  @date   05/02/2002
 */

class IAddFourMom : virtual public IAlgTool {
 public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IAddFourMom; }
  /// Calculate
  virtual StatusCode calculate( const std::vector<AxPartCandidate*>,
                        HepLorentzVector&, double charge) = 0; 
};

#endif // DAVINCITOOLS_IADDFOURMOM_H
