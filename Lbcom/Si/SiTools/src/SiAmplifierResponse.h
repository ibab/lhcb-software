// $Id: SiAmplifierResponse.h,v 1.4 2009-01-15 10:02:19 mneedham Exp $
#ifndef SiAmplifierResponse_H
#define SiAmplifierResponse_H 1

// Gaudi
#include "SiAmplifierResponseBase.h"
#include "GaudiMath/GaudiMath.h"


/** @class SiAmplifierResponse SiAmplifierResponse.h
 *
 *  Class for estimating Beetle response
 *
 *  @author M.Needham
 *  @date   13/3/2002
 */

class SiAmplifierResponse : public SiAmplifierResponseBase {

public:

  /** Constructer */
  SiAmplifierResponse(const std::string& type,
              const std::string& name,
              const IInterface* parent);

  /** initialize */
  StatusCode initialize() override;


};

#endif // SiAmplifierResponse_H
