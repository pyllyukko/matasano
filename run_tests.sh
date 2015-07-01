#!/bin/bash

dirs=(lib 03 04 05 06-hamming_distance 07 09-padding 10-cbc 11-ecb_detect 12-ecb_decrypt 13 14-ecb_decrypt 15-padding 16 21-mersenne_twister)
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
