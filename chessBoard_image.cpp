#include <opencv2/opencv.hpp>
#include <iostream>
#include <filesystem>


namespace fs = std::filesystem;

int main(){

std::string folderPath = "./chess_images/";
std::vector<std::string> imageFiles;

for (const auto& entry : fs::directory_iterator(folderPath)) {
    if (entry.path().extension() == ".jpg" || entry.path().extension() == ".png") {
        imageFiles.push_back(entry.path().string());
    }
}

cv::Size boardSize(7, 7);
std::vector<cv::Point2f> corners;


for (const auto& imagePath : imageFiles) {
    cv::Mat frame = cv::imread(imagePath);
    if (frame.empty()) {
        std::cerr << "Failed to load: " << imagePath << std::endl;
        continue;
    }

	cv::Mat gray;
	cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

	cv::imshow("Raw Grayscale",gray);

	cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
    	clahe->setClipLimit(4.0);
    	clahe->apply(gray, gray);

	cv::imshow("chahe aplied", gray);

	bool found = cv::findChessboardCorners(gray, boardSize, corners,
			cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE);

	std::cout << (found ? "Found" : "Not Found") << std::endl;



	if(found){
		cv::cornerSubPix(gray, corners, cv::Size(11,11), cv::Size(-1,-1),
				cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30,0.1));
					
					cv::drawChessboardCorners(frame, boardSize, corners, found);
					
	}

	cv::imshow("Chessboard Detection", frame);

	if(cv::waitKey(0) == 27) break;
}

	
	cv::destroyAllWindows();
	return 0;

}
