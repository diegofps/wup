#ifndef OPENCVUTILS_HPP
#define OPENCVUTILS_HPP

#include <wup/common/generic.hpp>
#include <wup/common/bundle.hpp>
#include <opencv2/opencv.hpp>
#include <vector>

namespace wup
{

const uchar KEY_NUMPAD_ON_1 = 177;
const uchar KEY_NUMPAD_ON_2 = 178;
const uchar KEY_NUMPAD_ON_3 = 179;
const uchar KEY_NUMPAD_ON_4 = 180;
const uchar KEY_NUMPAD_ON_5 = 181;
const uchar KEY_NUMPAD_ON_6 = 182;
const uchar KEY_NUMPAD_ON_7 = 183;
const uchar KEY_NUMPAD_ON_8 = 184;
const uchar KEY_NUMPAD_ON_9 = 185;
const uchar KEY_NUMPAD_ON_0 = 176;
const uchar KEY_NUMPAD_ON_DOT = 172;

const uchar KEY_NUMPAD_OFF_END = 156;
const uchar KEY_NUMPAD_OFF_DOWN = 153;
const uchar KEY_NUMPAD_OFF_PGDOWN = 155;
const uchar KEY_NUMPAD_OFF_LEFT = 150;
const uchar KEY_NUMPAD_OFF_5 = 157;
const uchar KEY_NUMPAD_OFF_RIGHT = 152;
const uchar KEY_NUMPAD_OFF_HOME = 149;
const uchar KEY_NUMPAD_OFF_UP = 151;
const uchar KEY_NUMPAD_OFF_PGUP = 154;
const uchar KEY_NUMPAD_OFF_0 = 158;
const uchar KEY_NUMPAD_OFF_DOT = 159;

const uchar KEY_NUMPAD_DIV = 175;
const uchar KEY_NUMPAD_MUL = 170;
const uchar KEY_NUMPAD_MINUS = 173;
const uchar KEY_NUMPAD_PLUS = 171;
const uchar KEY_NUMPAD_ENTER = 141;

const uchar KEY_NONE = 255;

const uchar KEY_ESC = 27;
const uchar KEY_SPACE = 32;
const uchar KEY_ENTER = 10;
const uchar KEY_BACKSPACE = 8;

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

// CLASS SIMILAR TO Rect2i, REPRESENTS A REGION OF INTEREST
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

    void
    updateSizeButKeepCenter(const int newHeight, const int newWidth)
    {
        y = y + height / 2 - newHeight / 2;
        x = x + width / 2 - newWidth / 2;
        height = newHeight;
        width  = newWidth;
    }

    void
    projectTo(Region & dst, cv::Size2i & srcSize, cv::Size2i & dstSize) const
    {
        dst.x = x * dstSize.width / srcSize.width;
        dst.y = y * dstSize.height / srcSize.height;
        dst.width  = width * dstSize.width / srcSize.width;
        dst.height = height * dstSize.height / srcSize.height;
    }

    template <typename RECT>
    void
    toRect(RECT & r)
    {
        r.x = x;
        r.y = y;
        r.width = width;
        r.height = height;
    }
    
    template <typename RECT>
    void
    fromRect(const RECT & r)
    {
        x = r.x;
        y = r.y;
        width = r.width;
        height = r.height;
    }
    
};

std::ostream & operator<<(std::ostream & o, const Region & r)
{
    o << "(" << r.x << "," << r.y << " " << r.width << "," << r.height << ")";
    return o;
}

template <typename T, typename INDEX>
inline uint
sumRegion(const T & view,
          const INDEX & i1, const INDEX & j1,
          const INDEX & i2, const INDEX & j2)
{
    return view(i2, j2) + view(i1, j1) - view(i2, j1) - view(i1, j2);
}

void
calculateImageIntegral(const cv::Mat & image, wup::Bundle<uint> & ii)
{
    ii.reshape(image.rows, image.cols);

    const uchar * const final = &image.at<uchar>(0, image.cols);
    const uchar * j = &image.at<uchar>(0, 0);
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

template <typename T1, typename T2>
void
projectPoint(const T1 & src, T2 & dst,
             const cv::Size2i & sizeSrc, const cv::Size2i & sizeDst)
{
    dst.x = src.x * sizeDst.width / sizeSrc.width;
    dst.y = src.y * sizeDst.height / sizeSrc.height;
}

void
__selectPoints(int event, int x, int y, int flags, void * userdata)
{
    std::vector<cv::Point2i> * points = static_cast<std::vector<cv::Point2i>*>(userdata);

    if  ( event == cv::EVENT_LBUTTONDOWN )
        points->push_back(cv::Point2i(x, y));

    (*points)[0].x = x;
    (*points)[0].y = y;
}

bool
selectROI(const char * window, cv::Mat & img, Region & roi)
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
            cv::setMouseCallback(window, nullptr, nullptr);
            return false;
        }
    }

    const int x0 = wmin(points[1].x, points[2].x);
    const int x1 = wmax(points[1].x, points[2].x);

    const int y0 = wmin(points[1].y, points[2].y);
    const int y1 = wmax(points[1].y, points[2].y);

    roi.x = x0;
    roi.y = y0;
    roi.width = x1 - x0;
    roi.height = y1 - y0;

    cv::setMouseCallback(window, nullptr, nullptr);
    return true;
}

void
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
