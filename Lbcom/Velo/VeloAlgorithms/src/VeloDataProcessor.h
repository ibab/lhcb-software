// $Id: VeloDataProcessor.h,v 1.9 2008-04-16 15:55:13 dhcroft Exp $
#ifndef VELODATAPROC_H 
#define VELODATAPROC_H 1

// Include files
// from STL
#include <string>

// include for tanh
#include <cmath>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// local

/** @class VeloDataProcessor VeloDataProcessor.h
 *
 * Emulate Data Processor Board 
 *
 *  @author Chris Parkes, update Tomasz Szumlak
 *  @date   14/01/02
 */

class VeloDataProcessor : public GaudiAlgorithm {
public:
  /// Standard constructor
  VeloDataProcessor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~VeloDataProcessor( ); ///< Destructor

  virtual StatusCode initialize(); ///< Algorithm initialize
  virtual StatusCode execute();    ///< Algorithm execution

protected:

private:

  /// tanh(y(q)-P*y(q)^2), where y(q) = (slope/scale)*(q-center) 
  inline double tanhFunc(double q){
    double y = (m_NL_Slope/m_NL_Scale)*(q-m_NL_Center);
    return tanh( y + m_NL_P_Term*y*y );
  }

  /// convert electrons to ADC counts using linear function
  int digitiseLinear(double electrons);
  /// convert electrons to ADC counts using non-linear function
  int digitiseNonLinear(double electrons);
  // data members
  std::string m_inputContainer;      ///< Name of input container
  std::string m_outputVeloDigit; ///< Name of output container for digi
  double m_ADCFullScale;         ///< number of ADC channels at full scale
  bool m_nonLinear;              ///< Use nonlinear e- to ADC scaling
  double m_electronsFullScale;   ///< linear factor e to ADC at full scale
  int m_maxADCOutput;            ///< Set the maximum possible ADC value
  int m_minADCOutput;            ///< Set the minimum possible ADC value
  // non-linear function is ADC = scale × (tanh(y(q)) - tanh(y(0)))
  //    where y(q) = (slope/scale)*(q-center)
  double m_NL_Scale; ///< Scale factor in non linear: units ADC counts
  double m_NL_Slope; ///< Slope factor in non linear: units ADC/e
  double m_NL_Center;///< Center factor in non linear: units e
  double m_NL_P_Term;///< Second order term in y(q) in non-linear : no unit 
  double m_tanh0;    ///< value of tanhFunc(0)

};
#endif // VELODATAPROC_H
