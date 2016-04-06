#ifndef HLTOUTPUTSVC_H
#define HLTOUTPUTSVC_H 1

// STD & STL
#include <vector>
#include <string>
#include <map>

// Include files
#include "GaudiKernel/Service.h"
#include "HltBase/IHltOutputSvc.h"

/** @class HltOutputSvc HltOutputSvc.h
 *
 *
 *  @author Roel Aaij
 *  @date   2016-03-26
 */
class HltOutputSvc : public extends1<Service, IHltOutputSvc> {
public:

   HltOutputSvc(const std::string& name, ISvcLocator* svcLocator);

   ~HltOutputSvc() override  = default;

   StatusCode initialize() override;
   StatusCode finalize() override;

   std::vector<std::string> groups() const override;

   std::vector<std::string> logicalStreams(std::string group) const override;

   std::vector<std::string> filters(std::string stream) const override;

private:

   using OutputMap = std::map<std::string, std::vector<std::string>>;

   OutputMap m_streams;
   OutputMap m_filters;

};

#endif // HLTOUTPUTSVC_H
