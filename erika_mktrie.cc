// $Id$
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include "erika_louds_trie.h"

using namespace std;
using namespace erika;
int main(int argc, char **argv) {
  try {
    basic_trie bt;
    louds_trie lt;

    ifstream ifs(argv[1]);
    while (ifs.eof() == false) {
      string key;
      ifs >> key; 
      if (key.length() > 0) { bt.add(key, ""); }
    }
    cout << "dic read ok." << endl;
//    t.dump();
    bt.write(string(string(argv[1]) + ".bt").c_str());

    cout << "make louds." << endl;
    lt.build(bt);
//    t2.dump();
    lt.write(string(string(argv[1]) + ".lt").c_str());

  } catch (const char *err) {
    cerr << err << endl;
    return 1;
  }
  return 0;
}

