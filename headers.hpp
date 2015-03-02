
#ifndef HEADRES_HPP
#define HEADRES_HPP


//
// video links
//
// http://linuxtv.org/downloads/v4l-dvb-apis/capture-example.html
// http://www.linuxconsulting.ro/picturegrabber/
// http://habrahabr.ru/post/212531/

// fourcc = cv2.cv.CV_FOURCC(*'XVID')#cv2.VideoWriter_fourcc(*'XVID')
// http://docs.opencv.org/trunk/doc/py_tutorials/py_gui/py_video_display/py_video_display.html


#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <exception>
#include <stdexcept>
#include <stack>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <stdexcept>

#include <cassert>
#include <cstdio>



class CBaseNode;
template <typename TPtr>
struct State {
	TPtr node;
	size_t length;
	char ch;
};


template <typename TPtr>
struct Ret {
	TPtr new_node;
	TPtr internal_node; // exist node
};


#endif // HEADRES_HPP



