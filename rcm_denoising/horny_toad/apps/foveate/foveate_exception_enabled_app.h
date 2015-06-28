// Exception enabled qt app
//
// Copyright (C) 2008
// Center for Perceptual Systems
// University of Texas at Austin
//
// jsp Tue May  6 08:17:31 CDT 2008

#ifndef EXCEPTION_ENABLED_APP_H
#define EXCEPTION_ENABLED_APP_H

#include <QApplication>
#include <QCoreApplication>
#include <iostream>
#include <stdexcept>

namespace horny_toad
{

/// @brief A Qt application that can throw
/// @param AppType QApplication or QCoreApplication
template<class AppType>
class ExceptionEnabledApplication : public AppType
{
    public:
    ExceptionEnabledApplication (int &argc, char *argv[])
        : AppType (argc, argv)
    {
    }
    bool notify (QObject *o, QEvent *e)
    {
        try
        {
            return AppType::notify (o, e);
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what () << std::endl;
            AppType::quit ();
            // You can't rethrow after you quit()
        }
        catch (...)
        {
            std::cerr << "unknown exception" << std::endl;
            AppType::quit ();
            // You can't rethrow after you quit()
        }
        return true;
    }
    private:
};

} // namespace horny_toad

#endif // EXCEPTION_ENABLED_APP_H
