// $Id: CelAutoTaggedCell.h,v 1.10 2009-06-22 13:06:32 cattanem Exp $
// ============================================================================
#ifndef CALOCA_CELAUTOTAGGEDCEL_H
#define CALOCA_CELAUTOTAGGEDCEL_H 1 
// Include files
#include <vector>
// Kernel
#include "GaudiKernel/SystemOfUnits.h"
#include "Kernel/CaloCellID.h"

/** @class CelAutoTaggedCell CelAutoTaggedCell.h
 *
 *  Object Tagged Cell 
 *
 *  @author  Nicole Brun 
 *  @date    27/02/2001
 */

class CelAutoTaggedCell{
  
public:
  
  enum Tag{
      DefaultFlag ,
      Clustered   ,
      Edge        
  };
  
  enum FlagState{
    NotTagged ,
    Tagged    
  } ;
  
public:
  
  // Constructor
  CelAutoTaggedCell (  )
    : m_tag      ( DefaultFlag )
    , m_status   ( NotTagged )
    , m_digit    ( NULL )
    {
      reset();
    }
  
  // Destructor
  ~CelAutoTaggedCell() {}
  
  // Getters 
  const LHCb::CaloDigit*  digit () const { return m_digit              ; }
  const LHCb::CaloCellID& cellID() const { return digit() -> cellID () ; }
  double            e     () const { return digit() -> e      () ; }  
  bool isEdge() const{ return ( ( Tagged == m_status ) && ( Edge == m_tag ) ); }  
  bool isClustered() const{ return ( ( Tagged == m_status ) && ( Clustered == m_tag ) ); }  
  const LHCb::CaloCellID&   seedForClustered() const { return m_seeds[0]; }  
  const std::vector<LHCb::CaloCellID>& seeds() const { return m_seeds; }  
  size_t numberSeeds() const { return m_seeds.size(); }  
  bool isSeed() const{ return ( ( m_seeds.size() != 1 ) ? false : cellID() ==  m_seeds [0] ); }  
  bool isWithSeed ( const LHCb::CaloCellID& seed ){ return m_seeds.end() != std::find( m_seeds.begin() , m_seeds.end() , seed );}
  Tag tag( ) const{return m_tag;}
  FlagState status( ) const{return m_status;}
  
  
  // Setters
  void setIsSeed(){
    m_tag = Clustered;
    m_status = Tagged;
    m_seeds.push_back ( cellID() );
  };
  void setEdge      () { m_tag = Edge  ; } 
  void setClustered () { m_tag = Clustered ; }
  void setStatus    () { if ( ( Edge == m_tag ) || ( Clustered == m_tag ) ) { m_status = Tagged; } }  
  void addSeed ( const LHCb::CaloCellID& seed ) { m_seeds.push_back ( seed ); }  
  
  // operator
  CelAutoTaggedCell& operator=( const LHCb::CaloDigit* digit ){
    reset() ;
    m_digit    = digit ;    
    return *this ;
  };
  
protected:
  
  void reset(){
    m_seeds.clear()        ; 
    m_seeds.reserve( 3 )   ;
    m_tag    = DefaultFlag ;
    m_status = NotTagged   ;
    m_digit  = 0           ;
  };
  
private:
  
  Tag              m_tag      ;
  FlagState        m_status   ;
  const LHCb::CaloDigit* m_digit    ;
  
  // Ident.seed(s)  
  std::vector<LHCb::CaloCellID> m_seeds;

};

// ============================================================================
#endif // CALOCA_CELAUTOTAGGEDCELL_H
// ============================================================================










