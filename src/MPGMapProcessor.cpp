#include "MPGMapProcessor.h"
#include <string>
#include <iostream>
#include <filesystem>
#include "BinaryReader.h"
#include "ByteBuffer.h"
#include "MAXContentMap.h"
#include "MAXContentMapImage.h"
#include "Dispatch/Dispatch.hpp"

namespace fs = std::filesystem;



MPGMapProcessor::MPGMapProcessor()
{
    std::string path = "/Users/antonkatekov/Downloads/maps";
    int i = 0;
    int c = 0;
    for (const auto & file : fs::directory_iterator(path)) {
        std::string output = file.path().string();
        if (output.substr(output.length() - 4) == ".png" ) {
            fs::remove(output);
            continue;
        }
        else if (output.substr(output.length() - 4) != ".wrl" ) {
            continue;
        }
        else {
            c++;
        }
    }
    int finished = 0;
    std::vector<DispatchWork> works;
    for (const auto & file : fs::directory_iterator(path)) {
        
        if (file.is_directory()) {
            continue;
        }
        auto path = file.path();
        auto pathString = path.string();
        if (pathString.substr(pathString.length() - 4) != ".wrl" ) {
            continue;
        }
        i++;
        works.push_back([this, path, &finished, i, c](DispatchOperation *o) {
            
            MAXContentMap map;
            {
                BinaryReader br(path);
                ByteBuffer bb(br.GetLength() + 1);
                br.ReadBuffer(br.GetLength(), reinterpret_cast<char *>(bb.getPointer()));
                bb.dataAppended(br.GetLength());
                map.LoadFromBuffer(bb);
            }
            
            MAXContentMapImage mapImageFull(map);
            int newSizeW = 0;
            int newSizeH = 0;
            float decrease = 0;
            if (mapImageFull.getSize().width > mapImageFull.getSize().height) {
                decrease = 1024.0 / mapImageFull.getSize().width; 
            }
            else {
                decrease = 1024.0 / mapImageFull.getSize().height;
            }
            newSizeW = mapImageFull.getSize().width * decrease;
            newSizeH = mapImageFull.getSize().height * decrease;
            
            MAXContentMapImage mapImage(mapImageFull, newSizeW, newSizeH);
            
            std::string output = path.string();
            output = output.substr(0, output.length() - 4) + ".png";
            std::string name = path.filename();
            if (fs::exists(output)) {
                fs::remove(output);
            }
            
            mapImage.Save(output, name);
            int fff = finished ++;
            std::cout << path << " " << fff+1 << "(" << i << ")/" << c <<  std::endl;
        });
    }
    Dispatch::SharedDispatch()->PerformAndWait(works, nullptr);
}
