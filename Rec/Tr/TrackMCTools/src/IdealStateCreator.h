// $Id: IdealStateCreator.h,v 1.5 2006-03-31 13:23:36 erodrigu Exp $
#ifndef TRACKMCTOOLS_IDEALSTATECREATOR_H
#define TRACKMCTOOLS_IDEALSTATECREATOR_H 1

// Include files

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from TrackMCInterfacces
#include "TrackMCInterfaces/IIdealStateCreator.h"

// from TrackInterfacces
#include "TrackInterfaces/ITrackExtrapolator.h"

namespace LHCb
{ 
  class State;
  class MCParticle;
  class MCHit;
}

using namespace LHCb;

/** @class IdealStateCreator IdealStateCreator.h "TrackMCTools/IdealStateCreator.h"
 * 
 *  An IdealStateCreator is an IIdealStateCreator tool that creates a
 *  State. There are two methods: one creates a state at a certain 
 *  z-position using the closest two extry/exit points from a MCParticle, 
 *  and a track extrapolator. The other creates a state at the vertex, using 
 *  the properties of the MCParticles and MCVertex.
 *
 *  The diagonal elements of the covariance matrix are set with
 *  the job options. Note that "eP" is dp/p.
 *
 *  Moved to LHCb v20r0. Adapted code to use updated Det packages.
 *  @author Edwin Bos
 *  @date   2006-02-02
 *
 *  @author Eduardo Rodrigues (adaptations to new track event model)
 *  @date   2005-04-06
 *
 *  @author Rutger van der Eijk, Jeroen van Tilburg
 *  @date   3-7-2002
 */

class IdealStateCreator: public GaudiTool,
                         virtual public IIdealStateCreator {
public:

  /// Standard constructor
  IdealStateCreator( const std::string& type,
                     const std::string& name,
                     const IInterface* parent );

  /// Destructor
  virtual ~IdealStateCreator();

  /// Tool initialization
  virtual StatusCode initialize();

  /** This method creates a state at z position from a MCParticle
   *  using the entry/exit points of the MCHits.
   *  @return StatusCode
   *  @param  mcPart The MCParticle from which the state will be created
   *  @param  zRec   The z-position at which the state will be created
   *  @param  pState The pointer to the State which is created.
   */
  virtual StatusCode createState( const MCParticle* mcPart,
                                  double zRec,
                                  State*& pState ) const;

  /** This method creates a state at the origin vertex from a MCParticle
   *  using the entry/exit points of the MCHits.
   *  @return StatusCode
   *  @param  mcPart The MCParticle from which the state will be created
   *  @param  pState The pointer to the State which is created.
   */
  virtual StatusCode createStateVertex( const MCParticle* mcPart,
                                        State*& pState ) const;

private:
  /// Determine Q/P for a MCParticle
  double qOverP( const MCParticle* mcPart ) const;

  void findClosestHits( const MCParticle* mcPart,
                        const double zRec,
                        MCHit*& closestHit,
                        MCHit*& secondClosestHit ) const;
  
private:
  ITrackExtrapolator* m_extrapolator; ///< Extrapolator Tool

  // Job options:
  std::string m_extrapolatorName;  ///< Name of track state extrapolator.
  double m_eX2;                    ///< Error^2 on x
  double m_eY2;                    ///< Error^2 on y
  double m_eTx2;                   ///< Error^2 on slope x
  double m_eTy2;                   ///< Error^2 on slope y
  double m_eP;                     ///< dp/p
};

#endif // TRACKMCTOOLS_IDEALSTATECREATOR_H
