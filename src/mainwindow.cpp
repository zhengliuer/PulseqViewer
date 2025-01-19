#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_sPulseqFilePath("")
    , m_spPulseqSeq(std::make_shared<ExternalSequence>())
{
    ui->setupUi(this);

    Init();

    m_listRecentPulseqFilePaths.resize(10);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Init()
{
    InitSlots();
    InitSequenceFigure();
}

void MainWindow::InitSlots()
{
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::OpenPulseqFile);
    connect(ui->actionCloseFile, &QAction::triggered, this, &MainWindow::ClosePulseqFile);
}

bool MainWindow::InitSequenceFigure()
{
    QCPAxisRect *rfRect = new QCPAxisRect(ui->customPlot);
    QCPAxisRect *gzRect = new QCPAxisRect(ui->customPlot);
    QCPAxisRect *gyRect = new QCPAxisRect(ui->customPlot);
    QCPAxisRect *gxRect = new QCPAxisRect(ui->customPlot);
    QCPAxisRect *adcRect = new QCPAxisRect(ui->customPlot);

    ui->customPlot->plotLayout()->clear();
    ui->customPlot->plotLayout()->addElement(0, 0, rfRect);
    ui->customPlot->plotLayout()->addElement(1, 0, gzRect);
    ui->customPlot->plotLayout()->addElement(2, 0, gyRect);
    ui->customPlot->plotLayout()->addElement(3, 0, gxRect);
    ui->customPlot->plotLayout()->addElement(4, 0, adcRect);

    rfRect->setupFullAxesBox(true);
    rfRect->axis(QCPAxis::atLeft)->setLabel("RF (Hz)");
    gzRect->setupFullAxesBox(true);
    gzRect->axis(QCPAxis::atLeft)->setLabel("GZ (Hz/m)");
    gyRect->setupFullAxesBox(true);
    gyRect->axis(QCPAxis::atLeft)->setLabel("GY (Hz/m)");
    gxRect->setupFullAxesBox(true);
    gxRect->axis(QCPAxis::atLeft)->setLabel("GX (Hz/m)");
    adcRect->setupFullAxesBox(true);
    adcRect->axis(QCPAxis::atLeft)->setLabel("ADC");

    // share the same time axis
    rfRect->axis(QCPAxis::atBottom)->setLabel("Time (us)");
    gzRect->axis(QCPAxis::atBottom)->setLabel("Time (us)");
    gyRect->axis(QCPAxis::atBottom)->setLabel("Time (us)");
    gxRect->axis(QCPAxis::atBottom)->setLabel("Time (us)");
    adcRect->axis(QCPAxis::atBottom)->setLabel("Time (us)");

    // Hide all time axis but the last one
    rfRect->axis(QCPAxis::atBottom)->setVisible(false);
    gzRect->axis(QCPAxis::atBottom)->setVisible(false);
    gyRect->axis(QCPAxis::atBottom)->setVisible(false);
    gxRect->axis(QCPAxis::atBottom)->setVisible(false);

    return true;
}

void MainWindow::OpenPulseqFile()
{
    m_sPulseqFilePath = QFileDialog::getOpenFileName(
        this,
        "Selec a Pulseq File",                           // Dialog title
        QDir::currentPath(),                    // Default open folder
        "Text Files (*.seq);;All Files (*)"  // File filter
        );

    if (!m_sPulseqFilePath.isEmpty())
    {
        if (!LoadPulseqFile(m_sPulseqFilePath))
        {
            m_sPulseqFilePath.clear();
            std::cout << "LoadPulseqFile failed!\n";
        }
    }
}

void MainWindow::ClearPulseqCache()
{
    if (nullptr != m_spPulseqSeq)
    {
        m_spPulseqSeq->reset();
        for (uint16_t ushBlockIndex=0; ushBlockIndex < m_vecSeqBlocks.size(); ushBlockIndex++)
        {
            SAFE_DELETE(m_vecSeqBlocks[ushBlockIndex]);
        }
        m_vecSeqBlocks.clear();
        std::cout << m_sPulseqFilePath.toStdString() << " Closed\n";
    }
}

bool MainWindow::LoadPulseqFile(const QString& sPulseqFilePath)
{
    ClearPulseqCache();
    if (!m_spPulseqSeq->load(sPulseqFilePath.toStdString()))
    {
        std::stringstream sLog;
        sLog << "Load " << sPulseqFilePath.toStdString() << " failed!";
        std::cout << sLog.str() << "\n";
        QMessageBox::critical(this, "File Error", sLog.str().c_str());
        return false;
    }
    const int64_t& lSeqBlockNum = m_spPulseqSeq->GetNumberOfBlocks();
    std::cout << lSeqBlockNum << " blocks detected!\n";
    m_vecSeqBlocks.resize(lSeqBlockNum);
    for (uint16_t ushBlockIndex=0; ushBlockIndex < lSeqBlockNum; ushBlockIndex++)
    {
        m_vecSeqBlocks[ushBlockIndex] = m_spPulseqSeq->GetBlock(ushBlockIndex);
        if (!m_spPulseqSeq->decodeBlock(m_vecSeqBlocks[ushBlockIndex]))
        {
            std::stringstream sLog;
            sLog << "Decode SeqBlock failed, block index: " << ushBlockIndex;
            QMessageBox::critical(this, "File Error", sLog.str().c_str());
            return false;
        }
    }

    return true;
}

bool MainWindow::ClosePulseqFile()
{
    ClearPulseqCache();

    return true;
}
