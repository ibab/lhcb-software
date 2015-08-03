// Include files 
#include <iostream>

// local
#include "ByteStreamTests.h"
#include "Event/SmartBank.h"
#include "Event/ByteStream.h"
#include "Event/BankWriter.h"

using namespace LHCb;
using namespace DAQEventTests;
//-----------------------------------------------------------------------------
// Implementation file for class : ByteStreamTests
//
// 2005-10-13 : Matt Needham
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_NAMESPACE_ALGORITHM_FACTORY(DAQEventTests,ByteStreamTests)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ByteStreamTests::ByteStreamTests( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{

}
//=============================================================================
// Destructor
//=============================================================================
ByteStreamTests::~ByteStreamTests() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode ByteStreamTests::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  BankWriter bw(6u);

  unsigned int a[3]; 
  unsigned char t1 = 6;
  unsigned short t2 = 0;
  unsigned int t3 = 12;
  bw << t1 << t2 << t3 << t3 << t3 << t2 << t1 ;

  info() << bw.dataBank().size() << endmsg;

  a[0] = 1+ (2<<8) + (3<<16) + (4<<24);
  info() << a[0] << endmsg;
  a[1] = 0;
  a[2] = 256;

  unsigned int* c = &a[0];

  SmartBank<char> cont(c,3*4);

  for (SmartBank<char>::iterator iter = cont.begin(); iter!= cont.end(); ++iter){
    info() << int(*iter)<< endmsg;
  }

  ByteStream smart(c,3*4); 
  unsigned char d; unsigned int e; unsigned short f;
  smart  >> e >> f >> d;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Execution
//=============================================================================
StatusCode ByteStreamTests::execute() {
  return StatusCode::SUCCESS;
}

//=============================================================================
