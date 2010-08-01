// $Id$
#include <iostream>
#include <string>
#include <vector>
#include "tsubomi_vertical_code.h"

// tsubomi_iiss: tsubomi inverted index simple search
using namespace std;
using namespace tsubomi;
int main(int argc, char **argv) {
  try {
   // read command line parameters
    const char *textname = "";
    const char *keyword  = "";
    bool        is_help  = false;
    for (int i = 1; i < argc; i++) {
      if (argv[i][0] == '-') {
        if (argv[i][1] == '\0') { continue; }
        switch (argv[i][1]) {
          case 'h': is_help = true; break;
        }
      } else {
        if (textname[0] == '\0') {
          textname = argv[i];
        } else {
          keyword  = argv[i];
        }
      }
    }

    // write helo message and exit
    if (keyword[0] == '\0' || is_help) {
      cout << "[USAGE] tsubomi_iiss [params] textfile keyword" << endl;
      cout << "[OPTIONS]" << endl;
      cout << "  h   : print help message." << endl;
      return 0;
    }

    // search inverted index
    string iiname = string(textname) + ".ii";
    vector<vertical_code *> vcs;
    read_vertical_codes(iiname.c_str(), vcs);
    ullong key_ull = atoi(keyword);
    vertical_code *pvc = vcs[key_ull];
    for (ullong i = 0; i < pvc->size(); i++) {
      cout << pvc->get(i) << " ";
    }
    cout << endl;
    clear_vertical_codes(vcs);
  } catch (const char *err) {
    cerr << err << endl;
    return 1;
  }
  return 0;
}
