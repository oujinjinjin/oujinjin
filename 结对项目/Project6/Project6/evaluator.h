
#include <vector>
#include <stack>
#include <sstream>
#include "fraction.h"

class ExpressionEvaluator {
    static void processOp(std::stack<Fraction>& nums, char op) {
        Fraction rhs = nums.top(); nums.pop();
        Fraction lhs = nums.top(); nums.pop();

        switch (op) {
        case '+': nums.push(lhs + rhs); break;
        case '-': nums.push(lhs - rhs); break;
        case '*': nums.push(lhs * rhs); break;
        case '/': nums.push(lhs / rhs); break;
        default: throw std::runtime_error("Î´ÖªÔËËã·û");
        }
    }

public:
    static Fraction evaluate(const std::string& expr) {
        std::istringstream iss(expr);
        std::stack<Fraction> nums;
        std::stack<char> ops;
        char c;
        int num;

        while (iss >> c) {
            if (isdigit(c) || c == '\'') {
                iss.putback(c);
                std::string token;
                while (iss >> c && (isdigit(c) || c == '/' || c == '\'')) {
                    token += c;
                }
                iss.putback(c);
                nums.push(Fraction::parse(token));
            }
            else if (c == '(') {
                ops.push(c);
            }
            else if (c == ')') {
                while (ops.top() != '(') {
                    processOp(nums, ops.top());
                    ops.pop();
                }
                ops.pop();
            }
            else if (c == '+' || c == '-' || c == '*' || c == '/') {
                while (!ops.empty() && ops.top() != '(' &&
                    ((c == '+' || c == '-') && (ops.top() == '*' || ops.top() == '/'))) {
                    processOp(nums, ops.top());
                    ops.pop();
                }
                ops.push(c);
            }
        }

        while (!ops.empty()) {
            processOp(nums, ops.top());
            ops.pop();
        }

        return nums.top();
    }
};
