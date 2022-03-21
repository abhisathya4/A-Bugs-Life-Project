#include "../includes/AntProc.h"

#include <memory>

using namespace sqlite_orm;
using namespace std;
using json = nlohmann::json;

////////////////////////////////////////////////////////////

const int max_value_H = 360/2;
const int max_value = 255;
const String window_capture_name = "Video Capture";
const String window_detection_name = "Object Detection";

// Color adjustments (low_v = 120, high_v = 200)
int low_H = 0, low_S = 0, low_V = 120;
int high_H = max_value_H, high_S = max_value, high_V = 200;

/*
 * ContourFinder is the "main" function. It initiates image processing,
 * contour finding, database processing and excel outputting.
 */
void AntProc::ContourFinder() {

    VideoCapture cap(path_);
    Mat img;

//    HSVTrackBars();
    vector<pair<bool,Point2f>> touched_vector;

    while (cap.isOpened()) {
        cap >> img;

        if (img.empty()) break;
        Mat imgCrop = FrameCrop(img);
        Mat imgProcessed = FrameProc(imgCrop);
        circle(imgCrop, Point(point_.x,point_.y), 10, Scalar(0,0,255), -1);

        vector<vector<Point>> contours;
        FindContours(imgProcessed, contours);
        DrawContours(imgCrop, contours);

        /// NEED TO STORE POINT AS A FILTER IN THE DATABASE ///
        Point2f point(point_.x, point_.y); // this is the filter

        TouchedVectorPopulator(contours, touched_vector, point);

        /// ABSTRACT THIS OUT SO WE CAN ADD FLAG FROM MAIN ///
        imshow(window_capture_name, imgCrop);
//        imshow(window_detection_name, imgProcessed);

        char key = (char) waitKey(1);
        if (key == 'q' || key == 27) break;
    }

    ToDatabase(touched_vector, path_);
    ToCSV(touched_vector);
}

/*
 * FrameProc processes the image for edge detection before returning a
 * dilated copy of edges detected.
 */
Mat AntProc::FrameProc(Mat frame) {
    Mat frameHSV = Cvt2HSV(frame);
    Mat frameThreshold = Cvt2InRange(frameHSV);

    Mat frameBlur, frameGray, frameCanny, frameDil, frameErode;

    //// cvtColor(FrameCrop, frameGray, COLOR_BGR2GRAY);
    GaussianBlur(frameThreshold, frameBlur, Size(3,3), 3, 0);
    Canny(frameBlur, frameCanny, 100, 200);

    Mat kernel = getStructuringElement(MORPH_RECT, Size(9, 9));
    dilate(frameCanny, frameDil, kernel);
    erode(frameDil, frameErode, kernel);

    return frameErode;
}

/*
 * FrameCrop crops the image, at this point the dimensions must be input manually.
 */
Mat AntProc::FrameCrop(Mat frame) {
    Mat frameCrop;
    Rect roi(800,frame.rows/2,frame.cols/2 -100,frame.rows/2);
    frameCrop = frame(roi);
    return frameCrop;
}
/*
 * Cvt2HSV.
 */
Mat AntProc::Cvt2HSV(Mat frame) {
    Mat frameHSV;
    cvtColor(frame, frameHSV, COLOR_BGR2HSV);
    return frameHSV;
}
/*
 * Cvt2InRange ...
 */
Mat AntProc::Cvt2InRange(Mat frame) {
    Mat frameThreshold;
    inRange(frame, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), frameThreshold);
    return frameThreshold;
}


void AntProc::FindContours(Mat imgProcessed, vector<vector<Point>>& contours) {
    vector<Vec4i> hierarchy;
    findContours(imgProcessed, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_TC89_KCOS);
}
void AntProc::DrawContours(Mat img, vector<vector<Point>>& contours) {
    for (int i = 0; i < contours.size(); ++i) {
        vector<vector<Point>> conPoly(contours.size());
        vector<Rect> boundRect(contours.size());

        int area = contourArea(contours[i]);
        if (area > this->kMinArea) {
            float peri = arcLength(contours[i], true);
            approxPolyDP(contours[i], conPoly[i], 0.02*peri, true);
            drawContours(img, conPoly, i, Scalar(255, 0, 255), 2);
        }
    }
}
bool AntProc::PointPolygonTest(vector<Point> contour, Point point) {
    int in_contours = pointPolygonTest(contour, point, false);
    if (in_contours >= 0) {
        return true;
    }
    return false;
}

void AntProc::TouchedVectorPopulator(vector<vector<Point>>& contours,
                                     vector<pair<bool,Point2f>>& touched_vector,
                                     Point2f point) {
    bool touched_bool = false;
    for (auto contour : contours) {
        if (contourArea(contour) > this->kMinArea) {
            if (PointPolygonTest(contour, point)) {
                touched_bool = true;
                break;
            }
        }
    }

    touched_vector.push_back(pair(touched_bool, point));
}

void AntProc::ToCSV(vector<pair<bool,Point2f>>& touched_vector) {
    std::vector<Interaction> interactions;

    bool continuous = false;
    size_t start_frame = 0;
    size_t end_frame = 0;
    size_t idx = 1;
    size_t id = 1;

    for (auto frame : touched_vector) {
        if (frame.first && !continuous) {
            continuous = true;
            start_frame = idx;
        } else if (!frame.first && continuous) {
            continuous = false;
            end_frame = idx; // minus 1
            Interaction interaction(id, start_frame, end_frame);
            interactions.push_back(interaction);
            ++id;
        }
        ++idx;
    }

    std::ofstream aFile(csv_name_);
    int vSize = interactions.size();
    aFile << "ID,Start_Frame,End_Frame,Start_Time,End_Time,Frame_Duration,Time_Duration" << endl;
    for (size_t i = 0; i < vSize; ++i) {
        aFile << interactions[i].id << ","
              << interactions[i].start_frame << ","
              << interactions[i].end_frame << ","
              << interactions[i].start_frame/fps_ << ","
              << interactions[i].end_frame/fps_ << ","
              << interactions[i].duration << ","
              << interactions[i].duration/fps_ << "," << std::endl;

    }
}
void AntProc::ToDatabase(const vector<pair<bool,Point2f>>& touched_vector, const string& path) {
    auto storage = make_storage(
            "AntDatabase.db",
            make_table("filter",
                       make_column("id", &Filter::id, autoincrement(), primary_key()),
                       make_column("name", &Filter::name),
                       make_column("x", &Filter::x),
                       make_column("y", &Filter::y)),
            make_table("interaction",
                       make_column("id", &Interaction::id, autoincrement(), primary_key()), /// video name, datetime
                       make_column("start", &Interaction::start_frame),
                       make_column("end", &Interaction::end_frame),
                       make_column("duration", &Interaction::duration),
                       make_column("filter_id", &Interaction::filter_id),
                       foreign_key(&Interaction::filter_id).references(&Filter::id))
    );
    storage.sync_schema();

    bool continuous = false;
    size_t start_frame = 0;
    size_t end_frame = 0;
    size_t idx = 1;
    size_t id = 1;

    for (auto frame : touched_vector) {
        if (frame.first && !continuous) {
            continuous = true;
            start_frame = idx;
        }
        if (!frame.first && continuous) {
            end_frame = idx - 1;
            int duration = start_frame - end_frame;
            continuous = false;
            auto filterwithID = storage.get_all<Filter>(where(frame.second.x == c(&Filter::x) && frame.second.y == c(&Filter::y)));
            if (filterwithID.empty()) {
                Filter f{-1, "", frame.second.x, frame.second.y};
                storage.insert(f);
            }
            Filter filter = filterwithID.at(0);
            auto filterID = std::make_unique<int>(filter.id);
            Interaction interaction;
            interaction.id = -1;
            interaction.start_frame = start_frame;
            interaction.end_frame = end_frame;
            interaction.duration = end_frame - start_frame;
            interaction.filter_id = std::move(filterID);
            auto insertedId = storage.insert(interaction);
            interaction.id = insertedId;
        }
        ++idx;
    }
}
string AntProc::GetName(string path) {
    reverse(path.begin(), path.end());
    bool ext = false;
    string file_name;
    for (auto c : path) {
        if (c == '.') ext = true;
        if (ext) {
            if (c == '/' || c == ' ') break;
            file_name += c;
        }
    }
    reverse(file_name.begin(), file_name.end());
//    file_name = file_name + ".db";
    return file_name;
}

static void on_low_H_thresh_trackbar(int, void *) {
    low_H = min(high_H-1, low_H);
    setTrackbarPos("Low H", window_detection_name, low_H);
}
static void on_high_H_thresh_trackbar(int, void *) {
    high_H = max(high_H, low_H+1);
    setTrackbarPos("High H", window_detection_name, high_H);
}
static void on_low_S_thresh_trackbar(int, void *) {
    low_S = min(high_S-1, low_S);
    setTrackbarPos("Low S", window_detection_name, low_S);
}
static void on_high_S_thresh_trackbar(int, void *) {
    high_S = max(high_S, low_S+1);
    setTrackbarPos("High S", window_detection_name, high_S);
}
static void on_low_V_thresh_trackbar(int, void *) {
    low_V = min(high_V-1, low_V);
    setTrackbarPos("Low V", window_detection_name, low_V);
}
static void on_high_V_thresh_trackbar(int, void *) {
    high_V = max(high_V, low_V+1);
    setTrackbarPos("High V", window_detection_name, high_V);
}

void AntProc::HSVTrackBars() {
    namedWindow(window_capture_name);
    namedWindow(window_detection_name);

    createTrackbar("Low H", window_detection_name, &low_H, max_value_H, on_low_H_thresh_trackbar);
    createTrackbar("High H", window_detection_name, &high_H, max_value_H, on_high_H_thresh_trackbar);
    createTrackbar("Low S", window_detection_name, &low_S, max_value, on_low_S_thresh_trackbar);
    createTrackbar("High S", window_detection_name, &high_S, max_value, on_high_S_thresh_trackbar);
    createTrackbar("Low V", window_detection_name, &low_V, max_value, on_low_V_thresh_trackbar);
    createTrackbar("High V", window_detection_name, &high_V, max_value, on_high_V_thresh_trackbar);
}
