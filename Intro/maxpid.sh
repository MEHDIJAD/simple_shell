#!/bin/bash

MAX_PID=$(sysctl -n kern.maxproc) 

echo "The maximum process ID (PID) on macOS is: $MAX_PID"