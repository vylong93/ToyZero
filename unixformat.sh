#!/bin/bash
dos2unix Makefile
dos2unix STM32F103C8Tx_FLASH.ld
cd Core
for f in $(find . -name "*.c"); do dos2unix $f; done
for f in $(find . -name "*.h"); do dos2unix $f; done
cd ..
