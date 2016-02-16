// $Id: ISTHitCleaner.h,v 1.3 2007-08-20 11:07:07 jonrob Exp $
#ifndef TFTOOLS_ISTHITCLEANER_H
#define TFTOOLS_ISTHITCLEANER_H 1

// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// From Tf
#include "TfKernel/STHit.h"
//#include "TfKernel/Region.h"

/// Interface ID
static const InterfaceID IID_ISTHitCleaner ( "ISTHitCleaner", 1, 0 );

namespace Tf
{

  /** @class ISTHitCleaner ISTHitCleaner.h TfKernel/ISTHitCleaner.h
   *
   *  Interface to tool that 'cleans' ranges of STHits
   *
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   2007-07-20
   */
  class ISTHitCleaner : virtual public IAlgTool
  {

  public:

    /// Return the interface ID
    static const InterfaceID& interfaceID() { return IID_ISTHitCleaner; }

  public:

    /** Clean the given range of ST hits
     *  @param[in]  input The range of ST hits to clean
     *  @param[out] output The selected hits
     *  @return The number of removed hits
     */
    template<class INPUTDATA >
    inline unsigned int cleanHits( const INPUTDATA & input,
                                   STHits& output ) const
    {
      return cleanHits ( input.begin(), input.end(), output );
    }

    /** Clean the given range of hits
     *  @param[in] begin Iterator to the start of a range of ST hits
     *  @param[in] end   Iterator to the start of a range of ST hits
     *  @param[out] output The selected hits   
     *  @return The number of removed hits
     */
    virtual unsigned int cleanHits( const STHits::const_iterator begin,
                                    const STHits::const_iterator end,
                                    STHits& output ) const = 0;

  };

}

#endif // TFTOOLS_ISTHITCLEANER_H
