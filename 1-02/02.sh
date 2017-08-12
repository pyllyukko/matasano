#!/bin/bash
################################################################################
# created:	13-05-2013
################################################################################
if [ ${BASH_VERSINFO[0]} -ne 4 ]
then
  echo -e "error: bash version != 4, this script might not work properly!" 1>&2
  echo    "       you can bypass this check by commenting out lines $[${LINENO}-2]-$[${LINENO}+2]." 1>&2
  exit 1
fi
export LANG=en_US
export LC_ALL=C
set -u

function chr() {
  # chr() - converts decimal value to its ASCII character representation
  case ${1} in
    # this is stupid, but i couldn't figure out any other way to do this. we
    # use "echo -en" to print the decrypted ciphertext.
    10)
      echo -n '\n'
    ;;
    *)
      printf \\$(printf '%03o' ${1})
    ;;
  esac
  return ${?}
}

#HEX_STRINGS=(
#  "1c0111001f010100061a024b53535009181c"
#  "686974207468652062756c6c277320657965"
#)
HEX1="1c0111001f010100061a024b53535009181c"
HEX2="686974207468652062756c6c277320657965"
STRING=""
declare -a HEX_BYTES=()

for ((I=0; I<$[ ${#HEX1} / 2 ]; I++))
do
  HEX_BYTES=( ${HEX1:$[ I * 2 ]:2} ${HEX2:$[ I * 2 ]:2} )
  let DEC1=0x${HEX_BYTES[0]}
  let DEC2=0x${HEX_BYTES[1]}
  XORRED=$[ DEC1 ^ DEC2 ]
  printf -v XORRED_HEX '%.2x' ${XORRED}
  STRING+="${XORRED_HEX}"
done

if [ "${STRING}" != "746865206b696420646f6e277420706c6179" ]
then
  echo "error!" 1>&2
fi
echo "${STRING}"

exit 0
