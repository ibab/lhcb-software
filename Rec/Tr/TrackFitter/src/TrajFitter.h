// $Id: TrajFitter.h,v 1.1 2007-04-17 15:22:13 graven Exp $
#ifndef TRACKFITTER_TRAJFITTER_H 
#define TRACKFITTER_TRAJFITTER_H 1

// Include files
// -------------
// from Gaudi
class StatusCode;
class ITrajPoca;
class ITrackProjectorSelector;
#include "GaudiAlg/GaudiTool.h"
#include "TrackInterfaces/ITrajFitter.h"
#include "Event/Measurement.h"
#include <vector>

/** @class TrajFitter TrajFitter.h
 *  
 *
 *  @author Gerhard Raven
 */


class TrajFitter : public GaudiTool,
                   virtual public ITrajFitter {

public: 
  /// Standard constructor
  TrajFitter( const std::string& type,
              const std::string& name,
              const IInterface* parent );
  StatusCode initialize() ;
  /// Destructor
  virtual ~TrajFitter( );


  /// the reason d'etre...
  virtual StatusCode Nfit(ITrajFitter::NDifTraj traj, 
                          ITrajFitter::NResiduals resids,
                          const LHCb::Measurement::Vector& measurements) const ;

private:
  template <typename TRAJ,typename RESIDS> StatusCode fit(TRAJ* traj, 
                                                          RESIDS* resids,
                                                          const LHCb::Measurement::Vector& measurements) const;

  /*const*/ ITrajPoca*           m_poca;
  const ITrackProjectorSelector* m_proj;

  unsigned    m_maxIter;    // maximum number of iterations allowed to minimize the chisq
  double      m_deltaChiSq; // iterate until the chisq improves by less than this
  
};

#endif // TRACKFITTER_TRAJFITTER_H
