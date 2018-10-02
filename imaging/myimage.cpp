#include "myimage.h"

// ===== CONSTRUCTOR / DESTRUCTOR =============================================
MyImage::MyImage(QString input)
{
    qTitle = input;
}
MyImage::~MyImage()
{
    // destructor call goes here
}
// ===== ACCESSORS ============================================================
// --- Get QImage conversions from OpenCV image to use in the GUI ---
QImage MyImage::getQImage(int selector)
{
    cv::Mat tmp;

    switch (selector) {
    case 0:
        tmp = imgRaw;
        break;
    case 1:
        tmp = imgResized;
        break;
    case 2:
        tmp = imgPadded;
        break;
    case 3:
        tmp = imgCenteredScaled;
         break;
    case 4:
        tmp = imgSpectrum;
        break;
    case 5:
        tmp = imgMask;
        break;
    case 6:
        tmp = imgFiltered;
        break;
    case 7:
        tmp = imgRecovered;
        break;
    case 8:
        tmp = imgCropped;
        break;
    default:
        tmp = imgRaw;
        break;
    }

    tmp.convertTo(tmp,CV_8UC1,255,0);
    QImage dest((const uchar *) tmp.data, tmp.cols, tmp.rows, tmp.step, QImage::Format_Indexed8);
    tmp.release();
    dest.bits();
    return dest;
}
// --- Save the OpenCV image to a PNG file ---
void MyImage::saveImageToPNG(QString outputPath)
{
    std::vector<int> compression_parameters;
    compression_parameters.push_back(cv::IMWRITE_PNG_COMPRESSION);
    compression_parameters.push_back(3);

    outputPath += "/";
    QString tmpString;
    cv::Mat tmpMat;

    tmpString = "img_raw.png";
    imgRaw.convertTo(tmpMat,CV_8UC1,255,0);
    cv::imwrite((outputPath+tmpString).toStdString(), tmpMat, compression_parameters);

    tmpString = "img_spectrum.png";
    imgSpectrum.convertTo(tmpMat,CV_8UC1,255,0);
    cv::imwrite((outputPath+tmpString).toStdString(), tmpMat, compression_parameters);

    tmpString = "img_mask.png";
    imgMask.convertTo(tmpMat,CV_8UC1,255,0);
    cv::imwrite((outputPath+tmpString).toStdString(), tmpMat, compression_parameters);

    tmpString = "img_filtered.png";
    imgFiltered.convertTo(tmpMat,CV_8UC1,255,0);
    cv::imwrite((outputPath+tmpString).toStdString(), tmpMat, compression_parameters);

    tmpString = "img_output.png";
    imgCropped.convertTo(tmpMat,CV_8UC1,255,0);
    cv::imwrite((outputPath+tmpString).toStdString(), tmpMat, compression_parameters);
}
// ===== INITIAL MUTATORS =====================================================
// --- Set image to data read from file ---
void MyImage::loadData(QString filePath)
{
    cv::destroyAllWindows();
    imgRaw.release();
    imgRaw = cv::imread(filePath.toStdString(), CV_LOAD_IMAGE_GRAYSCALE);
    preFilter();
}
// --- Set image to data read from default file or to default flat intensity ---
void MyImage::loadData(QString filePath, int intensityValue)
{
    cv::destroyAllWindows();
    imgRaw.release();
    if(intensityValue<0) {
        QFile file(filePath);

        if(file.open(QIODevice::ReadOnly)) {
            qint64 imageFileSize = file.size();
            std::vector<uchar> buf(imageFileSize);

            file.read((char*)buf.data(), imageFileSize);

            imgRaw = cv::imdecode(buf, CV_LOAD_IMAGE_GRAYSCALE);
        }
    }
    else {
        imgRaw = cv::Mat::ones(256, 256, CV_LOAD_IMAGE_GRAYSCALE) * intensityValue;
    }
    preFilter();
}
// ===== MUTATORS =============================================================
void MyImage::preFilter()
{
    resizeInputImage();
    padInputImage();
    centerInputImage();
    transform2Spectrum();
}
void MyImage::resizeInputImage()
{
    // --- Convert initial image from 8-bit uint to 32-bit float ---
    imgRaw.convertTo(imgRaw,CV_32FC1);
    cv::normalize(imgRaw,imgRaw,1,0,cv::NORM_MINMAX,CV_32FC1);

    // --- Get the minimum size dimension ---
    int tmpMin = imgRaw.rows;
    if(imgRaw.cols < tmpMin){
        tmpMin = imgRaw.cols;
    }

    // --- Define optimized sizing variables to use with image ---
    for(int i=1; i<12; i++){
        if (pow(2,i)>tmpMin){
            sizeM = pow(2,i-1);
            sizeN = sizeM;
            sizeK = sizeM * sizeN;
            break;
        }
    }
    sizeP = 2*sizeM;
    sizeQ = 2*sizeN;

    // --- Create the crop region of interest ---
    cv::Rect croppedRegion;
    croppedRegion.x = (imgRaw.cols - sizeM) / 2;
    croppedRegion.y = (imgRaw.rows - sizeN) / 2;
    croppedRegion.width = sizeM;
    croppedRegion.height = sizeN;

    // --- Crop the image to appropriate size for DFT/FFT ---
    cv::Mat tmp = imgRaw(croppedRegion);
    imgResized.release();
    tmp.copyTo(imgResized);
    imgResized.convertTo(imgResized,CV_32FC1);
    cv::normalize(imgResized,imgResized,1,0,cv::NORM_MINMAX,CV_32FC1);
}
void MyImage::padInputImage()
{
    imgPadded = cv::Mat::zeros(sizeP,sizeQ,CV_32FC1);
    cv::copyMakeBorder(imgResized,              // source
                       imgPadded,               // destination
                       0,                       // top padding (zero)
                       sizeP - sizeM,           // bottom padding
                       0,                       // left padding (zero)
                       sizeQ - sizeN,           // right padding
                       cv::BORDER_CONSTANT,     // padding type (constant)
                       cv::Scalar::all(0));     // padding value (0)
    cv::normalize(imgPadded,imgPadded,1,0,cv::NORM_MINMAX,CV_32FC1);
}
void MyImage::centerInputImage()
{
    imgCheckerBoard = cv::Mat::zeros(sizeP,sizeQ,CV_32FC1);
    for(int x=0; x<sizeM; x++){
        for(int y=0; y<sizeN; y++){
            if((x+y) % 2 == 0){
                imgCheckerBoard.at<float>(x,y) = 1.0;
            }
            else{
                imgCheckerBoard.at<float>(x,y) = -1.0; // TODO : change to -1
            }
        }
    }

    imgCentered = cv::Mat::zeros(sizeP,sizeQ,CV_32FC1);
    cv::multiply(imgPadded, imgCheckerBoard, imgCentered, 1, CV_32FC1);

    // --- Manually adjust this matrix for output to conserve negative values ---
    //cv::imshow("Correctly Centered Image",imgCentered);
    double tmpMin, tmpMax;
    cv::minMaxLoc(imgCentered,&tmpMin,&tmpMax);
    imgCentered.convertTo(imgCenteredScaled,CV_32FC1,1/tmpMax);
}
void MyImage::transform2Spectrum()
{
    // --- Generate complex data matrix to hold the DFT output ---
    complexSpectrum.release();
    cv::Mat tmpPlanes[] = {cv::Mat_<float>(imgCentered), cv::Mat::zeros(imgCentered.size(), CV_32FC1)};
    cv::merge(tmpPlanes, 2, complexSpectrum);

    // --- Apply FFT via the DFT method ---
    cv::dft(complexSpectrum,complexSpectrum);

    // --- Extract real spectrum from the complex spectrum ---
    imgSpectrum = cv::Mat::zeros(sizeP, sizeQ, CV_32FC1);
    cv::split(complexSpectrum,tmpPlanes);
    cv::magnitude(tmpPlanes[0], tmpPlanes[1], imgSpectrum);

    // --- Adjust real spectrum scaling for output display ---
    imgSpectrum += cv::Scalar::all(1);
    cv::log(imgSpectrum,imgSpectrum);
    cv::normalize(imgSpectrum, imgSpectrum, 1, 0, cv::NORM_MINMAX, CV_32FC1);
}
void MyImage::filterImage(int filterSelector, double D0, int n)
{
    std::cout << "Applying filter: " << filterSelector
              << " with " << "(" << D0 << " x " << n << ")" << std::endl;

    // --- Build the real part of the filter mask ---
    cv::Mat tmp = cv::Mat::zeros(sizeP,sizeQ,CV_32FC1);
    double tmpU, tmpV, tmpD;
    for(int u=0; u<sizeP; u++){
        for(int v=0; v<sizeQ; v++){
            tmpU = pow(u-sizeP/2,2);
            tmpV = pow(v-sizeQ/2,2);
            tmpD = pow((tmpU + tmpV),0.5);
            switch (filterSelector) {
            case 1: // Ideal low-pass filter
                if(tmpD <= D0){
                    tmp.at<float>(u,v) = 1.0;
                }
                else{
                    tmp.at<float>(u,v) = 0.0;
                }
                break;
            case 2: // Ideal high-pass filter
                if(tmpD <= D0){
                    tmp.at<float>(u,v) = 0.0;
                }
                else{
                    tmp.at<float>(u,v) = 1.0;
                }
                break;
            case 3: // Butterworth low-pass filter
                tmp.at<float>(u,v) = 1.0 / (1.0 + pow(tmpD/D0, 2.0*n));
                break;
            case 4: // Butterworth high-pass filter
                tmp.at<float>(u,v) = 1.0 / (1.0 + pow(D0/tmpD, 2.0*n));
                break;
            case 5: // Gaussian low-pass filter
                tmp.at<float>(u,v) = 1.0 * exp(-1.0 / 2.0 * std::pow(tmpD,2.0) / std::pow(D0,2.0));
                break;
            case 6: // Third high pass filter
                tmp.at<float>(u,v) = 1.0 - 1.0 * exp(-1.0 / 2.0 * std::pow(tmpD,2.0) / std::pow(D0,2.0));
                break;
            default: // Full pass filter
                if((u==0 && v==0) ||
                   (u==0 && v==(sizeQ-1)) ||
                   (u==(sizeP-1) && v==0) ||
                   (u==(sizeP-1) && v==(sizeQ-1))) {
                tmp.at<float>(u,v) = 0.0;
                }
                else{
                tmp.at<float>(u,v) = 1.0;
                }
                break;
            }
        }
    }

    // --- Generate complex data matrix to hold the spectral mask ---
    complexMask.release();
    cv::Mat tmpPlanes[] = {cv::Mat_<float>(tmp), cv::Mat::zeros(tmp.size(), CV_32FC1)};
    cv::merge(tmpPlanes, 2, complexMask);

    // --- Extract real spectrum from the complex spectrum ---
    imgMask = cv::Mat::zeros(sizeP, sizeQ, CV_32FC1);
    cv::split(complexMask,tmpPlanes);
    cv::magnitude(tmpPlanes[0], tmpPlanes[1], imgMask);
    // --- Adjust real spectrum scaling for output display ---
    imgMask += cv::Scalar::all(1);
    cv::log(imgMask,imgMask);
    cv::normalize(imgMask, imgMask, 1, 0, cv::NORM_MINMAX, CV_32FC1);

    // --- Perform complex matrix multiplication element-wise ---
    cv::mulSpectrums(complexSpectrum,complexMask,complexFiltered, cv::DFT_ROWS, false);

    // --- Extract real spectrum from the complex spectrum ---
    imgFiltered = cv::Mat::zeros(sizeP, sizeQ, CV_32FC1);
    cv::split(complexFiltered,tmpPlanes);
    cv::magnitude(tmpPlanes[0], tmpPlanes[1], imgFiltered);

    // --- Adjust real spectrum scaling for output display ---
    imgFiltered += cv::Scalar::all(1);
    cv::log(imgFiltered,imgFiltered);
    cv::normalize(imgFiltered, imgFiltered, 1, 0, cv::NORM_MINMAX, CV_32FC1);

    transform2Spatial();
    cropFinalImage();
}
void MyImage::transform2Spatial()
{
    // --- Recover the real part of the inverse DFT/FFT transform ---
    imgRecovered = cv::Mat::zeros(sizeP,sizeQ,CV_32FC1);
    cv::dft(complexFiltered, imgRecovered, cv::DFT_INVERSE|cv::DFT_REAL_OUTPUT);

    // --- Reverse the centering transform with the checkerboard matrix ---
    cv::multiply(imgRecovered, imgCheckerBoard, imgRecovered, 1, CV_32FC1);
    cv::normalize(imgRecovered, imgRecovered, 1, 0, cv::NORM_MINMAX, CV_32FC1);
}
void MyImage::cropFinalImage()
{
    imgCropped = cv::Mat::zeros(sizeP,sizeQ,CV_32FC1);
    cv::Rect myROI(0,0,sizeN,sizeM);
    imgRecovered(myROI).copyTo(imgCropped);
    cv::normalize(imgCropped, imgCropped, 1, 0, cv::NORM_MINMAX, CV_32FC1);
}
