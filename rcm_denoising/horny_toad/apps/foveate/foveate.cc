// Foveate
//
// Copyright (C) 2008
// Center for Perceptual Systems
// University of Texas at Austin
//
// jsp Thu Feb 28 15:47:19 CST 2008

#include "foveate_main_window.h"
#include "foveate_exception_enabled_app.h"
#include <iostream>

using namespace std;
using namespace horny_toad;

int main(int argc, char *argv[])
{
    try
    {
        ExceptionEnabledApplication<QApplication> app (argc, argv);
        MainWindow main_window (0);
        main_window.show();
        return app.exec();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception: " << e.what () << std::endl;
        return -1;
    }
    catch(...)
    {
        std::cerr << "Uncaught Exception" << std::endl;
        return -1;
    }
}
