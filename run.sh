#!/bin/bash

cd build/
google-chrome http://localhost:8000/boids.html &
python -m http.server

