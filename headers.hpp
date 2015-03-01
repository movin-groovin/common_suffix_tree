
#ifndef HEADRES_HPP
#define HEADRES_HPP


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



