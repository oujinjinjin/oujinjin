#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include "fraction.h"
#include "generator.h"
#include "evaluator.h"

using namespace std;

// 帮助信息
void printHelp() {
    cout << "四则运算题目生成与评分系统\n"
        << "用法：\n"
        << "  生成模式： program -n <数量> -r <范围>\n"
        << "  评分模式： program -e <题目文件> -a <答案文件>\n\n"
        << "选项说明：\n"
        << "  -n, --number    生成题目数量 (1-10000)\n"
        << "  -r, --range     数值范围（自然数/分母，≥1）\n"
        << "  -e, --exercise  题目文件路径\n"
        << "  -a, --answer    答案文件路径\n"
        << "  -h, --help      显示本帮助信息\n";
}

// 配置参数结构体
struct Config {
    enum Mode { GENERATE, CHECK } mode;
    int number = 0;
    int range = 0;
    string exerciseFile;
    string answerFile;
};

// 参数解析
Config parseArguments(int argc, char* argv[]) {
    Config config;
    vector<string> args(argv + 1, argv + argc);

    for (size_t i = 0; i < args.size(); ++i) {
        const string& arg = args[i];

        if (arg == "-h" || arg == "--help") {
            printHelp();
            exit(0);
        }
        else if (arg == "-n" || arg == "--number") {
            if (++i >= args.size()) throw runtime_error("缺少 -n 参数值");
            config.number = stoi(args[i]);
            config.mode = Config::GENERATE;
        }
        else if (arg == "-r" || arg == "--range") {
            if (++i >= args.size()) throw runtime_error("缺少 -r 参数值");
            config.range = stoi(args[i]);
        }
        else if (arg == "-e" || arg == "--exercise") {
            if (++i >= args.size()) throw runtime_error("缺少 -e 参数值");
            config.exerciseFile = args[i];
            config.mode = Config::CHECK;
        }
        else if (arg == "-a" || arg == "--answer") {
            if (++i >= args.size()) throw runtime_error("缺少 -a 参数值");
            config.answerFile = args[i];
        }
        else {
            throw runtime_error("未知参数: " + arg);
        }
    }

    // 验证参数组合
    if (config.mode == Config::GENERATE) {
        if (config.number < 1 || config.number > 10000)
            throw runtime_error("题目数量必须为1-10000");
        if (config.range < 1)
            throw runtime_error("数值范围必须≥1");
    }
    else {
        if (config.exerciseFile.empty() || config.answerFile.empty())
            throw runtime_error("必须指定题目文件和答案文件");
    }

    return config;
}

// 生成题目和答案文件
void generateProblems(int count, int range) {
    ProblemGenerator generator(range);
    vector<string> exercises;
    vector<string> answers;

    for (int i = 0; i < count; ++i) {
        try {
            auto expr = generator.generate();
            exercises.push_back(expr->toString() + " = ");
            answers.push_back(expr->evaluate().toString());
        }
        catch (const exception& e) {
            cerr << "题目生成失败: " << e.what() << endl;
        }
    }

    // 写入文件
    ofstream exFile("Exercises.txt"), ansFile("Answers.txt");
    if (!exFile || !ansFile) throw runtime_error("无法创建输出文件");

    for (const auto& ex : exercises) exFile << ex << '\n';
    for (const auto& ans : answers) ansFile << ans << '\n';
}

// 检查答案并生成评分报告
void checkAnswers(const string& exFile, const string& ansFile) {
    ifstream exercises(exFile), answers(ansFile);
    if (!exercises) throw runtime_error("无法打开题目文件: " + exFile);
    if (!answers) throw runtime_error("无法打开答案文件: " + ansFile);

    vector<int> wrongList;
    string problem, userAnswer;
    int lineNumber = 0;

    while (getline(exercises, problem) && getline(answers, userAnswer)) {
        lineNumber++;

        try {
            // 解析题目表达式
            size_t eqPos = problem.find('=');
            if (eqPos == string::npos) {
                throw runtime_error("题目格式错误");
            }
            string expr = problem.substr(0, eqPos);
            Fraction correct = ExpressionEvaluator::evaluate(expr);

            // 解析用户答案
            Fraction user = Fraction::parse(userAnswer);

            if (correct != user) {
                wrongList.push_back(lineNumber);
            }
        }
        catch (const exception& e) {
            cerr << "第" << lineNumber << "行处理错误: " << e.what() << endl;
            wrongList.push_back(lineNumber);
        }
    }

    // 生成评分报告
    ofstream gradeFile("Grade.txt");
    if (!gradeFile) throw runtime_error("无法创建评分文件");

    // 计算正确题号
    vector<int> correctList;
    for (int i = 1; i <= lineNumber; ++i) {
        if (find(wrongList.begin(), wrongList.end(), i) == wrongList.end()) {
            correctList.push_back(i);
        }
    }

    gradeFile << "Correct: " << correctList.size() << " (";
    for (size_t i = 0; i < correctList.size(); ++i) {
        gradeFile << correctList[i];
        if (i != correctList.size() - 1) gradeFile << ", ";
    }
    gradeFile << ")\n";

    gradeFile << "Wrong: " << wrongList.size() << " (";
    for (size_t i = 0; i < wrongList.size(); ++i) {
        gradeFile << wrongList[i];
        if (i != wrongList.size() - 1) gradeFile << ", ";
    }
    gradeFile << ")\n";
}

int main(int argc, char* argv[]) {
    try {
        Config config = parseArguments(argc, argv);

        if (config.mode == Config::GENERATE) {
            generateProblems(config.number, config.range);
            cout << "成功生成 " << config.number << " 道题目，范围 " << config.range << endl;
        }
        else {
            checkAnswers(config.exerciseFile, config.answerFile);
            cout << "答案校验完成，结果已保存到 Grade.txt" << endl;
        }
    }
    catch (const exception& e) {
        cerr << "错误: " << e.what() << endl;
        printHelp();
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}