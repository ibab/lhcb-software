// $Id: SimTell1Engine.cpp,v 1.1 2009-08-10 08:48:01 szumlat Exp $
// Include files

// local
#include "SimTell1Engine.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SimTell1Engine
//
// 2009-08-01 : Tomasz Szumlak
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SimTell1Engine::SimTell1Engine(  ):
  m_processEnable ( 0 ),
  m_dataSize ( sizeof(VeloTELL1::Data)/sizeof(int) ),
  m_linkPedestalSize ( sizeof(VeloTELL1::LinkPedestal)/sizeof(VeloTELL1::u_int16_t) ),
  m_linkMaskSize ( sizeof(VeloTELL1::LinkMask)/sizeof(VeloTELL1::u_int8_t) ),
  m_inData ( 0 ),
  m_outData ( m_dataSize, 0 )
{ }
//=============================================================================
// Destructor
//=============================================================================
SimTell1Engine::~SimTell1Engine() {}
//--
