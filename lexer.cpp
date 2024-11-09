#include <iostream>
#include <fstream>
#include <cctype>
#include <string>
#include <unordered_set>

enum TokenType
{
  IDENTIFIER,
  KEYWORD,
  INTEGER_LITERAL,
  FLOAT_LITERAL,
  CHAR_LITERAL,
  OPERATOR,
  PUNCTUATION,
  END_OF_INPUT,
  UNKNOWN
};

struct Token
{
  TokenType type;
  std::string value;
};

class Lexer
{
public:
  Lexer(const std::string &input) : input(input), position(0) {}

  Token getNextToken()
  {
    while (position < input.length())
    {
      char current = input[position];

      if (std::isspace(current))
      {
        ++position;
        continue; // Ignore whitespace
      }

      if (std::isalpha(current))
      {
        return readIdentifierOrKeyword();
      }

      if (std::isdigit(current))
      {
        return readNumberLiteral();
      }

      if (current == '\'')
      {
        return readCharLiteral();
      }

      if (isPunctuation(current))
      {
        ++position;
        return Token{PUNCTUATION, std::string(1, current)};
      }

      if (isOperator(current))
      {
        ++position;
        return Token{OPERATOR, std::string(1, current)};
      }

      if (position == input.length() - 1)
      {
        return Token{END_OF_INPUT, ""};
      }

      ++position; // Move past unknown characters
      return Token{UNKNOWN, std::string(1, current)};
    }

    return Token{END_OF_INPUT, ""};
  }

private:
  std::string input;
  size_t position;

  std::unordered_set<std::string> keywords{"if", "else", "while", "return", "int", "float", "char"};

  bool isPunctuation(char c)
  {
    return c == '{' || c == '}' || c == ';' || c == ',';
  }

  bool isOperator(char c)
  {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '%' ||
           c == '=' || c == '&' || c == '|' || c == '!' || c == '<' || c == '>';
  }

  Token readIdentifierOrKeyword()
  {
    size_t start = position;
    while (position < input.length() && std::isalnum(input[position]))
    {
      ++position;
    }
    std::string word = input.substr(start, position - start);
    if (keywords.count(word))
    {
      return Token{KEYWORD, word};
    }
    return Token{IDENTIFIER, word};
  }

  Token readNumberLiteral()
  {
    size_t start = position;
    bool isFloat = false;

    while (position < input.length() && std::isdigit(input[position]))
    {
      ++position;
    }

    if (position < input.length() && input[position] == '.')
    {
      isFloat = true;
      ++position;
      while (position < input.length() && std::isdigit(input[position]))
      {
        ++position;
      }
    }

    std::string num = input.substr(start, position - start);
    if (isFloat)
    {
      return Token{FLOAT_LITERAL, num};
    }
    else
    {
      return Token{INTEGER_LITERAL, num};
    }
  }

  Token readCharLiteral()
  {
    size_t start = position;
    position++; // Skip opening quote

    if (position < input.length() && input[position] != '\'')
    {
      position++; // Move to closing quote
      if (position < input.length() && input[position] == '\'')
      {
        position++; // Skip closing quote
        return Token{CHAR_LITERAL, input.substr(start + 1, 1)};
      }
    }

    return Token{UNKNOWN, ""}; // Invalid char literal
  }
};

void printToken(const Token &token)
{
  std::string typeName;
  switch (token.type)
  {
  case IDENTIFIER:
    typeName = "Identifier";
    break;
  case KEYWORD:
    typeName = "Keyword";
    break;
  case INTEGER_LITERAL:
    typeName = "Integer Literal";
    break;
  case FLOAT_LITERAL:
    typeName = "Float Literal";
    break;
  case CHAR_LITERAL:
    typeName = "Char Literal";
    break;
  case OPERATOR:
    typeName = "Operator";
    break;
  case PUNCTUATION:
    typeName = "Punctuation";
    break;
  case END_OF_INPUT:
    typeName = "End of Input";
    break;
  default:
    typeName = "Unknown";
    break;
  }
  std::cout << "Token(Type: " << typeName << ", Value: \"" << token.value << "\")\n";
}

std::string readFileContent(const std::string &filename)
{
  std::ifstream file(filename);
  if (!file.is_open())
  {
    std::cerr << "Error: Could not open file " << filename << "\n";
    exit(1);
  }

  std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  file.close();
  return content;
}

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    std::cerr << "Usage: " << argv[0] << " <filename>\n";
    return 1;
  }

  std::string filename = argv[1];
  std::string input = readFileContent(filename);

  Lexer lexer(input);
  Token token;
  do
  {
    token = lexer.getNextToken();
    printToken(token);
  } while (token.type != END_OF_INPUT);

  return 0;
}
