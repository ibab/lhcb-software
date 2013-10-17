// $Id: $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "VertexFunctionToolStarMax.h"
// Implementation file for class : VertexFunctionToolStarMax

//-----------------------------------------------------------------------------
/** @class VertexFunctionToolStarMax VertexFunctionToolStarMax.h
 *   This tool implements IVertexFunctionTool. See the interface class for a description
 *   of the purpose of these classes.
 *   This class uses the star based maximum Vf finder algorithm. This algorithm trytop
 *
 *  @author Julien Cogan and Mathieu Perrin-Terrin
 *  @date   2012-07-27
 */

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( VertexFunctionToolStarMax )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VertexFunctionToolStarMax::VertexFunctionToolStarMax( const std::string& type,
                                                      const std::string& name,
                                                      const IInterface* parent )
: VertexFunctionTool ( type, name, parent )
{

}

//=============================================================================
void VertexFunctionToolStarMax::computeValueMax(LHCb::RecVertex &V, Gaudi::XYZPoint &PMax, double &Max
                                                , double &step, int &NumberOfIterations)
{

  /**
   * This function look for the nearest point to the vertex where the vertex function (Vf) reaches a maximum.
   * It takes the vertex as input plus the current postion, max, step and iteration. The last ones are updated
   * each time the function is recursively called.
   * The fist time the function is called, it sets the current postion to the vertex position
   * Then its compute the gradient of the Vf at the current position.
   * If the gradient is too small the function stops so that the current postion and max are returned.
   * Else, it compute by calling the ParabolicExtrapolator how far it should translate along the gradient.
   * If the distance is null (cf ParabolicExtrapolator) it stops so that the current postion and max are returned.
   * Else, it update the current position and max to the new point.
   * If the maximum of iteration is reached  or if the translation was too small,
   * it stops and the point with the largest between the current and the new one Vf is returned.
   * Else if the distance from the initial position of the vertex is too large it stop and return the cuurent position and max
   * Else it checks what happens if it translate half, if the Vf is larger then the new point is updated.
   * If the step used to evaluate the gradiant is smaller than the translation then the step is step to half the ranslation.
   * Finally the function make a recurvise call with the updated step, postion, max value and number of iteration.
   */

  //if first recursive call then initialise correctly PMax and Max
  if(NumberOfIterations==0){
    PMax=V.position();
    Max=valueAt(V);
    step=m_step;
  }
  step=0.002;

  debug()<<"step : "<<step<<endmsg;

  Gaudi::XYZPoint CurrentPos = PMax;
  double CurrentVal = Max;

  //compute the gradient
  Gaudi::XYZPoint nextposX = CurrentPos + Gaudi::XYZVector(step, 0  , 0);
  Gaudi::XYZPoint nextposY = CurrentPos + Gaudi::XYZVector(0   ,step, 0);
  Gaudi::XYZPoint nextposZ = CurrentPos + Gaudi::XYZVector(0   ,0   ,step);
  Gaudi::XYZPoint nextposXY = CurrentPos + Gaudi::XYZVector(step/sqrt(2), step/sqrt(2)  , 0);
  Gaudi::XYZPoint nextposYZ = CurrentPos + Gaudi::XYZVector(0   ,step/sqrt(2), step/sqrt(2));
  Gaudi::XYZPoint nextposZX = CurrentPos + Gaudi::XYZVector(step/sqrt(2)   ,0   ,step/sqrt(2));
  Gaudi::XYZPoint nextposXm = CurrentPos + Gaudi::XYZVector(-step, 0  , 0);
  Gaudi::XYZPoint nextposYm = CurrentPos + Gaudi::XYZVector(0   ,-step, 0);
  Gaudi::XYZPoint nextposZm = CurrentPos + Gaudi::XYZVector(0   ,0   ,-step);
  Gaudi::XYZPoint nextposXYmm = CurrentPos + Gaudi::XYZVector(-step/sqrt(2), -step/sqrt(2)  , 0);
  Gaudi::XYZPoint nextposYZmm = CurrentPos + Gaudi::XYZVector(0   ,-step/sqrt(2), -step/sqrt(2));
  Gaudi::XYZPoint nextposZXmm = CurrentPos + Gaudi::XYZVector(-step/sqrt(2)   ,0   ,-step/sqrt(2));
  Gaudi::XYZPoint nextposXmY = CurrentPos + Gaudi::XYZVector(-step/sqrt(2), step/sqrt(2)  , 0);
  Gaudi::XYZPoint nextposYmZ = CurrentPos + Gaudi::XYZVector(0   ,-step/sqrt(2), step/sqrt(2));
  Gaudi::XYZPoint nextposZmX = CurrentPos + Gaudi::XYZVector(step/sqrt(2)   ,0   ,-step/sqrt(2));
  Gaudi::XYZPoint nextposXYm = CurrentPos + Gaudi::XYZVector(step/sqrt(2), -step/sqrt(2)  , 0);
  Gaudi::XYZPoint nextposYZm = CurrentPos + Gaudi::XYZVector(0   ,step/sqrt(2), -step/sqrt(2));
  Gaudi::XYZPoint nextposZXm = CurrentPos + Gaudi::XYZVector(-step/sqrt(2)   ,0   ,step/sqrt(2));
  Gaudi::XYZPoint nextposXYZmmm = CurrentPos + Gaudi::XYZVector(-step/sqrt(3)   ,-step/sqrt(3)   ,-step/sqrt(3));
  Gaudi::XYZPoint nextposXYmZm = CurrentPos + Gaudi::XYZVector(step/sqrt(3)   ,-step/sqrt(3)   ,-step/sqrt(3));
  Gaudi::XYZPoint nextposXmYZm = CurrentPos + Gaudi::XYZVector(-step/sqrt(3)   ,step/sqrt(3)   ,-step/sqrt(3));
  Gaudi::XYZPoint nextposXmYmZ = CurrentPos + Gaudi::XYZVector(-step/sqrt(3)   ,-step/sqrt(3)   ,step/sqrt(3));
  Gaudi::XYZPoint nextposXmYZ = CurrentPos + Gaudi::XYZVector(-step/sqrt(3)   ,step/sqrt(3)   ,step/sqrt(3));
  Gaudi::XYZPoint nextposXYmZ = CurrentPos + Gaudi::XYZVector(step/sqrt(3)   ,-step/sqrt(3)   ,step/sqrt(3));
  Gaudi::XYZPoint nextposXYZm = CurrentPos + Gaudi::XYZVector(step/sqrt(3)   ,step/sqrt(3)   ,-step/sqrt(3));
  Gaudi::XYZPoint nextposXYZ = CurrentPos + Gaudi::XYZVector(step/sqrt(3)   ,step/sqrt(3)   ,step/sqrt(3));
  Gaudi::XYZVector gradient;
  /*
    debug()<<"gradientxyz "<< gradient<<endmsg;
    debug()<<"VFatNow  "<< CurrentVal<<endmsg;
    debug()<<"VFatX    "<< this->computeValueAt(nextposX)<<endmsg;
    debug()<<"VFatY    "<< this->computeValueAt(nextposY)<<endmsg;
    debug()<<"VFatZ    "<< this->computeValueAt(nextposZ)<<endmsg;
    debug()<<"VFatXY   "<< this->computeValueAt(nextposXY)<<endmsg;
    debug()<<"VFatYZ   "<< this->computeValueAt(nextposYZ)<<endmsg;
    debug()<<"VFatZX   "<< this->computeValueAt(nextposZX)<<endmsg;
    debug()<<"VFatXYZ  "<< this->computeValueAt(nextposXYZ)<<endmsg;
    debug()<<"VFatXm   "<< this->computeValueAt(nextposXm)<<endmsg;
    debug()<<"VFatYm   "<< this->computeValueAt(nextposYm)<<endmsg;
    debug()<<"VFatZm   "<< this->computeValueAt(nextposZm)<<endmsg;
    debug()<<"VFatXYm  "<< this->computeValueAt(nextposXYm)<<endmsg;
    debug()<<"VFatYZm  "<< this->computeValueAt(nextposYZm)<<endmsg;
    debug()<<"VFatZXm  "<< this->computeValueAt(nextposZXm)<<endmsg;
    debug()<<"VFatXYmm  "<< this->computeValueAt(nextposXYmm)<<endmsg;
    debug()<<"VFatYZmm  "<< this->computeValueAt(nextposYZmm)<<endmsg;
    debug()<<"VFatZXmm  "<< this->computeValueAt(nextposZXmm)<<endmsg;
    debug()<<"VFatXYm  "<< this->computeValueAt(nextposXmY)<<endmsg;
    debug()<<"VFatYmZ  "<< this->computeValueAt(nextposYmZ)<<endmsg;
    debug()<<"VFatZmX  "<< this->computeValueAt(nextposZmX)<<endmsg;
    debug()<<"VFatXmYZ "<< this->computeValueAt(nextposXmYZ)<<endmsg;
    debug()<<"VFatXYmZ "<< this->computeValueAt(nextposXYmZ)<<endmsg;
    debug()<<"VFatXYZm "<< this->computeValueAt(nextposXYZm)<<endmsg;
    debug()<<"VFatXYmZm "<< this->computeValueAt(nextposXYmZm)<<endmsg;
    debug()<<"VFatXmYZm "<< this->computeValueAt(nextposXmYZm)<<endmsg;
    debug()<<"VFatXmYmZ "<< this->computeValueAt(nextposXmYmZ)<<endmsg;
    debug()<<"VFatXYZmmm "<< this->computeValueAt(nextposXYZmmm)<<endmsg;
  */
  if (this->computeValueAt(nextposXY)> CurrentVal)
  {
    CurrentVal = this->computeValueAt(nextposXY);
    gradient.SetX(1. );
    gradient.SetY(1. );
    gradient.SetZ(0. );
  }
  if (this->computeValueAt(nextposYZ)> CurrentVal)
  {
    CurrentVal = this->computeValueAt(nextposYZ);
    gradient.SetY(1. );
    gradient.SetZ(1. );
    gradient.SetX(0. );
  }
  if (this->computeValueAt(nextposZX)> CurrentVal)
  {
    CurrentVal = this->computeValueAt(nextposZX);
    gradient.SetZ(1. );
    gradient.SetX(1. );
    gradient.SetY(0. );
  }
  if (this->computeValueAt(nextposXmY)> CurrentVal)
  {
    CurrentVal = this->computeValueAt(nextposXmY);
    gradient.SetX(-1. );
    gradient.SetY(1. );
    gradient.SetZ(0. );
  }
  if (this->computeValueAt(nextposYmZ)> CurrentVal)
  {
    CurrentVal = this->computeValueAt(nextposYmZ);
    gradient.SetY(-1. );
    gradient.SetZ(1. );
    gradient.SetX(0. );
  }
  if (this->computeValueAt(nextposZmX)> CurrentVal)
  {
    CurrentVal = this->computeValueAt(nextposZmX);
    gradient.SetZ(-1. );
    gradient.SetX(1. );
    gradient.SetY(0. );
  }
  if (this->computeValueAt(nextposXYm)> CurrentVal)
  {
    CurrentVal = this->computeValueAt(nextposXYm);
    gradient.SetX(1. );
    gradient.SetY(-1. );
    gradient.SetZ(0. );
  }
  if (this->computeValueAt(nextposYZm)> CurrentVal)
  {
    CurrentVal = this->computeValueAt(nextposYZm);
    gradient.SetY(1. );
    gradient.SetZ(-1. );
    gradient.SetX(0. );
  }
  if (this->computeValueAt(nextposZXm)> CurrentVal)
  {
    CurrentVal = this->computeValueAt(nextposZXm);
    gradient.SetZ(1. );
    gradient.SetX(-1. );
    gradient.SetY(0. );
  }
  if (this->computeValueAt(nextposXYmm)> CurrentVal)
  {
    CurrentVal = this->computeValueAt(nextposXYmm);
    gradient.SetX(-1. );
    gradient.SetY(-1. );
    gradient.SetZ(0. );
  }
  if (this->computeValueAt(nextposYZmm)> CurrentVal)
  {
    CurrentVal = this->computeValueAt(nextposYZmm);
    gradient.SetY(-1. );
    gradient.SetZ(-1. );
    gradient.SetX(0. );
  }
  if (this->computeValueAt(nextposZXmm)> CurrentVal)
  {
    CurrentVal = this->computeValueAt(nextposZXmm);
    gradient.SetZ(-1. );
    gradient.SetX(-1. );
    gradient.SetY(0. );
  }
  if (this->computeValueAt(nextposX)> CurrentVal)
  {
    CurrentVal = this->computeValueAt(nextposX);
    gradient.SetX(1. );
    gradient.SetY(0. );
    gradient.SetZ(0. );
  }
  if (this->computeValueAt(nextposY)> CurrentVal)
  {
    CurrentVal = this->computeValueAt(nextposY);
    gradient.SetX(0. );
    gradient.SetY(1. );
    gradient.SetZ(0. );
  }
  if (this->computeValueAt(nextposZ)> CurrentVal)
  {
    CurrentVal = this->computeValueAt(nextposZ);
    gradient.SetX(0. );
    gradient.SetY(0. );
    gradient.SetZ(1. );
  }
  if (this->computeValueAt(nextposXm)> CurrentVal)
  {
    CurrentVal = this->computeValueAt(nextposXm);
    gradient.SetX(-1. );
    gradient.SetY(0. );
    gradient.SetZ(0. );
  }
  if (this->computeValueAt(nextposYm)> CurrentVal)
  {
    CurrentVal = this->computeValueAt(nextposYm);
    gradient.SetX(0. );
    gradient.SetY(-1. );
    gradient.SetZ(0. );
  }
  if (this->computeValueAt(nextposZm)> CurrentVal)
  {
    CurrentVal = this->computeValueAt(nextposZm);
    gradient.SetX(0. );
    gradient.SetY(0. );
    gradient.SetZ(-1. );
  }
  if (this->computeValueAt(nextposXYZ)> CurrentVal)
  {
    CurrentVal = this->computeValueAt(nextposXYZ);
    gradient.SetX(1. );
    gradient.SetY(1. );
    gradient.SetZ(1. );
  }
  if (this->computeValueAt(nextposXYZmmm)> CurrentVal)
  {
    CurrentVal = this->computeValueAt(nextposXYZmmm);
    gradient.SetX(-1. );
    gradient.SetY(-1. );
    gradient.SetZ(-1. );
  }
  if (this->computeValueAt(nextposXYZm)> CurrentVal)
  {
    CurrentVal = this->computeValueAt(nextposXYZm);
    gradient.SetX(1. );
    gradient.SetY(1. );
    gradient.SetZ(-1. );
  }
  if (this->computeValueAt(nextposXYmZ)> CurrentVal)
  {
    CurrentVal = this->computeValueAt(nextposXYmZ);
    gradient.SetX(1. );
    gradient.SetY(-1. );
    gradient.SetZ(1. );
  }
  if (this->computeValueAt(nextposXmYZ)> CurrentVal)
  {
    CurrentVal = this->computeValueAt(nextposXmYZ);
    gradient.SetX(-1. );
    gradient.SetY(1. );
    gradient.SetZ(1. );
  }
  if (this->computeValueAt(nextposXmYmZ)> CurrentVal)
  {
    CurrentVal = this->computeValueAt(nextposXmYmZ);
    gradient.SetX(-1. );
    gradient.SetY(-1. );
    gradient.SetZ(1. );
  }
  if (this->computeValueAt(nextposXmYZm)> CurrentVal)
  {
    CurrentVal = this->computeValueAt(nextposXmYZm);
    gradient.SetX(-1. );
    gradient.SetY(1. );
    gradient.SetZ(-1. );
  }
  if (this->computeValueAt(nextposXYmZm)> CurrentVal)
  {
    CurrentVal = this->computeValueAt(nextposXYmZm);
    gradient.SetX(1. );
    gradient.SetY(-1. );
    gradient.SetZ(-1. );
  }

  double stepExtra = 5*step;

  double gradient_mag = gradient.R();
  debug()<<"Max Findex iteration "<< NumberOfIterations<<endmsg;
  debug()<<"\tCurrent Value:"<< CurrentVal<<endmsg;
  debug()<<"\tGradient: "<< gradient_mag << "  component: "<<gradient<< endmsg;
  if ( gradient_mag <0.5){
    debug()<<"We're at the top "<<endmsg;
    return;
  }
  gradient*=(1.0 / gradient_mag);


  double dist = ParabolicExtrapolator(gradient, CurrentPos, stepExtra);
  debug()<<"\tDist Evaluation: "<<dist<<endmsg;
  NumberOfIterations++;



  double absDist = fabs(dist);
  //update Pmax and Max
  PMax= CurrentPos + (absDist * gradient);
  Max = computeValueAt(PMax);
  debug()<<"\tNew Value: "<<Max<< "\tOld:"<<CurrentVal<<endmsg;
  // Decide what to do:

  if (NumberOfIterations >= m_max_finder_max_iteration || absDist < m_max_finder_min_step){
    // Return the best known point if it is better than initialPoint:
    if(NumberOfIterations >= m_max_finder_max_iteration) warning()<<" computeValueMax too many iteration "
                                                                  << NumberOfIterations <<" vs "
                                                                  << m_max_finder_max_iteration <<endmsg;

    if (Max > CurrentVal){
      debug()<<"\tStep too small or too many, stop at new position (max:"<< Max<<")"<<endmsg;
      return;
    }
    else{
      //step back
      PMax = CurrentPos;
      Max = CurrentVal;
      debug()<<"\tStep too small or too many, stop at new previous (max:"<< Max<<")"<<endmsg;
      return;
    }
  }


  else{
    //if we went too far
    Gaudi::XYZVector distanceCurrentNext = PMax-V.position();
    if (distanceCurrentNext.rho()>m_max_finder_max_jump){
      PMax = CurrentPos;
      Max = CurrentVal;
      debug()<<"\tJumped Too Far Since begining, stop at new previous (max:"<< Max<<")"<<endmsg;
      return;
    }
    else{
      Gaudi::XYZPoint jumpPoint = CurrentPos + (absDist* 0.2)* gradient;
      double jumpPot = computeValueAt(jumpPoint);

      if (jumpPot > Max){
        //update
        PMax = jumpPoint;
        Max=jumpPot;
        debug()<<"\tJump Partially ("<< "0.2" <<") go to next Jump"<<endmsg;
      }

      // Recursive call:
      return computeValueMax(V,PMax, Max, step,NumberOfIterations);
    }
  }
}

//=============================================================================
// Destructor
//=============================================================================
VertexFunctionToolStarMax::~VertexFunctionToolStarMax() {}

//=============================================================================
