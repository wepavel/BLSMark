#include "productsform.h"
#include "ui_productsform.h"


ProductsForm::ProductsForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ProductsForm)
{
    ui->setupUi(this);
    productsRemaindModel = new ProductsRemaindModel(this);
    ui->tv_product_remaind->setModel(productsRemaindModel);
    ui->tv_product_remaind->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

ProductsForm::~ProductsForm()
{
    delete ui;
}
