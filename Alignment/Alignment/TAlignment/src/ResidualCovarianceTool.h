// $Id: ResidualCovarianceTool.h,v 1.2 2008-01-22 16:15:15 janos Exp $
#ifndef TRACKTOOLS_RESIDUALCOVARIANCETOOL_H 
#define TRACKTOOLS_RESIDUALCOVARIANCETOOL_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from TrackEvent
#include "Event/Track.h"
#include "Kernel/LHCbID.h"

#include <vector>
#include "CLHEP/Matrix/SymMatrix.h"



class ResidualCovarianceTool //: public GaudiTool
{
public:
  /// Standard constructor
  //ResidualCovarianceTool( const std::string& type, 
  //			  const std::string& name,
  //			  const IInterface* parent);
  ResidualCovarianceTool() {}
  virtual ~ResidualCovarianceTool( ) {}; ///< Destructor
  
  virtual StatusCode compute(const LHCb::Track& track) ;
  
  const CLHEP::HepSymMatrix& HCH_norm() const { return m_HCH ; }
  double HCH_norm(const LHCb::Node& i, const LHCb::Node& j) const;

private:
  CLHEP::HepSymMatrix m_HCH ;
  std::vector<const LHCb::Node*> m_nodes ;
};
#endif // TRACKTOOLS_MEASUREMENTPROVIDER_H
