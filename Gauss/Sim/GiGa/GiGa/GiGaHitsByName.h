// $Id: GiGaHitsByName.h,v 1.1 2002-12-07 21:05:30 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef GIGA_GIGAHITSBYNAME_H 
#define GIGA_GIGAHITSBYNAME_H 1
// Include files
// STD & STL 
#include <string>
// forward declaration from G4 
class G4VHitsCollection ;

/** @class GiGaHitsByName GiGaHitsByName.h GiGa/GiGaHitsByName.h
 *  
 *  Helpful class for extraction of G4 hit collections  
 *  by their unique Name 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2002-12-07
 */


class GiGaHitsByName
{
public:
  
  /** Standard constructor
   *  @param Name       the unique collection Name 
   *  @param collection the collection itself
   */
  GiGaHitsByName( const std::string& name           , 
                  G4VHitsCollection* collection = 0 ) 
    : m_name       ( name       ) 
    , m_collection ( collection ) {} ;
  
  // destructor
  ~GiGaHitsByName(){}; ///< Destructor
  
  /// access to unique collection Name
  const std::string&       name       () const { return m_name          ; }
  // access to the collection itself (const)
  const G4VHitsCollection* collection () const { return m_collection    ; }
  // access to the collection itself (const)
  const G4VHitsCollection* hits       () const { return   collection () ; }
  // access to the collection itself (const)
  const G4VHitsCollection* col        () const { return   collection () ; }
  
  /// access to the collection itself (non const)
  G4VHitsCollection*       collection ()       { return m_collection    ; }
  /// access to the collection itself (const)
  G4VHitsCollection*       hits       ()       { return   collection () ; }
  /// access to the collection itself (const)
  G4VHitsCollection*       col        ()       { return   collection () ; }
  
  /** set new collection pointer 
   *  @param value new value of collection pointer 
   */
  GiGaHitsByName& setCollection ( G4VHitsCollection* value ) 
  { m_collection = value ; return *this ; };
  
  /** set new collection pointer 
   *  @param value new value of collection pointer 
   */
  GiGaHitsByName& setHits       ( G4VHitsCollection* value ) 
  { return setCollection ( value ); }
  
  /** set new collection pointer 
   *  @param value new value of collection pointer 
   */
  GiGaHitsByName& setCol        ( G4VHitsCollection* value ) 
  { return setCollection ( value ); }  
  
private:
  
  /// default constructor is private 
  GiGaHitsByName();
  
private:
  
  std::string        m_name       ;
  G4VHitsCollection* m_collection ;
  
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif // GIGA_GIGAHITSBYNAME_H
// ============================================================================
