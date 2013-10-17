// $Id: $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "VertexFunctionTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VertexFunctionTool
//
// 2012-07-27 : Julien Cogan and Mathieu Perrin-Terrin
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( VertexFunctionTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VertexFunctionTool::VertexFunctionTool( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
: GaudiTool ( type, name, parent )
{
  declareInterface<IVertexFunctionTool>(this);


}
//=============================================================================
// Destructor
//=============================================================================
VertexFunctionTool::~VertexFunctionTool() {}

//=============================================================================
StatusCode VertexFunctionTool::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;

  // m_Geom = tool<IDistanceCalculator> ("LoKi::DistanceCalculator", "DistanceCalculator",this);
  // if ( ! m_Geom ) {
  //   fatal() << "DistanceCalculator could not be found" << endreq;
  //   return StatusCode::FAILURE;
  // }

  m_Geom = tool<IDistanceCalculator> ("LoKi::TrgDistanceCalculator", "DistanceCalculator",this);

  m_Trans = tool<ITrackExtrapolator> ("TrackLinearExtrapolator", "Extrapolator",this);
 
  return sc;
}


void VertexFunctionTool::sortByValueAt(std::vector<LHCb::RecVertex *> & vVtx)
{
  //====================================================================
  // This functin sorts a list of vector by descending vertex function
  // value at the vertex fitted position
  //====================================================================

  stable_sort ( vVtx.begin(),  vVtx.end(), orderValueAt(this) );
}




void VertexFunctionTool::sortByValueMax(std::vector<LHCb::RecVertex *> & vVtx)
{
  //====================================================================
  // This functin sorts a list of vector by descending vertex function
  // value at the vertex closest maximum position
  //====================================================================

  stable_sort ( vVtx.begin(),  vVtx.end(),orderValueMax(this));
}



double VertexFunctionTool::valueAt(std::vector<const LHCb::Track*> & vTr, LHCb::RecVertex & V)
{
  //====================================================================
  // This function set the traks to be used in the vertex function and
  // then returns the value of the vertex function at the fitted
  // vertex position. The value is store in the vertex extra info.
  //====================================================================

  std::vector<const LHCb::Track*> & tmp_vTr = m_vTr;
  setTracks(vTr);
  double value = valueAt(V);
  setTracks(tmp_vTr);
  return value;

}



double VertexFunctionTool::valueAt(LHCb::RecVertex & V)
{
  //====================================================================
  // This functionreturns the value of the vertex function at the fitted
  // vertex position. The value is store in the vertex extra info.
  //====================================================================

  //check if the vertex function value has already been computed
  if(V.hasInfo((int)VfAt) == true) return V.info((int)VfAt,0);

  //if the ertex function value has not been computed, do it
  Gaudi::XYZPoint Position = V.position();
  double VfValue = computeValueAt(Position);
  setVfAt(V, VfValue);
  return VfValue;
}



double VertexFunctionTool::computeValueAt(Gaudi::XYZPoint & P)
{

  //====================================================================
  // This function computes the value of the vertex function at
  // a given position.
  //====================================================================
  double CpC=0  ;
  double SumGaussT=0  ;
  double SumGaussTSquared=0  ;
  int index =0;
  const LHCb::VertexBase* vv = new LHCb::VertexBase(P);

  for(std::vector< const LHCb::Track* >::iterator it= m_vTr.begin(); it!=m_vTr.end(); ++it){
    index++;
    CpC=0  ;

    //get the z of the IMP
    Gaudi::XYZVector  imp;
    StatusCode Sc_m_Geom = m_Geom->distance((*it),vv,imp);
    if(Sc_m_Geom == StatusCode::FAILURE  || imp.X()!=imp.X()) {
      warning()<<"m_Geom->distance return failed status code or NaN! Track ignore. imp2.X()="<<imp.X()<<endmsg;
      continue;
    }

    // Gaudi::XYZVector imp2 = impactParam(*(*it),*vv);
    // if(imp.X()-imp2.X() > 0.0001 ||imp.X()-imp2.X() < -0.0001 ){
    //   std::cout<< "ERROR in the computation of the impact param "<<std::endl;
    //   std::cout<< imp2<<std::endl;
    //   std::cout<< "Correct one " << imp<<std::endl;
    // }

    //move the part at z = z(imp)+z(P) and get the covariance matrix
    Gaudi::XYZPoint pos;
    Gaudi::SymMatrix3x3 errPos3D;
    StatusCode Sc_m_Trans = m_Trans->position(*(*it), P.Z()+imp.Z(), pos, errPos3D);
    if(Sc_m_Trans == StatusCode::FAILURE || errPos3D[0][0]!=errPos3D[0][0]){
      warning()<<"m_Trans->position return failed status code or NaN!  errPos3D[0][0]="<<errPos3D[0][0]<<endmsg;
      continue;
    }

    //Reduce matrix and Imp vector
    Gaudi::SymMatrix2x2  errPos = errPos3D.Sub<Gaudi::SymMatrix2x2>   (0,0);
    //    std::cout<<"Matrix 3D\n"<<errPos3D<<" \n\nMatrix2D\n"<<errPos<<std::endl;

    ROOT::Math::SVector<double,2> Simp;
    Simp[0]=imp.X();
    Simp[1]=imp.Y();

    //    Simp[2]=imp.Z();

    // DEBUGGING OUTPUT
    // if ( msgLevel(MSG::DEBUG) ){
    //   Gaudi::SymMatrix6x6 cov6D;
    //   (*it)->posMomCovariance (cov6D);
    //   debug()<<" Distance from track: "<<pow(imp.Mag2(),0.5)<<"  Coordinate: "<<imp <<endmsg;
    //   debug()<<" Z of P: " << P.Z()<<"  Z of Imp: " <<imp.Z()<<"  Z to transport track: "<<  P.Z()+imp.Z()<< endmsg;
    //   debug()<<" Z before transport: "<<((*it)->position()).Z()  <<"   Cov(0,0) before transport:"<<cov6D[0][0] <<endmsg;
    //   debug()<<" Z after transport:  "<<pos.Z()                  <<"   Cov(0,0) after transport :"<<errPos[0][0]<<endmsg;
    // }
    // Gaudi::SymMatrix6x6 cov6D;
    // (*it)->posMomCovariance (cov6D);
    // std::cout<<" Distance from track: "<<pow(imp.Mag2(),0.5)<<"  Coordinate of Imp: "<<imp <<std::endl;
    // std::cout<<" Z of P: " << P.Z()<<"  Z of Imp: " <<imp.Z()<<"  Z to transport track: "<<  P.Z()+imp.Z()<<std::endl ;
    // std::cout<<" Z before transport: "<<((*it)->position()).Z()  <<"   Cov(0,0) before transport:"<<cov6D[0][0] <<std::endl;
    // std::cout<<" Z after transport:  "<<pos.Z()                  <<"   Cov(0,0) after transport :"<<errPos[0][0]<<std::endl;
    // print matrix covariance to check if the z componenet is null,
    // if so reduce the matrice to speed up the product and invertion
    // std::cout<<"Covariance Matix Transported:\n"<<errPos<<std::endl;


    //Invert the 2-D Covariance Matrix
    bool isInv = errPos.Invert();
    if (!isInv || errPos[0][0]!=errPos[0][0]) {
      warning()<<"Covariance matrice could not be inverted !!  errPos[0][0]="<<errPos[0][0] << endmsg;
      continue;
    }
    //debug()<<" Covariance Matrix :"<<errPos<<endmsg;

    //Compute the track contribution to the vertex function
    CpC = ROOT::Math::Similarity(errPos,Simp);

    //CpC = Simp[0]*Simp[0]*errPos[0][0]+Simp[1]*Simp[1]*errPos[1][1]+2*Simp[0]*Simp[1]*errPos[0][1];


    if(CpC<0){
      warning()<<"Imp * InvCov TransImp is negative track "<< index <<" skipped !!\n"
               <<"    CpC"<< CpC << " Simp x="<< Simp[0]<<"  y="<< Simp[1] << "  Cov ="<<errPos<<endmsg;
      continue;
    }
    if(CpC>40) {
      //warning()<<"CpC>5, "<<CpC<<" contribution set to 0!! "<< endmsg;
      continue;
    }

    double tmpSumGaussT = exp(-0.5*CpC);

    if(tmpSumGaussT != tmpSumGaussT) {
      warning()<<"NaN in the gauss tube !! "<< SumGaussT<< "  last CpC:"<<CpC
               <<"\n Err Mat:"<< errPos <<"\n Imp:\n"<< Simp<< endmsg;

      continue;

    }

    SumGaussT += exp(-0.5*CpC);
    SumGaussTSquared += exp(-0.5*CpC)*exp(-0.5*CpC);

    // DEBUGGING OUTPUT
    //std::cout<<"VfAt \t\t"<<index<< " gauss tube:"<< exp(-0.5*CpC)<<"  sum: "<<SumGaussT<<"  sum2: "<<SumGaussTSquared
    //                  << "  Vf: "<<SumGaussT-SumGaussTSquared/SumGaussT <<std::endl;
  }
  if(SumGaussT == 0. ) return 0.;
  delete vv;
  return SumGaussT-SumGaussTSquared/SumGaussT ;

}



double VertexFunctionTool::computeSumGaussTubeAt(Gaudi::XYZPoint & P)
{
  //====================================================================
  // This function compute the sum of the gauss tube at a given postion.
  //====================================================================
  double CpC=0  ;
  double SumGaussT=0  ;
  int index =0;
  const LHCb::VertexBase* vv = new LHCb::VertexBase(P);

  for(std::vector< const LHCb::Track* >::iterator it= m_vTr.begin(); it!=m_vTr.end(); ++it){
    index++;
    CpC=0  ;

    //get the z of the IMP
    Gaudi::XYZVector  imp;
    StatusCode Sc_m_Geom = m_Geom->distance((*it),vv,imp);
    if(Sc_m_Geom == StatusCode::FAILURE  || imp.X()!=imp.X()) {
      warning()<<"m_Geom->distance return failed status code or NaN! Track ignore. imp2.X()="<<imp.X()  <<endmsg;
      continue;
    }

    //move the part at z = z(imp)+z(P) and get the covariance matrix
    Gaudi::XYZPoint pos;
    Gaudi::SymMatrix3x3 errPos3D;
    StatusCode Sc_m_Trans = m_Trans->position(*(*it), P.Z()+imp.Z(), pos, errPos3D);
    if(Sc_m_Trans == StatusCode::FAILURE || errPos3D[0][0]!=errPos3D[0][0]){
      warning()<<"m_Trans->position return failed status code or NaN!  errPos3D[0][0]="<<errPos3D[0][0]<<endmsg;
      continue;
    }

    //Reduce matrix and Imp vector
    Gaudi::SymMatrix2x2  errPos = errPos3D.Sub<Gaudi::SymMatrix2x2>   (0,0);
    //    std::cout<<"Matrix 3D\n"<<errPos3D<<" \n\nMatrix2D\n"<<errPos<<std::endl;

    ROOT::Math::SVector<double,2> Simp;
    Simp[0]=imp.X();
    Simp[1]=imp.Y();

    //Invert the 2-D Covariance Matrix
    bool isInv = errPos.Invert();
    if (!isInv || errPos[0][0]!=errPos[0][0]) {
      warning()<<"Covariance matrice could not be inverted !!  errPos[0][0]="<<errPos[0][0] << endmsg;
      continue;
    }


    //Compute the track contribution to the vertex function
    CpC = ROOT::Math::Similarity(errPos,Simp);

    if(CpC<0){
      warning()<<"Imp * InvCov TransImp is negative track "<< index <<" skipped !!\n"
               <<"    CpC"<< CpC << " Simp x="<< Simp[0]<<"  y="<< Simp[1] << "  Cov ="<<errPos<<endmsg;
      continue;
    }
    if(CpC>40) {
      //warning()<<"CpC>5, "<<CpC<<" contribution set to 0!! "<< endmsg;
      continue;
    }

    double tmpSumGaussT = exp(-0.5*CpC);

    if(tmpSumGaussT != tmpSumGaussT) {
      warning()<<"NaN in the gauss tube !! "<< SumGaussT<< "  last CpC:"<<CpC
               <<"\n Err Mat:"<< errPos <<"\n Imp:\n"<< Simp<< endmsg;

      continue;

    }

    SumGaussT += exp(-0.5*CpC);

  }
  delete vv;
  return SumGaussT;

}

double VertexFunctionTool::valueMax(LHCb::RecVertex & V)
{

  //====================================================================
  // This function return the nearest local maximum around the vertex
  // intial postion. The maximum and its positions are stored in the
  // vertex extra infos.
  //====================================================================

  //check if the vertex function value has already been computed
  if(V.hasInfo((int)VfMax) == true) {
    double vmax = V.info((int)VfMax,0);
    //info()<<"VertexFunctionTool::valueMax for "<<&V<<" already computed = "<<vmax<<endmsg;
    return vmax;
  }

  //if the ertex function value has not been computed, do it
  Gaudi::XYZPoint PositionMax(0,0,0);
  double MaxValue;
  computeValueMax(V, PositionMax, MaxValue);
  setVfMax(V, PositionMax, MaxValue);
  //info()<<"VertexFunctionTool::valueMax for "<<&V<<" new computation = "<<MaxValue<<endmsg;
  return MaxValue;
}

Gaudi::XYZPoint VertexFunctionTool::positionMax(LHCb::RecVertex & V)
{
  //====================================================================
  // This function returns the postion at the  nearest local maximum
  // around the vertex intial postion.
  //====================================================================
  Gaudi::XYZPoint  PositionMax;
  //check if the vertex function value has already been computed
  if(V.hasInfo((int)VfMaxPosX) == true && V.hasInfo((int)VfMaxPosY) == true && V.hasInfo((int)VfMaxPosZ) == true){
    PositionMax.SetX(V.info((int)VfMaxPosX,0));
    PositionMax.SetY(V.info((int)VfMaxPosY,0));
    PositionMax.SetZ(V.info((int)VfMaxPosZ,0));
  }
  else{

    //if the ertex function value has not been computed, do it
    double MaxValue;
    computeValueMax(V, PositionMax, MaxValue);
    setVfMax(V, PositionMax, MaxValue);
  }
  return PositionMax;
}

void VertexFunctionTool::valueMax(LHCb::RecVertex & V, Gaudi::XYZPoint & PMax, double & Max)
{

  //====================================================================
  // This function returns the value of  nearest local maximum
  // around the vertex intial postion.
  //====================================================================

  PMax = positionMax(V);
  Max = valueMax(V);
}



void VertexFunctionTool::computeValueMax(LHCb::RecVertex & V,Gaudi::XYZPoint & PMax, double & Max){

  //initialisation
  PMax = V.position();
  Max  = valueAt(V);
  Gaudi::XYZVector Previous_gradient(0., 0., 0.);
  double step = m_step;

  for (int ite =0; ite<m_max_finder_max_iteration; ite++){
    debug()<<"iteration :"<<ite<<endmsg;
    //compute the gradient
    Gaudi::XYZPoint CurrentPos(PMax);
    double CurrentVal = Max;
    Gaudi::XYZVector Current_gradient = computeGradientAt(CurrentPos);
    if (Current_gradient.R() < m_max_finder_min_gradient_mag){
      debug()<<"\tThe potential doesn't vary over reasonable distances: grad_mag = "<< Current_gradient.R()<<endmsg;
      break;
    }
    //average the gradient with the previous one
    Gaudi::XYZVector gradient = Current_gradient + Previous_gradient;
    Previous_gradient = Current_gradient;
    gradient /=gradient.R();



    //compute the new position and pot
    double absDist = fabs(ParabolicExtrapolator(gradient, CurrentPos, step));
    PMax= CurrentPos + (absDist * gradient);

    if(absDist<step){
      Gaudi::XYZVector gradientAtNewPoint = computeGradientAt(PMax);
      if (gradientAtNewPoint.R() > m_max_finder_min_gradient_mag){
        debug()<<"\tDist is small"<< absDist <<"but gradient at new point is large so MOVE!"<<endmsg;
        absDist = 2.0*step;
        PMax= CurrentPos + (absDist * gradient);
      }
    }
    Max = computeValueAt(PMax);

    debug()<<"\tNew Value: "<<Max<< "\tOld:"<<CurrentVal<<endmsg;

    //decide what to do:

    //CASE 1: STEP TOO SMALL
    if (absDist < m_max_finder_min_step){
      // Return the best known point if it is better than initialPoint
      if (Max > CurrentVal){
        debug()<<"\tStep too small (max:"<< Max<<")"<<endmsg;
        break;
      }
      else {
        PMax = CurrentPos;
        Max = CurrentVal;
        debug()<<"\tStep too small (max:"<< Max<<")"<<endmsg;
        break;
      }
    }
    //CASE 2: WENT TOO FAR
    Gaudi::XYZVector distanceInitNext = PMax-V.position();
    if (distanceInitNext.R()>m_max_finder_max_jump) {
      //go the the furthest position
      Gaudi::XYZVector distanceInitCurrent = CurrentPos-V.position();
      double tempVal = -distanceInitCurrent.Dot(gradient);
      double k =  tempVal+sqrt(tempVal*tempVal -distanceInitCurrent.Mag2() +
                               m_max_finder_max_jump*m_max_finder_max_jump);
      Gaudi::XYZPoint jumpPoint = CurrentPos + k * gradient;
      double jumpPot = computeValueAt(jumpPoint);
      if (jumpPot< Max){
        debug()<<"\tMax Finder went too far"<<endmsg;
        break;
      }
      else {
        PMax = jumpPoint;
        Max = jumpPot;
        absDist = k;
      }
    }
    //CASE 3: WENT OVER A PEAK
    if(CurrentVal > Max){
      int n_step_back = 0;
      double partial_step = 1.0;
      double jumpPot = 0;
      Gaudi::XYZPoint jumpPoint;
      while ( n_step_back<8 and jumpPot<CurrentVal){
        partial_step*=0.5;
        jumpPoint = CurrentPos + absDist*partial_step*gradient;
        jumpPot = computeValueAt(jumpPoint);
        n_step_back+=1;
      }
      debug()<<"\t step was divided by 2^"<<n_step_back<< " dist_part ="<<absDist* partial_step<<endmsg;
      PMax = jumpPoint;
      Max = jumpPot;
      absDist = absDist*partial_step;
      if (step<absDist){
        step = absDist / 2.0;
        debug()<<"\tStep Too Large compared to the jump we just made"<<endmsg;
      }
    }

    //CASE 4: check is half step was not better
    double partial_jump = 0.5;
    Gaudi::XYZPoint jumpPoint = CurrentPos + (absDist* partial_jump)* gradient;
    double  jumpPot = computeValueAt(jumpPoint);
    if (jumpPot > Max){  // it was better!
      PMax = jumpPoint;
      Max=jumpPot;
      debug()<<"\tJump Partially ("<< partial_jump <<") go to next Jump"<<endmsg;
    }
    if (step<absDist){
      step = absDist / 2.0;
      debug()<<"\tStep Too Large compared to the jump we just made"<<endmsg;
    }

  }
  //info()<<"VertexFunctionTool::computeValueMax for "<<&V<<" new computation = "<<Max<<endmsg;


}

bool   VertexFunctionTool::areResolved(LHCb::RecVertex & V1,LHCb::RecVertex & V2)
{
  //info()<<"VertexFunctionTool::areResolved "<<&V1<<" "<<&V2<<endmsg;

  //====================================================================
  // This function check if two vertices are resolved. Return true if so.
  //====================================================================
  std::vector <double> MeshPoints;
  //compute Max Vf  position of the 2 vertex

  Gaudi::XYZPoint P1(positionMax(V1));
  Gaudi::XYZPoint P2(positionMax(V2));
  double distance = (P2-P1).R();
  Gaudi::XYZVector Dir(P2-P1);
  Dir *= 1./distance;

  MeshPoints.push_back(0);
  MeshPoints.push_back(distance);

  //compute Vmin
  double Vmin = valueMax(V1);
  if(Vmin>valueMax(V2)) Vmin = valueMax(V2);
  if (Vmin <= 1e-8) return false;


  //int NumberOfIterations =0;
  //return areResolveDicotomy(P1,Dir,Vmin,MeshPoints,NumberOfIterations);

  return areResolveDicotomy(P1,Dir,Vmin,1,distance/2.);

}




bool VertexFunctionTool::areResolveDicotomy(Gaudi::XYZPoint P1,Gaudi::XYZVector Dir,double &Vmin
                                            ,int nMeshPoints, double step)
{
  //====================================================================
  // This function is used to recursively check if two vertices are resolved
  //====================================================================
  //info()<<"VertexFunctionTool::areResolvedDicotomy step "<<step<<endmsg;

  Gaudi::XYZPoint newP;

  for(int i_mesh=0; i_mesh<nMeshPoints; i_mesh+=2){

    //try new point
    newP = P1+(double(i_mesh+1)*step)*Dir;
    if((computeValueAt(newP)/Vmin)<m_resolver_cut) return 1;

  }

  step /=2.;
  nMeshPoints*=2;
  if((step< m_resolver_min_step) || (nMeshPoints>m_resolver_max_iteration)) return 0;

  return areResolveDicotomy(P1,Dir,Vmin,nMeshPoints,step);

  // return 0;


}

void VertexFunctionTool::printParam()
{
  std::cout<<"MaxFinderStep : " <<m_step<<std::endl;
  std::cout<<"MaxFinderMinGradientMag : "<<m_max_finder_min_gradient_mag<<std::endl;
  std::cout<<"MaxFinderMaxIteration : " <<m_max_finder_max_iteration<<std::endl;
  std::cout<<"MaxFinderMinStep : " <<m_max_finder_min_step<<std::endl;
  std::cout<<"MaxFinderMaxjump : " <<m_max_finder_max_jump<<std::endl;
  std::cout<<"ResolverCut : " <<m_resolver_cut<<std::endl;
  std::cout<<"ResolverMinStep : " <<m_resolver_min_step<<std::endl;
  std::cout<<"ResolverMaxIteration : " <<m_resolver_max_iteration<<std::endl;
}

void VertexFunctionTool::setParam(TString name, double value){
  if (name=="MaxFinderStep") m_step =value;
  else if (name=="MaxFinderMinGradientMag") m_max_finder_min_gradient_mag =value;
  else if (name=="MaxFinderMaxIteration") m_max_finder_max_iteration =value;
  else if (name=="MaxFinderMinStep") m_max_finder_min_step =value;
  else if (name=="MaxFinderMaxjump") m_max_finder_max_jump =value;
  else if (name=="ResolverCut") m_resolver_cut =value;
  else if (name=="ResolverMinStep") m_resolver_min_step =value;
  else if (name=="ResolverMaxIteration") m_resolver_max_iteration =value;
  else warning()<<name<< " is not a valid parameter name"<<endmsg;
  debug()<<name<< " set to "<<value<<endmsg;

}

//Private Function


void VertexFunctionTool::setVfMax(LHCb::RecVertex & V, Gaudi::XYZPoint & PositionMax, double & MaxValue)
{
  //clean up the old info before setting new one
  V.eraseInfo ((int)VfMax);
  V.eraseInfo ((int)VfMaxPosX);
  V.eraseInfo ((int)VfMaxPosY);
  V.eraseInfo ((int)VfMaxPosZ);

  //set the new info
  V.addInfo((int)VfMax,MaxValue);
  V.addInfo((int)VfMaxPosX,PositionMax.X());
  V.addInfo((int)VfMaxPosY,PositionMax.Y());
  V.addInfo((int)VfMaxPosZ,PositionMax.Z());
}
void VertexFunctionTool::setVfAt(LHCb::RecVertex & V,double & VfValue)
{
  V.eraseInfo ((int)VfAt);
  V.addInfo((int)VfAt,VfValue);

}

double VertexFunctionTool::ParabolicExtrapolator(Gaudi::XYZVector &gradient, Gaudi::XYZPoint &initialPoint, double &deltaR)
{

  //====================================================================
  // This function is used by the computeValueMax in order to estimate
  // the position of the maximum assuming the Vf along the one direction
  // is parabola-like.
  //====================================================================

  // In one dimension along the gradient, evaluate a local parabola and find
  // the distance from initialPoint to its locallocal maximum:

  Gaudi::XYZPoint pointPlus  = initialPoint + (deltaR * gradient);
  Gaudi::XYZPoint pointMinus = initialPoint - (deltaR * gradient);

  double potPlus  = computeValueAt(pointPlus);
  double potMinus = computeValueAt(pointMinus);

  // Calculate the coefficients of the parabola:

  double deltaRSq = deltaR * deltaR;

  double a =
    (potPlus + potMinus - 2 * computeValueAt(initialPoint)) / (2 * deltaRSq);  //Mathieu: ok

  double b = (potPlus - potMinus) / (2 * deltaR);

  //debug()<<"\tPara extra: a="<<a<<" b="<<b<<endmsg;
  // The numerator above should have -4 * deltaR * a * initialPoint, but
  // we take initialPoint to be 0

  if (fabs(a)<=  m_max_finder_min_gradient_mag){
    // If near a minimum in the potential, then no point trying to find a
    // nearby maximum, and vice versa. Signal that this is the best point:
    return 2.*deltaR;
  }

  // The estimated distance to the maximum:
  return -b / (2 * a); //Mathieu: ok

}



Gaudi::XYZVector VertexFunctionTool::impactParam(const LHCb::Track &Tr ,const LHCb::VertexBase & V){

  //====================================================================
  // Simple impact parameter calculator
  //====================================================================
  double Tpx=Tr.momentum().X();
  double Tpy=Tr.momentum().Y();
  double Tpz=Tr.momentum().Z();
  double Ax=Tr.position().X();
  double Ay=Tr.position().Y();
  double Az=Tr.position().Z();
  double Vx=V.position().X();
  double Vy=V.position().Y();
  double Vz=V.position().Z();
  double k = -((Ax-Vx)*Tpx+(Ay-Vy)*Tpy+(Az-Vz)*Tpz)/(Tr.p()*Tr.p());

  Gaudi::XYZVector impact;
  impact.SetXYZ(Vx-Ax+k*Tpx, Vy-Ay+k*Tpy, Vz-Az+k*Tpz);
  return impact;

}


Gaudi::XYZVector VertexFunctionTool::computeGradientAt(Gaudi::XYZPoint & P) {
  //====================================================================
  // This function computes analytically the gradient of the Vf
  //====================================================================
  const LHCb::VertexBase* vv = new LHCb::VertexBase(P);
  double d_gradient[3];

  for(int xyz = 0; xyz<3; xyz++){

    double Sum_d_GaussT = 0;
    double Sum_GaussT = 0;
    double Sum_GaussT_Sq = 0;
    double Sum_GaussT_d_GaussT = 0;

    for(std::vector< const LHCb::Track* >::iterator it= m_vTr.begin(); it!=m_vTr.end(); ++it){
      Gaudi::XYZVector  imp;
      StatusCode Sc_m_Geom = m_Geom->distance((*it),vv,imp);

      Gaudi::XYZPoint pos;
      Gaudi::SymMatrix3x3 errPos3D;
      StatusCode Sc_m_Trans = m_Trans->position(*(*it), P.Z()+imp.Z(), pos, errPos3D);
      if(Sc_m_Trans == StatusCode::FAILURE || errPos3D[0][0]!=errPos3D[0][0]){
        warning()<<"m_Trans->position return failed status code or NaN!  errPos3D[0][0]="<<errPos3D[0][0]<<endmsg;
        continue;
      }


      //imp deriavtive (analytically)
      Gaudi::XYZVector p0 = (*it)->momentum();
      Gaudi::XYZVector p = p0/p0.R();
      double d_imp[2] = {0,0};
      switch ( xyz ){
      case 0:
        d_imp[0] = p.X()*p.X()-1;
        d_imp[1] = p.X()*p.Y();
        break;
      case 1:
        d_imp[0] = p.X()*p.Y();
        d_imp[1] = p.Y()*p.Y()-1;
        break;
      case 2:
        d_imp[0] = p.X()*p.Z();
        d_imp[1] = p.Y()*p.Z();
        break;
      }
      //debug()<< "Imp Analytical: "<<xyz<<" = "<< d_imp[0]<< " ,"<< d_imp[1]<<endmsg;
      Gaudi::SymMatrix2x2  errPos = errPos3D.Sub<Gaudi::SymMatrix2x2>   (0,0);
      ROOT::Math::SVector<double,2> Simp;
      Simp[0]=imp.X();
      Simp[1]=imp.Y();
      bool isInv = errPos.Invert();
      if (!isInv || errPos[0][0]!=errPos[0][0]) {
        warning()<<"Covariance matrice could not be inverted !!  errPos[0][0]="<<errPos[0][0] << endmsg;
        continue;
      }
      double CpC = ROOT::Math::Similarity(errPos,Simp);
      double dCpC = 2*imp.X()*d_imp[0] *errPos[0][0]+
        2*imp.Y()*d_imp[1] *errPos[1][1]+
        2*(d_imp[0] *imp.Y()+d_imp[1]*imp.X())*errPos[0][1];

      double dGaussT = -0.5*dCpC*exp(-0.5*CpC);
      double GaussT = exp(-0.5*CpC);
      Sum_GaussT+=GaussT;
      Sum_GaussT_Sq+=GaussT*GaussT;
      Sum_d_GaussT +=dGaussT;
      Sum_GaussT_d_GaussT+=dGaussT*GaussT;
    }
    d_gradient[xyz]=Sum_d_GaussT-(2.0*Sum_GaussT_d_GaussT*Sum_GaussT-Sum_GaussT_Sq*Sum_d_GaussT)/(Sum_GaussT*Sum_GaussT);
  }
  Gaudi::XYZVector gradient(d_gradient[0],d_gradient[1],d_gradient[2]);
  return gradient;
}


/*
  void VertexFunctionTool::computeValueMax(LHCb::RecVertex & V, Gaudi::XYZPoint & PMax, double & Max){

  //====================================================================
  // This function finds the nearest local maximum around the vertex
  // fitted postion. It uses ROOT standard minimizer.
  //====================================================================

  char* minName = "Minuit2";
  char* algoName = "";
  int randomSeed = -1;
  ROOT::Math::Minimizer* min =
  ROOT::Math::Factory::CreateMinimizer(minName, algoName);

  min->SetMaxFunctionCalls(m_max_finder_max_iteration); // for Minuit/Minuit2
  min->SetMaxIterations(10000);  // for GSL
  min->SetTolerance(m_max_finder_min_step);
  min->SetPrintLevel(0);
  // VfForMax Vf(this);

  // ROOT::Math::Functor f(&Vf.computeValueAt2,3);
  //ROOT::Math::Functor f(&VertexFunctionTool::computeValueAt2,3);
  Vf_Max f(this);

  double step[3] = {m_step,m_step,m_step};

  //starting point
  double variable[3] = {V.position().X(),V.position().Y(),V.position().Z()};
  std::cout<<"Probe Value Max Functor :"<<f.DoEval(variable)<<std::endl;
  min->SetFunction(f);
  min->SetVariable(0,"x",variable[0], step[0]);
  min->SetVariable(1,"y",variable[1], step[1]);
  min->SetVariable(2,"z",variable[2], step[2]);

  min->Minimize();


  const double *xs = min->X();
  PMax.SetXYZ(xs[0],xs[1],xs[2]);
  Max = -min->MinValue();


  }
*/
