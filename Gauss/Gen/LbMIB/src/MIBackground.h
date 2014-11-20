// $Id: MIBackground.h,v 1.1 2007-08-17 12:54:14 gcorti Exp $
#ifndef MIBACKGROUND_H
#define MIBACKGROUND_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "IMIBSource.h"

/** @class MIBackground MIBackground.h 
 *  
 *  Algorithm to read file containing particles reaching LHCb cavern
 *  and due to halo impinging on the Tertiary Collimators.
 *  The source can be re-weighted according to the options selected
 *  and produces particles from it.
 *
 *  @author Magnus Lieng
 *  @date   2006-10-10
 */
class MIBackground : public GaudiAlgorithm {
public:
  
  /// Standard constructor
  MIBackground(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~MIBackground(); ///< Destructor
 
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  /// Generate an "event" and put information in containers  
  StatusCode generate( LHCb::GenHeader* theHeader,
                       LHCb::GenCollisions* theCollisions,
                       LHCb::HepMCEvents* theEvents );

private:

  std::string m_eventLoc;             ///< TES location of HepMCEvent
  std::string m_headerLoc;            ///< TES location of GenHeader
  std::string m_collLoc;              ///< TES location of Collisions

  double m_luminosity;                ///< Luminosity to store in TES
  int m_evtType;                      ///< Event type 

  std::vector<std::string> m_sourcesNames;  ///< type/name of (tools) sources 
  std::vector<IMIBSource*> m_sources;       ///< pointers to (tools) sources

};   

#endif  // MIBACKGROUND_H

