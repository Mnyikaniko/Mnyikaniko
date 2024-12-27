#include <iostream>
#include <cmath>
#include <iomanip>
#include <string>
#include <stdexcept>
#include <limits>
#include <vector>

// Expression tokenizer and evaluator
class ExpressionEvaluator {
 public:

    std::string expr;
    size_t pos;

    bool isDegreeMode;

    // Constants
    static constexpr double PI = M_PI;

    ExpressionEvaluator(const std::string& expression = "",bool isDegreeMode = true) 
    : expr(expression), pos(0), isDegreeMode(isDegreeMode){}

    // Skip whitespace
    void skipWhitespace() {
        while (pos < expr.length() && std::isspace(expr[pos])) pos++;
    }

    void toggleAngleMode() {
        isDegreeMode = !isDegreeMode;
        std::cout << "Angle mode changed to: " << (isDegreeMode ? "Degrees" : "Radians") << std::endl;
    }

    // Get next token
    bool isOperator(char c) const {
        return c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == '!' || c == 'C' || c == 'P';
    }

    double toRadians(double angle) const {
                return isDegreeMode ? angle * PI / 180.0 : angle;
            }
            
    double toDegrees(double angle) const {
                return isDegreeMode ? angle * 180.0 / PI : angle;
            }
    

    // Basic arithmetic operations
    double add(double a, double b) const { return a + b; }
    double subtract(double a, double b) const { return a - b; }
    double multiply(double a, double b) const { return a * b; }
    double divide(double a, double b) const {
        if (std::abs(b) < std::numeric_limits<double>::epsilon()) {
            throw std::invalid_argument("Division by zero");
        }
        return a / b;
    }

    double parseNumber() {
        skipWhitespace();
        size_t start = pos;
        bool hasDecimal = false;
        
        // Check for negative number context
        bool isNegative = false;
        if (pos < expr.length() && expr[pos] == '-') {
            // It's a negative number only if:
            // 1. It's the start of the expression, OR
            // 2. It follows an operator or opening parenthesis
            bool isStart = (pos == 0);
            bool followsOperator = pos > 0 && (isOperator(expr[pos-1]) || expr[pos-1] == '(');
            
            if (!isStart && !followsOperator) {
                // This is a subtraction operator, not a negative sign
                return 0;
            }
            
            // This is indeed a negative number
            isNegative = true;
            pos++;
            start = pos; // Move start past the negative sign
        }
        
        // Must have at least one digit
        if (pos >= expr.length() || (!std::isdigit(expr[pos]) && expr[pos] != '.')) {
            if (isNegative) {
                // We found a negative sign but no number follows
                pos = start - 1; // Reset position to before the negative sign
                return 0; // Let the operator handling take care of it
            }
            throw std::invalid_argument("Invalid number format");
        }
        
        // Parse digits and decimal point
        while (pos < expr.length() && 
               (std::isdigit(expr[pos]) || expr[pos] == '.' || 
                tolower(expr[pos]) == 'e')) {
            
            if (expr[pos] == '.') {
                if (hasDecimal) {
                    throw std::invalid_argument("Multiple decimal points in number");
                }
                hasDecimal = true;
            } 
            else if (tolower(expr[pos]) == 'e') {
                size_t ePos = pos;
                // Handle scientific notation
                pos++;
                // Allow + or - after e
                if (pos < expr.length() && (expr[pos] == '+' || expr[pos] == '-')) {
                    pos++;
                }
                // Must have at least one digit after e
                if (pos >= expr.length() || !std::isdigit(expr[pos])) {
                    pos = ePos; // Reset to position of 'e'
                    break;  // Not scientific notation, might be a function name
                }
                // Continue parsing digits for exponent
                while (pos < expr.length() && std::isdigit(expr[pos])) {
                    pos++;
                }
                break;
            }
            pos++;
        }
        
        // Don't allow number to end with decimal point
        if (pos > start && expr[pos-1] == '.') {
            throw std::invalid_argument("Number cannot end with decimal point");
        }
        
        std::string numStr;
        if (isNegative) {
            numStr = "-" + expr.substr(start, pos - start);
        } else {
            numStr = expr.substr(start, pos - start);
        }
        
        try {
            return std::stod(numStr);
        } catch (const std::exception& e) {
            throw std::invalid_argument("Invalid number format: " + numStr);
        }
    }

    // Parse function
    std::string parseFunction() {
        skipWhitespace();
        size_t start = pos;
        while (pos < expr.length() && std::isalpha(expr[pos])) {
            pos++;
        }
        return expr.substr(start, pos - start);
    }

    // Get operator precedence
int getPrecedence(char op) const {
    switch (op) {
        case '+': case '-': return 1; 
        case '*': case '/': return 2; 
        case 'P': case 'C': return 3; 
        case '^': return 4;           
        case '!': return 5;           
        default: return 0;            
    }
}


        // Calculate factorial
        double factorial(double n) const {
            if (n < 0) {
                throw std::invalid_argument("Factorial is not defined for negative numbers");
            }
            if (n > 170) {
                throw std::overflow_error("Factorial result too large");
            }
            if (floor(n) != n) {
                throw std::invalid_argument("Factorial is only defined for integers");
            }
            
            double result = 1.0;
            for (int i = 2; i <= n; ++i) {
                result *= i;
            }
            return result;
        }

    // Apply operator
    double applyOperator(double a, double b, char op) const {
        switch (op) {
            case '+': return add(a , b);
            case '-': return subtract(a , b);
            case '*': return multiply(a , b);
            case '!': return factorial(a);
            case '/': return divide(a , b);
            case '^': return std::pow(a, b);
            case 'P': return nPr(a, b);
            case 'C': return nCr(a, b);
            default: throw std::invalid_argument("Invalid operator");
        }
    }

    double nCr(double n, double r) const {
        if (r > n) {
            throw std::invalid_argument("r cannot be greater than n in nCr");
        }
        return factorial(n) / (factorial(r) * factorial(n - r));
    }

    double nPr(double n, double r) const {
        if (r > n) {
            throw std::invalid_argument("r cannot be greater than n in nPr");
        }
        return factorial(n) / factorial(n - r);
    }


   
         // Toggle between degrees and radians
        double evaluate() {
        std::vector<double> values;
        std::vector<char> operators;

        while (pos < expr.length()) {
            skipWhitespace();
            
            if (pos >= expr.length()) break;

            char currentChar = expr[pos];

            if (std::isdigit(currentChar) || (currentChar == '-' && 
                (pos == 0 || (isOperator(expr[pos-1]) && (expr[pos-1] != '!')) || expr[pos-1] == '('))) {
                values.push_back(parseNumber());
            }
            
            else if (std::isalpha(currentChar) && !isOperator(currentChar)) {
                std::string func = parseFunction();
                skipWhitespace();
                
                if (pos >= expr.length() || expr[pos] != '(') {
                    throw std::invalid_argument("Expected '(' after function");
                }
                pos++; // skip '('
                
                size_t start = pos;
                int parentheses = 1;
                while (pos < expr.length() && parentheses > 0) {
                    if (expr[pos] == '(') parentheses++;
                    else if (expr[pos] == ')') parentheses--;
                    pos++;
                }
                
                if (parentheses > 0) {
                    throw std::invalid_argument("Mismatched parentheses");
                }
                
                std::string argExpr = expr.substr(start, pos - start - 1);
                ExpressionEvaluator argEval(argExpr,isDegreeMode);
                double arg = argEval.evaluate();
                
                // Apply function with expanded trigonometric support
                if (func == "sin") values.push_back(std::sin(toRadians(arg)));
                else if (func == "cos") values.push_back(std::cos(toRadians(arg)));
                else if (func == "tan") values.push_back(std::tan(toRadians(arg)));
                else if (func == "asin" || func == "arcsin") {
                    if (arg < -1 || arg > 1) throw std::invalid_argument("Arcsin argument must be between -1 and 1");
                    values.push_back(toDegrees(std::asin(arg)));
                }
                else if (func == "acos" || func == "arccos") {
                    if (arg < -1 || arg > 1) throw std::invalid_argument("Arccos argument must be between -1 and 1");
                    values.push_back(toDegrees(std::acos(arg)));
                }
                else if (func == "atan" || func == "arctan") values.push_back(toDegrees(std::atan(arg)));
                else if (func == "sinh") values.push_back(std::sinh(arg));
                else if (func == "cosh") values.push_back(std::cosh(arg));
                else if (func == "tanh") values.push_back(std::tanh(arg));
                else if (func == "log") values.push_back(std::log10(arg));
                else if (func == "ln") values.push_back(std::log(arg));
                else if (func == "sqrt") values.push_back(std::sqrt(arg));
                else if (func == "fact") values.push_back(factorial(arg));
                else throw std::invalid_argument("Unknown function: " + func);
            }
            else if (currentChar == '(') {
                operators.push_back(currentChar);
                pos++;
            }
            else if (currentChar == ')') {
                while (!operators.empty() && operators.back() != '(') {
                    double b = values.back(); values.pop_back();
                    double a = values.back(); values.pop_back();
                    char op = operators.back(); operators.pop_back();
                    values.push_back(applyOperator(a, b, op));
                }
                if (operators.empty()) {
                    throw std::invalid_argument("Mismatched parentheses");
                }
                operators.pop_back(); // Remove '('
                pos++;
            }
            else if (isOperator(currentChar)) {
                // Special handling for factorial
                if (currentChar == '!') {
                    if (values.empty()) {
                        throw std::invalid_argument("Invalid factorial placement");
                    }
                    double a = values.back();
                    values.pop_back();
                    values.push_back(factorial(a));
                    pos++;
                } else {
                    while (!operators.empty() && operators.back() != '(' && 
                           getPrecedence(operators.back()) >= getPrecedence(currentChar)) {
                        double b = values.back(); values.pop_back();
                        double a = values.back(); values.pop_back();
                        char op = operators.back(); operators.pop_back();
                        values.push_back(applyOperator(a, b, op));
                    }
                    operators.push_back(currentChar);
                    pos++;
                }
            }
            else {
                throw std::invalid_argument("Invalid character in expression");
            }
        }

        while (!operators.empty()) {
            if (operators.back() == '(' || operators.back() == ')') {
                throw std::invalid_argument("Mismatched parentheses");
            }
            double b = values.back(); values.pop_back();
            double a = values.back(); values.pop_back();
            char op = operators.back(); operators.pop_back();
            values.push_back(applyOperator(a, b, op));
        }

        if (values.size() != 1) {
            throw std::invalid_argument("Invalid expression");
        }

        return values[0];
    }
};




class ScientificCalculator : public ExpressionEvaluator{
    public:
    void runInteractiveCalculator() {
        std::string input;
        std::vector<std::string> history;  // Store calculation history
        const int MAX_HISTORY = 20;        // Maximum history entries to store
        
        printWelcomeBanner();
        printHelp();
        
        while (true) {
            std::cout << "\n Expression > ";
            std::getline(std::cin, input);
            
            // Trim whitespace from input
            input = trim(input);
            
            if (input.empty()) {
                continue;
            }
    
            // Handle special commands
            if (handleSpecialCommand(input, history)) {
                continue;
            }
            
            if (input == "exit" || input == "quit") {
                printFarewell();
                break;
            }
            
            try {
                ExpressionEvaluator evaluator(input,isDegreeMode);
                double result = evaluator.evaluate();
                
                // Format and display result
                std::string formattedResult = formatResult(result);
                std::cout << " = " << formattedResult << std::endl;
                
                // Add to history
                addToHistory(history, input + " = " + formattedResult, MAX_HISTORY);
                
            } catch (const std::exception& e) {
                std::cerr << " Error: " << e.what() << std::endl;
                std::cerr << " Type 'help' for usage examples" << std::endl;
            }
        }
    }
    
    void printWelcomeBanner() {
        std::cout << "\n\t\t\t╔════════════════════════════════════╗" << std::endl;
        std::cout << "\t\t\t║      Scientific Calculator         ║" << std::endl;
        std::cout << "\t\t\t╚════════════════════════════════════╝" << std::endl;
    }
    
    void printHelp() {
        std::cout << "\n Available Commands:" << std::endl;
        std::cout << " - help     : Show this help message" << std::endl;
        std::cout << " - clear    : Clear screen" << std::endl;
        std::cout << " - history  : Show calculation history" << std::endl;
        std::cout << " - exit/quit: Exit calculator" << std::endl;
        std::cout << " - mode: Swith between degree and radians" << std::endl;
        
        std::cout << "\n Basic Operations:" << std::endl;
        std::cout << " +  : Addition         (e.g., 2 + 3)" << std::endl;
        std::cout << " -  : Subtraction      (e.g., 5 - 2)" << std::endl;
        std::cout << " *  : Multiplication   (e.g., 4 * 3)" << std::endl;
        std::cout << " /  : Division         (e.g., 8 / 2)" << std::endl;
        std::cout << " ^  : Exponentiation   (e.g., 2 ^ 3)" << std::endl;
        
        std::cout << "\n Special Operations:" << std::endl;
        std::cout << " !  : Factorial        (e.g., 5!)" << std::endl;
        std::cout << " P  : Permutation      (e.g., 5P2)" << std::endl;
        std::cout << " C  : Combination      (e.g., 5C2)" << std::endl;
        
        std::cout << "\n Mathematical Functions:" << std::endl;
        std::cout << " - Trigonometric: sin, cos, tan" << std::endl;
        std::cout << " - Inverse Trig: asin/arcsin, acos/arccos, atan/arctan" << std::endl;
        std::cout << " - Hyperbolic : sinh, cosh, tanh" << std::endl;
        std::cout << " - Other      : log, ln, sqrt" << std::endl;
        
        std::cout << "\n Example Expressions:" << std::endl;
        std::cout << " - sin(45) + 2 * sqrt(16)" << std::endl;
        std::cout << " - 5! / 5C2" << std::endl;
        std::cout << " - 2 * pi + log(100)" << std::endl;
        std::cout << " - 3 * (2 + 4) ^ 2" << std::endl;
    }
    
    std::string formatResult(double result) {
        std::ostringstream oss;
        if (std::abs(result) < 1e-10) {
            result = 0;  // Handle very small numbers near zero
        }
        
        if (std::abs(result) > 1e6 || (std::abs(result) < 1e-4 && result != 0)) {
            // Use scientific notation for very large or small numbers
            oss << std::scientific;
        } else {
            oss << std::fixed;
        }
        
        oss << std::setprecision(8) << result;
        std::string str = oss.str();
        
        // Remove trailing zeros after decimal point
        if (str.find('.') != std::string::npos) {
            str = str.substr(0, str.find_last_not_of('0') + 1);
            if (str.back() == '.') {
                str = str.substr(0, str.size() - 1);
            }
        }
        
        return str;
    }
    
    bool handleSpecialCommand(const std::string& input, const std::vector<std::string>& history) {
        if (input == "help") {
            printHelp();
            return true;
        }
        
        if (input == "clear") {
            #ifdef _WIN32
                system("cls");
            #else
                system("clear");
            #endif
            printWelcomeBanner();
            return true;
        }
        if(input == "mode"){
            toggleAngleMode();
            return true;
        }
        
        if (input == "history") {
            if (history.empty()) {
                std::cout << " No calculations in history" << std::endl;
            } else {
                std::cout << "\n Calculation History:" << std::endl;
                for (size_t i = 0; i < history.size(); ++i) {
                    std::cout << " " << i + 1 << ": " << history[i] << std::endl;
                }
            }
            return true;
        }
        
        return false;
    }
    
    void addToHistory(std::vector<std::string>& history, const std::string& entry, size_t maxSize) {
        history.push_back(entry);
        if (history.size() > maxSize) {
            history.erase(history.begin());
        }
    }
    
    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\n\r");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\n\r");
        return str.substr(first, last - first + 1);
    }
    
    void printFarewell() {
        std::cout << " Goodbye!\n" << std::endl;
    }
};

int main() {
    std::cout << std::fixed << std::setprecision(6);

    ScientificCalculator calc;
    calc.runInteractiveCalculator();
    
    return 0;
}
