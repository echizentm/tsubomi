// $Id$
#include <iostream>
#include "tsubomi_compressor.h"

using namespace std;

// tsubomi_mkcsa: tsubomi make compressed suffix array
int main(int argc, char **argv) {
  try {
   // read command line parameters
    const char *textname = "";
    const char *seps     = "";
    bool is_help         = false;
    bool is_progress     = false;
    char param           = '\0';
    for (int i = 1; i < argc; i++) {
      if (argv[i][0] == '-') {
        if (argv[i][1] == '\0') { continue; }
        switch (argv[i][1]) {
          case 'p': is_progress = true;       break;
          case 'h': is_help     = true;       break;
          default : param       = argv[i][1]; break;
        }
      } else if (param) {
        param = '\0';
      } else {
        textname = argv[i];
      }
    }

    // write help message and exit
    if (textname[0] == '\0' || is_help) {
      cout << "[USAGE] tsubomi_mkcsa [params] textfile" << endl;
      cout << "[OPTIONS]" << endl;
      cout << "  p   : print progress bar while making suffix array." << endl;
      cout << "  h   : print help message." << endl;
      return 0;
    }

    tsubomi::compressor tbm();
    tbm.mkcsa(textname, is_progress);
    tbm.write(textname);
  } catch (const char *err) {
    cerr << err << endl;
    return 1;
  }
  return 0;
}
