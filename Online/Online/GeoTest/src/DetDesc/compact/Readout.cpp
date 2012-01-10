#include "DetDesc/compact/Readout.h"
#include "DetDesc/Internals.h"

using namespace std;
using namespace DetDesc::Geometry;

/// Constructor to be used when reading the already parsed object
Readout::Readout(Handle_t e) : RefElement(e) {
}

/// Initializing constructor to create a new object
Readout::Readout(const Document& doc, const string& nam) : RefElement(doc,"readout",nam) 
{
}

/// Access IDDescription structure
Handle_t Readout::idSpec() const   {
  return Handle_t(second_value<TNamed>(*this)->id);
}

/// Access segmentation structure
Handle_t Readout::segmentation() const  {
  return Handle_t(second_value<TNamed>(*this)->segmentation);
}

/// Assign IDDescription to readout structure
void Readout::setIDDescriptor(Handle_t new_descriptor)  const   {
  Object* ro = second_value<TNamed>(*this);
  if ( ro )  {                 // Remember: segmentation is NOT owned by readout structure!
    if ( new_descriptor )  {   // Do NOT delete!
      ro->id = new_descriptor;
      return;
    }
  }
  throw runtime_error("Readout::setSegmentation: Cannot assign ID descriptor [Invalid Handle]");
}

/// Assign segmentation structure to readout
void Readout::setSegmentation(const Handle_t seg)   const  {
  Object* ro = second_value<TNamed>(*this);
  if ( ro )  {
    if ( ro->segmentation )  {       // Remember: segmentation is owned by readout structure!
      delete ro->segmentation.ptr(); // Need to delete the segmentation object
    }
    if ( seg )  {
      ro->segmentation = seg;
      return;
    }
  }
  throw runtime_error("Readout::setSegmentation: Cannot assign segmentation [Invalid Handle]");
}
