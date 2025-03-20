// File: generator.h
#pragma once
#include <random>
#include <memory>
#include <unordered_set>
#include "expression.h"

class ProblemGenerator {
    std::mt19937 gen;
    std::unordered_set<std::string> generated;
    int max_range;

    std::unique_ptr<Expression> generateNumber() {
        std::uniform_int_distribution<> num_dist(0, max_range - 1);
        std::uniform_int_distribution<> den_dist(1, max_range - 1);

        if (std::bernoulli_distribution(0.5)(gen)) {
            return std::make_unique<Number>(num_dist(gen));
        }
        else {
            int den = den_dist(gen);
            int num = std::uniform_int_distribution<>(0, den * (max_range - 1) - 1)(gen);
            return std::make_unique<Number>(num, den);
        }
    }

    std::unique_ptr<Expression> generateExpression(int ops_left) {
        if (ops_left == 0) return generateNumber();

        std::uniform_int_distribution<> op_dist(0, 3);
        constexpr char ops[] = { '+', '-', '*', '/' };

        for (int i = 0; i < 100; ++i) { // 重试机制
            char op = ops[op_dist(gen)];
            int left_ops = std::uniform_int_distribution<>(0, ops_left - 1)(gen);
            auto left = generateExpression(left_ops);
            auto right = generateExpression(ops_left - 1 - left_ops);

            // 验证条件
            if (op == '-' && left->evaluate() < right->evaluate()) continue;
            if (op == '/' && right->evaluate() <= left->evaluate()) continue;

            auto expr = std::make_unique<BinaryExpression>(op, std::move(left), std::move(right));
            auto canon = expr->canonicalForm();

            if (generated.insert(canon).second) {
                return expr;
            }
        }
        return nullptr;
    }

public:
    ProblemGenerator(int range, unsigned seed = std::random_device{}())
        : max_range(range), gen(seed) {}

    std::unique_ptr<Expression> generate(int max_ops = 3) {
        for (int i = 0; i < 100; ++i) {
            if (auto expr = generateExpression(max_ops)) {
                return expr;
            }
        }
        throw std::runtime_error("无法生成唯一题目");
    }
};