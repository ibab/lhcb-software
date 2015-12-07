// $Id: ClearDaughters.h 193557 2015-08-12 11:07:07Z graven $
// ============================================================================
#ifndef CLEARDAUGHTERS_H
#define CLEARDAUGHTERS_H 1
// ============================================================================
// Include files
// DaVinci Kernel
// ============================================================================
#include "Kernel/DaVinciAlgorithm.h"
class ClearDaughters : public DaVinciAlgorithm
{
  // ==========================================================================
  /// friend factory for instantiation
  friend class AlgFactory<ClearDaughters> ;
  // ==========================================================================
protected:
  // ==========================================================================
  /// Base class type
  typedef DaVinciAlgorithm BaseClass;
  // ==========================================================================
public:
  // ==========================================================================
  /// the specific initialization
  virtual StatusCode initialize () ;           // the specific initialization
  /// the most interesting method
  virtual StatusCode execute    () ;           // the most interesting method
  // finalize /reset functors
  virtual StatusCode finalize   () ;
  // ==========================================================================
protected:
  // ==========================================================================
  /** standard constructor
   *  @see DaVinciAlgorithm
   *  @see GaudiTupleAlg
   *  @see GaudiHistoAlg
   *  @see GaudiAlgorithm
   *  @see Algorithm
   *  @see AlgFactory
   *  @see IAlgFactory
   *  @param name the algorithm instance name
   *  @param pSvc pointer to Service Locator
   */
  ClearDaughters                                        // standard contructor
  ( const std::string& name ,                  // the algorithm instance name
    ISvcLocator*       pSvc ) ;                 // pointer to Service Locator
  // ==========================================================================
  /// virtual & protected destructor
  virtual ~ClearDaughters() ;               // virtual & protected destructor
  // ==========================================================================
private:
  // ==========================================================================
  /// the default constructor is disabled
  ClearDaughters () ;                    // the default consructor is disabled
  /// copy constructor is disabled
  ClearDaughters ( const ClearDaughters& ) ;    // copy constructor is disabled
  /// assignement operator is disabled
  ClearDaughters& operator=( const ClearDaughters& ) ;        // no assignement
  // ==========================================================================
protected:
  // ==========================================================================
  /// Get the related PV of particle and relate it to clone.
  void cloneP2PVRelation
  ( const LHCb::Particle*   particle ,
    const LHCb::Particle*   clone      ) const;
  /// Write empty container if selection fails.
  void writeEmptyKeyedContainers  ( ) const;
  /// Write empty container if selection fails.
  void writeEmptySharedContainers ( ) const;
  // ============================================================================
private:
  // ============================================================================
  /// save (clone if needed) selected particles in TES
  template <class PARTICLES, class VERTICES, class CLONER>
  StatusCode _save () const ;
  // ============================================================================
protected :
  // ==========================================================================
  /// get the actual predicate:
  /// CloneFilteredParticles ?
  bool cloneFilteredParticles() const { return m_cloneFilteredParticles ; }
  /// CloneFilteredParticles ?
  void setCloneFilteredParticles ( const bool value )
  { m_cloneFilteredParticles = value ; }
  // ==========================================================================
  // ==========================================================================
private:
  // ==========================================================================
  /// save (clone if needed) selected particles in TES
  /// Success if number of saved particles == number saved
  /// to TES.
  /// Overwritten from DaVinciAlgorithm. Is called automatically.
  virtual StatusCode _saveInTES () ;
  /// Write empty containers if selection fails.
  virtual void writeEmptyTESContainers() ;
  // ==========================================================================
  // ==========================================================================
protected:
  // ==========================================================================
  /// CloneFilteredParticles ?
  bool m_cloneFilteredParticles;                    // CloneFilteredParticles ?
  // ==========================================================================
};
// ============================================================================

// ============================================================================
// The END
// ============================================================================
#endif // FILTERDESKTOP_H
// ============================================================================


