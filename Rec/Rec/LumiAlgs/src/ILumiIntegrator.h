// $Id: ILumiIntegrator.h,v 1.1 2009-06-12 08:17:50 panmanj Exp $
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
  class LumiIntegral;
};

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

  // Retrieve the integrated Lumi FSR data
  virtual const LHCb::LumiIntegral& integral( ) const = 0;

  // Count the number of events for a given file
  virtual void countEvents( ) = 0;

  // Retrieve the event count for a file
  virtual unsigned long events( ) const = 0;

  // Check the event count for a file
  virtual bool checkEvents( ) const = 0;

protected:

private:

};
#endif // ILUMIINTEGRATOR_H
