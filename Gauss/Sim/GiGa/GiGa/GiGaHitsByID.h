// $Id: GiGaHitsByID.h,v 1.2 2009-10-14 13:50:02 gcorti Exp $
#ifndef GIGA_GIGAHITSBYID_H 
#define GIGA_GIGAHITSBYID_H 1

// Include files
// forward declaration from G4 
class G4VHitsCollection ;

/** @class GiGaHitsByID GiGaHitsByID.h GiGa/GiGaHitsByID.h
 *  
 *  Helpful class for extraction of G4 hit collections  
 *  by their unique ID 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2002-12-07
 */

class GiGaHitsByID 
{
public:
  
  /** Standard constructor
   *  @param ID         the unique collection ID 
   *  @param collection the collection itself
   */
  GiGaHitsByID( const int          ID             , 
                G4VHitsCollection* collection = 0 ) 
    : m_id         ( ID         ) 
    , m_collection ( collection ) {} ;
  
  // destructor
  ~GiGaHitsByID(){}; ///< Destructor
  
  /// access to unique collection ID 
  int id() const { return m_id; }

  // access to the collection itself (const)
  const G4VHitsCollection* collection() const { return m_collection; }

  // access to the collection itself (const)
  const G4VHitsCollection* hits() const { return collection(); }

  // access to the collection itself (const)
  const G4VHitsCollection* col() const { return collection(); }
  
  /// access to the collection itself (non const)
  G4VHitsCollection* collection() { return m_collection; }

  /// access to the collection itself (const)
  G4VHitsCollection* hits() { return collection(); }

  /// access to the collection itself (const)
  G4VHitsCollection* col() { return collection(); }
  
  /** set new collection pointer 
   *  @param valeu new value of collection pointer 
   */
  GiGaHitsByID& setCollection ( G4VHitsCollection* value ){ 
    m_collection = value; 
    return *this; 
  }
  
  /** set new collection pointer 
   *  @param valeu new value of collection pointer 
   */
  GiGaHitsByID& setHits( G4VHitsCollection* value ) { 
    return setCollection( value ); 
  }
  
  /** set new collection pointer 
   *  @param valeu new value of collection pointer 
   */
  GiGaHitsByID& setCol( G4VHitsCollection* value ) {
    return setCollection( value ); 
  }  
  
private:
  
  /// default constructor is private 
  GiGaHitsByID();
  
private:
  
  int                m_id         ;
  G4VHitsCollection* m_collection ;
  
};


#endif // GIGA_GIGAHITSBYID_H
