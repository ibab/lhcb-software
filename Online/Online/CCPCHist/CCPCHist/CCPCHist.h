#ifndef CCPCHIST_TAG
#define CCPCHIST_TAG
//#include "hist_types.h"
#define H_IllegalID 1

#ifdef __cplusplus
class CCPCHisto;
class CCPCPHisto;
class CCPCRHisto;
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
/*! Constructor for 1-dimensional histogram

  \param name The name of the histogram.
  \param title The title of the histogram
  \param nx The number of bins
  \param xmin The minimum x-value
  \param xmax The maximum x-value
*/
  Histo(char *name, char *title, int nx, float xmin, float xmax );

/*! Constructor for 2-dimensional histogram

  \param name The name of the histogram.
  \param title The title of the histogram
  \param nx The number of bins
  \param xmin The minimum x-value
  \param xmax The maximum x-value
  \param ny The number of bins
  \param ymin The minimum y-value
  \param ymax The maximum y-value
*/
  Histo(char *name, char *title, int nx, float xmin, float xmax, int ny, float ymin, float ymax );
///
  Histo();
  virtual ~Histo();
  //int setname ( char* name);
///
/// Clears Contents of histogram
///
  void clear(void);

/*! Loads contents of float array as bin contents into histogram

  \param from Array of floats to be loaded into the histogram contents
*/
  int put (float *from) ;
/*! Loads contents of float array as bin contents into histogram

  \param from Array of doubles to be loaded into the histogram contents
*/
  int put (double *from) ;
/*! Loads the Number-of-Entries

  \param ne Integer number of entried to be loaded
*/
  int putnents (int ne) ;
/*! retrieve the contents of the histogram bins

  \param to Array of floats to receive the histogram contents
*/
  int get (float *to) ;


/*! retrieve the contents of the histogram

  \param to Array of double to receive the histogram contents
*/
  int get (double *to) ;

/*! retrieve the bin errors of the histogram
  \param to Array of floats to receive the bin errors of the histogram 
*/
  int geterr (float *to) ;

/*! retrieve the bin errors of the histogram
  \param to Array of doubles to receive the bin errors of the histogram 
*/
  int geterr (double *to) ;
///
/// returns the number of entries of a histogram 
///
  int getnents() ;

/*! Increment the contents of the bin containg the argument by 1

  \param x abszissa value of the bin to be incremented 
*/
  int fill (float x);
/*! Increments the contents of the bin containg the first argument by the weight argument
(1-dimensional histograms) \n
OR \n
Increments the bin contents of the bin containing the pair (x,weight) of a 2-dimentional histogram by 1

  \param x abszissa value of the bin to be incremented 
  \param weight increment of the bin contents (1-dimentional histograms)\n
  OR \n
 ordinate value of the bin to be incremented (2-dimentional histograms)
*/
  int fill (float x, float weight);
/*! Increments the contents of the bin containg the first two argument by the weight argument
This signature is only valid for 2-dimensional histograms

\param x abszissa value of the bin to be incremented 
\param y ordinate value of the bin to be incremented
\param weight increment of the bin contents
*/
  int fill (float x, float y, float weight);
};
class PHisto // public Histo
{
private:
  CCPCPHisto *m_h;
public:
/*! Constructor of a profile histogram
  \param name The name of the histogram.
  \param title The title of the histogram
  \param nx The number of bins
  \param xmin The minimum x-value
  \param xmax The maximum x-value

  \sa Histo(char *name, char *title, int nx, float xmin, float xmax )
*/
  PHisto(char *name, char *title, int nx, float xmin, float xmax );
  virtual ~PHisto();
/*!
 filling routine for a profile histogram
 \param x abszissa of the bin
 \param y weight to be added to bin statistics
*/
  int fill(float x, float y);
/*!
 retriving routine of sum of weights
 \param to float array to receive the sum of weights
*/
  int getsums (float *to) ;
/*!
 retriving routine of sum of square of weights
 \param to float array to receive the sum of square of weights
*/
  int getsum2s(float *to );
/*!
 retriving routine of sum of weights
 \param to double precision array to receive the sum of weights
*/
  int getsums (double *to) ;
/*!
 retriving routine of sum of square of weights
 \param to double precision array to receive the sum of square of weights
*/
  int getsum2s(double *to );
/*!
 retriving routine of sum of entries (calls)
 \param to float array to receive the total number of entries
*/
  int getentries(float *to);
/*!
 retriving routine of sum of entries (calls)
 \param to double array to receive the total number of entries
*/
  int getentries(double *to);
///
/// clear the profile histogram
///
  void clear(void);
};
#endif
#ifdef __cplusplus
extern "C"{
#endif
/*! Initializes the histogramming package.
\param name Name of the Histogramming system used in the creation of the DIM services.
\return pointer to the instance of the HSys object
*/
  void *hccpc_init(char* name);
/*! Booking (creating) a 1-dimensional histogram.
  \param name The name of the histogram.
  \param title The title of the histogram
  \param nx The number of bins
  \param xmin The minimum x-value
  \param xmax The maximum x-value
  \return pointer to the created instance of the histogram. The returned 
  pointer has to be used in subsequent calls, such a the filling routines.
*/
  void *hccpc_book1(char *name, char *title, int nx, float xmin, float xmax );
/*! Booking (creating) a profile histogram.
  \param name The name of the histogram.
  \param title The title of the histogram
  \param nx The number of bins in the x-direction
  \param xmin The minimum x-value
  \param xmax The maximum x-value
  \return pointer to the created instance of the histogram. The returned 
  pointer has to be used in subsequent calls, such a the filling routines.
*/
  void *hccpc_profile(char *name, char *title, int nx, float xmin, float xmax );
/*! Booking (creating) a 2-dimensional histogram.
  \param name The name of the histogram.
  \param title The title of the histogram
  \param nx The number of bins in the x-direction
  \param xmin The minimum x-value
  \param xmax The maximum x-value
  \param ny The number of bins in the y-direction
  \param ymin The minimum y-value
  \param ymax The maximum y-value
  \return pointer to the created instance of the histogram. The returned 
  pointer has to be used in subsequent calls, such a the filling routines.
*/
  void *hccpc_book2(char *name, char *title, int nx, float xmin, float xmax, 
             int ny, float ymin, float ymax );
/*! Fills a 1-dimensional histogram.
\param id Pointer to the histogram as returned from the booking routine
 \param x abszissa of the bin
 \param weight weight to be added to bin contents
*/
  int hfill1 (void *id, float x, float weight);
/*! Fills a 2-dimensional histogram.
\param id Pointer to the histogram as returned from the booking routine
 \param x abszissa of the bin
 \param y ordinate of the bin
 \param weight weight to be added to bin contents
*/
  int hfill2 (void *id, float x, float y, float weight);
/*! Fills a profile histogram.
 \param id Pointer to the histogram as returned from the booking routine
 \param x abszissa of the bin
 \param y y-value to be added to bin statistics
*/
  int hfillp (void *id, float x, float y);
/*! Load the bin contents of a histogram from a float array
 \param id Pointer to the histogram as returned from the booking routine
 \param buf array of floats to containig the values to be loaded into the bin 
 contents of the histogram. The array should be dimensioned at least to 
 nx+2 for 1-dimensional histograms and (nx+2)*(ny+2) for 2-dimensional histograms
*/
  int hputf (void *id, float* buf);
/*! Load the bin contents of a histogram from a double array
 \param id Pointer to the histogram as returned from the booking routine
 \param buf array of doubles to containig the values to be loaded into the bin 
 contents of the histogram. The array should be dimensioned at least to 
 nx+2 for 1-dimensional histograms and (nx+2)*(ny+2) for 2-dimensional histograms
*/
  int hputd (void *id, double* buf);
#ifdef __cplusplus
}
#endif

#endif

