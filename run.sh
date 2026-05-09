#!/bin/bash

cd build/
helium http://localhost:8000/boids.html &
python -m http.server

