// ----- Headers --------------------------------------------------------------
#include "mainwindow.h"
#include "ui_mainwindow.h"

// ----- Constructor / Destructor ---------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    imageData(MyImage("Image Data"))
{
    ui->setupUi(this);

    // --- Build the application components ---
    buildDirectories();
    buildMenu();
    buildComboBoxes();
    buildLCDs();
    buildSliderBars();
    buildGraphics();

    // TESTING
    openDefault();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ----- Build Methods --------------------------------------------------------
void MainWindow::buildResources()
{
    imageFileList.clear();
    imageFileList.append("Lenna (small)");
    imageFileList.append("Lenna (medium)");
    imageFileList.append("Lenna (large)");
    imageFileList.append("Autobot");
    imageFileList.append("Decepticon");
    imageFileList.append("Halo");
    imageFileList.append("University of Tennessee");
    imageFileList.append("Astronaut");
    imageFileList.append("Astronaut (resized)");
    imageFileList.append("Test");

    imageFilterList.clear();
    imageFilterList.append("No Filter (default)");
    imageFilterList.append("Low  Pass: Ideal");
    imageFilterList.append("Low  Pass: Butterworth");
    imageFilterList.append("Low  Pass: Gaussian");
    imageFilterList.append("High Pass: Ideal");
    imageFilterList.append("High Pass: Butterworth");
    imageFilterList.append("High Pass: Gaussian");
}

void MainWindow::buildDirectories()
{
    QString desktopPath = QDir::homePath() + QString("/Desktop");
    defaultDirectory = QDir(desktopPath);
}

void MainWindow::buildMenu()
{
    // --- New Menu Bars ---
    fileMenu = menuBar()->addMenu(tr("&File"));
    helpMenu = menuBar()->addMenu(tr("&Help"));

    // --- New Menu Actions ---
    openDefaultAction = new QAction(tr("Open &Default Image"), this);
    openAction = new QAction(tr("&Open from File"), this);
    resetAction = new QAction(tr("&Reset Image"), this);
    saveAction = new QAction(tr("&Save"), this);
    saveAsAction = new QAction(tr("Save &As..."), this);
    closeAction = new QAction(tr("&Close"), this);
    exitAction = new QAction(tr("&Exit"), this);
    aboutAction = new QAction(tr("&About This Application"), this);
    aboutQtAction = new QAction(tr("&About Qt"), this);
    aboutAuthorAction = new QAction(tr("&About Author"), this);

    // --- Set File Menu Shortcuts ---
    openDefaultAction->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_O));
    openAction->setShortcut(QKeySequence::Open);
    resetAction->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_R));
    saveAction->setShortcut(QKeySequence::Save);
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    closeAction->setShortcut(QKeySequence::Close);
    exitAction->setShortcut(QKeySequence::Quit);

    // --- Connect menu actions to slots ---
    connect(openDefaultAction, SIGNAL(triggered()), this, SLOT(openDefault()));
    connect(openAction, SIGNAL(triggered()), this, SLOT(open()));
    connect(resetAction, SIGNAL(triggered()), this, SLOT(reset()));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));
    connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveAs()));
    connect(closeAction, SIGNAL(triggered()), this, SLOT(close()));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(quit()));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
    connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(aboutAuthorAction, SIGNAL(triggered()), this, SLOT(aboutAuthor()));

    // --- Add the menu actions to the menu bar---
    fileMenu->addAction(openDefaultAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(resetAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);
    fileMenu->addAction(closeAction);
    fileMenu->addAction(exitAction);
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(aboutQtAction);
    helpMenu->addAction(aboutAuthorAction);
}

void MainWindow::buildComboBoxes()
{
    buildResources();

    ui->comboBoxImageSelector->addItems(imageFileList);
    connect(ui->comboBoxImageSelector, SIGNAL(activated(int)), this, SLOT(openDefault()));

    ui->comboBoxFilterSelector->addItems(imageFilterList);
    connect(ui->comboBoxFilterSelector, SIGNAL(activated(int)), this, SLOT(applyFilter()));
}

void MainWindow::buildSliderBars()
{
    // --- Connect the color sliders to their LCD value slot ---
    connect(ui->sliderA, SIGNAL(valueChanged(int)), this, SLOT(updateSliderA()));
    connect(ui->sliderB, SIGNAL(valueChanged(int)), this, SLOT(updateSliderB()));

    // --- Connect the color sliders to the color remap slots ---
    connect(ui->sliderA, SIGNAL(sliderReleased()), this, SLOT(applyFilter()));
    connect(ui->sliderB, SIGNAL(sliderReleased()), this, SLOT(applyFilter()));

    // --- Set the range of the color sliders ---
    int tmpMin = 0;
    int tmpMax = 100;

    ui->sliderA->setRange(tmpMin, tmpMax);
    ui->sliderB->setRange(tmpMin, tmpMax);

    ui->sliderA->setValue(tmpMax);
    ui->sliderB->setValue(tmpMax);
}

void MainWindow::buildLCDs()
{
    ui->lcdA->setDigitCount(3);
    ui->lcdB->setDigitCount(3);
    ui->lcdSize1->setDigitCount(4);
    ui->lcdSize2->setDigitCount(4);
    ui->lcdSize3->setDigitCount(4);
    ui->lcdSize4->setDigitCount(4);
}

void MainWindow::buildGraphics()
{
    item = new QGraphicsPixmapItem;

    rawImageScene = new QGraphicsScene(this);
    resizedImageScene = new QGraphicsScene(this);
    paddedImageScene = new QGraphicsScene(this);
    centeredImageScene = new QGraphicsScene(this);
    spectrumScene = new QGraphicsScene(this);
    filterMaskScene = new QGraphicsScene(this);
    filteredScene = new QGraphicsScene(this);
    recoveredScene = new QGraphicsScene(this);
    croppedScene = new QGraphicsScene(this);

    ui->graphicsViewRawImage->setScene(rawImageScene);
    ui->graphicsViewResizedImage->setScene(resizedImageScene);
    ui->graphicsViewPaddedImage->setScene(paddedImageScene);
    ui->graphicsViewCenteredImage->setScene(centeredImageScene);
    ui->graphicsViewSpectrum->setScene(spectrumScene);
    ui->graphicsViewFilterMask->setScene(filterMaskScene);
    ui->graphicsViewFilterSpectrum->setScene(filteredScene);
    ui->graphicsViewRecoveredImage->setScene(recoveredScene);
    ui->graphicsViewCroppedImage->setScene(croppedScene);

    ui->graphicsViewRawImage->setAlignment(Qt::AlignHCenter | Qt::AlignCenter);
    ui->graphicsViewResizedImage->setAlignment(Qt::AlignHCenter | Qt::AlignCenter);
    ui->graphicsViewPaddedImage->setAlignment(Qt::AlignHCenter | Qt::AlignCenter);
    ui->graphicsViewCenteredImage->setAlignment(Qt::AlignHCenter | Qt::AlignCenter);
    ui->graphicsViewSpectrum->setAlignment(Qt::AlignHCenter | Qt::AlignCenter);
    ui->graphicsViewFilterMask->setAlignment(Qt::AlignHCenter | Qt::AlignCenter);
    ui->graphicsViewFilterSpectrum->setAlignment(Qt::AlignHCenter | Qt::AlignCenter);
    ui->graphicsViewRecoveredImage->setAlignment(Qt::AlignHCenter | Qt::AlignCenter);
    ui->graphicsViewCroppedImage->setAlignment(Qt::AlignHCenter | Qt::AlignCenter);

    ui->graphicsViewRawImage->setDragMode(QGraphicsView::ScrollHandDrag);
    ui->graphicsViewResizedImage->setDragMode(QGraphicsView::ScrollHandDrag);
    ui->graphicsViewPaddedImage->setDragMode(QGraphicsView::ScrollHandDrag);
    ui->graphicsViewCenteredImage->setDragMode(QGraphicsView::ScrollHandDrag);
    ui->graphicsViewSpectrum->setDragMode(QGraphicsView::ScrollHandDrag);
    ui->graphicsViewFilterMask->setDragMode(QGraphicsView::ScrollHandDrag);
    ui->graphicsViewFilterSpectrum->setDragMode(QGraphicsView::ScrollHandDrag);
    ui->graphicsViewRecoveredImage->setDragMode(QGraphicsView::ScrollHandDrag);
    ui->graphicsViewCroppedImage->setDragMode(QGraphicsView::ScrollHandDrag);
}

// ----- File Menu Action Slots -----------------------------------------------
void MainWindow::openDefault()
{
    menuStatus("File","Open from Image List");

    QString tmp = ui->comboBoxImageSelector->currentText();
    tmp = QString(":/") +  tmp; // used to define path to a resource file
    appendStatus(QString("Loading from default list ... ") + tmp);

    imageData.loadData(tmp, -1);
    applyFilter();
    updateGraphicsScenes();
}

void MainWindow::open()
{
    menuStatus("File","Open");

    QFileDialog dialog(this);

    // --- Create a dialog window to generate a user path ---
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setViewMode(QFileDialog::Detail);

    dialog.setDirectory(defaultDirectory);
    dialog.setNameFilter(tr("Image (*.png)"));
    dialog.setDefaultSuffix("png");

    if (dialog.exec())
    {
        QStringList fileList = dialog.selectedFiles();
        QString filePath = fileList.at(0);

        appendStatus(QString("Loading from file ... ") + filePath);

        imageData.loadData(filePath);
        applyFilter();
        updateGraphicsScenes();
    }
    else {
        appendStatus(" ... open canceled");
    }
}

void MainWindow::reset()
{
    menuStatus("File","Reset");

    // --- Check if image data exists ---
    if(false)
    {
        appendStatus(QString("No data loaded."));
        return;
    }

    rawImageScene->clear();
    resizedImageScene->clear();
    paddedImageScene->clear();
    centeredImageScene->clear();
    spectrumScene->clear();
    filterMaskScene->clear();
    filteredScene->clear();
    recoveredScene->clear();
    croppedScene->clear();

    appendStatus(QString("Images cleared"));
}

void MainWindow::save()
{
    menuStatus("File","Save");

    // --- Check if image data exists ---
    if(false)
    {
        appendStatus(QString("No data loaded, save canceled."));
        return;
    }

    QString filePath = defaultDirectory.path() + "/";

    imageData.saveImageToPNG(filePath);
    appendStatus(QString("Saving to file ... ") + filePath);
}

void MainWindow::saveAs()
{
    menuStatus("File","Save As...");

    if(false)
    {
        appendStatus(QString("No data loaded, save canceled."));
        return;
    }

    // --- Create a dialog window to generate a user path ---
    QFileDialog dialog(this);

    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setViewMode(QFileDialog::Detail);

    dialog.setDirectory(QDir::homePath() + QString("/Desktop"));
    dialog.setNameFilter(tr("Image (*.png)"));
    dialog.setDefaultSuffix("png");

    if (dialog.exec())
    {
        QStringList fileList = dialog.selectedFiles();
        QString filePath = fileList.at(0);
        appendStatus(QString("Saving to file ... ") + filePath);

        imageData.saveImageToPNG(filePath);
    }
    else {
        appendStatus(" ... save canceled");
    }
}

void MainWindow::close()
{
    menuStatus("File","Close");

    // TODO: add an screen clear function here to close current image processing

}

void MainWindow::quit()
{
    menuStatus("File","Quit");
    QApplication::quit();
}

// ----- Help Menu Action Slots -----------------------------------------------
void MainWindow::about()
{
    menuStatus("Help","About Application");
}

void MainWindow::aboutQt()
{

}

void MainWindow::aboutAuthor()
{
    menuStatus("Help","About Author");
}

// ----- Status Bar Slots -----------------------------------------------------
void MainWindow::menuStatus(QString menuString, QString subMenuString)
{
    QString S = " >> ";
    QString tmp = menuString + S + subMenuString + S;
    statusBar()->showMessage(tmp);
}

void MainWindow::appendStatus(QString newString)
{
    statusBar()->showMessage(statusBar()->currentMessage()+newString);
}

void MainWindow::filterStatus()
{
    QString tmp = "Filter adjusted >>\tSettings: [ ";
            tmp += QString::number(ui->sliderA->value()) + ", ";
            tmp += QString::number(ui->sliderB->value());
            tmp += " ]";
    statusBar()->showMessage(tmp);
}

// ---- Graphics Slots -----------------------------------------------
void MainWindow::updateGraphicsScenes()
{
    rawImageScene->clear();
    resizedImageScene->clear();
    paddedImageScene->clear();
    centeredImageScene->clear();
    spectrumScene->clear();
    filterMaskScene->clear();
    filteredScene->clear();
    recoveredScene->clear();
    croppedScene->clear();

    rawImageScene->setSceneRect(0, 0, imageData.imgRaw.cols, imageData.imgRaw.rows);
    resizedImageScene->setSceneRect(0, 0, imageData.imgResized.cols, imageData.imgResized.rows);
    paddedImageScene->setSceneRect(0, 0, imageData.imgPadded.cols, imageData.imgPadded.rows);
    centeredImageScene->setSceneRect(0, 0, imageData.imgCentered.cols, imageData.imgCentered.rows);
    spectrumScene->setSceneRect(0, 0, imageData.imgSpectrum.cols, imageData.imgSpectrum.rows);
    filterMaskScene->setSceneRect(0, 0, imageData.complexMask.cols, imageData.complexMask.rows);
    filteredScene->setSceneRect(0, 0, imageData.imgFiltered.cols, imageData.imgFiltered.rows);
    recoveredScene->setSceneRect(0, 0, imageData.imgRecovered.cols, imageData.imgRecovered.rows);
    croppedScene->setSceneRect(0, 0, imageData.imgCropped.cols, imageData.imgCropped.rows);

    rawImageScene->addPixmap(QPixmap::fromImage(imageData.getQImage(0)));
    resizedImageScene->addPixmap(QPixmap::fromImage(imageData.getQImage(1)));
    paddedImageScene->addPixmap(QPixmap::fromImage(imageData.getQImage(2)));
    centeredImageScene->addPixmap(QPixmap::fromImage(imageData.getQImage(3)));
    spectrumScene->addPixmap(QPixmap::fromImage(imageData.getQImage(4)));
    filterMaskScene->addPixmap(QPixmap::fromImage(imageData.getQImage(5)));
    filteredScene->addPixmap(QPixmap::fromImage(imageData.getQImage(6)));
    recoveredScene->addPixmap(QPixmap::fromImage(imageData.getQImage(7)));
    croppedScene->addPixmap(QPixmap::fromImage(imageData.getQImage(8)));

    ui->lcdSize1->display(imageData.imgResized.rows);
    ui->lcdSize2->display(imageData.imgResized.cols);
    ui->lcdSize3->display(imageData.imgPadded.rows);
    ui->lcdSize4->display(imageData.imgPadded.cols);

    ui->sliderA->setMaximum(imageData.imgResized.rows * 2.0);
}

void MainWindow::applyFilter()
{   
    int filterSelection = 0;
    switch (ui->comboBoxFilterSelector->currentIndex()) {
    case 0:
        filterSelection = 0; // set back to 0 after testing
        break;
    case 1:
        filterSelection = 1;
        break;
    case 2:
        filterSelection = 3;
        break;
    case 3:
        filterSelection = 5;
        break;
    case 4:
        filterSelection = 2;
        break;
    case 5:
        filterSelection = 4;
        break;
    case 6:
        filterSelection = 6;
        break;
    default:
        break;
    }
    imageData.filterImage(filterSelection,ui->sliderA->value(),ui->sliderB->value());
    updateGraphicsScenes();
}

// ---- Slider Live Slots -----------------------------------------------------
void MainWindow::updateSliderA()
{
    ui->lcdA->display(ui->sliderA->value());
    filterStatus();
}

void MainWindow::updateSliderB()
{
    ui->lcdB->display(ui->sliderB->value());
    filterStatus();
}



