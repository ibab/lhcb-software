#ifndef CCPCHIST_TAG
#define CCPCHIST_TAG
#include "hist_types.h"
#define H_IllegalID 1

#ifdef __cplusplus
class CCPCHisto;
class CCPCPHisto;
class CCPCHSys;
class Histo;


class HSys
{
protected:
  CCPCHSys *m_sys;
public:
	HSys();
	~HSys();
  Histo *findhisto(char *name);
};

class Histo
{
private:
  CCPCHisto *m_h;
public:
///
///Constructor for 1-dim histogram
///
  Histo(char *name, char *title, int nx, float xmin, float xmax );
///
///Constructor for 2-dim histogram
///
	Histo(char *name, char *title, int nx, float xmin, float xmax, 
    int ny, float ymin, float ymax );
///
///Constructor generic histogram
///
  Histo();
	virtual ~Histo();
	//int setname ( char* name);
///
/// Clears Contents of histogram
///
  void clear(void);
///
/// Loads contents of float array as bin contents into histogram
///
  int put (float *from) ;
///
/// Loads contents of double array as bin contents into histogram
///
	int put (double *from) ;
///
/// Loads the number of entries of a histogram
///
	int putnents (int ne) ;
///
/// retrieves bin contents of a histogram into a float array 
///
	int get (float *to) ;
///
/// retrieves bin error of a histogram into a float array 
///
	int geterr (float *to) ;
///
/// retrieves bin contents of a histogram into a double array 
///
	int get (double *to) ;
///
/// retrieves bin error of a histogram into a doubles array 
///
	int geterr (double *to) ;
///
/// retrieves the number of entries of a hostogram 
///
	int getnents() ;

///
/// increments the bin containing x by 1 
///
	int fill (float x);
/** increments the bin containing x by weight 
OR
increments the bin of a 2-dim histogram containing x and y by 1
*/
	int fill (float x, bintype weight);
///
/// increments the bin of a 2-dim histogram containing x and y by weigh
///
	int fill (float x, float y, bintype weight);
};
class PHisto // public Histo
{
private:
  CCPCPHisto *m_h;
public:
///
/// Cobstructor of a Profile Histogram
///
  PHisto(char *name, char *title, int nx, float xmin, float xmax );
	virtual ~PHisto();
///
/// increments the bin containing x by weight y
///
	int fill(float x, float y);
///
/// retrieves the sum of all weights to a float array
///
	int getsums (float *to) ;
///
/// retrieves the sum of the squares of all weights to a float array
///
  int getsum2s(float *to );
///
/// retrieves the sum of all weights to a double array
///
	int getsums (double *to) ;
///
/// retrieves the sum of the squares of all weights to a double array
///
  int getsum2s(double *to );
///
/// retrieves the number of entries to a float array
///
  int getentries(float *to);
///
/// clears the profile histogram
///
  void clear(void);
};
#endif
#ifdef __cplusplus
extern "C"{
#endif
  void *hccpc_init(char*);
  void *hccpc_book1(char *name, char *title, int nx, float xmin, float xmax );
  void *hccpc_profile(char *name, char *title, int nx, float xmin, float xmax );
  void *hccpc_book2(char *name, char *title, int nx, float xmin, float xmax, 
					   int ny, float ymin, float ymax );
	int hfill1 (void *id, float x, bintype weight);
	int hfill2 (void *id, float x, float y, bintype weight);
  int hfillp (void *id, float x, float y);
	int hputf (void *id, float* buf);
	int hputd (void *id, double* buf);

#ifdef __cplusplus
}
#endif

#endif

