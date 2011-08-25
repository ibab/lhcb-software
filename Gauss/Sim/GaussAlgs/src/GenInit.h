// $Id: GenInit.h,v 1.3 2009-03-26 21:32:40 robbep Exp $
#ifndef GENINIT_H 
#define GENINIT_H 1

// Include files
// from LHCbKernel
#include "Kernel/LbAppInit.h"

// from Event
#include "Event/BeamParameters.h"

class IGenericTool;

/** @class GenInit GenInit.h
 *  
 *  First TopAlg for Generator phase of Gauss.
 *  Initializes random number and fills memory histogram.
 *  It also creates and fill the GenHeader.
 * 
 *  @author Gloria CORTI
 *  @date   2006-01-16
 */
class GenInit : public LbAppInit {
public: 
  /// Standard constructor
  GenInit( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~GenInit( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  /// Property to set the first event number, if event header is missing
  int           m_firstEvent;

  /// Property to set the run number, if event header is missing
  int           m_runNumber;

  IGenericTool* m_memoryTool;   ///< Pointer to (private) memory histogram tool
  
  std::string   m_mcHeader ; ///< Location of MCHeader
  std::string   m_beamParameters ; ///< Location of beam parameters

  double        m_beamEnergy ;
  double        m_bunchLengthRMS ;
  double        m_normalizedEmittance ;
  double        m_totalCrossSection ;
  double        m_horizontalCrossingAngle ;
  double        m_verticalCrossingAngle ;
  double        m_horizontalBeamlineAngle ;
  double        m_verticalBeamlineAngle ;
  double        m_betaStar ;
  double        m_bunchSpacing ;
  double        m_xLuminousRegion , m_yLuminousRegion , m_zLuminousRegion ;
  double        m_luminosity ;

  bool          m_createBeam;

  LHCb::BeamParameters m_beam ; ///< Local beam parameter object
};
#endif // GENINIT_H
