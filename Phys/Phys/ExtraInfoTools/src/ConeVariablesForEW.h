#ifndef CONEVARIABLESFOREW_H 
#define CONEVARIABLESFOREW_H 1

#include "Kernel/IExtraInfoTool.h" 
#include "GaudiAlg/GaudiTool.h"

/** @class ConeVariablesForEW ConeVariablesForEW.h
 *  
 * \brief Fill track isolation for DecayTreeTuple. 
 *    Open up a cone around head, exclude all tracks 
 *    that are in the decay descriptor 
 *    (i.e. that belong to the decay you are looking for), 
 *    build the variables with the remaining tracks.
 * \sa DecayTreeTuple
 * 
 *  @author Michel De Cian
 *  @date   2009-08-04
 */


class ConeVariablesForEW : public GaudiTool, virtual public IExtraInfoTool {

public: 
  /// Standard constructor
  ConeVariablesForEW( const std::string &type, 
                      const std::string &name,
                      const IInterface *parent );

  /// Loop over differnt conesizes and fill the variables into the tuple
  virtual StatusCode calculateExtraInfo( const LHCb::Particle*
                                       , const LHCb::Particle*);

  virtual int getFirstIndex(void);

  virtual int getNumberOfParameters(void);

  virtual int getInfo(int index, double & value, std::string & name);

  virtual ~ConeVariablesForEW( ); ///< Destructor

protected:

private:

  double m_coneAngle;
  int m_coneNumber;

  int m_mult;
  double m_px;
  double m_py;
  double m_pz;
  double m_vp;
  double m_vpt;
  double m_sp;
  double m_spt;
  double m_tp;
  double m_tpt;
  double m_minpte;
  double m_maxpte;
  double m_minptmu;
  double m_maxptmu;
  int m_nmult;
  double m_npx;
  double m_npy;
  double m_npz;
  double m_nvp;
  double m_nvpt;
  double m_nsp;
  double m_nspt;

  int m_trackType;
  std::string  m_extraParticlesLocation;
  std::string  m_extraPhotonsLocation;

  std::vector<const LHCb::Particle*> m_decayParticles;

  /// Save all particles in your decay descriptor in a vector
  void saveDecayParticles( const LHCb::Particle *top );

  /// Calculate properties of your remaining tracks inside the cone
  StatusCode ChargedCone( const LHCb::Particle *seed,
			  const LHCb::Particle::Range& parts,
			  const double rcut,
			  int &mult,
			  std::vector<double> &vP,
			  double &sP,
			  double &sPt,
			  double &minPtE,
			  double &maxPtE,
			  double &minPtMu,
			  double &maxPtMu );

  StatusCode NeutralCone( const LHCb::Particle *seed,
                          const LHCb::Particle::Range& parts,
                          const double rcut,
                          int &mult,
                          std::vector<double> &vP,
                          double &sP,
                          double &sPt );

  /*
  StatusCode PVCone( const LHCb::Particle *seed,
                     const LHCb::Particles *parts,
                     const double rcut,
                     int &mult,
                     std::vector<double> &vP,
                     double &sP,
                     double &sPt,
                     double &minM,
		     double &totM,
		     double &minPt,
		     double &maxPt );
  */

  /// Check if your track belongs to your decay or not
  bool isTrackInDecay( const LHCb::Track *track );

};

#endif // CONEVARIABLESFOREW_H
