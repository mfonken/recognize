//
//  Written and translated into C++ by Matthew Fonken
//  For LARC 2014-2015
//
//  !IMPORTANT NOTES!
//  1) Refer to README.txt to install opencv (MAC OSX for now), setup, and compile program
//  2) Control program from "status.txt", commands are - idle, read, and end
//  3) Get characters found from "characters.txt"
//  4) Getting pixel color is ***.at<cv::Vec3b>(row, col) or ***.at<cv::Vec3b>(Point(x,y))
//  5) "at" gives BGR color! Not RGB... (Easiest as Vec3b array [opencv struct])
//
//

#include <fstream>
#include <iostream>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <string>

using namespace std;
using namespace cv;

//***********************FUNCTIONS****************************//
float getDiff(Mat& video, int& x, int& y, int& r, int& g, int& b);  //"Color distance", literal (r-ro)^2+(g-go)^2+(b-bo)^2 (used /=1000 instead of sqrt to save proc time)
float scan(Mat& video, char dir, int& x, int& y);                   //Scans for continuation of the object in the given direction - output is border of object continuity
char checkChar(Mat& obj);                                           //Check to see if object found is a ASCII character - output is first match over given % threshold
char checkSimilar(Mat& obj, std::string checkList);                 //If one of many similar characters, check against similar characters - output is best match

int main( int argc, char** argv) {
//****************SETUP AND CONFIGURATION*********************//
    cout << "Begin Recognize for Debian" << endl;
    fstream statusFile ("status.txt");          //setup I/O files
    if(!statusFile) {
        statusFile.open("status.txt",fstream::out);
        statusFile.close();
        statusFile.open("status.txt",fstream::in);
    }
    ofstream charactersFile;
    charactersFile.open("characters.txt", std::ofstream::out | std::ofstream::trunc);   //clear and open "characters.txt"
    
    VideoCapture cap(0);                        //Initalize camera
    if(!cap.isOpened())
        return -1;
    
    Mat capture, object(100, 100,  CV_8UC3);    //create image containers
    
    int window_width = 1280,
        window_height = 720;
    
    bool mode = true;                           //initialize status variables
    bool flag = false;
    char last_char = '~', c;                    //characters last, current, ('~' acts as null)
    std::string chars;                          //and total
    
    std::string status;                               //program status pointer
    
    int avg_r = 0, avg_g = 0, avg_b = 0;        //variables for contrast adaption
    int offset = 100;
    
    int samp = 200,                             //sample attributes - cropped immediately after contrast filtering to B&W
        samp_window_width = 400,                //sample frame is centered
        samp_window_height = 400,
        samp_interval = 10;
    
    int object_width_min =  10,                 //object attributes - frame of expected object to be found
        object_width_max =  500,                //only max and min of width and height
        object_height_min =  75,
        object_height_max =  500;
    
    
    int diff_thresh = 20;                      //threshold attribute - applies to contrast filtering, used to check returns form 'color distance' function "getDiff"
    
//**********************MAIN LOOP***************************//
    for(;;) {
        std::getline(statusFile,status);
        if(status.compare("idle") == 0) {
            if(!flag) {                         //Runs once when status changes to "idle"
                charactersFile.close();
                charactersFile.open("characters.txt", std::ofstream::out | std::ofstream::trunc);
                charactersFile << "inactive" << endl;
                flag = true;
            }                                   //
        }
        else if (status.compare("read") == 0) {
            if(flag) {                          //Runs once when status changes to "read"
                charactersFile.close();         //Close, clear, and reopen "characters.txt"
                charactersFile.open("characters.txt", std::ofstream::out | std::ofstream::trunc);
                flag = false;
                cap >> capture;                 //Store video stream to capture matrix (happens once)
                Mat video(capture, Rect((window_width - samp_window_width)/2, (window_height - samp_window_height)/2, samp_window_width, samp_window_height)); //Crop to sample
                cout << video.size().width << " | " << video.size().height << endl; //...and display size of sample frame (happens once)
            }
        
            cap >> capture; //Store video stream to capture matrix and resize to centered sample frame, stored in "video" Mat
            Mat video(capture, Rect((window_width - samp_window_width)/2, (window_height - samp_window_height)/2, samp_window_width, samp_window_height));
            
            avg_r = 0;                                          //Average variable for each color
            avg_g = 0;
            avg_b = 0;
            for (int i = offset; i < samp+offset; i++) {        //Samples on a diagonal from top-left corner plus offset down size of "samp"
                Vec3b tmp_color = capture.at<cv::Vec3b>(i,i);   //Add up each color channel...
                avg_r += (int)tmp_color[2];
                avg_g += (int)tmp_color[1];
                avg_b += (int)tmp_color[0];
            }
            avg_r /= samp;                                      //...and divide by length to get average
            avg_g /= samp;
            avg_b /= samp;
            

            
            //cout << " " << x << " | " << y << " | " << window_width << endl;
            
            
            int tmp_x_min = 0,                                  //Reset object location variables for new scan
                tmp_x_max = 1,
                tmp_y_min = 0,
                tmp_y_max = 1;
            
            int x = 0,                                          //Reset to first pixel coordinate on top left of sample frame
                y = 0;
            while (y < video.size().height) {                   //Dual loop scans row from top to bottom, setting pixels to BGR to B&W based off distance from avgs
                while (x < video.size().width) {                //white if similar, black if unique
                    video.at<cv::Vec3b>(y,x) = (getDiff(video, x, y, avg_r, avg_g, avg_b) > diff_thresh) ? Vec3b(0,0,0):Vec3b(255,255,255);
                    x++;
                }
                x = 0;
                y++;
            }   //(Yes, for loops would have been easier)
            
            x = 0;                                              //Reset to first pixel again
            y = 0;
            
            char c = '~';                                       //Reset character variable to '~' which acts as null
            //cout << "video (default) = " << endl << video << endl << endl;
            //imshow("Video", video);
            while (y < video.size().height) {                   //Scan again until...
                while (x < video.size().width) {
                    
                    tmp_x_min = 0;
                    tmp_x_max = 1;
                    tmp_y_min = 0;
                    tmp_y_max = 1;
                    //imshow("Recognize for Debian", video);
                    if ((int)video.at<cv::Vec3b>(Point(x,y))[0] == 0) { //...a black pixel is found
                        circle(video, Point(x,y), 2, Scalar(0,0,255), 3, 8, 0);
                        //video.at<cv::Vec3b>(Point(x,y)) = Vec3b(255,0,0);
                        
                        //cout << "!" << endl;
                        
                        tmp_x_min = (int)scan(video, 'x', x, y);        //Possible object, scan all directions from this point
                        tmp_x_max = (int)scan(video, 'X', x, y);        //for continuity to find full object
                        tmp_y_min = (int)scan(video, 'y', x, y);
                        tmp_y_max = (int)scan(video, 'Y', x, y);
                         
                        //cout << "Coordinate (" << x << ", " << y << ") | x " << tmp_x_min << " | X " << tmp_x_max << " | y " << tmp_y_min << " | Y " << tmp_y_max << endl;
                        
                        int object_width = tmp_x_max-tmp_x_min;         //Generate width and height of the object
                        int object_height = tmp_y_max-tmp_y_min;
                                                                        //If it fits the defined possible object size...
                        if (object_width > object_width_min && object_width < object_width_max && object_height > object_height_min && object_height < object_height_max) {
                            rectangle(video, Point(tmp_x_min, tmp_y_min), Point(tmp_x_min+object_width, tmp_y_min+object_height), Scalar(0,255,0)); //draw a rectangle over it
                            //cout << "Rectangle made at (" << x-tmp_x_min << ", " << y-tmp_y_min << ")" << endl;
                            imshow("Object Found", object);             //display it in another window
                            Mat tmp (video, Rect(tmp_x_min, tmp_y_min, object_width, object_height));   //crop it out onto a new matrix, first a tmp one...
                            resize(tmp, object, object.size(), INTER_LINEAR);                           //...then resize it linearly to correct size (100x100)
                            c = checkChar(object);                                                      //Finally check it using "checkChar"
                            goto end_loop;                              //If any object is found, program exits scan, would otherwise check every black pixel
                        }
                    }
                    x+=samp_interval;
                }
                x = 0;
                y+=samp_interval;
            }
end_loop:   imshow("Recognize for Debian", video);                      //End of loop, below is where the string of found chars and "character.txt" are managed
            
            if (c != last_char && c != '~') {                           //If a different character is found (and not a '~' local null), c and last_char while be different...
                bool exists = false;                                    //reset temporary flag
                for(int i = 0; i < chars.length(); i++) {               //run through found characters and set flag if this new character has already been found
                    if(chars.at(i) == c) {                              //!!!This is logical for testing, remove if a single character can be appear more than once!!
                        exists = true;
                    }
                }
                if(!exists) {                                           //If the new character is in fact new...
                    chars += c;                                         //...add it to the chars list
                    charactersFile.close();                             //Close, clear, and reopen "characters.txt"...
                    charactersFile.open("characters.txt", std::ofstream::out | std::ofstream::trunc);
                    charactersFile << chars << endl;                    //..and print chars to it
                }
                //cout << endl;
                last_char = c;                                          //Finally, set this new character to be the last_char
            }
        }
        else if (status.compare("end") == 0) {      //Terminate program when status is "end"
            charactersFile.close();
            cout << "Recognize Debian is ending\n";
            return 0;                               //End.
        }
        
        statusFile.close();                         //In order to get new instructions, "status.txt" connection is reset every loop
        statusFile.open("status.txt",fstream::in);
    }
    
term:
    return 0;
}

//*******************getDiff*************************//
float getDiff(Mat &video, int &x, int &y, int &r, int &g, int &b) { //As described above, this is a literal distance formula, given r, g, and b are used as base references
    Vec3b tmp_color = video.at<cv::Vec3b>(Point(x,y));
    //cout << "* " << x << " | " << y << " | " << (int) tmp_color[0] << endl;
    return (pow((int)tmp_color[0]-r,2.0)+pow((int)tmp_color[1]-g,2.0)+pow((int)tmp_color[2]-b,2.0))/1000;   // /1000 is used instead of sqrt to save time
}
//*********************scan**************************//
float scan(Mat &video, char dir, int &x, int &y) {
                                    //Scan attributes, actual max each scan is allowed based on each direction
    int scan_size_x_max = 100,      //x is left
        scan_size_y_max = 100,      //X is right
        scan_size_X_max = 200,      //y is up
        scan_size_Y_max = 300,      //Y is down
        scan_thresh = 15,           //Number of required black pixels per line scan - if less, scan is "blank" and gap count is incremented
        gap_thresh = 2,             //Number of consecutive blank scans allowed
        gap_count = 0,              //Blank scan counter
        tmp_x = x,                  //temporary coordinates used by each scan
        tmp_y = y;
    
    bool gap = false;               //reset gap flag
    
    switch(dir) {
        case 'x':   //left
            for (tmp_x = x; tmp_x > x-scan_size_x_max && !gap && tmp_x > 0; tmp_x--) { //Outer loop is the main scan - reacts to the blank scans given by the inner loop
                int scan_count = 0; //reset blank scan count                           Inner loop scans along perpedicular direction until max or border, counting black pixels
                for (int scan_y = (tmp_y - scan_size_y_max >= 0) ? tmp_y - scan_size_y_max:0; scan_y < tmp_y + scan_size_Y_max && scan_y < video.size().height; scan_y++) {
                    if ((int)video.at<cv::Vec3b>(Point(tmp_x,scan_y))[0] == 0) {
                        //video.at<cv::Vec3b>(scan_y, tmp_x) = Vec3b(255,0,0);
                        scan_count++;
                    }
                }
                if (scan_count < scan_thresh)                   //If scan is blank, increment gap_count
                    gap_count++;
                else
                    gap_count = 0;                              //Else reset, only checks consecutive blank scans
                if (gap_count > gap_thresh)                     //If there is a gap, the gap flag is set, the loop ends and the current tmp_x is returned
                    gap = true;                                 //This is the same basic algorithm for each direction
            }
            //if (gap)
            return tmp_x;
            //else
                //return x - scan_size_x_max >= 0 ? x - scan_size_x_max:0;
        case 'X':   //right
            for (tmp_x = x; tmp_x < x+scan_size_X_max && !gap && tmp_x < video.size().width; tmp_x++) {
                int scan_count = 0;
                for (int scan_y = (tmp_y - scan_size_y_max >= 0) ? tmp_y - scan_size_y_max:0; scan_y < tmp_y + scan_size_Y_max && scan_y < video.size().height; scan_y++) {
                    if ((int)video.at<cv::Vec3b>(Point(tmp_x,scan_y))[0] == 0) {
                        //video.at<cv::Vec3b>(scan_y, tmp_x) = Vec3b(255,100,0);
                        scan_count++;
                    }
                }
                if (scan_count < scan_thresh)
                    gap_count++;
                else
                    gap_count = 0;
                if (gap_count > gap_thresh)
                    gap = true;
            }
            //if (gap)
            return tmp_x;
            //else
                //return x + scan_size_X_max < video.size().width ? x + scan_size_X_max:video.size().width;
        case 'y':   //up
            for (tmp_y = y; tmp_y > y-scan_size_y_max && !gap && tmp_y > 0; tmp_y--) {
                int scan_count = 0;
                for (int scan_x = (tmp_x - scan_size_x_max >= 0) ? tmp_x - scan_size_x_max:0; scan_x < tmp_x + scan_size_X_max && scan_x < video.size().width; scan_x++) {
                    if ((int)video.at<cv::Vec3b>(Point(scan_x,tmp_y))[0] == 0) {
                        //video.at<cv::Vec3b>(tmp_y, scan_x) = Vec3b(0,0,255);
                        scan_count++;
                    }
                }
                if (scan_count < scan_thresh)
                    gap_count++;
                else
                    gap_count = 0;
                if (gap_count > gap_thresh)
                    gap = true;
            }
            //if (gap)
            return tmp_y;
            //else
              //  return y - scan_size_y_max >= 0 ? y - scan_size_y_max:0;
        case 'Y':   //down
            for (tmp_y = y; tmp_y < y+scan_size_Y_max && !gap && tmp_y < video.size().height; tmp_y++) {
                int scan_count = 0;
                for (int scan_x = (tmp_x - scan_size_x_max >= 0) ? tmp_x - scan_size_x_max:0; scan_x < tmp_x + scan_size_X_max && scan_x < video.size().width; scan_x++) {
                    if ((int)video.at<cv::Vec3b>(Point(scan_x,tmp_y))[0] == 0) {
                        //video.at<cv::Vec3b>(tmp_y, scan_x) = Vec3b(0,100,255);
                        scan_count++;
                    }
                }
                if (scan_count < scan_thresh)
                    gap_count++;
                else
                    gap_count = 0;
                if (gap_count > gap_thresh)
                    gap = true;
            }
            //if (gap)
            return tmp_y;
            //else
                //return y + scan_size_y_max < video.size().height ? y + scan_size_y_max:video.size().height;
        default:
            return 0;
    }
}

//*********************checkChar**************************//
char checkChar(Mat &obj) {                              //Receives only the pointer to image matrix of the object in question
    float match_thresh = 0.7;                           //Match threshold, acts as percent
    std::string charList = "ABCDEFGHIJKLMNOPQRSTUVQWXYZ1234567890!@#$%^&*()";   //List of possible objects, each has a corresponding .jpg in the /data folder
    std::string similarChars[] = {"B8&", "I!", "J1", "O0", "S3$", "FE", "CG"};  //Collection of similar characters, add freely to this list
    for (int index = 0; index < charList.length (); index++) {                  //Go through each character starting with 'A'
        std::string path("data/");                      //Create path name
        path += charList.at(index);                     //...
        path += ".jpg";                                 //...
        //cout << "Path: " << path << endl;
        IplImage* img = cvLoadImage(path.c_str(),1);    //Load character image
        Mat character(img);                             //Translate into a Mat
        //imshow("Character Image", character);
        int match_count = 0;                            //Reset match counter
        //Mat diff(100, 100,  CV_8UC3);
        for (int i_y = 0; i_y < 100; i_y++) {           //Scan through each image starting from the top-left, count literal pixel matches
            for (int i_x = 0; i_x < 100; i_x++) {
                match_count += (character.at<cv::Vec3b>(Point(i_x,i_y)) == obj.at<cv::Vec3b>(Point(i_x,i_y))) ? 1:0;
            }
        }
        float match = (float)match_count/(100*100);     //Generate % match from the scan counter
        //cout << "% match on " << charList.at(index) << " is " << match << endl;
        char c;
        if (match > match_thresh) {                     //If % match is above the given threshhold
            c = charList.at(index);                     //Get actual character from the charList of the match
            //cout << "Size of similar chars is " << sizeof(similarChars)/sizeof(similarChars[0]) << endl;
            for (int s = 0; s < sizeof(similarChars)/sizeof(similarChars[0]); s++) {    //Run through the similar characters list
                //cout << "Current is " << similarChars[s] << endl;
                for (int k = 0; k < similarChars[s].length(); k++) {
                    if (c == similarChars[s].at(k))     //If the character found is in the list, send it to the checkSimilar function
                        c = checkSimilar(obj, similarChars[s]);
                }
            }
            return c;   //If there are no similar characters to worry about, return as a literal char
        } else {
            //...
        }
         
    }
    return '~';         //If there is no match, return a '~' (local null character)
}

//********************checkSimilar************************//
char checkSimilar(Mat &obj, std::string checkList) {    //Receives both the point to the object image matrix and a string of similar chars to check
    //cout << "Check list: " << checkList << endl;
    float best_match = 0.0;
    int best_index = 0;
    for (int x = 0; x < checkList.length(); x++) {  //Same as checkChar function, however, the object is checked against each character image in the group of similar chars
        std::string path("data/");
        path += checkList.at(x);
        path += ".jpg";
        //cout << "Path: " << path << endl;
        IplImage* img = cvLoadImage(path.c_str(),1);
        Mat character(img);
        int match_count = 0;
        Mat diff(100, 100,  CV_8UC3);
        for (int i_y = 0; i_y < 100; i_y++) {
            for (int i_x = 0; i_x < 100; i_x++) {
                match_count += (character.at<cv::Vec3b>(Point(i_x,i_y)) == obj.at<cv::Vec3b>(Point(i_x,i_y))) ? 1:0;
            }
        }
        if (match_count > best_match) {             //The best match is recorded...
            best_match = match_count;
            best_index = x;
        }
    }
    return checkList.at(best_index);                //...and is return as a literal char
}