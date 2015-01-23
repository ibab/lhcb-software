// ============================================================================
// Include files
// ============================================================================
#include <type_traits>
// ============================================================================
// LHCbKernel
// ============================================================================
#include "Kernel/CaloCellCode.h"
#include "Kernel/CaloCellID.h"
// ============================================================================
/** @file
 *  trivial assertion for CaloCellCode::ContentType size
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-09-29
 */
// ============================================================================
namespace CaloCellCode
{

  const CALONAMES CaloNames = { { "Spd" , "Prs", "Ecal" , "Hcal" } } ;
  const std::string SpdName   = CaloNames[SpdCalo]  ;                   //  Spd
  const std::string PrsName   = CaloNames[PrsCalo]  ;                   //  Prs
  const std::string EcalName  = CaloNames[EcalCalo] ;                   // Ecal
  const std::string HcalName  = CaloNames[HcalCalo] ;                   // Hcal

  // ==========================================================================
  /** @enum CaloArea
   *  The actual enumeration for the calorimeter areas
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2009-09-28
   */
  enum CaloAreas
    {
      Outer        = 0 ,
      Middle           ,  // NB: Inner for Hcal
      Inner            ,
      /// total number of Calorimeter areas
      CaloAreaNums = 4                     // total number of Calorimeter areas
   } ;
  // ==========================================================================
} //                        end of "the private part" of namespace CaloCellCode
// ============================================================================
namespace
{
  // ==========================================================================
  /** @typedef AREANMES
   *  the actual type for array of calorimeter areas
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2009-09-28
   */
  typedef boost::array<std::string,CaloCellCode::CaloAreaNums>  AREANAMES ;
  // ==========================================================================
  /** @var AreaName
   *  The array of calorimeter area names
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2009-09-28
   */
  const AREANAMES s_AreaName = { {  "Outer" , "Middle" , "Inner" ,"PinArea" } } ;
  // ==========================================================================
  /// get the area index from name
  inline int _area  ( const std::string& area )
  {
    AREANAMES::const_iterator it =
      std::find ( s_AreaName.begin() ,
                  s_AreaName.end  () , area ) ;
    return s_AreaName.end() == it ? -1 : ( it - s_AreaName.begin() );
  }
  // ==========================================================================
}
// ============================================================================

namespace
{
  // ==========================================================================
  typedef CaloCellCode::ContentType     _Type1 ;
  typedef LHCb::CaloCellID::ContentType _Type2 ;
  // ==========================================================================
}

static_assert ( std::numeric_limits<_Type1>::is_specialized &&
                std::is_integral<_Type1>   ::value    &&
                std::numeric_limits<_Type1>::is_integer     &&
                std::numeric_limits<_Type1>::digits == 32u  &&
                !std::is_signed<_Type1>    ::value     , "_Type1 not valid" ) ;

static_assert ( std::numeric_limits<_Type2>::is_specialized &&
                std::is_integral<_Type2>   ::value    &&
                std::numeric_limits<_Type1>::is_integer     &&
                std::numeric_limits<_Type2>::digits == 32u  &&
                !std::is_signed<_Type1>    ::value     , "_Type2 not valid" ) ;

static_assert ( CaloCellCode::BitsTotal == 32 , "32 bits");

static_assert ( CaloCellCode::SpdCalo  < CaloCellCode::CaloNums     &&
                      CaloCellCode::PrsCalo  < CaloCellCode::CaloNums     &&
                      CaloCellCode::EcalCalo < CaloCellCode::CaloNums     &&
                      CaloCellCode::HcalCalo < CaloCellCode::CaloNums  , "Det < CaloNums"   ) ;

static_assert ( CaloCellCode::Outer    < CaloCellCode::CaloAreaNums &&
                CaloCellCode::Middle   < CaloCellCode::CaloAreaNums &&
                CaloCellCode::Inner    < CaloCellCode::CaloAreaNums , "Region < AreaNums");

static_assert ( (unsigned int) CaloCellCode::PinArea      <
                (unsigned int) CaloCellCode::CaloAreaNums , "PinArea < CaloAreaNums") ;

// ============================================================================
/* get the area name from calorimeter index and number
 * @attention function make heavy use of hadcoded structure of Calorimeter!
 * @warning   different convention for Hcal
 * @param  calo (INPUT) calorimeter index
 * @param  area (INPUT) area index
 * @return name for the area
 * @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 * @date 2009-09-28
 */
// ============================================================================
const std::string& CaloCellCode::caloArea ( const int calo , const int area )
{
  switch ( calo )
  {
  case SpdCalo :
  case PrsCalo :
    switch ( area )
    {
    case 0 : return s_AreaName[ Outer  ] ; // Outer
    case 1 : return s_AreaName[ Middle ] ; // Middle
    case 2 : return s_AreaName[ Inner  ] ; // Inner
    default: return s_BadName          ;
    }
  case EcalCalo :
    switch ( area )
    {
    case 0 : return s_AreaName[ Outer   ] ; // Outer
    case 1 : return s_AreaName[ Middle  ] ; // Middle
    case 2 : return s_AreaName[ Inner   ] ; // Inner
    case 3 : return s_AreaName[ PinArea ] ; // Pin-Area
    default: return s_BadName             ;
    }
  case HcalCalo :
    switch ( area )
    {
    case 0 : return s_AreaName[ Outer   ] ; // Outer
    case 1 : return s_AreaName[ Inner   ] ; // Middle, return as Inner ! ATTENTION!
    case 3 : return s_AreaName[ PinArea ] ; // Pin-Area
    default: return s_BadName             ;
    }
  default:
    return s_BadName ;
  }
}
// ============================================================================
/*  get the area index from calorimeter index and name
 *  @attention function make heavy use of hadcoded structure of Calorimeter!
 *  @warning   the different convention for Hcal
 *  @param  calo (INPUT) calorimeter index
 *  @param  area (INPUT) area name
 *  @return indx for the area
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-09-28
 */
// ============================================================================
int CaloCellCode::caloArea ( const int calo , const std::string& area )
{
  //
  switch  ( calo )
  {
  case HcalCalo :
    switch ( _area ( area ) )
    {
    case 0 : return  Outer    ; // 0
    case 1 : return -1        ;             // ATTENTION!!!
    case 2 : return  Middle   ; // 1        // ATTENTION!!!
    case 3 : return  PinArea  ; // 3
      default: return -1        ;
    }
  case EcalCalo :
    return _area ( area ) ;
  case SpdCalo :
  case PrsCalo :
    switch ( _area ( area )  )
    {
    case 0 : return  Outer    ; // 0
    case 1 : return  Middle   ; // 1
    case 2 : return  Inner    ; // 2
    default: return -1        ;
    }
  default:
    return -1 ;
  }
}
// ============================================================================
/*  Is the given area is Pin-diod area ?
 *  @attention It must be coherent with CaloCellCode::caloArea
 *  @see CaloCellCode::caloArea
 *  @param calo (INPUT) calorimeetr index
 *  @param area (INPUT) calorimeter index
 *  @reutrn true if the area is Pin-diod area
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-09-28
 */
// ============================================================================
bool CaloCellCode::isPinArea ( const int calo , const int area )
{
  return
    (int) PinArea == area && ( (int) EcalCalo == calo || (int) HcalCalo == calo ) ;
}
// ============================================================================
// The END
// ============================================================================
