
//--------------------------------------------------------------------------------
/** @file RichPoissonEffFunctor.h
 *
 *  Header file for poisson efficiency functor : RichPoissonEffFunctor
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2003-09-08
 */
//--------------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHPOISSONEFFFUNCTOR_H
#define RICHKERNEL_RICHPOISSONEFFFUNCTOR_H 1

// STL
#include <functional>

// Gaudi
#include "GaudiKernel/MsgStream.h"

// boost
#include "boost/format.hpp"

namespace Rich
{

  class PoissonEffFunctor;

  //--------------------------------------------------------------------------------
  /** @class PoissonEffFunctorResult RichPoissonEffFunctor.h RichKernel/RichPoissonEffFunctor.h
   *
   *  Result object for poisson efficency and error calculator.
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   2004-12-20
   */
  //--------------------------------------------------------------------------------

  class PoissonEffFunctorResult : private std::pair<double,double>
  {

  public:

    /** Default constructor
     *
     *  @param result The result of the calculation
     *  @param error  The error on the result
     *  @param parent Point to the parent calculator
     */
    PoissonEffFunctorResult( const double result,
                             const double error,
                             const PoissonEffFunctor * parent )
      : std::pair<double,double>(result,error),
        m_parent ( parent ) { }

    /// Access the result of the calculation
    inline double result() const { return this->first; }

    /// Access the error on the result of the calculation
    inline double error() const { return this->second; }

    /// Access the parent calculator
    inline const PoissonEffFunctor * parent() const { return m_parent; }

  private:

    /// Pointer to parent calculator
    const PoissonEffFunctor * m_parent = nullptr;

  };

  //--------------------------------------------------------------------------------
  /** @class PoissonEffFunctor RichPoissonEffFunctor.h RichKernel/RichPoissonEffFunctor.h
   *
   *  Simple utility class to provide an easy way to produce a formated output division
   *  calculation, with the associated poisson error.
   *
   *  @code
   *  RichStatDivFunctor result("%8.2f +-%6.2f");
   *  info() << "Result = " << result(a,b) << endmsg;
   *  @endcode
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   2004-12-20
   */
  //--------------------------------------------------------------------------------

  class PoissonEffFunctor
    : public std::binary_function< const double,
                                   const double,
                                   PoissonEffFunctorResult >
  {

  public:

    /** Constructor with print format string
     *
     *  @param format The Printing format
     */
    PoissonEffFunctor( const std::string & format = "%8.2f +-%6.2f" )
      : m_format( format ) { }

      /** The efficiency calculation operator
       *
       *  @param top The numerator
       *  @param bot The denominator
       *
       *  @return The poisson efficiency and error
       */
      inline PoissonEffFunctorResult operator() ( const double top,
                                                  const double bot ) const
      {
        return PoissonEffFunctorResult( ( bot>0 ? 100.0 * top/bot                          : 0 ),
                                        ( bot>0 ? 100.0 * sqrt((top/bot)*(1.-top/bot)/bot) : 0 ),
                                        this );
      }

      /** Access the print format
       *
       *  @return The print format string
       */
      inline const std::string & printFormat() const { return m_format; }

  private:

      /// The print format
      std::string m_format;

  };

  /// overloaded output to MsgStream
  inline MsgStream & operator << ( MsgStream & os,
                                   const PoissonEffFunctorResult & res )
  {
    return os << boost::format( res.parent()->printFormat() ) % res.result() % res.error() ;
  }

}

#endif // RICHKERNEL_RICHPOISSONEFFFUNCTOR_H
