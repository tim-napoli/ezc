#!/bin/bash

./ezc samples/test-gl.ez > test-gl.cpp
g++ test-gl.cpp -DEZ_WITH_GLFW -std=c++11 -o test-gl -lglfw3 -lrt -lXrandr -lXinerama -lXi -lXxf86vm -lXcursor -lGL -lm -lXrender -lXfixes -lXext -lX11 -lpthread -lxcb -lXau -lXdmcp -ldl

