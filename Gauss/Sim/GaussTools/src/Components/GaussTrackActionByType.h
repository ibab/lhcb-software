// $Id: GaussTrackActionByType.h,v 1.3 2007-01-12 15:36:43 ranjard Exp $
#ifndef GAUSSTOOLS_GAUSSTRACKACTIONBYTYPE_H 
#define GAUSSTOOLS_GAUSSTRACKACTIONBYTYPE_H 1

// local
#include "GaussTools/GaussTrackActionZ.h"

class G4ParticleDefinition ;

/** @class GaussTrackActionByType GaussTrackActionByType.h 
 *  
 *  Track action, valid for certain region in Z based on 
 *  particle/children types 
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2004-02-19
 */
class GaussTrackActionByType : public GaussTrackActionZ 
{
  /// friend factory for instantiation 
  //  friend class GiGaFactory<GaussTrackActionByType> ;
public:

  /** initialize the track action  
   *  @see GiGaTrackActionBase 
   *  @see GiGaBase 
   *  @see  AlgTool 
   *  @see IAlgTool 
   *  @return status code 
   */
  virtual StatusCode initialize () ;
  
  /** perform action 
   *  @see G4VUserTrackingAction
   *  @param pointer to new track opbject 
   */
  virtual void PreUserTrackingAction  ( const G4Track* ) ;
  
  /** perform action 
   *  @see G4VUserTrackingAction
   *  @param pointer to new track opbject 
   */
  virtual void PostUserTrackingAction ( const G4Track* ) ;
  
  //protected:
  
  /** standard constructor 
   *  @see GiGaTrackActionBase 
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GaussTrackActionByType
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// destructor (virtual and protected)
  virtual ~GaussTrackActionByType();
  
private:
  
  // default constructor   is disabled 
  GaussTrackActionByType() ; 
  // copy    constructor   is disabled 
  GaussTrackActionByType           ( const GaussTrackActionByType& ) ; 
  // assignement operator  is disabled 
  GaussTrackActionByType& operator=( const GaussTrackActionByType& ) ;
  
protected:
  
  typedef std::vector<std::string>                  Names  ;
  typedef std::vector<const G4ParticleDefinition*>  PartDefs; 
  
  const PartDefs& ownTypes   () const { return m_ownTypes ; }
  const PartDefs& childTypes () const { return m_childTypes ; }
  
  bool  storeOwn ( const G4Track* track ) const 
  {
    if ( 0 == track        ) { return false ; }
    if ( m_ownTypes.empty() ) { return false ; }
    return std::binary_search ( m_ownTypes.begin () , 
                                m_ownTypes.end   () , 
                                track->GetDefinition() ) ;
  } ;
  
  bool  storeChild ( const G4Track* track ) const 
  {
    if ( 0 == track           ) { return false ; }
    if ( m_childTypes.empty() ) { return false ; }
    return std::binary_search ( m_childTypes.begin () , 
                                m_childTypes.end   () , 
                                track->GetDefinition() ) ;
  } ;

private:
  
  Names    m_ownNames    ;
  Names    m_childNames  ;
  // sorted list of definitions 
  PartDefs m_ownTypes    ; // sorted!
  // sorted list of definitions 
  PartDefs m_childTypes  ; // sorted 
  bool     m_rejectRICHphe ;
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // GAUSSTOOLS_GAUSSTRACKACTIONBYTYPE_H
// ============================================================================
