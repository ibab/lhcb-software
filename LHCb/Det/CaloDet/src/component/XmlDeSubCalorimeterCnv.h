#ifndef            __CALO_XML_XMLDESUBCALORIMETERCNV_H__
#define            __CALO_XML_XMLDESUBCALORIMETERCNV_H__ 1 

///
///
///


#include          "DetDesc/XmlUserDeCnv.h"

#include          "CaloDet/DeSubCalorimeter.h"


class XmlDeSubCalorimeterCnv: public XmlUserDeCnv<DeSubCalorimeter>
{
  
 public:
  
  XmlDeSubCalorimeterCnv( ISvcLocator* svc );
  
  virtual ~XmlDeSubCalorimeterCnv(){};
  
  ///  
  /// Start of the XML element callback
  ///
  virtual void uStartElement( const char* const name , XmlCnvAttributeList& attributes);

};


#endif //          __CALO_XML_XMLDESUBCALORIMETERCNV_H__
