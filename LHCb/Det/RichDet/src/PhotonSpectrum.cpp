// $Id: PhotonSpectrum.cpp,v 1.2 2001-10-26 13:02:13 rihill Exp $
// Include files
#include <cassert>
#include <numeric>
#include "CLHEP/Random/RandFlat.h"

// PhotonSpectrum
#include "RichDet/PhotonSpectrum.h"

  PhotonSpectrum::PhotonSpectrum(const PhotonSpectrum &right)
    : lowerEnergy_(right.lowerEnergy_), 
    deltaEnergy_(right.deltaEnergy_),
    value_(right.value_)
  {
  }

  PhotonSpectrum::PhotonSpectrum (const int nbin, 
                                  const double lower, 
                                  const double upper)
      : lowerEnergy_(lower),
      deltaEnergy_( (upper - lower) / nbin ),
      value_(nbin)
  {
  }

  PhotonSpectrum::PhotonSpectrum (const double lower, 
                                  const double delta, 
                                  const double* value)
      : lowerEnergy_(lower), deltaEnergy_( delta )
  {
      while ( *value > 0 ) {
	value_.push_back( *value );
	++value;
      }
  }


  PhotonSpectrum::~PhotonSpectrum()
  {
  }

  PhotonSpectrum & PhotonSpectrum::operator=(const PhotonSpectrum &right)
  {
    lowerEnergy_ = right.lowerEnergy_; 
    deltaEnergy_ = right.deltaEnergy_;
    value_       = right.value_;    
    return *this;
  }

  PhotonSpectrum & PhotonSpectrum::operator *= (const double scalar)
  {

      for ( vector<double>::iterator i = value_.begin();
	    i != value_.end();
	    i++ ) {
	(*i) *= scalar;
      };

      return *this;

  }

  PhotonSpectrum & PhotonSpectrum::operator *= (const PhotonSpectrum &spectrum)
  {
      
      //
      // Exceptions would be more elegent
      //

      assert( this->value_.size() == spectrum.value_.size() );
      assert( this->lowerEnergy_  == spectrum.lowerEnergy_ );
      assert( this->deltaEnergy_  == spectrum.deltaEnergy_ );

      vector<double>::const_iterator j = spectrum.value_.begin();
      for ( vector<double>::iterator i = this->value_.begin();
	    i != this->value_.end();
	    i++ ) {
	*i *= *j++;
      };
      
      return *this;

  }

  double PhotonSpectrum::integral () const
  {

      return accumulate(value_.begin(),value_.end(),0.);
      
      
  }

  const double & PhotonSpectrum::operator [] (const double energy) const
  {
      int bin = int( ( energy - lowerEnergy_ ) / deltaEnergy_ );
      return value_[bin];
  }

  const double & PhotonSpectrum::operator [] (const int bin) const
  {
      return value_[bin];
  }

  double & PhotonSpectrum::operator [] (const double energy)
  {
      int bin = int( ( energy - lowerEnergy_ ) / deltaEnergy_ );
      return value_[bin];
  }

  double & PhotonSpectrum::operator [] (const int bin)
  {
      return value_[bin];
  }

  double PhotonSpectrum::lowerEnergy () const
  {
      return lowerEnergy_;
  }

  double PhotonSpectrum::upperEnergy () const
  {
      return lowerEnergy_ + deltaEnergy_ * value_.size();
  }

  double PhotonSpectrum::deltaEnergy () const
  {
      return deltaEnergy_;
  }

  int PhotonSpectrum::energyBins () const
  {
    return value_.size(); 
  }

  double PhotonSpectrum::random () const
  {
    double sum       = this->integral();
    double randValue = RandFlat::shoot(sum);
    double energy    = lowerEnergy_;
    for(vector<double>::const_iterator value = value_.begin();
	value != value_.end();
	++value) {
      if ( *value > 0. ) {
	randValue -= *value;
	if ( randValue <= 0. ) {
	  //	  return energy + 0.5 * deltaEnergy_;
	  return energy - ( randValue / *value ) * deltaEnergy_;
	}
      energy += deltaEnergy_;
      }
    }
    cout << "Sum " << sum << " : " << randValue << endl; 
    assert(false);
    return energy;
  }

  double PhotonSpectrum::averageEnergy () const
  {
    double sumValues = 0;
    double avgEnergy = 0;
    double energy = lowerEnergy_ + 0.5 * deltaEnergy_;
    for(vector<double>::const_iterator value = value_.begin();
	value != value_.end();
	++value) {
      sumValues += *value;
      avgEnergy += *value * energy;   
      energy += deltaEnergy_;
    }
    return avgEnergy / sumValues;
  }

  // Additional Declarations

// } // namespace Rich

