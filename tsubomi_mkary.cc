// $Id$
#include <iostream>
#include "tsubomi.h"

using namespace std;

// tsubomi_mkary: tsubomi make suffix array
int main(int argc, char **argv) {
  try {
   // read command line parameters
    char *textname   = "";
    char *seps       = "";
    bool is_help     = false;
    bool is_progress = false;
    char param     = '\0';
    for (int i = 1; i < argc; i++) {
      if (argv[i][0] == '-') {
        if (argv[i][1] == '\0') { continue; }
        switch (argv[i][1]) {
          case 'l': seps        = "\n";       break;
          case 'w': seps        = " \t\n";    break;
          case 'p': is_progress = true;       break;
          case 'h': is_help     = true;       break;
          default : param       = argv[i][1]; break;
        }
      } else if (param) {
        switch (param) {
          case 't': seps = argv[i]; break;
        }
        param = '\0';
      } else {
        textname = argv[i];
      }
    }

    // write helo message and exit
    if (textname == "" || is_help) {
      cout << "[USAGE] tsubomi_mkary [params] textfile" << endl;
      cout << "[OPTIONS]" << endl;
      cout << "  l   : index fot each line." << endl;
      cout << "  w   : index fot each word." << 
                      "(word is separated by \" \\t\\n\")" << endl;
      cout << "  t(s): index for pointer after separators (s)." << endl;
      cout << "  p   : print progress bar while making suffix array." << endl;
      cout << "  h   : print help message." << endl;
      return 0;
    }

    // make aryfile and write
    tsubomi::indexer tbm(textname);
    tbm.mkary(seps, is_progress);
  } catch (const char *err) {
    cerr << err << endl;
    return 1;
  }
  return 0;
}
