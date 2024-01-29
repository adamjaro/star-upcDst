
#ifndef StUPCMakerFromPicoDst_h
#define StUPCMakerFromPicoDst_h

class StPicoDstMaker;
class StPicoDst;
class StUPCEvent;
class StUPCSelectV0;
class StUPCSelectCEP;
class StPicoTrack;
class St_db_Maker;

#include "StMaker.h"
#include "TVector3.h" // why there are no other root classes: TFile, TH1I...

class StUPCMakerFromPicoDst: public StMaker {

 public:

  StUPCMakerFromPicoDst(StPicoDstMaker *pm, string outnam);
  virtual ~StUPCMakerFromPicoDst() {}

  Int_t Init();
  Int_t Make();
  Int_t Finish();

 private:

  void setUpcTrackFromPicoTrack(StUPCTrack *utrk, StPicoTrack *ptrk, bool writeBemc=false);
  void readBeamLine();
  double calculateTofPathLength(const TVector3 beginPoint, const TVector3 endPoint, const double curvature);

  StPicoDstMaker *mPicoDstMaker; // maker for picoDst provided to the constructor
  StPicoDst *mPicoDst; // input picoDst data

  StEmcGeom *mBemcGeom; // BEMC geometry

  string mOutName;  // name of the output file
  TFile *mOutFile;  // output file

  TList *mHistList; // list of output histograms

  TH1I *mCounter; // analysis counter
  enum EvtCount{ kAna=1, kWritten, kMaxCnt };

  StUPCEvent *mUPCEvent; // output UPC event
  TTree *mUPCTree; // output tree

  St_db_Maker *mDbMk; // database to read beam-line position

  StUPCSelectV0 *mSelectV0; // selector for tracks from V0 candidates
  StUPCSelectCEP *mSelectCEP; // selector for tracks from CEP candidates

  ClassDef(StUPCMakerFromPicoDst, 1);

};

#endif

