#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAbstractButton>
#include <QKeyEvent>

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void digitClicked(QAbstractButton* button);
    void operatorClicked(QAbstractButton* button);
    void unaryOperatorClicked(QAbstractButton* button);

    void on_equals_clicked();

    void on_clean_evryth_clicked();

    void on_backspsce_clicked();

    void on_clean_input_clicked();

    void on_change_sign_clicked();

private:
    Ui::MainWindow *ui;

    void keyPressEvent(QKeyEvent* e);

    bool lineEdit_empty();
    void calculate(double rigt_operand, QString const& oper);
    void clearAll();
    void cantDivideByZero();
    void restoreFromDivisionByZero();

    QString oper;
    QString text_up_an_operator;

    bool pending_oper;
    bool after_oper;
    bool no_calc;
    bool equal_done;
    bool unary_op;
    bool divideByZero;

    double total;
};
#endif // MAINWINDOW_H
