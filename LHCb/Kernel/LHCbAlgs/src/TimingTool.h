// $Id: TimingTool.h,v 1.3 2005-09-07 12:23:40 cattanem Exp $
#ifndef TIMINGTOOL_H 
#define TIMINGTOOL_H 1

// Include files

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/Chrono.h"
#include "GaudiKernel/IRndmGenSvc.h"

// from LHCbKernel
#include "Kernel/INormalizeTool.h"


/** @class TimingTool TimingTool.h
 *  Tool to get a timing normalisation for the Chrono service
 *  Adapted from BrunelAlgs/TimingAlg by M.Ferro-Luzzi 
 *
 *  @author Marco Cattaneo
 *  @date   2003-11-04
 */
class TimingTool : public GaudiTool, 
                   virtual public INormalizeTool {
public:
  /// Standard constructor
  TimingTool( const std::string& type, 
              const std::string& name,
              const IInterface* parent);

  virtual ~TimingTool( ) {}; ///< Destructor

  virtual StatusCode finalize();
  
  double normalize();    ///< Return normalization (in ns)
  IChronoStatSvc* chronoSvc(); ///< Returns pointer to ChronSvc
  IRndmGenSvc*    randSvc();   ///< Returns pointer to random number service

private:
  unsigned int  m_shots; ///< Number of random number shots for normalisation
  IChronoStatSvc* m_CSS; ///< Pointer to Chrono service
  IRndmGenSvc*    m_RGS; ///< Pointer to Random numbers service
};
#endif // TIMINGTOOL_H
