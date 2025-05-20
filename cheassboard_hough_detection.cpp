#include <opencv2/opencv.hpp>
#include <iostream>

#include <map>
#include <algorithm>
#include <cmath>



std::vector<float> filterLines(std::vector<float> rhos, float threshold = 10.0f) {
        std::sort(rhos.begin(), rhos.end());
        std::vector<float> filtered;
        for (float rho : rhos) {
                if (filtered.empty() || std::abs(rho - filtered.back()) > threshold) {
                        filtered.push_back(rho);
                }
        }
    return filtered;
}

bool containsPieceContour(const cv::Mat& roi){
	cv::Mat gray, binary;
	cv::cvtColor(roi, gray, cv::COLOR_BGR2GRAY);
    	cv::threshold(gray, binary, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);

	std::vector<std::vector<cv::Point>> contours;
    	cv::findContours(binary, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	for (const auto& contour : contours) {
        double area = cv::contourArea(contour);
        if (area > 100.0) {
            return true;
        }
    }
    return false;
	
}


int main() {
    
    cv::Mat frame = cv::imread("./chess_images/chessboard_full.png");
    cv::Mat refImage = cv::imread("./chess_images/chessboard_empty.png");

    if (frame.empty()) {
        std::cerr << "Could not load image." << std::endl;
        return -1;
    }

   	cv::resize(frame, frame, cv::Size(600, 600));
	cv::resize(refImage, refImage, cv::Size(600, 600));

   	cv::Mat gray, blurred, edges;
    	cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    	cv::GaussianBlur(gray, blurred, cv::Size(5, 5), 0);
    	cv::Canny(blurred, edges, 50, 150);

	std::vector<cv::Vec4i> lines;
    	cv::HoughLinesP(edges, lines, 1, CV_PI / 180, 40, 60,30);

	std::vector<float> horizontalRhos, verticalRhos;

	for (const auto& l :lines) {
       /* float rho = l[0], theta =l[1];
	if ((theta < CV_PI * 0.25) || (theta > CV_PI * 0.75)) {
            verticalRhos.push_back(rho);
        } else {
            horizontalRhos.push_back(rho);
        }
	*/

	int x1 = l[0], y1 =l[1], x2 =l[2], y2 =l[3];
	double angel =std::atan2(y2 - y1, x2 -x1)* 180 /CV_PI;
	
	if(std::abs(angel) <10){
		horizontalRhos.push_back((y1+y2)/2.0f);
	}else if(std::abs(angel -90) < 10 || std::abs(angel +90) <10) {
		verticalRhos.push_back((x1+x2)/2.0f);
		}

	}

/*
	horizontalRhos = filterLines(horizontalRhos, 25.0f);
    	verticalRhos = filterLines(verticalRhos, 25.0f);

	std::sort(horizontalRhos.begin(), horizontalRhos.end());
	std::sort(verticalRhos.begin(), verticalRhos.end());


	if (horizontalRhos.size() > 9) {
		horizontalRhos = std::vector<float>(horizontalRhos.begin(), horizontalRhos.begin() +9);
	}
	if (verticalRhos.size() > 9) {
		verticalRhos = std::vector<float>(verticalRhos.begin(), verticalRhos.begin() +9);
	}

	if(horizontalRhos.size() <9 || verticalRhos.size()<9){
		std::cerr << "Not enough grid lines detected to form an 8x8 board." << std::endl;
        	return -1;
	}

*/


std::vector<std::vector<cv::Point>> contours;
cv::findContours(edges, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

double maxArea = 0;
std::vector<cv::Point> maxContour;

for (const auto& contour : contours) {
    double area = cv::contourArea(contour);
    if (area > maxArea) {
        maxArea = area;
        maxContour = contour;
    }
}

cv::Rect boardRect = cv::boundingRect(maxContour);
cv::rectangle(frame, boardRect, cv::Scalar(0, 255, 0), 2);


int cellW = boardRect.width / 8;
int cellH = boardRect.height / 8;

	std::map<std::string, cv::Rect> boardSquares;
    	std::string cols = "ABCDEFGH";


	for(int r = 0; r < 8; ++r){
		for(int c = 0; c < 8; ++c){
			/*
			int x = std::min((int)verticalRhos[c], (int)verticalRhos[c + 1]);
            		int y = std::min((int)horizontalRhos[r], (int)horizontalRhos[r + 1]);
            		int w = std::abs((int)verticalRhos[c + 1] - (int)verticalRhos[c]);
            		int h = std::abs((int)horizontalRhos[r + 1] - (int)horizontalRhos[r]);

			std::string label = cols[c] + std::to_string(8 - r);
			boardSquares[label] = cv::Rect(x, y, w, h);

			cv::rectangle(frame, boardSquares[label], cv::Scalar(0, 255, 0), 1);
            		cv::putText(frame, label, cv::Point(x + 5, y + 15), cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(255, 0, 0), 1);
	*/
	
	int x = boardRect.x + c * cellW;
        int y = boardRect.y + r * cellH;
        cv::Rect cell(x, y, cellW, cellH);
        std::string label = cols[c] + std::to_string(8 - r);
        boardSquares[label] = cell;


	cv::rectangle(frame, cell, cv::Scalar(0, 255, 0), 1);
        cv::putText(frame, label, cv::Point(x + 5, y + 15), cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(255, 0, 0), 1);
	
	
		}
	}

	for (const auto& [label, rect] : boardSquares) {
	
		std::cout<<"Processing squre: "<<label<< std::endl;	
		if(rect.x >= 0 && rect.y >= 0 && rect.x + rect.width <= frame.cols && rect.y +rect.height <= frame.rows){
			cv::Mat liveROI = frame(rect);
                	cv::Mat refROI = refImage(rect);

			cv::Mat liveGray, refGray, diff;
                	cv::cvtColor(liveROI, liveGray, cv::COLOR_BGR2GRAY);
                	cv::cvtColor(refROI, refGray, cv::COLOR_BGR2GRAY);
                	cv::absdiff(liveGray, refGray, diff);

			double score = cv::sum(diff)[0] / (diff.rows * diff.cols);

			
			if (score > 15.0) {
                        std::cout << label << ": Piece detected" << std::endl;
                        cv::rectangle(frame, rect, cv::Scalar(0, 0, 255), 2); // Red if piece
                	
			cv::Mat gray, binary;
			cv::cvtColor(liveROI, gray, cv::COLOR_BGR2GRAY);
			//cv::threshold(gray, binary, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);

			
			cv::adaptiveThreshold(gray, binary, 255,cv::ADAPTIVE_THRESH_GAUSSIAN_C,cv::THRESH_BINARY_INV, 11, 2);

			std::vector<std::vector<cv::Point>> pieceContours;
			cv::findContours(binary, pieceContours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

			// Skip if no contours found
			if (pieceContours.empty()){
				std::cout << label << ": Could not identify shape (no contour)\n";
    				continue;
			} 

			// Find the largest contour
			int maxIndex = -1;
			double maxArea = 0;
			for (size_t i = 0; i < pieceContours.size(); ++i) {
    				double area = cv::contourArea(pieceContours[i]);
				cv::Rect bb = cv::boundingRect(pieceContours[i]);

				if (area > 200.0 && bb.x > 5 && bb.x + bb.width < rect.width - 5 && bb.y > 5 && bb.y + bb.height < rect.height - 5) {
    					if (area > maxArea) {
        					maxArea = area;
        					maxIndex = i;
    					}
				}

			}

			if(maxIndex != -1){
				cv::Moments m = cv::moments(pieceContours[maxIndex]);
                        	double hu[7];
                        	cv::HuMoments(m, hu);
                        	std::cout << label << ": Hu Moments = [ ";
                        	for (int i = 0; i < 7; ++i)
                                	std::cout << -std::copysign(1.0, hu[i]) * std::log10(std::abs(hu[i]) + 1e-10) << " ";
                                	std::cout << "]" << std::endl;

                //      cv::imshow(label + " Binary", binary);
                //      cv::waitKey(1);

                        	cv::drawContours(liveROI, pieceContours, maxIndex, cv::Scalar(255, 0, 0), 1);
                        //	cv::imshow("Detected Contour: " + label, liveROI);
			//	cv::waitKey(1);

				}else{
				std::cout << label << ": No valid contour for Hu moments\n";
				}
			

		}
	}




   	cv::imshow("Label Chessboard", frame);
    	cv::waitKey(0);
    	return 0;

}

}


