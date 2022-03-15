#include <mainwindow.h>
#include "ui_mainwindow.h"




MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent),
ui(new Ui::MainWindow),
mRenderWindow1(vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New()),
mRenderWindow2(vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New()),
mRenderWindow3(vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New()),
mRenderWindow4(vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New()),
mRenderer1(vtkSmartPointer<vtkRenderer>::New()),
mRenderer2(vtkSmartPointer<vtkRenderer>::New()),
mRenderer3(vtkSmartPointer<vtkRenderer>::New()),
mRenderer4(vtkSmartPointer<vtkRenderer>::New()),
mInteractor1(vtkSmartPointer<QVTKInteractor>::New()),
mInteractor2(vtkSmartPointer<QVTKInteractor>::New()),
mInteractor3(vtkSmartPointer<QVTKInteractor>::New()),
mInteractor4(vtkSmartPointer<QVTKInteractor>::New()),
mInteractorStyle1(vtkSmartPointer<myVtkInteractorStyleImage>::New()),
mInteractorStyle2(vtkSmartPointer<myVtkInteractorStyleImage>::New()),
mInteractorStyle3(vtkSmartPointer<myVtkInteractorStyleImage>::New()),
mImageViewer1(vtkSmartPointer<vtkImageViewer2>::New()),
mImageViewer2(vtkSmartPointer<vtkImageViewer2>::New()),
mImageViewer3(vtkSmartPointer<vtkImageViewer2>::New()),
mReader(vtkSmartPointer<vtkDICOMImageReader>::New()),
colors(vtkSmartPointer<vtkNamedColors>::New()),
mExtractor(vtkSmartPointer<vtkMarchingCubes>::New()),
mDiscreteExtractor(vtkSmartPointer<vtkDiscreteMarchingCubes>::New()),
mStripper(vtkSmartPointer<vtkStripper>::New()),
mMapper(vtkSmartPointer<vtkPolyDataMapper>::New()),
mActor(vtkSmartPointer<vtkActor>::New()),
mPicker(vtkSmartPointer<vtkPointPicker>::New()),
mThreshold(vtkSmartPointer<vtkImageThresholdConnectivity>::New())
//mLoadingImageCallback(vtkSmartPointer<vtkCallbackCommand>::New())
{
    ui->setupUi(this);
    // set up ui componens
    mProgressBar = new QProgressBar();
    mProgressBar->setFixedWidth(200);
    mProgressBar->hide();
    ui->statusBar->addPermanentWidget(mProgressBar,0);
    ui->openGLWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->openGLWidget_2->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->openGLWidget_3->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->openGLWidget_4->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->openGLWidget, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));
    connect(ui->openGLWidget_2, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));
    connect(ui->openGLWidget_3, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));
    connect(ui->openGLWidget_4, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));

    // Set the UI connections
    connect(ui->btn_generateVolumn, &QPushButton::clicked, this, &MainWindow::generate3D);
    connect(ui->actionImport_Dicom, &QAction::triggered, this, &MainWindow::onImportDicomClick);
    connect(ui->btn_selectRegion, &QPushButton::clicked, this, &MainWindow::onBtnSelectRegionClick);
    connect(ui->btn_generateDiscreteVolumn, &QPushButton::clicked, this, &MainWindow::generateDiscrete3D);
    connect(ui->actionExport_STL, &QAction::triggered, this, &MainWindow::exportSTL);

    // Set up the widget1 rendering
    mRenderWindow1->AddRenderer(mRenderer1);
    mRenderWindow1->SetInteractor(mInteractor1);
    ui->openGLWidget->setRenderWindow(mRenderWindow1);
    //mImageViewer1->SetRenderWindow(mRenderWindow);
    mInteractor1->SetInteractorStyle(mInteractorStyle1);
    mInteractor1->Initialize();

    // Set up widget 2 rendering
    mRenderWindow2->AddRenderer(mRenderer2);
    mRenderWindow2->SetInteractor(mInteractor2);
    ui->openGLWidget_2->setRenderWindow(mRenderWindow2);
    mInteractor2->SetInteractorStyle(mInteractorStyle2);
    mInteractor2->Initialize();

    // set up widget 3 rendering
    mRenderWindow3->AddRenderer(mRenderer3);
    mRenderWindow3->SetInteractor(mInteractor3);
    ui->openGLWidget_3->setRenderWindow(mRenderWindow3);
    mInteractor3->SetInteractorStyle(mInteractorStyle3);
    mInteractor3->Initialize();

    
    // Set up the widget4 rendering
    mRenderWindow4->AddRenderer(mRenderer4);
    mRenderWindow4->SetInteractor(mInteractor4);
    ui->openGLWidget_4->setRenderWindow(mRenderWindow4);
    mInteractor4->Initialize();

    // Set the color
    std::array<unsigned char, 4> skinColor{ { 255, 125, 64 } };
    colors->SetColor("SkinColor", skinColor.data());
    std::array<unsigned char, 4> bkgColor{ { 51, 77, 102, 255 } };
    colors->SetColor("BkgColor", bkgColor.data());


    // set up threshold
    //mThreshold->SetInputConnection(mReader->GetOutputPort());
    //mThreshold->ThresholdByLower(400);
    //mThreshold->ReplaceInOn();
    //mThreshold->SetInValue(1);
    //mThreshold->ReplaceOutOn();
    //mThreshold->SetOutValue(0);
    //mThreshold->Update();
    
}
MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::onBtnSelectRegionClick()
{
    // Picker to pick pixels
    mPicker->PickFromListOn();

    // Give the picker a prop to pick
    mPicker->AddPickList(mImageViewer1->GetImageActor());

    mInteractorStyle1->SetPointPicker(mPicker);
    mInteractorStyle1->SetThreshold(mThreshold);
}

void MainWindow::onImportDicomClick() {
    QString foldername = QFileDialog::getExistingDirectory(
        this,
        tr("Open Folder"),
        "C://",
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );
    if (!foldername.isEmpty()) {
        LoadImages(foldername);
    }
}

void MainWindow::exportSTL() {
    QString filename = QFileDialog::getSaveFileName(
        this,
        tr("Save STL file"), "",
        tr("STL file (*.stl)")
    );
    if (!filename.isEmpty()) {
        ui->statusBar->showMessage(QString::fromStdString("Export Model..."));
        mProgressBar->show();

        // setup callback
        vtkNew<vtkCallbackCommand> progressCallback;
        progressCallback->SetCallback(ExportSTLCallback);
        progressCallback->SetClientData(mProgressBar);

        // setup writer
        vtkNew<vtkSTLWriter> stlWriter;
        stlWriter->AddObserver(vtkCommand::ProgressEvent, progressCallback);
        stlWriter->SetFileName(filename.toStdString().c_str());
        stlWriter->SetInputConnection(mExtractor->GetOutputPort());
        stlWriter->Write();

        ui->statusBar->showMessage(QString::fromStdString("Ready"));
        mProgressBar->hide();
    }
}

//void MainWindow::LoadImage(QString filepath) {
//    ui->statusBar->showMessage(QString::fromStdString("Loading Image..."));
//    // Read all the DICOM files in the specified directory.
//    vtkNew<vtkDICOMImageReader> reader;
//    reader->SetFileName(filepath.toStdString().c_str());
//    reader->Update();
//
//    // Visualize
//    mImageViewer1->SetInputConnection(reader->GetOutputPort());
//    mImageViewer1->SetRenderWindow(mRenderWindow1);
//    mRenderer1->ResetCamera();
//    mImageViewer1->Render();
//    
//    ui->statusBar->showMessage(QString::fromStdString("Ready"));
//}


void MainWindow::LoadImages(QString filepath) {
    ui->statusBar->showMessage(QString::fromStdString("Loading Images..."));
    mProgressBar->show();
    //mImageViewer1->New();
    // Set up callback
    vtkNew<vtkCallbackCommand> progressCallback;
    progressCallback->SetCallback(LoadingImageProgressCallback);
    progressCallback->SetClientData(mProgressBar);

    // Read DICOM files
    // vtkNew<vtkDICOMImageReader> reader;
    mReader->AddObserver(vtkCommand::ProgressEvent, progressCallback);
    mReader->SetDirectoryName(filepath.toStdString().c_str());
    mReader->Update();

    // set up image size
    imageHeight = mReader->GetHeight();
    imageWidth = mReader->GetWidth();
    
    
    // visualize
    //vtkNew<vtkImageViewer2> imageViewer;
    mImageViewer1->SetInputConnection(mReader->GetOutputPort());
    mImageViewer1->SetRenderWindow(mRenderWindow1);
    
    
    // slice status message
    vtkNew<vtkTextProperty> sliceTextProp;
    sliceTextProp->SetFontFamilyToCourier();
    sliceTextProp->SetFontSize(20);
    sliceTextProp->SetVerticalJustificationToBottom();
    sliceTextProp->SetJustificationToLeft();
    
    vtkNew<vtkTextMapper> sliceTextMapper;
    std::string msg = StatusMessage::Format(mImageViewer1->GetSliceMin(),
        mImageViewer1->GetSliceMax());
    sliceTextMapper->SetInput(msg.c_str());
    sliceTextMapper->SetTextProperty(sliceTextProp);

    vtkNew<vtkActor2D> sliceTextActor;
    sliceTextActor->SetMapper(sliceTextMapper);
    sliceTextActor->SetPosition(15, 10);

    // usage hint message
    vtkNew<vtkTextProperty> usageTextProp;
    usageTextProp->SetFontFamilyToCourier();
    usageTextProp->SetFontSize(14);
    usageTextProp->SetVerticalJustificationToTop();
    usageTextProp->SetJustificationToLeft();

    vtkNew<vtkTextMapper> usageTextMapper;
    usageTextMapper->SetInput(
        "- Slice with mouse wheel\n  or Up/Down-Key\n- Zoom with pressed right\n "
        " mouse button while dragging");
    usageTextMapper->SetTextProperty(usageTextProp);

    vtkNew<vtkActor2D> usageTextActor;
    usageTextActor->SetMapper(usageTextMapper);
    usageTextActor->GetPositionCoordinate()
        ->SetCoordinateSystemToNormalizedDisplay();
    usageTextActor->GetPositionCoordinate()->SetValue(0.05, 0.95);

    // set up style

    mInteractorStyle1->SetImageViewer(mImageViewer1);
    mInteractorStyle1->SetStatusMapper(sliceTextMapper);

    mImageViewer1->GetRenderer()->AddActor2D(sliceTextActor);
    mImageViewer1->GetRenderer()->AddActor2D(usageTextActor);

    mRenderer1->ResetCamera();
    mImageViewer1->Render();

    GenerateSagital();
    GenerateCoronal();
    //ShowThresholdResult();

    ui->statusBar->showMessage(QString::fromStdString("Ready"));
    mProgressBar->hide();
    mReader->RemoveAllObservers();
}

void MainWindow::GenerateSagital()
{
    
    // visualize
    mImageViewer2->SetInputConnection(mReader->GetOutputPort());
    mImageViewer2->SetRenderWindow(mRenderWindow2);
    mImageViewer2->SetSliceOrientationToXZ();

    // slice status message
    vtkNew<vtkTextProperty> sliceTextProp;
    sliceTextProp->SetFontFamilyToCourier();
    sliceTextProp->SetFontSize(20);
    sliceTextProp->SetVerticalJustificationToBottom();
    sliceTextProp->SetJustificationToLeft();

    vtkNew<vtkTextMapper> sliceTextMapper;
    std::string msg = StatusMessage::Format(mImageViewer2->GetSliceMin(),
        mImageViewer2->GetSliceMax());
    sliceTextMapper->SetInput(msg.c_str());
    sliceTextMapper->SetTextProperty(sliceTextProp);

    vtkNew<vtkActor2D> sliceTextActor;
    sliceTextActor->SetMapper(sliceTextMapper);
    sliceTextActor->SetPosition(15, 10);

    // set up style
    mInteractorStyle2->SetImageViewer(mImageViewer2);
    mInteractorStyle2->SetStatusMapper(sliceTextMapper);

    mImageViewer2->GetRenderer()->AddActor2D(sliceTextActor);

    mRenderer2->ResetCamera();
    mRenderWindow2->Render();
}

void MainWindow::GenerateCoronal()
{
    // visualize
    mImageViewer3->SetInputConnection(mReader->GetOutputPort());
    mImageViewer3->SetRenderWindow(mRenderWindow3);
    mImageViewer3->SetSliceOrientationToYZ();

    // slice status message
    vtkNew<vtkTextProperty> sliceTextProp;
    sliceTextProp->SetFontFamilyToCourier();
    sliceTextProp->SetFontSize(20);
    sliceTextProp->SetVerticalJustificationToBottom();
    sliceTextProp->SetJustificationToLeft();

    vtkNew<vtkTextMapper> sliceTextMapper;
    std::string msg = StatusMessage::Format(mImageViewer3->GetSliceMin(),
        mImageViewer3->GetSliceMax());
    sliceTextMapper->SetInput(msg.c_str());
    sliceTextMapper->SetTextProperty(sliceTextProp);

    vtkNew<vtkActor2D> sliceTextActor;
    sliceTextActor->SetMapper(sliceTextMapper);
    sliceTextActor->SetPosition(15, 10);

    // set up style
    mInteractorStyle3->SetImageViewer(mImageViewer3);
    mInteractorStyle3->SetStatusMapper(sliceTextMapper);

    mImageViewer3->GetRenderer()->AddActor2D(sliceTextActor);

    mRenderer3->ResetCamera();
    mRenderWindow3->Render();
}


void MainWindow::ShowThresholdResult() {
    vtkNew<vtkPoints> seedPoint;
    const double point[] = { 93, 67, 0 };
    seedPoint->InsertNextPoint(point);
    mThreshold->SetInputConnection(mReader->GetOutputPort());
    mThreshold->SetSeedPoints(seedPoint);
    mThreshold->ThresholdByUpper(400);
    mThreshold->ReplaceInOn();
    mThreshold->SetInValue(1000);
    mThreshold->ReplaceOutOn();
    mThreshold->SetOutValue(0);
    mThreshold->Update();
    // visualize
    mImageViewer3->SetInputConnection(mThreshold->GetOutputPort());
    mImageViewer3->SetRenderWindow(mRenderWindow3);

    // slice status message
    vtkNew<vtkTextProperty> sliceTextProp;
    sliceTextProp->SetFontFamilyToCourier();
    sliceTextProp->SetFontSize(20);
    sliceTextProp->SetVerticalJustificationToBottom();
    sliceTextProp->SetJustificationToLeft();

    vtkNew<vtkTextMapper> sliceTextMapper;
    std::string msg = StatusMessage::Format(mImageViewer3->GetSliceMin(),
        mImageViewer3->GetSliceMax());
    sliceTextMapper->SetInput(msg.c_str());
    sliceTextMapper->SetTextProperty(sliceTextProp);

    vtkNew<vtkActor2D> sliceTextActor;
    sliceTextActor->SetMapper(sliceTextMapper);
    sliceTextActor->SetPosition(15, 10);

    // set up style
    mInteractorStyle3->SetImageViewer(mImageViewer3);
    mInteractorStyle3->SetStatusMapper(sliceTextMapper);
    mImageViewer3->GetRenderer()->AddActor2D(sliceTextActor);

    mRenderer3->ResetCamera();
    mRenderer3->SetBackground(colors->GetColor3d("BkgColor").GetData());
    mImageViewer3->Render();
}

void MainWindow::LoadingImageProgressCallback(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData) {
    vtkDICOMImageReader* reader = static_cast<vtkDICOMImageReader*>(caller);
    QProgressBar* progressBar = reinterpret_cast<QProgressBar*>(clientData);
    progressBar->setValue(reader->GetProgress()*100);
    //std::cout << reader->GetProgress() << std::endl;
}

void MainWindow::Generate3DModelCallback(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData) {
    vtkMarchingCubes* extractor = static_cast<vtkMarchingCubes*>(caller);
    QProgressBar* progressBar = reinterpret_cast<QProgressBar*>(clientData);
    progressBar->setValue(extractor->GetProgress() * 100);
}

void MainWindow::GenerateDiscrete3DModelCallback(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData) {
    vtkDiscreteMarchingCubes* extractor = static_cast<vtkDiscreteMarchingCubes*>(caller);
    QProgressBar* progressBar = reinterpret_cast<QProgressBar*>(clientData);
    progressBar->setValue(extractor->GetProgress() * 100);
}

void MainWindow::GenerateThresholdCallback(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData) {
    vtkImageThresholdConnectivity* threshold = static_cast<vtkImageThresholdConnectivity*>(caller);
    QProgressBar* progressBar = reinterpret_cast<QProgressBar*>(clientData);
    progressBar->setValue(threshold->GetProgress() * 100);
}

void MainWindow::ExportSTLCallback(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData) {
    vtkSTLWriter* writer = static_cast<vtkSTLWriter*>(caller);
    QProgressBar* progressBar = reinterpret_cast<QProgressBar*>(clientData);
    progressBar->setValue(writer->GetProgress() * 100);
}

void MainWindow::HideAllWidget() {
    ui->openGLWidget->hide();
    ui->openGLWidget_2->hide();
    ui->openGLWidget_3->hide();
    ui->openGLWidget_4->hide();
}

void MainWindow::displayAllWidget() {
    ui->openGLWidget->show();
    ui->openGLWidget_2->show();
    ui->openGLWidget_3->show();
    ui->openGLWidget_4->show();
}

void MainWindow::displayTargetWidget() {
    QWidget* target = GetTargetWidget();
    HideAllWidget();
    target->show();
}

QWidget* MainWindow::GetTargetWidget() {
    QPoint pos = QCursor::pos();
    return qApp->widgetAt(pos);
}

void MainWindow::showContextMenu(const QPoint& pos) {
    QWidget* target = GetTargetWidget();
    QMenu contextMenu(tr("Context Menu"), target);

    QAction displayWidget("Maximize Window", target);
    QAction displayAll("Display All Windows", target);

    connect(&displayWidget, &QAction::triggered, this, &MainWindow::displayTargetWidget);
    connect(&displayAll, &QAction::triggered, this, &MainWindow::displayAllWidget);

    contextMenu.addAction(&displayWidget);
    contextMenu.addAction(&displayAll);

    contextMenu.exec(target->mapToGlobal(pos));
}

void MainWindow::generate3D() {
    ui->statusBar->showMessage(QString::fromStdString("Generate 3D model..."));
    mProgressBar->show();
    if (mActor) {
        mRenderer4->RemoveActor(mActor);
    }
    // set up callback
    vtkNew<vtkCallbackCommand> progressCallback;
    progressCallback->SetCallback(Generate3DModelCallback);
    progressCallback->SetClientData(mProgressBar);

    mReader->Update();
    // Set up extractor
    mExtractor->SetInputConnection(mReader->GetOutputPort());
    mExtractor->SetValue(0, ui->input_IsosurfaceValue->value());
    mExtractor->AddObserver(vtkCommand::ProgressEvent, progressCallback);
    mExtractor->Update();

    mStripper->SetInputConnection(mExtractor->GetOutputPort());
    mMapper->SetInputConnection(mStripper->GetOutputPort());
    mMapper->ScalarVisibilityOff();
    mStripper->Update();
    mMapper->Update();

    mActor->SetMapper(mMapper);
    mActor->GetProperty()->SetDiffuseColor(colors->GetColor3d("SkinColor").GetData());
    mActor->GetProperty()->SetSpecular(.3);
    mActor->GetProperty()->SetSpecularPower(20);
    mActor->GetProperty()->SetOpacity(.5);

    // Start by creating a black/white lookup table.
    vtkNew<vtkLookupTable> bwLut;
    bwLut->SetTableRange(0, 2000);
    bwLut->SetSaturationRange(0, 0);
    bwLut->SetHueRange(0, 0);
    bwLut->SetValueRange(0, 1);
    bwLut->Build(); // effective built

    // Now create a lookup table that consists of the full hue circle
    // (from HSV).
    vtkNew<vtkLookupTable> hueLut;
    hueLut->SetTableRange(0, 2000);
    hueLut->SetHueRange(0, 1);
    hueLut->SetSaturationRange(1, 1);
    hueLut->SetValueRange(1, 1);
    hueLut->Build(); // effective built

    // Finally, create a lookup table with a single hue but having a range
    // in the saturation of the hue.
    vtkNew<vtkLookupTable> satLut;
    satLut->SetTableRange(0, 2000);
    satLut->SetHueRange(0.6, 0.6);
    satLut->SetSaturationRange(0, 1);
    satLut->SetValueRange(1, 1);
    satLut->Build(); // effective built

    vtkNew<vtkImageMapToColors> sagittalColors;
    sagittalColors->SetInputConnection(mReader->GetOutputPort());
    sagittalColors->SetLookupTable(hueLut);
    sagittalColors->Update();

    vtkNew<vtkImageActor> sagittal;
    sagittal->GetMapper()->SetInputConnection(sagittalColors->GetOutputPort());
    sagittal->SetDisplayExtent(192, 192, 0, 289, 0, 135);
    sagittal->ForceOpaqueOn();

    // Create the second (axial) plane of the three planes. We use the
    // same approach as before except that the extent differs.
    vtkNew<vtkImageMapToColors> axialColors;
    axialColors->SetInputConnection(mReader->GetOutputPort());
    axialColors->SetLookupTable(hueLut);
    axialColors->Update();

    vtkNew<vtkImageActor> axial;               //top down
    axial->GetMapper()->SetInputConnection(axialColors->GetOutputPort());
    axial->SetDisplayExtent(0, 383, 0, 289, 68, 68);
    axial->ForceOpaqueOn();

    // Create the third (coronal) plane of the three planes. We use
    // the same approach as before except that the extent differs.
    vtkNew<vtkImageMapToColors> coronalColors;
    coronalColors->SetInputConnection(mReader->GetOutputPort());
    coronalColors->SetLookupTable(hueLut);
    coronalColors->Update();

    vtkNew<vtkImageActor> coronal;
    coronal->GetMapper()->SetInputConnection(coronalColors->GetOutputPort());
    coronal->SetDisplayExtent(0, 383, 145, 145, 0, 135);
    coronal->ForceOpaqueOn();

    // visulization
    mRenderer4->AddActor(mActor);
    //mRenderer4->AddActor(sagittal);
    //mRenderer4->AddActor(axial);
    //mRenderer4->AddActor(coronal);
    mRenderer4->ResetCamera();
    mRenderer4->SetBackground(colors->GetColor3d("BkgColor").GetData());
    mRenderWindow4->Render();

    ui->statusBar->showMessage(QString::fromStdString("Ready"));
    mProgressBar->hide();
    mExtractor->RemoveAllObservers();
}

void MainWindow::generateDiscrete3D() {
    ui->statusBar->showMessage(QString::fromStdString("Generate Discrete 3D model..."));
    mProgressBar->show();
    if (mActor) {
        mRenderer4->RemoveActor(mActor);
    }
    // set up callback
    vtkNew<vtkCallbackCommand> thresholdProgressCallback;
    thresholdProgressCallback->SetCallback(GenerateThresholdCallback);
    thresholdProgressCallback->SetClientData(mProgressBar);

    mReader->Update();
    mThreshold->SetInputConnection(mReader->GetOutputPort());
    mThreshold->ThresholdByUpper(ui->input_ThresholdUp->value());
    mThreshold->ReplaceInOn();
    mThreshold->SetInValue(ui->input_InValue->value());
    mThreshold->ReplaceOutOn();
    mThreshold->SetOutValue(ui->input_OutValue->value());
    mThreshold->Update();
    
    // set up callback
    vtkNew<vtkCallbackCommand> generationProgressCallback;
    generationProgressCallback->SetCallback(GenerateDiscrete3DModelCallback);
    generationProgressCallback->SetClientData(mProgressBar);
    // Set up extractor
    mDiscreteExtractor->SetInputConnection(mThreshold->GetOutputPort());
    mDiscreteExtractor->GenerateValues(1, ui->input_InValue->value(), ui->input_InValue->value());
    mDiscreteExtractor->AddObserver(vtkCommand::ProgressEvent, generationProgressCallback);
    mDiscreteExtractor->Update();

    // Smoother
    if (ui->cb_Smooth->isChecked()) {
        unsigned int smoothingIterations = 15;
        double passBand = 0.001;
        double featureAngle = 120.0;
        vtkNew<vtkWindowedSincPolyDataFilter> smoother;
        smoother->SetInputConnection(mDiscreteExtractor->GetOutputPort());
        smoother->SetNumberOfIterations(smoothingIterations);
        smoother->BoundarySmoothingOff();
        smoother->FeatureEdgeSmoothingOff();
        smoother->SetFeatureAngle(featureAngle);
        smoother->SetPassBand(passBand);
        smoother->NonManifoldSmoothingOn();
        smoother->NormalizeCoordinatesOn();
        smoother->Update();
        mStripper->SetInputConnection(smoother->GetOutputPort());
    }
    else {
        mStripper->SetInputConnection(mDiscreteExtractor->GetOutputPort());
    }

    
    mMapper->SetInputConnection(mStripper->GetOutputPort());
    mMapper->ScalarVisibilityOff();
    mStripper->Update();
    mMapper->Update();

    mActor->SetMapper(mMapper);
    mActor->GetProperty()->SetDiffuseColor(colors->GetColor3d("SkinColor").GetData());
    mActor->GetProperty()->SetSpecular(.3);
    mActor->GetProperty()->SetSpecularPower(20);
    mActor->GetProperty()->SetOpacity(.5);

    // visulization
    mRenderer4->AddActor(mActor);
    mRenderer4->ResetCamera();
    mRenderer4->SetBackground(colors->GetColor3d("BkgColor").GetData());
    mRenderWindow4->Render();

    ui->statusBar->showMessage(QString::fromStdString("Ready"));
    mProgressBar->hide();
    mDiscreteExtractor->RemoveAllObservers();
    mThreshold->RemoveAllObservers();
}