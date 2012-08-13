#ifndef ILUMIINTEGRATOR_H 
#define ILUMIINTEGRATOR_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// forward declarations
namespace LHCb {
  class LumiFSR;
  class TimeSpanFSR;
  class LumiIntegral;
}

static const InterfaceID IID_ILumiIntegrator ( "ILumiIntegrator", 1, 0 );

/** @class ILumiIntegrator ILumiIntegrator.h
 *   
 *
 *  @author Jaap Panman
 *  @date   2009-03-16
 */
class ILumiIntegrator : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ILumiIntegrator; }

  // Integrate Lumi FSR data
  virtual StatusCode integrate( LHCb::LumiFSR& fsr ) = 0;

  // Integrate Lumi FSR data
  virtual StatusCode integrate( LHCb::LumiFSR* fsr ) = 0;
 
  // Integrate Lumi FSR data
  virtual StatusCode integrate( LHCb::LumiIntegral& fsr ) = 0;

  // Integrate Lumi FSR data
  virtual StatusCode integrate( LHCb::LumiIntegral* fsr ) = 0;
 
  // Integrate Lumi FSR data with mask sum one scalar
  virtual StatusCode integrate( LHCb::LumiIntegral& fsr, std::vector<double> v , double f=1.0) = 0;
 
  // Integrate Lumi FSR data with mask sum one scalar
  virtual StatusCode integrate( LHCb::LumiIntegral* fsr, std::vector<double> v , double f=1.0) = 0;

  // Accumulate mu from Lumi FSR data with mask 
  virtual StatusCode accumulate_mu( LHCb::LumiIntegral& fsr, LHCb::TimeSpanFSR* timeSpanFSR, int mukey, std::vector<double> v , double f=1.0) = 0;
 
  // Accumulate mu from Lumi FSR data with mask 
  virtual StatusCode accumulate_mu( LHCb::LumiIntegral* fsr, LHCb::TimeSpanFSR* timeSpanFSR, int mukey, std::vector<double> v , double f=1.0) = 0;

  // Set absolute scale
  virtual StatusCode setAbsolute(double scale, double relerror) = 0;

  // Get final result
  virtual double lumiValue() = 0;

  // Get error on result
  virtual double lumiError() = 0;

  // Retrieve the integrated Lumi FSR data
  virtual const LHCb::LumiIntegral& integral( ) const = 0;

  // Retrieve duplicates
  virtual std::vector<std::string> duplicates( ) const = 0;

  // Count the number of events for a given file
  virtual void countEvents( ) = 0;

  // Retrieve the event count for a file
  virtual unsigned long events( ) const = 0;

  // Check the event count for a file
  virtual bool checkEvents( ) const = 0;

  struct muTuple{ 
    unsigned long run;
    std::string guid;
    ulonglong time0;
    ulonglong time1;
    double deltaLumi;
    double norm;
    double mu;
  };
  // Get mu results
  virtual std::vector<muTuple> muValues( ) = 0;

protected:

private:

};
#endif // ILUMIINTEGRATOR_H
