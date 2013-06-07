// $Id: ITsaStubFind.h,v 1.3 2007-10-09 18:03:38 smenzeme Exp $
#ifndef _ITsaStubFind_H
#define _ITsaStubFind_H

#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_ITsaStubFind( "ITsaStubFind", 0, 0 );

#include <vector>

namespace Tf
{
  namespace Tsa
  {

    class SeedStub;
    class SeedHit;

    /** @class ITsaStubFind ITsaStubFind.h TsaKernel/ITsaStubFind.h
     *
     *  Interface to Tsa Stub finder
     *
     *  @author M.Needham
     *  @date   12/11/2006
     */

    class ITsaStubFind: virtual public IAlgTool 
    {
    public:
      
      /// Retrieve interface ID
      static const InterfaceID& interfaceID() { return IID_ITsaStubFind ; }
      
      virtual StatusCode execute(std::vector<SeedHit*> hits[], std::vector<SeedHit*> sHits[],
                                 std::vector<SeedStub*> stubs[]  ) = 0;
      
    };
    
  }
}

#endif
