    
#ifndef StUPCSelectCEP_h
#define StUPCSelectCEP_h

#include "TH1I.h"

class StUPCSelectCEP {

 public:

  StUPCSelectCEP();
  int selectTracks(StPicoDst *pd, std::vector<UChar_t>& sel);

 private:
    TH1I *hTof, *hBemc, *hFast; 

};

#endif

