// $Id: TrackNTuple.h,v 1.3 2005-03-21 15:11:38 pkoppenb Exp $
#ifndef TRACKNTUPLE_H 
#define TRACKNTUPLE_H 1
 
// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"
class IDecayFinder ;

/** @class TrackNTuple TrackNTuple.h
 *  
 *  Fills an ntuple with all particles used to make Bs passing selections.
 *
 *  @author Patrick KOPPENBURG
 *  @date   2005-02-08
 */
class TrackNTuple : public DVAlgorithm {
public: 
  /// Standard constructor
  TrackNTuple( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TrackNTuple( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  StatusCode getStables(const ParticleVector&,ParticleVector&); ///< get stables
  StatusCode fillTuple(const ParticleVector&); ///< fill tuple
  StatusCode fillTuple(const Particle*,Tuple&); ///< fill tuple
  StatusCode fillTuple(const MCParticle*,Tuple&); ///< fill tuple
  StatusCode fillStatsTuple(const int&,const int&); ///< fill tuple
  StatusCode fillHlt(Tuple&); ///< fill tuple
  StatusCode fillL1Score(Tuple&); ///< fill tuple
  StatusCode fillPV(Tuple&);
  std::vector<const MCVertex*> getMCPVs();
  
  double mcIP(const Hep3Vector&,const HepPoint3D&,const HepPoint3D&);
  double getQuark(const MCParticle*) ;
  int shortPID(int);
  int trackType(const Particle*);
  int Hlt(const std::string&); ///< fill tuple
  int bitPattern();

private:

  bool m_truth ; ///< use MC truth 
  IDecayFinder* m_decayFinder ; ///< Decay finder
  Particle2MCLinksAsct::IAsct* m_pAsct;   ///<  Composite associator
  SelResults* m_sr ; ///< selresults
  int m_pattern ; ///< pattern
  

};
#endif // TRACKNTUPLE_H
