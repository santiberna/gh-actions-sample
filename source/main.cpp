#include <algorithm>
#include <algorithm>
#include <array>
#include <cctype>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <cstdint>
#include <cstddef>
#include <functional>
#include <iostream>
#include <limits>
#include <mutex>
#include <memory>
#include <numeric>
#include <optional>
#include <queue>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

#include "util/combinations.hpp"
#include "util/graph.hpp"
#include "util/json.hpp"
#include "util/log.hpp"
#include "util/math_helpers.hpp"
#include "util/md5_hash.hpp"
#include "util/text_helpers.hpp"
#include "util/thread_pool.hpp"

namespace y2015
{

static void c1()
{
    auto input = OpenFileReadStream("input/2015/1.txt");
    if (input)
    {
        auto lines = SplitStreamIntoLines(input.value());
        bool first = false;
        int counter = 0;

        int sum = 0;

        for (auto& line : lines)
        {
            for (auto c : line)
            {
                if (c == '(') {
                    sum++;
}
                if (c == ')') {
                    sum--;
}

                counter++;
                if (sum == -1 && !first)
                {
                    first = true;
                    Log("First step to basement: {}", counter);
                }
            }
        }

        Log("Final Floor: {}", sum);
    }
    Log("Challenge One Complete");
}

static void c2()
{
    auto input = OpenFileReadStream("input/2015/2.txt");
    std::vector<Box> volumes;

    if (input)
    {
        auto lines = SplitStreamIntoLines(input.value());

        for (auto& l : lines)
        {
            auto numbers = SplitString(l, "x");

            if (numbers.size() >= 3)
            {
                Box n {};
                n.x = ParseNumber<float>(numbers[0]).value_or(0.0F);
                n.y = ParseNumber<float>(numbers[1]).value_or(0.0F);
                n.z = ParseNumber<float>(numbers[2]).value_or(0.0F);
                volumes.emplace_back(n);
            }
        }
    }

    float sum1 = 0;
    float sum2 = 0;
    for (auto& v : volumes)
    {
        sum1 += v.GetSmallestSideArea() + v.GetSurfaceArea();
        sum2 += v.GetSmallestPerimeter() + v.GetVolume();
    }

    Log("Total area of wrapping paper: {}", sum1);
    Log("Total length of ribbon: {}", sum2);
    Log("Challenge Two Complete");
}

static void c3()
{
    auto input = OpenFileReadStream("input/2015/3.txt");
    std::vector<IVec2> moves[2] { {}, {} };

    if (input)
    {
        auto text = StreamToString(input.value());
        int i = 0;

        for (auto c : text)
        {
            switch (c)
            {
            case '>':
                moves[i].emplace_back(1, 0);
                break;
            case '^':
                moves[i].emplace_back(0, 1);
                break;
            case 'v':
                moves[i].emplace_back(0, -1);
                break;
            case '<':
                moves[i].emplace_back(-1, 0);
                break;
            default:
                Log("Unindentified character {}", (int)c);
                break;
            }

            i = (i + 1) % 2;
        }
    }

    auto current = IVec2 { .x=0, .y=0 };
    std::unordered_set<IVec2> positions_reached { current };

    for (auto move : moves[0])
    {
        current = current + move;
        positions_reached.insert(current);
    }

    current = IVec2 { .x=0, .y=0 };

    for (auto move : moves[1])
    {
        current = current + move;
        positions_reached.insert(current);
    }

    Log("Houses Visited {}", positions_reached.size());
    Log("Challenge Three Complete");
}

static void c4()
{
    std::string base = "ckczppom";
    auto start = std::chrono::high_resolution_clock::now();

    std::condition_variable await_result;
    std::mutex result_mutex;
    bool run = true;

    uint32_t result {};

    auto find_hash = [&](const std::string& base, uint32_t start, uint32_t increment)
    {
        uint32_t i = start;
        while (run)
        {
            std::string const test_key = base + std::to_string(i);
            auto n = MD5Hash(StringToBytes(test_key));

            bool const check_zeros = n.as_bytes[0] == 0 && n.as_bytes[1] == 0 && n.as_bytes[2] == 0;

            if (check_zeros)
            {
                {
                    std::scoped_lock<std::mutex> const lock(result_mutex);
                    run = false;
                    result = i;
                }
                await_result.notify_one();
            }

            i += increment;
        }
    };

    for (uint32_t i = 0; i < threads.GetNumThreads(); i++)
    {
        auto incr = (uint32_t)threads.GetNumThreads();
        uint32_t start = i;

        threads.QueueTask(find_hash, base, start, incr);
    }

    // threads.QueueTask(find_hash, base, 0, 1);

    {
        std::unique_lock<std::mutex> lock { result_mutex };
        await_result.wait(lock, [&]()
            { return !run; });
    }

    auto time = std::chrono::high_resolution_clock::now() - start;
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(time);

    Log("Append number found: {} in {} ms", result, ms.count());
    Log("Challenge 4 Complete");
}

static void c5()
{
    std::vector<std::string> strings;

    if (auto input = OpenFileReadStream("input/2015/5.txt"))
    {
        strings = SplitStreamIntoLines(input.value());
    }

    // Part 1
    {
        std::vector<std::string> const bad_words { "ab", "cd", "pq", "xy" };

        auto check_word1 = [=](const std::string& t)
        {
            // Check for bad words
            for (const auto& bad : bad_words)
            {
                if (t.find(bad) != std::string::npos) {
                    return false;
}
            }

            // Twice in a row
            bool has_row = false;
            for (size_t i = 0; i < t.size() - 1; i++)
            {
                char const current = t[i];
                char const next = t[i + 1];

                if (current == next)
                {
                    has_row = true;
                    break;
                }
            }

            if (!has_row) {
                return false;
}

            // Three vowels
            size_t vowel_count = 0;
            for (auto c : t)
            {
                bool const is_vowel = c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
                if (is_vowel) {
                    vowel_count++;
}
            }

            return vowel_count >= 3;
        };
        uint32_t good = 0;
        for (auto& str : strings)
        {
            if (check_word1(str)) {
                good++;
}
        }
        Log("Number of Nice Words: {}", good);
    }

    // Part 2
    {
        auto get_pairs = [](const std::string& t)
        {
            using Pair = std::tuple<std::string, size_t>;
            std::vector<Pair> out {};
            out.reserve(t.size() - 1);
for (size_t i = 0; i < t.size() - 1; i++)
            {
                out.emplace_back( t.substr(i, 2), i );
            }
            return out;
        };

        auto get_triplets = [](const std::string& t)
        {
            using Triplet = std::tuple<char, char, char>;
            std::vector<Triplet> out {};
            for (size_t i = 0; i < t.size() - 2; i++)
            {
                char const a = t[i];
                char const b = t[i + 1];
                char const c = t[i + 2];
                out.emplace_back( a, b, c );
            }
            return out;
        };

        auto evaluate = [=](const std::string& t)
        {
            auto pairs = get_pairs(t);
            auto triples = get_triplets(t);

            // Find identical pairs

            bool found = false;
            for (const auto& [n, i] : pairs)
            {
                size_t const index = t.find(n, i + 2);
                bool const valid = index != std::string::npos;
                if (valid)
                {
                    found = true;
                    break;
                }
            }

            if (!found) {
                return false;
}

            for (const auto& [a, b, c] : triples)
            {
                if (a == c) {
                    return true;
}
            }
            return false;
        };

        uint32_t great = 0;
        for (auto& str : strings)
        {
            if (evaluate(str)) {
                great++;
}
        }

        Log("Number of Great Words: {}", great);
    }
    Log("Challenge 5 Complete");
}

static void c6()
{
    enum class Instruction
    {
        NONE,
        TOGGLE,
        ON,
        OFF
    };
    struct Entry
    {
        Instruction m_instruction;
        IVec2 m_start;
        IVec2 m_end;
    };

    std::vector<Entry> entries;
    using Grid = std::array<std::array<uint32_t, 1000>, 1000>;
    auto light_grid = std::make_unique<Grid>();

    auto parse_line = [&](const std::string& str)
    {
        auto numbers = ParseAllNumbers<int>(str);

        if (numbers.size() >= 4)
        {
            auto start = IVec2 { .x=numbers[0], .y=numbers[1] };
            auto end = IVec2 { .x=numbers[2], .y=numbers[3] };

            auto type = Instruction::NONE;

            if (str.find("toggle") != std::string::npos)
            {
                type = Instruction::TOGGLE;
            }
            else if (str.find("turn on") != std::string::npos)
            {
                type = Instruction::ON;
            }
            else if (str.find("turn off") != std::string::npos)
            {
                type = Instruction::OFF;
            }

            entries.emplace_back(type, start, end);
        }
    };

    if (auto input = OpenFileReadStream("input/2015/6.txt"))
    {
        /*std::stringstream ss;
        ss << "toggle 0,0 through 999,999" << "\n";
        ForeachLine(ss, parse_line);*/

        ForeachLine(input.value(), parse_line);
    }

    for (auto entry : entries)
    {
        switch (entry.m_instruction)
        {
        case Instruction::TOGGLE:
        {
            for (auto y = entry.m_start.y; y <= entry.m_end.y; y++) {
                for (auto x = entry.m_start.x; x <= entry.m_end.x; x++) {
                    (*light_grid)[y][x] += 2;
}
}
            break;
        }
        case Instruction::ON:
        {
            for (auto y = entry.m_start.y; y <= entry.m_end.y; y++) {
                for (auto x = entry.m_start.x; x <= entry.m_end.x; x++) {
                    (*light_grid)[y][x] += 1;
}
}
            break;
        }
        case Instruction::OFF:
        {
            for (auto y = entry.m_start.y; y <= entry.m_end.y; y++) {
                for (auto x = entry.m_start.x; x <= entry.m_end.x; x++)
                {
                    auto& ref = (*light_grid)[y][x];
                    if (ref > 0) {
                        ref -= 1;
}
                }
}
            break;
        }
        }
    }

    size_t count = 0;
    for (auto& arr : *light_grid) {
        for (auto e : arr) {
            count += e;
}
}

    Log("Total Light: {}", count);
    Log("Challenge 6 Complete");
}

static void c7()
{
    std::vector<std::string> lines;
    if (auto input = OpenFileReadStream("input/2015/7.txt"))
    {
        lines = SplitStreamIntoLines(input.value());
    }

    enum class Operation
    {
        NONE,
        AND,
        OR,
        NOT,
        RSHIFT,
        LSHIFT
    };
    struct BinaryOp
    {
        std::string m_first, m_second;
        Operation m_op;
    };
    struct UnaryOp
    {
        std::string m_var;
        Operation m_op;
    };

    using Term = std::variant<uint16_t, std::string, BinaryOp, UnaryOp>;
    std::unordered_map<std::string, Term> circuit;

    for (auto& str : lines)
    {
        auto tokens = SplitString(str, " ");
        size_t const token_count = tokens.size();

        if (token_count == 3) // n -> var
        {
            if (auto num = ParseNumber<uint16_t>(tokens.front()))
            {
                circuit.emplace(tokens.back(), num.value());
            }
            else
            {
                circuit.emplace(tokens.back(), tokens.front());
            }
        }
        else if (token_count == 4) // NOT n -> var
        {
            circuit.emplace(tokens.back(), UnaryOp { .var=tokens.at(1), .op=Operation::NOT });
        }
        else if (token_count == 5)
        {
            BinaryOp op {};
            op.m_first = tokens.front();
            op.m_second = tokens.at(2);

            auto& operand = tokens.at(1);
            if (operand == "AND")
            {
                op.m_op = Operation::AND;
            }
            else if (operand == "OR")
            {
                op.m_op = Operation::OR;
            }
            else if (operand == "RSHIFT")
            {
                op.m_op = Operation::RSHIFT;
            }
            else if (operand == "LSHIFT")
            {
                op.m_op = Operation::LSHIFT;
            }

            circuit.emplace(tokens.back(), op);
        }
    }

    struct VisitorSolver
    {
        const std::unordered_map<std::string, Term> circuit;
        std::unordered_map<std::string, uint16_t> cache;

        uint16_t solve(const std::string& var)
        {
            if (auto it = cache.find(var); it != cache.end())
            {
                return it->second;
            }
            
            
                auto& operation = circuit.find(var)->second;
                auto result = std::visit(*this, operation);
                cache.emplace(var, result);

                return result;
           
        }

        uint16_t operator()(uint16_t v) { return v; };

        uint16_t operator()(const std::string& v) { return solve(v); };

        uint16_t operator()(const UnaryOp& v)
        {
            auto result = solve(v.m_var);
            return ~(result);
        };

        uint16_t operator()(const BinaryOp& v)
        {
            uint16_t result1 {};
            uint16_t result2 {};

            if (auto n = ParseNumber<uint16_t>(v.m_first))
            {
                result1 = n.value();
            }
            else
            {
                result1 = solve(v.m_first);
            }

            if (auto n = ParseNumber<uint16_t>(v.m_second))
            {
                result2 = n.value();
            }
            else
            {
                const auto& second = (*circuit.find(v.m_second)).second;
                result2 = solve(v.m_second);
            }

            switch (v.m_op)
            {
            case Operation::AND:
            {
                return result1 & result2;
            }
            case Operation::OR:
            {
                return result1 | result2;
            }
            case Operation::LSHIFT:
            {
                return result1 << result2;
            }
            case Operation::RSHIFT:
            {
                return result1 >> result2;
            }
            default:
                return -1;
            }
        };
    };

    VisitorSolver solver { .circuit=circuit };

    std::string const target = "a";
    uint16_t result = solver.solve(target);
    Log("First Run: {}", result);

    // Rewire result -> b

    circuit["b"] = result;
    VisitorSolver solver2 { .circuit=circuit };

    uint16_t result2 = solver2.solve(target);
    Log("Second Run: {}", result2);

    Log("Challenge Seven Complete");
}

static void c8()
{
    std::vector<std::string> lines;

    if (auto in = OpenFileReadStream("input/2015/8.txt"))
    {
        lines = SplitStreamIntoLines(in.value());
    }

    // Decoding sequence

    size_t code_characters {};
    size_t string_characters {};
    for (auto& str : lines)
    {
        code_characters += str.size();
        std::string copy = str;

        // Double slashes
        copy = ReplaceAll(copy, "\\\\", "/");

        // Hexadecimal characters

        size_t it = 0;
        while ((it = copy.find("\\x")) != std::string::npos)
        {
            auto substr = copy.substr(it + 2, 2);
            auto num = ParseHex<int>(substr);

            if (num)
            {
                copy.replace(it, 4, 1, (char)(num.value()));
            }
        }

        // Escape sequences
        copy = ReplaceAll(copy, "\\\"", "\'");
        copy = copy.substr(1, copy.size() - 2);
        string_characters += copy.size();
    }

    // Encoding sequence

    size_t expanded_characters {};
    for (auto& str : lines)
    {
        std::string copy = str;
        copy = ReplaceAll(copy, "\"", "/\"");
        copy = ReplaceAll(copy, "\\", "\\\\");

        expanded_characters += copy.size() + 2;
    }

    Log("Literal Chars - String Chars = {}", code_characters - string_characters);
    Log("Expanded Chars - Literal Chars = {}",
        expanded_characters - code_characters);
    Log("Challenge Eight Complete");
}

static void c9()
{
    std::vector<std::string> lines;
    if (auto in = OpenFileReadStream("input/2015/9.txt"))
    {
        lines = SplitStreamIntoLines(in.value());
    }

    std::unordered_map<std::string, size_t> places;
    size_t count = 0;

    for (auto& str : lines)
    {
        auto tokens = SplitString(str, " ");
        if (tokens.size() >= 3)
        {
            if (auto it = places.find(tokens.front()); it == places.end())
            {
                places.emplace(tokens.front(), count);
                count++;
            }
            if (auto it = places.find(tokens[2]); it == places.end())
            {
                places.emplace(tokens[2], count);
                count++;
            }
        }
    }

    AdjacencyMatrix graph { count };

    for (auto& str : lines)
    {
        auto tokens = SplitString(str, " ");
        if (tokens.size() < 5) {
            continue;
}

        auto& from = tokens[0];
        auto& to = tokens[2];
        auto& distance = tokens[4];

        size_t const start = places[from];
        size_t const end = places[to];
        float const weight = ParseNumber<float>(distance).value();

        graph.AddUndirectedEdge(start, end, weight);
    }

    using Path = std::vector<size_t>;
    using PathArray = std::vector<Path>;

    std::function<void(PathArray & out, const Path& path,
        std::unordered_set<size_t> not_visited)>
        generate_paths;
    generate_paths = [&](PathArray& out, const Path& path,
                        const std::unordered_set<size_t>& not_visited)
    {
        if (not_visited.empty())
        {
            out.emplace_back(path);
        }
        for (auto i : not_visited)
        {
            auto copy = path;
            auto n = not_visited;

            copy.push_back(i);
            n.erase(i);

            generate_paths(out, copy, n);
        }
    };

    PathArray all_paths {};
    std::unordered_set<size_t> n;
    for (size_t i = 0; i < graph.GetSize(); i++) {
        n.insert(i);
}

    generate_paths(all_paths, {}, n);

    std::vector<float> path_distances;

    for (auto& path : all_paths)
    {
        float path_sum = 0;
        for (size_t i = 0; i < path.size() - 1; i++)
        {
            auto from = path[i];
            auto to = path[i + 1];

            path_sum += *graph.At(from, to);
        }

        path_distances.push_back(path_sum);
    }

    auto it = std::ranges::min_element(path_distances);
    auto it2 = std::ranges::max_element(path_distances);

    Log("Minimum distance for all Places: {}", *it);
    Log("Maximum distance for all Places: {}", *it2);
    Log("Challenge Nine Complete");
}

static void c10()
{
    std::string input = "3113322113";

    auto expand = [](const std::string& in)
    {
        std::vector<std::pair<char, size_t>> sequences;
        for (auto c : in)
        {
            if (sequences.empty())
            {
                sequences.emplace_back(c, 1);
            }
            else if (c != sequences.back().first)
            {
                sequences.emplace_back(c, 1);
            }
            else
            {
                sequences.back().second++;
            }
        }

        // Stringify
        std::string out {};
        for (auto [c, s] : sequences)
        {
            out += char(s) + '0';
            out += c;
        }

        return out;
    };

    for (int i = 0; i < 50; i++) {
        input = expand(input);
}

    Log("Expanded Sequence Size: {}", input.size());
    Log("Challenge Ten complete");
}

static void c11()
{
    std::string input = "hxbxwxba";

    std::vector<int> digits(input.size());
    for (int i = 0; i < input.size(); ++i)
    {
        digits.at(i) = input.at(i) - 'a';
    }

    std::function<void(std::vector<int> & digits, int rightmost)> increment;
    increment = [&](std::vector<int>& digits, int rightmost)
    {
        if (rightmost == -1) {
            return;
}
        digits.at(rightmost) = (digits.at(rightmost) + 1) % 26;

        if (digits.at(rightmost) == 0)
        {
            increment(digits, rightmost - 1);
        }
    };

    // Check straight sequences
    auto check_straight = [](const std::vector<int>& digits)
    {
        for (int i = 1; i < digits.size() - 1; ++i)
        {
            int const left_diff = digits.at(i) - digits.at(i - 1);
            int const right_diff = digits.at(i + 1) - digits.at(i);

            if (left_diff == 1 && right_diff == 1)
            {
                return true;
            }
        }
        return false;
    };

    // Check ILO
    auto check_ilo = [](const std::vector<int>& digits)
    {
        for (int current : digits)
        {
            if (current == ('i' - 'a') || current == ('l' - 'a') || current == ('o' - 'a'))
            {
                return false;
            }
        }
        return true;
    };

    // Check Pairs
    auto check_double_pair = [](const std::vector<int>& digits)
    {
        int pairs = 0;
        for (int i = 0; i < (int)digits.size() - 1; ++i)
        {
            int const current = digits.at(i);
            int const next = digits.at(i + 1);

            if (current == next)
            {
                pairs++;
                i++;
                if (pairs == 2) {
                    return true;
}
            }
        }
        return false;
    };

    auto gen_password = [](const std::vector<int>& digits)
    {
        std::string new_password;
        for (auto digit : digits)
        {
            new_password += char(digit) + 'a';
        }
        return new_password;
    };

    bool valid_password = false;
    while (!valid_password)
    {
        increment(digits, (int)(digits.size() - 1));
        valid_password = check_ilo(digits) && check_double_pair(digits) && check_straight(digits);
    }

    Log("Found First Valid Password: {}", gen_password(digits));

    valid_password = false;
    while (!valid_password)
    {
        increment(digits, (int)(digits.size() - 1));
        valid_password = check_ilo(digits) && check_double_pair(digits) && check_straight(digits);
    }

    Log("Found Second Valid Password: {}", gen_password(digits));
    Log("Challenge Eleven complete");
}

static void c12()
{
    auto input = OpenFileReadStream("input/2015/12.txt");
    if (!input)
    {
        Log("Failed To Read JSON input file");
        return;
    }

    class JSONSumVisitor
    {
    public:
        double process_value(const JSONValue& v)
        {
            return std::visit(*this, v.data);
        }

        double operator()(std::monostate /*unused*/) { return 0.0; };
        double operator()(bool /*unused*/) { return 0.0; };
        double operator()(double v) { return v; };
        double operator()() { return 0.0; };

        double operator()(const std::vector<JSONValue>& object)
        {
            double sum = 0.0F;
            for (const auto& val : object)
            {
                sum += process_value(val);
            }
            return sum;
        }
        double
        operator()(const std::unordered_map<std::string, JSONValue>& object)
        {
            double sum = 0.0F;
            for (const auto& [key, val] : object)
            {
                if (val.IsString())
                {
                    if (std::get<std::string>(val.data) == "red") {
                        return 0.0;
}
                }

                sum += process_value(val);
            }

            if (sum != sum) {
                sum = 0.0;
}
            return sum;
        };

        bool m_invalidate = false;
    };

    std::stringstream err;
    JSONParser parser(&err);

    auto result = parser.ParseValue(input.value());
    if (!result)
    {
        Log("Failed to parse JSON: {}", err.str());
        return;
    }

    Log("Sum of all JSON elements (excluding \"red\"): {}",
        JSONSumVisitor().process_value(result.value()));
    Log("Challenge Twelve Complete");
}

static void c13()
{
    std::vector<std::string> lines;

    if (auto input = OpenFileReadStream("input/2015/13.txt"))
    {
        lines = SplitStreamIntoLines(input.value());
    }

    std::unordered_set<std::string> unique_names;
    std::vector<std::tuple<std::string, int, std::string>> data;

    for (auto& line : lines)
    {
        line.pop_back(); // Pop trailing '.'
        auto tokens = SplitString(line, " ");

        if (tokens.size() < 2) {
            continue;
}

        unique_names.emplace(tokens.front());
        unique_names.emplace(tokens.back());

        int sign = -1;
        size_t number_index = -1;

        if (auto i = line.find("gain"); i != std::string::npos)
        {
            number_index = i;
            sign = 1;
        }
        else if (auto i = line.find("lose"); i != std::string::npos)
        {
            number_index = i;
            sign = -1;
        }

        std::string const n_str = line.substr(number_index + 5);
        if (auto num = ParseNumber<int>(n_str))
        {
            data.emplace_back(tokens.front(), num.value() * sign, tokens.back());
        }
    }

    std::vector<std::string> names(unique_names.begin(), unique_names.end());

    auto find_index = [](std::vector<std::string>& vec, const std::string& look)
    {
        auto it = std::ranges::find_if(vec,
            [&](const auto& e)
            { return e == look; });
        return it - vec.begin();
    };

    AdjacencyMatrix graph { names.size() };
    for (auto& [s, d, e] : data)
    {
        auto start = find_index(names, s);
        auto end = find_index(names, e);

        graph.AddDirectedEdge(start, end, static_cast<float>(d));
    }

    // graph.Print();

    using Path = std::vector<size_t>;
    using PathArray = std::vector<Path>;

    std::function<void(PathArray & out, const Path& path,
        std::unordered_set<size_t> not_visited)>
        generate_paths;
    generate_paths = [&](PathArray& out, const Path& path,
                        const std::unordered_set<size_t>& not_visited)
    {
        if (not_visited.empty())
        {
            out.emplace_back(path);
        }
        for (auto i : not_visited)
        {
            auto copy = path;
            auto n = not_visited;

            copy.push_back(i);
            n.erase(i);

            generate_paths(out, copy, n);
        }
    };

    {

        PathArray all_paths {};
        std::unordered_set<size_t> n;
        for (size_t i = 1; i < graph.GetSize(); i++) {
            n.insert(i);
}

        generate_paths(all_paths, { 0 }, n);

        std::vector<float> path_distances;

        for (auto& path : all_paths)
        {
            float path_sum = 0;
            for (size_t i = 0; i < path.size(); i++)
            {
                auto from = path[i];
                auto to = path[(i + 1) % path.size()];

                float const val = (*graph.At(from, to) + *graph.At(to, from));
                path_sum += val;
            }

            path_distances.push_back(path_sum);
        }

        auto it = std::ranges::max_element(path_distances);
        Log("Happiest Room setup provides: {} happiness", *it);
    }

    {
        PathArray all_paths {};

        // Adding myself
        graph.AddNode();
        // graph.Print();

        std::unordered_set<size_t> m;
        for (size_t i = 1; i < graph.GetSize(); i++) {
            m.insert(i);
}

        all_paths.clear();
        generate_paths(all_paths, { 0 }, m);

        std::vector<float> path_distances;
        for (auto& path : all_paths)
        {
            float path_sum = 0;
            for (size_t i = 0; i < path.size(); i++)
            {
                auto from = path[i];
                auto to = path[(i + 1) % path.size()];

                float const val = (*graph.At(from, to) + *graph.At(to, from));
                path_sum += val;
            }

            path_distances.push_back(path_sum);
        }

        auto it2 = std::ranges::max_element(path_distances);
        Log("Happiest Room setup provides (including myself): {} happiness", *it2);
    }
    Log("Challenge Thirteen Complete");
}

static void c14()
{
    struct Runner
    {
        std::string m_name;
        int m_speed, m_sprint_time, m_rest_time;

        [[nodiscard]] int calculate_distance(int seconds) const
        {
            int const cycle_time = m_rest_time + m_sprint_time;
            int const cycles_complete = seconds / cycle_time;
            int const distance_per_cycle = m_speed * m_sprint_time;
            int const remaining_seconds = seconds % cycle_time;
            int const remaining_distance = std::min(remaining_seconds, m_sprint_time) * m_speed;

            return (cycles_complete * distance_per_cycle) + remaining_distance;
        }
    };

    std::vector<Runner> all_runners;

    if (auto input = OpenFileReadStream("input/2015/14.txt"))
    {
        auto lines = SplitStreamIntoLines(input.value());
        for (auto& line : lines)
        {
            auto numbers = ParseAllNumbers<int>(line);
            if (numbers.size() >= 3)
            {
                all_runners.emplace_back(std::string(), numbers[0], numbers[1],
                    numbers[2]);
            }

            auto words = SplitString(line, " ");
            if (!words.empty() != 0u) {
                all_runners.back().m_name = words.front();
}
        }
    }

    int race_time = 2503;

    std::ranges::sort(all_runners,
        [&](const auto& lhs, const auto& rhs)
        {
            return lhs.CalculateDistance(race_time) > rhs.CalculateDistance(race_time);
        });

    Log("Race 1 - Fastest Racer: {} with a distance of {}",
        all_runners.front().m_name,
        all_runners.front().calculate_distance(race_time));

    std::vector<int> distances_travelled;
    distances_travelled.resize(all_runners.size());
    std::vector<int> score_accumulated;
    score_accumulated.resize(all_runners.size());

    for (int t = 1; t <= race_time; t++)
    {
        for (size_t r = 0; r < all_runners.size(); r++)
        {
            auto& runner = all_runners[r];
            distances_travelled[r] = runner.calculate_distance(t);
        }

        auto it = std::ranges::max_element(distances_travelled
           );
        for (size_t r = 0; r < all_runners.size(); r++)
        {
            if (distances_travelled[r] == *it) {
                score_accumulated[r]++;
}
        }
    }

    auto it = std::ranges::max_element(score_accumulated);
    size_t const w = it - score_accumulated.begin();

    Log("Race 2 - Pointiest Racer: {} with a score of {}", all_runners[w].m_name,
        *it);
    Log("Challenge Fourteen Complete");
}

static void c15()
{
    using InputMatrix = TMat<int, 5, 4>;
    InputMatrix input;

    if (auto in = OpenFileReadStream("input/2015/15.txt"))
    {
        auto lines = SplitStreamIntoLines(in.value());

        using Transposed = TMat<int, 4, 5>;
        Transposed::ArrayType data {};

        size_t r = 0;
        for (const auto& line : lines)
        {
            auto numbers = ParseAllNumbers<int>(line);
            for (size_t c = 0; c < input.GetRows(); c++)
            {
                data[(r * input.GetRows()) + c] = numbers[c];
            }
            r++;
        }

        Transposed const t = { data };
        input = t.Transpose();
        input.Print(std::cout);
    }

    int result = 0;

    for (int x = 0; x < 100; x++) {
        for (int y = 0; y < 100; y++) {
            for (int z = 0; z < 100; z++)
            {
                int const w = 100 - x - y - z;
                if (w <= 0) {
                    continue;
}

                using TestMat = TMat<int, 4, 1>;
                auto mul = input.Mul(TestMat({ x, y, z, w }));

                int val = 1;
                for (size_t i = 0; i < 4; i++)
                {
                    val *= std::clamp(mul.At(i, 0), 0, std::numeric_limits<int>::max());
                }

                if (mul.At(4, 0) == 500)
                {
                    result = std::max(result, val);
                }
            }
}
}

    Log("Highest Score Cookie: {}", result);
    Log("Challenge Fifteen Complete!");
}

static void c16()
{
    std::vector<std::string> keywords {
        "children", "cats", "samoyeds", "pomeranians", "akitas",
        "vizslas", "goldfish", "trees", "cars", "perfumes"
    };

    constexpr int CAT_NR = 1;
    constexpr int TREE_NR = 7;

    constexpr int POMERANIANS_NR = 3;
    constexpr int GOLDFISH_NR = 6;

    constexpr int WILDCARD = -1;
    using Sue = std::array<int, 10>;

    std::vector<Sue> all_sues;

    if (auto input = OpenFileReadStream("input/2015/16.txt"))
    {
        auto lines = SplitStreamIntoLines(input.value());

        for (const auto& line : lines)
        {
            Sue sue {};
            for (size_t i = 0; i < keywords.size(); i++)
            {
                auto& word = keywords[i];
                auto it = line.find(word);

                if (it != std::string::npos)
                {
                    auto substring = line.substr(it + word.size() + 2);

                    sue[i] = ParseNumber<int>(substring).value();
                }
                else
                {
                    sue[i] = WILDCARD;
                }
            }

            all_sues.push_back(sue);
        }
    }

    Sue pattern = { 3, 7, 2, 3, 0, 0, 5, 3, 2, 1 };

    std::vector<std::pair<Sue, size_t>> matches;

    for (size_t j = 0; j < all_sues.size(); j++)
    {
        auto& sue = all_sues[j];

        bool match = true;
        for (size_t i = 0; i < 10; i++)
        {
            auto& s = sue[i];
            auto& m = pattern[i];

            if (s == WILDCARD) {
                continue;
}

            if (i == CAT_NR || i == TREE_NR) // Lower equal
            {
                if (s <= m) {
                    match = false;
}
            }
            else if (i == POMERANIANS_NR || i == GOLDFISH_NR) // Greater equal
            {
                if (s >= m) {
                    match = false;
}
            }
            else // Unequal
            {
                if (s != m) {
                    match = false;
}
            }
        }

        if (match)
        {
            matches.emplace_back(sue, j + 1);
        }
    }

    Log("Aunt Sue {} gave you the gift!", matches.front().second);
    Log("Challenge Sixteen Complete!");
}

static void c17()
{
    int target = 150;
    std::vector<int> buckets;

    if (auto in = OpenFileReadStream("input/2015/17.txt"))
    {
        auto lines = SplitStreamIntoLines(in.value());
        for (const auto& l : lines)
        {
            buckets.push_back(ParseNumber<int>(l).value());
        }
    }

    std::unordered_map<size_t, size_t> counts;

    std::function<void(const std::vector<int>& buckets, int* count, int index,
        int current, int used)>
        find_combinations;
    find_combinations = [&](const std::vector<int>& buckets, int* count, int index,
                           int current, int used) mutable
    {
        if (current == 0)
        {
            (*count)++;
            counts[used]++;

            return;
        }

        for (int i = index; i < (int)buckets.size(); ++i)
        {
            if (current - buckets[i] >= 0)
            {
                find_combinations(buckets, count, i + 1, current - buckets[i], used + 1);
            }
        }
    };

    int count = 0;
    find_combinations(buckets, &count, 0, target, 0);

    Log("{} many ways to arrange {} liters", count, target);
    Log("Challenge Seventeen Complete!");
}

static void c18()
{
    std::vector<std::string> lines;

    if (auto in = OpenFileReadStream("input/2015/18.txt"))
    {
        lines = SplitStreamIntoLines(in.value());
    }

    size_t width = lines.front().size();
    size_t height = lines.size();

    Grid<int> first = { width, height };
    Grid<int> second = { width, height };

    for (size_t j = 0; j < height; j++) {
        for (size_t i = 0; i < width; i++)
        {
            auto c = lines[j][i];
            if (c == '#') {
                first.Set(i, j, 1);
            } else {
                first.Set(i, j, 0);
}
        }
}

    // first.Print();

    auto check_neighbours = [](const Grid<int>& in, int x, int y)
    {
        int ret = 0;
        for (int j = y - 1; j <= y + 1; j++) {
            for (int i = x - 1; i <= x + 1; i++)
            {
                if (j < 0 || i < 0) {
                    continue;
}
                if (j == y && i == x) {
                    continue;
}

                if (auto val = in.At(i, j))
                {
                    if (val.value() == 1) {
                        ret++;
}
                }
            }
}
        return ret;
    };

    Grid<int>* selected[] = { &first, &second };
    size_t current = 0;

    for (size_t t = 0; t < 100; t++)
    {
        size_t const next = (current + 1) % 2;

        auto& in = *selected[current];
        auto& out = *selected[next];

        // Set corners
        in.Set(0, 0, 1);
        in.Set(width - 1, 0, 1);
        in.Set(0, height - 1, 1);
        in.Set(width - 1, height - 1, 1);

        for (size_t y = 0; y < height; y++) {
            for (size_t x = 0; x < width; x++)
            {
                int const val = in.At(x, y).value();
                int const neighbours = check_neighbours(in, x, y);

                int output_val = 2;

                if (val == 1)
                {
                    if (neighbours == 2 || neighbours == 3)
                    {
                        output_val = 1;
                    }
                    else
                    {
                        output_val = 0;
                    }
                }
                else
                {
                    if (neighbours == 3)
                    {
                        output_val = 1;
                    }
                    else
                    {
                        output_val = 0;
                    }
                }

                out.Set(x, y, output_val);
            }
}

        // Set corners
        out.Set(0, 0, 1);
        out.Set(width - 1, 0, 1);
        out.Set(0, height - 1, 1);
        out.Set(width - 1, height - 1, 1);

        // Log("");
        // out.Print();
        current = next;
    }

    size_t on = 0;
    for (size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width; x++)
        {
            if (first.At(x, y).value() == 1) {
                on++;
}
        }
}

    Log("Lights on at the end of the Animation: {}", on);
    Log("Challenge Eighteen Complete");
}

static void c19()
{
    std::string molecule {};
    std::unordered_map<std::string, std::string> rules {};

    if (auto in = OpenFileReadStream("input/2015/19.txt"))
    {
        auto lines = SplitStreamIntoLines(in.value());

        for (const auto& l : lines)
        {
            bool const is_rule = l.find("=>") != std::string::npos;

            if (is_rule)
            {
                auto words = SplitString(l);
                rules.emplace(words.back(), words.front());
            }
            else
            {
                molecule = l;
            }
        }
    }

    std::string accum;
    std::vector<std::string> elements;

    for (auto it = molecule.begin(); it != molecule.end(); it++)
    {
        accum.push_back(*it);

        auto next = it + 1;
        if (next == molecule.end() || (std::isupper(*next) != 0) || *next == 'e')
        {
            elements.emplace_back(accum);
            accum.clear();
        }
    }

    size_t rn_ar_count = 0;
    size_t y_count = 0;

    for (const auto& str : elements)
    {
        if (str == "Ar" || str == "Rn")
        {
            rn_ar_count++;
        }
        else if (str == "Y")
        {
            y_count++;
        }
    }

    size_t result = elements.size() - rn_ar_count - (y_count * 2) - 1;

    Log("Minimum replacements needed: {}", result);
    Log("Challenge Nineteen Complete");
}

static void c20()
{
    int const puzzle_input = 34000000;

    /*std::vector<int> precalculated_primes{};

    if (auto stream = OpenFileReadStream("input/primes1.txt"))
    {
            auto& ss = stream.value();

            std::string number{};
            while (std::getline(ss, number, ' '))
            {
                    if (auto n = ParseNumber<int>(number))
                    {
                            if (n.value() > puzzle_input) break;
                            precalculated_primes.push_back(n.value());
                    }
            }
    }*/

    /*auto factorize = [](std::span<int> prime_table, int number) ->
    std::vector<int>
            {
                    std::vector<int> out{};
                    int prime_index = 0;

                    while (number > 1)
                    {
                            int current_prime = prime_table[prime_index++];
                            int multiplicity = 0;

                            while (number % current_prime == 0)
                            {
                                    multiplicity++;
                                    number /= current_prime;
                            }

                            out.emplace_back(multiplicity);
                    }
                    return out;
            };

    auto div_sum = [](std::span<int> prime_table, std::span<int> multiplicity) ->
    int
            {
                    int out = 1;

                    for (size_t i = 0; i < multiplicity.size(); i++)
                    {
                            int prime = prime_table[i];
                            int mult = multiplicity[i];

                            int upper = PowerOfInt(prime, mult + 1) - 1;
                            int lower = prime - 1;
                            out *= (upper / lower);
                    }

                    return out;
            };

    int i = 781199;
    while (++i)
    {
            auto factors = factorize(precalculated_primes, i);
            int divisors = div_sum(precalculated_primes, factors);
            int presents = divisors;

            int cutoff = i / 50;
            int subtract = 0;

            for (int i = 0; i < cutoff; i++)
            {
                    if (divisors % (i + 1) == 0)
                            subtract += (i + 1);
            }

            int result = presents - subtract;

            if (result * 11 > puzzle_input)
            {
                    Log("House Number found: {}", i);
                    Log("Total presents delivered: {}", result * 11);

                    break;
            }

            if (i % 10000 == 0) Log("At {}", i);
    }*/

    auto sum1 = [](int n)
    {
        int sum = 0;
        int const d = (int)sqrt((double)n) + 1;

        for (int i = 1; i <= d; i++)
        {
            if (n % i == 0)
            {
                sum += i;
                sum += n / i;
            }
        }
        return sum;
    };

    auto sum2 = [](int n)
    {
        int sum = 0;
        int const d = (int)std::sqrt((double)n) + 1;

        for (int i = 1; i <= d; i++)
        {
            if (n % i == 0)
            {
                if (i <= 50)
                {
                    sum += n / i;
                }
                if (n / i <= 50)
                {
                    sum += i;
                }
            }
        }
        return sum;
    };

    int part1 = 0;
    while (sum1(part1) * 10 < puzzle_input) {
        part1++;
}

    int part2 = 0;
    while (sum2(part2) * 11 < puzzle_input) {
        part2++;
}

    Log("Answers: {}, {}", part1, part2);
    Log("Challenge Twenty Complete");
}

static void c21()
{
    std::vector<std::pair<int, int>> const weapon_shop {
        { 8, 4 },
        { 10, 5 },
        { 25, 6 },
        { 40, 7 },
        { 74, 8 },
    };

    std::vector<std::pair<int, int>> const armor_shop {
        { 0, 0 },
        { 13, 1 },
        { 31, 2 },
        { 53, 3 },
        { 75, 4 },
        { 102, 5 },
    };

    std::vector<std::tuple<int, int, int>> ring_shop {
        { 0, 0, 0 }, { 0, 0, 0 }, { 20, 1, 0 }, { 40, 2, 0 },
        { 80, 3, 0 }, { 25, 0, 1 }, { 50, 0, 2 }, { 100, 0, 3 }
    };

    struct Stats
    {
        int m_hitpoints = 0;
        int m_armor = 0;
        int m_damage = 0;
    };

    auto fight = [](const Stats& a, const Stats& b) -> bool
    {
        int const a_att = std::max(a.m_damage - b.m_armor, 1);
        int const b_att = std::max(b.m_damage - a.m_armor, 1);

        int ha = a.m_hitpoints;
        int hb = b.m_hitpoints;

        while (true)
        {
            hb -= a_att;
            if (hb <= 0)
            {
                return true;
            }

            ha -= b_att;
            if (ha <= 0)
            {
                return false;
            }
        }

        /*int a_rounds = (int)std::ceilf((float)a.hitpoints / (float)b_att);
        int b_rounds = (int)std::ceilf((float)b.hitpoints / (float)a_att);

        return a_rounds >= b_rounds;*/
    };

    Stats const player = { .hitpoints=100, .armor=0, .damage=0 };
    Stats const boss = { .hitpoints=104, .armor=1, .damage=8 };

    std::vector<int> gold_amounts {};

    for (auto& selected_weapon : weapon_shop) {
        for (auto& selected_armor : armor_shop)
        {
            for (auto& acc1 : ring_shop) {
                for (auto& acc2 : ring_shop)
                {
                    if (&acc1 == &acc2) {
                        continue;
}

                    Stats test = player;
                    int gold_spent = selected_weapon.first + selected_armor.first + std::get<0>(acc1) + std::get<0>(acc2);
                    test.m_armor = selected_armor.second + std::get<1>(acc1) + std::get<1>(acc2);
                    test.m_damage = selected_weapon.second + std::get<2>(acc1) + std::get<2>(acc2);

                    if (!fight(test, boss))
                    {
                        Log("Lost battle with: {}/{} - {}", test.m_damage, test.m_armor,
                            gold_spent);
                        gold_amounts.push_back(gold_spent);
                    }
                }
}
        }
}

    Log("Minimum gold: {}",
        *std::ranges::max_element(gold_amounts));
    Log("Challenge 21 Complete");
}

static void c22()
{
    enum class Choice
    {
        MISSILE, // 53 4
        DRAIN, // 73 2 2
        SHIELD, // 113 R6 7
        POISON, // 173 R6 3
        RECHARGE // 229 R5 101
    };

    enum class State
    {
        RUNNING,
        VICTORY,
        DEFEAT,
        INVALID
    };

    class FightContext
    {
        int m_current_health = 50;
        int m_current_mana = 500;
        int m_total_mana_spent = 0;

        int m_shield_timer = 0;
        int m_poison_timer = 0;
        int m_recharge_timer = 0;

        int m_boss_health = 51;
        int m_boss_damage = 9;

    public:
        [[nodiscard]] int get_spent() const { return m_total_mana_spent; }

        State next(Choice c)
        {
            // DOT

            m_current_health--;
            if (m_current_health <= 0) {
                return State::DEFEAT;
}

            // Apply effects

            if (m_shield_timer > 0)
            {
                m_shield_timer--;
            }

            if (m_poison_timer > 0)
            {
                m_boss_health -= 3;
                m_poison_timer--;

                if (m_boss_health <= 0)
                {
                    return State::VICTORY;
                }
            }

            if (m_recharge_timer > 0)
            {
                m_recharge_timer--;
                m_current_mana += 101;
            }

            // Use Choice

            switch (c)
            {
            case Choice::MISSILE:
                if (m_current_mana < 53) {
                    return State::INVALID;
}

                m_current_mana -= 53;
                m_total_mana_spent += 53;
                m_boss_health -= 4;

                if (m_boss_health <= 0) {
                    return State::VICTORY;
}

                break;

            case Choice::DRAIN:
                if (m_current_mana < 73) {
                    return State::INVALID;
}
                m_current_mana -= 73;
                m_total_mana_spent += 73;

                m_current_health += 2;
                m_boss_health += -2;

                if (m_boss_health <= 0) {
                    return State::VICTORY;
}

                break;

            case Choice::POISON:
                if (m_current_mana < 173) {
                    return State::INVALID;
}
                if (m_poison_timer > 0) {
                    return State::INVALID;
}

                m_current_mana -= 173;
                m_total_mana_spent += 173;
                m_poison_timer += 6;

                break;

            case Choice::SHIELD:
                if (m_current_mana < 113) {
                    return State::INVALID;
}
                if (m_shield_timer > 0) {
                    return State::INVALID;
}

                m_current_mana -= 113;
                m_total_mana_spent += 113;
                m_shield_timer += 6;

                break;

            case Choice::RECHARGE:
                if (m_current_mana < 229) {
                    return State::INVALID;
}
                if (m_recharge_timer > 0) {
                    return State::INVALID;
}

                m_current_mana -= 229;
                m_total_mana_spent += 229;
                m_recharge_timer += 5;

                break;
            }

            // Boss Turn
            int temp_armor = 0;

            if (m_shield_timer > 0)
            {
                m_shield_timer--;
                temp_armor = 7;
            }

            if (m_poison_timer > 0)
            {
                m_boss_health -= 3;
                m_poison_timer--;

                if (m_boss_health <= 0)
                {
                    return State::VICTORY;
                }
            }

            if (m_recharge_timer > 0)
            {
                m_recharge_timer--;
                m_current_mana += 101;
            }

            m_current_health -= std::max(1, m_boss_damage - temp_armor);

            if (m_current_health <= 0) {
                return State::DEFEAT;
}

            return State::RUNNING;
        }
    };

    const FightContext STARTING {};
    int lowest_mana_spent = std::numeric_limits<int>::max();

    std::queue<FightContext> queue {};
    queue.push(STARTING);

    auto process_choice = [&queue, &lowest_mana_spent](const FightContext& f,
                              Choice c)
    {
        FightContext n = f;
        State const result = n.next(c);

        switch (result)
        {
        case State::RUNNING:
            queue.push(n);
            break;
        case State::VICTORY:
            lowest_mana_spent = std::min(lowest_mana_spent, n.get_spent());
            break;
        default:
            break;
        }
    };

    while (!queue.empty() != 0u)
    {
        const FightContext THIS_FIGHT = queue.front();
        queue.pop();

        if (THIS_FIGHT.get_spent() >= lowest_mana_spent) {
            continue;
}

        process_choice(THIS_FIGHT, Choice::MISSILE);
        process_choice(THIS_FIGHT, Choice::DRAIN);
        process_choice(THIS_FIGHT, Choice::POISON);
        process_choice(THIS_FIGHT, Choice::RECHARGE);
        process_choice(THIS_FIGHT, Choice::SHIELD);
    }

    Log("Lowest Mana to defeat boss: {}", lowest_mana_spent);
    Log("Challenge 22 Complete!");
}

static void c23()
{
    struct VirtualMachine
    {
        const size_t INSTRUCTION_SIZE = 3;
        const char OFFSET_START = ',';

        const char* m_half = "hlf";
        const char* m_triple = "tpl";
        const char* m_inc = "inc";
        const char* m_jmp = "jmp";
        const char* m_jmp_even = "jie";
        const char* m_jmp_one = "jio";

        uint32_t m_registers[2] { 0, 0 };
        int32_t m_instruction_ptr { 0 };

        void run(std::vector<std::string>& program)
        {
            do
            {
                m_instruction_ptr = read_line(program[m_instruction_ptr]);
            } while (m_instruction_ptr < (int32_t)program.size() && m_instruction_ptr >= 0);
        }

    private:
        uint32_t get_register(const std::string& line) const
        {
            return line[1 + INSTRUCTION_SIZE] - 'a';
        }

        static int32_t get_number(const std::string& line, size_t off)
        {
            auto view = std::string_view(line).substr(off);
            if (view.starts_with('+'))
            {
                view = view.substr(1);
            }

            return ParseNumber<int32_t>(view).value();
        }

        int32_t read_line(const std::string& line)
        {
            // HALF
            if (size_t const i = line.find(m_half); i != std::string::npos)
            {
                uint32_t const reg = get_register(line);
                m_registers[reg] /= 2;
                return m_instruction_ptr + 1;
            }

            // TRIPLE
            if (size_t const i = line.find(m_triple); i != std::string::npos)
            {
                uint32_t const reg = get_register(line);
                m_registers[reg] *= 3;
                return m_instruction_ptr + 1;
            }

            // INC
            if (size_t const i = line.find(m_inc); i != std::string::npos)
            {
                uint32_t const reg = get_register(line);
                m_registers[reg] += 1;
                return m_instruction_ptr + 1;
            }

            // JMP
            if (size_t const i = line.find(m_jmp); i != std::string::npos)
            {
                int32_t const offset = get_number(line, INSTRUCTION_SIZE + 1);
                return m_instruction_ptr + offset;
            }

            // JIE
            if (size_t const i = line.find(m_jmp_even); i != std::string::npos)
            {
                uint32_t const reg = get_register(line);

                if (m_registers[reg] % 2 == 0)
                {
                    int32_t const offset = get_number(line, line.find(OFFSET_START) + 2);
                    return m_instruction_ptr + offset;
                }

                return m_instruction_ptr + 1;
            }

            // JIO
            if (size_t const i = line.find(m_jmp_one); i != std::string::npos)
            {
                uint32_t const reg = get_register(line);

                if (m_registers[reg] == 1)
                {
                    int32_t const offset = get_number(line, line.find(OFFSET_START) + 2);
                    return m_instruction_ptr + offset;
                }

                return m_instruction_ptr + 1;
            }

            throw;
        }
    };

    VirtualMachine vm {};
    vm.m_registers[0] = 1;

    if (auto stream = OpenFileReadStream("input/2015/23.txt"))
    {
        auto lines = SplitStreamIntoLines(stream.value());
        vm.run(lines);
    }

    Log("Register b : {}", vm.m_registers[1]);
    Log("Challenge 23 Complete!");
}

static void c24()
{
    std::vector<int> all_packages;

    if (auto stream = OpenFileReadStream("input/2015/24.txt"))
    {
        auto lines = SplitStreamIntoLines(stream.value());
        for (auto& l : lines)
        {
            all_packages.emplace_back(ParseNumber<int>(l).value());
        }
    }

    int const box_sum = std::accumulate(all_packages.begin(), all_packages.end(), 0);
    constexpr int TARGET_SIZE = 6;

    auto res = FindCombinations(all_packages, TARGET_SIZE);
    std::vector<std::array<int, TARGET_SIZE>> possibilities {};

    for (size_t j = 0; j < res.GetHeight(); j++)
    {
        int const sum = std::accumulate(&res(0, j), &res(TARGET_SIZE, j), 0);
        if (sum == box_sum / 3)
        {
            std::array<int, TARGET_SIZE> t {};
            std::copy(&res(0, j), &res(TARGET_SIZE, j), t.begin());
            possibilities.emplace_back(t);
        }
    }

    auto check_sum = [](const std::vector<int>& remaining, int target)
    {
        for (size_t i = 1; i < remaining.size(); i++)
        {
            auto check = FindCombinations(remaining, i);
            size_t const width = check.GetWidth();

            for (size_t j = 0; j < check.GetHeight(); j++)
            {
                int const sum = std::accumulate(&check(0, j), &check(width, j), 0);
                if (sum == target)
                {
                    return true;
                }
            }
        }

        return false;
    };

    std::vector<std::array<int, TARGET_SIZE>> checked {};

    for (const auto& p : possibilities)
    {
        std::vector<int> remaining = all_packages;

        for (auto i : p)
        {
            std::ranges::remove_if(remaining,
                [=](const auto& l)
                { return l == i; });
            remaining.pop_back();
        }

        if (check_sum(remaining, box_sum / 3))
        {
            checked.emplace_back(p);
        }
    }

    // Log("{}", possibilities);
    Log("Challenge 23 Complete!");
}

} // namespace Y2015

int main(int argc, char* argv[])
{
    std::cout << "Hello World" << '\n';

    if (argc > 1 && std::string(argv[1]) == "-T")
    {
        return 0;
    }

    // Y2015::C1();
    // Y2015::C2();
    // Y2015::C3();
    // Y2015::C4();
    // Y2015::C5();
    // Y2015::C6();
    // Y2015::C7();
    // Y2015::C8();
    // Y2015::C9();
    // Y2015::C10();

    // Y2015::C11();
    // Y2015::C12();
    // Y2015::C13();
    // Y2015::C14();
    // Y2015::C15();
    // Y2015::C16();
    // Y2015::C17();
    // Y2015::C18();
    // Y2015::C19();
    // Y2015::C20();

    // Y2015::C21();
    // Y2015::C22();
    // Y2015::C23();
    // Y2015::C24();

    return 0;
}