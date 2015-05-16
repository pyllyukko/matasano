#!/bin/bash
################################################################################
# created:	27-05-2013
################################################################################
set -u

FILE="08.txt"
declare -i I=1

if [ ! -f "${FILE}" ]
then
  echo "error: file \`${FILE}' not found!" 1>&2
  exit 1
fi

while read
do
  if [ ${#REPLY} -ne 320 ]
  then
    echo "hmm. line != 320 bytes?" 1>&2
  fi
  UNIQ_BLOCKS=$( echo -n "${REPLY}" | sed 's/\([a-f0-9]\{32\}\)/\1\n/g' | sort | uniq | wc -l )
  if [ ${UNIQ_BLOCKS} -ne 10 ]
  then
    echo "line no. ${I} has only ${UNIQ_BLOCKS} unique blocks:"
    echo -n "${REPLY}" | sed 's/\([a-f0-9]\{32\}\)/\1\n/g' | sort | uniq -c
    break
  fi
  let I++
done 0<"${FILE}"
