#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QIcon>
#include <QPixmap>
#include <string>
#include <QMessageBox>
#include <math.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , pending_oper(false)
    , after_oper(false)
    , no_calc(true)
    , equal_done(false)
    , unary_op(false)
    , divideByZero(false)
    , total(0.0)
{
    ui->setupUi(this);
    this->setWindowTitle("Calculator");
    this->setFixedSize(this->width(), this->height());

    ui->lineEdit->setReadOnly(true);
    ui->lineEdit->setStyleSheet("color:white;");
    ui->lineEdit->setAlignment(Qt::AlignRight);
    ui->lineEdit->setText("0");
    ui->lineEdit->setMaxLength(15);
    ui->lineEdit->setFrame(false);

    ui->lineEdit_2->setReadOnly(true);
    ui->lineEdit_2->setStyleSheet("color:white;");
    ui->lineEdit_2->setAlignment(Qt::AlignRight);
    ui->lineEdit_2->setFrame(false);

    ui->change_sign->setText(tr("\302\261"));
    ui->power_2->setText(tr("x\u00B2"));
    ui->times->setText(tr("\303\227"));
    ui->divide->setText(tr("\u00F7"));
    ui->sqrt->setText(tr("\u221A"));
//    ui->one_divides->setText(tr("\u215F x"));
    ui->backspsce->setIcon(QIcon(QPixmap(":/Icons/Icons/backspace.png")));

    connect(ui->digits,          SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(digitClicked(QAbstractButton*)));
    connect(ui->operators,       SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(operatorClicked(QAbstractButton*)));
    connect(ui->unary_operators, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(unaryOperatorClicked(QAbstractButton*)));

    //connect(ui->zero,        SIGNAL(clicked()), this, SLOT(digitClicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::digitClicked(QAbstractButton* button)
{
    if(divideByZero)
        restoreFromDivisionByZero();

    //QPushButton* button = qobject_cast<QPushButton*>(sender());
    if(button->text() == "0" && lineEdit_empty() )
        return;

    if(lineEdit_empty())
        ui->lineEdit->clear();

    if(equal_done)
    {
        clearAll();
        ui->lineEdit->clear();
    }

    if(unary_op && ! equal_done)
    {
        ui->lineEdit_2->setText(text_up_an_operator);
        ui->lineEdit->setText(button->text());

        unary_op = false;
    }
    else if(pending_oper)
    {
        ui->lineEdit->clear();

        if(button->text() == ".")
            ui->lineEdit->setText("0.");

        if( ! ui->lineEdit->text().contains('.'))
            ui->lineEdit->setText(ui->lineEdit->text() + button->text());
        else if(button->text() != ".")
            ui->lineEdit->setText(ui->lineEdit->text() + button->text());

        pending_oper = false;
        after_oper = true;
    }
    else
    {
        if( button->text() == "." && lineEdit_empty())
        {
            ui->lineEdit->setText("0.");
        }

        if( ! ui->lineEdit->text().contains('.'))
            ui->lineEdit->setText(ui->lineEdit->text() + button->text());
        else if(button->text() != ".")
            ui->lineEdit->setText(ui->lineEdit->text() + button->text());
    }
}

void MainWindow::operatorClicked(QAbstractButton* button)
{
    if(divideByZero)
        return;

    QString str = ui->lineEdit->text();

    if(str.endsWith("."))
    {
        str.chop(1);

        ui->lineEdit->setText(str);
    }

    if(unary_op)
    {
        calculate(str.toDouble(), oper);

        ui->lineEdit_2->setText(ui->lineEdit_2->text() + " " + button->text() + " ");

        equal_done = false;
        unary_op = false;

        oper = button->text();
    }
    else if(equal_done)
    {
        ui->lineEdit_2->setText(str + " " + button->text() + " ");

        equal_done = false;
        oper = button->text();
    }
    else if(pending_oper)
    {
        QString str = ui->lineEdit_2->text();

        str.chop(3);

        ui->lineEdit_2->setText(str + " " + button->text() + " ");

        oper = button->text();
    }
    else if(after_oper)
    {
        calculate(str.toDouble(), oper);

        ui->lineEdit_2->setText(ui->lineEdit_2->text() + str + " " + button->text() + " ");
        ui->lineEdit->setText(QString::number(total));

        oper = button->text();

        if(str == "0")
            cantDivideByZero();
    }
    else if( ! lineEdit_empty() )
    {
        if(ui->lineEdit_2->text().isEmpty())
            ui->lineEdit_2->setText(str);

        ui->lineEdit_2->setText(str + " " + button->text() + " ");

        oper = button->text();

        calculate(str.toDouble(), oper);

    }

    pending_oper = true;
    after_oper = false;

    text_up_an_operator = ui->lineEdit_2->text();
}

void MainWindow::unaryOperatorClicked(QAbstractButton* button)
{
    QString operator_clicked = button->text();


    double num = ui->lineEdit->text().toDouble();
    double oper_num;
    QString str_op;

    if(operator_clicked == tr("x\u00B2")) // power_2
    {
        oper_num = pow(num, 2);
        str_op = "sqr(";
    }
    else if(operator_clicked == tr("\u221A")) // sqrt
    {
        oper_num = sqrt(num);
        str_op = tr("\u221A") + "(";
    }
    else if(operator_clicked == "1/x") // one_divides
    {
        if(num == 0)
        {
            ui->lineEdit_2->setText("1/(0)");
            cantDivideByZero();
            return;
        }

        oper_num = 1/num;
        str_op = "1/(";
    }

    ui->lineEdit->setText(QString::number(oper_num));

    QString str = str_op + QString::number(num) + ")";
    QString current_text = ui->lineEdit_2->text();
    static QString s_str;
    static QString s_current_text;

    if( ! unary_op)
    {
        s_str = str;
        s_current_text = current_text;
    }

    if(equal_done)
    {
        ui->lineEdit_2->setText(s_str);

        s_current_text = "";

        total = 0;

        // ui->lineEdit->setText(QString::number(total));

        equal_done = false;
    }
    else if(unary_op)
    {
        s_str = str_op + s_str + ")";

        ui->lineEdit_2->setText(s_current_text + s_str);
    }
    else
    {
        ui->lineEdit_2->setText(ui->lineEdit_2->text() + str);
    }

    unary_op = true;

}

void MainWindow::on_equals_clicked()
{
    static QString right_operand;

    if(divideByZero)
    {
        restoreFromDivisionByZero();
        return;
    }

    QString str = ui->lineEdit->text();

    if(((str == "0" && pending_oper) || (str == "0" && after_oper)) && oper == ui->divide->text() )
    {
        pending_oper = after_oper = false;

        cantDivideByZero();
        return;
    }

    if(str.endsWith("."))
    {
        str.chop(1);
        ui->lineEdit->setText(str);
    }

    if(! equal_done )
    {
        right_operand = str;
    }

    if( equal_done )
    {
        if( ! oper.isEmpty())
            calculate(right_operand.toDouble(), oper);

        if( ! oper.isEmpty())
            ui->lineEdit_2->setText(ui->lineEdit->text() + " " + oper + " " + right_operand + " = ");

        ui->lineEdit->setText(QString::number(total));
    }
    else
    {
        calculate(str.toDouble(), oper);

        if(unary_op)
        {
            ui->lineEdit_2->setText(ui->lineEdit_2->text() + " = ");
        }
        else if(pending_oper)
        {
            QString str = ui->lineEdit_2->text();
            QString str1 = ui->lineEdit->text();

            ui->lineEdit_2->setText(str + str1 + " = ");
        }
        else if(after_oper)
        {
            if(unary_op)
                ui->lineEdit_2->setText(ui->lineEdit_2->text() + " = ");
            else
                ui->lineEdit_2->setText(ui->lineEdit_2->text() + ui->lineEdit->text() + " = ");

//            pending_oper = true;
            after_oper = false;
        }
        else if( ! lineEdit_empty() )
        {
            if(ui->lineEdit_2->text().isEmpty())
                ui->lineEdit_2->setText(ui->lineEdit->text());

            ui->lineEdit_2->setText(ui->lineEdit->text() + " = ");
        }

        ui->lineEdit->setText(QString::number(total));

        equal_done = true;
        unary_op = false;
    }
}

void MainWindow::calculate(double rigt_operand, QString const& oper)
{
    if(oper == "+")
    {
        total += rigt_operand;
    }
    else if(oper == "-")
    {
        if( no_calc )
        {
            total = rigt_operand;

            rigt_operand = 0;
        }

        total -= rigt_operand;
    }
    else if(oper == ui->times->text())
    {
        if( no_calc )
        {
            total = rigt_operand;

            rigt_operand = 1;
        }

        total *= rigt_operand;
    }
    else if(oper == ui->divide->text())
    {
        if(rigt_operand == 0.0)
            return;

        if( no_calc)
        {
            total = rigt_operand;

            rigt_operand = 1;
        }

        total /= rigt_operand;
    }
    else
        total = rigt_operand;

    no_calc = false;
}

void MainWindow::keyPressEvent(QKeyEvent* e)
{
    switch (e->key())
    {
    case Qt::Key_0:
        digitClicked(ui->zero);
        break;

    case Qt::Key_1:
        digitClicked(ui->one);
        break;

    case Qt::Key_2:
        digitClicked(ui->two);
        break;

    case Qt::Key_3:
        digitClicked(ui->three);
        break;

    case Qt::Key_4:
        digitClicked(ui->four);
        break;

    case Qt::Key_5:
        digitClicked(ui->five);
        break;

    case Qt::Key_6:
        digitClicked(ui->six);
        break;

    case Qt::Key_7:
        digitClicked(ui->seven);
        break;

    case Qt::Key_8:
        digitClicked(ui->eight);
        break;

    case Qt::Key_9:
        digitClicked(ui->nine);
        break;

    case Qt::Key_Period:
        digitClicked(ui->dot);
        break;

    case Qt::Key_Plus:
        operatorClicked(ui->plus);
        break;

    case Qt::Key_Asterisk:
        operatorClicked(ui->times);
        break;

    case Qt::Key_Slash:
        operatorClicked(ui->divide);
        break;

    case Qt::Key_Minus:
        operatorClicked(ui->minus);
        break;


    case Qt::Key_Return:
    case Qt::Key_Enter:
        on_equals_clicked();
        break;

    case Qt::Key_Backspace:
        on_backspsce_clicked();
        break;

    case Qt::Key_Delete:
        clearAll();
        break;
    }
}

bool MainWindow::lineEdit_empty()
{
    return ui->lineEdit->text() == "0" or ui->lineEdit->text() == "";
}

void MainWindow::on_clean_evryth_clicked()
{
    if(divideByZero)
    {
        restoreFromDivisionByZero();
        return;
    }

    clearAll();
}

void MainWindow::on_clean_input_clicked()
{
    if(divideByZero)
    {
        restoreFromDivisionByZero();
        return;
    }

    if(equal_done)
        clearAll();
    else
        ui->lineEdit->setText("0");
}

void MainWindow::on_backspsce_clicked()
{
    if(divideByZero)
    {
        restoreFromDivisionByZero();
        return;
    }

    if(! lineEdit_empty() && ! equal_done && ! pending_oper)
    {
        QString str = ui->lineEdit->text();

        str.chop(1);

        if(str.isEmpty())
            ui->lineEdit->setText("0");
        else
            ui->lineEdit->setText(str);
    }
}

void MainWindow::on_change_sign_clicked()
{
    if(! lineEdit_empty())
        ui->lineEdit->setText(QString::number(ui->lineEdit->text().toDouble() * (-1)));
}

void MainWindow::clearAll()
{
    ui->lineEdit->setText("0");
    ui->lineEdit->setMaxLength(15);
    ui->lineEdit_2->clear();

    pending_oper = false;
    after_oper = false;
    no_calc = true;
    equal_done = false;
    unary_op = false;

    oper = "";

    total = 0;
}

void MainWindow::cantDivideByZero()
{
    ui->lineEdit->setMaxLength(-1);
    ui->lineEdit->setText("Can not divide by zero");

    ui->divide->setEnabled(false);
    ui->plus->setEnabled(false);
    ui->minus->setEnabled(false);
    ui->times->setEnabled(false);
    ui->dot->setEnabled(false);
    ui->change_sign->setEnabled(false);
    ui->power_2->setEnabled(false);
    ui->sqrt->setEnabled(false);
    ui->one_divides->setEnabled(false);
    ui->module->setEnabled(false);

    divideByZero = true;
}

void MainWindow::restoreFromDivisionByZero()
{
    clearAll();

    ui->divide->setEnabled(true);
    ui->plus->setEnabled(true);
    ui->minus->setEnabled(true);
    ui->times->setEnabled(true);
    ui->dot->setEnabled(true);
    ui->change_sign->setEnabled(true);
    ui->power_2->setEnabled(true);
    ui->sqrt->setEnabled(true);
    ui->one_divides->setEnabled(true);
    ui->module->setEnabled(true);

    divideByZero = false;
}
