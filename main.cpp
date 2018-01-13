#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

double getHIST( const Mat& i1, const Mat& i2);
double getPSNR(const Mat& i1, const Mat& i2);


int main(int argc, char** argv ) {
    //check args
    if(argc<4) return 0;
    //get video path
    char videoPath[200];
    sprintf(videoPath, "%s", argv[1]);
    //get thresh
    double thresh;
    thresh = atof(argv[3]);
    //get method (0 represents for HIST, 1 represents for PSNR
    int method = atoi(argv[2]);
    //video reader
    VideoCapture capture = VideoCapture(videoPath);
    if( !capture.isOpened() ){
        throw "Error when reading video";
    }
    //two frames used for comparison
    Mat frameA, frameB;
    long count=0;
    int cooling = 0;//cooling is used for prevent continuous detection output
    while(true) {
        count++;
        //read frame
        if (count % 2) {
            capture >> frameA;
            if (frameA.empty())
                break;
        } else {
            capture >> frameB;
            if (frameB.empty())
                break;
        }
        if (count > 1) {//if it is the first frame, then skip
            double diff;
            if(method==0){
                diff = getHIST(frameA, frameB);
            }else{
                diff = getPSNR(frameA, frameB);
            }
//            cout<<diff<<endl;
            if (diff < thresh) {
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


double getHIST( const Mat& i1, const Mat& i2){
    //set some parameters
    int channels[] = { 0, 1 };
    int histSize[] = { 50, 60 };
    float h_ranges[] = { 0, 180 };
    float s_ranges[] = { 0, 256 };
    const float* ranges[] = { h_ranges, s_ranges };
    Mat hsv1, hsv2;
    MatND hist1, hist2;
    //convert to HSV
    cvtColor( i1, hsv1, COLOR_BGR2HSV );
    cvtColor( i2, hsv2, COLOR_BGR2HSV );
    //calculate histograms
    calcHist( &hsv1, 1, channels, Mat(), hist1, 2, histSize, ranges, true, false );
    calcHist( &hsv2, 1, channels, Mat(), hist2, 2, histSize, ranges, true, false );
    //normalize
    normalize( hist1, hist1, 0, 1, NORM_MINMAX, -1, Mat() );
    normalize( hist2, hist2, 0, 1, NORM_MINMAX, -1, Mat() );
    //compare
    return compareHist(hist1, hist2, 0);
}


double getPSNR(const Mat& i1, const Mat& i2) {
    Mat s1;
    //calculate diff
    absdiff(i1, i2, s1);
    s1.convertTo(s1, CV_32F);
    //|i1 - i2|^2
    s1 = s1.mul(s1);
    //sum pixels for each channel
    Scalar s = sum(s1);
    double ss = s.val[0] + s.val[1] + s.val[2]; // sum channels
    if( ss <= 1e-10){ // for small values return zero
        return 0;
    }else{
        //calculate psnr
        double  mse =ss /(double)(i1.channels() * i1.total());
        double psnr = 10.0*log10((255*255)/mse);
        return psnr;
    }
}
