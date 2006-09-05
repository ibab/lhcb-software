// $Id: TsaSeed.h,v 1.4 2006-09-05 15:48:51 mneedham Exp $
#ifndef _TSASEED_H_
#define _TSASEED_H_

/** @class TsaSeed
 *
 *
 *  @author M.Needham
 *  @date   07/03/2002
 */

#include "GaudiAlg/GaudiTupleAlg.h"
#include <string>

#include <functional>
#include "TsaKernel/stopwatch.h"

#include "SeedHit.h"
#include "SeedPnt.h"
#include "SeedTrack.h"
#include "SeedStub.h"


#include "TsaBaseAlg.h"

class IOTExpectedHits;
class IITExpectedHits;
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

  void extendStubs( int& sector, std::vector<SeedStub*> stubs[], std::vector<SeedHit*> hits[], 
                    std::vector<SeedHit*> sHits[], std::vector<SeedTrack*>& seeds );
  void linkStubs( std::vector<SeedStub*> stubs[], std::vector<SeedTrack*>& seeds );
  void findStubs( int& sector, std::vector<SeedHit*> hits[], std::vector<SeedHit*> sHits[], 
                  std::vector<SeedStub*> stubs[] );
  void searchX( int& sector, std::vector<SeedHit*> hits[], std::vector<SeedTrack*>& seeds );
  void searchStereo( int& sector, std::vector<SeedHit*> hits[], std::vector<SeedTrack*>& seeds );
  int fitLine( SeedTrack* seed );
  int fitParabola( SeedTrack* seed, double &cth );
  int refitParabola( SeedTrack* seed, double &cth );
  void likelihood( int& sector, std::vector<SeedTrack*>& seeds );
  void select( std::vector<SeedTrack*>& seeds );

  std::vector<SeedHit*>::iterator startX(std::vector<SeedHit*>& hits, const double x1, 
                                          const double z1, const double sx) const;  


  std::vector<SeedHit*>::iterator startStereo(std::vector<SeedHit*>& hits, const double x ) const;  

  class increasingX  {
  public:
    bool operator() ( SeedHit* first, SeedHit* second ) const;
  };

  class decreasingLikelihood  {
  public:
    bool operator() ( SeedTrack* first, SeedTrack* second ) const;
  };

  
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


  void collectXHits(std::vector<SeedHit*>& hits, std::vector<SeedHit*>::iterator& start, 
		    const double x, const double win, std::vector<SeedHit*>& selected) const;


  std::string m_otDataSvcType;
  std::string m_itDataSvcType;

  std::string m_otDataSvcName;
  std::string m_itDataSvcName;

  std::string m_seedTrackLocation;
  std::string m_seedHitLocation;
  std::string m_seedStubLocation;


  double m_likCut;
  IOTDataSvc* m_otDataSvc;
  IITDataSvc* m_itDataSvc;
                         
  IOTExpectedHits* m_expectedHits;
  IITExpectedHits* m_expectedITHits;

  double m_z0;
  double m_sth;

  std::vector<double> m_xSearch_sxCut;
  std::vector<double> m_xSearch_xsCut;
  std::vector<double> m_xSearch_x0Cut;
  std::vector<double> m_xSearch_sx2Cut;
  std::vector<double> m_xSearch_dthCut;
  std::vector<double> m_xSearch_tdCut;
  std::vector<double> m_xSearch_win;
  std::vector<double> m_xSearch_win2;
  std::vector<int> m_xSearch_nWin;
  std::vector<int> m_xSearch_nWin2;
  std::vector<int> m_xSearch_nxCut;

 
  Tsa::stopWatch m_xWatch;
  Tsa::stopWatch m_yWatch;
  Tsa::stopWatch m_lWatch;
  Tsa::stopWatch m_sWatch;
  Tsa::stopWatch m_stubWatch;




};


inline void TsaSeed::collectXHits(std::vector<SeedHit*>& hits, std::vector<SeedHit*>::iterator& start, 
                                  const double x, const double win, std::vector<SeedHit*>& selected) const{

 std::vector<SeedHit*>::iterator it = start; 
 if (it !=  hits.end()){
   ++it;
   while ( it != hits.end() && (*it)->x() - x < win ) {
     selected.push_back( *it );
      ++it;
   }
 }

 it = start; 
 if (it != hits.begin()){
    --it;
   while ( it != hits.begin() && x - (*it)->x() < win) {
     selected.push_back( *it );
     --it;
   }
 }
}

#endif // _TSASEED
