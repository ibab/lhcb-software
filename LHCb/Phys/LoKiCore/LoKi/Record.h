// $Id$
// ============================================================================
#ifndef LOKI_RECORD_H 
#define LOKI_RECORD_H 1
// ============================================================================
// Include files
// ============================================================================
#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/Tuples.h"
// ============================================================================
/** @file
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
// ============================================================================
namespace LoKi
{  
  /** @class Record Record.h LoKi/Record.h
   *  
   *  Simple wrapper class to make more easy ("1 line") 
   *  n-tuple handling in LoKi
   * 
   *  It is supposed to be ALWAYS LOCAL AUTOMATIC object
   *
   *  The simplest example of LoKi::Record usage:
   *  @code
   *  Tuple tuple = ntuple( "some more or less uniqe tuple title ");
   *  for( Loop D0 = loop( "K- pi+", "D0" ) , D0 , ++D0 ) 
   *  {
   *     Record( tuple , "mass pt , p ", M(D0)/GeV,PT(D0)/GeV,P(D0)/GeV ) ;
   *  }
   *  @endcode 
   *
   *  Other entries/items could be added to already existing record: 
   *  @code
   *  Tuple tuple = ntuple( "some more or less uniqe tuple title ");
   *  for( Loop D0 = loop( "K- pi+", "D0" ) , D0 , ++D0 ) 
   *  {
   *     Record record( tuple , "pt , p " ,PT(D0)/GeV,P(D0)/GeV ) ;
   *     record->column( "CL" , CL(D0) );
   *     record->fill  ( "px,py pz e" , PX(D0),PY(D0),PZ(D0),E(D0) );
   *  }
   *  @endcode 
   *
   *  @see Tuples::Tuple
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-10-30
   */
  class Record : public Tuples::Tuple 
  {
  public:
    /** standard constructor
     *  @param ntuple LoKi Tuple adapter 
     */
    Record ( const Tuples::Tuple& ntuple ) 
      : Tuples::Tuple ( ntuple ) {};
    /** constructor 
     *  @param tuple pointer to LoKi Tuple adapter 
     *  @param format blank-separated list of variables, 
     *          followed by variable number of arguments  
     *  @attention  All variables are assumed to be <c>double</c> numbers
     */
    Record ( const Tuples::Tuple& tuple      , 
             const char*        format ... ) ;
    /** copy conctructor 
     *  @param record the record to be copied 
     */
    Record ( const Record& record ) 
      : Tuples::Tuple ( record ) {};
    /// destructor 
    virtual ~Record();
  private:
    /// default constructor is private 
    Record ();
    /// assignemenet operator is private 
    Record& operator=( const Record& );
  };
  // ============================================================================  
}  // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_RECORD_H
// ============================================================================
