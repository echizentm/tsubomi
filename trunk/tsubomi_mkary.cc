// $Id$
#include <string>
#include "tsubomi.h"

using namespace std;

// tsubomi_mkary: tsubomi make suffix array
int main(int argc, char **argv) {
  try {
   // read command line parameters
    string textname  = "";
    string seps      = "";
    bool   is_help   = false;
    char   param     = '\0';
    for (int i = 1; i < argc; i++) {
      if (argv[i][0] == '-') {
        if (argv[i][1] == '\0') { continue; }
        switch (argv[i][1]) {
          case 'l': seps      = "\n";       break;
          case 'w': seps      = " \t\n";    break;
          case 'h': is_help   = true;       break;
          default : param     = argv[i][1]; break;
        }
      } else if (param) {
        switch (param) {
          case 't': seps = string(argv[i]); break;
        }
        param = '\0';
      } else {
        textname = string(argv[i]);
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
      cout << "  h   : print help message." << endl;
      return 0;
    }

    // make aryfile and write
    tsubomi::indexer tbm(textname.c_str());
    tbm.mkary(seps.c_str());
    string aryname = textname + ".ary";
    tbm.write(aryname.c_str());
  } catch (const char *err) {
    cout << err << endl;
    return 1;
  }
  return 0;
}
