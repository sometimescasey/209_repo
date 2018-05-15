#!/usr/bin/env bash

./echo_arg csc209 > echo_out.txt
./echo_stdin < echo_stdin.c
./count 210 | wc --chars
echo $(ls --sort=size -1 | head -1) | ./echo_stdin