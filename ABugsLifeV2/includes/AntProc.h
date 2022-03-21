#ifndef ABUGSLIFEV2_ANTPROC_H
#define ABUGSLIFEV2_ANTPROC_H

#include "../includes/data_models.hpp"
#include "../includes/sqlite_orm.hpp"
#include "../includes/json.hpp"

#include <string>
#include <iostream>
#include <fstream>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

////////////////////////////////////////////////////////////

class AntProc {
public:
    AntProc(string path, float x, float y) : path_(path), point_(Point2f(x,y)) {
        VideoCapture cap(path);
        this->fps_ = cap.get(CAP_PROP_FPS);

        database_name_ = "AntDatabase.db";
        csv_name_ = GetName(path) + "csv";
    }
    void ContourFinder();

    void SetPoint(Point2f point) { point_ = point; }

private:
    string path_;
    float fps_;
    Point2f point_;
    static constexpr int kMinArea = 2000;

    string database_name_;
    string csv_name_;

    Mat FrameProc(Mat frame);
    Mat FrameCrop(Mat frame);
    Mat Cvt2HSV(Mat frame);
    Mat Cvt2InRange(Mat frame);

    void FindContours(Mat imgProcessed, vector<vector<Point>>& contours);
    void DrawContours(Mat img, vector<vector<Point>>& contours);
    bool PointPolygonTest(vector<Point> contour, Point point);
    void TouchedVectorPopulator(vector<vector<Point>>& contours,
                                vector<pair<bool,Point2f>>& touched_vector,
                                Point2f point);

    void HSVTrackBars();

    void ToCSV(vector<pair<bool,Point2f>>& touched_vector);
    void ToDatabase(const vector<pair<bool,Point2f>>& frames, const string& path);
    string GetName(string path);
};

#endif //ABUGSLIFEV2_ANTPROC_H
