// $Id: CaloCellCode.h,v 1.12 2009-10-25 14:35:32 ibelyaev Exp $ 
// ============================================================================
#ifndef      KERNEL_CALOCELLCODE_H 
#define      KERNEL_CALOCELLCODE_H 1
// ============================================================================
// Include files
// ============================================================================
// STD and STL 
// ============================================================================
#include <string>
#include <vector>
#include <set>
#include <algorithm>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
// Boost
// ============================================================================
#ifdef __INTEL_COMPILER        // Disable ICC warning from Boost
  #pragma warning(disable:279) // controlling expression is constant
#endif
#include "boost/array.hpp"
// ============================================================================
/** @class CaloCellCode CaloCellCode.h Kernel/CaloCellCode.h
 *
 *  Namespace for all code/decode rules of CaloCellID class  
 *
 *  @todo CaloCellCode.h: switch from C-string to std::string
 *  @todo CaloCellCode.h: make code more robust & CPU-efficient
 *  @todo CaloCellCode.h: remove many redundant & confusing "static"
 *  @todo CaloCellCode.h: return by reference 
 *
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date    19/02/2001
 */
// ============================================================================
/** @namespace CaloCellCode 
 *
 *  Namespace for all code/decode rules of CaloCellID class  
 *
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date    19/02/2001
 */
namespace CaloCellCode
{
  // ==========================================================================
  /** @var s_BadName 
   *  representation of "bad-name"
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2009-09-28
   */
  const std::string s_BadName = "????" ;
  // ==========================================================================
  /** @typedef ContentType 
   *  the actual type for 32 bits representation of internal data 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2009-09-28
   */
  typedef unsigned int   ContentType     ;  
  // ==========================================================================
  // Coding of CaloCellID objects : Lengths, basic  
  // ==========================================================================
  const unsigned int BitsCol     = 6 ; 
  const unsigned int BitsRow     = 6 ; 
  const unsigned int BitsArea    = 2 ; 
  const unsigned int BitsCalo    = 2 ;
  // ==========================================================================
  // Coding of CaloCellID objects : Lengths, extra
  // ==========================================================================
  const unsigned int BitsIndex   = BitsCol + BitsRow + BitsArea ; 
  const unsigned int BitsAll     = BitsCol + BitsRow + BitsArea + BitsCalo; 
  const unsigned int BitsTotal   = 32 ;
  const unsigned int BitsRest    = BitsTotal - BitsAll;   
  // ==========================================================================
  // Coding of CaloCellID objects : Shifts, basic 
  // ==========================================================================
  const unsigned int ShiftCol    = 0 ; 
  const unsigned int ShiftRow    = ShiftCol  + BitsCol  ; 
  const unsigned int ShiftArea   = ShiftRow  + BitsRow  ; 
  const unsigned int ShiftCalo   = ShiftArea + BitsArea ; 
  // ==========================================================================
  // Coding of CaloCellID objects : Shifts, extra
  // ==========================================================================
  const unsigned int ShiftIndex  = ShiftCol             ;
  const unsigned int ShiftAll    = ShiftCol             ; 
  const unsigned int ShiftRest   = ShiftCalo + BitsCalo ; 
  // ==========================================================================
  // Coding of CaloCellID objects : Masks, basic 
  // ==========================================================================
  const ContentType  MaskCol     = ( ( ( (ContentType) 1 ) << BitsCol  ) - 1  ) << ShiftCol  ; 
  const ContentType  MaskRow     = ( ( ( (ContentType) 1 ) << BitsRow  ) - 1  ) << ShiftRow  ; 
  const ContentType  MaskArea    = ( ( ( (ContentType) 1 ) << BitsArea ) - 1  ) << ShiftArea ; 
  const ContentType  MaskCalo    = ( ( ( (ContentType) 1 ) << BitsCalo ) - 1  ) << ShiftCalo ; 
  // ==========================================================================
  // Coding of CaloCellID objects : Masks, extra
  // ==========================================================================
  const ContentType  MaskIndex   = ( ( ( (ContentType) 1 ) << BitsIndex) - 1  ) << ShiftIndex ; 
  const ContentType  MaskAll     = ( ( ( (ContentType) 1 ) << BitsAll  ) - 1  ) << ShiftAll   ; 
  const ContentType  MaskRest    = ( ( ( (ContentType) 1 ) << BitsRest ) - 1  ) << ShiftRest  ; 
  // ==========================================================================
  /** set the active fields 
   *  @param Where (INPUT) the initial value 
   *  @param Value (INPUT) value to be set 
   *  @param Shift (INPUT) shift 
   *  @param Mask  (INPUT) mask 
   *  @return new Field 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl  
   *  @date 2009-09-28
   */
  inline ContentType setField 
  ( const ContentType  Field , 
    const ContentType  Value , 
    const ContentType  Shift , 
    const ContentType  Mask  ) 
  {
    ContentType tmp1 = ( Value << Shift ) &  Mask ;
    ContentType tmp2 =   Field            & ~Mask ;
    return tmp1 | tmp2 ;
  }
  // ==========================================================================
  /** @enum CaloIndex 
   *  the indices for  Calorimeter Detectors (to extract the coding of the "calo" )
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2009-09-28
   */
  enum CaloIndex 
    {
      SpdCalo      = 0 , 
      PrsCalo          , 
      EcalCalo         , 
      HcalCalo         ,
      /// total number of NAMED Calorimeters      
      CaloNums     = 4                   // total number of NAMED Calorimeters 
    } ;
  // ==========================================================================
  /** @typedef CALONAMES 
   *  the actual type for the container of Calorimeetr names 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2009-09-28
   */
  typedef boost::array<std::string,CaloNums>  CALONAMES ;
  // ==========================================================================
  /** @var CaloNames
   *  The actual list of Calorimter names 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2009-09-28
   */
  const CALONAMES CaloNames = { { "Spd" , "Prs", "Ecal" , "Hcal" } } ;
  // ==========================================================================
  /// the calorimeter names:
  const std::string SpdName   = CaloNames[SpdCalo]  ;                   //  Spd 
  const std::string PrsName   = CaloNames[PrsCalo]  ;                   //  Prs 
  const std::string EcalName  = CaloNames[EcalCalo] ;                   // Ecal
  const std::string HcalName  = CaloNames[HcalCalo] ;                   // Hcal 
  // ==========================================================================
  /** simple function to get the calorimeter name from number 
   *  @param num (INPUT) calorimeter index 
   *  @return calorimeter name 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2009-09-28
   */
  inline const std::string& caloName ( const int num ) 
  {
    return 
      num < 0        ? s_BadName      :
      num < CaloNums ? CaloNames[num] : s_BadName ;
  }
  // ==========================================================================
  /** simple function to get the calorimeter name from number 
   *  @param num (INPUT) calorimeter index 
   *  @return calorimeter name 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2009-09-28
   */
  inline const std::string& CaloNameFromNum ( const int num ) 
  { return caloName ( num ) ; }
  // ==========================================================================
  /** get the calorimeter index from name, returns -1 for wrong name! 
   *  @param name (INPUT) the calorimeter name (can be long string)
   *  @return calorimeter index from name, returns -1 for wrong name! 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2009-09-28
   */
  inline int caloNum ( const std::string& name )
  {
    typedef CALONAMES::const_iterator iterator ;
    iterator begin = CaloNames.begin () ;
    iterator end   = CaloNames.end   () ;
    for ( iterator current = begin ; current != end ; ++current ) 
    { if ( current->size()   <= name.size() && 
           std::string::npos != name.find ( *current ) ) 
    { return current - begin ; } }
    //
    return -1 ;                                                       // RETURN 
  }
  // ==========================================================================
  /** get the calorimeter index from name, returns -1 for wrong name! 
   *  @param first (INPUT)  begin of the sequence 
   *  @param last  (INPUT)  end   of the sequence 
   *  @return calorimeter index from name, returns -1 for wrong name! 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2009-09-28
   */
  template <class ITERATOR>
  inline int caloNum ( ITERATOR first , ITERATOR last ) 
  {
    typedef CALONAMES::const_iterator iterator ;
    iterator begin   = CaloNames.begin () ;
    iterator end     = CaloNames.end   () ;
    std::size_t size = std::distance ( first , last ) ;
    for ( iterator current = begin ; current != end ; ++current ) 
    {
      if ( current->size() <= size && 
           last != std::search 
           ( first , last , current->begin() , current->end() ) ) 
      { return current - begin ; }                                  // RETURN 
    }
    //
    return -1 ;                                                       // RETURN 
  }
  // ==========================================================================
  /** get the calorimeter index from name, returns -1 for wrong name! 
   *  @paran name (INPUT) the calorimeter name (can be long string)
   *  @return calorimeter index from name, returns -1 for wrong name! 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2009-09-28
   */
  template <unsigned N>
  inline int caloNum ( const char(&name)[N] ) 
  { return caloNum ( name , name + N ) ; }
  // =========================================================================
  /** get the calorimeter index from name, returns -1 for wrong name! 
   *  @paran name (INPUT) the calorimeter name (can be long string)
   *  @return calorimeter index from name, returns -1 for wrong name! 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2009-09-28
   */
  template <unsigned N>
  inline int caloNum ( char(&name)[N] )
  { return caloNum ( name , name + N ) ; }
  // =========================================================================
  /** get the calorimeter index from name, returns -1 for wrong name! 
   *  @paran name (INPUT) the calorimeter name (can be long string)
   *  @return calorimeter index from name, returns -1 for wrong name! 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2009-09-28
   */
  inline int CaloNumFromName( const std::string & name ) 
  { return caloNum ( name ); } 
  // ==========================================================================
  /** get the calorimeter index from name, returns -1 for wrong name! 
   *  @param name (INPUT) the calorimeter name (can be long string)
   *  @return calorimeter index from name, returns -1 for wrong name! 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2009-09-28
   */
  template <unsigned N>
  inline int CaloNumFromName( const char (&name)[N] ) { return caloNum ( name ); } 
  // ==========================================================================
  /** get the calorimeter index from name, returns -1 for wrong name! 
   *  @param name (INPUT) the calorimeter name (can be long string)
   *  @return calorimeter index from name, returns -1 for wrong name! 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2009-09-28
   */
  template <unsigned N>
  inline int CaloNumFromName(       char (&name)[N] ) { return caloNum ( name ); }
  // ==========================================================================
  /** get the area name from calorimeter index and number
   *  @attention function make heavy use of hadcoded structure of Calorimeter! 
   *  @warning   the different convention for Hcal
   *  @param  calo (INPUT) calorimeter index 
   *  @param  area (INPUT) area index 
   *  @return name for the area 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2009-09-28
   */
  GAUDI_API 
  const std::string& caloArea ( const int calo , const int area )  ;
  // =========================================================================
  /** get the area index from calorimeter index and name 
   *  @attention function make heavy use of hadcoded structure of Calorimeter! 
   *  @warning   the different convention for Hcal
   *  @param  calo (INPUT) calorimeter index 
   *  @param  area (INPUT) area name 
   *  @return indx for the area 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2009-09-28
   */
  GAUDI_API 
  int caloArea ( const int calo , const std::string& area )  ;
  // ==========================================================================
  /** Is the given area is Pin-diod area ?
   *  @attention It must be coherent with CaloCellCode::caloArea
   *  @see CaloCellCode::caloArea 
   *  @param calo (INPUT) calorimeetr index 
   *  @param area (INPUT) calorimeter index 
   *  @reutrn true if the area is Pin-diod area 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2009-09-28   
   */
   GAUDI_API 
   bool  isPinArea ( const int calo , const int area ) ;
  // ==========================================================================
  /** @enum CaloArea
   *  The actual enumeration for the calorimeter areas 
   *  @attention some code lines explicitly rely on the actual value of PinArea, 
   *             therefore this symbol is the pubilc one.
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2009-09-28
   */   
  enum CaloArea 
    { 
      // ======================================================================
      /// some code lines explicitly rely on the alue of PinArea
      PinArea      = 3         // some code lines explicitly rely on this value 
      // ======================================================================
    } ;
  // ==========================================================================
} //                                                end of namespace CaloCellID 

// ============================================================================
// The End 
// ============================================================================
#endif  //   CALOKERNEL_CALOCELLCODE_H
// ============================================================================
