// Include files 
#include "GaudiKernel/MsgStream.h"

// local
#include "MilleConfig.h"

using namespace std;

//-----------------------------------------------------------------------------
// Implementation file for class : MilleConfig
//
// 2005-07-28 : Sebastien Viret
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MilleConfig::MilleConfig(  )
{
  // Default values

  MilleConfig::SetInternal(false);   // Box alignment
  MilleConfig::SetLeft(0);          // Left box 
  MilleConfig::SetNlc(4);            // Num. of local params
  MilleConfig::SetNgb(10);           // Num. of global params per DOF
}

//=============================================================================
// Destructor
//=============================================================================
MilleConfig::~MilleConfig() {}; 

//=============================================================================


/*
----------------------------------------------------------------------------
  DEFINEMAP: Create the map of the stations to align 
----------------------------------------------------------------------------

 --> Update also the parameters necessary to include constraints 
 --> Set the number of global params/DOF (number of stations to align)

----------------------------------------------------------------------------
*/

void MilleConfig::DefineMap(double VELOmap[])
{
  for (int j=0; j<21; j++) {m_VELO_map[j] = VELOmap[j+21*MilleConfig::isLeft()];}

//
// The parameters zmoy** and s_zmoy** are used in the constraints
// We also get the first and last sensors here
//

  int ialc   = -1;
  int iblc   = 0;
  double zmoy   = 0.0;
  double s_zmoy = 0.0;
  int nonzer    = 0;

  for (int j=0; j<21; j++)
  {
    if (m_VELO_map[j] != -999.) 
    {
      if (ialc == -1) ialc=j;	 // first station
      iblc = j;       	     	 // last  station
      zmoy  += m_VELO_map[j];
      nonzer++;
    }
  }

  zmoy /= nonzer;
  
  for (int j=0; j<21; j++)
  {
    if (m_VELO_map[j] != -999.) 
    {
      s_zmoy += (m_VELO_map[j]-zmoy)*(m_VELO_map[j]-zmoy);
    }
  }

  s_zmoy /= nonzer;

  MilleConfig::Set_zmoy(zmoy);     // The mean z of the stations to align
  MilleConfig::Set_szmoy(s_zmoy);  // The corresponding StdDev
  MilleConfig::SetNgb(nonzer);     // The number of stations to align (could be different from iblc-ialc+1)
  MilleConfig::SetFirst(ialc);     // The first station to align
  MilleConfig::SetLast(iblc);      // The last station to align

}
