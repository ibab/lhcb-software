
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

namespace Rich
{
  namespace MC
  {
    namespace Digi
    {

      /// Rich Registry utility class
      class RichRegistry
      {

      public:

        RichRegistry() {}

        ~RichRegistry() { cleanUp(); }

        static const RichBase* GetNewBase( const LHCb::RichSmartID::Vector & pixels );

        static const RichBase* GetBase();

        static void RegisterMe(RichBase*);

      private: // methods

        void cleanUp();

      private: // data

        typedef std::set< RichBase*, std::less<RichBase*> > RegisterMap;
        static RegisterMap theRegister;

      };

      inline const RichBase * RichRegistry::GetNewBase( const LHCb::RichSmartID::Vector & pixels )
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
