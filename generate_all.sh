#!/bin/bash

OUT_DIR=output

for file in *.png ; do
	sdaw -g "$file"
done

mkdir -p ${OUT_DIR}
mv *.wav ${OUT_DIR}
