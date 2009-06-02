// $Id: CCGrammarTest.cpp,v 1.1 2009-06-02 16:47:35 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
// ============================================================================
// Include files 
// ============================================================================
// STT & STL 
// ============================================================================
#include <iostream>
#include <string>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/StatusCode.h"
// ============================================================================
/** @file
 *  Simple application to test CC-utilities parsers 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-05-12
 */
// ============================================================================
namespace
{
  std::string cc   ( const std::string& input ) 
  { return " [ " + input + " , #" + input + "#] " ; }
  // ==========================================================================
  std::string ccfy ( std::string result  ) 
  {
    
    // Current position at result
    int pos = result.length()-1;
    
    int backpos = 0; // Position from back of string
    int level = 0; // Current nesting level  INSIDE [...]cc construction
    int end=0 ; // Position before the ]cc

    for(;pos>=0;pos--,backpos++){

      if (result[pos]==']'){
        if((backpos>1) && (result[pos+1]=='c') && (result[pos+2]=='c') && (end==0)){
          end = pos-1;
        }else if (end != 0){
          // We count only brackets inside [...]cc construction
          level++;
        }
        continue;
      }
      
      if (result[pos] == '['){
        if (end != 0) { // At the current cycle we also do not interested at [...]cc construction nested at another [...]cc
          if (level == 0) {// The start of [...]cc construction was founded
            // Build new string
            result = result.substr(0,pos)+cc(result.substr(pos+1,end-pos))+result.substr(end+4,result.length()-end+4);
            // Reset positions to the default values. And we start from the end.
            pos = result.length()-1;
            backpos = level = end = 0;
          }else {
            level--;
          }
        }
      }
    }
    return  result ;
  }
  
}

// ============================================================================
int main() 
{
  
  std::string input ;
  std::cout << " Enter the expression " << std::endl ;
  
  while ( std::getline ( std::cin , input ) ) 
  {
    if ( input.empty() ) { break ; }
    
    std::cout << "INPUT   : '" << input          << "'" << std::endl 
              << "RESULT  : '" << ccfy( input )  << "'" << std::endl ;
  }
      
}
// ============================================================================
// The END 
// ============================================================================
