// $Id$
#include <iostream>
#include <string>
#include "tsubomi_compressor.h"

using namespace std;

// tsubomi_csass: tsubomi compressed suffix array simple search
int main(int argc, char **argv) {
 try {
    // read command line parameters
    const char *textname = "";
    const char *keyword  = "";
    bool is_help         = false;
    char param           = '\0';
    for (int i = 1; i < argc; i++) {
      if (argv[i][0] == '-') {
        if (argv[i][1] == '\0') { continue; }
        switch (argv[i][1]) {
          case 'h': is_help    = true;       break;
          default : param      = argv[i][1]; break;
        }
      } else if (param) {
        param = '\0';
      } else {
        if (textname[0] == '\0') {
          textname = argv[i];
        } else {
          keyword  = argv[i];
        }
      }
    }

    // write help message and exit
    if (keyword[0] == '\0' || is_help) {
      cout << "[USAGE] tsubomi_csass [params] textfile keyword" << endl;
      cout << "[OPTIONS]" << endl;
      cout << "  h   : print help message." << endl;
      return 0;
    }

    // search matched strings
    tsubomi::compressor tbm;
    tbm.read(textname);

    tsubomi::sa_range p = tbm.search(keyword);
    if (p.first < 0) { return 0; }

    for (tsubomi::sa_index i = p.first; i <= p.second; i++) {
      char     buf[1024];
      tsubomi::sa_index pos;
      tbm.get_line(i, buf, 1024, &pos);
      cout << buf << "(" << pos << ")" << endl;
    }
  } catch (const char *err) {
    cerr << err << endl;
    return 1;
  }
  return 0;
}
