#ifndef     GIGA_SPLITNAME_H
#define     GIGA_SPLITNAME_H 1 


#include <string>
#include <algorithm>

#include "GaudiKernel/StatusCode.h" 


///
/// split "Type/Name" into "Type" and "Name"
static inline StatusCode SplitTypeAndName ( const std::string& TypeAndName ,
					    std::string      & Type        , 
					    std::string      & Name        )
{
  /// clear output strings
  Type.erase();
  Name.erase();
  ///
  if( TypeAndName.empty() ) { return StatusCode::FAILURE ; }
  ///
  std::string::const_iterator it = std::find( TypeAndName.begin() , 
					      TypeAndName.end  () , '/' );
  ///
  if( TypeAndName.end() == it )
    {
      Type = TypeAndName ; 
      Name = Type        ; 
      return StatusCode::SUCCESS;                        /// RETURN !!!
    }
  ///
  Type = std::string( TypeAndName.begin() , it                ) ;
  if( Type.empty() )      { return StatusCode::FAILURE; } /// RETURN  !!! 
  Name = std::string( ++it                , TypeAndName.end() ) ;
  if( Name.empty() ) { Name = Type; } 
  it = std::find( Name.begin() , Name.end() , '/' );
  if( Name.end() == it )  { return StatusCode::SUCCESS; } /// RETURN
  Type.erase() ; 
  Name.erase() ; 
  ///
  return StatusCode::FAILURE; 
  ///
} 


#endif  //  GIGA_SPLITNAME_H
