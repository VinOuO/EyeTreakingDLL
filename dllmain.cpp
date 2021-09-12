#include "pch.h"
#include <iostream>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <vector>
#include "OpencvDLL.h"
//#include <fileapi.h>
#include <windows.h>
#include <conio.h>
#include <stdlib.h>
#include <direct.h>
#include <stdio.h>

#include <fstream>

using namespace std;
using namespace cv;

namespace UsingOpenCV
{
    Vec3f getEyeball(Mat& eye, vector<Vec3f>& circles)
    {
        vector<int> sums(circles.size(), 0);
        for (int y = 0; y < eye.rows; y++)
        {
            uchar* ptr = eye.ptr<uchar>(y);
            for (int x = 0; x < eye.cols; x++)
            {
                int value = static_cast<int>(*ptr);
                for (int i = 0; i < circles.size(); i++)
                {
                    Point center((int)round(circles[i][0]), (int)round(circles[i][1]));
                    int radius = (int)round(circles[i][2]);
                    if (pow(x - center.x, 2) + pow(y - center.y, 2) < pow(radius, 2))
                    {
                        sums[i] += value;
                    }
                }
                ++ptr;
            }
        }
        int smallestSum = 9999999;
        int smallestSumIndex = -1;
        for (int i = 0; i < circles.size(); i++)
        {
            if (sums[i] < smallestSum)
            {
                smallestSum = sums[i];
                smallestSumIndex = i;
            }
        }
        return circles[smallestSumIndex];
    }

    Rect getLeftmostEye(vector<Rect>& eyes)
    {
        int leftmost = 99999999;
        int leftmostIndex = -1;
        for (int i = 0; i < eyes.size(); i++)
        {
            if (eyes[i].tl().x < leftmost)
            {
                leftmost = eyes[i].tl().x;
                leftmostIndex = i;
            }
        }
        return eyes[leftmostIndex];
    }

    Rect getRightmostEye(vector<Rect>& eyes)
    {
        int rightmost = -1;
        int righttmostIndex = -1;
        for (int i = 0; i < eyes.size(); i++)
        {
            if (eyes[i].tl().x > rightmost)
            {
                rightmost = eyes[i].tl().x;
                righttmostIndex = i;
            }
        }
        return eyes[righttmostIndex];
    }

    vector<Point> centers;
    Point lastPoint;
    Point mousePoint;

    Point stabilize(vector<Point>& points, int windowSize)
    {
        float sumX = 0;
        float sumY = 0;
        int count = 0;
        for (int i = max(0, (int)(points.size() - windowSize)); i < points.size(); i++)
        {
            sumX += points[i].x;
            sumY += points[i].y;
            ++count;
        }
        if (count > 0)
        {
            sumX /= count;
            sumY /= count;
        }
        return Point(sumX, sumY);
    }

    bool Is_Gray(Vec3b _Color)
    {
        int _Big = 0, _Small = 255;

        for (int i = 0; i <= 2; i++)
        {
            if (_Color[i] > _Big)
            {
                _Big = _Color[i];
            }
            if (_Color[i] < _Small)
            {
                _Small = _Color[i];
            }
        }

        if (_Big - _Small <= 30)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    int Max(vector<int> _Vector)
    {
        int _Max[21] = { INT16_MIN,INT16_MIN,INT16_MIN,INT16_MIN,INT16_MIN,INT16_MIN };
        for (int i = 0; i < _Vector.size(); i++)
        {
            if (_Vector[i] > _Max[20])
            {
                for (int k = 0; k < 20; k++)
                {
                    _Max[k] = _Max[k + 1];
                }
                _Max[20] = _Vector[i];
            }
            else
            {
                for (int j = 0; j < 20; j++)
                {
                    if (_Vector[i] > _Max[j] && _Vector[i] <= _Max[j + 1])
                    {
                        for (int k = 0; k < j; k++)
                        {
                            _Max[k] = _Max[k + 1];
                        }
                        _Max[j] = _Vector[i];
                    }
                }
            }

        }
        return _Max[0];
    }
    int Min(vector<int> _Vector)
    {
        int _Min[6] = { INT16_MAX,INT16_MAX,INT16_MAX,INT16_MAX,INT16_MAX,INT16_MAX };
        for (int i = 0; i < _Vector.size(); i++)
        {
            if (_Vector[i] < _Min[5])
            {
                _Min[0] = _Min[1];
                _Min[1] = _Min[2];
                _Min[2] = _Min[3];
                _Min[3] = _Min[4];
                _Min[4] = _Min[5];
                _Min[5] = _Vector[i];
            }
            else
            {
                for (int j = 0; j < 5; j++)
                {
                    if (_Vector[i] < _Min[j] && _Vector[i] >= _Min[j + 1])
                    {
                        for (int k = 0; k < j; k++)
                        {
                            _Min[k] = _Min[k + 1];
                        }
                        _Min[j] = _Vector[i];
                    }
                }
            }

        }
        return _Min[0];
    }
    float Max_f(vector<float> _Vector)
    {
        float _Max[21] = { INT16_MIN,INT16_MIN,INT16_MIN,INT16_MIN,INT16_MIN,INT16_MIN };
        for (int i = 0; i < _Vector.size(); i++)
        {
            if (_Vector[i] > _Max[20])
            {
                for (int k = 0; k < 20; k++)
                {
                    _Max[k] = _Max[k + 1];
                }
                _Max[20] = _Vector[i];
            }
            else
            {
                for (int j = 0; j < 20; j++)
                {
                    if (_Vector[i] > _Max[j] && _Vector[i] <= _Max[j + 1])
                    {
                        for (int k = 0; k < j; k++)
                        {
                            _Max[k] = _Max[k + 1];
                        }
                        _Max[j] = _Vector[i];
                    }
                }
            }

        }
        return _Max[0];
    }
    float Min_f(vector<float> _Vector)
    {
        float _Min[6] = { INT16_MAX,INT16_MAX,INT16_MAX,INT16_MAX,INT16_MAX,INT16_MAX };
        for (int i = 0; i < _Vector.size(); i++)
        {
            if (_Vector[i] < _Min[5])
            {
                _Min[0] = _Min[1];
                _Min[1] = _Min[2];
                _Min[2] = _Min[3];
                _Min[3] = _Min[4];
                _Min[4] = _Min[5];
                _Min[5] = _Vector[i];
            }
            else
            {
                for (int j = 0; j < 5; j++)
                {
                    if (_Vector[i] < _Min[j] && _Vector[i] >= _Min[j + 1])
                    {
                        for (int k = 0; k < j; k++)
                        {
                            _Min[k] = _Min[k + 1];
                        }
                        _Min[j] = _Vector[i];
                    }
                }
            }

        }
        return _Min[0];
    }

    int Eye_Max_Pixel, Eye_Min_Pixel;
    float Eye_Max_Ratio, Eye_Min_Ratio;

    void Max_and_Min_Normal(vector<int> _Vector)
    {
        vector<int> _temp = _Vector;
        sort(_temp.begin(), _temp.end());
        float Average = 0, Stander = 0;
        int _Flag = 0;
        float _Num = 1.5f;
        for (int i = 0; i < _temp.size(); i++)
        {
            Average += _temp[i];
        }
        Average /= _temp.size();

        for (int i = 0; i < _temp.size(); i++)
        {
            Stander += pow((_temp[i] - Average), 2);
        }
        Stander /= _temp.size();
        Stander = sqrtf(Stander);
        for (int i = 0; i < _temp.size() && _Flag < 2; i++)
        {
            if (i == _temp.size() - 1)
            {
                Eye_Max_Pixel = _temp[_temp.size() - 1];
                Eye_Min_Pixel = _temp[0];
                cerr << "8787" << endl;
            }
            else
            {
                if (_temp[i] < (Average - Stander * _Num) && _temp[i + 1] > (Average - Stander * _Num))
                {
                    Eye_Min_Pixel = _temp[i + 1];
                    _Flag++;
                }
                if (_temp[i] < (Average + Stander * _Num) && _temp[i + 1] > (Average + Stander * _Num))
                {
                    Eye_Max_Pixel = _temp[i];
                    _Flag++;
                }
            }
        }
    }

    void Maxf_and_Minf_Normal(vector<float> _Vector)
    {
        vector<float> _temp = _Vector;
        sort(_temp.begin(), _temp.end());
        float Average = 0, Stander = 0;
        int _Flag = 0;
        float _Num = 1.5f;
        for (int i = 0; i < _temp.size(); i++)
        {
            Average += _temp[i];
        }
        Average /= _temp.size();

        for (int i = 0; i < _temp.size(); i++)
        {
            Stander += pow((_temp[i] - Average), 2);
        }
        Stander /= _temp.size();
        Stander = sqrtf(Stander);

        for (int i = 0; i < _temp.size() && _Flag < 2; i++)
        {
            if (i == _temp.size() - 1)
            {
                Eye_Max_Ratio = _temp[_temp.size() - 1];
                Eye_Min_Ratio = _temp[0];
                cerr << "7788" << endl;
            }
            else
            {
                if (_temp[i] < (Average - Stander * _Num) && _temp[i + 1] > (Average - Stander * _Num))
                {
                    Eye_Min_Ratio = _temp[i + 1];
                    _Flag++;
                }
                if (_temp[i] < (Average + Stander * _Num) && _temp[i + 1] > (Average + Stander * _Num))
                {
                    Eye_Max_Ratio = _temp[i];
                    _Flag++;
                }
            }
        }
    }
    /*
    wstring s2ws(const string& s)
    {
        int len;
        int slength = (int)s.length() + 1;
        len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
        wchar_t* buf = new wchar_t[len];
        MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
        wstring r(buf);
        delete[] buf;
        return r;
    }

    string GetFullPath(char* partialPath)
    {
        char full[_MAX_PATH];
        if (_fullpath(full, partialPath, _MAX_PATH) != NULL)
            return string(full);
        else
            return "";
    }
    */

    void writeFile(char* _Path)
    {
        ofstream myfile;
        myfile.open("V:/Work/Game Project/Project Spy/Project Spy - Unity/For_OpencvA/Test.txt");
        myfile << string(_Path);
        myfile.close();
    }

    VideoCapture cap(0);
    CascadeClassifier faceCascade;
    CascadeClassifier eyeCascade;
    extern "C"
    {
        int Functions::Initialize(char* _Path)
        {
            /*
            //---
            string _File_Name;
            static const size_t BufferSize = 256;
            wchar_t canonicalPath[BufferSize];
            _File_Name = "haarcascade_frontalface_alt2.xml";
            GetFullPathName(s2ws(_File_Name).c_str(), BufferSize, canonicalPath, 0);
            wstring _ws(canonicalPath);
            //---
            */
            //writeFile(_Path);

            if (!faceCascade.load(string(_Path) + "/haarcascade_frontalface_alt2.xml"))
            {
                return 3;
            }

            if (!eyeCascade.load(string(_Path) + "/haarcascade_eye_tree_eyeglasses.xml"))
            {
                return 2;
            }

            if (!cap.isOpened())
            {
                return 1;
            }
            return 0;
        }



        vector<int> Eye_Pixels = {};
        vector<float> Eye_Ratios = {};
        static float _Ratio[2] = { 0 ,0 };
        void Functions::Set_Eye_On_Screan_Pos()
        {
            Mat frame;
            cap >> frame;
            Mat grayscale = frame;

            vector<Rect> faces;
            faceCascade.detectMultiScale(grayscale, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(150, 150));
            if (faces.size() == 0) return; // none face was detected
            Mat face = grayscale(faces[0]); // crop the face
            vector<Rect> eyes;
            eyeCascade.detectMultiScale(face, eyes, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30)); // same thing as above    
            rectangle(frame, faces[0].tl(), faces[0].br(), Scalar(255, 0, 0), 2);
            if (eyes.size() != 2) return; // both eyes were not detected

            for (Rect& eye : eyes)
            {
                rectangle(frame, faces[0].tl() + eye.tl(), faces[0].tl() + eye.br(), Scalar(255, 0, 0), 2);
            }
            Rect eyeRectL = getLeftmostEye(eyes);
            Rect eyeRectR = getRightmostEye(eyes);
            int Eye_Pixel = 0;
            //---Right Eye
            Mat eye_R = face(eyeRectR);
            int eye_R_Black_Pixel_Num = 1;
            Point eye_R_Black_Center(0, 0);
            for (int y = 0; y < eye_R.rows; y++)
            {
                for (int x = 0; x < eye_R.cols; x++)
                {
                    Vec3b& color = eye_R.at<Vec3b>(y, x);
                    if (Is_Gray(color) && y < eye_R.rows * 0.65f && y > eye_R.rows * 0.35f && x < eye_R.cols * 0.8f && x > eye_R.cols * 0.2f)
                    {
                        Eye_Pixel++;
                        if (color[0] <= 30 && color[1] <= 30 && color[2] <= 30)
                        {
                            color[0] = 0;
                            color[1] = 0;
                            color[2] = 255;
                            eye_R_Black_Center.x += x;
                            eye_R_Black_Center.y += y;
                            eye_R_Black_Pixel_Num++;
                        }
                        else
                        {
                            color[0] = 0;
                            color[1] = 255;
                            color[2] = 0;
                        }
                    }
                    else
                    {
                        color[0] = 255;
                        color[1] = 255;
                        color[2] = 255;
                    }

                }
            }
            eye_R_Black_Center /= eye_R_Black_Pixel_Num;

            int eye_R_White_Pixel_Num = 1, eye_R_white1_Pixel_Num = 1, eye_R_white2_Pixel_Num = 1;
            Point eye_R_White_Center(0, 0);
            for (int y = 0; y < eye_R.rows; y++)
            {
                for (int x = 0; x < eye_R.cols; x++)
                {
                    Vec3b& color = eye_R.at<Vec3b>(y, x);
                    if (color[0] == 0 && color[1] == 255 && color[2] == 0)
                    {
                        eye_R_White_Center.x += x;
                        eye_R_White_Center.y += y;
                        eye_R_White_Pixel_Num++;
                        if (x <= eye_R_Black_Center.x)
                        {
                            eye_R_white1_Pixel_Num++;
                        }
                        else
                        {
                            eye_R_white2_Pixel_Num++;
                        }
                    }
                }
            }
            eye_R_White_Center /= eye_R_White_Pixel_Num;
            //---Right Eye

            //---Left Eye
            Mat eye_L = face(eyeRectL);
            int eye_L_Black_Pixel_Num = 1;
            Point eye_L_Black_Center(0, 0);
            for (int y = 0; y < eye_L.rows; y++)
            {
                for (int x = 0; x < eye_L.cols; x++)
                {
                    Vec3b& color = eye_L.at<Vec3b>(y, x);
                    if (Is_Gray(color) && y < eye_L.rows * 0.65f && y > eye_L.rows * 0.35f && x < eye_L.cols * 0.8f && x > eye_L.cols * 0.2f)
                    {
                        Eye_Pixel++;
                        if (color[0] <= 30 && color[1] <= 30 && color[2] <= 30)
                        {
                            color[0] = 0;
                            color[1] = 0;
                            color[2] = 255;
                            eye_L_Black_Center.x += x;
                            eye_L_Black_Center.y += y;
                            eye_L_Black_Pixel_Num++;
                        }
                        else
                        {
                            color[0] = 0;
                            color[1] = 255;
                            color[2] = 0;
                        }
                    }
                    else
                    {
                        color[0] = 255;
                        color[1] = 255;
                        color[2] = 255;
                    }

                }
            }
            eye_L_Black_Center /= eye_L_Black_Pixel_Num;

            int eye_L_White_Pixel_Num = 1, eye_L_white1_Pixel_Num = 1, eye_L_white2_Pixel_Num = 1;
            Point eye_L_White_Center(0, 0);
            for (int y = 0; y < eye_L.rows; y++)
            {
                for (int x = 0; x < eye_L.cols; x++)
                {
                    Vec3b& color = eye_L.at<Vec3b>(y, x);
                    if (color[0] == 0 && color[1] == 255 && color[2] == 0)
                    {
                        eye_L_White_Center.x += x;
                        eye_L_White_Center.y += y;
                        eye_L_White_Pixel_Num++;
                        if (x > eye_L_Black_Center.x)
                        {
                            eye_L_white1_Pixel_Num++;
                        }
                        else
                        {
                            eye_L_white2_Pixel_Num++;
                        }
                    }
                }
            }
            eye_L_White_Center /= eye_L_White_Pixel_Num;
            //---Left Eye

            Eye_Pixel /= 2;
            if (Eye_Pixels.size() < 30)
            {
                Eye_Pixels.push_back(Eye_Pixel);
                Eye_Max_Pixel = Max(Eye_Pixels);
                Eye_Min_Pixel = Min(Eye_Pixels);
            }
            else
            {
                while (Eye_Pixels.size() > 500)
                {
                    Eye_Pixels.erase(Eye_Pixels.begin());
                }
                Eye_Pixels.push_back(Eye_Pixel);
                Max_and_Min_Normal(Eye_Pixels);
            }
            float Eye_Ratio_Now = float(eye_L_white1_Pixel_Num) / float(eye_L_white1_Pixel_Num + eye_R_white1_Pixel_Num) + (float(eye_R_white2_Pixel_Num) / float(eye_R_white2_Pixel_Num + eye_L_white2_Pixel_Num)) / 2;
            if (Eye_Ratios.size() < 30)
            {
                Eye_Ratios.push_back(Eye_Ratio_Now);
                Eye_Max_Ratio = Max_f(Eye_Ratios);
                Eye_Min_Ratio = Min_f(Eye_Ratios);
            }
            else
            {
                while (Eye_Ratios.size() > 500)
                {
                    Eye_Ratios.erase(Eye_Ratios.begin());
                }
                Eye_Ratios.push_back(Eye_Ratio_Now);
                Maxf_and_Minf_Normal(Eye_Ratios);
            }
            //Mat BK = imread("BK.png");
            float x_Ratio = 0.5f, y_Ratio = 0.5f;
            if (Eye_Max_Ratio > Eye_Min_Ratio)
            {
                x_Ratio = float(Eye_Ratio_Now - Eye_Min_Ratio) / float(Eye_Max_Ratio - Eye_Min_Ratio);
            }
            if (Eye_Max_Pixel > Eye_Min_Pixel)
            {
                y_Ratio = float(Eye_Pixel - Eye_Min_Pixel) / float(Eye_Max_Pixel - Eye_Min_Pixel);
            }
            //circle(BK, Point(int(x_Ratio * 1920), int(y_Ratio * 1080)), 5, Scalar(0, 255, 0), 60);

            _Ratio[0] = x_Ratio;
            _Ratio[1] = y_Ratio;
        }

        float Functions::Get_Eye_On_Screan_Pos_x()
        {
            return _Ratio[0];
        }

        float Functions::Get_Eye_On_Screan_Pos_y()
        {
            return _Ratio[1];
        }

    }
}