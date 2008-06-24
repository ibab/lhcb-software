// $Id: G4Higgses.h,v 1.1 2008-06-24 10:29:45 ibelyaev Exp $
// ============================================================================
#ifndef G4HIGGSES_H 
#define G4HIGGSES_H 1
// ============================================================================
// Include files
// ============================================================================
// GEANT4
// ============================================================================
#include "G4ParticleDefinition.hh"
// ============================================================================
/** @class G4H_10
 *  Helper class for implementation of various higgses and v-particles 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-06-19
 */
class G4H_10: public G4ParticleDefinition 
{
public:
  // ==========================================================================
  /** get the definition (Access to the static instance)
   *  @return G4H_10 particle definition 
   *  @attention this method *COULD* (on purpose!) return NULL pointer!!
   */
  static G4H_10* Definition     () ;
  /** get the definition (Access to the static instance)
   *  @return G4H_10 particle definition 
   *  @attention this method *COULD* (on purpose!) return NULL pointer!!
   */
  static G4H_10* H_10Definition () ;
  /** get the definition (Access to the static instance)
   *  @return G4H_10 particle definition 
   *  @attention this method *COULD* (on purpose!) return NULL pointer!!
   */
  static G4H_10* H_10           () ;
  // ==========================================================================
public:
  // ==========================================================================
  /** create the particle definition 
   *  @param mass the mass 
   *  @param ctau the lifetime 
   */
  static G4H_10* Create 
  ( const double mass ,                                    // the mass 
    const double ctau ) ;                                  // the lifetime 
  // ==========================================================================
protected:
  // ==========================================================================
  /** constructor from the mass and lifetime 
   *  @param mass the mass 
   *  @param ctau the lifetime 
   */
  G4H_10 ( const double mass , 
           const double ctau ) ;
  // ==========================================================================
public:
  // ==========================================================================
  /// virtual destructor 
  virtual ~G4H_10() ;  // virtual desctructor 
  // ==========================================================================
private:
  // ==========================================================================
  /// default consstructor is disabled 
  G4H_10 () ;                               // default consstructor is disabled 
  /// copy constructor is disabled 
  G4H_10 ( const G4H_10& ) ;                    // copy constructor is disabled 
  /// assignement operator is disabed 
  G4H_10& operator=( const G4H_10& ) ;       // assignement operator is disabed 
  // ==========================================================================
private:
  // ==========================================================================
  /// the static instance of the class 
  static G4H_10* s_instance ;
  // ==========================================================================  
} ;
// ============================================================================
/** @class G4H_20
 *  Helper class for implementation of various higgses and v-particles 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-06-19
 */
class G4H_20: public G4ParticleDefinition 
{
public:
  // ==========================================================================
  /** get the definition (Access to the static instance)
   *  @return G4H_20 particle definition 
   *  @attention this method *COULD* (on purpose!) return NULL pointer!!
   */
  static G4H_20* Definition     () ;
  /** get the definition (Access to the static instance)
   *  @return G4H_20 particle definition 
   *  @attention this method *COULD* (on purpose!) return NULL pointer!!
   */
  static G4H_20* H_20Definition () ;
  /** get the definition (Access to the static instance)
   *  @return G4H_10 particle definition 
   *  @attention this method *COULD* (on purpose!) return NULL pointer!!
   */
  static G4H_20* H_20           () ;
  // ==========================================================================
public:
  // ==========================================================================
  /** create the particle definition 
   *  @param mass the mass 
   *  @param ctau the lifetime 
   */
  static G4H_20* Create 
  ( const double mass ,                                    // the mass 
    const double ctau ) ;                                  // the lifetime 
  // ==========================================================================
protected:
  // ==========================================================================
  /** constructor from the mass and lifetime 
   *  @param mass the mass 
   *  @param ctau the lifetime 
   */
  G4H_20 ( const double mass , 
           const double ctau ) ;
  // ==========================================================================
public:
  // ==========================================================================
  /// virtual destructor 
  virtual ~G4H_20() ;  // virtual desctructor 
  // ==========================================================================
private:
  // ==========================================================================
  /// default consstructor is disabled 
  G4H_20 () ;                               // default consstructor is disabled 
  /// copy constructor is disabled 
  G4H_20 ( const G4H_20& ) ;                    // copy constructor is disabled 
  /// assignement operator is disabed 
  G4H_20& operator=( const G4H_20& ) ;       // assignement operator is disabed 
  // ==========================================================================
private:
  // ==========================================================================
  /// the static instance of the class 
  static G4H_20* s_instance ;
  // ==========================================================================  
} ;
// ============================================================================
/** @class G4H_30
 *  Helper class for implementation of various higgses and v-particles 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-06-19
 */
class G4H_30: public G4ParticleDefinition 
{
public:
  // ==========================================================================
  /** get the definition (Access to the static instance)
   *  @return G4H_10 particle definition 
   *  @attention this method *COULD* (on purpose!) return NULL pointer!!
   */
  static G4H_30* Definition     () ;
  /** get the definition (Access to the static instance)
   *  @return G4H_10 particle definition 
   *  @attention this method *COULD* (on purpose!) return NULL pointer!!
   */
  static G4H_30* H_30Definition () ;
  /** get the definition (Access to the static instance)
   *  @return G4H_10 particle definition 
   *  @attention this method *COULD* (on purpose!) return NULL pointer!!
   */
  static G4H_30* H_30           () ;
  // ==========================================================================
public:
  // ==========================================================================
  /** create the particle definition 
   *  @param mass the mass 
   *  @param ctau the lifetime 
   */
  static G4H_30* Create 
  ( const double mass ,                                    // the mass 
    const double ctau ) ;                                  // the lifetime 
  // ==========================================================================
protected:
  // ==========================================================================
  /** constructor from the mass and lifetime 
   *  @param mass the mass 
   *  @param ctau the lifetime 
   */
  G4H_30 ( const double mass , 
           const double ctau ) ;
  // ==========================================================================
public:
  // ==========================================================================
  /// virtual destructor 
  virtual ~G4H_30() ;  // virtual desctructor 
  // ==========================================================================
private:
  // ==========================================================================
  /// default consstructor is disabled 
  G4H_30 () ;                               // default consstructor is disabled 
  /// copy constructor is disabled 
  G4H_30 ( const G4H_30& ) ;                    // copy constructor is disabled 
  /// assignement operator is disabed 
  G4H_30& operator=( const G4H_30& ) ;       // assignement operator is disabed 
  // ==========================================================================
private:
  // ==========================================================================
  /// the static instance of the class 
  static G4H_30* s_instance ;
  // ==========================================================================  
} ;
// ============================================================================
// The END 
// ============================================================================
#endif // G4HIGGSES_H
// ============================================================================

