// $Id: TsaSeed.h,v 1.2 2006-08-17 08:36:09 mneedham Exp $
#ifndef _TSASEED_H_
#define _TSASEED_H_

/** @class TsaSeed
 *
 *
 *  @author M.Needham
 *  @date   07/03/2002
 */

#include "TsaBaseAlg.h"
#include <string>
#include <functional>
#include "TsaKernel/stopwatch.h"
#include "SeedPnt.h"
#include "SeedTrack.h"

class IOTExpectedHits;
class IOTDataSvc;
class IITDataSvc;

namespace Tsa{
  class Cluster;
}


class TsaSeed: public TsaBaseAlg {

public:
  
  // Constructors and destructor
  TsaSeed(const std::string& name, 
                   ISvcLocator* pSvcLocator); 
  virtual ~TsaSeed();  

  // IAlgorithm members
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

private:

  void search( int& sector, std::vector<SeedHit*> hits[], std::vector<SeedTrack*>& seeds );
  void searchStereo( int& sector, std::vector<SeedHit*> hits[], std::vector<SeedTrack*>& seeds );
  int fitLine( SeedTrack* seed );
  int fitParabola( SeedTrack* seed, double &cth );
  int refitParabola( SeedTrack* seed, double &cth );
  void likelihood( std::vector<SeedTrack*>& seeds , const unsigned iSector);
  void select( std::vector<SeedTrack*>& seeds, std::vector<SeedHit*> hits[], std::vector<SeedHit*> hit2[] );


  class compByX_LB: public std::binary_function<const SeedHit*, const double, bool>{
    double testVal;
  public:
    inline bool operator() (const SeedHit* obj,const double& testVal) const{
      return ((!obj) ? false : testVal > obj->x());
    }
  };

  
  class compByXMin_LB: public std::binary_function<const SeedHit*, const double, bool>{
    double testVal;
  public:
    inline bool operator() (const SeedHit* obj,const double& testVal) const{
      return ((!obj) ? false : testVal > obj->xMin());
    }
  };

  class compByXMax_LB: public std::binary_function<const SeedHit*, const double, bool>{
    double testVal;
  public:
    inline bool operator() (const SeedHit* obj,const double& testVal) const{
      return ((!obj) ? false : testVal > obj->xMax());
    }
  };


  class increasingX  {
  public:
    bool operator() ( SeedHit* first, SeedHit* second ) const;
  };

  class decreasingLikelihood  {
  public:
    bool operator() ( SeedTrack* first, SeedTrack* second ) const;
  };

  std::vector<SeedHit*>::iterator startX(std::vector<SeedHit*>& hits, const double x1, 
                                         const double z1, const double sx) const;  


  std::vector<SeedHit*>::iterator startStereo(std::vector<SeedHit*>& hits, const double x ) const;  

  //  SeedHitRange collectHitsInY(const std::vector<SeedHit*>& hits, const double x) const;


  std::string m_otDataSvcName;
  std::string m_itDataSvcName;

  double m_maxDriftRadius;


  IOTDataSvc* m_otDataSvc;
  IITDataSvc* m_itDataSvc;
                         

  IOTExpectedHits* m_expectedHits;

  Tsa::stopWatch m_xWatch;
  Tsa::stopWatch m_yWatch;
  Tsa::stopWatch m_lWatch;


};

#endif // _TSASEED
