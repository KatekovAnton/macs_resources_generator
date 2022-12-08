#include <stdio.h>
#include <iostream>
#include <filesystem>
#include "BitmapTexture.h"
#include "NoiseGenerator.h"
#include "Map.h"
#include "MPGMapProcessor.h"
#include "Dispatch/DispatchPrivate.hpp"
#include <time.h>
#include "anyoption/anyoption.h"



void SetColor(BitmapTexture* texture, int x, int y, int scaleFactor, Color color)
{
    for (int i = 0; i < scaleFactor; i++) {
        for (int j = 0; j < scaleFactor; j++) {
            texture->setColor(color, x * scaleFactor + i, y * scaleFactor + j);
        }
    }
}

void CreateTestImage(BitmapTexture *texture, Map &map, float moreWater, int scaleFactor)
{
    for (int x = 0; x < map.GetW(); x++) {
        for (int y = 0; y < map.GetH(); y++) {
            Color color;
            if (map.GetCell(x, y)._type == Map_Cell_Ground) {
                color = Color(222, 206, 142, 255);
            }
            else {
                color = Color(128, 128, 255, 255);
            }
            SetColor(texture, x, y, scaleFactor, color);
        }
    }
}

void GenerateMaps(int offset, int count, float water)
{
    int width = 112;
    int height = 112;
    int scaleFactor = 10;
    for (int i = offset; i < offset + count; i++) {
        printf("Creating Image\n");
        Map p(GSize2D(width, height));
        p.Random(water);
        BitmapTexture texture(GSize2D(width * scaleFactor, height * scaleFactor));
        CreateTestImage(&texture, p, water, scaleFactor);
        
        AIMapAnalyzer analyzer(&p);
        analyzer.AI_helper1();
        for (int i = 0; i < 2; i++) {
            auto location = p.CalculateDeployPosition(p.GetRect(), &analyzer);
            auto rect = p.GetDeployRegion(location);
            SetColor(&texture, location.x, location.y, scaleFactor, Color(255, 0, 0, 255));
            texture.DrawRect(GRect2D(rect.getX() * scaleFactor, rect.getY() * scaleFactor, rect.getW() * scaleFactor, rect.getH() * scaleFactor), Color(0, 0, 0, 255));
            p._landedPlayers.push_back(location);
        }

        printf("Saving PNG\n");
        std::string name = "result";
        name += std::to_string(i);
        name += ".png";
        std::string title = "test image";
        texture.Save(name, title);
    }
}


int main(int argc, char **argv)
{
    AnyOption* opt = new AnyOption();
    opt->setVerbose();
    opt->autoUsagePrint(true);
    opt->addUsage("Tool usage: ");
    opt->addUsage(" -h  --help  		Prints this help ");
    opt->addUsage(" -d  --dir <directory>	Maps directory");
    opt->addUsage("");

    opt->setFlag(
        "help",
        'h');
    opt->setOption(
        "dir",
        'd');
    opt->processCommandArgs(argc, argv);
    if (!opt->hasOptions()) {
        std::cout << "Directory is not provided";
        opt->printUsage();
        delete opt;
        return 1;
    }
    std::string folder;
    if (opt->getFlag("help") || opt->getFlag('h')) {
        opt->printUsage();
    }
    if (opt->getValue('d') != NULL || opt->getValue("dir") != NULL) {
        folder = opt->getValue('d');
    }
    if (folder.length() == 0) {
        std::cout << "Directory is not provided";
        opt->printUsage();
        delete opt;
        return 1;
    }
    if (!std::filesystem::exists(folder) || 
        !std::filesystem::is_directory(folder)) {
        std::cout << "-d --dir should point on the folder with wrl files";
        opt->printUsage();
        delete opt;
        return 1;
    }

    DispatchImpl *dispatch = new DispatchImpl();
    DispatchPrivate::SetSharedDispatch(dispatch);
    // time_t t;
    // srand((unsigned) time(&t));
    
    // GenerateMaps(0, 10, 0);
    // GenerateMaps(10, 10, 0.1);
    // GenerateMaps(20, 10, -0.1);
    
    MPGMapProcessor m;
    dispatch->FlushMainThread();

    delete opt;
    return 0;
}


