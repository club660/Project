#include <vtkNew.h>
#include <vtkInteractorStyleImage.h>
#include <vtkImageViewer2.h>
#include <vtkTextMapper.h>
#include <vtkTextProperty.h>
#include <vtkActor2D.h>
#include <sstream>
#include <vtkObjectFactory.h>
#include <vtkImageActor.h>
#include <vtkAbstractPicker.h>
#include <vtkRendererCollection.h>
#include <vtkImageData.h>
#include <vtkPointPicker.h>
#include <vtkAssemblyPath.h>
#include <vtkPointData.h>
#include <vtkImageThresholdConnectivity.h>
#include <vtkPoints.h>

namespace {

    // helper class to format slice status message
    class StatusMessage
    {
    public:
        static std::string Format(int slice, int maxSlice)
        {
            std::stringstream tmp;
            tmp << "Slice Number  " << slice + 1 << "/" << maxSlice + 1;
            return tmp.str();
        }
    };


    class myVtkInteractorStyleImage : public vtkInteractorStyleImage
    {
    public:
        static myVtkInteractorStyleImage* New();
        vtkTypeMacro(myVtkInteractorStyleImage, vtkInteractorStyleImage);

    protected:
        vtkImageViewer2* _ImageViewer = NULL;
        vtkTextMapper* _StatusMapper = NULL;
        vtkPointPicker* _Picker = NULL;
        vtkImageThresholdConnectivity* _Threshold = NULL;

        int _Slice;
        int _MinSlice;
        int _MaxSlice;

    public:
        void SetImageViewer(vtkImageViewer2* imageViewer)
        {
            _ImageViewer = imageViewer;
            _MinSlice = imageViewer->GetSliceMin();
            _MaxSlice = imageViewer->GetSliceMax();
            _Slice = _MinSlice;
        }

        void SetPointPicker(vtkPointPicker* picker) {
            _Picker = picker;
        }

        void SetThreshold(vtkImageThresholdConnectivity* threshold) {
            _Threshold = threshold;
        }

        void SetStatusMapper(vtkTextMapper* statusMapper)
        {
            _StatusMapper = statusMapper;
        }

    protected:
        void MoveSliceForward()
        {
            if (_Slice < _MaxSlice)
            {
                _Slice += 1;
                _ImageViewer->SetSlice(_Slice);
                std::string msg = StatusMessage::Format(_Slice, _MaxSlice);
                _StatusMapper->SetInput(msg.c_str());
                _ImageViewer->Render();
            }
        }

        void MoveSliceBackward()
        {
            if (_Slice > _MinSlice)
            {
                _Slice -= 1;
                cout << "MoveSliceBackward::Slice = " << _Slice << std::endl;
                _ImageViewer->SetSlice(_Slice);
                std::string msg = StatusMessage::Format(_Slice, _MaxSlice);
                _StatusMapper->SetInput(msg.c_str());
                _ImageViewer->Render();
            }
        }

        virtual void OnKeyDown()
        {
            std::string key = this->GetInteractor()->GetKeySym();
            if (key.compare("Up") == 0)
            {
                // cout << "Up arrow key was pressed." << endl;
                MoveSliceForward();
            }
            else if (key.compare("Down") == 0)
            {
                // cout << "Down arrow key was pressed." << endl;
                MoveSliceBackward();
            }
            // forward event
            vtkInteractorStyleImage::OnKeyDown();
        }

        virtual void OnMouseWheelForward()
        {
            // std::cout << "Scrolled mouse wheel forward." << std::endl;
            MoveSliceForward();
            // don't forward events, otherwise the image will be zoomed
            // in case another interactorstyle is used (e.g. trackballstyle, ...)
            // vtkInteractorStyleImage::OnMouseWheelForward();
        }

        virtual void OnMouseWheelBackward()
        {
            // std::cout << "Scrolled mouse wheel backward." << std::endl;
            if (_Slice > _MinSlice)
            {
                MoveSliceBackward();
            }
            // don't forward events, otherwise the image will be zoomed
            // in case another interactorstyle is used (e.g. trackballstyle, ...)
            // vtkInteractorStyleImage::OnMouseWheelBackward();
        }

        virtual void OnLeftButtonDown()
        {
            if (_Picker) {

                // Pick at the mouse location
                _Picker->Pick(this->Interactor->GetEventPosition()[0],
                this->Interactor->GetEventPosition()[1], 0, _ImageViewer->GetRenderer());

                // There could be other props assigned to this picker, so
                // make sure we picked the image actor
                vtkAssemblyPath* path = _Picker->GetPath();
                bool validPick = false;
                if (path)
                {
                    vtkCollectionSimpleIterator sit;
                    path->InitTraversal(sit);
                    // vtkAssemblyNode *node;
                    for (int i = 0; i < path->GetNumberOfItems() && !validPick; ++i)
                    {
                        auto node = path->GetNextNode(sit);
                        if (_ImageViewer->GetImageActor() == dynamic_cast<vtkImageActor*>(node->GetViewProp()))
                        {
                            validPick = true;

                            vtkImageData* image = _ImageViewer->GetInput();

                            double picked[3];
                            _Picker->GetPickPosition(picked);

                            //std::cout << "original position of widget:" << this->Interactor->GetEventPosition()[0] << " " << this->Interactor->GetEventPosition()[1] << " " << this->Interactor->GetEventPosition()[2] << std::endl;
                            //std::cout << "original position of picker:" << picked[0] << " " << picked[1] << " " << picked[2] << std::endl;

                            // Fixes some numerical problems with the picking
                            double* bounds = _ImageViewer->GetImageActor()->GetDisplayBounds();
                            int axis = _ImageViewer->GetSliceOrientation();
                            picked[axis] = bounds[2 * axis];

                            vtkNew<vtkPoints> seedPoint;
                            seedPoint->InsertNextPoint(picked);
                            _Threshold->SetSeedPoints(seedPoint);

                            


                            //vtkPointData* pd = image->GetPointData();
                            //if (pd)
                            //{
                            //    vtkNew<vtkPointData> PointData;
                            //    PointData->InterpolateAllocate(pd, 1, 1);

                            //    // Use tolerance as a function of size of source data
                            //    double tol2 = image->GetLength();
                            //    tol2 = tol2 ? tol2 * tol2 / 1000.0 : 0.001;

                            //    // Find the cell that contains pos
                            //    int subId;
                            //    double pcoords[3], weights[8];
                            //    vtkCell* cell =
                            //        image->FindAndGetCell(picked, NULL, -1, tol2, subId, pcoords, weights);

                            //    if (cell)
                            //    {
                            //        // Interpolate the point data
                            //        PointData->InterpolatePoint(pd, 0, cell->PointIds, weights);
                            //        int components = PointData->GetScalars()->GetNumberOfComponents();
                            //        double* tuple = PointData->GetScalars()->GetTuple(0);


                            //        // debug output
                            //        std::cout << picked[0] << " " << picked[1] << " " << picked[2] << " value: ";
                            //        for (int c = 0; c < components; ++c)
                            //        {
                            //            std::cout<< vtkVariant(tuple[c]).ToString();
                            //            if (c != components - 1)
                            //            {
                            //                std::cout<<", ";
                            //            }
                            //            else {
                            //                std::cout << std::endl;
                            //            }
                            //        }
                            //    }
                            //}
                        }
                    }
                }
            }
        }
    };

    vtkStandardNewMacro(myVtkInteractorStyleImage);
}