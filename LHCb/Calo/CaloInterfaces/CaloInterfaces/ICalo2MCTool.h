// $Id: ICalo2MCTool.h,v 1.2 2009-09-21 10:17:34 odescham Exp $
#ifndef ICALO2MCTOOL_H 
#define ICALO2MCTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace LHCb{
  class CaloCluster;
  class CaloDigit;
  class CaloHypo;
  class ProtoParticle;
  class Particle;
  class MCParticle;
}



static const InterfaceID IID_ICalo2MCTool ( "ICalo2MCTool", 2, 0 );

/** @class ICalo2MCTool ICalo2MCTool.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2009-07-27
 */
class ICalo2MCTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ICalo2MCTool; }

  // setters
  virtual ICalo2MCTool* from(const LHCb::CaloDigit*     digit    )=0;
  virtual ICalo2MCTool* from(const LHCb::CaloCluster*   cluster  )=0;
  virtual ICalo2MCTool* from(const LHCb::CaloHypo*      hypo     )=0;
  virtual ICalo2MCTool* from(const LHCb::ProtoParticle* proto    )=0;
  virtual ICalo2MCTool* from(const LHCb::Particle*      particle )=0;
  virtual StatusCode  _setProperty(const std::string& p,const std::string& v)=0;
  // getters
  virtual const LHCb::MCParticle* bestMC() = 0;
  virtual const LHCb::MCParticle* maxMC() = 0;
  virtual const LHCb::MCParticle* findMC(std::string name, double threshold = 0 ) = 0;
  virtual double weight(const LHCb::MCParticle*)  = 0;
  virtual double quality(const LHCb::MCParticle*) = 0;
  virtual std::string descriptor() = 0;

protected:

private:

};
#endif // ICALO2MCTOOL_H
