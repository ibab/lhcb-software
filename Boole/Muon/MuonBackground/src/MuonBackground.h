// $Id: MuonBackground.h,v 1.7 2005-04-14 13:10:41 cattanem Exp $
#ifndef MUONBACKGROUND_H 
#define MUONBACKGROUND_H 1

// Include files
// from STL
#include <string>
#include <cmath> 

// from Gaudi
#include "GaudiKernel/Algorithm.h" 
#include "GaudiKernel/RndmGenerators.h"
#include "MuBgDistribution.h"

// forward declarations
class IHistogram1D;
class IHistogram2D;
class IMuonTileXYZTool;
class IMuonGetInfoTool;

typedef std::vector<std::string> VectorName;   

/** @class MuonBackground MuonBackground.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2003-02-18
 */

class MuonBackground : public Algorithm {
public:
  /// Standard constructor
  MuonBackground( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MuonBackground( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  StatusCode HitInCurrentEvent (int pileupNumber);
  StatusCode initializeGeometry ();
  StatusCode calculateNumberOfCollision(int ispill);
  inline int mapHistoCode(int icode, int station, int multy )
  {return m_histogramsUpCode+icode*10000+(station+1)*1000+(multy+1)*100;  
  }
  
  inline void setCollsionNumber(int number)
  {m_collisionNumber=number;
  
  }
  inline int collisions()
  {
    return m_collisionNumber;  
  }
  
  StatusCode calculateStartingNumberOfHits(int ispill);
  StatusCode initializeParametrization();
  
  inline std::vector<float>* getVectorOfGapPosition(int station){
    return &containerOfFirstGapPosition[station];
};
  StatusCode initializeRNDDistribution1D(IHistogram1D* histoPointer,
                                         std::vector<Rndm::Numbers*>& 
                                         distributions , std::vector<bool>& 
                                            pointerToFlags,double& xmin, 
                                         double& xmax);
  StatusCode initializeRNDDistribution2D(IHistogram2D* histoPointer,
                                         std::vector<Rndm::Numbers*>& 
                                         distributions , std::vector<bool>& 
                                            pointerToFlags,double& xmin, 
                                         double& xmax, int& nbinx ,
                                         double &ymin, double& ymax  );
  StatusCode createHit(KeyedContainer<MCMuonHit>** hitsContainer,
                                    int station,int multi , int ispill); 
  StatusCode correctInterceptPosition(float xlow,float xup,float ylow,
                                      float yup,float zlow,float zup,
                                      float xave,float yave,float zave,
                                      float xslope,float yslope,
                                      float& xentry,float & xexit,
                                      float& yentry,float& yexit, 
                                      float& zentry,float& zexit);
  float max(float a,float b);
  float min(float a,float b);
  int howManyHit( float floatHit);
  
protected:

private:
  int m_type;  
  std::string m_typeOfBackground; 
  
  std::vector<double> m_safetyFactor;  
  int m_stationNumber;
  std::vector<int> m_numberOfGaps;
  int m_gaps,m_maxDimension;  
  int m_regionNumber;
  int m_partition;  
  int m_numberOfEventsNeed;
  bool m_histos; ///< Job option to switch on debug histos

  float m_luminosity;
  std::vector<double> m_flatSpilloverHit;
  int m_numberOfFlatSpill;
  
  std::vector<std::vector <float> > containerOfFirstGapPosition;
  static std::string spill[5];
  static std::string numreg[4];
  static std::string numsta[5];
  static std::string TESPathOfHitsContainer[4];
  std::string m_containerName;
  int  m_histogramsUpCode;
  float  m_BXTime;  
  int m_readSpilloverEvents;  
  std::vector< std::vector<int> >* m_resultPointer ;
  int m_collisionNumber;
  std::string m_histoFile;  
  VectorName m_histoName;  
  std::vector<int> m_histogramsMapNumber;
  std::vector<int> m_histogramsDimension;
  std::vector<MuBgDistribution*> m_correlation;
  std::vector<MuBgDistribution*> m_radial;
  std::vector<MuBgDistribution*> m_phiglobvsradial;
  std::vector<MuBgDistribution*> m_thetalocvsradial;
  std::vector<MuBgDistribution*> m_philocvsradial;
  std::vector<MuBgDistribution*> m_logtimevsradial;  
  std::vector<MuBgDistribution*> m_lintimevsradial;
  std::vector<MuBgDistribution*> m_hitgap;
  double m_luminosityFactor; 
  IMuonTileXYZTool* m_pMuonTileXYZ ;
  IMuonGetInfoTool* m_pGetInfo ;
  //only to test the histos  
  IHistogram1D * m_pointer1D[20];
  IHistogram2D * m_pointer2D[20];
  Rndm::Numbers* m_flatDistribution;
  std::string m_persType;
};



#endif // MUONBACKGROUND_H
