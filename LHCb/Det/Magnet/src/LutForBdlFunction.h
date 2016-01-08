#ifndef LUTFORBDLFUNCTION_H
#define LUTFORBDLFUNCTION_H 1

/*
  LutForBdlFunction class
  Lookup table implementation to tabulate
  function of n double arguments

  Idea:
  fun(x1,x2,x3,...) - function to tabulate

  Initialize
    constructor          -  specify ranges and nr of bins for x1,x2,x3... (n-dim net)
    resetIndexVector     -  set start point (the lowest values x1,x2,x3,...)

  Loop to fill table
    incrementIndexVector -  increment over all (x1,x2,x3,...) n-dim points
    getVariableVector    -  to get (x1,x2,x3...) n-dim point
    fillTable            -  fill table with f(x1,x2,x3,...) values

  Retrieve stored values
    getValueFromTable              -  get value stored in node closest to (x1,x2,x3,...)
    getInterpolatedValueFromTable  -  get value using interpolation

    getInterpolatedValueFromTable is slower than getValueFromTable by a factor of 2 approximately
    but uses less memory

Example:

  int nvar =3;
  int  nbin[3] = {30, 30, 30};
  double lv[3] = { 0., 0., 0.};
  double rv[3] = { 5., 5., 5.};
  LutForBdlFunction tabfun(nvar, nbin, lv, rv);

  tabfun.resetIndexVector();
  double var[3];
  int iover = 0;
  while(!iover) {
    tabfun.getVariableVector(var);
    tabfun.fillTable( fun (var[0],var[1],var[2]) );
    iover= tabfun.incrementIndexVector();
  }

...

Retrieve information

  var[0]=1.5;
  var[1]=4.1;
  var[2]=2.9;
  tabfun.getValueFromTable(var);
or
  tabfun.getInterpolatedValueFromTable(var);


*/


class LutForBdlFunction {

public:

  LutForBdlFunction(int nVar, int* nBinVar, double* minVar, double* maxVar);

  ~LutForBdlFunction();

  void   resetIndexVector();
  int    incrementIndexVector();
  void   fillTable(double lutValue);
  double getVariable(int ivar);
  void   getVariableVector(double* var);
  double getValueFromTable(double* var);
  double getInterpolatedValueFromTable(double* var);


private:

  void   createVectors();
  void   deleteVectors();
  void   createTable();
  void   deleteTable();
  void   resetVariableVector();
  int    tableLocation();
  void   calculateVariableVector();
  void   calculateIndexVector(double* var);
  void   calculateClosestIndexVector(double* var);



  int     m_nVar;
  int*    m_nPointVar;
  double* m_minVar;
  double* m_maxVar;
  double* m_deltaVar;
  float*  m_table;

  int*    m_indexVector;
  double* m_variableVector;

};

#endif // LUTFORBDLFUNCTION_H

