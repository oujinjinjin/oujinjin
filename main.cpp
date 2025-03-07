
#include <cstdint>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <iomanip>




using namespace std;

vector<unsigned char> read_bytes(const string& filename) {
    ifstream file(filename, ios::binary | ios::ate);
    if (!file) {
        cerr << "Error opening file: " << filename << endl;
        exit(1);
    }
    streamsize size = file.tellg();
    file.seekg(0, ios::beg);

    vector<unsigned char> bytes(size);
    if (!file.read(reinterpret_cast<char*>(bytes.data()), size)) {
        cerr << "Error reading file: " << filename << endl;
        exit(1);
    }
    return bytes;
}

vector<uint32_t> utf8_to_codepoints(const vector<unsigned char>& bytes) {
    vector<uint32_t> codepoints;
    size_t i = 0;
    while (i < bytes.size()) {
        unsigned char byte = bytes[i];
        if (byte <= 0x7F) {
            codepoints.push_back(byte);
            i += 1;
        }
        else if ((byte & 0xE0) == 0xC0) {
            if (i + 1 >= bytes.size()) break; // 跳过不完整字符
            uint32_t cp = ((byte & 0x1F) << 6) | (bytes[i + 1] & 0x3F);
            codepoints.push_back(cp);
            i += 2;
        }
        else if ((byte & 0xF0) == 0xE0) {
            if (i + 2 >= bytes.size()) break;
            uint32_t cp = ((byte & 0x0F) << 12) | ((bytes[i + 1] & 0x3F) << 6) | (bytes[i + 2] & 0x3F);
            codepoints.push_back(cp);
            i += 3;
        }
        else if ((byte & 0xF8) == 0xF0) {
            if (i + 3 >= bytes.size()) break;
            uint32_t cp = ((byte & 0x07) << 18) | ((bytes[i + 1] & 0x3F) << 12) | ((bytes[i + 2] & 0x3F) << 6) | (bytes[i + 3] & 0x3F);
            codepoints.push_back(cp);
            i += 4;
        }
        else {
            i += 1; // 跳过无效字节
        }
    }
    return codepoints;
}

int lcs(const vector<uint32_t>& s1, const vector<uint32_t>& s2) {
    int m = s1.size();
    int n = s2.size();
    if (m == 0 || n == 0) return 0;

    if (m < n) return lcs(s2, s1);

    vector<int> dp(n + 1, 0);
    for (int i = 1; i <= m; ++i) {
        int prev = 0;
        for (int j = 1; j <= n; ++j) {
            int temp = dp[j];
            if (s1[i - 1] == s2[j - 1]) {
                dp[j] = prev + 1;
            }
            else {
                dp[j] = max(dp[j], dp[j - 1]);
            }
            prev = temp;
        }
    }
    return dp[n];
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Usage: " << argv[0] << " original.txt plagiarized.txt output.txt\n";
        return 1;
    }

    string original_path = argv[1];
    string plagiarized_path = argv[2];
    string output_path = argv[3];

    auto original_bytes = read_bytes(original_path);
    auto s1 = utf8_to_codepoints(original_bytes);

    auto plagiarized_bytes = read_bytes(plagiarized_path);
    auto s2 = utf8_to_codepoints(plagiarized_bytes);

    int lcs_len = lcs(s1, s2);
    double rate = 0.0;
    if (!s1.empty()) {
        rate = (static_cast<double>(lcs_len) / s1.size()) * 100.0;
    }

    ofstream outfile(output_path);
    if (!outfile) {
        cerr << "Error opening output file: " << output_path << endl;
        return 1;
    }
    outfile << fixed << setprecision(2) << rate;

    return 0;
}