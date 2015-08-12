// $Id: $
#ifndef IMUONINFORMATION_H 
#define IMUONINFORMATION_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "RichRecBase/RichRecTupleAlgBase.h"

#include "RichKernel/IRichParticleProperties.h"
#include "GaudiAlg/GaudiTool.h"



static const InterfaceID IID_IMuonInformation ( "IMuonInformation", 1, 0 );

/** @class IMuonInformation IMuonInformation.h
 *  
 *
 *  @author Matt Coombes
 *  @date   2010-10-04
 */
class IMuonInformation : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IMuonInformation; }

  virtual int HasMuonInformation(const LHCb::Track*)=0;
  
  virtual StatusCode initialize() =0;


protected:

private:

};
#endif // IMUONINFORMATION_H
