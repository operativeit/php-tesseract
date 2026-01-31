dnl config.m4
PHP_ARG_WITH(tesseract, [ Whether to enable the Tesseract OCR extension ],
[  --with-tesseract[=DIR]    Where to find libtesseract and headers ])

if test "$PHP_TESSERACT" != "no"; then
  dnl Use pkg-config if available
  AC_PATH_PROG(PKG_CONFIG, pkg-config, no)
  AC_MSG_CHECKING([for libtesseract])
  
  TESS_FOUND=0
  if test "$PKG_CONFIG" != "no" && $PKG_CONFIG --exists tesseract; then
    TESS_INCLUDE=`$PKG_CONFIG --cflags-only-I tesseract | sed 's/-I//g' | xargs`
    TESS_LIBDIR=`$PKG_CONFIG --libs-only-L tesseract | sed 's/-L//g' | xargs`
    TESS_LIBS=`$PKG_CONFIG --libs-only-l tesseract | xargs`
    AC_MSG_RESULT([found via pkg-config])
    PHP_ADD_INCLUDE($TESS_INCLUDE)
    PHP_EVAL_LIBLINE($TESS_LIBS, TESSERACT_SHARED_LIBADD)
    if test -n "$TESS_LIBDIR"; then
      PHP_ADD_LIBRARY_WITH_PATH(tesseract, $TESS_LIBDIR, TESSERACT_SHARED_LIBADD)
    fi
    TESS_FOUND=1
  else
    dnl Manual search fallback
    for i in $PHP_TESSERACT /opt/homebrew /usr/local /usr; do
      if test -f "$i/include/tesseract/baseapi.h"; then
        PHP_TESSERACT_DIR=$i
        break
      fi
    done
    
    if test -n "$PHP_TESSERACT_DIR"; then
      AC_MSG_RESULT([found in $PHP_TESSERACT_DIR])
      PHP_ADD_INCLUDE($PHP_TESSERACT_DIR/include)
      PHP_ADD_LIBRARY_WITH_PATH(tesseract, $PHP_TESSERACT_DIR/lib, TESSERACT_SHARED_LIBADD)
      TESS_FOUND=1
    fi
  fi

  if test "$TESS_FOUND" = "0"; then
    AC_MSG_ERROR([tesseract not found])
  fi

  AC_MSG_CHECKING([for libleptonica])
  LEPT_FOUND=0
  if test "$PKG_CONFIG" != "no" && $PKG_CONFIG --exists lept; then
    LEPT_INCLUDE=`$PKG_CONFIG --cflags-only-I lept | sed -e 's/-I//g' -e 's/\/leptonica$//' | xargs`
    LEPT_LIBDIR=`$PKG_CONFIG --libs-only-L lept | sed 's/-L//g' | xargs`
    LEPT_LIBS=`$PKG_CONFIG --libs-only-l lept | xargs`
    AC_MSG_RESULT([found via pkg-config])
    PHP_ADD_INCLUDE($LEPT_INCLUDE)
    if test -n "$LEPT_LIBDIR"; then
      PHP_ADD_LIBRARY_WITH_PATH(leptonica, $LEPT_LIBDIR, TESSERACT_SHARED_LIBADD)
    fi
    LEPT_FOUND=1
  else
    dnl Manual search fallback
    for i in $PHP_TESSERACT /opt/homebrew /usr/local /usr; do
      if test -f "$i/include/leptonica/allheaders.h"; then
        PHP_LEPTONICA_DIR=$i
        break
      fi
    done

    if test -n "$PHP_LEPTONICA_DIR"; then
      AC_MSG_RESULT([found in $PHP_LEPTONICA_DIR])
      PHP_ADD_INCLUDE($PHP_LEPTONICA_DIR/include)
      PHP_ADD_LIBRARY_WITH_PATH(leptonica, $PHP_LEPTONICA_DIR/lib, TESSERACT_SHARED_LIBADD)
      LEPT_FOUND=1
    fi
  fi

  if test "$LEPT_FOUND" = "0"; then
    AC_MSG_ERROR([leptonica not found])
  fi

  PHP_SUBST(TESSERACT_SHARED_LIBADD)
  PHP_REQUIRE_CXX()
  PHP_NEW_EXTENSION(tesseract, tesseract.cpp, $ext_shared,, -std=c++11)
fi
