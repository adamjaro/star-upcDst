#ifndef StUPCEemcTower_h
#define StUPCEemcTower_h

//_____________________________________________________________________________
//    Class for storing EEMC tower information
//    Author: Alex Jentsch
//    Date: 3/16/2022
//_____________________________________________________________________________

#include "TObject.h"

class StUPCEemcTower: public TObject
{

public:

  StUPCEemcTower();
  ~StUPCEemcTower() {}

  //setters
  void setEta(Float_t eta) { mEta = eta; }
  void setPhi(Float_t phi) { mPhi = phi; }
  void setSigmaEta(Float_t se) { mSigmaEta = se; }
  void setSigmaPhi(Float_t sp) { mSigmaPhi = sp; }
  void setEnergy(Float_t en) { mEnergy = en; }
  void setTowerId(UInt_t id) { mId = id; }


  //getters
  Float_t getEta() const { return mEta; }
  Float_t getPhi() const { return mPhi; }
  Float_t getSigmaEta() const { return mSigmaEta; }
  Float_t getSigmaPhi() const { return mSigmaPhi; }
  Float_t getEnergy() const { return mEnergy; }
  UInt_t  getTowerId() const { return mId; }


private:

  StUPCEemcTower(const StUPCEemcTower &o); //not implemented
  StUPCEemcTower &operator=(const StUPCEemcTower &o); //not implemented

  Float_t mEta; // eta of cluster
  Float_t mPhi; // phi of cluster
  Float_t mSigmaEta; // sigma of eta
  Float_t mSigmaPhi; // sigma of phi
  Float_t mEnergy; // energy of cluster;
  UInt_t mId; // original ID







  ClassDef(StUPCEemcTower, 2)

};

#endif






































