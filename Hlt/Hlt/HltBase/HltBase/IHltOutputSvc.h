#ifndef IHLTOUTPUTSVC_H
#define IHLTOUTPUTSVC_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/INamedInterface.h"


/** @class IHltOutputSvc IHltOutputSvc.h HltCommon/IHltOutputSvc.h
 *
 *  @author Roel Aaij
 *  @date   2016-03-26
 */
class GAUDI_API IHltOutputSvc : virtual public INamedInterface {
public:

   DeclareInterfaceID(IHltOutputSvc, 1, 0);

   virtual ~IHltOutputSvc() {}

   /// Return the list of stream groups
   virtual std::vector<std::string> groups() const = 0;

   /// Return the list of logical streams for a given group
   virtual std::vector<std::string> logicalStreams(std::string group) const = 0;

   /// Return the list of filters for a given logical stream
   virtual std::vector<std::string> filters(std::string stream) const = 0;

};
#endif // IHLTOUTPUTSVC_H
