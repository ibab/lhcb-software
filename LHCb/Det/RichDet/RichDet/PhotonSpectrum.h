// $Id: PhotonSpectrum.h,v 1.3 2001-10-31 16:50:12 rihill Exp $
#ifndef RICHDET_PHOTONSPECTRUM_H
#define RICHDET_PHOTONSPECTRUM_H 1

#include <vector>

class PhotonSpectrum
{

public:
  PhotonSpectrum(const PhotonSpectrum &right);

  PhotonSpectrum (const int nbin, 
                  const double lower, 
                  const double upper);

  PhotonSpectrum (const double lower, 
                  const double delta, 
                  const double* value);

  virtual ~PhotonSpectrum();

  PhotonSpectrum & operator=(const PhotonSpectrum &right);


  virtual PhotonSpectrum & operator *= (const double scalar);

  virtual PhotonSpectrum & operator *= (const PhotonSpectrum &spectrum);

  virtual double integral () const;

  virtual const double & operator [] (const double energy) const;

  virtual const double & operator [] (const int bin) const;

  virtual double & operator [] (const double energy);

  virtual double & operator [] (const int bin);

  virtual double lowerEnergy () const;

  virtual double upperEnergy () const;

  virtual double deltaEnergy () const;

  virtual int energyBins () const;

  virtual double random () const;

  virtual double averageEnergy () const;


private:
  PhotonSpectrum();

  double lowerEnergy_;

  double deltaEnergy_;

  std::vector<double> value_;

};

#endif // RICHDET_PHOTONSPECTRUM_H
