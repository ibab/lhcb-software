// $Id: ANNSvc.h,v 1.1 2010-05-05 13:20:43 graven Exp $
#ifndef ANNSVC_H  
#define ANNSVC_H 1

// Include files
#include "Kernel/IANNSvc.h"
//#include "GaudiKernel/VectorMap.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"
#include <string>
//#include "GaudiKernel/ParsersFactory.h"

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


class ANNSvc : public extends1<Service, IANNSvc> {
public:
  ANNSvc( const std::string& name, ISvcLocator* pSvcLocator,
          const std::vector<major_key_type>& majors );
  ~ANNSvc() = default;
  
  StatusCode finalize() override;

  boost::optional<minor_value_type>  value(const major_key_type& major, const std::string& minor) const override;
  boost::optional<minor_value_type>  value(const major_key_type& major, int minor) const override;

  bool                           hasMajor(const major_key_type& major) const override;

  std::vector<minor_key_type>    keys(const major_key_type& major) const override;
  std::vector<minor_value_type>  items(const major_key_type& major) const override;
  GaudiUtils::VectorMap< minor_value_type::first_type, minor_value_type::second_type > item_map(const major_key_type& major) const override;
  std::vector<major_key_type>    majors() const override;

protected:
  // Call handleUndefined if an unknown key/value is requested...
  //  default implementation just returns an unitialized boost::optional
  virtual boost::optional<minor_value_type> handleUndefined(const major_key_type& major, int minor) const;
  virtual boost::optional<minor_value_type> handleUndefined(const major_key_type& major, const std::string& minor) const;

  MsgStream& log() const { return m_log; }


private:
  mutable MsgStream m_log;
  
  typedef ANNSvcUtilities::bimap_t<minor_key_type, minor_mapped_type> bimap_type;
  typedef GaudiUtils::VectorMap< major_key_type, bimap_type* > maps_type;
  maps_type   m_maps;
  friend class ANNSvcUtilities::bimap_t<minor_key_type, minor_mapped_type> ;

};



#endif // ANNSVC_H 1
