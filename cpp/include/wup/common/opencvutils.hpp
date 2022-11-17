#ifndef OPENCVUTILS_HPP
#define OPENCVUTILS_HPP

#include <wup/common/bundle3d.hpp>
#include <wup/common/generic.hpp>
#include <wup/common/bundle.hpp>
#include <wup/common/math.hpp>
#include <wup/common/threads.hpp>
#include <opencv2/opencv.hpp>
#include <vector>

//#include "../../../tinyml/wisard_cmd/priorityflow.hpp"

namespace wup
{

const uchar KEY_LEFT   = 81;
const uchar KEY_UP     = 82;
const uchar KEY_RIGHT  = 83;
const uchar KEY_DOWN   = 84;
const uchar KEY_HOME   = 80;
const uchar KEY_END    = 87;
const uchar KEY_INSERT = 99;
const uchar KEY_PGUP   = 85;
const uchar KEY_PGDOWN = 86;

const uchar KEY_NUMPAD_ON_1   = 177;
const uchar KEY_NUMPAD_ON_2   = 178;
const uchar KEY_NUMPAD_ON_3   = 179;
const uchar KEY_NUMPAD_ON_4   = 180;
const uchar KEY_NUMPAD_ON_5   = 181;
const uchar KEY_NUMPAD_ON_6   = 182;
const uchar KEY_NUMPAD_ON_7   = 183;
const uchar KEY_NUMPAD_ON_8   = 184;
const uchar KEY_NUMPAD_ON_9   = 185;
const uchar KEY_NUMPAD_ON_0   = 176;
const uchar KEY_NUMPAD_ON_DOT = 172;

const uchar KEY_NUMPAD_OFF_END    = 156;
const uchar KEY_NUMPAD_OFF_DOWN   = 153;
const uchar KEY_NUMPAD_OFF_PGDOWN = 155;
const uchar KEY_NUMPAD_OFF_LEFT   = 150;
const uchar KEY_NUMPAD_OFF_5      = 157;
const uchar KEY_NUMPAD_OFF_RIGHT  = 152;
const uchar KEY_NUMPAD_OFF_HOME   = 149;
const uchar KEY_NUMPAD_OFF_UP     = 151;
const uchar KEY_NUMPAD_OFF_PGUP   = 154;
const uchar KEY_NUMPAD_OFF_0      = 158;
const uchar KEY_NUMPAD_OFF_DOT    = 159;

const uchar KEY_NUMPAD_DIV   = 175;
const uchar KEY_NUMPAD_MUL   = 170;
const uchar KEY_NUMPAD_MINUS = 173;
const uchar KEY_NUMPAD_PLUS  = 171;
const uchar KEY_NUMPAD_ENTER = 141;

const uchar KEY_NONE      = 255;

const uchar KEY_BACKSPACE = 8;
const uchar KEY_ENTER     = 13;
const uchar KEY_ESC       = 27;
const uchar KEY_SPACE     = 32;
const uchar KEY_MINUS     = 45;
const uchar KEY_EQUAL     = 61;


const uchar KEY_A = 65;
const uchar KEY_B = 66;
const uchar KEY_C = 67;
const uchar KEY_D = 68;
const uchar KEY_E = 69;
const uchar KEY_F = 70;
const uchar KEY_G = 71;
const uchar KEY_H = 72;
const uchar KEY_I = 73;
const uchar KEY_J = 74;
const uchar KEY_K = 75;
const uchar KEY_L = 76;
const uchar KEY_M = 77;
const uchar KEY_N = 78;
const uchar KEY_O = 79;
const uchar KEY_P = 80;
const uchar KEY_Q = 81;
const uchar KEY_R = 82;
const uchar KEY_S = 83;
const uchar KEY_T = 84;
const uchar KEY_U = 85;
const uchar KEY_V = 86;
const uchar KEY_W = 87;
const uchar KEY_X = 88;
const uchar KEY_Y = 89;
const uchar KEY_Z = 90;

const uchar KEY_a = 97;
const uchar KEY_b = 98;
const uchar KEY_c = 99;
const uchar KEY_d = 100;
const uchar KEY_e = 101;
const uchar KEY_f = 102;
const uchar KEY_g = 103;
const uchar KEY_h = 104;
const uchar KEY_i = 105;
const uchar KEY_j = 106;
const uchar KEY_k = 107;
const uchar KEY_l = 108;
const uchar KEY_m = 109;
const uchar KEY_n = 110;
const uchar KEY_o = 111;
const uchar KEY_p = 112;
const uchar KEY_q = 113;
const uchar KEY_r = 114;
const uchar KEY_s = 115;
const uchar KEY_t = 116;
const uchar KEY_u = 117;
const uchar KEY_v = 118;
const uchar KEY_w = 119;
const uchar KEY_x = 120;
const uchar KEY_y = 121;
const uchar KEY_z = 122;

const uchar KEY_0 = 48;
const uchar KEY_1 = 49;
const uchar KEY_2 = 50;
const uchar KEY_3 = 51;
const uchar KEY_4 = 52;
const uchar KEY_5 = 53;
const uchar KEY_6 = 54;
const uchar KEY_7 = 55;
const uchar KEY_8 = 56;
const uchar KEY_9 = 57;

} /* wup */

namespace wup
{

template <typename POINT1, typename POINT2, typename A, typename B>
void
projectPoint(POINT1 const & src,
             POINT2 & dst,
             A const fromWidth,
             A const fromHeight,
             B const toWidth,
             B const toHeight)
{
    dst.x = src.x * toWidth  / fromWidth;
    dst.y = src.y * toHeight / fromHeight;
}

template <typename POINT, typename A, typename B>
void
projectPoint(POINT & p,
             A const fromWidth,
             A const fromHeight,
             B const toWidth,
             B const toHeight)
{
    projectPoint(p, p, fromWidth, fromHeight, toWidth, toHeight);
}

template <typename RECT1, typename RECT2, typename A, typename B>
void
projectRect(RECT1 const & src,
            RECT2 & dst,
            A const fromWidth,
            A const fromHeight,
            B const toWidth,
            B const toHeight)
{
    dst.x      = src.x * toWidth  / fromWidth;
    dst.y      = src.y * toHeight / fromHeight;
    dst.width  = src.width * toWidth  / fromWidth;
    dst.height = src.height * toHeight / fromHeight;
}

template <typename RECT, typename A, typename B>
void
projectRect(RECT & rect,
            A const fromWidth,
            A const fromHeight,
            B const toWidth,
            B const toHeight)
{
    projectRect(rect, rect, fromWidth, fromHeight, toWidth, toHeight);
}

template <typename POINT1, typename POINT2, typename SIZE1, typename SIZE2>
void
projectPoint(POINT1 const & src,
             POINT2 & dst,
             SIZE1 const & fromSize,
             SIZE2 const & toSize)
{
    projectPoint(src, dst, fromSize.width, fromSize.height, toSize.width, toSize.height);
}

template <typename POINT, typename SIZE1, typename SIZE2>
void
projectPoint(POINT & point,
             SIZE1 const & fromSize,
             SIZE2 const & toSize)
{
    projectPoint(point, point, fromSize.width, fromSize.height, toSize.width, toSize.height);
}

template <typename RECT1, typename RECT2, typename SIZE1, typename SIZE2>
void
projectRect(RECT1 const & src,
            RECT2 & dst,
            SIZE1 const & fromSize,
            SIZE2 const & toSize)
{
    projectRect(src, dst, fromSize.width, fromSize.height, toSize.width, toSize.height);
}

template <typename RECT, typename SIZE1, typename SIZE2>
void
projectRect(RECT & rect,
            SIZE1 const & fromSize,
            SIZE2 const & toSize)
{
    projectRect(rect, rect, fromSize.width, fromSize.height, toSize.width, toSize.height);
}


template <typename RECT1, typename RECT2>
bool
intersectsRect(RECT1 const & r1,
               RECT2 const & r2)
{
    return (r2.x + r2.width  >= r1.x && r2.x < r1.x + r1.width ) &&
           (r2.y + r2.height >= r1.y && r2.y < r1.y + r1.height);
}

template <typename RECT, typename POINT>
bool
intersectsPoint(RECT const & r,
                POINT const & p)
{
    return (p.x >= r.x && p.x < r.x + r.width ) &&
           (p.y >= r.y && p.y < r.y + r.height);
}

template <typename RECT1, typename RECT2>
void
copyRect(RECT1 & dst,
         RECT2 const & src)
{
    dst.x      = src.x;
    dst.y      = src.y;
    dst.width  = src.width;
    dst.height = src.height;
}


// CLASS SIMILAR TO Rect2i, REPRESENTS A REGION OF INTEREST BUT ADDS CUSTOM METHODS

class Region
{
public:

    int x;
    int y;
    int width;
    int height;

public:

    Region()
    {

    }

    Region(const int w, const int h) :
        x(0), y(0), width(w), height(h)
    {

    }

    Region(const cv::Size2i & size) :
        x(0), y(0), width(size.width), height(size.height)
    {

    }

    Region(const int x, const int y, const int w, const int h) :
        x(x), y(y), width(w), height(h)
    {

    }

    cv::Point2i
    p1()
    {
        return cv::Point2i(x, y);
    }

    cv::Point2i
    p2()
    {
        return cv::Point2i(x+width, y+height);
    }

    cv::Point2i
    center()
    {
        return cv::Point2i(x+width/2, y+height/2);
    }

    int
    area()
    {
        return width * height;
    }

    void
    updateSizeButKeepCenter(const int newHeight, const int newWidth)
    {
        y      = y + height / 2 - newHeight / 2;
        x      = x + width / 2 - newWidth / 2;
        height = newHeight;
        width  = newWidth;
    }

//    void
//    projectTo(Region & dst, cv::Size2i & srcSize, cv::Size2i & dstSize) const
//    {
//        projectTo(dst, srcSize.width, srcSize.height, dstSize.width, dstSize.height);
//    }

//    template <typename A, typename B>
//    void
//    projectTo(Region & r, A fromWidth, A fromHeight, B toWidth, B toHeight) const
//    {
//        projectrect(r, )
//        r.x      = r.x * toWidth  / fromWidth;
//        r.y      = r.y * toHeight / fromHeight;
//        r.width  = r.width * toWidth  / fromWidth;
//        r.height = r.height * toHeight / fromHeight;
//    }

//    void
//    projectTo(cv::Size2i & srcSize, cv::Size2i & dstSize)
//    {
//        projectTo(*this, srcSize.width, srcSize.height, dstSize.width, dstSize.height);
//    }

//    template <typename A, typename B>
//    void
//    projectTo(A fromWidth, A fromHeight, B toWidth, B toHeight)
//    {
//        projectTo(*this, fromWidth, fromHeight, toWidth, toHeight);
//    }

    template <typename RECT>
    void
    toRect(RECT & r)
    {
        r.x      = x;
        r.y      = y;
        r.width  = width;
        r.height = height;
    }
    
    template <typename RECT>
    void
    fromRect(const RECT & r)
    {
        x      = r.x;
        y      = r.y;
        width  = r.width;
        height = r.height;
    }
    
    template <typename T1, typename T2>
    bool
    contains(const T1 & x, const T2 & y) const
    {
        return (x >= this->x && x < this->x + this->width) &&
               (y >= this->y && y < this->y + this->height);
    }

    void
    intersection(const Region & other, Region & result)
    {
        math::intersect1D(other.x, other.width, x, width, result.x, result.width);
        math::intersect1D(other.y, other.height, y, height, result.y, result.height);
    }
    
};

inline std::ostream &
operator<<(std::ostream & o, const Region & r) {
    o << "Rect(x=" << r.x << ", y=" << r.y << ", w=" << r.width << ", h=" << r.height << ")";
    return o;
}

//inline std::ostream &
//operator<<(std::ostream & o, const Region & r)
//{
//    o << "(" << r.x << "," << r.y << " " << r.width << "," << r.height << ")";
//    return o;
//}

inline double
distance(const cv::Point2i p1, const cv::Point2i p2)
{
    const int dx = p1.x - p2.x;
    const int dy = p1.y - p2.y;
    return sqrt(dx*dx + dy*dy);
}



template <typename T, typename INDEX>
inline uint
sumRegion(const T & view,
          const INDEX & i1, const INDEX & j1,
          const INDEX & i2, const INDEX & j2)
{
    return view(i2, j2) + view(i1, j1) - view(i2, j1) - view(i1, j2);
}

inline void
calculateImageIntegral1(const cv::Mat & image, wup::Bundle3D<uint> & ii)
{
    ii.reshape(image.rows, image.cols);

    const uchar * const final = &image.at<uchar>(0, image.cols);
    const uchar * j           = &image.at<uchar>(0, 0);

    uint * b = &ii(0,0);
    uint * a = b;

    *b = *j;
    ++b;
    ++j;

    while(j != final)
    {
        *b = *j + *a;
        a = b;
        ++j;
        ++b;
    }

    for (int i=1; i!=image.rows; ++i)
    {
        const uchar * const final = &image.at<uchar>(i, image.cols);
        const uchar * j = &image.at<uchar>(i, 0);
        uint * b2 = & ii(i-1, 0);
        uint * a2 = b2;

        uint * b = & ii(i, 0);
        uint * a = b;

        *b = *b2 + *j;
        ++b2;
        ++j;
        ++b;

        while(j != final)
        {
            *b = *b2 + *a - *a2 + *j;

            a2 = b2;
            a = b;
            ++b2;
            ++j;
            ++b;
        }
    }
}

inline void
calculateImageIntegral(const cv::Mat & image, wup::Bundle<uint> & ii)
{
    ii.reshape(static_cast<uint>(image.rows),
               static_cast<uint>(image.cols));

    const uint imgHeight = static_cast<uint>(image.rows);
    const uint imgWidth = static_cast<uint>(image.cols);

    const uchar * currPixel = image.data;
    const uchar * finalRowPixel = currPixel + imgWidth;
    const uchar * finalPixel = currPixel + imgWidth * imgHeight;

    uint * currPos     = ii.data();
    uint * startRowPos = currPos;
    uint * leftPos     = currPos;
    uint * diagPos;
    uint * topPos;

    // First pixel of first row
    *currPos = *currPixel;
    ++currPixel;
    ++currPos;

    // Remainder first row
    while(currPixel != finalRowPixel)
    {
        *currPos = *currPixel + *leftPos;

        leftPos = currPos;
        ++currPixel;
        ++currPos;
    }

    // All remainder rows
    while (currPixel != finalPixel)
    {
        topPos = startRowPos;
        startRowPos = currPos;
        finalRowPixel = currPixel + imgWidth;

        // First pixel in this row
        *currPos = *currPixel + *topPos;

        diagPos = topPos;
        leftPos = currPos;
        ++currPixel;
        ++currPos;
        ++topPos;

        // Remainder pixels in this row
        while (currPixel != finalRowPixel)
        {
            *currPos = *currPixel + *topPos + *leftPos - *diagPos;

            diagPos = topPos;
            leftPos = currPos;
            ++currPixel;
            ++currPos;
            ++topPos;
        }
    }
}

inline void
calculateImageIntegral3(const cv::Mat & image, wup::Bundle3D<uint> & ii)
{
    ii.reshape(static_cast<uint>(image.rows),
               static_cast<uint>(image.cols));

    const int imgHeight = image.rows;
    const int imgWidth  = image.cols;

    ii(0,0) = image.at<uchar>(0,0);

    for (int j=1;j!=imgWidth;++j)
        ii(0,j) = ii(0,j) + image.at<uchar>(0,j);

    for (int i=1;i!=imgHeight;++i)
    {
        ii(i,0) = image.at<uchar>(i,0) + ii(i-1,0);

        for (int j=1;j!=imgWidth;++j)
            ii(i,j) = image.at<uchar>(i,j) + ii(i-1,j) + ii(i,j-1) -  ii(i-1,j-1);
    }
}

inline void
calculateImageIntegral4(const cv::Mat & image, wup::Bundle3D<uint> & ii)
{
    ii.reshape(image.rows, image.cols);

    const uchar * const finalRowPixel = &image.at<uchar>(0, image.cols);
    const uchar * currPixel           = &image.at<uchar>(0, 0);

    uint * currPos = &ii(0,0);
    uint * leftPos = currPos;

    *currPos = *currPixel;
    ++currPos;
    ++currPixel;

    while(currPixel != finalRowPixel)
    {
        *currPos = *currPixel + *leftPos;
        leftPos = currPos;
        ++currPixel;
        ++currPos;
    }

    for (int i=1; i!=image.rows; ++i)
    {
        const uchar * const finalRowPixel = &image.at<uchar>(i, image.cols);
        const uchar * currentPixel = &image.at<uchar>(i, 0);

        uint * topPos = & ii(i-1, 0);
        uint * diagPos = topPos;

        uint * currPos = & ii(i, 0);
        uint * leftPos = currPos;

        *currPos = *topPos + *currentPixel;
        ++topPos;
        ++currentPixel;
        ++currPos;

        while(currentPixel != finalRowPixel)
        {
            *currPos = *topPos + *leftPos - *diagPos + *currentPixel;

            diagPos = topPos;
            leftPos = currPos;
            ++topPos;
            ++currentPixel;
            ++currPos;
        }
    }
}

inline void
calculateImageIntegral5(const cv::Mat & image, wup::Bundle3D<int> & ii)
{
    ii.reshape(static_cast<uint>(image.rows+1),
               static_cast<uint>(image.cols+1));

    cv::Mat tmp(image.cols+1, image.rows+1, CV_32SC1, ii.data());
    cv::integral(image, tmp);
}

inline void
calculateImageIntegral6(const cv::Mat & image, wup::Bundle3D<uint> & ii)
{
    ii.reshape(static_cast<uint>(image.rows),
               static_cast<uint>(image.cols));

    const int imgHeight = image.rows;
    const int imgWidth  = image.cols;

    ii(0,0) = image.at<uchar>(0,0);

    for (int j=1;j!=imgWidth;++j)
        ii(0,j) = ii(0,j) + image.at<uchar>(0,j);

    for (int i=1;i!=imgHeight;++i)
        ii(i,0) = ii(i,0) + image.at<uchar>(i,0);

    const uint numThreads = 2;
    int * poss = new int[imgHeight];

    poss[0] = imgWidth;
    for (int i=1;i!=imgHeight;++i)
        poss[i] = 1;

    wup::parallel(numThreads, numThreads, [&](const uint tid, const uint jid)
    {
        int i = tid + 1;

        while (i < imgHeight)
        {
            int & parentJ = poss[i-1];
            int & j = poss[i];

            while (j != imgWidth)
            {
                asm("");
                while(j != parentJ)
                {
                    ii(i,j) = image.at<uchar>(i,j) + ii(i-1,j) + ii(i,j-1) - ii(i-1,j-1);
                    ++j;
                }
            }

            i += numThreads;
        }
    });
}

inline void
calculateImageIntegral3D1(const cv::Mat & image, wup::Bundle3D<int> & ii)
{
    if (image.type() != CV_8UC3)
        error("The input image must have 3 color channels");

    ii.reshape(static_cast<uint>(image.rows),
               static_cast<uint>(image.cols),
               3);

    const uint imgHeight = static_cast<uint>(image.rows);
    const uint imgWidth  = static_cast<uint>(image.cols);

    const uchar * currPixel     = image.data;
    const uchar * finalRowPixel = currPixel + imgWidth * 3;
    const uchar * finalPixel    = currPixel + imgWidth * imgHeight * 3;

    int * currPos     = ii.data();
    int * startRowPos = currPos;
    int * leftPos;
    int * diagPos;
    int * topPos;

    // First pixel of first row
    currPos[0] = currPixel[0];
    currPos[1] = currPixel[1];
    currPos[2] = currPixel[2];

    leftPos = currPos;

    currPixel += 3;
    currPos   += 3;

    // Remainder first row
    while(currPixel != finalRowPixel)
    {
        currPos[0] = currPixel[0] + leftPos[0];
        currPos[1] = currPixel[1] + leftPos[1];
        currPos[2] = currPixel[2] + leftPos[2];

        leftPos = currPos;

        currPixel += 3;
        currPos   += 3;
    }

    // All remainder rows
    while (currPixel != finalPixel)
    {
        topPos        = startRowPos;
        startRowPos   = currPos;
        finalRowPixel = currPixel + imgWidth * 3;

        // First pixel in this row
        currPos[0] = currPixel[0] + topPos[0];
        currPos[1] = currPixel[1] + topPos[1];
        currPos[2] = currPixel[2] + topPos[2];

        diagPos = topPos;
        leftPos = currPos;

        currPixel += 3;
        currPos   += 3;
        topPos    += 3;

        // Remainder pixels in this row
        while (currPixel != finalRowPixel)
        {
            currPos[0] = currPixel[0] + topPos[0] + leftPos[0] - diagPos[0];
            currPos[1] = currPixel[1] + topPos[1] + leftPos[1] - diagPos[1];
            currPos[2] = currPixel[2] + topPos[2] + leftPos[2] - diagPos[2];

            diagPos = topPos;
            leftPos = currPos;

            currPixel += 3;
            currPos   += 3;
            topPos    += 3;
        }
    }
}

inline void
calculateImageIntegral3D(const cv::Mat & image, wup::Bundle3D<uint> & ii)
{
    if (image.type() != CV_8UC3)
        error("The input image must have 3 color channels");

    ii.reshape(static_cast<uint>(image.rows),
               static_cast<uint>(image.cols),
               3);

    const uint imgHeight = static_cast<uint>(image.rows);
    const uint imgWidth  = static_cast<uint>(image.cols);

    const uchar * currPixel     = image.data;
    const uchar * finalRowPixel = currPixel + imgWidth * 3;
    const uchar * finalPixel    = currPixel + imgWidth * imgHeight * 3;

    uint * currPos     = ii.data();
    uint * startRowPos = currPos;
    uint * leftPos     = currPos;
    uint * diagPos;
    uint * topPos;

    // First pixel of first row
    *currPos = *currPixel;
    ++currPixel;
    ++currPos;

    *currPos = *currPixel;
    ++currPixel;
    ++currPos;

    *currPos = *currPixel;
    ++currPixel;
    ++currPos;

    // Remainder first row
    while(currPixel != finalRowPixel)
    {
        *currPos = *currPixel + *leftPos;
        ++leftPos;
        ++currPixel;
        ++currPos;

        *currPos = *currPixel + *leftPos;
        ++leftPos;
        ++currPixel;
        ++currPos;

        *currPos = *currPixel + *leftPos;
        ++leftPos;
        ++currPixel;
        ++currPos;
    }

    // All remainder rows
    while (currPixel != finalPixel)
    {
        topPos        = startRowPos;
        startRowPos   = currPos;
        finalRowPixel = currPixel + imgWidth * 3;
        diagPos       = topPos;
        leftPos       = currPos;

        // First pixel in this row
        *currPos = *currPixel + *topPos;
        ++currPixel;
        ++currPos;
        ++topPos;

        *currPos = *currPixel + *topPos;
        ++currPixel;
        ++currPos;
        ++topPos;

        *currPos = *currPixel + *topPos;
        ++currPixel;
        ++currPos;
        ++topPos;

        // Remainder pixels in this row
        while (currPixel != finalRowPixel)
        {
            *currPos = *currPixel + *topPos + *leftPos - *diagPos;
            ++diagPos;
            ++leftPos;
            ++currPixel;
            ++currPos;
            ++topPos;

            *currPos = *currPixel + *topPos + *leftPos - *diagPos;
            ++diagPos;
            ++leftPos;
            ++currPixel;
            ++currPos;
            ++topPos;

            *currPos = *currPixel + *topPos + *leftPos - *diagPos;
            ++diagPos;
            ++leftPos;
            ++currPixel;
            ++currPos;
            ++topPos;
        }
    }
}

//void
//calculateImageIntegral3D(const cv::Mat & image, wup::Bundle3D<uint> & ii)
//{
//    if (image.dims != 3)
//        error("This method expects an RGB image");

//    ii.reshape(image.rows, image.cols, 3);

//    const uint imgHeight = image.rows;
//    const uint imgWidth = image.cols;

//    uint * topPos;
//    uint * topLeftPos;
//    uint * leftPos;
//    uint * currPos;

//    const cv::Vec3b * currPixel = &image.at<cv::Vec3b>(0, 0);
//    const cv::Vec3b * finalPixel = currPixel + imgWidth * imgHeight;
//    const cv::Vec3b * finalRowPixel = currPixel + imgWidth;

//    currPos = &ii(0, 0, 0);
//    leftPos = currPos;

//    *(currPos) = (*currPixel)[0]; currPos += 1;
//    *(currPos) = (*currPixel)[1]; currPos += 1;
//    *(currPos) = (*currPixel)[2]; currPos += 1;

//    currPixel += 1;

//    while(currPixel != finalRowPixel)
//    {
//        *(currPos) = (*currPixel)[0] + *(leftPos);
//        currPos += 1; leftPos += 1;

//        *(currPos) = (*currPixel)[1] + *(leftPos);
//        currPos += 1; leftPos += 1;

//        *(currPos) = (*currPixel)[2] + *(leftPos);
//        currPos += 1; leftPos += 1;
//    }

//    while (currPixel != finalPixel)
//    {
//        finalRowPixel = currPixel + imgWidth;
//        topPos = l

//        while(currPixel != finalRowPixel)
//        {
//            *(currPos) = (*currPixel)[0] + *(leftPos) + *(topPos) - *(topLeftPos);

//            *(currPos+1) = (*currPixel)[1];
//            *(currPos+2) = (*currPixel)[2];
//        }
//    }

//    while(currPixel != final)
//    {
//        *(currPos+0) = (*imgPixel)[0] + *(leftPos+0);
//        *(currPos+1) = (*imgPixel)[1] + *(leftPos+1);
//        *(currPos+2) = (*imgPixel)[2] + *(leftPos+2);

//        leftPos = currPos;

//        imgPixel += 1;
//        currPos += 3;
//    }

//    for (int i=1; i!=image.rows; ++i)
//    {
//        const cv::Vec3b * const final = &image.at<cv::Vec3b>(i, image.cols);
//        const cv::Vec3b * j = &image.at<cv::Vec3b>(i, 0);

//        uint * topPos = & ii(i-1, 0, 0);
//        uint * topLeftPos = topPos;

//        uint * currPos = & ii(i, 0, 0);
//        uint * leftPos = currPos;

//        *b = *b2 + *j;
//        ++topPos;
//        ++j;
//        ++currPos;

//        while(j != final)
//        {
//            *b = *b2 + *a - *a2 + *j;

//            topLeftPos = topPos;
//            leftPos = currPos;
//            ++topPos;
//            ++j;
//            ++currPos;
//        }
//    }
//}

inline void
__selectPoints(int event, int x, int y, int flags, void * userdata)
{
    std::vector<cv::Point2i> * points = static_cast<std::vector<cv::Point2i>*>(userdata);

    if  ( event == cv::EVENT_LBUTTONDOWN )
        points->push_back(cv::Point2i(x, y));

    // The first point is the hover point. It represents the current mouse
    // position and helps the drawing process.
    (*points)[0].x = x;
    (*points)[0].y = y;
}

inline bool
selectROI(const char * window,
          cv::Mat & img,
          Region & roi,
          cv::MouseCallback mouseCallback=nullptr,
          void * mouseCallbackData=nullptr,
          const int minSize=0)
{
    std::vector<cv::Point2i> points(1, cv::Point2i(0,0));
    cv::Scalar color2(255,255,255);
    cv::Scalar color1(0,0,0);
    cv::Mat img2;

    setMouseCallback(window, __selectPoints, & points);

    while(true)
    {
        img.copyTo(img2);

        if (points.size() == 2)
        {
            rectangle(img2, points[0], points[1], color1, 2);
            rectangle(img2, points[0], points[1], color2, 1);
        }

        else if (points.size() == 3)
            break;

        imshow(window, img2);

        const uchar key = cv::waitKey(16) & 0xFF;

        if (key == 27)
        {
            cv::setMouseCallback(window, mouseCallback, mouseCallbackData);
            return false;
        }
    }

    const int x0 = math::min(points[1].x, points[2].x);
    const int x1 = math::max(points[1].x, points[2].x);

    const int y0 = math::min(points[1].y, points[2].y);
    const int y1 = math::max(points[1].y, points[2].y);

    roi.x      = x0;
    roi.y      = y0;
    roi.width  = x1 - x0;
    roi.height = y1 - y0;

    cv::setMouseCallback(window, mouseCallback, mouseCallbackData);

    if (roi.width < minSize || roi.height < minSize)
        return false;
    else
        return true;
}

inline bool
selectPoints(const char * window,
             cv::Mat & img,
             std::vector<cv::Point2i> & points,
             cv::MouseCallback mouseCallback=nullptr,
             void * mouseCallbackData=nullptr)
{
    cv::Scalar color2(255,255,255);
    cv::Scalar color1(0,0,0);
    cv::Mat img2;

    points.clear();
    points.push_back(cv::Point2i(0,0));

    setMouseCallback(window, __selectPoints, & points);

    while(true)
    {
        img.copyTo(img2);

        for (auto & point : points) {
            circle(img2, point, 3, color2);
        }

        imshow(window, img2);

        const uchar key = cv::waitKey(16) & 0xFF;

        if (key == KEY_ESC)
        {
            cv::setMouseCallback(window, mouseCallback, mouseCallbackData);
            points.clear();
            return false;
        }

        else if (key == KEY_ENTER)
        {
            for (size_t i=1;i<points.size();++i)
                points[i-1] = points[i];

            cv::setMouseCallback(window, mouseCallback, mouseCallbackData);
            points.resize(points.size()-1);
            return true;
        }
    }
}

inline void
bundleToMat(wup::Bundle<uint> & src, cv::Mat & dst, const uint srcMax)
{
    if (uint(dst.rows) != src.numRows())
        wup::error("Number of rows differ");

    if (uint(dst.cols) != src.numCols())
        wup::error("Number of cols differ");

    int size = int(dst.total());
    int k = 0;

    uint  * ptr1 = src.data();
    uchar * ptr2 = dst.data;

    while (k != size)
    {
        ptr2[k] = static_cast<uchar>(ptr1[k] * 255 / srcMax);
        ++k;
    }
}

inline void
drawCross(cv::Mat & canvas,
          const uint x, const uint y,
          cv::Scalar & color)
{
    line(canvas, cv::Point(0, y), cv::Point(canvas.cols, y), color, 1);
    line(canvas, cv::Point(x, 0), cv::Point(x, canvas.rows), color, 1);
}

inline void
drawLabel(cv::Mat & canvas,
          const std::string name,
          const int x,
          const int y,
          const int width=-1,
          const int height=-1,
          const double scale=0.7,
          const cv::Scalar backColor=cv::Scalar(0,0,0),
          const cv::Scalar textColor=cv::Scalar(255,255,255),
          const int pl=5, // padding left
          const int pt=5, // padding top
          const int pr=5, // padding right
          const int pb=5, // padding bottom
          const int font=cv::FONT_HERSHEY_DUPLEX,
          const int thickness=1,
          const int lineType=cv::LINE_AA)
{

    // Get text size

    int baseline;
    cv::Size s = cv::getTextSize(name, font, scale, thickness, &baseline);
    s.width  += pl + pr;
    s.height += pt + pb;

    // These will hold the label and text coordinates

    cv::Point2i p1; // rect topleft
    cv::Point2i p2; // rect bottomright
    cv::Point2i p3; // text

    // Calculate x coordinates

    if (width == -1)
    {
        p1.x = x;
        p2.x = x + s.width;
        p3.x = p1.x + pl;
    }
    else if (s.width <= width)
    {
        p1.x = x;
        p2.x = p1.x + width;
        p3.x = p1.x + pl + (width - s.width) / 2;
    }
    else
    {
        p1.x = x + width / 2 - s.width / 2;
        p2.x = p1.x + s.width;
        p3.x = p1.x + pl;
    }

    // Calculate y coordinates

    if (height == -1)
    {
        p1.y = y;
        p2.y = y + s.height;
        p3.y = p1.y + s.height - pb;
    }
    else if (s.height <= height)
    {
        p1.y = y;
        p2.y = p1.y + height;
        p3.y = p1.y + s.height - pb + (height - s.height) / 2;
    }
    else
    {
        p1.y = y + height / 2 - s.height / 2;
        p2.y = p1.y + s.height;
        p3.y = p1.y + s.height - pb;
    }

    // Draw label background

    rectangle(canvas, p1, p2, backColor, -1);

    // Draw label text

    cv::putText(canvas, name, p3, font, scale, textColor, thickness, lineType, false);
}

template <typename RECT>
void
drawNamedRect(cv::Mat & canvas,
              char const * name,
              RECT & rect,
              cv::Scalar const & rectColor=cv::Scalar(0,0,255),
              int const rectThickness=3,
              cv::Scalar const & textColor=cv::Scalar(255,255,255),
              double const & textScale=0.3,
              int const offset=0,
              int const maxOffset=1)
{
    // Draw rect rectangle

    cv::Point p1(rect.x, rect.y);
    cv::Point p2(rect.x + rect.width, rect.y + rect.height);
    rectangle(canvas, p1, p2, rectColor, rectThickness);

    // Draw label

    int const x = rect.x + rect.width * offset / maxOffset;
    int const y = rect.y + rect.height;
    int const w = offset == -1 ? -1 : rect.width  * (offset+1) / maxOffset - (rect.width  * offset / maxOffset);

    drawLabel(canvas, name, x, y, w, 25, textScale, rectColor, textColor);
}

inline void
drawObjectRegion(cv::Mat & canvas,
               const uint scanX, const uint scanY,
               const uint scanWidth, const uint scanHeight,
               const uint iterWidth, const uint iterHeight,
               const uint stepWidth, const uint stepHeight,
               cv::Scalar & color)
{
    cv::Point p1(scanX, scanY);
    cv::Point p2(scanX + scanWidth, scanY + scanHeight);

    rectangle(canvas, p1, p2, color, 2);

    uint yy = scanY;
    for (uint i=0;i!=iterHeight;++i)
    {
        uint xx = scanX;

        for (uint j=0;j!=iterWidth;++j)
        {
            cv::Point p1h(scanX, yy);
            cv::Point p2h(scanX + scanWidth, yy);
            line(canvas, p1h, p2h, color, 1);

            cv::Point p1j(xx, scanY);
            cv::Point p2j(xx, scanY + scanHeight);
            line(canvas, p1j, p2j, color, 1);

            xx += stepWidth;
        }

        yy += stepHeight;
    }
}

} /* wup */

#endif // OPENCVUTILS_HPP
