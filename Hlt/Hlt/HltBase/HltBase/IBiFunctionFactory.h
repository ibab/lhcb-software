// $Id: IBiFunctionFactory.h,v 1.4 2009-10-12 18:12:39 graven Exp $
#ifndef HLTBASE_IBIFUNCTIONFACTORY_H
#define HLTBASE_IBIFUNCTIONFACTORY_H 1

// Include files
// from STL
#include <string>
#include "HltBase/IFunctions.h"

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IBiFunctionFactory( "IBiFunctionFactory", 1, 0 );

/** @class IBiFunctionFactory IBiFunctionFactory.h HltBase/IBiFunctionFactory.h
 *
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2007-11-26
 */
template <class T1, class T2>
class IBiFunctionFactory : virtual public IAlgTool
{
  public:
    // Return the interface ID
    static const InterfaceID& interfaceID()
    {
        return IID_IBiFunctionFactory;
    }

    virtual bool command( const std::string& command,
                          const std::string& value = "" ) = 0;

    virtual zen::bifunction<T1, T2>* function( const std::string& functioname ) = 0;
    virtual zen::bifilter<T1, T2>* filter( const std::string& filtername ) = 0;
};
#endif // HLTBASE_IBiFUNCTIONSFACTORY_H
