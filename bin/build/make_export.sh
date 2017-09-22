
echo "export SPICELIB="$(pwd) > bin/thisjlib.sh
echo "export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:\$SPICELIB/bin:\$SPICELIB/bin/build:\$SPICELIB">> bin/thisjlib.sh
echo "export CPATH=\$CPATH:\$SPICELIB/include">> bin/thisjlib.sh
echo "export PATH=\$PATH:\$SPICELIB/include:\$SPICELIB/src:\$SPICELIB/bin:\$SPICELIB/bin/build">> bin/thisjlib.sh
echo "export LIBRARY_PATH=\$LIBRARY_PATH:\$SPICELIB/bin:\$SPICELIB/bin/build:\$SPICELIB/src:\$SPICELIB/include:\$SPICELIB">> bin/thisjlib.sh
