#ifndef OPENCVUTILS_HPP
#define OPENCVUTILS_HPP

#include <wup/common/generic.hpp>
#include <wup/common/bundle.hpp>
#include <opencv2/opencv.hpp>
#include <vector>

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

};

std::ostream & operator<<(std::ostream & o, const Region & r)
{
    o << "(" << r.x << "," << r.y << " " << r.width << "," << r.height << ")";
    return o;
}

template <typename T>
inline uint
sumRegion(const T & view,
          const uint & i1, const uint & j1,
          const uint & i2, const uint & j2)
{
    return view(i2, j2) - view(i2, j1) - view(i1, j2) + view(i1, j1);
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

    const int x0 = wup::min(points[1].x, points[2].x);
    const int x1 = wup::max(points[1].x, points[2].x);

    const int y0 = wup::min(points[1].y, points[2].y);
    const int y1 = wup::max(points[1].y, points[2].y);

    roi.x = x0;
    roi.y = y0;
    roi.width = x1 - x0;
    roi.height = y1 - y0;

    cv::setMouseCallback(window, nullptr, nullptr);
    return true;
}

} /* wup */

#endif // OPENCVUTILS_HPP
