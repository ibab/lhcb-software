// $Id: MassDifferenceFilterCriterion.h,v 1.3 2004-08-23 12:40:48 ranjard Exp $
#ifndef MASSDIFFERENCEFILTERCRITERION_H 
#define MASSDIFFERENCEFILTERCRITERION_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from DaVinciTools
#include "DaVinciTools/IFilterCriterion.h"

// Forward declarations
class IParticlePropertySvc;

/** @class MassDifferenceFilterCriterion MassDifferenceFilterCriterion.h
 *  Returns a yes/no depending on the mass difference between a 
 *  particle and one of its daugthers
 *  @author Gerhard Raven
 *  @date   24/06/2002
 */
class MassDifferenceFilterCriterion : public GaudiTool, 
                           virtual public IFilterCriterion {
public:

  /// Standard constructor
  MassDifferenceFilterCriterion( const std::string& type, 
                                 const std::string& name,
                                 const IInterface* parent);
  
  /// Destructor
  virtual ~MassDifferenceFilterCriterion( ){ }; 

  /// Initialize: Prints Out the cut values.
  StatusCode initialize( );

  /// Test if filter is satisfied.
  inline bool isSatisfied( const Particle* const & part );

  /// Test if filter is satisfied.
  inline bool operator()( const Particle* const & part )
  { return isSatisfied(part); }

private:
  
  double     m_massWindow;     ///< Mass Window size
  double     m_massOffset;     ///< Mass Window offset
  std::string m_daughterName;  ///< Daughter to use for difference
  std::string m_motherName;    ///< Mother to use for difference
  int m_parID;
  int m_dauID;

};

#endif // MASSFILTERCRITERION_H
