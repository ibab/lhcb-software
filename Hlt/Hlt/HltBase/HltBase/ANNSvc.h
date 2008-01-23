// $Id: ANNSvc.h,v 1.2 2008-01-23 13:37:23 graven Exp $
#ifndef ANNSVC_H  
#define ANNSVC_H 1

// Include files
#include "HltBase/IANNSvc.h"
#include "GaudiKernel/VectorMap.h"
#include "GaudiKernel/Service.h"
#include <string>

namespace ANNSvcUtilities {
  template <typename KEY, typename VALUE> class bimap_t;
}
/** @class ANNSvc ANNSvc.h
 *  
 *  functionality:
 *     maps between 'Assigned Names and Numbers'
 *
 *  @author Gerhard Raven
 *  @date   2008-01-02
 */


class ANNSvc : public Service, 
               virtual public IANNSvc {
public:
  ANNSvc( const std::string& name, ISvcLocator* pSvcLocator,const std::vector<std::string>& majors);
  ~ANNSvc();
  
  StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown);

  boost::optional<int>           asInt   (const std::string& major, const std::string& minor) const;
  boost::optional<std::string>   asString(const std::string& major, int minor) const;

  bool                           hasMajor(const std::string& major) const;

  std::vector< std::pair<std::string,int> > items(const std::string& major) const;
  std::vector< std::string > majors() const;
private:
  
  typedef ANNSvcUtilities::bimap_t<std::string,  int>       bimap_type;
  typedef GaudiUtils::VectorMap< std::string, bimap_type* > maps_type;
  maps_type   m_maps;
};
#endif // ANNSVC_H 1
