#ifndef PVREFITTER_H
#define PVREFITTER_H 1

// from STL
#include <vector>
#include <string>

// Include files from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "Event/TrStoredTrack.h"

#include "Kernel/IPVReFitter.h"

/** @class PVReFitter PVReFitter.h 
 * remove (B) tracks from a primary vertex and refit
 *
 * @author  Yuehong Xie
 *
 * @date 17/08/2005
 *
*/

class PVReFitter : public GaudiTool,
                  virtual public IPVReFitter {

public:

  /// Standard Constructor
  PVReFitter( const std::string& type, 
             const std::string& name, 
             const IInterface* parent);

  /// Standard Destructor
  virtual ~PVReFitter() { }

  /// initialization
  StatusCode initialize();    

  /// Method to refit a particle
  StatusCode reFit(Vertex* );

  /// Method to refit a particle after remove tracks from a Particle
  StatusCode remove(Particle* ,Vertex* );
  
private:

  /// Private members and methods

  int m_maxIter;
  double m_maxDeltaChiSq;
  std::string m_fullextrapolatorName;
  std::string m_linearextrapolatorName;
  ITrExtrapolator*  m_pFullExtrap;
  ITrExtrapolator*  m_pLinearExtrap;

  /// Private  methods
  StatusCode fitPV(Vertex* , std::vector<TrStoredTrack*> & );
  StatusCode fitTwoTracks(TrStoredTrack* , TrStoredTrack*,
                          HepVector&, HepSymMatrix& , double& chi2);
  StatusCode  addTr(TrStoredTrack*,
                    HepVector&, HepSymMatrix&, double & );
  StatusCode  setPara(Vertex*, HepVector&, HepSymMatrix&, double & chi2 );
  double getZEstimate(TrStateP&, TrStateP &);
  StatusCode  getInitialStateP(TrStoredTrack*, TrStateP &);
  void getFinalTracks(Particle*, std::vector<TrStoredTrack*> &);
};

#endif // PVREFITTER_H

