#ifndef            __CALO_XML_XMLDECALORIMETERCNV_H__
#define            __CALO_XML_XMLDECALORIMETERCNV_H__ 1 

///
///
///


#include          "DetDesc/XmlUserDeCnv.h"

#include          "CaloDet/DeCalorimeter.h"


class XmlDeCalorimeterCnv: public XmlUserDeCnv<DeCalorimeter>
{
  
 public:
  
  XmlDeCalorimeterCnv( ISvcLocator* svc );
  
  virtual ~XmlDeCalorimeterCnv(){};
  
  ///  
  /// Start of the XML element callback
  ///
  virtual void uStartElement( const char* const name , XmlCnvAttributeList& attributes);
  
};


#endif //          __CALO_XML_XMLDECALORIMETERCNV_H__
