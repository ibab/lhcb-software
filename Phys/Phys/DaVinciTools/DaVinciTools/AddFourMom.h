// $Id: AddFourMom.h,v 1.1.1.1 2001-07-09 09:28:42 gcorti Exp $
#ifndef DAVINCITOOLS_ADDFOURMOMOPERATOR_H
#define DAVINCITOOLS_ADDFOURMOMOPERATOR_H 1

// Include files
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "CLHEP/Vector/LorentzVector.h"
#include <vector>
#include <string>


// Forward declarations
class AxPartCandidate;

//--------------------------------------------------------------------
//
//  ClassName  : AddFourMom
// 
//  Description: Calculate impact parameter of a "track" with respect
//               to a "vertex": take AxPartCandidate and MyAxVertex as
//               input arguments.
//               
//
//  Author     : G.Corti  
//
//--------------------------------------------------------------------

class AddFourMom : public AlgTool {

public:

  // Standard Constructor
  AddFourMom( const std::string& type, const std::string& name, 
              const IInterface* parent);

  // Standard Destructor
  virtual ~AddFourMom() { }

  // Actual operator function 
  StatusCode calculate( const std::vector<AxPartCandidate*> plist,
                        HepLorentzVector& p4, double charge ); 
  

private:
  
  // The Particle Property Service is set internally at creation 
  IParticlePropertySvc* m_ppSvc;
  
};

#endif // DAVINCITOOLS_ADDFOURMOMOPERATOR_H
