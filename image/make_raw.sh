#! /bin/bash
convert $1 -depth 8 -size $2x$3 rgba:${1%.*}.raw
#./revimg ${1%.*}.raw 4