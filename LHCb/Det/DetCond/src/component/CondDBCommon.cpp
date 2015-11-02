#include "CondDBCommon.h"

#include <sstream>

#include "CoolKernel/RecordSpecification.h"
#include "CoolKernel/Record.h"
#include "CoolKernel/StorageType.h"

static std::unique_ptr<cool::RecordSpecification> s_XMLStorageSpec{};

namespace CondDB {

const cool::RecordSpecification& getXMLStorageSpec() {
  if ( s_XMLStorageSpec.get() == NULL){
    // attribute list spec template
    s_XMLStorageSpec = std::unique_ptr<cool::RecordSpecification>(new cool::RecordSpecification());
    s_XMLStorageSpec->extend("data", cool::StorageType::String16M);
  }
  return *s_XMLStorageSpec;
}

namespace {
  inline bool ends_with(const std::string& s, const std::string &suff) {
    const auto count = suff.size();
    const auto size = s.size();
    return (size >= count) &&
           (s.compare(size - count, count, suff) == 0);
  }
}

void generateXMLCatalog(const std::string &name,
                        const std::vector<std::string> &fldrs,
                        const std::vector<std::string> &fldrsets,
                        std::string &data) {
  std::ostringstream xml; // buffer for the XML

  // XML header, root element and catalog initial tag
  xml << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>"
      << "<!DOCTYPE DDDB SYSTEM \"conddb:/DTD/structure.dtd\">"
      << "<DDDB><catalog name=\"" << name << "\">";

  // sub-folders are considered as container of conditions
  std::vector<std::string>::const_iterator f;
  for (const auto& f: fldrs) {
    // Ignore folders with the .xml extension.
    // We never used .xml for Online conditions and after the Hlt1/Hlt2 split
    // we need to avoid automatic mapping for the .xml files.
    if (!ends_with(f, ".xml")) {
      xml << "<conditionref href=\"" << name << '/' << f << "\"/>";
    }
  }
  // sub-foldersets are considered as catalogs
  for (const auto& f: fldrsets) {
    xml << "<catalogref href=\"" << name << '/' << f << "\"/>";
  }
  // catalog and root element final tag
  xml << "</catalog></DDDB>";

  data = xml.str();
}

StatusCode generateXMLCatalog(ICondDBReader *reader, const std::string &path,
                              ICondDBReader::DataPtr &payload){
  // get the list of subnodes
  std::vector<std::string> folders, foldersets;
  StatusCode sc = reader->getChildNodes(path,folders,foldersets);
  if (sc.isFailure()) return sc;

  // extract the name of the folderset
  std::string::size_type pos = path.rfind('/');
  std::string name;
  if ( std::string::npos != pos ) {
    name = path.substr(pos+1);
  } else {
    name = path;
  }

  // generate the XML catalog
  std::string xml;
  generateXMLCatalog(name,folders,foldersets,xml);

  // prepare new payload
  cool::Record *rec = new cool::Record(getXMLStorageSpec());
  (*rec)["data"].setValue<cool::String16M>(xml);
  payload.reset(rec);

  return sc;
}

}
