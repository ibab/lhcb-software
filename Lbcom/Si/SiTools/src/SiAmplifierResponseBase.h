// $Id: SiAmplifierResponseBase.h,v 1.2 2009-03-19 08:19:42 mneedham Exp $
#ifndef SiAmplifierResponseBase_H
#define SiAmplifierResponseBase_H 1

// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiMath/GaudiMath.h"

// Interface from LHCbKernel
#include "Kernel/ISiAmplifierResponse.h"

/** @class SiAmplifierResponseBase SiAmplifierResponseBase.h
 *
 *  Base Class for estimating Beetle response
 *
 *  @author M.Needham
 *  @date   9/2/2009
 */

class SiAmplifierResponseBase : public extends<GaudiTool, ISiAmplifierResponse> {

public:

  /** Constructer */
  SiAmplifierResponseBase(const std::string& type,
              const std::string& name,
              const IInterface* parent);

  /** Initialize */
  StatusCode initialize() override;

  /** calculate Beetle response
  * @param time in nanoseconds
  * @return response
  */
  double response(const double time) const override;

  /** The response is only valid for a certain capacitance, Vfs, etc.
  * This method allows you to find out when the curve is valid
  * @return validity info
  */
  ISiAmplifierResponse::Info validity() const override;

  /** calculate the remainder ie signal left after 25 ns
  * @param time time of sampling on peak [default is zero ]
  * @return response
  */
  double remainder(double time = 0) const override;

  /** calculate the rise time [10 - 90 %]
  * @return ristime
  */
  double risetime() const override;

protected:

  /// sample the spline function
  void sample(std::vector<double>& times, std::vector<double>& val) const;

   /// Internal method to convert the spline type from string to GaudiMath type
  GaudiMath::Interpolation::Type typeFromString() const;

  /// internal print method
  void printToScreen() const;

  /// print a TGraph for root
  void printForRoot() const;

  /// helper to print array
  void printArray(const std::vector<double>& values,
		  const std::string& name) const;

  std::unique_ptr<GaudiMath::SimpleSpline> m_responseSpline; ///< The fitted spline

  double m_tMin; ///< First entry in vector of times
  double m_tMax; ///< Last entry in vector of times

  // job options
  std::vector<double> m_times;  ///< List of times in ns
  std::vector<double> m_values; ///< Corresponding values
  std::string m_splineType;     ///< Spline type (default is "Cspline")
  std::string m_type;           ///< Info object: Type of data
  unsigned int m_vfs;           ///< Info object: Signal shaping parameter
  double m_capacitance;         ///< Info object: Capacitance
  bool m_printToScreen;
  bool m_printForRoot;
  double m_printDt;

private:

  double findValue(double value) const;
  ISiAmplifierResponse::Info m_info;         ///< Holds information on validity

};

#endif // SiAmplifierResponseBase_H
