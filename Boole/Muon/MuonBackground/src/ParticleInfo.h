// $Id: ParticleInfo.h,v 1.2 2005-04-14 13:10:41 cattanem Exp $
#ifndef PARTICLEINFO_H 
#define PARTICLEINFO_H 1

// Forward declarations
class MCParticle;

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
  
  std::vector<std::vector<int> >* m_storagePointer;
  std::vector<int>* m_resultPointer;
};
#endif // PARTICLEINFO_H
