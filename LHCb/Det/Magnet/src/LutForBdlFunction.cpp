//$ Id: $
#include <iostream>
#include "LutForBdlFunction.h"

LutForBdlFunction::LutForBdlFunction(int nVar, int* nBinVar, double* minVar, double* maxVar) {

    m_nVar = 0;
    m_nPointVar = NULL;
    m_minVar    = NULL;
    m_maxVar    = NULL;
    m_deltaVar  = NULL;
    m_table     = NULL;

    m_indexVector    = NULL;
    m_variableVector = NULL;

  int i;
  int good_input = 1;

  // check arguments
  if(nVar<1 || nVar>100) {
    good_input=0;
  }
  else {
    for (i=0; i<nVar; i++) {
      if( nBinVar[i] < 1 || (maxVar[i] - minVar[i]) < 1.e-10 ) {
        good_input=0;
      }
    }
  }

  if(good_input) {
    m_nVar = nVar;
    createVectors();
    for (i=0; i< m_nVar; i++) {
      m_nPointVar[i]  = nBinVar[i]+1;
      m_minVar[i]     = minVar[i];
      m_maxVar[i]     = maxVar[i];
      m_deltaVar[i]   = ( maxVar[i] - minVar[i] ) /  (m_nPointVar[i]-1);
    }
    createTable();
    // reset internal vectors
    for (i=0; i<m_nVar; i++) {
      m_indexVector[i] = 0;
      m_variableVector[i]   = 0.0;
    }
  }
  else {
    std::cout << " PatVTTLutForFunction: incorrect input for lookup table \n";
  }
}

LutForBdlFunction::~LutForBdlFunction() {
  deleteVectors();
  deleteTable();
}

void LutForBdlFunction::createVectors() {
  m_nPointVar    = new    int[m_nVar];
  m_minVar       = new double[m_nVar];
  m_maxVar       = new double[m_nVar];
  m_deltaVar     = new double[m_nVar];

  m_indexVector       = new    int[m_nVar];
  m_variableVector    = new double[m_nVar];
}

void LutForBdlFunction::deleteVectors() {
  if(m_nPointVar   != NULL) delete[] m_nPointVar;
  if(m_minVar      != NULL) delete[] m_minVar;
  if(m_maxVar      != NULL) delete[] m_maxVar;
  if(m_deltaVar    != NULL) delete[] m_deltaVar;

  if(m_indexVector      != NULL) delete[] m_indexVector;
  if(m_variableVector   != NULL) delete[] m_variableVector;
}

void LutForBdlFunction::createTable() {
  int i;
  int iSize = 1;
  for (i=0; i<m_nVar; i++) {
    iSize *= (m_nPointVar[i]);
  }

  m_table = new float[iSize];
}

void LutForBdlFunction::deleteTable() {
  if(m_table != NULL) delete[] m_table;
}

void LutForBdlFunction::resetIndexVector() {
  int i;
  for (i=0; i<m_nVar; i++) {
    m_indexVector[i] = 0;
  }
}

void LutForBdlFunction::resetVariableVector() {
  int i;
  for (i=0; i<m_nVar; i++) {
    m_variableVector[i] = 0;
  }
}

int LutForBdlFunction::incrementIndexVector() {
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


void LutForBdlFunction::fillTable(double lutValue) {
  m_table[tableLocation()] = (float) lutValue;
}

double LutForBdlFunction::getVariable(int iv) {
  calculateVariableVector();
  if(iv<0 || iv> m_nVar-1) return 0.0;
  return m_variableVector[iv];
}

void LutForBdlFunction::getVariableVector(double* var) {
  int i;
  calculateVariableVector();
  for (i=0; i<m_nVar; i++) {
    var[i] = m_variableVector[i];
  }
}

double LutForBdlFunction::getValueFromTable(double* var) {
  calculateClosestIndexVector(var);
  return (double) m_table[tableLocation()];
}

double LutForBdlFunction::getInterpolatedValueFromTable(double* var) {
  int i;
  calculateIndexVector(var);
  calculateVariableVector();
  double tabVal = m_table[tableLocation()];
  double addTabVal = 0.0;
  for(i=0; i<m_nVar; i++) {
    if(var[i]<m_variableVector[i]) continue;
    if(m_indexVector[i] > m_nPointVar[i]-2) continue;
    if(var[i]<m_minVar[i] || var[i]>m_maxVar[i]) continue;
    m_indexVector[i]++;
    double dTab_dVar =  (m_table[tableLocation()] - tabVal) / m_deltaVar[i];
    m_indexVector[i]--;
    double dVar = (var[i]-m_variableVector[i]);
    addTabVal += dTab_dVar*dVar;
  }
  tabVal+=addTabVal;
  return tabVal;
}

int LutForBdlFunction::tableLocation() {
  int i;
  if(1==m_nVar) return m_indexVector[0];
  int location;
  location = m_indexVector[m_nVar-1];
  for (i=m_nVar-2; i>=0; i--) {
    location = location*m_nPointVar[i] + m_indexVector[i];
  }
  return location;
}

void LutForBdlFunction::calculateVariableVector() {
  int i;
  for (i=0; i<m_nVar; i++) {
    m_variableVector[i]=m_minVar[i]+m_indexVector[i]*m_deltaVar[i];
  }
}

void LutForBdlFunction::calculateIndexVector(double* var) {
  int i;
  for (i=0; i<m_nVar; i++) {
    int idx = (int) ((var[i]-m_minVar[i])/m_deltaVar[i] + 0.000000001);
    if(idx<0) idx=0;
    if(idx>(m_nPointVar[i]-1)) idx=m_nPointVar[i]-1;
    m_indexVector[i]=idx;
  }
}

void LutForBdlFunction::calculateClosestIndexVector(double* var) {
  int i;
  for (i=0; i<m_nVar; i++) {
    int idx = (int) ((var[i]-m_minVar[i])/m_deltaVar[i] + 0.5);
    if(idx<0) idx=0;
    if(idx>(m_nPointVar[i]-1)) idx=m_nPointVar[i]-1;
    m_indexVector[i]=idx;
  }
}
