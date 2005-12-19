// $Id: STAmplifierResponse.h,v 1.1.1.1 2005-12-19 15:43:15 mneedham Exp $
#ifndef _STAmplifierResponse_H
#define _STAmplifierResponse_H

#include <string>
#include <vector>
// $Id: STAmplifierResponse.h,v 1.1.1.1 2005-12-19 15:43:15 mneedham Exp $

#include "GaudiAlg/GaudiTool.h"
#include "STAlgorithms/ISTAmplifierResponse.h"

class GSLSpline;

/** @class STAmplifierResponse STAmplifierResponse.h STAlgorithms/STAmplifierResponse.h
 *
 *  Class for estimating noise in IT Channel
 *
 *  @author M.Needham
 *  @date   13/3/2002
 */

class STAmplifierResponse : public GaudiTool, virtual public ISTAmplifierResponse {

public: 

  /// Constructer
  STAmplifierResponse(const std::string& type, 
              const std::string& name,
              const IInterface* parent);

  /// destructer
  virtual ~STAmplifierResponse();

  /// init
  virtual StatusCode initialize();

  /// calc response
  virtual double response(const double time) const;

  /// validity
  virtual ISTAmplifierResponse::Info validity() const; 

private:

  std::vector<double> m_times;
  std::vector<double> m_values;

  double m_tMin;
  double m_tMax;

  double m_capacitance;
  unsigned int m_vfs;
  std::string m_type;
  ISTAmplifierResponse::Info m_info;

  GSLSpline* m_responseSpline;

};

#endif // _STAmplifierResponse_H





