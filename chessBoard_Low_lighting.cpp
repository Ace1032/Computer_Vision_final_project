#include <opencv2/opencv.hpp>
#include <iostream>

int main(){
cv::VideoCapture cap(0);
if (!cap.isOpened()) {
        std::cerr << "Error: Cannot open camera." << std::endl;
        return -1;
    }



cv::Size boardSize(7, 7);
std::vector<cv::Point2f> corners;


 int frameCount = 0;

while(true){
	cv::Mat frame, gray;
	cap >> frame;
	if(frame.empty()) break;

	cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
	//Historam Equalization
        cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
        clahe->setClipLimit(4.0);
        clahe->apply(gray, gray);

	

	if(++frameCount % 5 == 0){
	

	bool found = cv::findChessboardCorners(gray, boardSize, corners,
			cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE);

	std::cout << (found ? "Found" : "Not Found") << std::endl;

	if(found){
		cv::cornerSubPix(gray, corners, cv::Size(11,11), cv::Size(-1,-1),
				cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30,0.1));
					
					cv::drawChessboardCorners(frame, boardSize, corners, found);
					
	}

	}//every5freames

	cv::imshow("Chessboard Detection", frame);

	if(cv::waitKey(30) == 27) break;
}

	cap.release();
	cv::destroyAllWindows();
	return 0;

}
