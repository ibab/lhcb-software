// $Id: IDirectoryAccess.h,v 1.2 2010-05-05 21:05:05 graven Exp $
#ifndef GaudiKernel_IDirectoryAccess_H
#define GaudiKernel_IDirectoryAccess_H

// Include Files
#include "GaudiKernel/IInterface.h"
#include <string>
#include <vector>

/** @class IDirectoryAccess IDirectoryAccess.h GaudiKernel/IDirectoryAccess.h
 *
 * Abstract interface for a service or tool implementing lookup
 * access to directories. Modelled after IFileAccess by Marco Clemencic,
 * and the python os module.
 *
 * @author Gerhard Raven
 * @date   2010-02-11
 */
class GAUDI_API IDirectoryAccess : virtual public IInterface
{
  public:
    /// InterfaceID
    DeclareInterfaceID( IDirectoryAccess, 1, 0 );

    virtual ~IDirectoryAccess()
    {
    } //< Virtual destructor.

    /// Find the URL and returns a list of file entries
    /// Returns an empty list if the URL cannot be resolved.
    virtual std::vector<std::string> listdir( const std::string& url ) = 0;

    /// Protocols supported by the instance.
    virtual const std::vector<std::string>& protocols() const = 0;
};

#endif // GaudiKernel_IDirectoryAccess_H

