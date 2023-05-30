#include <iostream>
#include <chrono>
#include <omp.h>

#include "GameOfLife.hpp"

int main(int argc, char *argv[])
{
    const uint16_t width = 50;
    const uint16_t height = 50;

    const uint16_t iterations = 60 * 10; // One minute at 10 FPS
    const std::filesystem::path outputDirectory("anim");

    const auto zfill = [](int i, int n, char fill = '0')
    {
        std::stringstream fmtd;
        fmtd << std::setw(n) << std::setfill(fill) << i;
        return fmtd.str();
    };

    if (!std::filesystem::is_directory(outputDirectory))
    {
        std::filesystem::create_directories(outputDirectory);
    }

    int filenameDigits = std::to_string(iterations).length();

    std::unique_ptr<GameOfLife> golState;
    if (argc > 1)
    {
        std::cout << "Loading initial state from \"" << argv[1] << ":" << std::endl;
        auto convertedInputImagePath = outputDirectory / (zfill(0, filenameDigits) + ".pbm");
        std::string imagemagicCmd = std::string("convert -compress none ") + argv[1] + " " + (convertedInputImagePath).string();
        std::cout << imagemagicCmd << std::endl;
        system(imagemagicCmd.c_str());
        golState = GameOfLife::load(convertedInputImagePath);
    }
    else
    {
        std::cout << "Creating random initial state (" << width << "x" << height << "):" << std::endl;
        golState = GameOfLife::random(width, height);
    }

    auto timeStart = std::chrono::high_resolution_clock::now();
    std::cout << "Simulating... (Writing output to " << outputDirectory << ")" << std::endl;
    std::cout << "Running with " << omp_get_max_threads() << " threads" << std::endl;
    int i = 0;
    golState->store(outputDirectory / (zfill(i, filenameDigits) + ".pbm"));
    for (i = 1; i <= iterations; i++)
    {
        // std::cout << "Step " << zfill(i, filenameDigits, ' ') << "/" << iterations << std::flush;
        golState->update();
        golState->store(outputDirectory / (zfill(i, filenameDigits) + ".pbm"));
        // std::cout << '\r' << std::flush;
    }
    std::cout << std::endl;
    auto timeStop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(timeStop - timeStart);
    std::cout << "Simulation and IO took " << duration.count() << "ms" << std::endl;

    std::cout << "Creating animation" << std::endl;
    std::string ffmpegCmd = std::string("ffmpeg -loglevel panic -f image2 -framerate 10 -i ") + (outputDirectory / ("\%0" + std::to_string(filenameDigits) + "d.pbm")).string() + " anim.gif -y";
    system(ffmpegCmd.c_str());
    for (auto &file : std::filesystem::directory_iterator(outputDirectory))
    {
        std::filesystem::remove(file);
    }
    std::filesystem::remove(outputDirectory);

    return 0;
}
