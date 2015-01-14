#!/bin/bash

./build/programs/suffix_array $1 $1.sa
./build/programs/memer $1 $1.sa $2 $3 $4
