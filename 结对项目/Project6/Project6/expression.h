

#include <memory>
#include <string>
#include <unordered_set>
#include "fraction.h"

class Expression {
public:
    virtual ~Expression() = default;
    virtual Fraction evaluate() const = 0;
    virtual std::string toString(bool bracket = false) const = 0;
    virtual std::string canonicalForm() const = 0;
};

class Number : public Expression {
    Fraction value;
public:
    Number(int num, int den = 1) : value(num, den) {}
    Fraction evaluate() const override { return value; }
    std::string toString(bool) const override { return value.toString(); }
    std::string canonicalForm() const override { return value.toString(); }
};

class BinaryExpression : public Expression {
    char op;
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;

public:
    BinaryExpression(char o, std::unique_ptr<Expression> l, std::unique_ptr<Expression> r)
        : op(o), left(std::move(l)), right(std::move(r)) {}

    Fraction evaluate() const override {
        auto l = left->evaluate();
        auto r = right->evaluate();
        switch (op) {
        case '+': return l + r;
        case '-': return l - r;
        case '*': return l * r;
        case '/': return l / r;
        default: throw std::runtime_error("Invalid operator");
        }
    }

    std::string toString(bool bracket = false) const override {
        std::string s;
        if (bracket) s += "(";
        s += left->toString(needBracket(left.get()))
            + " " + op + " "
            + right->toString(needBracket(right.get()));
        if (bracket) s += ")";
        return s;
    }

    std::string canonicalForm() const override {
        if (op == '+' || op == '*') {
            auto leftCanon = left->canonicalForm();
            auto rightCanon = right->canonicalForm();
            if (leftCanon > rightCanon)
                std::swap(leftCanon, rightCanon);
            return leftCanon + " " + op + " " + rightCanon;
        }
        return left->canonicalForm() + " " + op + " " + right->canonicalForm();
    }

private:
    bool needBracket(const Expression* expr) const {
        auto be = dynamic_cast<const BinaryExpression*>(expr);
        return be && getPriority(be->op) < getPriority(op);
    }

    int getPriority(char op) const {
        switch (op) {
        case '+': case '-': return 1;
        case '*': case '/': return 2;
        default: return 0;
        }
    }
};
