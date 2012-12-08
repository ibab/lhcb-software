// $Id: $
#ifndef SAVEPARTICLESFROM_H 
#define SAVEPARTICLESFROM_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DaVinciAlgorithm.h"

//To associate a particle to the true one.
#include "Kernel/Particle2MCLinker.h"

/** @class SaveParticlesFrom SaveParticlesFrom.h
 *  
 *  @Version 2r0
 *  @author Neal Gauvin
 *  @date  6 august 2008
 */

class SaveParticlesFrom : public DaVinciAlgorithm
{

public: 

  /// Standard constructor
  SaveParticlesFrom( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~SaveParticlesFrom( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  Particle2MCLinker* m_pLinker;

  double VertDistance( const Gaudi::XYZPoint &, const Gaudi::XYZPoint &);
  bool   IsaPrey( const LHCb::MCParticle *);
  bool   IsItFromaPrey( const LHCb::MCParticle *, const Gaudi::XYZPoint &);

  std::vector<std::string> m_Preys ;  //Names of the preys to save
  std::vector<int>         m_PreyIDs ;//IDs of the preys to save
  LHCb::MCParticle::ConstVector m_daughters ; //All stable prey daughters
  int  m_nEvents ;         //Number of events
  bool m_Dgts ;            //Save also daughters of preys
  bool m_Charged ;         //retrieve only charged Particles or not
  bool m_Vtx ;             //Save only Particles that come from the prey's vtx

};

#endif // SAVEPARTICLESFROM_H
