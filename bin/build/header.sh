echo "// SPICE library master " $(date "+%d %b %Y") > include/smaster.h

for f in include/spice_*.h
do
h=${f:8:${#f}} 
echo "#include <"$h">" >> include/smaster.h
done
