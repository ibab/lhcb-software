
// local
#include "Event/StandardPacker.h"

// STL
#include <sstream>

// Gaudi
#include "GaudiKernel/GaudiException.h"

//-----------------------------------------------------------------------------
// Implementation file for class : StandardPacker
//
// 2015-03-07 : Chris Jones
//-----------------------------------------------------------------------------

int StandardPacker::reference32( DataObject* out,
                                 const DataObject* parent,
                                 const int key ) const
{

  if ( key != (key & 0x0FFFFFFF) )
  {
    std::ostringstream mess;
    mess << "************************* Key over 28 bits in StandardPacker ***********************" 
         << " " << key;
    std::cerr << mess.str() << std::endl;
    throw GaudiException( mess.str(), "StandardPacker", StatusCode::FAILURE );
  }

  const int rawLinkID = (int)linkID(out,parent);
  if ( rawLinkID != (rawLinkID & 0x0000000F) )
  {
    std::ostringstream mess;
    mess << "************************* LinkID over 4 bits in StandardPacker ***********************" 
         << " " << rawLinkID;
    std::cerr << mess.str() << std::endl;
    throw GaudiException( mess.str(), "StandardPacker", StatusCode::FAILURE );
  }

  const int myLinkID = rawLinkID << 28;
  return key + myLinkID;
}

int StandardPacker::reference32( DataObject* out,
                                 const std::string& targetName,
                                 const int key ) const
{
  if ( key != (key & 0x0FFFFFFF) )
  {
    std::ostringstream mess;
    mess << "************************* Key over 28 bits in StandardPacker ***********************" 
         << " " << key;
    std::cerr << mess.str() << std::endl;
    throw GaudiException( mess.str(), "StandardPacker", StatusCode::FAILURE );
  }

  const int ID = (int)linkID(out,targetName);
  if ( ID != (ID & 0x0000000F) )
  {
    std::ostringstream mess;
    mess << "************************* LinkID over 4 bits in StandardPacker ***********************" 
         << " " << ID;
    std::cerr << mess.str() << std::endl;
    throw GaudiException( mess.str(), "StandardPacker", StatusCode::FAILURE );
  }

  const int myLinkID = (ID << 28);
  return key + myLinkID;
}
