#include <cstdint>    // 标准整数类型（如uint32_t）
#include <iostream>   // 输入输出流
#include <fstream>    // 文件流操作
#include <vector>     // 动态数组容器
#include <algorithm>  // 算法函数（如max）
#include <string>     // 字符串操作
#include <iomanip>    // 格式化输出（如setprecision）

using namespace std;  // 使用标准命名空间（简化代码）

/**
 * 读取文件二进制内容
 * @param filename 输入文件名
 * @return 包含文件所有字节的无符号字符向量
 */
vector<unsigned char> read_bytes(const string& filename) {
    // 以二进制模式打开文件，并将文件指针定位到末尾以获取文件大小
    ifstream file(filename, ios::binary | ios::ate);
    if (!file) {
        cerr << "Error opening file: " << filename << endl;
        exit(1);
    }

    // 获取文件大小并重置指针到文件开头
    streamsize size = file.tellg();
    file.seekg(0, ios::beg);

    // 读取全部字节内容到vector中
    vector<unsigned char> bytes(size);
    if (!file.read(reinterpret_cast<char*>(bytes.data()), size)) {
        cerr << "Error reading file: " << filename << endl;
        exit(1);
    }
    return bytes;
}

/**
 * 将UTF-8字节序列转换为Unicode码点向量
 * @param bytes UTF-8编码的字节数据
 * @return Unicode码点数组（每个元素为4字节码点）
 */
vector<uint32_t> utf8_to_codepoints(const vector<unsigned char>& bytes) {
    vector<uint32_t> codepoints;
    size_t i = 0;
    while (i < bytes.size()) {
        unsigned char byte = bytes[i];
        // 处理1字节字符（ASCII）
        if (byte <= 0x7F) {
            codepoints.push_back(byte);
            i += 1;
        }
        // 处理2字节字符（如拉丁字母扩展）
        else if ((byte & 0xE0) == 0xC0) {
            if (i + 1 >= bytes.size()) break; // 跳过不完整字符
            uint32_t cp = ((byte & 0x1F) << 6) | (bytes[i + 1] & 0x3F);
            codepoints.push_back(cp);
            i += 2;
        }
        // 处理3字节字符（如常用汉字）
        else if ((byte & 0xF0) == 0xE0) {
            if (i + 2 >= bytes.size()) break;
            uint32_t cp = ((byte & 0x0F) << 12) | ((bytes[i + 1] & 0x3F) << 6) | (bytes[i + 2] & 0x3F);
            codepoints.push_back(cp);
            i += 3;
        }
        // 处理4字节字符（如生僻字或表情符号）
        else if ((byte & 0xF8) == 0xF0) {
            if (i + 3 >= bytes.size()) break;
            uint32_t cp = ((byte & 0x07) << 18) | ((bytes[i + 1] & 0x3F) << 12) | ((bytes[i + 2] & 0x3F) << 6) | (bytes[i + 3] & 0x3F);
            codepoints.push_back(cp);
            i += 4;
        }
        // 无效字节处理（跳过）
        else {
            i += 1;
        }
    }
    return codepoints;
}

/**
 * 计算两个码点序列的最长公共子序列（LCS）长度
 * @param s1 输入序列1
 * @param s2 输入序列2
 * @return LCS长度（优化空间复杂度至O(min(m,n))）
 */
int lcs(const vector<uint32_t>& s1, const vector<uint32_t>& s2) {
    int m = s1.size();
    int n = s2.size();
    if (m == 0 || n == 0) return 0;

    // 确保s2是较短序列，以优化空间使用
    if (m < n) return lcs(s2, s1);

    // 使用滚动数组优化空间
    vector<int> dp(n + 1, 0);
    for (int i = 1; i <= m; ++i) {
        int prev = 0; // 保存左上角的值
        for (int j = 1; j <= n; ++j) {
            int temp = dp[j];
            if (s1[i - 1] == s2[j - 1]) {
                dp[j] = prev + 1; // 字符匹配，长度+1
            }
            else {
                dp[j] = max(dp[j], dp[j - 1]); // 取左或上方的最大值
            }
            prev = temp; // 更新左上角值
        }
    }
    return dp[n]; // 返回最后一个元素
}

int main(int argc, char* argv[]) {
    // 参数校验：需要原始文件、抄袭文件和输出文件三个参数
    if (argc != 4) {
        cerr << "Usage: " << argv[0] << " original.txt plagiarized.txt output.txt\n";
        return 1;
    }

    string original_path = argv[1];
    string plagiarized_path = argv[2];
    string output_path = argv[3];

    // 读取并解析两个文件的码点序列
    auto original_bytes = read_bytes(original_path);
    auto s1 = utf8_to_codepoints(original_bytes);

    auto plagiarized_bytes = read_bytes(plagiarized_path);
    auto s2 = utf8_to_codepoints(plagiarized_bytes);

    // 计算LCS长度和重复率（基于原始文本长度）
    int lcs_len = lcs(s1, s2);
    double rate = 0.0;
    if (!s1.empty()) {
        rate = (static_cast<double>(lcs_len) / s1.size()) * 100.0;
    }

    // 输出结果到文件（保留两位小数）
    ofstream outfile(output_path);
    if (!outfile) {
        cerr << "Error opening output file: " << output_path << endl;
        return 1;
    }
    outfile << fixed << setprecision(2) << rate;

    return 0;
}