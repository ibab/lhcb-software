// $Id: SiAmplifierResponse.h,v 1.1.1.1 2006-05-16 08:33:54 mneedham Exp $
#ifndef _SiAmplifierResponse_H
#define _SiAmplifierResponse_H

#include <string>
#include <vector>

#include "GaudiAlg/GaudiTool.h"
#include "Kernel/ISiAmplifierResponse.h"
#include "GaudiMath/GaudiMath.h"


/** @class SiAmplifierResponse SiAmplifierResponse.h SiAlgorithms/SiAmplifierResponse.h
 *
 *  Class for estimating Beetle response
 *
 *  @author M.Needham
 *  @date   13/3/2002
 */

class SiAmplifierResponse : public GaudiTool, virtual public ISiAmplifierResponse {

public: 

  /** Constructer */
  SiAmplifierResponse(const std::string& type, 
              const std::string& name,
              const IInterface* parent);

  /** destructer */
  virtual ~SiAmplifierResponse();

  /** initialize */
  virtual StatusCode initialize();

  /** calculate Beetle response
  * @param time in ns 
  * @return response 
  */
  virtual double response(const double time) const;
  
  /** The response is only valid for a certain capacitance, Vfs, ..
  * This method allows you to find out when the curve is valid
  * @return validity info
  */
  virtual ISiAmplifierResponse::Info validity() const; 

private:

  GaudiMath::Interpolation::Type typeFromString() const;

  std::vector<double> m_times;
  std::vector<double> m_values;

  double m_tMin;
  double m_tMax;

  double m_capacitance;
  unsigned int m_vfs;
  std::string m_type;
  std::string m_splineType;

  ISiAmplifierResponse::Info m_info;
  GaudiMath::SimpleSpline* m_responseSpline;

};

#endif // _SiAmplifierResponse_H





