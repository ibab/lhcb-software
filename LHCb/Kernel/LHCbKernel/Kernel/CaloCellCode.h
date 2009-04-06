// $Id: CaloCellCode.h,v 1.7 2009-04-06 15:57:15 odescham Exp $ 
// ============================================================================
#ifndef      KERNEL_CALOCELLCODE_H 
#define      KERNEL_CALOCELLCODE_H 1
// STD and STL 
#include <iostream> 
#include <iomanip> 
#include <string>
#include <functional>
#include <algorithm>
#include <numeric>
#include <cstring>  // for strlen with gcc 4.3

/** @class CaloCellCode CaloCellCode.h Kernel/CaloCellCode.h
 *
 *  Namespace for all code/decode rules of CaloCellID class  
 *
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date    19/02/2001
 */



namespace CaloCellCode{
  enum CaloArea{ Outer = 0,
                 Middle,
                 Inner,
                 PinArea
  };
  static const unsigned int CaloAreaNums =  4;
  static const std::string AreaName[CaloAreaNums] = {"Outer","Middle","Inner","PinArea"};

  //
  /// 32 bits representation
  typedef unsigned int   ContentType     ;  
  //
  static const unsigned int BitsCol     = 6 ; 
  static const unsigned int BitsRow     = 6 ; 
  static const unsigned int BitsArea    = 2 ; 
  static const unsigned int BitsCalo    = 2 ;
  //
  static const unsigned int BitsIndex   =   
  BitsCol + BitsRow + BitsArea ; 
  static const unsigned int BitsAll     = 
  BitsCol + BitsRow + BitsArea + BitsCalo; 
  static const unsigned int BitsTotal   = 
  32 ;
  static const unsigned int BitsRest    = 
  BitsTotal - BitsAll;   
  //
  static const unsigned int ShiftCol    = 0 ; 
  static const unsigned int ShiftRow    = ShiftCol  + BitsCol  ; 
  static const unsigned int ShiftArea   = ShiftRow  + BitsRow  ; 
  static const unsigned int ShiftCalo   = ShiftArea + BitsArea ; 
  //
  static const unsigned int ShiftIndex  = ShiftCol             ;
  static const unsigned int ShiftAll    = ShiftCol             ; 
  static const unsigned int ShiftRest   = ShiftCalo + BitsCalo ; 
  //
  static const ContentType  MaskCol     =  
  ( ( ( (ContentType) 1 ) << BitsCol  ) - 1  ) << ShiftCol  ; 
  static const ContentType  MaskRow     =  
  ( ( ( (ContentType) 1 ) << BitsRow  ) - 1  ) << ShiftRow  ; 
  static const ContentType  MaskArea    =  
  ( ( ( (ContentType) 1 ) << BitsArea ) - 1  ) << ShiftArea ; 
  static const ContentType  MaskCalo    =  
  ( ( ( (ContentType) 1 ) << BitsCalo ) - 1  ) << ShiftCalo ; 
  //
  static const ContentType  MaskIndex   =  
  ( ( ( (ContentType) 1 ) << BitsIndex) - 1  ) << ShiftIndex ; 
  static const ContentType  MaskAll     =  
  ( ( ( (ContentType) 1 ) << BitsAll  ) - 1  ) << ShiftAll   ; 
  static const ContentType  MaskRest    =  
  ( ( ( (ContentType) 1 ) << BitsRest ) - 1  ) << ShiftRest  ; 
  //
  
  /// names of Calorimeter Detectors (to extract the coding of the "calo" )
  /// number of named calorimeters 
  static const unsigned int CaloNums = 4; 
  /// list of names of NAMED calorimeters 
  static const char* const  CaloNames[CaloNums] = 
  { "Spd" , "Prs" , "Ecal" , "Hcal" };
  ///
  class NameMatch: public std::unary_function<const char*,bool> 
  {
  public:
    explicit NameMatch( const char* name ) 
      : str(name), len( strlen( name ) ){};
    bool operator() ( const char* name ) const
    {
      typedef const char* iterator; 
      iterator b1 = str                  ;      
      iterator e1 = str + len            ; 
      iterator b2 = name                 ; 
      iterator e2 = name + strlen( name) ; 
      return 
        std::search( b1 , e1 , b2 , e2 ) != e1 ? true :
        std::search( b2 , e2 , b1 , e1 ) != e2 ? true : false;  
    }
  private:
    const    char* str;  
    unsigned int   len ;
  };
  // return calorimeter number from name, returns -1 for wrong name! 
  static inline int CaloNumFromName( const char* name ) 
  {
    typedef const char* const* iterator;  
    iterator  begin = CaloNames          ;
    iterator  end   = CaloNames+CaloNums ;
    iterator  it    = std::find_if( begin , end , NameMatch( name ) );        
    return end == it ? -1 : (int) ( it - begin) ;
  };
  // return calorimeter number from name, returns -1 for wrong name! 
  static inline int CaloNumFromName( const std::string & name ) 
  { return CaloNumFromName( name.c_str() ); } 
  // return calorimeter names from nums
  static inline const std::string CaloNameFromNum( const int num )
  {
    return 
      ( num <  0              ) ? std::string("????") :
      ( num >= (int) CaloNums ) ? std::string("????") : 
      std::string( CaloNames[num] ) ; 	
  };
  static inline const std::string CaloAreaFromNum( const int calo , const int num){
    if( CaloNameFromNum( calo ) == "Hcal" ){
      if( num == 0 )return AreaName[Outer];
      else if( num == 1 )return AreaName[Inner];
      else if( num == 3 )return AreaName[PinArea];
      else return "????";
    }else if( CaloNameFromNum( calo ) == "Ecal" ){
      if( num == 0 )return      AreaName[Outer];
      else if( num == 1 )return AreaName[Middle];
      else if( num == 2 )return AreaName[Inner];
      else if( num == 3 )return AreaName[PinArea];
      else return "????";
    }else{
      if( num == 0 )return      AreaName[Outer];
      else if( num == 1 )return AreaName[Middle];
      else if( num == 2 )return AreaName[Inner];
      else return "????";
    }
  };


  ///
};

// ============================================================================
// The End 
// ============================================================================
#endif  //   CALOKERNEL_CALOCELLCODE_H
// ============================================================================












