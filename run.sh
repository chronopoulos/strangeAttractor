#!/bin/bash

pd-extended -audiodev 3 -noadc -nogui -audiobuf 15 pd/main.pd &
sleep 5
cpp/main | pdsend 8000
