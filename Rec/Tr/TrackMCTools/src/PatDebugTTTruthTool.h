// $Id: PatDebugTTTruthTool.h,v 1.4 2008-12-04 09:05:07 cattanem Exp $
#ifndef PATDEBUGTTTRUTHTOOL_H 
#define PATDEBUGTTTRUTHTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleTool.h"
#include "PatKernel/IPatDebugTTTool.h"            // Interface

class DeSTDetector;

/** @class PatDebugTTTruthTool PatDebugTTTruthTool.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2007-10-22
 */
class PatDebugTTTruthTool : public GaudiTupleTool, virtual public IPatDebugTTTool {
public: 
  /// Standard constructor
  PatDebugTTTruthTool( const std::string& type, 
                       const std::string& name,
                       const IInterface* parent);

  virtual ~PatDebugTTTruthTool( ); ///< Destructor

  virtual StatusCode initialize(); /// initialize

  virtual void debugTTClusterOnTrack( const LHCb::Track* track, 
                                      const PatTTHits::const_iterator beginCoord,
                                      const PatTTHits::const_iterator endCoord   );
  
  virtual void debugTTCluster( MsgStream& msg, const PatTTHit* hit );

  virtual bool isTrueHit( const LHCb::Track* track, const PatTTHit* hit);

  virtual double fracGoodHits( const LHCb::Track* track, const PatTTHits& hits);
  
  virtual bool isTrueTrack( const LHCb::Track* track, const PatTTHits& hits);
  
  virtual void chi2Tuple( const double p, const double chi2, const unsigned int nHits);
  


protected:

private:

  DeSTDetector* m_tracker;

};
#endif // PATDEBUGTTTRUTHTOOL_H
