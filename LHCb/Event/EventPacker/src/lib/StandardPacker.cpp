
// local
#include "Event/StandardPacker.h"

// STL
#include <sstream>

// Gaudi
#include "GaudiKernel/GaudiException.h"
#include "GaudiAlg/GaudiAlgorithm.h"

//-----------------------------------------------------------------------------
// Implementation file for class : StandardPacker
//
// 2015-03-07 : Chris Jones
//-----------------------------------------------------------------------------

void StandardPacker::Warning( const std::string & mess ) const
{
  if ( parent() ) 
  { parent()->Warning( mess, StatusCode::SUCCESS ).ignore(); }
  else 
  { std::cout << "WARNING : StandardPacker : " << mess << std::endl; }
}

int StandardPacker::reference32( DataObject* out,
                                 const DataObject* parent,
                                 const int key ) const
{
  Warning( "Creating obsolete 32bit packed reference" );

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
  Warning( "Creating obsolete 32bit packed reference" );

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

bool StandardPacker::hintAndKey32( const int data,
                                   const DataObject* source,
                                   DataObject* target,
                                   int& hint,
                                   int& key ) const
{
  // return status is bad by default
  bool OK = false;

  // Proceed if target and source are OK
  if ( target && source && target->linkMgr() && source->linkMgr() )
  {
    // Extract the packed index and key from the data word
    int indx(0); indexAndKey32(data,indx,key);

    // Get the source link
    const auto * sourceLink = source->linkMgr()->link(indx);
    if ( sourceLink )
    {
      // If link is valid, saved to target and get the hint
      hint = target->linkMgr()->addLink( sourceLink->path(), 0 );
      // finally return status is OK
      OK = true;
    }

  }

  // If failed to extract the data, reset values
  if ( !OK ) { hint = key = 0; }

  // return final status
  return OK;
}

bool StandardPacker::hintAndKey64( const long long data,
                                   const DataObject* source,
                                   DataObject* target,
                                   int& hint,
                                   int& key ) const
{
  // return status is bad by default
  bool OK = false;

  // Proceed if target and source are OK
  if ( target && source && target->linkMgr() && source->linkMgr() )
  {
    // Extract the packed index and key from the data word
    int indx(0); indexAndKey64(data,indx,key);

    // Get the source link
    const auto * sourceLink = source->linkMgr()->link(indx);
    if ( sourceLink )
    {
      // If link is valid, saved to target and get the hint
      hint = target->linkMgr()->addLink( sourceLink->path(), 0 );
      // finally return status is OK
      OK = true;
    }
  }

  // If failed to extract the data, reset values
  if ( !OK ) { hint = key = 0; }

  // return final status
  return OK;
}
