#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QWidget>
#include <QLineEdit>
#include <QGridLayout>
#include <QPushButton>

class Calculator : public QWidget
{
    Q_OBJECT
public:
    explicit Calculator(QWidget *parent = nullptr);

private slots:
    void digitClicked();
    void operatorClicked();
    bool equalsClicked();
    void clearClicked();

private:
    void createButtons();
    void abortOperation();

    QLineEdit *display;
    double operand;
    QString pendingOperator;
    bool waitingForOperand;
    QGridLayout *layout;
};

#endif // CALCULATOR_H
