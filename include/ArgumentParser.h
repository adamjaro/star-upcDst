#ifndef ArgumentParser_h
#define ArgumentParser_h

#include <map>
#include <boost/tokenizer.hpp>

class ArgumentParser {

  typedef std::map<std::string, Double_t*> map_double;
  typedef std::map<std::string, Int_t*> map_int;
  typedef std::map<std::string, Bool_t*> map_bool;
  typedef std::map<std::string, std::string*> map_string;

  map_double fMapDouble;
  map_int fMapInt;
  map_bool fMapBool;
  map_string fMapString;

  std::string fConfigDir; // directory with configuration file

public:

  ArgumentParser();
  virtual ~ArgumentParser();

  void AddDouble(std::string name, Double_t *par) { fMapDouble.insert(make_pair(name, par)); }
  void AddInt(std::string name, Int_t *par) { fMapInt.insert(make_pair(name, par)); }
  void AddBool(std::string name, Bool_t *par) { fMapBool.insert(make_pair(name, par)); }
  void AddString(std::string name, std::string *par) { fMapString.insert(make_pair(name, par)); }

  void SetConfigDirectory(std::string dir) { fConfigDir = dir; }

  Bool_t Parse(std::string configfile);


private:

  typedef boost::tokenizer< boost::char_separator<char> >::iterator token_it;
  template<class par_type> Bool_t LoadPar(std::map<std::string, par_type*> cmap, token_it cline_it);

};

#endif



















