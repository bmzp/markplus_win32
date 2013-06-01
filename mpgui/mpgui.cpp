#include "ui_mpgui.h"
#include "mpgui.h"

MpGui::MpGui(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::MpGui)
{
	ui->setupUi(this);
	setWindowFlags( Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint);
	initTitleBar();
	initWorkWidget();
	initStatusBar();
}

MpGui::~MpGui()
{
	delete ui;
}

void MpGui::initTitleBar()
{
	//��ʼ��������
	titleBar = new TitleBar(true, true, true, this);
	ui->mainLayout->addWidget(titleBar);
}

void MpGui::initStatusBar()
{
	//��ʼ��״̬��
	statusBar = new StatusBar();
	ui->mainLayout->addWidget(statusBar);
}

StatusBar* MpGui::getStatusBar()
{
	return this->statusBar;
}

void MpGui::initWorkWidget()
{
	//��ʼ��workwidget
	workWidget = new WorkWidget();
	ui->mainLayout->addWidget(workWidget);
}