// $ Exp $
#ifndef _TsaStereoBase_H
#define _TsaStereoBase_H

/** @class TsaStereoBase TsaStereoBase Tsa/TsaStereoBase
* Follow track and pick up hits
* @author M. Needham
**/

#include "TsaKernel/ITsaSeedStep.h"
#include "GaudiAlg/GaudiTool.h"
#include <algorithm>

class SeedHit; 
class SeedLineFit;

class TsaStereoBase:  public GaudiTool,
                       virtual public ITsaSeedStep {

public:

  /// constructer
  TsaStereoBase(const std::string& type,
                const std::string& name,
                const IInterface* parent);

  // destructer
  virtual ~TsaStereoBase();

  virtual StatusCode execute(LHCb::State& hint, std::vector<SeedTrack*>& seeds, std::vector<SeedHit*> hits[6] = 0 ) ;

  virtual StatusCode execute(std::vector<SeedTrack*>& seeds, std::vector<SeedHit*> hits[6] = 0 ) ;

protected:

 int sector() const;

 void cleanup(std::vector<SeedHit*> hits[6]);

 unsigned int collectHits(SeedTrack* seed, std::vector<SeedHit*> hits[6],
                          std::vector<SeedHit*> yHits[6] );


 double m_syCut;
 double m_win;
 double m_nWin;
 double m_yTol;
 double m_nY;

 SeedLineFit* m_fitLine;  

private:  

  int m_sector;
  std::string m_seedHitLocation;
 
};

inline int TsaStereoBase::sector() const{
  return m_sector;
}

#endif  // _TsaStereoBase_H

  

