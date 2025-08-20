#include <opencv2/opencv.hpp>
#include <string>
#include <cstring>
#include <iostream>
using namespace std;
using namespace cv;

// ============================= GET FILE NAME =============================
const char *getFileName(const char *file)
{
    const char *check2 = strrchr(file, '/');

    const char *only_file = (check2) ? (check2 + 1) : (file);
    return only_file;
}

// ============================= ENERGY CALCULATION USING DUAL GRADIENT =============================
void calculateEnergy(const Mat &image, Mat &energy, long long rows, long long cols)
{
    for (long long i = 0; i < image.rows; i++)
    {
        for (long long j = 0; j < image.cols; j++)
        {
            Vec3b pixel1, pixel2, pixel3, pixel4;

            // ============================= DOWN NEIGHBOUR =============================
            if (i + 1 < image.rows)
                pixel1 = image.at<Vec3b>(i + 1, j);
            else
                pixel1 = image.at<Vec3b>(i, j);

            // ============================= UP NEIGHBOUR =============================
            if (i - 1 >= 0)
                pixel2 = image.at<Vec3b>(i - 1, j);
            else
                pixel2 = image.at<Vec3b>(i, j);

            // ============================= RIGHT NEIGHBOUR =============================
            if (j + 1 < image.cols)
                pixel3 = image.at<Vec3b>(i, j + 1);
            else
                pixel3 = image.at<Vec3b>(i, j);

            // ============================= LEFT NEIGHBOUR =============================
            if (j - 1 >= 0)
                pixel4 = image.at<Vec3b>(i, j - 1);
            else
                pixel4 = image.at<Vec3b>(i, j);

            // ============================= DIFFERENCE =============================
            int dxR = pixel3[2] - pixel4[2];
            int dxG = pixel3[1] - pixel4[1];
            int dxB = pixel3[0] - pixel4[0];

            int dyR = pixel1[2] - pixel2[2];
            int dyG = pixel1[1] - pixel2[1];
            int dyB = pixel1[0] - pixel2[0];

            energy.at<double>(i, j) = sqrt(dxR * dxR + dxG * dxG + dxB * dxB + dyR * dyR + dyG * dyG + dyB * dyB);
        }
    }
}

// ============================= SEAM CALCULATION ROW MAJOR =============================
void seamCalculation_col(Mat &dp, long long row, long long col, long long seamPoint[])
{
    // ============================= DYNAMIC APPROACH FOR VALUES =============================
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
        if (dp.at<double>(minRow, j) < dp.at<double>(minRow, minCol))
        {
            minCol = j;
        }
    }

    // ============================= BACKTRACKING =============================
    while (minRow > 0)
    {
        seamPoint[minRow] = minCol;

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
    seamPoint[minRow] = minCol;
}

// ============================= REMOVING A COLUMN =============================
void imageUpdate_col(Mat &image, Mat &dp, long long row, long long col, long long seamPoint[])
{
    Mat temp = image.clone();

    // ============================= RED LINE =============================
    for (long long i = 0; i < row; i++)
    {
        long long j = seamPoint[i];

        temp.at<Vec3b>(i, j) = Vec3b(0, 0, 255);
    }
    imshow("Display Image", temp);
    waitKey(15);

    // ============================= REMOVING COL =============================

    for (long long i = 0; i < row; i++)
    {
        int seamCol = seamPoint[i];

        for (long long j = seamCol; j < col - 1; j++)
        {
            image.at<Vec3b>((int)i, (int)j) = image.at<Vec3b>((int)i, (int)(j + 1));
        }
    }
    image = image(Rect(0, 0, col - 1, row)).clone();
}

// ============================= SEAM CALCULATION COL MAJOR =============================
void seamCalculation_row(Mat &dp, long long row, long long col, long long seamPoint[])
{
    // ============================= DYNAMIC APPROACH =============================
    for (long long j = 1; j < col; j++)
    {
        for (long long i = 0; i < row; i++)
        {
            long long A = (i - 1 < 0) ? LLONG_MAX : dp.at<double>(i - 1, j - 1);
            long long B = dp.at<double>(i, j - 1);
            long long C = (i + 1 >= row) ? LLONG_MAX : dp.at<double>(i + 1, j - 1);

            dp.at<double>(i, j) = dp.at<double>(i, j) + min(A, min(B, C));
        }
    }

    long long minCol = col - 1;
    long long minRow = 0;

    for (long long i = 1; i < row; i++)
    {
        if (dp.at<double>(i, minCol) < dp.at<double>(minRow, minCol))
        {
            minRow = i;
        }
    }

    // ============================= BACKTRACKING =============================
    while (minCol > 0)
    {
        seamPoint[minCol] = minRow;

        long long A = (minRow - 1 < 0) ? LLONG_MAX : dp.at<double>(minRow - 1, minCol - 1);
        long long B = dp.at<double>(minRow, minCol - 1);
        long long C = (minRow + 1 >= row) ? LLONG_MAX : dp.at<double>(minRow + 1, minCol - 1);

        minCol--;

        long long minVal = min(A, min(B, C));

        if (A == minVal)
        {
            minRow--;
        }
        else if (C == minVal)
        {
            minRow++;
        }
    }
    seamPoint[minCol] = minRow;
}

// ============================= REMOVING A ROW =============================
void imageUpdate_row(Mat &image, Mat &dp, long long row, long long col, long long seamPoint[])
{
    Mat temp = image.clone();
    // ============================= RED LINE =============================
    for (long long i = 0; i < col; i++)
    {
        long long j = seamPoint[i];

        temp.at<Vec3b>(j, i) = Vec3b(0, 0, 255);
    }
    imshow("Display Image", temp);
    waitKey(15);

    // ============================= REMOVING ROW =============================
    for (long long i = 0; i < col; i++)
    {
        int seamRow = seamPoint[i];

        for (long long j = seamRow; j < row - 1; j++)
        {
            image.at<Vec3b>((int)j, (int)i) = image.at<Vec3b>((int)(j + 1), (int)i);
        }
    }
    image = image(Rect(0, 0, col, row - 1)).clone();
}

int main(int argc, char **argv)
{
    // ======== VALIDATIONS ========
    if (argc < 2)
    {
        cout << "Usage: ./program <image_path>" << endl;
        return -1;
    }

    Mat image = imread(argv[1]);
    // ======== IMAGE PROCESSING ERROR ========
    if (image.empty())
    {
        cout << "Error: Cannot load image!" << endl;
        return -1;
    }

    long long desiredRow;
    long long desiredCol;

    cout << "Image size: " << image.cols << " x " << image.rows << endl;

    cout << "Enter Desired Width : ";
    cin >> desiredCol;

    cout << "Enter Desired Height : ";
    cin >> desiredRow;

    long long row = (long long)image.rows;
    long long col = (long long)image.cols;

    // ======== VERTICAL SEAM REMOVAL ========
    while (col > desiredCol)
    {
        Mat energy(row, col, CV_64F, Scalar(0));
        calculateEnergy(image, energy, row, col);

        Mat dp = energy.clone();
        long long seamPoint[row] = {0};
        seamCalculation_col(dp, row, col, seamPoint);

        imageUpdate_col(image, dp, row, col, seamPoint);
        col = (long long)image.cols;

        imshow("Display Image", image);
        waitKey(15);
    }

    // ======== HORIZONTAL SEAM REMOVAL ========
    while (row > desiredRow)
    {
        Mat energy(row, col, CV_64F, Scalar(0));
        calculateEnergy(image, energy, row, col);

        Mat dp = energy.clone();
        long long seamPoint[col] = {0};
        seamCalculation_row(dp, row, col, seamPoint);

        imageUpdate_row(image, dp, row, col, seamPoint);
        row = (long long)image.rows;

        imshow("Display Image", image);
        waitKey(15);
    }

    // ======== IMAGE SAVE ========
    string fileName = string("Compressed_") + getFileName(argv[1]);
    imwrite(fileName, image);

    cout << "Image saved as " << fileName << endl;
    imshow("Display Image", image);
    waitKey(0);

    return 0;
}
