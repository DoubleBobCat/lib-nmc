#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <map>

using namespace cv;
using namespace std;

// Structure to hold color and corresponding dBZ range
struct ColorRange {
    Vec3b color;
    string range_name;
    string filename;
};

// Function to convert hex color to BGR format
Vec3b hexToBGR(const string& hex) {
    // Remove # if present
    string cleanHex = hex;
    if (cleanHex[0] == '#') {
        cleanHex = cleanHex.substr(1);
    }
    
    // Convert hex to RGB, then to BGR for OpenCV
    int r = stoi(cleanHex.substr(0, 2), nullptr, 16);
    int g = stoi(cleanHex.substr(2, 2), nullptr, 16);
    int b = stoi(cleanHex.substr(4, 2), nullptr, 16);
    
    return Vec3b(b, g, r); // BGR format for OpenCV
}

// Function to calculate color distance (Euclidean distance in RGB space)
double colorDistance(const Vec3b& c1, const Vec3b& c2) {
    return sqrt(pow(c1[0] - c2[0], 2) + pow(c1[1] - c2[1], 2) + pow(c1[2] - c2[2], 2));
}

// Function to create mask for specific color with tolerance
Mat createColorMask(const Mat& image, const Vec3b& targetColor, double tolerance = 10.0) {
    Mat mask = Mat::zeros(image.size(), CV_8UC1);
    
    for (int y = 0; y < image.rows; y++) {
        for (int x = 0; x < image.cols; x++) {
            Vec3b pixel = image.at<Vec3b>(y, x);
            if (colorDistance(pixel, targetColor) <= tolerance) {
                mask.at<uchar>(y, x) = 255;
            }
        }
    }
    
    return mask;
}

int main() {
    // Define the 15 dBZ gradient colors and their ranges
    vector<ColorRange> gradientColors = {
        {hexToBGR("#AD8FF0"), "75-70dBZ", "75-70dBZ.png"},
        {hexToBGR("#9600B4"), "70-65dBZ", "70-65dBZ.png"},
        {hexToBGR("#FF00F0"), "65-60dBZ", "65-60dBZ.png"},
        {hexToBGR("#C00000"), "60-55dBZ", "60-55dBZ.png"},
        {hexToBGR("#D60000"), "55-50dBZ", "55-50dBZ.png"},
        {hexToBGR("#FF0000"), "50-45dBZ", "50-45dBZ.png"},
        {hexToBGR("#FF9000"), "45-40dBZ", "45-40dBZ.png"},
        {hexToBGR("#E7C000"), "40-35dBZ", "40-35dBZ.png"},
        {hexToBGR("#FFFF00"), "35-30dBZ", "35-30dBZ.png"},
        {hexToBGR("#019000"), "30-25dBZ", "30-25dBZ.png"},
        {hexToBGR("#00D800"), "25-20dBZ", "25-20dBZ.png"},
        {hexToBGR("#6DFA3D"), "20-15dBZ", "20-15dBZ.png"},
        {hexToBGR("#64E7EB"), "15-10dBZ", "15-10dBZ.png"},
        {hexToBGR("#419DF1"), "10-5dBZ", "10-5dBZ.png"},
        {hexToBGR("#0000EF"), "5-0dBZ", "5-0dBZ.png"}
    };
    
    // Load the original image
    Mat originalImage = imread("a.png", IMREAD_COLOR);
    if (originalImage.empty()) {
        cerr << "Error: Could not load image a.png" << endl;
        return -1;
    }
    
    cout << "Original image size: " << originalImage.cols << "x" << originalImage.rows << endl;
    
    // Check if image dimensions are sufficient for cropping
    if (originalImage.cols < 730 || originalImage.rows < 730) {
        cerr << "Error: Image is too small for 730x730 crop" << endl;
        return -1;
    }
    
    // Crop 730x730 from top-left corner (anchor point)
    Rect cropRect(0, 0, 730, 730);
    Mat croppedImage = originalImage(cropRect);
    
    cout << "Cropped image size: " << croppedImage.cols << "x" << croppedImage.rows << endl;
    
    // Save the cropped image for reference
    imwrite("cropped_image.png", croppedImage);
    cout << "Saved cropped image as cropped_image.png" << endl;
    
    // Process each gradient color
    for (const auto& colorRange : gradientColors) {
        cout << "Processing " << colorRange.range_name << "..." << endl;
        
        // Create mask for current color with tolerance
        Mat colorMask = createColorMask(croppedImage, colorRange.color, 15.0);
        
        // Create output image (black background with colored pixels)
        Mat outputImage = Mat::zeros(croppedImage.size(), CV_8UC3);
        croppedImage.copyTo(outputImage, colorMask);
        
        // Save the extracted gradient channel
        if (imwrite(colorRange.filename, outputImage)) {
            cout << "Successfully saved " << colorRange.filename << endl;
        } else {
            cerr << "Error saving " << colorRange.filename << endl;
        }
        
        // Optional: Save mask as well (for debugging purposes)
        string maskFilename = colorRange.range_name + "_mask.png";
        imwrite(maskFilename, colorMask);
        
        // Count non-zero pixels to show statistics
        int pixelCount = countNonZero(colorMask);
        cout << "Found " << pixelCount << " pixels for " << colorRange.range_name << endl;
    }
    
    cout << "\nProcessing completed!" << endl;
    cout << "All gradient channels have been extracted and saved." << endl;
    
    return 0;
}