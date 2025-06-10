//	c0xHeaders.h - Some c++0x stuff.
#ifndef C0XHEADERS_H_
#define C0XHEADERS_H_

///	Here we have code which brings in the correct versions of shared_ptr etc.
/*!
	This is necessary because XCode (whichever version it is I'm using, anyway)
	doesn't have c++11 support, meaning we need to use boost.
 */

#include <memory>

#ifdef __APPLE__
#include <boost/pointer_cast.hpp>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#else
#include <array>
#endif

//------------------------------------------------------------------------------
#ifdef __APPLE__
using boost::array;
using boost::shared_ptr;
using boost::weak_ptr;
using boost::dynamic_pointer_cast;
#else
using std::array;
using std::shared_ptr;
using std::weak_ptr;
using std::dynamic_pointer_cast;
#endif

#endif
