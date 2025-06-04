#include "calculator.h"
#include <QtWidgets>

Calculator::Calculator(QWidget *parent)
    : QWidget(parent), operand(0.0), waitingForOperand(true)
{
    display = new QLineEdit("0");
    display->setReadOnly(true);
    display->setAlignment(Qt::AlignRight);
    display->setMaxLength(15);

    layout = new QGridLayout(this);
    layout->addWidget(display, 0, 0, 1, 4);

    createButtons();
}

void Calculator::createButtons()
{
    QStringList digits = {"7","8","9","4","5","6","1","2","3","0"};
    int pos = 0;
    for(int row=1; row<=4; ++row) {
        for(int col=0; col<3; ++col) {
            if(pos >= digits.size()) break;
            QPushButton *button = new QPushButton(digits[pos++]);
            connect(button, &QPushButton::clicked, this, &Calculator::digitClicked);
            layout->addWidget(button, row, col);
        }
    }

    QStringList ops = {"+","-","*","/"};
    for(int i=0;i<ops.size();++i) {
        QPushButton *button = new QPushButton(ops[i]);
        connect(button, &QPushButton::clicked, this, &Calculator::operatorClicked);
        layout->addWidget(button, i+1, 3);
    }

    QPushButton *eqButton = new QPushButton("=");
    connect(eqButton, &QPushButton::clicked, this, &Calculator::equalsClicked);
    layout->addWidget(eqButton, 5, 2);

    QPushButton *clrButton = new QPushButton("C");
    connect(clrButton, &QPushButton::clicked, this, &Calculator::clearClicked);
    layout->addWidget(clrButton, 5, 0, 1, 2);
}

void Calculator::digitClicked()
{
    QPushButton *clickedButton = qobject_cast<QPushButton*>(sender());
    int digitValue = clickedButton->text().toInt();
    if (display->text() == "0" && digitValue == 0)
        return;
    if (waitingForOperand) {
        display->clear();
        waitingForOperand = false;
    }
    display->setText(display->text() + QString::number(digitValue));
}

void Calculator::operatorClicked()
{
    QPushButton *clickedButton = qobject_cast<QPushButton*>(sender());
    QString clickedOperator = clickedButton->text();
    double displayValue = display->text().toDouble();

    if (!pendingOperator.isEmpty()) {
        if (!equalsClicked()) {
            return;
        }
    } else {
        operand = displayValue;
    }

    pendingOperator = clickedOperator;
    waitingForOperand = true;
}

void Calculator::abortOperation()
{
    clearClicked();
    display->setText("####");
}

void Calculator::clearClicked()
{
    display->setText("0");
    waitingForOperand = true;
    pendingOperator.clear();
    operand = 0.0;
}

bool Calculator::equalsClicked()
{
    double operand2 = display->text().toDouble();

    if (pendingOperator == "+")
        operand += operand2;
    else if (pendingOperator == "-")
        operand -= operand2;
    else if (pendingOperator == "*")
        operand *= operand2;
    else if (pendingOperator == "/") {
        if (operand2 == 0.0) {
            abortOperation();
            return false;
        }
        operand /= operand2;
    }
    display->setText(QString::number(operand));
    pendingOperator.clear();
    waitingForOperand = true;
    return true;
}
