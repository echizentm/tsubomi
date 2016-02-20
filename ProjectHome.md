**TSUBOMI** is a Full-Text Search library using Compressed Suffix Array.<br />
This library is inspired by SUFARY(http://nais.to/~yto/tools/sufary/)

```
Copyright (c) 2010 Toshiyuki Maezawa

This software is freeware.
You can use this software for any purposes,
but the author is NOT responsible for any problems about using it.
===============================
このソフトウェアはフリーウェアです。
ご自由に利用していただいて構いませんが
本ソフトを使用したことによるいかなる問題についても作者は責任を負いかねます。
```

**INSTALL**
```
$$ cd src/tsubomi/
$$ make
$$ sudo make install
$$ cd ../../

# edit Makefile if you need.
```

**UNINSTALL**
```
$$ cd src/tsubomi/
$$ sudo make uninstall
$$ cd ../../
```

**USAGE(suffix array)**
```
$$ cat sample.txt
abracadabra

$$ tsubomi_mkary sample.txt
$$ tsubomi_search sample.txt abra
abracadabra : 7
abracadabra : 0

# add -h option if you need help message
```

**USAGE(compressed suffix array)**
```
$$ cat sample.txt
abracadabra

$$ tsubomi_mkary sample.txt
$$ tsubomi_mkcsa sample.txt
$$ tsubomi_search -c sample.txt abra
abracadabra : 7
abracadabra : 0

# add -h option if you need help message
```



&lt;hr/&gt;


**FUTABA** is a simple matching engine that supports tfidf and sgd-svm.<br />
Install TSUBOMI before using FUTABA.

**INSTALL**
```
$$ cd src/futaba/
$$ make
$$ sudo make install
$$ cd ../../

# edit Makefile if you need.
```

**UNINSTALL**
```
$$ cd src/futaba/
$$ sudo make uninstall
$$ cd ../../
```

**USAGE(tfidf)**
```
$$ futaba_make_tfidf.pl < var/color.txt > var/color.tfidf
$$ tsubomi_mkary -l var/color.tfidf
$$ futaba_match var/color.tfidf < var/color.txt

# execute with no options if you need help message
```

**USAGE(sgd-svm)**
```
$$ futaba_make_svm.pl 10 0.1 < var/color.txt > var/color.tfidf
$$ tsubomi_mkary -l var/color.tfidf
$$ futaba_match var/color.tfidf < var/color.txt

# execute with no options if you need help message
```