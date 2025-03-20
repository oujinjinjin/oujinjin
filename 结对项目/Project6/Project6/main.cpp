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

// ������Ϣ
void printHelp() {
    cout << "����������Ŀ����������ϵͳ\n"
        << "�÷���\n"
        << "  ����ģʽ�� program -n <����> -r <��Χ>\n"
        << "  ����ģʽ�� program -e <��Ŀ�ļ�> -a <���ļ�>\n\n"
        << "ѡ��˵����\n"
        << "  -n, --number    ������Ŀ���� (1-10000)\n"
        << "  -r, --range     ��ֵ��Χ����Ȼ��/��ĸ����1��\n"
        << "  -e, --exercise  ��Ŀ�ļ�·��\n"
        << "  -a, --answer    ���ļ�·��\n"
        << "  -h, --help      ��ʾ��������Ϣ\n";
}

// ���ò����ṹ��
struct Config {
    enum Mode { GENERATE, CHECK } mode;
    int number = 0;
    int range = 0;
    string exerciseFile;
    string answerFile;
};

// ��������
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
            if (++i >= args.size()) throw runtime_error("ȱ�� -n ����ֵ");
            config.number = stoi(args[i]);
            config.mode = Config::GENERATE;
        }
        else if (arg == "-r" || arg == "--range") {
            if (++i >= args.size()) throw runtime_error("ȱ�� -r ����ֵ");
            config.range = stoi(args[i]);
        }
        else if (arg == "-e" || arg == "--exercise") {
            if (++i >= args.size()) throw runtime_error("ȱ�� -e ����ֵ");
            config.exerciseFile = args[i];
            config.mode = Config::CHECK;
        }
        else if (arg == "-a" || arg == "--answer") {
            if (++i >= args.size()) throw runtime_error("ȱ�� -a ����ֵ");
            config.answerFile = args[i];
        }
        else {
            throw runtime_error("δ֪����: " + arg);
        }
    }

    // ��֤�������
    if (config.mode == Config::GENERATE) {
        if (config.number < 1 || config.number > 10000)
            throw runtime_error("��Ŀ��������Ϊ1-10000");
        if (config.range < 1)
            throw runtime_error("��ֵ��Χ�����1");
    }
    else {
        if (config.exerciseFile.empty() || config.answerFile.empty())
            throw runtime_error("����ָ����Ŀ�ļ��ʹ��ļ�");
    }

    return config;
}

// ������Ŀ�ʹ��ļ�
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
            cerr << "��Ŀ����ʧ��: " << e.what() << endl;
        }
    }

    // д���ļ�
    ofstream exFile("Exercises.txt"), ansFile("Answers.txt");
    if (!exFile || !ansFile) throw runtime_error("�޷���������ļ�");

    for (const auto& ex : exercises) exFile << ex << '\n';
    for (const auto& ans : answers) ansFile << ans << '\n';
}

// ���𰸲��������ֱ���
void checkAnswers(const string& exFile, const string& ansFile) {
    ifstream exercises(exFile), answers(ansFile);
    if (!exercises) throw runtime_error("�޷�����Ŀ�ļ�: " + exFile);
    if (!answers) throw runtime_error("�޷��򿪴��ļ�: " + ansFile);

    vector<int> wrongList;
    string problem, userAnswer;
    int lineNumber = 0;

    while (getline(exercises, problem) && getline(answers, userAnswer)) {
        lineNumber++;

        try {
            // ������Ŀ���ʽ
            size_t eqPos = problem.find('=');
            if (eqPos == string::npos) {
                throw runtime_error("��Ŀ��ʽ����");
            }
            string expr = problem.substr(0, eqPos);
            Fraction correct = ExpressionEvaluator::evaluate(expr);

            // �����û���
            Fraction user = Fraction::parse(userAnswer);

            if (correct != user) {
                wrongList.push_back(lineNumber);
            }
        }
        catch (const exception& e) {
            cerr << "��" << lineNumber << "�д������: " << e.what() << endl;
            wrongList.push_back(lineNumber);
        }
    }

    // �������ֱ���
    ofstream gradeFile("Grade.txt");
    if (!gradeFile) throw runtime_error("�޷����������ļ�");

    // ������ȷ���
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
            cout << "�ɹ����� " << config.number << " ����Ŀ����Χ " << config.range << endl;
        }
        else {
            checkAnswers(config.exerciseFile, config.answerFile);
            cout << "��У����ɣ�����ѱ��浽 Grade.txt" << endl;
        }
    }
    catch (const exception& e) {
        cerr << "����: " << e.what() << endl;
        printHelp();
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}