
//C++
#include <vector>
#include <iostream>

//StRoot
#include "StPicoEvent/StPicoDst.h"
#include "StPicoEvent/StPicoTrack.h"
#include "StPicoEvent/StPicoEvent.h"

//local classes
#include "StUPCSelectCEP.h"
#include "StUPCTrack.h"

using namespace std;

//_____________________________________________________________________________
StUPCSelectCEP::StUPCSelectCEP(){
  //default constructor
  /*
  hTof = new TH1I("hTof", "hTof", 50, 0, 49);
  hBemc = new TH1I("hBemc", "hBemc", 50, 0, 49);
  hFast = new TH1I("hFast", "hFast", 50, 0, 49);
  */
}//StUPCSelectCEP



//_____________________________________________________________________________
int StUPCSelectCEP::selectTracks(StPicoDst *pd, std::vector<UChar_t>& sel) {

  //cout << "StUPCSelectCEP::selectTracks" << endl;

  //run over global tracks and store all global tracks of interest.

  int nfound = 0;
  vector<int> hadronId;
  int totalCharge = 0; 
  TVector3 vertex = pd->event()->primaryVertex(); 

  StPicoTrack *ptrack;
  int nTof = 0;
  int nBemc = 0;
  for(UInt_t itrk=0; itrk < pd->numberOfTracks(); itrk++) {
    ptrack = pd->track(itrk); 
    if ( !ptrack ) 
      continue;
    if ( ptrack->isPrimary() ) // skip all tracks asigned to the main vertex 
    {
      if ( ptrack->isTofTrack() )
        nTof++;
      if ( ptrack->isBemcTrack() )
        nBemc++;
      continue;
    }
    if ( !ptrack->isTofTrack() )
      continue;
    if ( ptrack->nHitsFit() < 25 ) 
      continue;
    if ( TMath::Abs(ptrack->nHitsDedx()) < 15 ) 
      continue;
    if ( ptrack->gDCA(vertex).Mag()  < 10 ) 
      continue;

    hadronId.push_back(itrk);
    totalCharge += static_cast<int>( ptrack->charge() );
  }
 
  if(hadronId.size() != 2)
    return nfound;

  if(totalCharge) 
    return nfound;
/*
  hTof->Fill(nTof);
  hBemc->Fill(nBemc);
  hFast->Fill(nTof+nBemc);
*/
/*
  double zVertex = 0;
  double vertexWidth = 0;
  for (unsigned int id = 0; id < hadronId.size(); ++id)
  {
    ptrack = pd->track( hadronId[id] );  // Pointer to a track
    zVertex += ptrack->origin().Z();
    if( id == 0)
       vertexWidth += ptrack->origin().Z();
    if( id == 1)
       vertexWidth -= ptrack->origin().Z();
  }

  vertexWidth = TMath::Abs( vertexWidth );
  zVertex = zVertex/2;

  if(TMath::Abs(zVertex) > 100)
    return nfound;

  if( vertexWidth > 10)
    return nfound;
*/

  //counter for found V0 candidates
  nfound += hadronId.size();

  //mark the tracks as used
  for (unsigned int id = 0; id < hadronId.size(); ++id)
    sel[ hadronId[id] ] |= (1 << StUPCTrack::kCEP);

  return nfound;

}//selectTracks

