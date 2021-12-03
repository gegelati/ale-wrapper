#define _USE_MATH_DEFINES // To get M_PI
#include <math.h>

#include "instructions.h"

void fillInstructionSet(Instructions::Set& set) {
    auto minus = [](double a, double b) -> double { return a - b; };
    auto add = [](double a, double b) -> double { return a + b; };
    auto times = [](double a, double b) -> double { return a * b; };
    auto divide = [](double a, double b) -> double { return a / b; };
    auto cond = [](double a, double b) -> double { return a < b ? -a : a; };
    auto cos = [](double a) -> double { return std::cos(a); };
    auto ln = [](double a) -> double { return std::log(a); };
    auto exp = [](double a) -> double { return std::exp(a); };

    set.add(*(new Instructions::LambdaInstruction<double, double>(minus, "$0 = $1 - $2;")));
    set.add(*(new Instructions::LambdaInstruction<double, double>(add, "$0 = $1 + $2;")));
    set.add(*(new Instructions::LambdaInstruction<double, double>(times, "$0 = $1 * $2;")));
    set.add(*(new Instructions::LambdaInstruction<double, double>(divide, "$0 = $1 / $2;")));
    set.add(*(new Instructions::LambdaInstruction<double, double>(cond, "$0 = (($1) < ($2)) ? -($1) : ($1);")));
    set.add(*(new Instructions::LambdaInstruction<double>(cos, "$0 = cos($1);")));
    set.add(*(new Instructions::LambdaInstruction<double>(ln, "$0 = log($1);")));
    set.add(*(new Instructions::LambdaInstruction<double>(exp, "$0 = exp($1);")));
}