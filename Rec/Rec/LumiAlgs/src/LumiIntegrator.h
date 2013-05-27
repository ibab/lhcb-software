// $Id: LumiIntegrator.h,v 1.1 2009-06-12 08:17:50 panmanj Exp $
#ifndef LUMIINTEGRATOR_H 
#define LUMIINTEGRATOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// event model
#include "Event/LumiFSR.h"
#include "Event/TimeSpanFSR.h"
#include "Event/LumiIntegral.h"

#include "ILumiIntegrator.h"            // Interface


/** @class LumiIntegrator LumiIntegrator.h
 *   
 *
 *  @author Jaap Panman
 *  @date   2009-03-16
 */
class LumiIntegrator : public GaudiTool, virtual public ILumiIntegrator {
public: 
  /// Standard constructor
  LumiIntegrator( const std::string& type, 
                  const std::string& name,
                  const IInterface* parent);

  virtual ~LumiIntegrator( ); ///< Destructor

  // Integrate Lumi FSR data
  StatusCode integrate( LHCb::LumiFSR& fsr );

  // Integrate Lumi FSR data
  StatusCode integrate( LHCb::LumiFSR* fsr );

  // Integrate Lumi FSR data
  StatusCode integrate( LHCb::LumiIntegral& fsr );

  // Integrate Lumi FSR data
  StatusCode integrate( LHCb::LumiIntegral* fsr );

  // Integrate Lumi FSR data with mask sum one scalar
  StatusCode integrate( LHCb::LumiIntegral& fsr, std::vector<double> v, double f=1.0 );

  // Integrate Lumi FSR data with mask sum one scalar
  StatusCode integrate( LHCb::LumiIntegral* fsr, std::vector<double> v, double f=1.0 );

  // Accumulate mu from Lumi FSR data with mask 
  StatusCode accumulate_mu( LHCb::LumiIntegral& fsr, LHCb::TimeSpanFSR* timeSpanFSR, int mukey, std::vector<double> v, double f=1.0 );

  // Accumulate mu from Lumi FSR data with mask 
  StatusCode accumulate_mu( LHCb::LumiIntegral* fsr, LHCb::TimeSpanFSR* timeSpanFSR, int mukey, std::vector<double> v, double f=1.0 );

  // Set absolute scale
  StatusCode setAbsolute(double scale, double relerror);

  // Get final result
  double lumiValue();

  // Get error on result
  double lumiError();

  // Retrieve the integrated Lumi FSR data
  const LHCb::LumiIntegral& integral( ) const;

  // Retrieve duplicates
  std::vector<std::string> duplicates( ) const;

  // Count the number of events for a given file
  void countEvents( );

  // Retrieve the event count for a file
  unsigned long events( ) const;

  // Check the event count for a file
  bool checkEvents( ) const;
  
  // Get mu results
  std::vector<ILumiIntegrator::muTuple> muValues( );

 public:
  
 protected:
  
 private:
  std::string m_ToolName;                       // name of tool for normalization

  unsigned long m_count_events;                 // number of events
  std::vector<std::string> m_duplicateFiles;    // keep track of duplicates

  double m_integral;                            // the sum
  double m_lumi_scale;                          // absolute scale 
  double m_lumi_rel_error;                      // absolute scale error

  LHCb::LumiIntegral m_LumiSum;                 // overall sum of FSRs

  std::vector<ILumiIntegrator::muTuple> m_muTuple; // result of mu calculation
};
#endif // LUMIINTEGRATOR_H
