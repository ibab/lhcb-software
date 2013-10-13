// $Id:$ 
// ============================================================================
// Include files
// ============================================================================
// 
#include "Analysis/StatVar.h"
#include "Analysis/Formula.h"

#include "TTree.h"
/** @file 
 *  Implementation file for class Analysis::StatVar
 *  @date 2013-10-13 
 *  @author Vanya BELYAEV Ivan.Brlyaev@itep.ru
 * 
 *                    $Revision:$
 *  Last modification $Date:$
 *                 by $Author:$
 */
// ============================================================================
namespace 
{
  // ==========================================================================
  /** @class Notifier
   *  Local helper class to keep the proper notifications for TTree
   *  @date 2013-10-13 
   *  @author Vanya BELYAEV Ivan.Brlyaev@itep.ru
   */
  class Notifier : public TObject 
  {
    //
  public:
    //
    Notifier 
    ( TTree*   tree     , 
      TObject* obj0     , 
      TObject* obj1 = 0 , 
      TObject* obj2 = 0 , 
      TObject* obj3 = 0 , 
      TObject* obj4 = 0 ,
      TObject* obj5 = 0 , 
      TObject* obj6 = 0 , 
      TObject* obj7 = 0 , 
      TObject* obj8 = 0 , 
      TObject* obj9 = 0 ) ;
    /// virtual destructor 
    virtual        ~Notifier () ;  // virtual destructor 
    virtual Bool_t  Notify   () ;
    //
  private:
    Notifier () ;
    Notifier ( const Notifier & ) ;
  private:
    TTree*   m_tree ;
    TObject* m_old  ; // old notofier  
    // list of fobject to be notified 
    std::vector<TObject*> m_objects ;
  } ;
  // ==========================================================================
  Notifier::Notifier 
  ( TTree*   tree , 
    TObject* obj0 , 
    TObject* obj1 , 
    TObject* obj2 , 
    TObject* obj3 , 
    TObject* obj4 ,
    TObject* obj5 , 
    TObject* obj6 , 
    TObject* obj7 , 
    TObject* obj8 , 
    TObject* obj9 ) 
    : TObject   () 
    , m_tree    ( tree ) 
    , m_old     ( 0    )
    , m_objects () 
  {
    //
    if ( 0 != m_tree ) { m_old = m_tree->GetNotify ()  ; }
    //
    if ( 0 != m_old  ) { m_objects.push_back ( m_old ) ; } // prepend it! 
    //
    if ( 0 != obj0   ) { m_objects.push_back ( obj0  ) ; }
    if ( 0 != obj1   ) { m_objects.push_back ( obj1  ) ; }
    if ( 0 != obj2   ) { m_objects.push_back ( obj2  ) ; }
    if ( 0 != obj3   ) { m_objects.push_back ( obj3  ) ; }
    if ( 0 != obj4   ) { m_objects.push_back ( obj4  ) ; }
    if ( 0 != obj5   ) { m_objects.push_back ( obj5  ) ; }
    if ( 0 != obj6   ) { m_objects.push_back ( obj6  ) ; }
    if ( 0 != obj7   ) { m_objects.push_back ( obj7  ) ; }
    if ( 0 != obj8   ) { m_objects.push_back ( obj8  ) ; }
    if ( 0 != obj9   ) { m_objects.push_back ( obj9  ) ; }
    //
    if ( 0 != m_tree ) { m_tree->SetNotify   ( this  ) ; }
    //
  }
  // ==========================================================================
  Notifier::~Notifier() { if ( 0 != m_tree ) { m_tree->SetNotify ( m_old ) ; } }
  // ==========================================================================
  Bool_t Notifier::Notify() 
  {
    //
    for (  std::vector<TObject*>::iterator it = m_objects.begin() ;
           m_objects.end() != it ; ++it ) 
    {
      TObject* o = *it ;
      if ( 0 != o ) { o->Notify() ; }
    }
    //
    return kTRUE ;
  }
  // ========================================================================== 
} //                                                 end of anonymous namespace 
// ============================================================================
/*  build statistic for the <code>expression</code>
 *  @param tree (INPUT) the tree 
 *  @param expression (INPUT) the expression
 *
 *  @code
 *  tree = ... 
 *  stat = tree.statVar( 'S_sw' ) 
 *  @endcode 
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2013-10-13
 */
// ============================================================================
StatEntity 
Analysis::StatVar::statVar 
( TTree*             tree       , 
  const std::string& expression ) 
{
  StatEntity result ;
  if ( 0 == tree          ) { return result ; }  // RETURN 
  Analysis::Formula formula ( "" , expression , tree ) ;
  if ( !formula.GetNdim() ) { return result ; }  // RETURN 
  //
  Notifier notify ( tree , &formula ) ;
  //
  const unsigned long nEntries = 
    (unsigned long ) tree->GetEntries() ;
  //
  for ( unsigned long entry = 0 ; entry < nEntries ; ++entry )   
  {
    //
    long ievent = tree->GetEntryNumber ( entry ) ;
    if ( 0 > ievent ) { return result ; }                // RETURN
    //
    ievent      = tree->LoadTree ( ievent ) ;
    if ( 0 > ievent ) { return result ; }                // RETURN 
    //
    result += formula.evaluate() ;
  }
  //
  return result ;
}
// ============================================================================
/*  build statistic for the <code>expression</code>
 *  @param tree       (INPUT) the tree 
 *  @param expression (INPUT) the expression
 *  @param cuts       (INPUT) the selection criteria 
 *
 *  @code
 *  tree = ... 
 *  stat = tree.statVar( 'S_sw' ,'pt>1000') 
 *  @endcode 
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2013-10-13
 */
// ============================================================================
StatEntity 
Analysis::StatVar::statVar 
( TTree*             tree       , 
  const std::string& expression ,
  const std::string& cuts       ) 
{
  StatEntity result ;
  if ( 0 == tree        ) { return result ; }            // RETURN 
  Analysis::Formula selection ( "" , cuts      , tree ) ;
  if ( !selection.ok () ) { return result ; }            // RETURN 
  Analysis::Formula formula   ( "" , expression , tree ) ;
  if ( !formula  .ok () ) { return result ; }            // RETURN 
  //
  Notifier notify ( tree , &selection,  &formula ) ;
  //
  const unsigned long nEntries = 
    (unsigned long ) tree->GetEntries() ;
  //
  for ( unsigned long entry = 0 ; entry < nEntries ; ++entry )   
  {
    //
    long ievent = tree->GetEntryNumber ( entry ) ;
    if ( 0 > ievent ) { return result ; }                // RETURN
    //
    ievent      = tree->LoadTree ( ievent ) ;
    if ( 0 > ievent ) { return result ; }                // RETURN 
    //
    if ( selection.evaluate() ) { result += formula.evaluate() ; }
    //
  }
  //
  return result ;
}
// ============================================================================
// The END 
// ============================================================================
