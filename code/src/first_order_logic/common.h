#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <unordered_set>

#define UNUSED_ARG(x) ((void)x)

using Variable = std::string;
using FunctionSymbol = std::string;
using RelationSymbol = std::string;
using Arity = unsigned;
using VariablesSet = std::unordered_set<Variable>;

#endif // COMMON_H
