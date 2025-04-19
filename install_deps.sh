if [ ! -d ./deps/cstd ]; then
  git clone https://github.com/jmatth11/cstd.git deps/cstd
  cd deps/cstd
  make
  zig build
  cd -
fi
