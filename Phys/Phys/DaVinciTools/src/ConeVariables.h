#ifndef CONEVARIABLES_H 
#define CONEVARIABLES_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
//#include "Kernel/DVAlgorithm.h"
#include "Kernel/IConeVariables.h" 
#include "GaudiAlg/GaudiTool.h"

/** @class ConeVariables ConeVariables.h
 *  
 * \brief Calculate track isolation. 
 *    Open up a cone around head, exclude all tracks 
 *    that are in the decay descriptor 
 *    (i.e. that belong to the decay you are looking for), 
 *    build the variables with the remaining tracks.
 * 
 * - head_cmult : Number of tracks inside cone.
 * - head_cp : Summed p inside cone
 * - head_cpt : Summed pt inside cone
 * - head_cpx : Summed px inside cone
 * - head_cpy : Summed py inside cone
 * - head_cpz : Summed pz inside cone
 *
 * If Verbose, or other flags are set:
 *
 * Asymmetry variables
 * - head_pasy : (head_P - head_cp)/(head_P + head_cp)
 * - head_ptasy : (head_PT - head_cpt)/(head_PT + head_cpt)
 * - head_pxasy : (head_Px - head_cpx)/(head_Px + head_cpx)
 * - head_pyasy : (head_Py - head_cpy)/(head_Py + head_cpy)
 * - head_pzasy : (head_Pz - head_cpz)/(head_Pz + head_cpz)
 * Delta angle variables
 * - head_DeltaEta : Difference in eta between summed tracks and head
 * - head_DeltaPhi : Difference in phi between summed tracks and head
 *
 *
 * Options:
 *
 * - ConeAngle: Set the maximum deltaR of the cone (default = 1.0), in radians
 * - TrackType: Set the type of tracks which are considered inside the cone (default = 3)
 * 
 *  Converted from TupleTool by A. Poluektov 19/02/2012
 * 
 *  @author Michel De Cian
 *  @date   2009-08-04
 *  
 */

class ConeVariables : public GaudiTool, virtual public IConeVariables {
public: 
  /// Standard constructor
  ConeVariables( const std::string& type, 
              const std::string& name,
              const IInterface* parent );

  /// Loop over differnt conesizes and fill the variables into the tuple
  virtual StatusCode getConeInfo( const LHCb::Particle*
                           , const LHCb::Particle*
                           , LHCb::ConeInfo& );
  
  

  virtual ~ConeVariables( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization

protected:

private:

  double m_coneAngle;
  int m_trackType;

  std::vector<const LHCb::Particle*> m_decayParticles;

  /// Save all particles in your decay descriptor in a vector
  void saveDecayParticles( const LHCb::Particle *top);

  /// Calculate properties of your remaining tracks inside the cone
  std::pair< std::vector<double>, int> ConeP(const LHCb::Particle *part, const LHCb::Tracks* tracks, const double rcut);

  /// Check if your track belongs to your decay or not
  bool isTrackInDecay(const LHCb::Track* track);
};



#endif // TUPLETOOLTRACKISOLATION_H
