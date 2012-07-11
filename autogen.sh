#!/bin/sh
#
aclocal
automake --foreign --copy --ignore-deps --add-missing
autoconf
rm -f stamp-h1
touch defines.h.in

# Run configure for this platform
#./configure $*
echo "Now you are ready to run ./configure"
