mkdir build-tmp
cd build-tmp
cmake -DCMAKE_INSTALL_PREFIX=${PREFIX} ${SRC_DIR}
make
#make install
mkdir -p ${PREFIX}/lib/python${PY_VER}/site-packages
cp src/sim.so ${PREFIX}/lib/python${PY_VER}/site-packages
