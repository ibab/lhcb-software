// Include files

// local
#include "VeloTrack.h"
using namespace std; 

//-----------------------------------------------------------------------------
// Implementation file for class : VeloTrack
//
// 2007-09-03 : Sebastien Viret 
//-----------------------------------------------------------------------------


//------------------------------------------------------------------
//  GETTRACKSLOPE: get the track parameters
//------------------------------------------------------------------
//
//  atrack      = the track to be processed and updated
//
//------------------------------------------------------------------


void VeloTrack::GetTrackSlope()
{
  int Ncoords      = m_nGoodCoordinates;

  double local_vecx[2];
  double local_matx[2][2];
  double local_vecy[2];
  double local_maty[2][2];

  double sx, sy, cx, cy, err_cx, err_cy;
	
  double wghtx, rmeasx, detx;
  double wghty, rmeasy, dety;
  double z_cor;

  for (int i=0; i<2; i++) // reset local params
  {
    local_vecx[i] = 0.0;
    local_vecy[i] = 0.0;

    for (int j=0; j<2; j++)
    {
      local_matx[i][j] = 0.0;
      local_maty[i][j] = 0.0;
    }
  }

  for (int k=0; k<Ncoords; k++)
  {

    z_cor = ((m_Coords[k]).first).z();
    wghtx = ((m_Coords[k]).second).x();
    rmeasx = ((m_Coords[k]).first).x();
    wghty = ((m_Coords[k]).second).y();
    rmeasy = ((m_Coords[k]).first).y();

    local_vecx[0] += wghtx*rmeasx;
    local_vecx[1] += wghtx*rmeasx*z_cor;
    local_vecy[0] += wghty*rmeasy;
    local_vecy[1] += wghty*rmeasy*z_cor;

    local_matx[0][0] += wghtx;
    local_matx[1][0] += wghtx*z_cor;
    local_matx[1][1] += wghtx*z_cor*z_cor;
    local_maty[0][0] += wghty;
    local_maty[1][0] += wghty*z_cor;
    local_maty[1][1] += wghty*z_cor*z_cor;					
  }

  detx = 0.;
  dety = 0.;

  detx = local_matx[0][0]*local_matx[1][1]-local_matx[1][0]*local_matx[1][0];
  dety = local_maty[0][0]*local_maty[1][1]-local_maty[1][0]*local_maty[1][0];

  if (detx == 0. || dety == 0.) 
  {
    m_nIsGood = false;
    return;
  }

  sx = (local_vecx[1]*local_matx[0][0]-local_vecx[0]*local_matx[1][0])/detx;
  sy = (local_vecy[1]*local_maty[0][0]-local_vecy[0]*local_maty[1][0])/dety;
  cx = (local_vecx[0]*local_matx[1][1]-local_vecx[1]*local_matx[1][0])/detx;
  cy = (local_vecy[0]*local_maty[1][1]-local_vecy[1]*local_maty[1][0])/dety;

  m_nSlope_x = sx;
  m_nSlope_y = sy;

  m_nXo_x = cx;
  m_nYo_y = cy;

  double zclos = 0.;
	  
  if (sx!=0.0 || sy!=0.0) zclos  = -(sx*cx+sy*cy)/(sx*sx+sy*sy);

  err_cx = sqrt(fabs(local_matx[1][1]/detx));
  err_cy = sqrt(fabs(local_maty[1][1]/dety));

  m_nErrX_x = err_cx;
  m_nErrY_y = err_cy;

  m_nZclos  = zclos;
  m_nIsGood = true;

  return; 
}

