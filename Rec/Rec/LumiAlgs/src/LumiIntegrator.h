// $Id: LumiIntegrator.h,v 1.1 2009-06-12 08:17:50 panmanj Exp $
#ifndef LUMIINTEGRATOR_H 
#define LUMIINTEGRATOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// event model
#include "Event/LumiFSR.h"
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

  // Retrieve the integrated Lumi FSR data
  const LHCb::LumiIntegral& integral( ) const;

  // Count the number of events for a given file
  void countEvents( );

  // Retrieve the event count for a file
  unsigned long events( ) const;

  // Check the event count for a file
  bool checkEvents( ) const;

protected:

private:
  std::string m_ToolName;                       // name of tool for normalization

  unsigned long m_count_files;                  // number of files
  unsigned long m_count_events;                 // number of events

  LHCb::LumiIntegral m_LumiSum;                 // overall sum of FSRs
};
#endif // LUMIINTEGRATOR_H
