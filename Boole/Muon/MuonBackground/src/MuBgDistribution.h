// $Id: MuBgDistribution.h,v 1.1.1.1 2003-04-04 09:10:14 asatta Exp $
#ifndef MUBGDISTRIBUTION_H 
#define MUBGDISTRIBUTION_H 1

// Include files
#include "GaudiKernel/RndmGenerators.h" 
/** @class MuBgDistribution MuBgDistribution.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2003-03-03
 */
class MuBgDistribution {
public:
  /// Standard constructor
  MuBgDistribution(std::vector<Rndm::Numbers*>  pointerToDistributions, 
                  std::vector<bool> m_pointerToFlags,
                   float xmin, float xmax ); 

  MuBgDistribution(std::vector<Rndm::Numbers*>  
                   pointerToDistributions, 
                   std::vector<bool> m_pointerToFlags,
                   float xmin, float xmax, int nbinx, 
                   float ymin,float ymax  );
  
  virtual ~MuBgDistribution( ); ///< Destructor
  inline float giveMin(){if(m_dimension==2)return m_ymin;
  else return m_xmin; };
  inline float giveMax(){if (m_dimension==2)return m_ymax;  
  else return m_xmax; };
  float giveRND();
  float giveRND(float x);
protected:

private:
  std::vector<Rndm::Numbers*> m_pointerToDistributions;
  std::vector<bool> m_pointerToFlags;
  
  int m_nbinx;
  float m_xmin,m_xmax,m_ymin,m_ymax;
  int m_dimension;
  

};
#endif // MUBGDISTRIBUTION_H
