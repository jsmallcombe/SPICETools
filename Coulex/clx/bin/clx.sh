#!/bin/bash

# read input an pipe to clx (in the same directory
# where this script is located)  after removing comments

instdir=$(dirname $(type -p $0))

if ! test -x $instdir/clx; then
    echo "$instdir/clx is not executable" 1>&2
    exit
fi

echo "Running $instdir/clx after removing comments from input file" 1>&2

cat $1 | awk '
$0 !~ /^#/ {
  if ($0 ~ /#/) {        # lines containing a #: remove everything after and in cluding #
    $0 = substr($0,1,index($0,"#")-1);
  }
  print;
}
' | $instdir/clx

exit 0
