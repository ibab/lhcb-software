// $Id: ParticleInfo.h,v 1.1.1.1 2003-04-04 09:10:14 asatta Exp $
#ifndef PARTICLEINFO_H 
#define PARTICLEINFO_H 1

// Include files
#include "Event/Collision.h" 
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
  ParticleInfo(MCParticle* particle, int station, int gaps ); 
  ParticleInfo(MCParticle* particle);
  
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
  // const int pippo=10;
  
  std::vector<std::vector<int> >* m_storagePointer;
  std::vector<int>* m_resultPointer;
};
#endif // PARTICLEINFO_H
