// $Id: XMLTags.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4hep_XML_TAGS_H
#define DD4hep_XML_TAGS_H

#define DECLARE_UNICODE_TAG(x)  namespace DD4hep { namespace XML { extern const Tag_t Unicode_##x (#x); }}
#define IMPORT_UNICODE_TAG(x)   extern const DD4hep::XML::Tag_t DD4hep::XML::Unicode_##x

#include "XML/XMLElements.h"
#define UNICODE(x)  extern const Tag_t Unicode_##x
#include "XML/UnicodeValues.h"
#undef  UNICODE

// Helpers to access tags and attributes quickly without specifying explicitly namespaces
#define _U(a) DD4hep::XML::Unicode_##a
#define _Unicode(a) DD4hep::XML::Strng_t(#a)


#endif // DD4hep_XML_TAGS_H
