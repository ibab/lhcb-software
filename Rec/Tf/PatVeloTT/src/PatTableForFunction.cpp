// Include files

// local
#include "PatTableForFunction.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatTableForFunction
//
// 2006-09-25 : Mariusz Witek
//-----------------------------------------------------------------------------


// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( PatTableForFunction )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatTableForFunction::PatTableForFunction( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_nVar(0)
{
  declareInterface<PatTableForFunction>(this);
}
//=============================================================================
// Destructor
//=============================================================================
PatTableForFunction::~PatTableForFunction() {}

void PatTableForFunction::clear()
{
  m_nVar = 0 ;
  m_nPointVar.clear() ;
  m_minVar.clear();
  m_maxVar.clear();
  m_deltaVar.clear();
  m_table.clear();
  m_indexVector.clear();
  m_variableVector.clear();
}


void PatTableForFunction::addVariable(int nBin, float lowVal, float highVal) {
  m_nPointVar.push_back(nBin+1);
  m_minVar.push_back(lowVal);
  m_maxVar.push_back(highVal);

  m_deltaVar.push_back((highVal-lowVal)/nBin);

  m_nVar = m_nPointVar.size();
}

void PatTableForFunction::prepareTable() {

  int i;
  for (i=0; i<m_nVar; i++) {
    m_indexVector.push_back(0);
    m_variableVector.push_back(0.);
  }

  int iSize = 1;
  for (i=0; i<m_nVar; i++) {
    iSize *= (m_nPointVar[i]);
  }

  for (i=0; i<iSize; i++) {
    m_table.push_back(0.);
  }

}

void PatTableForFunction::resetIndexVector() {
  int i;
  for (i=0; i<m_nVar; i++) {
    m_indexVector[i] = 0;
  }
}

void PatTableForFunction::resetVariableVector() {
  int i;
  for (i=0; i<m_nVar; i++) {
    m_variableVector[i] = 0;
  }
}

int PatTableForFunction::incrementIndexVector() {
  int i;
  m_indexVector[0]++;
  for (i=0; i<m_nVar-1; i++) {
    if(m_indexVector[i] > m_nPointVar[i]-1) {
      m_indexVector[i]=0;
      m_indexVector[i+1]++;
    }
    else {
      break;
    }
  }
  if(m_indexVector[m_nVar-1] > m_nPointVar[m_nVar-1]-1) {
    resetIndexVector();
    return 1;
  }

  return 0;
}


void PatTableForFunction::fillTable(float lutValue) {
  m_table[tableLocation()] = lutValue;
}

float PatTableForFunction::getVariable(int iv) {
  calculateVariableVector();
  if(iv<0 || iv> m_nVar-1) return 0.0;
  return m_variableVector[iv];
}

void PatTableForFunction::getVariableVector(std::vector<float>& var) {
  int i;
  calculateVariableVector();
  for (i=0; i<m_nVar; i++) {
    var[i] = m_variableVector[i];
  }
}

float PatTableForFunction::getValueFromTable(std::vector<float>& var) {
  calculateClosestIndexVector(var);
  return (float) m_table[tableLocation()];
}

float PatTableForFunction::getInterpolatedValueFromTable(std::vector<float>& var) {
  int i;
  calculateIndexVector(var);
  calculateVariableVector();
  float tabVal = m_table[tableLocation()];
  float addTabVal = 0.0;
  for(i=0; i<m_nVar; i++) {
    if(var[i]<m_variableVector[i]) continue;
    if(m_indexVector[i] > m_nPointVar[i]-2) continue;
    if(var[i]<m_minVar[i] || var[i]>m_maxVar[i]) continue;
    m_indexVector[i]++;
    float dTab_dVar =  (m_table[tableLocation()] - tabVal) / m_deltaVar[i];
    m_indexVector[i]--;
    float dVar = (var[i]-m_variableVector[i]);
    addTabVal += dTab_dVar*dVar;
  }
  tabVal+=addTabVal;
  return tabVal;
}

int PatTableForFunction::tableLocation() {
  int i;
  if(1==m_nVar) return m_indexVector[0];
  int location;
  location = m_indexVector[m_nVar-1];
  for (i=m_nVar-2; i>=0; i--) {
    location = location*m_nPointVar[i] + m_indexVector[i];
  }
  return location;
}

void PatTableForFunction::calculateVariableVector() {
  int i;
  for (i=0; i<m_nVar; i++) {
    m_variableVector[i]=m_minVar[i]+m_indexVector[i]*m_deltaVar[i];
  }
}

void PatTableForFunction::calculateIndexVector(std::vector<float>& var) {
  int i;
  for (i=0; i<m_nVar; i++) {
    int idx = (int) ((var[i]-m_minVar[i])/m_deltaVar[i] + 0.000000001);
    if(idx<0) idx=0;
    if(idx>(m_nPointVar[i]-1)) idx=m_nPointVar[i]-1;
    m_indexVector[i]=idx;
  }
}

void PatTableForFunction::calculateClosestIndexVector(std::vector<float>& var) {
  int i;
  for (i=0; i<m_nVar; i++) {
    int idx = (int) ((var[i]-m_minVar[i])/m_deltaVar[i] + 0.5);
    if(idx<0) idx=0;
    if(idx>(m_nPointVar[i]-1)) idx=m_nPointVar[i]-1;
    m_indexVector[i]=idx;
  }
}




//=============================================================================
