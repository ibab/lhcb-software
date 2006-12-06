// $ Exp $
#ifndef _TsaSeedAddHits_H
#define _TsaSeedAddHits_H

/** @class TsaXSearchBase TsaXSearchBase Tsa/TsaXSearchBase
* Follow track and pick up hits
* @author M. Needham
**/

#include "ITsaSeedAddHits.h"
#include "GaudiAlg/GaudiTool.h"
#include "TsaKernel/Types.h"

#include "TsaKernel/SeedTrack.h"

#include <string>
#include <vector>

class SeedLineFit;
class SeedParabolaFit;
class IITDataSvc;
class DeSTDetector;

class TsaSeedAddHits: public GaudiTool,
                       virtual public ITsaSeedAddHits{

public:

  /// constructer
  TsaSeedAddHits(const std::string& type,
                 const std::string& name,
                 const IInterface* parent);

  // destructer
  virtual ~TsaSeedAddHits();
  
  virtual StatusCode initialize();

  // execute method
  virtual StatusCode execute(SeedTracks* seeds, SeedHits* hits);

private:

  Tsa::STRange range(const int hint, const int station, 
		     const int layer, const int box) const;

  SeedParabolaFit* m_parabolaFit;
  SeedLineFit* m_fitLine;
  
  std::string m_dataSvcType;
  std::string m_dataSvcName;
  IITDataSvc* m_dataSvc;

  DeSTDetector* m_tracker;
  unsigned int m_nStations;
  unsigned int m_nBox;
  std::vector<double> m_zBox;
  std::vector<double> m_xMin;
  std::vector<double> m_xMax;
  std::vector<double> m_yMin;
  std::vector<double> m_yMax;
  double m_tol;
  double m_dCut;

};

#endif  // _TsaSeedAddHits_H

  

