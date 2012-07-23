// Include files
// local
#include "LbAlpGen/AlpGenFunctions.h"

//-----------------------------------------------------------------------------
// Implementation file for class : AlpGenFunctions
//
// 2012-07-13 : Patrick Robbe, Stephane Tourneur
//-----------------------------------------------------------------------------

extern "C" {
  void alsprc_() ;
}

void AlpGenFunctions::AlSprc( ) {
  alsprc_() ;
}

extern "C" {
  void alsdef_() ;
}

void AlpGenFunctions::AlSdef( ) 
{
  alsdef_() ;
}

extern "C" {
  void alhset_() ;
}

void AlpGenFunctions::AlHset( ) 
{
  alhset_() ;
}

extern "C" {
  void alinit_() ;
}

void AlpGenFunctions::AlInit( ) 
{
  alinit_() ;
}

extern "C" {
  void alsbkk_() ;
}

void AlpGenFunctions::AlSbkk( ) 
{
  alsbkk_() ;
}

extern "C" {
  void alshis_() ;
}

void AlpGenFunctions::AlShis( ) 
{
  alshis_() ;
}

extern "C" {
  void alsgrd_() ;
}

void AlpGenFunctions::AlSgrd( ) 
{
  alsgrd_() ;
}

extern "C" {
  void aligrd_() ;
}

void AlpGenFunctions::AlIgrd() 
{
  aligrd_() ;
}

extern "C" {
  void alegen_() ;
}

void AlpGenFunctions::AlEgen( ) 
{
  alegen_() ;  
}

extern "C" {
  void alfhis_() ;
}

void AlpGenFunctions::AlFhis( ) 
{
  alfhis_() ;
}

extern "C" {
  void alfbkk_() ;
}

void AlpGenFunctions::AlFbkk( ) 
{
  alfbkk_() ;
}

extern "C" {
  void alsfin_() ;
}

void AlpGenFunctions::AlSfin( ) 
{
  alsfin_() ;
}
