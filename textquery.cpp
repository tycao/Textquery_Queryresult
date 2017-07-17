#include <iostream>
#include <vector>
#include <memory>
#include <map>
#include <set>
#include <sstream>
#include <fstream>

using namespace std;


using line_no = std::vector<std::string>::size_type;

class QueryResult;  //  需要提前声明（定义）该类，然后才能被TextQuery的成员函数当作返回类型
class TextQuery
{
public:
    TextQuery(std::ifstream&);
    QueryResult query(const std::string&) const;
private:
    std::shared_ptr<std::vector<std::string>> file; //输入文件
    // 每个单词到它所在的行号的集合的映射
    std::map<std::string, std::shared_ptr<std::set<line_no>>> wm;
};
TextQuery::TextQuery(std::ifstream& in) : file(new std::vector<std::string>())
{
    std::string text;
    while (getline(in, text))
    {
        file->push_back(text);
        int n = file->size() - 1;
        std::istringstream line(text);
        string word;
        while (line >> word)
        {
            std::shared_ptr<std::set<line_no>> &lines = wm[word];
            if (!lines)
                lines.reset(new std::set<line_no>);
            lines->insert(n);
        }
    }
}

class QueryResult
{
    friend std::ostream& print(std::ostream&, const QueryResult&);
public:
    QueryResult(std::string s, std::shared_ptr<std::set<line_no>> p, std::shared_ptr<std::vector<std::string>> f) :
        sought(s), lines(p), file(f) {}

private:
    std::string sought; //查询单词
    std::shared_ptr<std::set<line_no>> lines;   //出现的行号
    std::shared_ptr<std::vector<std::string>> file; //输入文件
};

QueryResult TextQuery::query(const std::string& sought) const
{
    static std::shared_ptr<std::set<line_no>> nodata(new std::set<line_no>);
    map<string, shared_ptr<set<line_no>>>::const_iterator lock = wm.find(sought);
    if (lock == wm.end())
        return QueryResult(sought, nodata, file);
    else
        return QueryResult(sought, lock->second, file);
}

std::ostream& print(ostream &os, const QueryResult &qr)
{
    os << qr.sought << " occurs " << qr.lines->size() << ((qr.lines->size() > 1)? " times." : " time.") << endl;
    for (auto num : *qr.lines)
        os << "\t(line " << num + 1 << ") " << *(qr.file->begin() + num) << endl;
    return os;
}

void runQuery(ifstream& infile)
{
    TextQuery tp(infile);
    while (true)
    {
        cout << "input word to look for, or q to quit: " << endl;
        string s;
        if (!(cin >> s) || s == "q") break;
        print(cout, tp.query(s)) << endl;
    }
}

int main()
{
    ifstream in("d:\\aa.txt");
    runQuery(in);
    return 0;
}
