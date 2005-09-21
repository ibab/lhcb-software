#ifndef TRACKMCTOOLS_IDEALSTATECREATOR_H
#define TRACKMCTOOLS_IDEALSTATECREATOR_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from Event
#include "Event/MCParticle.h"
#include "Event/MCHit.h"
#include "Event/MCVeloHit.h"

// from Det
#include "VeloDet/DeVelo.h"

// from LHCbKernel
#include "Relations/IAssociator.h" 

// from TrackInterfacces
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "TrackInterfaces/IIdealStateCreator.h"

class State;

/** @class IdealStateCreator IdealStateCreator.h "TrackMCTools/IdealStateCreator.h"
 * 
 *  A IdealStateCreator is a IIdealStateCreator tool that creates a
 *  State. There are two methods: one creates a state at a certain 
 *  z-position using the closest two extry/exit points from a MCParticle, 
 *  and a track extrapolator. The other creates a state at the vertex, using 
 *  the properties of the MCParticles and MCVertex.
 *
 *  The diagonal elements of the covariance matrix are set with
 *  the job options. Note that "eP" is dp/p.
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
  /// Typedefs
  typedef IAssociator<MCParticle, MCHit>     MCHitAsct;
  typedef IAssociator<MCParticle, MCVeloHit> MCVeloHitAsct;

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

  DeVelo* m_velo;                 ///< Velo detector information

  MCVeloHitAsct* m_p2VeloHitAsct; ///< MCParticle to Velo MCHit Associator
  MCHitAsct*     m_p2ITHitAsct;   ///< MCParticle to IT MCHit Associator
  MCHitAsct*     m_p2OTHitAsct;   ///< MCParticle to OT MCHit Associator

  ITrackExtrapolator* m_extrapolator; ///< Extrapolator Tool

  // Job options:
  std::string m_p2VeloHitAsctName; ///< Name of MCParticle to Velo MCHit Asc.
  std::string m_p2ITHitAsctName;   ///< Name of MCParticle to IT MCHit Asc.
  std::string m_p2OTHitAsctName;   ///< Name of MCParticle to OT MCHit Asc.
  std::string m_extrapolatorName;  ///< Name of track state extrapolator.
  double m_eX2;                    ///< Error^2 on x
  double m_eY2;                    ///< Error^2 on y
  double m_eTx2;                   ///< Error^2 on slope x
  double m_eTy2;                   ///< Error^2 on slope y
  double m_eP;                     ///< dp/p
};


#endif // TRACKMCTOOLS_IDEALSTATECREATOR_H
