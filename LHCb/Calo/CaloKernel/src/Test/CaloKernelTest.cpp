// $Id: CaloKernelTest.cpp,v 1.7 2003-12-09 10:14:42 cattanem Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2002/04/22 16:56:05  ibelyaev
//  move several files to Kernel/LHCbKernel package
//
// Revision 1.5  2002/03/28 12:10:11  ibelyaev
//  minor fix in OS_iterator class
//
// Revision 1.4  2002/03/27 15:06:42  ibelyaev
//  rename OutputStreamIterator to OS_iterator
//
// Revision 1.3  2002/03/26 18:37:06  ibelyaev
//  some restructurisation and map wrappers
//
// Revision 1.2  2002/03/20 15:49:09  ibelyaev
//  add simple adaptor for std::map/HashTable classes
//
// Revision 1.1.1.1  2001/11/25 14:07:38  ibelyaev
// New Package: substitution of the  previous CaloGen package
//
// ============================================================================
// STD & STL 
#include <iostream>
#include <algorithm>
// local 
#include "CaloKernel/CaloCellCode.h"
#include "CaloKernel/CaloCellID.h"
#include "CaloKernel/CaloCollection.h"
#include "CaloKernel/CaloException.h"
#include "CaloKernel/CaloVector.h"
#include "CaloKernel/InputStreamIterator.h"
#include "CaloKernel/OutputStreamIterator.h"

#include "Kernel/CaloMap.h"
#include "Kernel/CaloPrint.h"
#include "Kernel/CaloHashMap.h"

// ============================================================================
/**  @file CaloKernelTest.cpp 
 * 
 * small test program to test some includes from CaloGen package 
 *  
 * @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 * @date 23/06/2001 
 */
// ============================================================================

int main()
{

  ///
  unsigned short icell = 10970;
  CaloCellID cell1;
  CaloCellID cell2( icell) ;
  CaloCellID cell3( 0 , 2, 43 , 26 );
  
  ///
  std::cout 
    << " \tCellID " << cell1 
    << " \tCalo=  " << cell1.calo () 
    << " \tArea=  " << cell1.area () 
    << std::endl 
    << " \t\t" 
    << " \tRow=   " << cell1.row  () 
    << " \tCol=   " << cell1.col  () 
    << " \tRaw=   " << cell1.raw  ()
    << " \tBits=  " << cell1.bits ()
    << " \tSize of all/raw = " 
    << sizeof( cell1       ) 
    << "/" 
    << sizeof( cell1.raw() ) 
    << std::endl ;

  std::cout
    << " \tCellID " << cell2 
    << " \tCalo=  " << cell2.calo () 
    << " \tArea=  " << cell2.area () 
    << std::endl 
    << " \t\t" 
    << " \tRow=   " << cell2.row  () 
    << " \tCol=   " << cell2.col  () 
    << " \tRaw=   " << cell2.raw  ()
    << " \tBits=  " << cell2.bits () 
    << " \tSize of all/raw = " 
    << sizeof( cell2       ) 
    << "/" 
    << sizeof( cell2.raw() ) 
    << std::endl ;

  std::cout
    << " \tCellID " << cell3 
    << " \tCalo=  " << cell3.calo () 
    << " \tArea=  " << cell3.area () 
    << std::endl 
    << " \t\t" 
    << " \tRow=   " << cell3.row  () 
    << " \tCol=   " << cell3.col  () 
    << " \tRaw=   " << cell3.raw  ()
    << " \tBits=  " << cell3.bits () 
    << " \tSize of all/raw = " 
    << sizeof( cell3       ) 
    << "/" 
    << sizeof( cell3.raw() ) 
    << std::endl ;
  
  {
    CaloPrint print;
    std::cout << " Test Of CaloPrint " << std::endl 
              << "\t double  ='" << print( 1./ 3. ) << "'" << std::endl 
              << "\t int     ='" << print( 1      ) << "'" << std::endl ;
  }
  
  {    
    CaloMap<int,std::string>     mm;
    
    mm[1]   = "value of 1st element" ;
    mm[100] = "value of 100th element" ;
    
    std::cout << " Test of CaloMap      "            << std::endl 
              << "\t mm[1]   ='" << mm[1]    << "' " << std::endl 
              << "\t mm[100] ='" << mm[100]  << "' " << std::endl
              << "\t mm[50]  ='" << mm[50]   << "' " << std::endl;
    
  }
  
  {
    CaloHashMap<int,std::string> hm ;
    
    hm[1]       = "value of 1st element" ;
    hm[1000000] = "value of 1000000th element" ;
    
    std::cout << " Test of CaloHashMap  "                   << std::endl 
              << "\t mm[1]       ='" << hm[1]       << "' " << std::endl 
              << "\t mm[1000000] ='" << hm[1000000] << "' " << std::endl
              << "\t mm[50]      ='" << hm[50]      << "' " << std::endl; 
  };

  
  {
    
      std::vector<double> vint(10,  1.0e+10 / 3.0  ) ;
    
      
    std::cout << "Test #1 of output stream iterators " << std::endl ;
    
    std::copy( vint.begin () ,
               vint.end   () , 
               OS_iterator<double,std::ostream> ( std::cout ) );
    
    std::cout << std::endl ;
    std::cout << "end of Test #1 of output stream iterators " << std::endl ;
    
    std::cout << "Test #2 of output stream iterators " << std::endl ;
    
    std::copy( vint.begin () ,
               vint.end   () , 
               OS_iterator<double,std::ostream>( std::cerr , 
                                                          std::endl ) );

    std::cout << std::endl ;
    std::cout << "end of Test #2 of output stream iterators " << std::endl ;
    
    std::cout << "Test #3 of output stream iterators " << std::endl ;
    
    std::copy( vint.begin () ,
               vint.end   () , 
               OS_iterator<double,std::ostream> 
               ( std::cout , " ljshfdalf" , std::endl ) );
    
    std::cout << std::endl ;
    std::cout << "end of Test #3 of output stream iterators " << std::endl ;
  };
  
  try
    {
      throw CaloException("This is an exception", StatusCode::FAILURE );
    }
  catch(...)
    {}
  ///
  return 0;
  ///
};

// ============================================================================

