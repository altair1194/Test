#include "calculator.h"
#include <QtWidgets>

Calculator::Calculator(QWidget *parent)
    : QWidget(parent), operand(0.0), waitingForOperand(true)
{
    setStyleSheet("background:#e0e0e0;font-family:'Segoe UI','Roboto','Arial';font-size:14pt;");

    display = new QLineEdit("0");
    display->setReadOnly(true);
    display->setAlignment(Qt::AlignRight);
    display->setMaxLength(15);
    display->setFixedHeight(50);
    display->setStyleSheet("background:white;border:1px solid #ccc;border-radius:4px;padding:4px;");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(5);
    mainLayout->setContentsMargins(5,5,5,5);
    mainLayout->addWidget(display);

    layout = new QGridLayout();
    layout->setSpacing(5);
    mainLayout->addLayout(layout);

    createButtons();
}

void Calculator::createButtons()
{
    auto createBtn=[&](const QString &txt, auto slot){
        QPushButton *b=new QPushButton(txt);
        b->setFixedSize(50,50);
        b->setStyleSheet("QPushButton{background:white;border:1px solid #bbb;border-radius:4px;}"
                         "QPushButton:hover{background:#f5f5f5;}"
                         "QPushButton:pressed{background:#d6d6d6;}");
        connect(b,&QPushButton::clicked,this,slot);
        return b;
    };

    QStringList topRow={"CE","C","\xE2\x86\x90","/"};
    for(int i=0;i<topRow.size();++i){
        QPushButton *btn=nullptr;
        if(topRow[i]=="CE") btn=createBtn(topRow[i],&Calculator::clearEntryClicked);
        else if(topRow[i]=="C") btn=createBtn(topRow[i],&Calculator::clearClicked);
        else if(topRow[i]=="\xE2\x86\x90") btn=createBtn(QString::fromUtf8("\xE2\x86\x90"),&Calculator::backspaceClicked);
        else btn=createBtn(topRow[i],&Calculator::operatorClicked);
        layout->addWidget(btn,0,i);
    }

    QStringList digits={"7","8","9","4","5","6","1","2","3"};
    int pos=0;
    for(int r=1;r<=3;++r){
        for(int c=0;c<3;++c){
            QPushButton *b=createBtn(digits[pos++],&Calculator::digitClicked);
            layout->addWidget(b,r,c);
        }
    }

    QStringList ops={"*","-","+"};
    for(int i=0;i<ops.size();++i){
        QPushButton *b=createBtn(ops[i],&Calculator::operatorClicked);
        layout->addWidget(b,i+1,3);
    }

    QPushButton *signBtn=createBtn("+/-",&Calculator::changeSignClicked);
    layout->addWidget(signBtn,4,0);

    QPushButton *zeroBtn=createBtn("0",&Calculator::digitClicked);
    layout->addWidget(zeroBtn,4,1);

    QPushButton *dotBtn=createBtn(".",&Calculator::dotClicked);
    layout->addWidget(dotBtn,4,2);

    QPushButton *eqBtn=createBtn("=",&Calculator::equalsClicked);
    layout->addWidget(eqBtn,4,3);
}

void Calculator::digitClicked()
{
    QPushButton *clickedButton = qobject_cast<QPushButton*>(sender());
    QString digit = clickedButton->text();
    if (display->text() == "0" && digit == "0" && !display->text().contains('.'))
        return;
    if (waitingForOperand) {
        display->clear();
        waitingForOperand = false;
    }
    display->setText(display->text() + digit);
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

void Calculator::dotClicked()
{
    if (waitingForOperand)
        display->setText("0");
    if (!display->text().contains('.')) {
        display->setText(display->text() + '.');
        waitingForOperand = false;
    }
}

void Calculator::changeSignClicked()
{
    QString text = display->text();
    if (text.startsWith('-'))
        text.remove(0, 1);
    else if (text != "0")
        text.prepend('-');
    display->setText(text);
}

void Calculator::backspaceClicked()
{
    if (waitingForOperand)
        return;
    QString text = display->text();
    text.chop(1);
    if (text.isEmpty()) {
        text = "0";
        waitingForOperand = true;
    }
    display->setText(text);
}

void Calculator::clearEntryClicked()
{
    display->setText("0");
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
    display->setText(QString::number(operand, 'g', 15));
    pendingOperator.clear();
    waitingForOperand = true;
    return true;
}
