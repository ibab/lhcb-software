// $Id: SiAmplifierResponse.h,v 1.2 2007-01-09 14:57:21 jvantilb Exp $
#ifndef SiAmplifierResponse_H
#define SiAmplifierResponse_H 1

// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiMath/GaudiMath.h"

// Interface from LHCbKernel
#include "Kernel/ISiAmplifierResponse.h"

/** @class SiAmplifierResponse SiAmplifierResponse.h
 *
 *  Class for estimating Beetle response
 *
 *  @author M.Needham
 *  @date   13/3/2002
 */

class SiAmplifierResponse : public GaudiTool, 
                            virtual public ISiAmplifierResponse {

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
  * @param time in nanoseconds
  * @return response 
  */
  virtual double response(const double time) const;
  
  /** The response is only valid for a certain capacitance, Vfs, ..
  * This method allows you to find out when the curve is valid
  * @return validity info
  */
  virtual ISiAmplifierResponse::Info validity() const; 

private:

  /// Internal method to convert the spline type from string to GaudiMath type
  GaudiMath::Interpolation::Type typeFromString() const;

  ISiAmplifierResponse::Info m_info;         ///< Holds information on validity
  GaudiMath::SimpleSpline* m_responseSpline; ///< The fitted spline

  double m_tMin; ///< First entry in vector of times
  double m_tMax; ///< Last entry in vector of times

  // job options
  std::vector<double> m_times;  ///< List of times in ns
  std::vector<double> m_values; ///< Corresponding values
  std::string m_splineType;     ///< Spline type (default is "Cspline")
  std::string m_type;           ///< Info object: Type of data
  unsigned int m_vfs;           ///< Info object: Signal shaping parameter
  double m_capacitance;         ///< Info object: Capacitance

};

#endif // SiAmplifierResponse_H
