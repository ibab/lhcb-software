// $Id: AddFourMom.h,v 1.1 2002-02-08 18:25:41 gcorti Exp $
#ifndef ADDFOURMOM_H
#define ADDFOURMOM_H 1

// Include files
// from STL
#include <vector>
#include <string>

// from Gaudi
#include "GaudiKernel/AlgTool.h"
#include "CLHEP/Vector/LorentzVector.h"

// from DaVinciTools
#include "DaVinciTools/IAddFourMom.h"

// Forward declarations
class IParticlePropertySvc;

/** @class AddFourMom AddFourMom.h 
 *  Add four momentum interface. 
 *
 * @author G. Corti
*/

class AddFourMom : public AlgTool,
                   virtual public IAddFourMom {

public:

  /// Standard Constructor
  AddFourMom( const std::string& type, const std::string& name, 
              const IInterface* parent);

  /// Standard Destructor
  virtual ~AddFourMom() { }

  /// Actual operator function 
  StatusCode calculate( const std::vector<AxPartCandidate*> plist,
                        HepLorentzVector& p4, double charge ); 
  

private:
  
  // The Particle Property Service is set internally at creation 
  IParticlePropertySvc* m_ppSvc;
  
};

#endif // ADDFOURMOM_H
