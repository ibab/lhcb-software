// $Id: XMLTags.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4hep_XML_TYPES_H
#define DD4hep_XML_TYPES_H

#include "XML/XMLTags.h"
#include "XML/XMLDimension.h"
#include "XML/DocumentHandler.h"

typedef DD4hep::XML::Handle_t      xml_h;
typedef DD4hep::XML::Tag_t         xml_tag_t;
typedef DD4hep::XML::Attribute     xml_attr_t;
typedef DD4hep::XML::Collection_t  xml_coll_t;
typedef DD4hep::XML::Element       xml_elt_t;
typedef DD4hep::XML::RefElement    xml_ref_t;
typedef DD4hep::XML::Dimension     xml_dim_t;
typedef DD4hep::XML::Document      xml_doc_t;
typedef DD4hep::XML::Strng_t       Unicode;

#endif // DD4hep_XML_TYPES_H
