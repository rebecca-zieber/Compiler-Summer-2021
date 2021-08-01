#include <iostream>
#include <string>

using namespace std;

template <typename T>
class stack {
    T stuff[20] = {};
    int size = 0;

public:
    void push(T element) {
        stuff[size++] = element;
    }

    T pop() {
        if (size <= 0) {
            return NULL;
        }
        return stuff[--size];
    }

    T top() {
        if (size <= 0) {
            return NULL;
        }
        return stuff[size - 1];
    }

    int stack_size() {
        return this->size;
    }
};

// ---------------------------------

// adding a little comment so that i can test pushing and not pull requesting!

int order_of_ops[22] = {
    '(', 61,
    ')', 62,
    '@', 51,
    '*', 41,
    '/', 42,
    '+', 31,
    '-', 32,
    'E', 13,
    'F', 12,
    'N', 11,
    '$', 0
};

// ---------------------------------

class node {
    int digit;
    char value;
    node* left;
    node* right;

public:
    node(int digit, char val, node* left, node* right) {
        this->digit = digit;
        this->value = val;
        this->left = left;
        this->right = right;
    }

    node(char val) {
        this->digit = -1;
        this->value = val;
        this->left = nullptr;
        this->right = nullptr;
    }

    node(char val, node* left, node* right) {
        this->digit = -1;
        this->value = val;
        this->left = left;
        this->right = right;
    }

    void print_postfix() {
        if (this->left)
            left->print_postfix(); 

        if (this->right) 
            right->print_postfix();

        switch (this->value) {
            case 'N':  cout << "Push " << this->digit << "\n"; break;
            case '+':  cout << "Sum\n";   break;
            case '-':  cout << "Minus\n"; break;
            case '*':  cout << "Mul\n";   break;
            case '/':  cout << "Div\n";   break;
            case '@':  cout << "Neg\n";   break;
            case 'E': break;
            case 'F': break;
            default:  
                cerr << "Unknown node " << this->value << "\n";  
                exit(1);
        }
    }

    int get_precedence() {
        int index = 0;
        while (order_of_ops[index] != this->value) {
            index += 2;
        }

        return order_of_ops[++index]/10;
    }

    char get_char() {
        return this->value;
    }

    int get_digit() {
        return this->digit;
    }

    void set_digit(int digit) {
        this->digit = digit;
    }

    void peek_left() {
        cout << this->left->get_char() << " char\n";
        cout << this->left->get_digit() << " digit\n";
    }

    void peek_right() {
        cout << this->right->get_char() << " char\n";
        cout << this->right->get_digit() << " digit\n";
    }
};

// -------------------------------

stack<node*> digit_stack;

const int OP_STACK_SIZE = 10;
char operator_stack[OP_STACK_SIZE] = {};
int operator_index = 0; // points to the next available slot

char top_operator() {
    char top = operator_stack[--operator_index];
    operator_index++;
    return top;
}

void push_operator(char op) {
    operator_stack[operator_index++] = op;

    if (operator_index == OP_STACK_SIZE) {
        cout << "Operator stack full.\n";
        operator_index--;
    }
}

char pop_operator() {
    char top = top_operator();
    operator_stack[--operator_index] = '\0';
    return top;
}

void print_op_stack() {
    for (int i = 0; i < OP_STACK_SIZE; i++) {
        cout << operator_stack[i] << '\n';
    }
}

// --------------------------------

void reduce_negation() {
    node* num1 = digit_stack.pop();
    node* result = new node(-num1->get_digit(), '@', num1, nullptr);
    digit_stack.push(result);
    (void)pop_operator();
}

void reduce_to_number(int num) {
    node* temp = new node(num, 'N', nullptr, nullptr);
    digit_stack.push(temp);
}

void reduce_number_to_factor() {
    node* num2 = digit_stack.pop();
    node* num1 = digit_stack.pop();
    node* result;
    node* op;
    char op_char = pop_operator();

    /*if (num1->get_precedence() == num2->get_precedence()) {*/    
    // this messes up the arithmatic between nums that have been neg'ed and normal positive nums
        op = new node(op_char, num1, num2);
        if (op_char == '*') {
            result = new node((num1->get_digit() * num2->get_digit()), 'F', op, nullptr);
        }
        else if (op_char == '/') {
            result = new node((num1->get_digit() / num2->get_digit()), 'F', op, nullptr);
        }
        else {
            cout << "Cannot reduce with type " << op_char << endl;
            return;
        }
        digit_stack.push(result);
    /*}
    else {
        cout << "Cannot reduce type " << num1->get_precedence() << " with type " << num2->get_precedence() << "\n";
        return;
    }*/
}

void reduce_factor_to_sum() {
    node* num2 = digit_stack.pop();
    node* num1 = digit_stack.pop();
    node* result;
    node* op;
    char op_char = pop_operator();

    /*if (num1->get_precidence() == num2->get_precidence()) {*/
        op = new node(op_char, num1, num2);
        if (op_char == '+') {
            result = new node((num1->get_digit() + num2->get_digit()), 'E', op, nullptr);
        }
        else if (op_char == '-') {
            result = new node((num1->get_digit() - num2->get_digit()), 'E', op, nullptr);
        }
        else {
            cout << "Cannot reduce with type " << op_char << endl;
            return;
        }
        digit_stack.push(result);
    /*}
    else {
        cout << "Cannot reduce type " << num1->get_precidence() << " with type " << num2->get_precidence() << "\n";
        return;
    } */
}

// --------------------------------

int main() {
    int index = 0;
    string expression = "";
    std::cout << "Enter an expression:\n";
    cin >> expression;
    char current = expression[index];

    bool seen_paren = false;

    digit_stack.push(new node('$'));    // push OP not this wordy ass shit
    push_operator('$');

    cout << ">> " << expression << "\n";

    int q = 0;
    for (current = expression[index]; current != '\0'; current = expression[++index]) {
        if (current == ' ') {
            continue;
        }

        if (q == 0) {
            if (current == '(') {
                seen_paren = true;
                push_operator(current);
                continue;
            }

            if (current == '-') {
                if (top_operator() == '@') {
                    (void)pop_operator();
                }
                else {
                    push_operator('@');
                }
                continue;
            }

            if (current >= '0' && current <= '9') {
                int num = 0;
                while (current >= '0' && current <= '9') {
                    num = num * 10 + current - '0';
                    current = expression[++index];
                }
                index--;
                reduce_to_number(num);
                q = 1;

                while (top_operator() == '@') { 
                    reduce_negation();
                }
            }
            else {
                cout << "Can't read " << current << endl;
                return -1;
            }
        }
        else {
            if (current == ')') {
                if (!seen_paren) {
                    cout << "Improper paren syntax -- can't start with closing paren\n";
                    return -1;
                }

                while (top_operator() != '(') {
                    char top = top_operator();
                    switch (top) {
                    case '-':
                    case '+':
                        reduce_factor_to_sum();
                        break;
                    case '/':
                    case '*':
                        reduce_number_to_factor();
                        break;
                    }
                }

                (void) pop_operator();

                while (top_operator() == '@') {
                    reduce_negation();
                }

                continue;
            }

            char top = top_operator();
            if (current == '+' || current == '-' 
                || current == '*' || current == '/') {
                switch (top) {
                    case '$':
                        break;
                    case '-':
                    case '+':
                        if (current == '+' || current == '-') {
                            reduce_factor_to_sum();
                        }
                        break;
                    case '/':
                    case '*':
                        reduce_number_to_factor();
                        break;
                }
                push_operator(current);
                q = 0;
            }
            else {
                cout << "Operator error: " << current << endl;
                return -1;
            }
        }
    }

    if (q == 1) {
        while (top_operator() != '$') {
            char top = top_operator();
            switch (top) {
            case '-':
            case '+':
                reduce_factor_to_sum();
                break;
            case '/':
            case '*':
                reduce_number_to_factor();
                break;
            case '(':
                cout << "Error -- unclosed paren\n";
                return -1;
            case '@':
                reduce_negation();
                break;
            }
        }
        node* expression_tree = digit_stack.pop();
        expression_tree->print_postfix();
    }
    else {
        cout << "Not a valid expression: " << expression << endl;
        return -1;
    }


    return 0;
}