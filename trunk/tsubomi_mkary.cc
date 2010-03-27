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

    // open textfile and aryfile
    FILE *fin_text = fopen(textname.c_str(), "r");
    if (fin_text == NULL) { throw "error at tsubomi_mkary::main(). filename cannot open."; }
    string aryname = textname + ".ary";
    FILE *fout_ary = fopen(aryname.c_str(), "wb");
    if (fout_ary == NULL) { throw "error at tsubomi_mkary::main(). filename.ary cannot open."; }

    // make aryfile and write
    tsubomi::indexer tbm(fin_text);
    tbm.mkary(seps.c_str());
    tbm.write(fout_ary);
  } catch (const char *err) {
    cout << err << endl;
    return 1;
  }
  return 0;
}
