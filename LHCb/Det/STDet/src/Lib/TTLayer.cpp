// $
//
// This File contains the definition of the ITSTLayer-class
//
// C++ code for 'LHCb Tracking package(s)'
//
//   Author: Matthew Needham
//   Created: 10-05-1999

//_________________________________________________
// ITSTLayer
// ST layer properties 

#include "STDet/TTLayer.h"
#include "Kernel/ITChannelID.h"
#include "STDet/STWafer.h"

#include <iterator>


TTLayer::TTLayer(int stationID, int layerID, double z, 
                    double stereoAngle, double pitch, double waferWidth, 
		    double waferHeight, double waferThickness,
		    double waferOverlap1, double waferOverlap2,
		    unsigned int nOverlap1, unsigned int nFine,
                    unsigned int wafersX1,
		    unsigned int wafersX2,
                    std::vector<int> ladderSize1,
                    std::vector<int> ladderSize2,
                    double holeX, double holeY, 
		    double ladderDist,
		    unsigned int numStrips, double vertGuardRing):
  STDetectionLayer(stationID, layerID, z, 
                   stereoAngle, pitch,waferThickness, 
                   ladderDist),
  m_firstInnerColumn(0),
  m_lastInnerColumn(0),
  m_outerRowOffset(0)
{
  // constructer

  m_type = "TT";

  // ladder size 3
  std::vector<int> ladderSize3;
  ladderSize3.insert(ladderSize3.begin(),
                     ladderSize1.begin(),
                     ladderSize1.end());
  
  ladderSize3.insert(ladderSize3.end(),
                     ladderSize1.rbegin(),
                     ladderSize1.rend());


  // outer half size
  m_outerHalfSize = ladderSize2.size()/2;

  // sensitive area of wafer (excludes guard ring...)
  double sensWaferWidth = pitch*(double)numStrips;

  unsigned int nSensorHigh=0;
  // max dimensions
  for (unsigned int iLad1=0; iLad1<ladderSize1.size();iLad1++){
    nSensorHigh += ladderSize1[iLad1]; 
  } // iLad

  // cache vector of wafer numbers in columns
  unsigned int iC;
  unsigned int colOff = 0;
  unsigned int numFineCol = (2*nFine) + wafersX1;  
  for (iC = 0; iC < (wafersX2+1-nFine); ++iC ){
    m_firstInColumn.push_back(colOff+(iC*ladderSize2.size()));
  } // iC
  
  colOff =  m_firstInColumn.back();
  for (iC = 1u; iC <= numFineCol; ++iC){
    m_firstInColumn.push_back(colOff+(ladderSize3.size()*iC));
  }

  colOff =  m_firstInColumn.back();
  for (iC = 1u; iC <= (wafersX2-nFine-1); ++iC ){
    m_firstInColumn.push_back(colOff+(ladderSize2.size()*iC));
  } // iC


  // for this funny row thing
  if (nFine > 0){
    m_firstInnerColumn =  wafersX2-nFine+1; 
    m_lastInnerColumn =  wafersX2+nFine+wafersX1;
    m_outerRowOffset = ladderSize3.size() - ladderSize2.size();
  }

  m_WafersNum = m_firstInColumn.back()+ladderSize2.size();
  m_Wafers.resize(m_WafersNum);

  // layout top and bottom boxes...
  double yLad = holeY + 0.5*((nSensorHigh*waferHeight*cosAngle())
                             + (waferWidth*fabs(sinAngle())));
  double xLad = -yLad*sinAngle()/cosAngle();
  // go to u,v
  double uLad = xLad*cosAngle() + yLad*sinAngle();
  double vLad = yLad*cosAngle() - xLad*sinAngle();
  double vMax = vLad + (0.5*(double)nSensorHigh*waferHeight);

  // bottom box
  int currStrip = 0;
  double v = 0.;

  unsigned int iWafer = 0;
  for (unsigned int iWafer=1;iWafer<=wafersX1;iWafer++){
    
    double dz = -(1.-(2.*(iWafer%2)))*0.5*ladderDist;
    unsigned int waferOffset = m_firstInColumn[wafersX2+iWafer-1];
    v = -vMax;  

    for (unsigned int iBWafer=1; iBWafer<=ladderSize1.size();iBWafer++){

      v += (waferHeight*(double)ladderSize1[iBWafer-1])/2.0; 
      double ladderHeight = (ladderSize1[iBWafer-1]*waferHeight)
                            -2.0*vertGuardRing;

    
      STWafer* aWafer = new STWafer(pitch, 1,ladderSize1[iBWafer-1],
				    stationID,layerID,waferOffset+iBWafer,
                                    uLad-0.5*sensWaferWidth, uLad+0.5*sensWaferWidth, 
	   		            v - 0.5*ladderHeight, 
		 	            v + 0.5*ladderHeight, 
                                    dz,
                                    vertGuardRing);
       
       m_Wafers[aWafer->waferID()-1] = aWafer;
       currStrip += aWafer->lastStrip();
       v+= (waferHeight*(double)ladderSize1[iBWafer-1])/2.0;

    }  // iBWafer
  } // iWafer

  v = vMax - ((double)nSensorHigh*waferHeight);

  for (iWafer=1;iWafer<=wafersX1;iWafer++){

    unsigned int waferOffset = m_firstInColumn[wafersX2+iWafer-1]+(ladderSize1.size());
    double dz = -(1.-(2.*(iWafer%2)))*0.5*ladderDist;

    for (unsigned int iTWafer=1; iTWafer<=ladderSize1.size();iTWafer++){

        v += (waferHeight*(double)ladderSize1[ladderSize1.size()-iTWafer])/2.0;
        double ladderHeight = (ladderSize1[iTWafer-1]*waferHeight)
                            -2.0*vertGuardRing;


        STWafer* aWafer = new STWafer(pitch, 1,ladderSize1[iTWafer-1],
				    stationID,layerID,waferOffset+iTWafer,
                                    uLad-0.5*sensWaferWidth, uLad+0.5*sensWaferWidth, 
	   		            v - 0.5*ladderHeight, 
		 	            v + 0.5*ladderHeight, 
                                    dz,
                                    vertGuardRing);

        m_Wafers[aWafer->waferID()-1] = aWafer;
        currStrip += aWafer->lastStrip();

        v += (waferHeight*(double)ladderSize1[ladderSize1.size()-iTWafer])/2.0;

    } // iTWafer
  } // iWafer


  // side boxes

  // recalculate size in sensors
  nSensorHigh = 0;
  unsigned int iLad2=0;
  for (iLad2=0; iLad2<ladderSize2.size();iLad2++){
    nSensorHigh += ladderSize2[iLad2]; 
  } // iLad

  // cache the x centers of ladders
  double xStart = holeX+(0.5*waferWidth*cosAngle());
  double step1 = (waferWidth-waferOverlap1)/cosAngle();
  double step2 = (waferWidth-waferOverlap2)/cosAngle();

  std::vector<double> xSideLad; 
  for (iLad2=0;iLad2<=nOverlap1;iLad2++){
     xSideLad.push_back(xStart+(iLad2*step1));
  } // iLad2

  xStart = xSideLad.back();
  unsigned int nOverlap2 =  wafersX2-nOverlap1; 
  for (iLad2=1;iLad2<=nOverlap2;iLad2++){
     xSideLad.push_back(xStart+(iLad2*step2));
  } // iLad2

  double ySideLad = 0.;


  std::vector<int>::iterator startIter;
  std::vector<int>::iterator stopIter;
  
  for (unsigned int iLWafer=1;iLWafer<=wafersX2;iLWafer++){

    double dz = (1.-(2.*(iLWafer%2)))*0.5*ladderDist;
    double xLad = -xSideLad[iLWafer-1]; 
    double uLad = xLad*cosAngle() + ySideLad*sinAngle();
    double vLad = ySideLad*cosAngle() - xLad*sinAngle();
    double v = vLad-((double)(nSensorHigh)/2.0)*waferHeight;
    unsigned int waferOffset = m_firstInColumn[wafersX2-iLWafer];

    unsigned int iWafer = 1;
    if (iLWafer<= nFine){
      startIter = ladderSize3.begin();
      stopIter = ladderSize3.end();
    }  
    else {
      startIter = ladderSize2.begin();
      stopIter = ladderSize2.end();
    }

    for ( ; startIter != stopIter ; ++startIter){

      // wafer center in V
      v += 0.5*(waferHeight*(double)(*startIter)); 
  
      // ladder height
      double ladderHeight = waferHeight*(double)(*startIter)
	-2.*vertGuardRing; 
               
      STWafer* aWafer = new STWafer(pitch, 1, *startIter,  
				    stationID,layerID,iWafer+waferOffset,
                        uLad-0.5*sensWaferWidth,uLad+0.5*sensWaferWidth, 
			v - 0.5*ladderHeight, 
		        v + 0.5*ladderHeight, 
                        dz,
                        vertGuardRing);
      m_Wafers[aWafer->waferID()-1] = aWafer;
      currStrip += aWafer->lastStrip();
 
      v += 0.5*(waferHeight*(double)(*startIter)); 

      ++iWafer;

    } //iWafer
  } // iLWafer

  for (unsigned int iRWafer=1;iRWafer<=wafersX2;iRWafer++){

    double dz = (1.-(2.*(iRWafer%2)))*0.5*ladderDist;
    double xLad = xSideLad[iRWafer-1];
    double uLad = xLad*cosAngle() + ySideLad*sinAngle();
    double vLad = ySideLad*cosAngle() - xLad*sinAngle();
    double v = vLad-((double)(nSensorHigh)/2.0)*waferHeight;
    unsigned int waferOffset = m_firstInColumn[wafersX2+wafersX1+iRWafer-1];
    
    unsigned int iWafer = 1;
    if (iRWafer<= nFine){
      startIter = ladderSize3.begin();
      stopIter = ladderSize3.end();
    } 
    else {
      startIter = ladderSize2.begin();
      stopIter = ladderSize2.end();
    }

    for ( ; startIter != stopIter ; ++startIter){

      // wafer center in V
      v += 0.5*(waferHeight*(double)(*startIter)); 
      
      // ladder height
      double ladderHeight = waferHeight*(double)(*startIter)
	-2.*vertGuardRing; 
     
      STWafer* aWafer = new STWafer(pitch, 1, *startIter,  
				    stationID,layerID,iWafer+waferOffset,
                        uLad-0.5*sensWaferWidth,uLad+0.5*sensWaferWidth, 
			v - 0.5*ladderHeight, 
		        v + 0.5*ladderHeight, 
                        dz,
                        vertGuardRing);
      m_Wafers[aWafer->waferID()-1] = aWafer;
      currStrip += aWafer->lastStrip();
      v += 0.5*(waferHeight*(double)(*startIter)); 
      ++iWafer;

    } //iWafer
  } // iLWafer

  m_totStrips = currStrip;

}

TTLayer::~TTLayer(){
  //des
  if (m_Wafers.size() != 0) {
    std::vector<STWafer*>::iterator iWafer = m_Wafers.begin();
    while (iWafer != m_Wafers.end()) {
      STWafer* aWafer = *iWafer;
      delete aWafer;
      m_Wafers.erase(iWafer);
    }
  }
}

double TTLayer::centerX(const ITChannelID stripID) const {
  double xC = 0.;
  const unsigned int waferNum = stripID.wafer();
  if (waferNum>=1 && waferNum <= m_Wafers.size()) {
    const double u = m_Wafers[waferNum-1]->U(stripID.strip());
    const double v = m_Wafers[waferNum-1]->centerV();
    xC = u*cosAngle() - v*sinAngle();
  }

  return xC;
}

double TTLayer::centerY(const ITChannelID stripID) const {

  double yC = 0.;
  const unsigned int waferNum = stripID.wafer();
  if (waferNum>=1 && waferNum <= m_Wafers.size()) {
    const double u = m_Wafers[waferNum-1]->U(stripID.strip());
    const double v = m_Wafers[waferNum-1]->centerV();
    yC = v*cosAngle() + u*sinAngle();
  }

  return yC;
}

double TTLayer::centerZ(const ITChannelID stripID) const {

  // z center of channel
  double zC =0.;
  const unsigned int waferNum = stripID.wafer();
  if (waferNum>=1 && waferNum <= m_WafersNum) {
    zC = this->z() + m_Wafers[waferNum-1]->dZ();
  }

  return zC;
}

double TTLayer::halfLengthX(const ITChannelID stripID) const {
  return -this->halfLengthY(stripID)*sinAngle()/cosAngle(); 
}

double TTLayer::halfLengthY(const ITChannelID stripID) const {

  double halfLengthY = 0;
  const unsigned int waferNum = stripID.wafer();
  if (waferNum>=1 && waferNum <= m_WafersNum) {
    halfLengthY = m_Wafers[waferNum-1]->halfLengthV()*cosAngle();
  }

  return halfLengthY;
}

unsigned int TTLayer::numStrips() const {
  return m_totStrips;
}

unsigned int TTLayer::columnID(const unsigned int iWafer) const {

  // return row number
  unsigned int iC = 0u;

  if (iWafer>=1 && iWafer <= m_WafersNum){

    std::vector<unsigned int>::const_iterator iter = m_firstInColumn.begin();

    while ((iter != m_firstInColumn.end())&&(iWafer>*iter)){
      ++iter;
    } 
  
    std::vector<unsigned int>::difference_type n = 0;
    n = std::distance(m_firstInColumn.begin(),iter);
    iC = (unsigned int)n;
  }  // valid wafer

  return iC;
}

unsigned int TTLayer::numStripsInColumn(const unsigned int iColumn) const {

  unsigned int nStrip = 0u;
 
  if ((iColumn<=nColumns())&&(iColumn>0u)){

    unsigned int lastWafer = this->lastWaferInColumn(iColumn);

    unsigned int iWafer = this->firstWaferInColumn(iColumn);

    while (iWafer <= lastWafer){
      nStrip += wafer(iWafer)->numStrips(); 
      ++iWafer;
    }
  } // valid column

  return nStrip;
}

unsigned int TTLayer::firstWaferInColumn(const unsigned int iColumn) const {

  return ((iColumn <= m_firstInColumn.size())&&(iColumn>0) ? m_firstInColumn[iColumn-1]+1 : 0);
}


unsigned int TTLayer::lastWaferInColumn(const unsigned int iColumn) const {

  unsigned int lastWafer = 0;
  if ((iColumn < m_firstInColumn.size())&&(iColumn>0)){
    lastWafer =  m_firstInColumn[iColumn];
  }
  else if(iColumn ==  m_firstInColumn.size()){
    lastWafer = m_WafersNum;
  }
  else {
    // nothing
  }

  return lastWafer;
} 

unsigned int TTLayer::rowID(const unsigned int iWafer) const{

  // Tennyson function to return row
  unsigned int iRow = 0;

  // look up the column
  unsigned iC = columnID(iWafer);
  unsigned int iR = iWafer - m_firstInColumn[iC-1];

  if ((iC>=m_firstInnerColumn)&&(iC<=m_lastInnerColumn)){
    iRow = iR; 
  }
  else {
    if (iR <= m_outerHalfSize){
      iRow = iR;
    }
    else {
      iRow = iR + m_outerRowOffset;
    }
  }
  return iRow;
}
