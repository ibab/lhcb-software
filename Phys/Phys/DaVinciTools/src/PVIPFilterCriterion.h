// $Id: PVIPFilterCriterion.h,v 1.2 2004-07-08 10:14:26 pkoppenb Exp $
#ifndef PVIPFILTERCRITERION_H 
#define PVIPFILTERCRITERION_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiTools/GaudiTool.h"

// from DaVinciTools
#include "DaVinciTools/IFilterCriterion.h"

// forward declarations
class IGeomDispCalculator;
class IDataProviderSvc;

/** @class PVIPFilterCriterion PVIPFilterCriterion.h
 *  Returns a yes/no depending on IP on PV(s).
 *  Min IP: Checks there is no PV with IP < MinIP (typically for a B track)
 *  Max IP: Checks there is at least one PV with IP < MaxIP (typically for B)
 *  @author P. Koppenburg
 *  @date   24/06/2004
 */
class PVIPFilterCriterion : public GaudiTool, 
                           virtual public IFilterCriterion {

public:

  /// Standard constructor
  PVIPFilterCriterion( const std::string& type, 
                      const std::string& name,
                      const IInterface* parent);
  
  /// Destructor
  virtual ~PVIPFilterCriterion( ){ }; 

  /// Initialize: Prints Out the cut values.
  StatusCode initialize( );

  /// Test if impact parameter filter is satisfied.
  inline bool isSatisfied( const Particle* const & part );

  /// Test if impact parameter filter is satisfied.
  inline bool operator()( const Particle* const & part );

private:
  
  double     m_minIP; ///< Minimum IP
  double     m_maxIP; ///< Maximum IP
  double     m_minIPsignif ; ///< Minimum IP significance
  double     m_maxIPsignif ; ///< Maximum IP significance
  IDataProviderSvc* m_EDS;
  IGeomDispCalculator* m_ipTool;


};

#endif // PVIPFILTERCRITERION_H
