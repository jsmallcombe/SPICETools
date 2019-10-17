fname=$(basename $1)
cd JTS
bash energy_bash.sh "../input/$fname"
rm -rf CLXDCY
cd ..