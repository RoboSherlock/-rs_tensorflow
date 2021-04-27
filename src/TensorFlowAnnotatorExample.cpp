#include <uima/api.hpp>

#include <pcl/point_types.h>
#include <robosherlock/types/all_types.h>
//RS
#include <robosherlock/scene_cas.h>
#include <robosherlock/utils/time.h>
#include "../include/cppflow/include/cppflow/cppflow.h"

#include <ros/package.h>


using namespace uima;


class TenserFlowAnnotator : public Annotator
{
private:
    std::string modelName;
    std::shared_ptr<cppflow::model> model_ptr_;

public:
    std::string rosPath;
    std::string fullPath;
    std::string picturePath;

    cv::Mat rgb_;

    TyErrorId initialize(AnnotatorContext &ctx)
    {
        outInfo("initialize");
        //example if you use your own model
       /* rosPath = ros::package::getPath("rs_tensorflow");
        fullPath = rosPath + "/data/EASE_R02_1obj_test" ;
        picturePath = rosPath + "/data/pictures/";*/

       // just for the cppflow example purpose

       rosPath = ros::package::getPath("rs_tensorflow");
       fullPath = rosPath + "/model" ;

       // Take from https://commons.wikimedia.org/wiki/File:Tabby-cat.jpg
       // public domain
       picturePath = rosPath + "/data/example_cat.jpg";

       // As an example, we'll use an EfficientNetB0 as a pretrained network
       // Documentation on how to generate pretrained nets for cppflow can be found here:
       //   https://serizba.github.io/cppflow/examples.html#efficientnet
       model_ptr_ = std::make_shared<cppflow::model>(cppflow::model(fullPath));
       return UIMA_ERR_NONE;
    }

    TyErrorId destroy()
    {
        outInfo("destroy");
        return UIMA_ERR_NONE;
    }

    std::shared_ptr<cppflow::tensor> create_tensor_from_mat(cv::Mat &img, bool convert_bgr_to_rgb=false)
    {
      if(convert_bgr_to_rgb)
        cv::cvtColor(img, img, cv::COLOR_BGR2RGB);

      int rows = img.rows; int cols = img.cols; int channels = img.channels();

      std::vector<uint8_t > img_data;
      img_data.assign(img.data, img.data + img.total() * channels);
      return std::make_shared<cppflow::tensor>(cppflow::tensor(img_data, {rows, cols, channels}));
    }


    TyErrorId process(CAS &tcas, ResultSpecification const &res_spec)
    {
        outInfo("process start");
        rs::StopWatch clock;
        //
        //rs::SceneCas cas(tcas);
        //rs::Scene scene = cas.getScene();
        //
        
        // 1. step: load input data. Either from a RGB image in a RS CAS a), via openCV b) or directly via cppflow c)
        //

        // a) Load from RGB image stored in RS CAS. These are usually stored in BGR order
        //// Fetch sensor data
        //// if(use_hd_images_) {
        //  // cas.get(VIEW_COLOR_IMAGE_HD, rgb_);
        //// }
        //// else
        //// {
        //  // cas.get(VIEW_COLOR_IMAGE, rgb_);
        //// }

        // b) Load from RGB image via openCV
        cv::Mat img = cv::imread(picturePath);
        auto img_tensor_ptr = create_tensor_from_mat(img,true);
        auto img_tensor = *img_tensor_ptr;
        auto input = cppflow::cast(img_tensor, TF_UINT8, TF_FLOAT);
        input = cppflow::expand_dims(input, 0);


        // c) Load from RGB image via cppflow
        // auto input = cppflow::decode_jpeg(cppflow::read_file(picturePath));
        // input = cppflow::cast(linput, TF_UINT8, TF_FLOAT);
        // input = cppflow::expand_dims(input, 0);
        auto model = *model_ptr_;
        auto output = model(input);

        std::cout << "in shape: "  << input.shape()  << std::endl;
        std::cout << "out shape: " << output.shape() << std::endl;

        // This should output '281' for tabby cat.
        // https://gist.github.com/yrevar/942d3a0ac09ec9e5eb3a
        std::cout << "Tensor index with highest probability " << cppflow::arg_max(output, 1) << std::endl;

        /*in the follow the code is from https://github.com/serizba/cppflow/tree/master/examples/load_model to
        show a working example*/

        // auto input = cppflow::fill({10, 5}, 1.0f);
        // cppflow::model model(fullPath);
        // auto output = model(input);

        // outInfo("" << output);

        // auto values = output.get_data<float>();

        // for (auto v : values) {
        //     outInfo("" << v);
        // }       

        // Output image from tensor
        // auto tensor_data = input.get_data<uint8_t>(); // should be done before the float conversions
        // cv::Mat outImage(rows,cols,CV_8UC3);
        // memcpy(outImage.data, tensor_data.data(), tensor_data.size()*sizeof(uint8_t));
        // cv::imwrite("/tmp/out.jpg", outImage);

        return UIMA_ERR_NONE;
    }
};

// This macro exports an entry point that is used to create the annotator.
MAKE_AE(TenserFlowAnnotator)
