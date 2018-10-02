#ifndef MYIMAGE_H
#define MYIMAGE_H

#include <iostream>
#include <cmath>

#include <QFile>
#include <QImage>
#include <QString>
#include <QVector>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class MyImage
{
private:
    QString qTitle;

    // --- Sizing Variables ---
    int sizeK = 0;
    int sizeM = 0;
    int sizeN = 0;
    int padP = 0;
    int sizeP = 0;
    int padQ = 0;
    int sizeQ = 0;

public:
    // --- Consructor / Destructor ---
    MyImage(QString input);
    ~MyImage();

    // --- Matrices ---
    cv::Mat imgRaw;
    cv::Mat imgResized;
    cv::Mat imgPadded;
    cv::Mat imgCentered;
    cv::Mat imgCenteredScaled;
    cv::Mat imgSpectrum;
    cv::Mat imgFiltered;
    cv::Mat imgRecovered;
    cv::Mat imgCropped;
    cv::Mat imgCheckerBoard;
    cv::Mat imgMask;

    cv::Mat complexSpectrum;
    cv::Mat complexMask;
    cv::Mat complexFiltered;

    // --- Accessors ---
    QImage getQImage(int selector);
    void saveImageToPNG(QString outputPath);

    // --- Initial Mutators ---
    void loadData(QString filePath);
    void loadData(QString filePath, int intensityValue);

    // --- Mutators ---
    void preFilter();
    void resizeInputImage();
    void padInputImage();
    void centerInputImage();
    void transform2Spectrum();

    void filterImage(int filterSelector, double D0, int n);
    void buildFilter();
    void transform2Spatial();
    void cropFinalImage();


};

#endif // MYIMAGE_H
