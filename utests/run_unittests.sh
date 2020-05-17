#!/usr/bin/env bash


NO_LOST_PATTERN="definitely lost: 0 bytes"
NO_ERROR_PATTERN="ERROR SUMMARY: 0 errors"
VALGRIND_LOG="valgrind.log"

remove_logs () {
  rm -f ${VALGRIND_LOG}
  rm -f ${VALGRIND_LOG}.*
}

PROG_PATH=${1}
if [ ! -f "${PROG_PATH}" ]; then
    echo "Invalid prog path: '${PROG_PATH}'"
    exit 1
fi


echo "Unit testing"
valgrind --tool=memcheck --leak-check=summary --log-file=${VALGRIND_LOG} "${PROG_PATH}"

if [ "$?" -ne 0 ]; then
  echo "Unit testing FAILED"
  remove_logs
  exit 1
fi

NO_LOST=$(grep "${NO_LOST_PATTERN}" ${VALGRIND_LOG})
NO_ERROR=$(grep "${NO_ERROR_PATTERN}" ${VALGRIND_LOG})
if [ -z "${NO_LOST}" ] || [ -z "${NO_ERROR}" ]; then
    echo -e "Memory leak testing FAILED\n"
    cat ${VALGRIND_LOG}
    remove_logs
    exit 1
fi
remove_logs
echo "Memory leak testing PASSED"