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
    	cv::HoughLinesP(edges, lines, 1, CV_PI / 180, 80, 30,10);

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


	horizontalRhos = filterLines(horizontalRhos);
    	verticalRhos = filterLines(verticalRhos);

	std::sort(horizontalRhos.begin(), horizontalRhos.end());
	std::sort(verticalRhos.begin(), verticalRhos.end());


	if (horizontalRhos.size() > 9) {
		horizontalRhos = std::vector<float>(horizontalRhos.begin() + 1, horizontalRhos.end() - 1);
	}
	if (verticalRhos.size() > 9) {
		verticalRhos = std::vector<float>(verticalRhos.begin() + 1, verticalRhos.end() - 1);
	}

	if(horizontalRhos.size() <9 || verticalRhos.size()<9){
		std::cerr << "Not enough grid lines detected to form an 8x8 board." << std::endl;
        	return -1;
	}


	std::map<std::string, cv::Rect> boardSquares;
    	std::string cols = "ABCDEFGH";


	for(int r = 0; r < 8; ++r){
		for(int c = 0; c < 8; ++c){
			int x = std::min((int)verticalRhos[c], (int)verticalRhos[c + 1]);
            		int y = std::min((int)horizontalRhos[r], (int)horizontalRhos[r + 1]);
            		int w = std::abs((int)verticalRhos[c + 1] - (int)verticalRhos[c]);
            		int h = std::abs((int)horizontalRhos[r + 1] - (int)horizontalRhos[r]);

			std::string label = cols[c] + std::to_string(8 - r);
			boardSquares[label] = cv::Rect(x, y, w, h);

			cv::rectangle(frame, boardSquares[label], cv::Scalar(0, 255, 0), 1);
            		cv::putText(frame, label, cv::Point(x + 5, y + 15), cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(255, 0, 0), 1);
		}
	}

	for (const auto& [label, rect] : boardSquares) {
		
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
                	}

		}else {
			std::cerr << "Skipping invalid rect: " << label << std::endl;
		}
	}




   	cv::imshow("Label Chessboard", frame);
    	cv::waitKey(0);
    	return 0;

}




