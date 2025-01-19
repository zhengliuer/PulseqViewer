#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProgressBar>
#include <QLabel>

#include <ExternalSequence.h>


#define SAFE_DELETE(p) { if(p) { delete p; p = nullptr; } }

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    static const int MAX_RECENT_FILES = 10;

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void Init();
    void InitSlots();
    bool InitSequenceFigure();

    // Slots
    void OpenPulseqFile();

    // Pulseq
    void ClearPulseqCache();
    bool LoadPulseqFile(const QString& sPulseqFilePath);
    bool ClosePulseqFile();

private:
    Ui::MainWindow                       *ui;

    QLabel                               *m_pVersionLabel;
    QProgressBar                         *m_pProgressBar;

    // Pulseq
    QString                              m_sPulseqFilePath;
    QStringList                          m_listRecentPulseqFilePaths;
    std::shared_ptr<ExternalSequence>    m_spPulseqSeq;
    std::vector<SeqBlock*>               m_vecSeqBlocks;
};

#endif // MAINWINDOW_H
