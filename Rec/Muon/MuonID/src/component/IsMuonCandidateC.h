// $Id: IsMuonCandidateC.h,v 1.1 2009-07-01 18:27:11 polye Exp $
#ifndef ISMUONCANDIDATEC_H 
#define ISMUONCANDIDATEC_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "IIsMuonCandidateC.h"            // Interface


/** @class Ismuoncandidatec Ismuoncandidatec.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @author Xabier Cid Vidal
 *  @date   2009-03-12
 */
class IsMuonCandidateC : public GaudiTool, public IIsMuonCandidateC {
public: 
  /// Standard constructor
  IsMuonCandidateC( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);

  virtual ~IsMuonCandidateC( ); ///< Destructor

  
  virtual StatusCode initialize();
  bool IsMuonCandidate(const LHCb::Track& muTrack);
  bool IsMuonCandidate(const std::vector<LHCb::LHCbID>& LHCbIDs,const double& mom);

  bool stInStations(const int myst,const std::vector<int>& stations);
  bool IsMuonSimple(const std::vector<int>& stations);
  bool IsMuon(const std::vector<int>& stations,double p);
  bool IsMuonLoose(const std::vector<int>& stations,double p);

protected:

  std::vector<double> m_MomRangeIsMuon;
  int m_ismopt;
  int m_minhits;

};
#endif // ISMUONCANDIDATEC_H
