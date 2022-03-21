/// IMAGE ///

//    string path = "/Users/granthale/Documents/ABugsLifeV2/resources/AntsImageTest2.png";
//    Mat img = imread(path);

//    Mat imgCrop, imgGray, imgBlur, imgCanny, imgDil, imgErode;
//
//    // cout << img.cols << img.rows << endl;
//    Rect roi(img.cols/3,img.rows/3,img.cols/2,img.rows/2);
//    imgCrop = img(roi);
//
//    cvtColor(imgCrop, imgGray, COLOR_BGR2GRAY);
//    GaussianBlur(imgCrop, imgBlur, Size(3,3), 3, 0);
//    Canny(imgBlur, imgCanny, 100, 200);
//
//    Mat kernel = getStructuringElement(MORPH_RECT, Size(15, 15));
//    dilate(imgCanny, imgDil, kernel);
//    erode(imgDil, imgErode, kernel);
//
//    GetContours(imgDil, imgCrop);
//
//    // imshow("Image", img);
// imshow("Cropped", imgCrop);
//    // imshow("Gray", imgGray);
//    // imshow("Blurred", imgBlur);
//    // imshow("Canny", imgCanny);
//    // imshow("Dilation", imgDil);
//    // imshow("Erosion", imgErode);
//    waitKey(0);

//////// VIDEO ////////

//    /// Standard Contours ///
//    VideoCapture cap(path);
//    Mat frame;
//
//    while (true) {
//        Mat frameCrop, frameGray, frameBlur, frameCanny, frameDil, frameErode;
//        cap >> frame;
//        if(frame.empty()) break;
//
//        /* Crops Image (Manual at this point) */
//        Rect roi(800,frame.rows/2,frame.cols/2 -100,frame.rows/2);
//        frameCrop = frame(roi);
//
//        cvtColor(frameCrop, frameGray, COLOR_BGR2GRAY);
//        GaussianBlur(frameCrop, frameBlur, Size(3, 3), 3, 0);
//        Canny(frameBlur, frameCanny, 100, 200);
//
//        Mat kernel = getStructuringElement(MORPH_RECT, Size(9, 9));
//        dilate(frameCanny, frameDil, kernel);
//        erode(frameDil, frameErode, kernel);
//
//        GetContours(frameDil, frameCrop);
//
//        /* Shows Processed Image */
//        imshow("Processed Image", frameCrop);
//        waitKey(1);
//    }

