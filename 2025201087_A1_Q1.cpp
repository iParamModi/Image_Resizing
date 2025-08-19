#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;

void calculateEnergy_row(const Mat &image, Mat &energy, long long rows, long long cols)
{
    for (long long i = 0; i < image.rows; i++)
    {
        for (long long j = 0; j < image.cols; j++)
        {
            Vec3b pixel1, pixel2, pixel3, pixel4;

            // Down neighbor (i+1, j)
            if (i + 1 < image.rows)
                pixel1 = image.at<Vec3b>(i + 1, j);
            else
                pixel1 = image.at<Vec3b>(i, j);

            // Up neighbor (i-1, j)
            if (i - 1 >= 0)
                pixel2 = image.at<Vec3b>(i - 1, j);
            else
                pixel2 = image.at<Vec3b>(i, j);

            // Right neighbor (i, j+1)
            if (j + 1 < image.cols)
                pixel3 = image.at<Vec3b>(i, j + 1);
            else
                pixel3 = image.at<Vec3b>(i, j);

            // Left neighbor (i, j-1)
            if (j - 1 >= 0)
                pixel4 = image.at<Vec3b>(i, j - 1);
            else
                pixel4 = image.at<Vec3b>(i, j);

            // Gradient differences
            int dxR = pixel3[2] - pixel4[2];
            int dxG = pixel3[1] - pixel4[1];
            int dxB = pixel3[0] - pixel4[0];

            int dyR = pixel1[2] - pixel2[2];
            int dyG = pixel1[1] - pixel2[1];
            int dyB = pixel1[0] - pixel2[0];

            energy.at<double>(i, j) = dxR * dxR + dxG * dxG + dxB * dxB + dyR * dyR + dyG * dyG + dyB * dyB;
        }
    }
}

vector<pair<long long, long long>> seamCalculation_row(Mat &dp, long long row, long long col)
{
    for (long long i = 1; i < row; i++)
    {
        for (long long j = 0; j < col; j++)
        {
            long long A = (j - 1 < 0) ? LLONG_MAX : dp.at<double>(i - 1, j - 1);
            long long B = dp.at<double>(i - 1, j);
            long long C = (j + 1 >= col) ? LLONG_MAX : dp.at<double>(i - 1, j + 1);

            dp.at<double>(i, j) = dp.at<double>(i, j) + min(A, min(B, C));
        }
    }

    long long minRow = row - 1;
    long long minCol = 0;

    for (long long j = 0; j < col; j++)
    {
        if (dp.at<double>(row - 1, j) < dp.at<double>(minRow, minCol))
        {
            minCol = j;
        }
    }

    vector<pair<long long, long long>> seamPoint;

    while (minRow > 0)
    {
        seamPoint.push_back({minRow, minCol});

        long long A = (minCol - 1 < 0) ? LLONG_MAX : dp.at<double>(minRow - 1, minCol - 1);
        long long B = dp.at<double>(minRow - 1, minCol);
        long long C = (minCol + 1 >= col) ? LLONG_MAX : dp.at<double>(minRow - 1, minCol + 1);

        minRow--;

        long long minVal = min(A, min(B, C));

        if (A == minVal)
        {

            minCol--;
        }
        else if (C == minVal)
        {
            minCol++;
        }
    }
    seamPoint.push_back({0, minCol});
    reverse(seamPoint.begin(), seamPoint.end());

    return seamPoint;
}

void imageUpdate_row(Mat &image, Mat &dp, long long row, long long col, vector<pair<long long, long long>> &seamPoint)
{
    Mat temp = image.clone();

    for (auto pt : seamPoint)
    {
        long long i = pt.first;
        long long j = pt.second;

        temp.at<Vec3b>(i, j) = Vec3b(0, 0, 255);
    }
    imshow("Display Image", temp);
    waitKey(50);

    for (long long i = 0; i < row; i++)
    {
        int seamCol = seamPoint[i].second;

        for (long long j = seamCol; j < col - 1; j++)
        {
            image.at<Vec3b>((int)i, (int)j) = image.at<Vec3b>((int)i, (int)(j + 1));
        }
    }
    image = image.colRange(0, col - 1).clone();
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        cout << "Usage: ./program <image_path>" << endl;
        return -1;
    }

    // Load image
    Mat image = imread(argv[1]);

    // Mat image(50, 50, CV_8UC3);

    long long desiredRow = 40;
    long long desiredCol = 40;

    cout << "Enter Desired Row : ";
    cin >> desiredRow;

    cout << "Enter Desired Column : ";
    cin >> desiredCol;

    // Fill with random values so pixels aren’t all zero
    // randu(image, Scalar(0, 0, 0), Scalar(255, 255, 255));

    if (image.empty())
    {
        cout << "Error: Cannot load image!" << endl;
        return -1;
    }

    cout << "Image size: " << image.rows << " x " << image.cols << endl;

    long long row = (long long)image.rows;
    long long col = (long long)image.cols;

    while (col > desiredCol)
    {
        Mat energy(row, col, CV_64F, Scalar(0));    
        calculateEnergy_row(image, energy, row, col);

        Mat dp = energy.clone();
        vector<pair<long long, long long>> seamPoint = seamCalculation_row(dp, row, col);

        imageUpdate_row(image, dp, row, col, seamPoint);
        col = (long long)image.cols;

        imshow("Display Image", image);
        waitKey(80);
    }

    // while (row > desiredRow)
    // {
    //     Mat energy(row, col, CV_64F, Scalar(0));    
    //     calculateEnergy(image, energy, row, col);

    //     Mat dp = energy.clone();
    //     vector<pair<long long, long long>> seamPoint = seamCalculation(dp, row, col);

    //     imageUpdate(image, dp, row, col, seamPoint);
    //     row = (long long)image.row;

    //     imshow("Display Image", image);
    //     waitKey(80);
    // }

    imwrite("original.png", image);
    cout << "Image saved to original.png" << endl;

    return 0;
}
