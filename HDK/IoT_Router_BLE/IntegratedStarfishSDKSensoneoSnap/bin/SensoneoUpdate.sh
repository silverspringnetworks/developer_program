#!/bin/bash
PYTHONUNBUFFERED=1; export PYTHONUNBUFFERED
sensoneo -n $1 -t $2 $3
