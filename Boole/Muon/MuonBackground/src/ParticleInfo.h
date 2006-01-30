// $Id: ParticleInfo.h,v 1.3 2006-01-30 11:04:47 asatta Exp $
#ifndef PARTICLEINFO_H 
#define PARTICLEINFO_H 1

// Forward declarations
#include "Event/MCParticle.h"

/** @class ParticleInfo ParticleInfo.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2003-02-19
 */
class ParticleInfo {
public:
  /// Standard constructor
  ParticleInfo(const LHCb::MCParticle* particle, int station, int gaps ); 
  ParticleInfo(const LHCb::MCParticle* particle);
  
  virtual ~ParticleInfo( ); ///< Destructor
  void setHitIn(int station, int gap,int chamber);
  std::vector<int> multiplicityResults();
  int getCollision();
  
protected:

private:
  int m_pileupEventNumber;
  static int m_stationNumber;
  static int m_gapsNumber;
  static  int maxDimension;  
  
  std::vector<std::vector<int> >* m_storagePointer;
  std::vector<int>* m_resultPointer;
};
#endif // PARTICLEINFO_H
