
#ifndef RichRegistry_h
#define RichRegistry_h 1

#include <set>
#include <string>
#include <vector>

#include "RichBase.h"

namespace LHCb
{
  class RichSmartID;
}

//-----------------------------------------------------------------------------
/** @namespace Rich
 *
 *  General namespace for RICH software
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------
namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @namespace MC
   *
   *  General namespace for RICH MC related software
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   05/12/2006
   */
  //-----------------------------------------------------------------------------
  namespace MC
  {

    //-----------------------------------------------------------------------------
    /** @namespace Digi
     *
     *  General namespace for RICH Digitisation simuation related software
     *
     *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
     *  @date   17/01/2007
     */
    //-----------------------------------------------------------------------------
    namespace Digi
    {

      /// Rich Registry utility class
      class RichRegistry
      {

      public:

        RichRegistry() {}

        ~RichRegistry() { cleanUp(); }

        static const RichBase* GetNewBase( const std::vector<LHCb::RichSmartID> & pixels );

        static const RichBase* GetBase();

        static void RegisterMe(RichBase*);

      private: // methods

        void cleanUp();

      private: // data

        typedef std::set<RichBase*,std::less<RichBase*> > RegisterMap;
        static RegisterMap theRegister;

      };

      inline const RichBase * RichRegistry::GetNewBase( const std::vector<LHCb::RichSmartID> & pixels )
      {
        RichBase * theBase = new RichBase();
        theBase->upDate(pixels);
        return theBase;
      }

      inline const RichBase * RichRegistry::GetBase( )
      {
        return *(theRegister.begin());
      }

      inline void RichRegistry::RegisterMe(RichBase*b)
      {
        theRegister.insert(b);
      }

    }
  }
}

#endif
