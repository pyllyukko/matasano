#!/bin/bash

dirs=(lib 1-03 1-04 1-05 1-06-hamming_distance 1-07 2-09-padding 2-10-cbc 2-11-ecb_detect 2-12-ecb_decrypt 2-13 2-14-ecb_decrypt 2-15-padding 2-16 3-21-mersenne_twister)
declare -a rets=()

for dir in ${dirs[*]}
do
  pushd "${dir}" 1>/dev/null || continue
  if [ -f Makefile ]
  then
    make
    rets+=(${?})
  elif [ -f "${dir%-*}.c" ]
  then
    gcc "${dir%-*}.c" -o "${dir%-*}"
    rets+=(${?})
  fi
  popd 1>/dev/null
done

echo -e "\nreturn values:"
for ret in ${rets[*]}
do
  echo "  ${ret}"
done
