#!/bin/bash

 if [ "$1" = "web" ]; then
	cd build/
	python -m http.server
 else
	./build/boids
 fi
