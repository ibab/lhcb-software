// $Id: Solid.cpp,v 1.5 2005-01-25 14:09:19 cattanem Exp $
/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/** STD & STL includes  */ 
#include <string>
#include <vector>
#include <functional>
#include <algorithm>

/** GaudiKernel includes */ 
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IObjManager.h"
#include "GaudiKernel/IFactory.h"
#include "GaudiKernel/Bootstrap.h"

/** DetDesc includes */ 
#include "DetDesc/ISolid.h"
#include "DetDesc/ISolidFactory.h"
#include "DetDesc/Solid.h"
#include "DetDesc/SolidException.h"

// ============================================================================
/** @file Solid.cpp
 * 
 * Implementation file for methods from namespace Solid 
 * @date  09/08/2001 
 * @author Vanya  Belyaev Ivan.Belyaev@itep.ru 
 */
// ============================================================================


// ============================================================================
/** @class SolidFactoryType Solid.cpp 
 *  
 *  helper class to locate soldi factory of given type 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 09/08/2001
 */
// ============================================================================
class SolidFactoryType: 
  public std::unary_function<const ISolidFactory*, bool>
{
public:
  /** explicit constructor */
  explicit SolidFactoryType( const std::string& type )
    : m_type ( type ){};
  /** check the solid factory type 
   *  @param factory pointer to thr ISolidFactory object
   *  @return true if the factory is able to create objects of given type
   */
  inline bool operator()( const ISolidFactory* factory )
  { return ( 0 == factory ) ? false :  ( factory->ident() == m_type ) ; }  
private:
  std::string m_type ; ///< type name of the object 
};

// ============================================================================
/** create (using abstract factory technique) the 
 *  concrete solid of given type 
 *  
 *  @see ISolid
 *  @see ISolidFactory
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date 03/08/2001 
 *
 *  @exception SolidException  wrong solid type or environment 
 *  @param solidType   type of solid 
 *  @return pointer to new created ISolid object 
 */
// ============================================================================
ISolid* Solid::createSolid( const std::string& solidType )
{
  /// typedef for collection of solid factories 
  typedef std::vector<const ISolidFactory*> Factories;
  /// static local copy of solid factories
  static Factories s_Factories;
  /// 
  Factories::const_iterator iFac = 
    std::find_if( s_Factories.begin ()            , 
                  s_Factories.end   ()            , 
                  SolidFactoryType  ( solidType ) );
  ///
  if( s_Factories.end() == iFac )
    {
      /// locate object manager 
      IObjManager* objMgr = 0 ;
      ISvcLocator* svcLoc = Gaudi::svcLocator();
      if( 0 == svcLoc ) 
        { throw SolidException("Solid: ISvcLocator* point to NULL!"); }
      /// locate the object manager
      StatusCode sc = svcLoc->service( "ApplicationMgr" , objMgr );
      if( sc.isFailure() ) 
        { throw SolidException("Solid: Could not locate IObjManager!");}
      if( 0 == objMgr  )
        { throw SolidException("Solid: IObjManager* points to NULL!"); }
      const IFactory* fact = objMgr->objFactory( solidType );
      objMgr->release();
      if( 0 == fact ) 
        { throw SolidException("Solid: no factory for type '" +
                               solidType + "' is found!"); }
      const ISolidFactory* factory  = 
        dynamic_cast<const ISolidFactory*> (fact);
      if( 0 == factory ) 
        { throw SolidException("Solid: no solid factory for type '" +
                               solidType + "' is found!"); }
      /// add factory to the list of known factories 
      s_Factories.push_back( factory );
      /// instantiate the solid object 
      return factory->instantiate() ;  ///< RETURN !!!
      ///
    }
  ///
  return (*iFac)->instantiate();
};



