#include <iostream>
#include <vector>
#include <string>
#include <variant>
#include <unordered_map>
// подключаем для std::transform() в 65 строке
#include <algorithm>
// подключаем для тестов в мэйне
#include <cassert>

// до 24 строки код без изменений
struct OpeningBracket {};
struct ClosingBracket {};
struct Number { int value; };
struct UnknownToken { std::string value; };
struct MinToken {};
struct AbsToken {};
struct Plus {};
struct Minus {};
struct Multiply {};
struct Modulo {};
struct Divide {};

// выполнение первого пункта задания - добавляем еще два докена для операции поиска максимума и нахождения квадрата
struct MaxToken {};

struct SqrToken {};

// без запятой не получится использовать методы поиска наибольшего/наименьшего
struct Comma {};
// конец выполнения первого пункта задания

// выполнение второго пункта задания - добавляем оставшиеся варианты токена, а также пары символ-токен для обработки возможных символов
using Token = std::variant<OpeningBracket, ClosingBracket, Number, UnknownToken, MinToken, AbsToken,
                           Plus, Minus, Multiply, Modulo, Divide, MaxToken, SqrToken, Comma>;

const std::unordered_map<char, Token> kSymbol2Token{
    {'+', Plus{}},
    {'-', Minus{}},
    {'*', Multiply{}},
    {'/', Divide{}},
    {'%', Modulo{}},
    {'(', OpeningBracket{}},
    {')', ClosingBracket{}},
    {',', Comma{}}};
// конец выполнения второго пункта задания

// выполнение третьего пункта задания - реализуем метод обработки последовательности букв как математических операций
Token ParseName(const std::string &input, size_t &pos)
{
    std::string operationName{};
    auto symbol = static_cast<unsigned char>(input[pos]);
    while (std::isalpha(symbol))
    {
        operationName += symbol;
        if (pos == input.size() - 1)
        {
            ++pos;
            break;
        }
        symbol = static_cast<unsigned char>(input[++pos]);
    }
    // функция преобразует посимвольно строку в ловеркейс, используем начальные итераторы .begin() и .end(),
    // итератор полученной строки .begin() и унарный оператор ::tolower соответственно
    std::transform(operationName.begin(), operationName.end(), operationName.begin(), ::tolower);

    if (operationName == "abs") return AbsToken{};
    else if (operationName == "max") return MaxToken{};
    else if (operationName == "min") return MinToken{};
    else if (operationName == "sqr") return SqrToken{};
    else return UnknownToken{operationName};
}
// конец выполнения третьего пункта задания

// до 93 строки код без изменений
Number ParseNumber(const std::string &input, size_t &pos)
{
    int value = 0;
    auto symbol = static_cast<unsigned char>(input[pos]);
    while (std::isdigit(symbol))
    {
        value = value * 10 + symbol - '0';
        if (pos == input.size() - 1)
        {
            ++pos;
            break;
        }
        symbol = static_cast<unsigned char>(input[++pos]);
    }
    return Number{value};
}

// выполнение четвертого пункта задания - дописываем обработку последовательности букв и инкремент позиции в случае символьных операций
std::vector<Token> Tokenize(const std::string &input)
{
    std::vector<Token> tokens;
    const size_t size = input.size();
    size_t pos = 0;
    while (pos < size)
    {
        const auto symbol = static_cast<unsigned char>(input[pos]);
        if (std::isspace(symbol))
            ++pos;
        else if (std::isdigit(symbol))
            tokens.emplace_back(ParseNumber(input, pos));
        else if (std::isalpha(symbol))
            tokens.emplace_back(ParseName(input, pos));
        else if (auto iterator = kSymbol2Token.find(symbol); iterator != kSymbol2Token.end()) {
            tokens.emplace_back(iterator->second);
            ++pos;
        }
    }
    return tokens;
}
// конец выполнения четвертого пункта задания

int main()
{
    // проверяем выражения с числами, скобками и операциями
    std::string expr1 = "5 + (2 * 3) - 7";
    auto tokens1 = Tokenize(expr1);
    assert(std::get<Number>(tokens1[0]).value == 5);
    assert(std::holds_alternative<Plus>(tokens1[1]));
    assert(std::holds_alternative<OpeningBracket>(tokens1[2]));
    assert(std::get<Number>(tokens1[3]).value == 2);
    assert(std::holds_alternative<Multiply>(tokens1[4]));
    assert(std::get<Number>(tokens1[5]).value == 3);
    assert(std::holds_alternative<ClosingBracket>(tokens1[6]));
    assert(std::holds_alternative<Minus>(tokens1[7]));
    assert(std::get<Number>(tokens1[8]).value == 7);

    // проверяем выражение с мат. функциями
    std::string expr2 = "max(3, min(4, abs(-2)), sqr(5))";
    auto tokens2 = Tokenize(expr2);
    assert(std::holds_alternative<MaxToken>(tokens2[0]));
    assert(std::holds_alternative<OpeningBracket>(tokens2[1]));
    assert(std::get<Number>(tokens2[2]).value == 3);
    assert(std::holds_alternative<Comma>(tokens2[3]));
    assert(std::holds_alternative<MinToken>(tokens2[4]));
    assert(std::holds_alternative<OpeningBracket>(tokens2[5]));
    assert(std::get<Number>(tokens2[6]).value == 4);
    assert(std::holds_alternative<Comma>(tokens2[7]));
    assert(std::holds_alternative<AbsToken>(tokens2[8]));
    assert(std::holds_alternative<OpeningBracket>(tokens2[9]));
    assert(std::holds_alternative<Minus>(tokens2[10]));
    assert(std::get<Number>(tokens2[11]).value == 2);
    assert(std::holds_alternative<ClosingBracket>(tokens2[12]));
    assert(std::holds_alternative<ClosingBracket>(tokens2[13]));
    assert(std::holds_alternative<Comma>(tokens2[14]));
    assert(std::holds_alternative<SqrToken>(tokens2[15]));
    assert(std::holds_alternative<OpeningBracket>(tokens2[16]));
    assert(std::get<Number>(tokens2[17]).value == 5);
    assert(std::holds_alternative<ClosingBracket>(tokens2[18]));
    assert(std::holds_alternative<ClosingBracket>(tokens2[19]));

    // проверяем выражение с неизвестным токеном
    std::string expr3 = "hello(42)";
    auto tokens3 = Tokenize(expr3);
    assert(std::get<UnknownToken>(tokens3[0]).value == "hello");
    assert(std::holds_alternative<OpeningBracket>(tokens3[1]));
    assert(std::get<Number>(tokens3[2]).value == 42);
    assert(std::holds_alternative<ClosingBracket>(tokens3[3]));

    std::cout << "All tests passed.\n";
    return 0;
}