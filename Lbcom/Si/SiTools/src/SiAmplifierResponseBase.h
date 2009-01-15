// $Id: SiAmplifierResponseBase.h,v 1.1 2009-01-15 10:02:19 mneedham Exp $
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

class SiAmplifierResponseBase : public GaudiTool, 
                            virtual public ISiAmplifierResponse {

public: 

  /** Constructer */
  SiAmplifierResponseBase(const std::string& type, 
              const std::string& name,
              const IInterface* parent);

  /** destructer */
  virtual ~SiAmplifierResponseBase();

  /** Initialize */
  virtual StatusCode initialize();

  /** calculate Beetle response
  * @param time in nanoseconds
  * @return response 
  */
  virtual double response(const double time) const;
  
  /** The response is only valid for a certain capacitance, Vfs, etc.
  * This method allows you to find out when the curve is valid
  * @return validity info
  */
  virtual ISiAmplifierResponse::Info validity() const; 

private:

  ISiAmplifierResponse::Info m_info;         ///< Holds information on validity

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
  bool m_printToScreen;
  bool m_printForRoot;
  double m_printDt;

};

#endif // SiAmplifierResponseBase_H
