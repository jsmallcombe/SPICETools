fname=$(basename $1)
echo "CLX output" > "output/$fname.txt"
bin/clx <"input/$fname" | bin/clx_mk_table.awk | tee -a "output/$fname.txt"
rm -rf CLXDCY