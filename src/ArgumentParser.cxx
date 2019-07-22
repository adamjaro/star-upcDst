
//c++ headers
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/tokenizer.hpp>

//ROOT headers
#include "Rtypes.h"

//local headers
#include "ArgumentParser.h"

using namespace std;
using namespace boost;

//_____________________________________________________________________________
ArgumentParser::ArgumentParser(): fConfigDir("") {

}//ArgumentParser

//_____________________________________________________________________________
ArgumentParser::~ArgumentParser() {

}//~ArgumentParser

//_____________________________________________________________________________
Bool_t ArgumentParser::Parse(std::string configfile) {

  if( !fConfigDir.empty() ) {
    configfile = fConfigDir + "/" + configfile;
  }

  char_separator<char> sep(" ");

  ifstream in(configfile.c_str());
  string line;
  Int_t n_missing = 0;
  //config file loop
  while( getline(in, line) ) {
    if(line.empty()) continue;
    if(line.find("#") == 0) continue;

    //split config line to name and value
    tokenizer< char_separator<char> > cline(line, sep);
    token_it cline_it = cline.begin();

    //double parameters
    if( LoadPar(fMapDouble, cline_it) ) continue;
    //integer parameters
    if( LoadPar(fMapInt, cline_it) ) continue;
    //boolean parameters
    if( LoadPar(fMapBool, cline_it) ) continue;
    //string parameters
    if( LoadPar(fMapString, cline_it) ) continue;

    //parameter not found

    ++n_missing;
    cout << "Error in ArgumentParser::Parse, parameter not found: " << line << endl;

  }//config file loop

  if( n_missing == 0 ) return kTRUE;

  cout << "Number of unrecognized parameters: " << n_missing << endl;

  return kFALSE;

}//Parse

//_____________________________________________________________________________
template<class par_type> Bool_t ArgumentParser::LoadPar(std::map<std::string, par_type*> cmap, token_it cline_it) {

  //load parameter value from map

  typename map<std::string, par_type*>::iterator val_it;

  val_it = cmap.find(*cline_it);
  if( val_it == cmap.end() ) return kFALSE;

  istringstream st(*(++cline_it));
  par_type val;
  st >> val;
  *((*val_it).second) = val;

  return kTRUE;

}//LoadPar






















