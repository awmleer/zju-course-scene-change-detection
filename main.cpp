#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

void a(char* videoPath);
double getPSNR(const Mat& I1, const Mat& I2);

int main(int argc, char** argv ) {
    if(argc<3) return 0;
    //handle video
    char videoPath[200];
    sprintf(videoPath, "%s", argv[1]);
    double thresh;
    thresh = atof(argv[2]);
    VideoCapture capture = VideoCapture(videoPath);
    Mat frameA, frameB;
    if( !capture.isOpened() ){
        throw "Error when reading avi";
    }
    long count=0;
    int cooling = 0;
    while(true) {
        count++;
        if (count % 2) {
            capture >> frameA;
            if (frameA.empty())
                break;
        } else {
            capture >> frameB;
            if (frameB.empty())
                break;
        }
        if (count > 1) {
            double diff = getPSNR(frameA, frameB);
            if (cooling){

            }
            if (diff > thresh) {
                if(!cooling){
                    cout << "[" << count << "] " << diff << endl;
                }
                cooling=10;
            }else{
                if(cooling){
                    cooling--;
                }
            }
        }
    }
    return 0;
}

double getPSNR(const Mat& I1, const Mat& I2) {
    Mat s1;
    absdiff(I1, I2, s1);       // |I1 - I2|
    s1.convertTo(s1, CV_32F);  // cannot make a square on 8 bits
    s1 = s1.mul(s1);           // |I1 - I2|^2

    Scalar s = sum(s1);         // sum elements per channel

    double sse = s.val[0] + s.val[1] + s.val[2]; // sum channels
//    cout<<sse<<endl;
    if( sse <= 1e-10) // for small values return zero
        return 0;
    else
    {
        double  mse =sse /(double)(I1.channels() * I1.total());
        double psnr = 10.0*log10((255*255)/mse);
        return psnr;
    }
}