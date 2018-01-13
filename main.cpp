#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

double getPSNR(const Mat& I1, const Mat& I2);
double getMSSIM( const Mat& i1, const Mat& i2);


int main(int argc, char** argv ) {
    //check args
    if(argc<4) return 0;
    //get video path
    char videoPath[200];
    sprintf(videoPath, "%s", argv[1]);
    //get thresh
    double thresh;
    thresh = atof(argv[3]);
    //get method (0 represents for PSNR, 1 represents for MSSIM
    int method = atoi(argv[2]);
    //video reader
    VideoCapture capture = VideoCapture(videoPath);
    if( !capture.isOpened() ){
        throw "Error when reading video";
    }
    //two frames used for comparison
    Mat frameA, frameB;
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
            double diff;
            if(method==0){
                diff = getPSNR(frameA, frameB);
            }else{
                diff = getMSSIM(frameA, frameB);
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

double getMSSIM( const Mat& i1, const Mat& i2)
{
    const double C1 = 6.5025, C2 = 58.5225;
    /***************************** INITS **********************************/
    int d     = CV_32F;

    Mat I1, I2;
    i1.convertTo(I1, d);           // cannot calculate on one byte large values
    i2.convertTo(I2, d);

    Mat I2_2   = I2.mul(I2);        // I2^2
    Mat I1_2   = I1.mul(I1);        // I1^2
    Mat I1_I2  = I1.mul(I2);        // I1 * I2

    /***********************PRELIMINARY COMPUTING ******************************/

    Mat mu1, mu2;   //
    GaussianBlur(I1, mu1, Size(11, 11), 1.5);
    GaussianBlur(I2, mu2, Size(11, 11), 1.5);

    Mat mu1_2   =   mu1.mul(mu1);
    Mat mu2_2   =   mu2.mul(mu2);
    Mat mu1_mu2 =   mu1.mul(mu2);

    Mat sigma1_2, sigma2_2, sigma12;

    GaussianBlur(I1_2, sigma1_2, Size(11, 11), 1.5);
    sigma1_2 -= mu1_2;

    GaussianBlur(I2_2, sigma2_2, Size(11, 11), 1.5);
    sigma2_2 -= mu2_2;

    GaussianBlur(I1_I2, sigma12, Size(11, 11), 1.5);
    sigma12 -= mu1_mu2;

    ///////////////////////////////// FORMULA ////////////////////////////////
    Mat t1, t2, t3;

    t1 = 2 * mu1_mu2 + C1;
    t2 = 2 * sigma12 + C2;
    t3 = t1.mul(t2);              // t3 = ((2*mu1_mu2 + C1).*(2*sigma12 + C2))

    t1 = mu1_2 + mu2_2 + C1;
    t2 = sigma1_2 + sigma2_2 + C2;
    t1 = t1.mul(t2);               // t1 =((mu1_2 + mu2_2 + C1).*(sigma1_2 + sigma2_2 + C2))

    Mat ssim_map;
    divide(t3, t1, ssim_map);      // ssim_map =  t3./t1;

    Scalar mssim = mean( ssim_map ); // mssim = average of ssim map
    return (mssim[0]+mssim[1]+mssim[2])/3;
}