// $Id: LHCbKernelTest.cpp,v 1.2 2002-05-10 12:42:39 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/05/10 12:30:09  ibelyaev
//  see $LHCBKERNELROOT/doc/release.notes 10 May 2002
//
// ============================================================================
// Include files
// LHCbKernel
#include "Relations/TypeConversion.h"
#include "Relations/TypeSelect.h"
#include "Relations/TypePersistent.h"
#include "Relations/ObjectTypeTraits.h"
#include "Relations/Relation1D.h"
#include "Relations/RelationWeighted1D.h"
// STD & STL 
#include <iostream>
#include <string>
// CLHEP
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Matrix/SymMatrix.h"
// Gaudi Kernel
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/System.h"

/** @file
 *  Helpful file to test some properties of Relations sub-package
 *  @author Vanya Belyaev
 *  @date   9 May 2002
 */
int main()
{

  typedef ContainedObject CO;
  typedef DataObject      DO;

  std::string stars( 80 , '*' );

  std::cout << stars                         << std::endl;
  std::cout << " Test for 'convertability' " << std::endl;
  std::cout << stars                         << std::endl;

  std::cout << " 'Conversion' from double to int         "
            << (bool) Relations::TypeConversion<double,int>::exists
            << " (should be " << ( 1 < 2 ) << ")" << std::endl ;

  std::cout << " 'Conversion' from char to unsigned long "
            << (bool) Relations::TypeConversion<char,unsigned long>::exists
            << " (should be " << ( 1 < 2 ) << ")" << std::endl ;

  std::cout << " 'Conversion' from double to long double "
            << (bool) Relations::TypeConversion<double,long double>::exists
            << " (should be " << ( 1 < 2 ) << ")" << std::endl ;

  std::cout << " 'Conversion' from int    to double      "
            << (bool) Relations::TypeConversion<int,double>::exists
            << " (should be " << ( 1 < 2 ) << ")" << std::endl ;

  std::cout << " 'Conversion' from CO     to CO          "
            << (bool) Relations::TypeConversion<CO*,CO*>::exists
            << " (should be " << ( 1 < 2 ) << ")" << std::endl ;

  std::cout << " 'Conversion' from CO     to DO          "
            << (bool) Relations::TypeConversion<CO*,DO*>::exists
            << " (should be " << ( 1 > 2 ) << ")" << std::endl ;

  std::cout << " 'Conversion' from DO     to DO          "
            << (bool) Relations::TypeConversion<DO*,DO*>::exists
            << " (should be " << ( 1 < 2 ) << ")" << std::endl ;

  std::cout << " 'Conversion' from DO     to CO          "
            << (bool) Relations::TypeConversion<DO*,CO*>::exists
            << " (should be " << ( 1 > 2 ) << ")" << std::endl ;

  std::cout << " 'Conversion' from DO     to int         "
            << (bool) Relations::TypeConversion<DO*,int*>::exists
            << " (should be " << ( 1 > 2 ) << ")" << std::endl ;

  std::cout << " 'Conversion' from char   to CO          "
            << (bool) Relations::TypeConversion<char,CO*>::exists
            << " (should be " << ( 1 > 2 ) << ")" << std::endl ;

  std::cout << stars                       << std::endl;
  std::cout << " Test for type selection " << std::endl;
  std::cout << stars                       << std::endl;
  
  {
    typedef Relations::TypeSelect<true,int,double>::Result Result;
    std::cout << " 'First'  Type from (int,double) is "          << std::endl
              << " \t'" << System::typeinfoName( typeid( Result ) ) 
              << "'"                                             << std::endl 
              << " \t(should be 'int')"                          << std::endl;
  }
  
  {
    typedef Relations::TypeSelect<false,int,double>::Result Result;
    std::cout << " 'Second' Type from (int,double) is "          << std::endl
              << " \t'" << System::typeinfoName( typeid( Result ) ) 
              << "'"                                             << std::endl 
              << " \t(should be 'double')"                       << std::endl;
  }
  
  {
    typedef Relations::TypeSelect<true,CO,double>::Result Result;
    std::cout << " 'First'  Type from (CO,double) is "           << std::endl
              << " \t'" << System::typeinfoName( typeid( Result ) ) 
              << "'"                                             << std::endl 
              << " \t(should be 'ContainedObject')"              << std::endl;
  }
  
  {
    typedef Relations::TypeSelect<false,CO,DO>::Result Result;
    std::cout << " 'Second'  Type from (CO,DO) is "            << std::endl
              << " \t'" << System::typeinfoName( typeid( Result ) ) 
              << "'"                                           << std::endl 
              << " \t(should be 'DataObject')"                 << std::endl;
  }
  
  
  std::cout 
    << stars                                               << std::endl;
  std::cout 
    << " Test for persistent representation of the types " << std::endl;
  std::cout 
    << stars                                               << std::endl;
  
  {
    typedef DO                               Type           ;
    typedef Relations::TypePersistent<Type>::Result Result  ;
    std::cout << " 'Persistent' representation of type " << std::endl
              << " \t'"    << System::typeinfoName( typeid( Type   ) ) 
              << "'"                                     << std::endl 
              << " \tis '" << System::typeinfoName( typeid( Result ) ) 
              << "'"                                     << std::endl 
              << " \t(should be 'SmartRef<DataObject>')" << std::endl;
  }

  {
    typedef HepLorentzVector                 Type           ;
    typedef Relations::TypePersistent<Type>::Result Result  ;
    std::cout << " 'Persistent' representation of type " << std::endl
              << " \t'"    << System::typeinfoName( typeid( Type   ) ) 
              << "'"                                     << std::endl 
              << " \tis '" << System::typeinfoName( typeid( Result ) ) 
              << "'"                                     << std::endl 
              << " \t(should be 'HepLorentzVector')"    << std::endl;
  }

  {
    typedef unsigned char                    Type           ;
    typedef Relations::TypePersistent<Type>::Result Result  ;
    std::cout << " 'Persistent' representation of type " << std::endl
              << " \t'"    << System::typeinfoName( typeid( Type   ) ) 
              << "'"                                     << std::endl 
              << " \tis '" << System::typeinfoName( typeid( Result ) ) 
              << "'"                                     << std::endl 
              << " \t(should be 'unsigned char')"        << std::endl;
  }

  {
    typedef CO                                      Type           ;
    typedef Relations::TypePersistent<Type>::Result Result  ;
    std::cout << " 'Persistent' representation of type " << std::endl
              << " \t'"    << System::typeinfoName( typeid( Type   ) ) 
              << "'"                                     << std::endl 
              << " \tis '" << System::typeinfoName( typeid( Result ) ) 
              << "'"                                     << std::endl 
              << " \t(should be 'SmartRef<ContainedObject>')" << std::endl;
  }


  std::cout << stars                          << std::endl;
  std::cout << " Test for 'ObjectTypeTraits'" << std::endl;
  std::cout << stars                          << std::endl;
  
  {
    typedef CO                                Type    ;
    typedef Relations::ObjectTypeTraits<Type> Traits ;
    std::cout << " Types for type \t'"
              << System::typeinfoName( typeid( Type   ) ) 
              << "'"                                 << std::endl 
              << " \t'TYPE'       = \t'"      
              << System::typeinfoName( typeid( Traits::TYPE        ) ) 
              << "'"                                 << std::endl
              << " \t'Traits'     = \t'"      
              << System::typeinfoName( typeid( Traits::Traits      ) ) 
              << "'"                                 << std::endl 
              << " \t'Type'       = \t'"      
              << System::typeinfoName( typeid( Traits::Type        ) ) 
              << "'"                                 << std::endl 
              << " \t'Serializer' = \t'"      
              << System::typeinfoName( typeid( Traits::Serializer  ) ) 
              << "'"                                 << std::endl
              << " \t'Apply'      = \t'"      
              << System::typeinfoName( typeid( Traits::Apply       ) ) 
              << "'"                                 << std::endl
              << " \t'Less'       = \t'"      
              << System::typeinfoName( typeid( Traits::Less        ) ) 
              << "'"                                 << std::endl
              << " \t'Id/CLID'    = \t'"      
              << Traits::id() 
              << "'"                                 << std::endl  ;
  }
  
  {
    typedef DO                               Type    ;
    typedef Relations::ObjectTypeTraits<Type> Traits ;
    std::cout << " Types for type \t'"
              << System::typeinfoName( typeid( Type   ) ) 
              << "'"                                 << std::endl 
              << " \t'TYPE'       = \t'"      
              << System::typeinfoName( typeid( Traits::TYPE        ) ) 
              << "'"                                 << std::endl
              << " \t'Traits'     = \t'"      
              << System::typeinfoName( typeid( Traits::Traits      ) ) 
              << "'"                                 << std::endl 
              << " \t'Type'       = \t'"      
              << System::typeinfoName( typeid( Traits::Type        ) ) 
              << "'"                                 << std::endl 
              << " \t'Serializer' = \t'"      
              << System::typeinfoName( typeid( Traits::Serializer  ) ) 
              << "'"                                 << std::endl
              << " \t'Apply'      = \t'"      
              << System::typeinfoName( typeid( Traits::Apply       ) ) 
              << "'"                                 << std::endl
              << " \t'Less'       = \t'"      
              << System::typeinfoName( typeid( Traits::Less        ) ) 
              << "'"                                 << std::endl
              << " \t'Id/CLID'    = \t'"      
              << Traits::id() 
              << "'"                                 << std::endl  ;
  }
  

  {
    typedef unsigned long                    Type    ;
    typedef Relations::ObjectTypeTraits<Type> Traits ;
    std::cout << " Types for type \t'"
              << System::typeinfoName( typeid( Type   ) ) 
              << "'"                                 << std::endl 
              << " \t'TYPE'       = \t'"      
              << System::typeinfoName( typeid( Traits::TYPE        ) ) 
              << "'"                                 << std::endl
              << " \t'Traits'     = \t'"      
              << System::typeinfoName( typeid( Traits::Traits      ) ) 
              << "'"                                 << std::endl 
              << " \t'Type'       = \t'"      
              << System::typeinfoName( typeid( Traits::Type        ) ) 
              << "'"                                 << std::endl 
              << " \t'Serializer' = \t'"      
              << System::typeinfoName( typeid( Traits::Serializer  ) ) 
              << "'"                                 << std::endl
              << " \t'Apply'      = \t'"      
              << System::typeinfoName( typeid( Traits::Apply       ) ) 
              << "'"                                 << std::endl
              << " \t'Less'       = \t'"      
              << System::typeinfoName( typeid( Traits::Less        ) ) 
              << "'"                                 << std::endl
              << " \t'Id/CLID'    = \t'"      
              << Traits::id() 
              << "'"                                 << std::endl  ;
  }

  {
    typedef double                           Type    ;
    typedef Relations::ObjectTypeTraits<Type> Traits ;
    std::cout << " Types for type \t'"
              << System::typeinfoName( typeid( Type   ) ) 
              << "'"                                 << std::endl 
              << " \t'TYPE'       = \t'"      
              << System::typeinfoName( typeid( Traits::TYPE        ) ) 
              << "'"                                 << std::endl
              << " \t'Traits'     = \t'"      
              << System::typeinfoName( typeid( Traits::Traits      ) ) 
              << "'"                                 << std::endl 
              << " \t'Type'       = \t'"      
              << System::typeinfoName( typeid( Traits::Type        ) ) 
              << "'"                                 << std::endl 
              << " \t'Serializer' = \t'"      
              << System::typeinfoName( typeid( Traits::Serializer  ) ) 
              << "'"                                 << std::endl
              << " \t'Apply'      = \t'"      
              << System::typeinfoName( typeid( Traits::Apply       ) ) 
              << "'"                                 << std::endl
              << " \t'Less'       = \t'"      
              << System::typeinfoName( typeid( Traits::Less        ) ) 
              << "'"                                 << std::endl
              << " \t'Id/CLID'    = \t'"      
              << Traits::id() 
              << "'"                                 << std::endl  ;
  }
  

  {
    typedef HepSymMatrix                     Type    ;
    typedef Relations::ObjectTypeTraits<Type> Traits ;
    std::cout << " Types for type \t'"
              << System::typeinfoName( typeid( Type   ) ) 
              << "'"                                 << std::endl 
              << " \t'TYPE'       = \t'"      
              << System::typeinfoName( typeid( Traits::TYPE        ) ) 
              << "'"                                 << std::endl
              << " \t'Traits'     = \t'"      
              << System::typeinfoName( typeid( Traits::Traits      ) ) 
              << "'"                                 << std::endl 
              << " \t'Type'       = \t'"      
              << System::typeinfoName( typeid( Traits::Type        ) ) 
              << "'"                                 << std::endl 
              << " \t'Serializer' = \t'"      
              << System::typeinfoName( typeid( Traits::Serializer  ) ) 
              << "'"                                 << std::endl
              << " \t'Apply'      = \t'"      
              << System::typeinfoName( typeid( Traits::Apply       ) ) 
              << "'"                                 << std::endl
              << " \t'Less'       = \t'"      
              << System::typeinfoName( typeid( Traits::Less        ) ) 
              << "'"                                 << std::endl
              << " \t'Id/CLID'    = \t'"      
              << Traits::id() 
              << "'"                                 << std::endl  ;
  }
  
  {
    typedef std::string                       Type    ;
    typedef Relations::ObjectTypeTraits<Type> Traits ;
    std::cout << " Types for type \t'"
              << System::typeinfoName( typeid( Type   ) ) 
              << "'"                                 << std::endl 
              << " \t'TYPE'       = \t'"      
              << System::typeinfoName( typeid( Traits::TYPE        ) ) 
              << "'"                                 << std::endl
              << " \t'Traits'     = \t'"      
              << System::typeinfoName( typeid( Traits::Traits      ) ) 
              << "'"                                 << std::endl 
              << " \t'Type'       = \t'"      
              << System::typeinfoName( typeid( Traits::Type        ) ) 
              << "'"                                 << std::endl 
              << " \t'Serializer' = \t'"      
              << System::typeinfoName( typeid( Traits::Serializer  ) ) 
              << "'"                                 << std::endl
              << " \t'Apply'      = \t'"      
              << System::typeinfoName( typeid( Traits::Apply       ) ) 
              << "'"                                 << std::endl
              << " \t'Less'       = \t'"      
              << System::typeinfoName( typeid( Traits::Less        ) ) 
              << "'"                                 << std::endl
              << " \t'Id/CLID'    = \t'"      
              << Traits::id() 
              << "'"                                 << std::endl  ;
  }
  {
#ifndef WIN32 
    std::cout << " Only for non MicroSoft compiler: " << std::endl ;
    typedef const std::string&              Type    ;
    typedef Relations::ObjectTypeTraits<Type> Traits ;
    std::cout << " Types for type \t'"
              << System::typeinfoName( typeid( Type   ) ) 
              << "'"                                 << std::endl 
              << " \t'TYPE'       = \t'"      
              << System::typeinfoName( typeid( Traits::TYPE        ) ) 
              << "'"                                 << std::endl
              << " \t'Traits'     = \t'"      
              << System::typeinfoName( typeid( Traits::Traits      ) ) 
              << "'"                                 << std::endl 
              << " \t'Type'       = \t'"      
              << System::typeinfoName( typeid( Traits::Type        ) ) 
              << "'"                                 << std::endl 
              << " \t'Serializer' = \t'"      
              << System::typeinfoName( typeid( Traits::Serializer  ) ) 
              << "'"                                 << std::endl
              << " \t'Apply'      = \t'"      
              << System::typeinfoName( typeid( Traits::Apply       ) ) 
              << "'"                                 << std::endl
              << " \t'Less'       = \t'"      
              << System::typeinfoName( typeid( Traits::Less        ) ) 
              << "'"                                 << std::endl
              << " \t'Id/CLID'    = \t'"      
              << Traits::id() 
              << "'"                                 << std::endl  ;
#endif
  }

  {
#ifndef WIN32 
    std::cout << " Only for non MicroSoft compiler: " << std::endl ;
    typedef const std::string********&        Type   ;
    typedef Relations::ObjectTypeTraits<Type> Traits ;
    std::cout << " Types for type \t'"
              << System::typeinfoName( typeid( Type   ) ) 
              << "'"                                 << std::endl 
              << " \t'TYPE'       = \t'"      
              << System::typeinfoName( typeid( Traits::TYPE        ) ) 
              << "'"                                 << std::endl
              << " \t'Traits'     = \t'"      
              << System::typeinfoName( typeid( Traits::Traits      ) ) 
              << "'"                                 << std::endl 
              << " \t'Type'       = \t'"      
              << System::typeinfoName( typeid( Traits::Type        ) ) 
              << "'"                                 << std::endl 
              << " \t'Serializer' = \t'"      
              << System::typeinfoName( typeid( Traits::Serializer  ) ) 
              << "'"                                 << std::endl
              << " \t'Apply'      = \t'"      
              << System::typeinfoName( typeid( Traits::Apply       ) ) 
              << "'"                                 << std::endl
              << " \t'Less'       = \t'"      
              << System::typeinfoName( typeid( Traits::Less        ) ) 
              << "'"                                 << std::endl
              << " \t'Id/CLID'    = \t'"      
              << Traits::id() 
              << "'"                                 << std::endl  ;
#endif
  }
  
  
  
    std::cout << stars                         << std::endl;
    std::cout << " Test for 'Relation1D' "    << std::endl;
    std::cout << stars                         << std::endl;
    
    {
      typedef Relation1D<ContainedObject,DataObject>  Rel;
      Rel* rel = new Rel();
      
      std::cout 
        << " Relations type: "                                     
        << std::endl
        << " \t'" << System::typeinfoName( typeid( *rel                 ) )  
        << "'" << std::endl 
        << " Interfaces: " 
        << " \t'" << System::typeinfoName( typeid( Rel::IBase           ) ) 
        << "'" << std::endl << " 'From'      " 
        << " \t'" << System::typeinfoName( typeid( Rel::IBase::From     ) ) 
        << "'" << std::endl 
        << " 'To'        " 
        << " \t'" << System::typeinfoName( typeid( Rel::IBase::To       ) ) 
        << "'" << std::endl 
        ;

      rel->release();
    }
    
    {
		typedef RelationWeighted1D<ContainedObject,std::string,double>  Rel;
      Rel * rel = new Rel();
      
      std::cout 
        << " Relations type: "                                     
        << std::endl
        << " \t'" << System::typeinfoName( typeid( *rel                 ) )  
        << "'" << std::endl 
        << " Interfaces: " 
        << " \t'" << System::typeinfoName( typeid( Rel::IBase           ) ) 
        << "'" << std::endl << " 'From'      " 
        << " \t'" << System::typeinfoName( typeid( Rel::IBase::From     ) ) 
        << "'" << std::endl 
        << " 'To'        " 
        << " \t'" << System::typeinfoName( typeid( Rel::IBase::To       ) ) 
        << "'" << std::endl 
        ;

      rel->release();
      
    }
    

    {
      typedef RelationWeighted1D<int,std::string,double>  Rel;
      Rel * rel = new Rel();
      
      std::cout 
        << " Relations type: "                                     
        << std::endl
        << " \t'" << System::typeinfoName( typeid( *rel                 ) )  
        << "'" << std::endl 
        << " Interfaces: " 
        << " \t'" << System::typeinfoName( typeid( Rel::IBase           ) ) 
        << "'" << std::endl << " 'From'      " 
        << " \t'" << System::typeinfoName( typeid( Rel::IBase::From     ) ) 
        << "'" << std::endl 
        << " 'To'        " 
        << " \t'" << System::typeinfoName( typeid( Rel::IBase::To       ) ) 
        << "'" << std::endl 
        << " 'Weight'    " 
        << " \t'" << System::typeinfoName( typeid( Rel::IBase::Weight   ) ) 
        << "'" << std::endl 
        ;

      
      // fill relations:
      for( int i = 0 ; i < 15 ; ++i )
        {
          if( 0 == i%2 ) 
            { rel->relate( i ,  "odd"  ,  100.0 * sin( 100. * i ) + i ) ; }
          else           
            { rel->relate( i ,  "even" ,  100.0 * sin( 100. * i ) + i ) ; }
        }
      { /// print all relations 
        Rel::Range r = rel->relations();
        for( Rel::iterator it = r.begin() ; r.end() != it ; ++it )
          {
            std::cout << " integer  " 
                      << it->from   () 
                      << " \t is related to string '"
                      << it->to     () << "'"
                      << " \t with the weigth equal to "
                      << it->weight () << std::endl ;
          }
      }
      /// filter out some weights :
      rel->filter( 50  , false );
      std::cout << " filter using '->filter(50,false)' " << std::endl ;
      { /// print all relations 
        Rel::Range r = rel->relations();
        for( Rel::iterator it = r.begin() ; r.end() != it ; ++it )
          {
            std::cout << " integer  " 
                      << it->from   () 
                      << " \t is related to string '"
                      << it->to     () << "'"
                      << " \t with the weigth equal to "
                      << it->weight () << std::endl ;
          }
      }
      /// filter out some weights :
      rel->filter( -50  , true );
      std::cout << " filter using '->filter(-50,true)' " << std::endl ;
      { /// print all relations 
        Rel::Range r = rel->relations();
        for( Rel::iterator it = r.begin() ; r.end() != it ; ++it )
          {
            std::cout << " integer  " 
                      << it->from   () 
                      << " \t is related to string '"
                      << it->to     () << "'"
                      << " \t with the weigth equal to "
                      << it->weight () << std::endl ;
          }
      }
      
      rel->release() ;
    }
    
  return 0;
  
};

// ============================================================================
// The END
// ============================================================================
