#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderer.h>
#include <QVTKInteractor.h>
#include <vtkInteractorStyle.h>
#include <vtkDICOMImageReader.h>
#include <vtkImageViewer2.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleImage.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <qfiledialog.h>
#include <string>
#include <vtkCallbackCommand.h>
#include <QProgressBar>
#include <qcursor.h>
#include <vtkNamedColors.h>
#include <vtkMarchingCubes.h>
#include <vtkStripper.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkLookupTable.h>
#include <vtkImageMapToColors.h>
#include <vtkImageActor.h>
#include <vtkImageMapper3D.h>
#include <vtkMarchingCubes.h>
#include <vtkDiscreteFlyingEdges3D.h>
#include <vtkDiscreteMarchingCubes.h>
#include <vtkPointPicker.h>
#include <vtkImageThresholdConnectivity.h>
#include <vtkImageThreshold.h>
#include <vtkPoints.h>
#include <vtkWindowedSincPolyDataFilter.h>
#include <vtkSTLWriter.h>

#include <myInteractionStyle.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow* ui;

    vtkSmartPointer<vtkNamedColors> colors;

    // rendering part
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> mRenderWindow1;
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> mRenderWindow2;
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> mRenderWindow3;
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> mRenderWindow4;
    vtkSmartPointer<vtkRenderer> mRenderer1;
    vtkSmartPointer<vtkRenderer> mRenderer2;
    vtkSmartPointer<vtkRenderer> mRenderer3;
    vtkSmartPointer<vtkRenderer> mRenderer4;
    vtkSmartPointer<vtkImageViewer2> mImageViewer1;
    vtkSmartPointer<vtkImageViewer2> mImageViewer2;
    vtkSmartPointer<vtkImageViewer2> mImageViewer3;

    // interactor part
    vtkSmartPointer<QVTKInteractor> mInteractor1;
    vtkSmartPointer<QVTKInteractor> mInteractor2;
    vtkSmartPointer<QVTKInteractor> mInteractor3;
    vtkSmartPointer<QVTKInteractor> mInteractor4;
    vtkSmartPointer<myVtkInteractorStyleImage> mInteractorStyle1;
    vtkSmartPointer<myVtkInteractorStyleImage> mInteractorStyle2;
    vtkSmartPointer<myVtkInteractorStyleImage> mInteractorStyle3;

    // read image part
    vtkSmartPointer<vtkDICOMImageReader> mReader;
    int imageWidth;
    int imageHeight;
    int imageSlices;

    // 3D generation part
    vtkSmartPointer<vtkMarchingCubes> mExtractor;
    vtkSmartPointer<vtkDiscreteMarchingCubes> mDiscreteExtractor;
    vtkSmartPointer<vtkStripper> mStripper;
    vtkSmartPointer<vtkPolyDataMapper> mMapper;
    vtkSmartPointer<vtkActor> mActor;

    // UI part
    QProgressBar* mProgressBar;

    // segmentation part
    vtkSmartPointer<vtkPointPicker> mPicker;
    vtkSmartPointer<vtkImageThresholdConnectivity> mThreshold;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void LoadImages(QString filepath);
    void GenerateSagital();
    void GenerateCoronal();
    void HideAllWidget();
    void ShowThresholdResult();
    
    QWidget* GetTargetWidget();
    static void LoadingImageProgressCallback(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData);
    static void Generate3DModelCallback(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData);
    static void GenerateDiscrete3DModelCallback(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData);
    static void GenerateThresholdCallback(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData);
    static void ExportSTLCallback(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData);

public slots:
    void onBtnSelectRegionClick();
    void onImportDicomClick();
    void displayTargetWidget();
    void displayAllWidget();
    void showContextMenu(const QPoint& pos);
    void generate3D();
    void generateDiscrete3D();
    void exportSTL();

};
#endif // MAINWINDOW_H
