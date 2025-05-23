# Computer_Vision_final_project
Computer_Vision




# Chessboard Detection with OpenCV

This project is part of the final assignment for **CSCI 612 – Applied Computer Vision**, and explores machine vision techniques to detect a chessboard and its pieces using OpenCV in C++.

##  Project Objective

Develop a vision system that can:
- Detect a chessboard under various lighting conditions
- Label grid squares (A1 to H8)
- Detect pieces on the board using computer vision techniques
- Perform in near real-time on embedded hardware

##  Technologies Used
- **Language:** C++
- **Library:** OpenCV
- **Hardware:** Logitech C270 webcam, Jetson Nano 2GB (or compatible Linux device)
- **Tools:** Histogram Equalization, CLAHE, Canny Edge Detection, Hough Line Transform, Contour Detection

---

##  Detection Methods

### 1. `cv::findChessboardCorners`
- **Script:** [`chessBoard_detection.cpp`](https://github.com/Ace1032/Computer_Vision_final_project/blob/main/chessBoard_detection.cpp)
- **Outcome:** Works in ideal lighting but fails in low light.

### 2. CLAHE for Low Light Enhancement
- **Script:** [`chessBoard_Low_lighting.cpp`](https://github.com/Ace1032/Computer_Vision_final_project/blob/main/chessBoard_Low_lighting.cpp)
- **Outcome:** Applied adaptive histogram equalization. Slightly improved detection but not reliable.

### 3. Canny + Hough Transform
- **Script:** [`cheassboard_hough_detection.cpp`](https://github.com/Ace1032/Computer_Vision_final_project/blob/main/cheassboard_hough_detection.cpp)
- **Outcome:** Custom pipeline using:
  - Grayscale conversion
  - Gaussian blur
  - Canny edge detection
  - Hough line detection

---

##  Chessboard Labeling

- Intersections of detected lines were analyzed to form an 8x8 matrix.
- Edge lines were trimmed and sorted:
```cpp
std::sort(horizontalRhos.begin(), horizontalRhos.end());
std::sort(verticalRhos.begin(), verticalRhos.end());



## Replace horizontalRhos and verticalRhos to contour-based board detection

### looking to improve 
Robust to real-world imperfections
Detects one clear board region
Simple bounding box + divide by 8


## Hu Moments for Chess Piece Classification

for each detected pice conver the squre to binary using threshold
Extract the Largest Contour
Compute Hu Moments
Compare to Known Templates
most detected pices still giving the same Hu Moments
replace threshold. 
cv::threshold(gray, binary, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);
using adaptive threshold insted:
cv::adaptiveThreshold(gray, binary, 255,cv::ADAPTIVE_THRESH_GAUSSIAN_C,cv::THRESH_BINARY_INV, 11, 2);


## Issue: Detection Loop Stuck on First Square
when running the program, the problem gets stuck on the first squre (A1) and does not proceed to analyze the rest of the chessboard.
root cause:
inside the loop for detecting pieces, the program attemps to calculate Hu Moments for a contour even when no valied contour was found




###
