// $Id: ANNSvc.h,v 1.5 2008-01-25 12:18:50 graven Exp $
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
 *         and also 'Assigned Numbers and Names'
 *
 *  @author Gerhard Raven
 *  @date   2008-01-02
 */


class ANNSvc : public Service, 
               virtual public IANNSvc {
public:
  ANNSvc( const std::string& name, ISvcLocator* pSvcLocator,
          const std::vector<major_key_type>& majors );
  ~ANNSvc();
  
  StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown);

  boost::optional<int>           asInt   (const major_key_type& major, const std::string& minor) const;
  boost::optional<std::string>   asString(const major_key_type& major, int minor) const;

  bool                           hasMajor(const major_key_type& major) const;

  std::vector<minor_value_type>  items(const major_key_type& major) const;
  std::vector<major_key_type>    majors() const;
private:
  
  typedef ANNSvcUtilities::bimap_t<minor_key_type, minor_mapped_type> bimap_type;
  typedef GaudiUtils::VectorMap< major_key_type, bimap_type* > maps_type;
  maps_type   m_maps;
};
#endif // ANNSVC_H 1
