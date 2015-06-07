#ifndef BOOLE_V26R9_SIPMRESPONSE_H 
#define BOOLE_V26R9_SIPMRESPONSE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiMath/GaudiMath.h"

static const InterfaceID IID_SiPMResponse ( "SiPMResponse", 1, 0 );

/** @class SiPMResponse SiPMResponse.h Boole_v26r9/SiPMResponse.h
 *  
 *  This class describes the SiPM response to a single pulse
 *
 *  @author Maurizio Martinelli
 *  @date   2013-11-12
 */
class SiPMResponse : public GaudiTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_SiPMResponse; }

  /// Standard constructor
  SiPMResponse( const std::string& type, 
                const std::string& name,
                const IInterface* parent);
  

  virtual ~SiPMResponse( ); ///< Destructor
  
  virtual StatusCode initialize();
  
  virtual double response(const double time) const;
  

protected:

  GaudiMath::Interpolation::Type typeFromString() const;
  void sample(std::vector<double>& times, std::vector<double>& val) const;

private:

  GaudiMath::SimpleSpline* m_responseSpline; 
  // 
  double m_tMin;
  double m_tMax;
  // Job options
  double m_samplingDt;
  std::vector<double> m_times;  
  std::vector<double> m_values; 
  std::string m_splineType;
  
  int m_useNewResponse;
  

};
#endif // BOOLE_V26R9_SIPMRESPONSE_H
