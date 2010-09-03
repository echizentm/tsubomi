// $Id$
#include <iostream>
#include <string>
#include "tsubomi_basic_searcher.h"

using namespace std;

// tsubomi_sass: tsubomi suffix array simple search
int main(int argc, char **argv) {
 try {
    // read command line parameters
    const char *textname = "";
    const char *keyword  = "";
    const char *seps     = "";
    bool is_help         = false;
    bool is_key          = false;
    bool is_offset       = false;
    bool is_snippet      = false;
    char param           = '\0';
    for (int i = 1; i < argc; i++) {
      if (argv[i][0] == '-') {
        if (argv[i][1] == '\0') { continue; }
        switch (argv[i][1]) {
          case 'l': seps       = "\n";       break;
          case 'w': seps       = " \t\n";    break;
          case 'k': is_key     = true;       break;
          case 'o': is_offset  = true;       break;
          case 'h': is_help    = true;       break;
          case 's': is_snippet = true;       break;
          default : param      = argv[i][1]; break;
        }
      } else if (param) {
        switch (param) {
          case 't': seps    = argv[i];       break;
        }
        param = '\0';
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
      cout << "[USAGE] tsubomi_sass [params] textfile keyword" << endl;
      cout << "[OPTIONS]" << endl;
      cout << "  l   : print matched strings as line." << endl;
      cout << "  w   : print matched strings as word." << 
                      "(word is separated by \" \\t\\n\")" << endl;
      cout << "  t(s): print matched strings to separators (s)." << endl;
      cout << "  k   : match only 1st columnn of tsv." << endl;
      cout << "  o   : print index offset for matched strings." << endl;
      cout << "  s   : snnipet mode. print left & right text of keyword." << endl;
      cout << "  h   : print help message." << endl;
      return 0;
    }

    // search matched strings
    tsubomi::basic_searcher tbm(textname);

    string keyword_str = keyword;
    if (is_key) { keyword_str += "\t"; }
    tsubomi::sa_range p = tbm.search(keyword_str.c_str());
    if (p.first < 0) { return 0; }

    for (tsubomi::sa_index i = p.first; i <= p.second; i++) {
      if (is_offset) {
        tsubomi::sa_index offset = tbm.get_offset(i);
        cout << offset << ": "; 
      }
      char buf[1024];
      if (is_snippet) {
        tbm.get_string(i, buf, 511, seps, 511, seps);
      } else {
        tbm.get_string(i, buf, 1024, seps);
      }
      cout << buf << endl;
    }
  } catch (const char *err) {
    cerr << err << endl;
    return 1;
  }
  return 0;
}