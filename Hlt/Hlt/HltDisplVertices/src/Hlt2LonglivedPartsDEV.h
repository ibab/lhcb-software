// $Id: Hlt2LonglivedPartsDEV.h,v 1.1 2010-01-28 15:20:11 ngueissa Exp $
#ifndef HLT2LONGLIVEDPARTSDEV_H 
#define HLT2LONGLIVEDPARTSDEV_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"


/** @class Hlt2LonglivedPartsDEV Hlt2LonglivedPartsDEV.h
 *  
 * THIS IS A DEVELOPMENT VERSION
 * please refer to Hlt2LonglivedParts
 *
 *  @author Neal Gauvin
 *  @date   2010-01-22
 */
class Hlt2LonglivedPartsDEV : public DVAlgorithm {
public: 
  /// Standard constructor
  Hlt2LonglivedPartsDEV( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~Hlt2LonglivedPartsDEV( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  double RadDist( const Gaudi::XYZPoint & );

  int m_nbevent;
  int m_nbpassed;

  unsigned int m_MinNbParts;///< Min number of candidates passing the cuts
  int m_MinNbtrks;    ///< Min number of daughters tracks in a RecVertex
  double m_RMin;      ///< Min radial displacement 
  double m_MinRecpt;  ///< Min measured pT
  double m_MinMass;   ///< Min measured mass
  double m_MinSumpt;  ///< Min sum of all daughters track
  double m_MinX;      ///< Min X position     
  double m_MaxX;      ///< Max X position     
  double m_MinY;      ///< Min Y position     
  double m_MaxY;      ///< Max Y position     
  double m_MinZ;      ///< Min Z position     
  double m_MaxZ;      ///< Max Z position     

  LHCb::Particle * m_BeamLine; ///< dummy Particle to store the Beam Line 

};
#endif // HLT2LONGLIVEDPARTSDEV_H
