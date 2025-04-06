#include <iostream>
#include <stack>
#include <cctype>
#include <cmath>
#include <stdexcept>
#include <fstream>

using namespace std;

bool checkParentheses(const string& input) {
    int opened = 0, closed = 0;
    for (char c : input) {
        if (c == '(')
            opened++;
        else if (c == ')')
            closed++;
    }
    if (opened != closed)
        throw runtime_error("Parentheses are not matched");
    return true;
}

int precedence(char op) {
    switch (op) {
        case '_': return 4;
        case '^': return 3;
        case '*': case '/': return 2;
        case '+': case '-': return 1;
        default: return 0;
    }
}

bool isOperator(char a) {
    return a == '+' || a == '-' || a == '*' || a == '/' || a == '^' || a == '_';
}

double applyOperator(double a, double b, char op) {
    switch (op) {
        case '_': return -b;
        case '^': return pow(a, b);
        case '*': return a * b;
        case '/':
            if (b != 0)
                return a / b;
            else
                throw domain_error("Division by zero");
        case '+': return a + b;
        case '-': return a - b;
        default: throw runtime_error("Unknown operator");
    }
}

double evaluate(string input) {
    stack<double> values;
    stack<char> operators;
    bool negative = false;

    for (size_t i = 0; i < input.length(); i++) {
        if (input[i] == ' ')
            continue;

        if (input[i] == '(') {
            operators.push(input[i]);
            if (i > 0 && input[i - 1] == '-') {
                negative = true;
            }
        } else if (input[i] == ')') {
            while (!operators.empty() && operators.top() != '(') {
                char op = operators.top();
                operators.pop();
                double num2 = values.top();
                values.pop();
                double num1 = (op != '_') ? values.top() : 0;
                if (op != '_') values.pop();
                values.push(applyOperator(num1, num2, op));
            }
            if (!operators.empty()) operators.pop();
        } else if (isdigit(input[i]) || input[i] == '.') {
            string num;
            while (isdigit(input[i]) || input[i] == '.') {
                num += input[i];
                i++;
            }
            i--;
            double value = stod(num);
            if (negative) {
                value = -value;
                negative = false;
            }
            values.push(value);
        } else if (isOperator(input[i])) {
            while (!operators.empty() && precedence(operators.top()) >= precedence(input[i])) {
                char op = operators.top();
                operators.pop();
                double num2 = values.top();
                values.pop();
                double num1 = (op != '_') ? values.top() : 0;
                if (op != '_') values.pop();
                values.push(applyOperator(num1, num2, op));
            }
            operators.push(input[i]);
        } else {
            throw runtime_error(string(1, input[i]) + " is incorrect");
        }
    }

    while (!operators.empty()) {
        char op = operators.top();
        operators.pop();
        double num2 = values.top();
        values.pop();
        double num1 = (op != '_') ? values.top() : 0;
        if (op != '_') values.pop();
        values.push(applyOperator(num1, num2, op));
    }

    return values.top();
}

int main() {
    ofstream file("calculations.txt", ios::app);
    if (!file.is_open()) {
        cout << "Error opening file!" << endl;
        return 1;
    }

    string expression;
    while (true) {
        cout << "Enter a mathematical expression (or type 'exit' to quit): ";
        getline(cin, expression);

        if (expression == "exit")
            break;

        try {
            if (checkParentheses(expression)) {
                double result = evaluate(expression);
                cout << "Result: " << result << endl;
                file << "Expression: " << expression << " Result: " << result << endl;
            }
        } catch (const exception& e) {
            cout << "Error: " << e.what() << endl;
        }
    }

    file.close();
    return 0;
}
